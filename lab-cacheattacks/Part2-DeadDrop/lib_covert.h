#ifndef LIB_COVERT_H
#define LIB_COVERT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Hardware Configuration (Skylake L2)
#define BUFF_SIZE (1<<21)       // 2MB Huge Page
#define L2_STRIDE 65536         // 64KB
#define CACHE_LINE_SIZE 64
#define NUM_SETS 1024           // Skylake L2 has 1024 sets

// Allocates a 2MB Huge Page. Returns NULL on failure.
void *allocate_huge_page();

// Sets up a circular linked list (pointer chasing) for a specific cache set.
void **setup_eviction_set(void *base, int set_index, int ways);

// Serializing timestamp counter (rdtscp)
uint64_t get_time_serializing();

// Fisher-Yates shuffle for an integer array
void shuffle(int *array, size_t n);

// Traverses the linked list in C (Legacy)
void **traverse_list(void **start_node, int ways);

// --- NEW: Paper Implementation ---
// Precision probing using assembly (Listing 1 from paper)
// Hardcoded for 16-way associativity (Skylake L2)
uint64_t probe_16way_asm(void **start_node);

// Hammers the set for a specific duration (in cycles)
void hammer_set(void **start_node, int ways, uint64_t duration_cycles);

#endif
