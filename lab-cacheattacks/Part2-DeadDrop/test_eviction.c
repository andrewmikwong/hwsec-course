#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <time.h>
#include "util.h"

#define BUFF_SIZE (1<<21) // 2MB
#define L2_WAYS 16
#ifndef STRIDE
#define STRIDE (1<<16) // 64KB
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

// Shuffle an array of pointers
void shuffle(struct node **array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        struct node *temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int main() {
    srand(time(NULL));

    // Allocate huge page
    void *buf = mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, 
                     MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    
    if (buf == (void*) -1) {
        perror("mmap() error");
        exit(EXIT_FAILURE);
    }
    
    // Dummy write
    *((char *)buf) = 1;

    char *base = (char *)buf;
    int set_index = 0;
    
    // Construct Victim List (Set 0, first 16 colors)
    struct node *victim_nodes[L2_WAYS];
    for (int i = 0; i < L2_WAYS; i++) {
        victim_nodes[i] = (struct node *)(base + set_index * 64 + i * STRIDE);
    }
    shuffle(victim_nodes, L2_WAYS);
    
    struct node *victim_list = victim_nodes[0];
    for (int i = 0; i < L2_WAYS - 1; i++) {
        victim_nodes[i]->next = victim_nodes[i+1];
    }
    victim_nodes[L2_WAYS-1]->next = NULL;

    // Construct Attacker List (Set 0, next 16 colors)
    struct node *attacker_nodes[L2_WAYS];
    for (int i = 0; i < L2_WAYS; i++) {
        attacker_nodes[i] = (struct node *)(base + set_index * 64 + (i + L2_WAYS) * STRIDE);
    }
    shuffle(attacker_nodes, L2_WAYS);
    
    struct node *attacker_list = attacker_nodes[0];
    for (int i = 0; i < L2_WAYS - 1; i++) {
        attacker_nodes[i]->next = attacker_nodes[i+1];
    }
    attacker_nodes[L2_WAYS-1]->next = NULL;

    // 1. Prime Victim Set (Traverse list)
    struct node *curr;
    for (int k = 0; k < 3; k++) {
        curr = victim_list;
        while (curr) {
            curr = curr->next;
        }
    }

    // 2. Measure Victim Set (Hit)
    uint64_t start = rdtscp();
    curr = victim_list;
    while (curr) {
        curr = curr->next;
    }
    uint64_t end = rdtscp();
    uint64_t time_hit = end - start;
    printf("Victim Set Probe (Hit): %llu cycles (Avg per line: %llu)\n", time_hit, time_hit / L2_WAYS);

    // 3. Evict (Attacker accesses their set)
    // Traverse attacker list multiple times to ensure eviction
    for (int k = 0; k < 3; k++) {
        curr = attacker_list;
        while (curr) {
            curr = curr->next;
        }
    }

    // 4. Measure Victim Set (Miss)
    start = rdtscp();
    curr = victim_list;
    while (curr) {
        curr = curr->next;
    }
    end = rdtscp();
    uint64_t time_miss = end - start;
    printf("Victim Set Probe (Miss): %llu cycles (Avg per line: %llu)\n", time_miss, time_miss / L2_WAYS);

    if (time_miss > time_hit * 1.5) {
        printf("SUCCESS: Eviction observed!\n");
    } else {
        printf("FAILURE: No significant eviction observed.\n");
    }

    return 0;
}
