#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "util.h"

#define BUFF_SIZE (1<<21) // 2MB
#define L2_WAYS 16
#define STRIDE (1<<16) // 64KB

int main() {
    // Allocate huge page
    void *buf = mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, 
                     MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    
    if (buf == (void*) -1) {
        perror("mmap() error");
        exit(EXIT_FAILURE);
    }
    
    // Dummy write to trigger allocation
    *((char *)buf) = 1;

    char *base = (char *)buf;
    int set_index = 0; // Target Set 0
    
    // Addresses for Set 0
    // We have 32 addresses for Set 0 in a 2MB page (2MB / 64KB = 32)
    // Use first 16 as "victim" set, next 16 as "attacker" set
    
    volatile char *victim_set[L2_WAYS];
    volatile char *attacker_set[L2_WAYS];
    
    printf("Constructing sets for Set Index %d...\n", set_index);
    for (int i = 0; i < L2_WAYS; i++) {
        victim_set[i] = base + set_index * 64 + i * STRIDE;
        attacker_set[i] = base + set_index * 64 + (i + L2_WAYS) * STRIDE;
    }

    // 1. Prime (Victim accesses their set)
    printf("Priming victim set...\n");
    for (int i = 0; i < L2_WAYS; i++) {
        *victim_set[i]; // Read access
    }
    
    // 2. Probe (Measure latency of one victim line - should be fast/hit)
    CYCLES time_hit = measure_one_block_access_time((ADDR_PTR)victim_set[0]);
    printf("Victim probe (Hit): %u cycles\n", time_hit);

    // 3. Evict (Attacker accesses their set)
    printf("Attacker evicting...\n");
    for (int i = 0; i < L2_WAYS; i++) {
        *attacker_set[i]; // Read access
    }

    // 4. Probe again (Measure latency of victim line - should be slow/miss)
    CYCLES time_miss = measure_one_block_access_time((ADDR_PTR)victim_set[0]);
    printf("Victim probe (Miss): %u cycles\n", time_miss);

    if (time_miss > time_hit * 2) {
        printf("SUCCESS: Eviction observed!\n");
    } else {
        printf("FAILURE: No significant eviction observed.\n");
    }

    return 0;
}
