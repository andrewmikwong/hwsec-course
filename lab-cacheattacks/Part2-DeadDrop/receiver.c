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

// Keep Spacing 16, Base 0 as it was the most promising
#define SET_SPACING 16
#define BASE_SET 0

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
            thresholds[i] = avg_hit * 2; // Back to 2.0x for safety
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
    
    // History buffer for majority voting (last 20 samples)
    #define HISTORY_LEN 20
    int history[HISTORY_LEN];
    int history_idx = 0;
    for(int i=0; i<HISTORY_LEN; i++) history[i] = -1;

    int last_printed = -1;
    int cooldown = 0;

    while (listening) {
        // 1. Prime Sets 0-8
        for (int i = 0; i <= 8; i++) {
            prime_set(i);
        }
        
        // 2. Wait
        for(volatile int k=0; k<5000; k++); 
        
        // 3. Probe Sets 0-8
        // Check Valid Bit (Set 8) first
        uint64_t t8 = probe_set(8);
        
        int current_val = -1;

        if (t8 > thresholds[8]) {
            // Valid signal detected!
            int received_byte = 0;
            for (int i = 0; i < 8; i++) {
                uint64_t t = probe_set(i);
                if (t > thresholds[i]) {
                    received_byte |= (1 << i);
                }
            }
            current_val = received_byte;
        } else {
            current_val = -1; // No signal
        }

        // Add to history
        history[history_idx] = current_val;
        history_idx = (history_idx + 1) % HISTORY_LEN;

        // Majority Vote
        // Count occurrences of each value in history
        // Since values are 0-255, we can use a simple array or just find the most frequent
        // -1 counts as "no signal"
        
        int counts[257]; // 0-255, and 256 for -1
        for(int i=0; i<257; i++) counts[i] = 0;
        
        for(int i=0; i<HISTORY_LEN; i++) {
            int val = history[i];
            if (val == -1) counts[256]++;
            else counts[val]++;
        }

        int max_count = 0;
        int winner = -1;
        for(int i=0; i<256; i++) { // Ignore -1 for winner selection
            if (counts[i] > max_count) {
                max_count = counts[i];
                winner = i;
            }
        }

        // Check if winner is dominant enough (> 70%)
        if (winner != -1 && max_count > (HISTORY_LEN * 0.7)) {
            if (winner != last_printed) {
                if (cooldown == 0) {
                    printf("%d\n", winner);
                    last_printed = winner;
                    cooldown = 100; // Wait 100 cycles before allowing new print
                }
            } else {
                // Same as last printed, keep cooldown active if signal persists
                if (cooldown > 0) cooldown = 100;
            }
        } else {
            // No dominant signal
            if (counts[256] > (HISTORY_LEN * 0.8)) {
                // Dominant silence -> reset last_printed to allow re-printing same number
                if (cooldown == 0) last_printed = -1;
            }
        }

        if (cooldown > 0) cooldown--;
    }
    
    printf("Receiver finished.\n");
    return 0;
}
