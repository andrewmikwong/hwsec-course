#include "lib_covert.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Skylake L2: 1MB / 64B line / 16 ways = 1024 sets
#define L2_SETS 1024
#define L2_WAYS 16

// Duration of one bit in cycles (approx 0.5 seconds at 2GHz)
// Slower is safer for "basics"
#define BIT_PERIOD (1ULL << 30) 

int main(int argc, char **argv)
{
    // 1. Allocate a large buffer (Huge Page)
    void *buf = allocate_huge_page();
    if (!buf) {
        fprintf(stderr, "Failed to allocate huge page\n");
        return 1;
    }

    // 2. Prepare eviction sets for the WHOLE L2 cache
    printf("Sender: Initializing %d sets (covering whole L2)...\n", L2_SETS);
    void **sets[L2_SETS];
    for (int i = 0; i < L2_SETS; i++) {
        sets[i] = setup_eviction_set(buf, i, L2_WAYS);
    }

    // 3. Calibration Mode (optional)
    if (argc > 1 && strcmp(argv[1], "-c") == 0) {
        printf("Sender: Calibration Mode (Sending continuous 1s)...\n");
        printf("Run receiver with ./receiver -c to measure threshold.\n");
        while (1) {
            for (int s = 0; s < L2_SETS; s++) {
                traverse_list(sets[s], L2_WAYS);
            }
        }
    }

    printf("Sender: Ready. Enter message:\n");

    while (1) {
        char text_buf[128];
        if (fgets(text_buf, sizeof(text_buf), stdin) == NULL) break;
        
        // Strip newline
        text_buf[strcspn(text_buf, "\n")] = 0;
        if (strlen(text_buf) == 0) continue;

        char *binary = string_to_binary(text_buf);
        printf("Sending: '%s' (%s)\n", text_buf, binary);

        for (int i = 0; i < strlen(binary); i++) {
            char bit = binary[i];
            uint64_t start = get_time_serializing();
            
            if (bit == '1') {
                // SEND 1: Create contention by thrashing L2
                while (get_time_serializing() < start + BIT_PERIOD) {
                    for (int s = 0; s < L2_SETS; s++) {
                        traverse_list(sets[s], L2_WAYS);
                    }
                }
            } else {
                // SEND 0: Do nothing (low contention)
                while (get_time_serializing() < start + BIT_PERIOD) {
                    // Busy wait (nop) is better than sleep for precise timing,
                    // but sleep is fine for coarse-grained.
                    // We'll just spin lightly.
                    asm volatile("nop");
                }
            }
            // Optional: Small guard interval between bits?
            // For now, no.
        }
        
        printf("Sent.\n");
        free(binary);
    }

    return 0;
}
