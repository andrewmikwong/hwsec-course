#include "lib_covert.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

// Convert 8 bits to a character
char bits_to_char(int *bits) {
    char c = 0;
    for (int i = 0; i < 8; i++) {
        if (bits[i]) {
            c |= (1 << (7 - i));
        }
    }
    return c;
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

    // --- STEP 1 & 2: CALIBRATION (Deriving the Threshold) ---
    if (argc > 1) {
        threshold = atoll(argv[1]);
        printf("Using provided threshold: %lu\n", threshold);
    } else {
        printf("\n=== CALIBRATION ===\n");
        printf("1. Ensure Sender is NOT running.\n");
        printf("   Press Enter to measure IDLE noise...\n");
        getchar();

        for (int k = 0; k < SAMPLES; k++) {
            uint64_t t_sum = 0;
            for (int i = 0; i < L2_SETS; i++) {
                t_sum += probe_16way_asm(sets[i]);
            }
            idle_samples[k] = t_sum / L2_SETS;
            usleep(1000);
        }

        uint64_t avg_idle, min_idle, max_idle;
        calculate_stats(idle_samples, SAMPLES, &avg_idle, &min_idle, &max_idle);
        printf("IDLE Stats: Avg=%lu, Min=%lu, Max=%lu\n", avg_idle, min_idle, max_idle);

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
        printf("LOAD Stats: Avg=%lu, Min=%lu, Max=%lu\n", avg_load, min_load, max_load);

        if (avg_load <= avg_idle) {
            printf("\n[CRITICAL ERROR] No signal detected. Check CPU Pinning!\n");
            threshold = avg_idle * 2;
        } else {
            threshold = (avg_idle + avg_load) / 2;
        }
        printf("\nCalculated Threshold: %lu\n", threshold);
        printf("===================\n\n");
    }

    // --- STEP 3: DECODE (Listening for Message) ---
    printf("Receiver: Listening for message...\n");
    
    int bit_buffer[8];
    int bit_index = 0;
    
    while (1) {
        uint64_t t_sum = 0;
        for (int i = 0; i < L2_SETS; i++) {
            t_sum += probe_16way_asm(sets[i]);
        }
        uint64_t avg_time = t_sum / L2_SETS;

        // Physical Decode: 0 or 1?
        // INVERTED LOGIC based on observed behavior (Power/Wake-up Latency Channel)
        // Sender ON -> Keeps core awake -> Low Latency (< Threshold) -> Bit 1
        // Sender OFF -> Core sleeps -> High Latency (> Threshold) -> Bit 0
        int bit = (avg_time < threshold) ? 0 : 1;
        
        // Print raw bit for debugging
        printf("%d", bit);
        fflush(stdout);

        // Application Decode: Collect 8 bits -> Char
        // Note: Real synchronization requires a start sequence.
        // For this simple lab, we just group every 8 bits.
        bit_buffer[bit_index++] = bit;
        
        if (bit_index == 8) {
            char c = bits_to_char(bit_buffer);
            printf(" [%c]\n", c); // Print the decoded character
            bit_index = 0;
        }
        
        // Wait for next bit period (Sender sends 1 bit every ~0.5s)
        // We sleep slightly less to account for processing time
        usleep(400000); 
    }

    return 0;
}
