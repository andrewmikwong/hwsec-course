#ifndef LIB_COVERT_H
#define LIB_COVERT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Hardware Configuration (Skylake L2)
#define BUFF_SIZE (1<<21)       // 2MB Huge Page
#define L2_STRIDE 65536         // 64KB
#define CACHE_LINE_SIZE 64
#define NUM_SETS 256            // We use 256 sets for 8-bit encoding

// Allocates a 2MB Huge Page. Returns NULL on failure.
void *allocate_huge_page();

// Sets up a circular linked list (pointer chasing) for a specific cache set.
// Returns the pointer to the first node (start of the list).
// base: Base address of the huge page
// set_index: The L2 set index (0-255)
// ways: Number of ways to chain (e.g., 8 for receiver, 32 for sender)
void **setup_eviction_set(void *base, int set_index, int ways);

// Serializing timestamp counter (rdtscp)
uint64_t get_time_serializing();

// Fisher-Yates shuffle for an integer array
void shuffle(int *array, size_t n);

// Traverses the linked list once (used for Prime and Probe)
// Returns the last pointer (useful to prevent compiler optimization)
void **traverse_list(void **start_node, int ways);

// Measures the time to traverse the list
uint64_t measure_probe_time(void **start_node, int ways);

// Hammers the set for a specific duration (in cycles)
void hammer_set(void **start_node, int ways, uint64_t duration_cycles);

#endif
