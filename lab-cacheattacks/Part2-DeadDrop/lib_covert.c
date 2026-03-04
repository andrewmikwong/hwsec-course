#include "lib_covert.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

// Try to allocate Huge Page, fall back to posix_memalign if needed (but warn!)
void *allocate_huge_page() {
    void *buf = mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, 
                    MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    
    if (buf == MAP_FAILED) {
        perror(" [WARN] mmap huge page failed! Trying standard allocation...");
        // Fallback: 2MB aligned allocation
        if (posix_memalign(&buf, BUFF_SIZE, BUFF_SIZE) != 0) {
            perror("posix_memalign failed");
            return NULL;
        }
        // Lock memory to prevent swapping
        if (mlock(buf, BUFF_SIZE) != 0) {
            perror("mlock failed");
        }
        printf(" [WARN] Using standard 4KB pages. Attack may be unreliable!\n");
    } else {
        printf(" [INFO] Huge Page Allocated Successfully.\n");
    }
    return buf;
}

void **setup_eviction_set(void *base, int set_index, int ways) {
    void **pointers[ways];
    
    // Calculate addresses
    for (int w = 0; w < ways; w++) {
        uint64_t offset = (w * L2_STRIDE) + (set_index * CACHE_LINE_SIZE);
        if (offset >= BUFF_SIZE) {
            fprintf(stderr, "Error: Offset %lu exceeds buffer size %d\n", offset, BUFF_SIZE);
            exit(1);
        }
        pointers[w] = (void **)((char *)base + offset);
    }

    // Link them circularly: 0->1->...->(ways-1)->0
    for (int w = 0; w < ways; w++) {
        *pointers[w] = pointers[(w + 1) % ways];
    }

    return pointers[0];
}

uint64_t get_time_serializing() {
    uint64_t a, d;
    unsigned int aux;
    asm volatile("rdtscp" : "=a"(a), "=d"(d), "=c"(aux) : : "memory");
    return (d << 32) | a;
}

void shuffle(int *array, size_t n) {
    if (n > 1) {
        for (size_t i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void **traverse_list(void **start_node, int ways) {
    void **p = start_node;
    for (int i = 0; i < ways; i++) {
        p = (void **)*p;
    }
    return p;
}

// --- NEW: Paper Implementation ---
uint64_t probe_16way_asm(void **start_node) {
    uint64_t cycles;
    
    // We modify start_node in place (it becomes the last pointer), 
    // so we use "+r" constraint.
    asm volatile (
        "lfence\n\t"            // Protect against instruction re-ordering
        "rdtsc\n\t"             // Measure start time
        "mov %%eax, %%edi\n\t"  // Save low 32 bits of start time
        
        // Pointer-chasing through the eviction set
        // 16 moves for 16-way associativity (Skylake L2)
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        "mov (%1), %1\n\t"
        
        "lfence\n\t"            // Ensure loads complete before timing
        "rdtsc\n\t"             // Measure end time
        "sub %%edi, %%eax\n\t"  // Calculate elapsed cycles
        : "=a" (cycles), "+r" (start_node)
        : 
        : "rdx", "edi", "memory"
    );
    
    return cycles;
}

void hammer_set(void **start_node, int ways, uint64_t duration_cycles) {
    uint64_t start = get_time_serializing();
    void **p = start_node;
    while (get_time_serializing() < start + duration_cycles) {
        // Unrolled for pressure
        p = (void **)*p; p = (void **)*p; p = (void **)*p; p = (void **)*p;
        p = (void **)*p; p = (void **)*p; p = (void **)*p; p = (void **)*p;
    }
}
