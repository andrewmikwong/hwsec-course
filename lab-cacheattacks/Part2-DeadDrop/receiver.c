#include "lib_covert.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define L2_SETS 1024
#define L2_WAYS 16
#define SAMPLES 100

// Helper to calculate statistics
void calculate_stats(uint64_t *data, int n, uint64_t *avg, uint64_t *min, uint64_t *max) {
    uint64_t sum = 0;
    *min = -1ULL;
    *max = 0;
    for(int i=0; i<n; i++) {
        sum += data[i];
        if(data[i] < *min) *min = data[i];
        if(data[i] > *max) *max = data[i];
    }
    *avg = sum / n;
}

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
    uint64_t idle_samples[SAMPLES];
    uint64_t load_samples[SAMPLES];

    if (argc > 1) {
        threshold = atoll(argv[1]);
        printf("Using provided threshold: %lu\n", threshold);
    } else {
        printf("\n=== CALIBRATION (Using Assembly Probing) ===\n");
        printf("1. Ensure Sender is NOT running.\n");
        printf("   Press Enter to measure IDLE noise...\n");
        getchar();

        for (int k = 0; k < SAMPLES; k++) {
            uint64_t t_sum = 0;
            // Probe all sets
            for (int i = 0; i < L2_SETS; i++) {
                t_sum += probe_16way_asm(sets[i]);
            }
            idle_samples[k] = t_sum / L2_SETS; // Average time per set
            usleep(1000);
        }

        uint64_t avg_idle, min_idle, max_idle;
        calculate_stats(idle_samples, SAMPLES, &avg_idle, &min_idle, &max_idle);
        printf("IDLE Stats (per set): Avg=%lu, Min=%lu, Max=%lu\n", avg_idle, min_idle, max_idle);

        printf("\n2. Start 'sender -c' (calibration mode) on SIBLING CORE.\n");
        printf("   Press Enter to measure LOAD noise...\n");
        getchar();

        for (int k = 0; k < SAMPLES; k++) {
            uint64_t t_sum = 0;
            for (int i = 0; i < L2_SETS; i++) {
                t_sum += probe_16way_asm(sets[i]);
            }
            load_samples[k] = t_sum / L2_SETS;
            usleep(1000);
        }

        uint64_t avg_load, min_load, max_load;
        calculate_stats(load_samples, SAMPLES, &avg_load, &min_load, &max_load);
        printf("LOAD Stats (per set): Avg=%lu, Min=%lu, Max=%lu\n", avg_load, min_load, max_load);

        if (avg_load <= avg_idle) {
            printf("\n[CRITICAL ERROR] No signal detected. Check CPU Pinning!\n");
            threshold = avg_idle * 2;
        } else {
            threshold = (avg_idle + avg_load) / 2;
        }
        printf("\nCalculated Threshold: %lu\n", threshold);
        printf("===================\n\n");
    }

    printf("Receiver: Listening...\n");
    
    while (1) {
        uint64_t t_sum = 0;
        for (int i = 0; i < L2_SETS; i++) {
            t_sum += probe_16way_asm(sets[i]);
        }
        uint64_t avg_time = t_sum / L2_SETS;

        if (avg_time > threshold) {
            printf("1 (%lu)\n", avg_time);
        } else {
            printf("0 (%lu)\n", avg_time);
        }
        
        usleep(100000); // 0.1s
    }

    return 0;
}
