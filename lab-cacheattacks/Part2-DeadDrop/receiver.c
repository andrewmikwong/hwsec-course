#include "util.h"
#include <sys/mman.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BUFF_SIZE (1<<21)
#define L2_WAYS 16
#ifndef STRIDE
#define STRIDE (1<<16)
#endif

// Spacing between sets to avoid adjacent line prefetcher AND L1 aliasing.
// Spacing 16 caused L1 aliasing (Set 0 and Set 4 mapped to same L1 set).
// Spacing 17 avoids this (17 is coprime to 64).
#define SET_SPACING 17

// Start at a higher set index to avoid Set 0 which is often busy
#define BASE_SET 32

// Inline rdtscp for timing
static inline uint64_t rdtscp(void) {
    uint32_t lo, hi;
    asm volatile("rdtscp" : "=a"(lo), "=d"(hi) :: "rcx");
    return ((uint64_t)hi << 32) | lo;
}

// Linked list node
struct node {
    struct node *next;
    char pad[64 - sizeof(struct node *)]; // Pad to cache line size
};

void *buf;
struct node *sets[256]; // Pointers to the start of each set's list
uint64_t thresholds[256]; // Per-set thresholds

// Shuffle an array of pointers
void shuffle(struct node **array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        struct node *temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// Build shuffled linked list for a set
void build_set(int logical_set_index) {
    char *base = (char *)buf;
    struct node *nodes[L2_WAYS];
    
    // Map logical set index to physical set index with spacing and offset
    int physical_set_index = BASE_SET + (logical_set_index * SET_SPACING);
    
    for (int i = 0; i < L2_WAYS; i++) {
        nodes[i] = (struct node *)(base + physical_set_index * 64 + i * STRIDE);
    }
    
    shuffle(nodes, L2_WAYS);
    
    for (int i = 0; i < L2_WAYS - 1; i++) {
        nodes[i]->next = nodes[i+1];
    }
    nodes[L2_WAYS-1]->next = NULL;
    
    sets[logical_set_index] = nodes[0];
}

// Prime a specific set
void prime_set(int set_index) {
    struct node *curr = sets[set_index];
    while (curr) {
        curr = curr->next;
    }
}

// Probe a specific set and return the access time
uint64_t probe_set(int set_index) {
    uint64_t start = rdtscp();
    struct node *curr = sets[set_index];
    while (curr) {
        curr = curr->next;
    }
    uint64_t end = rdtscp();
    return end - start;
}

// Calibrate thresholds
void calibrate(uint64_t manual_threshold) {
    printf("Calibrating thresholds...\n");
    for (int i = 0; i <= 8; i++) {
        if (manual_threshold > 0) {
            thresholds[i] = manual_threshold;
        } else {
            uint64_t sum = 0;
            int samples = 1000;
            for (int k = 0; k < samples; k++) {
                prime_set(i);
                // No wait
                sum += probe_set(i);
            }
            uint64_t avg_hit = sum / samples;
            // 2.0x is a safe conservative threshold
            thresholds[i] = avg_hit * 2; 
            printf("Set %d (Phys %d): Avg Hit = %llu, Threshold = %llu\n", 
                   i, BASE_SET + (i * SET_SPACING), (unsigned long long)avg_hit, (unsigned long long)thresholds[i]);
        }
    }
    if (manual_threshold > 0) {
        printf("Using manual threshold: %llu\n", (unsigned long long)manual_threshold);
    }
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    uint64_t manual_threshold = 0;
    if (argc > 1) {
        manual_threshold = strtoull(argv[1], NULL, 10);
    }

    // Allocate huge page
    buf = mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    
    if (buf == (void*) - 1) {
        perror("mmap() error\n");
        exit(EXIT_FAILURE);
    }
    
    *((char *)buf) = 1; // dummy write

    // Build sets 0-8
    for (int i = 0; i <= 8; i++) {
        build_set(i);
    }

    calibrate(manual_threshold);

    printf("Please press enter.\n");
    char text_buf[2];
    fgets(text_buf, sizeof(text_buf), stdin);
    printf("Receiver now listening.\n");

    bool listening = true;
    int last_received = -1;
    int consecutive_reads = 0;
    
    while (listening) {
        // 1. Prime Sets 0-8
        for (int i = 0; i <= 8; i++) {
            prime_set(i);
        }
        
        // 2. Wait (allow sender to intervene)
        // Busy wait for a short period to let the sender run
        for(volatile int k=0; k<5000; k++); 
        
        // 3. Probe Sets 0-8
        // Check Valid Bit (Set 8) first
        uint64_t t8 = probe_set(8);
        
        if (t8 > thresholds[8]) {
            // Valid signal detected! (Set 8 was evicted)
            int received_byte = 0;
            
            // Decode bits 0-7
            for (int i = 0; i < 8; i++) {
                uint64_t t = probe_set(i);
                if (t > thresholds[i]) {
                    received_byte |= (1 << i);
                }
            }
            
            // Debouncing
            if (received_byte == last_received) {
                consecutive_reads++;
            } else {
                consecutive_reads = 1;
                last_received = received_byte;
            }
            
            if (consecutive_reads == 50) { 
                printf("%d\n", received_byte);
                
                // Wait for signal to drop (Set 8 becomes Hit again)
                int timeout = 0;
                while(1) {
                    prime_set(8);
                    for(volatile int k=0; k<5000; k++);
                    if (probe_set(8) < thresholds[8]) {
                        break; // Signal dropped
                    }
                    
                    timeout++;
                    if (timeout > 2000) {
                        break;
                    }
                }
                last_received = -1;
                consecutive_reads = 0;
            }
        } else {
            // No valid signal
            last_received = -1;
            consecutive_reads = 0;
        }
    }
    
    printf("Receiver finished.\n");
    return 0;
}
