#include "lib_covert.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define L2_SETS 1024
#define L2_WAYS 16

// Number of samples for calibration
#define SAMPLES 100

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    void *buf = allocate_huge_page();
    if (!buf) {
        fprintf(stderr, "Failed to allocate huge page\n");
        return 1;
    }

    printf("Receiver: Initializing %d sets...\n", L2_SETS);
    void **sets[L2_SETS];
    for (int i = 0; i < L2_SETS; i++) {
        sets[i] = setup_eviction_set(buf, i, L2_WAYS);
    }

    uint64_t threshold = 0;

    // --- CALIBRATION STEP ---
    // We need to know the difference between "Sender Idle" and "Sender Active"
    if (argc > 1) {
        threshold = atoll(argv[1]);
        printf("Using provided threshold: %lu\n", threshold);
    } else {
        printf("\n=== CALIBRATION ===\n");
        printf("1. Ensure Sender is NOT running (or sending 0).\n");
        printf("   Press Enter to measure IDLE noise...\n");
        getchar();

        uint64_t sum_idle = 0;
        for (int k = 0; k < SAMPLES; k++) {
            uint64_t t1 = get_time_serializing();
            for (int i = 0; i < L2_SETS; i++) {
                traverse_list(sets[i], L2_WAYS);
            }
            uint64_t t2 = get_time_serializing();
            sum_idle += (t2 - t1);
        }
        uint64_t avg_idle = sum_idle / SAMPLES;
        printf("Avg IDLE time (0): %lu cycles\n", avg_idle);

        printf("\n2. Start 'sender -c' (calibration mode) in the other terminal.\n");
        printf("   Press Enter to measure LOAD noise...\n");
        getchar();

        uint64_t sum_load = 0;
        for (int k = 0; k < SAMPLES; k++) {
            uint64_t t1 = get_time_serializing();
            for (int i = 0; i < L2_SETS; i++) {
                traverse_list(sets[i], L2_WAYS);
            }
            uint64_t t2 = get_time_serializing();
            sum_load += (t2 - t1);
        }
        uint64_t avg_load = sum_load / SAMPLES;
        printf("Avg LOAD time (1): %lu cycles\n", avg_load);

        if (avg_load <= avg_idle) {
            printf("\n[ERROR] Load time is not significantly higher than Idle time!\n");
            printf("        Are Sender/Receiver on the same physical core?\n");
            printf("        Is the Sender actually running?\n");
            // Set a fallback threshold just in case
            threshold = avg_idle * 2; 
        } else {
            threshold = (avg_idle + avg_load) / 2;
        }
        printf("\nCalculated Threshold: %lu\n", threshold);
        printf("===================\n\n");
    }

    printf("Receiver: Listening... (Press Ctrl+C to stop)\n");
    
    // Simple loop to print 1s and 0s
    while (1) {
        uint64_t t1 = get_time_serializing();
        for (int i = 0; i < L2_SETS; i++) {
            traverse_list(sets[i], L2_WAYS);
        }
        uint64_t t2 = get_time_serializing();
        uint64_t diff = t2 - t1;

        if (diff > threshold) {
            printf("1");
        } else {
            printf("0");
        }
        
        // Small delay to avoid flooding the screen too fast, 
        // but not too long to miss the bit period.
        // Since BIT_PERIOD is ~0.5s, we can sleep a bit.
        usleep(100000); // 0.1s
    }

    return 0;
}
