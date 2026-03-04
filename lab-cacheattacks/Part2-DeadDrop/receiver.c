#include "lib_covert.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

#define L2_WAYS_RECEIVER 8
#define THRESHOLD 1000
#define CONFIDENCE 10

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);

    void *buf = allocate_huge_page();
    if (!buf) return 1;

    // Scan order array
    int scan_order[256];
    for(int i=0; i<256; i++) scan_order[i] = i;

    // Noise Mask
    int noise_mask[256] = {0};

    printf("Please press enter.\n");
    char text_buf[2];
    fgets(text_buf, sizeof(text_buf), stdin);

    printf("Receiver now listening.\n");

    // --- CALIBRATION ---
    printf("Calibrating (2s)...\n");
    uint64_t cal_start = get_time_serializing();
    while (get_time_serializing() < cal_start + 4000000000ULL) {
        shuffle(scan_order, 256);
        for (int i = 0; i < 256; i++) {
            int set = scan_order[i];
            void **start = setup_eviction_set(buf, set, L2_WAYS_RECEIVER);
            
            traverse_list(start, L2_WAYS_RECEIVER); // Prime
            
            // Wait 2000 cycles
            uint64_t w = get_time_serializing();
            while(get_time_serializing() < w + 2000);
            
            uint64_t time = measure_probe_time(start, L2_WAYS_RECEIVER);
            if (time > THRESHOLD) noise_mask[set] = 1;
        }
    }
    
    int bad = 0;
    for(int i=0; i<256; i++) if(noise_mask[i]) bad++;
    printf("Calibration Done. %d noisy sets ignored.\n", bad);

    // --- LISTENING ---
    while (1) {
        shuffle(scan_order, 256);

        for (int i = 0; i < 256; i++) {
            int set = scan_order[i];
            if (noise_mask[set]) continue;

            void **start = setup_eviction_set(buf, set, L2_WAYS_RECEIVER);
            int conf = 0;

            for (int k = 0; k < CONFIDENCE; k++) {
                traverse_list(start, L2_WAYS_RECEIVER); // Prime
                
                uint64_t w = get_time_serializing();
                while(get_time_serializing() < w + 2000);
                
                uint64_t time = measure_probe_time(start, L2_WAYS_RECEIVER);
                if (time > THRESHOLD) conf++;
                else { conf = 0; break; }
            }

            if (conf == CONFIDENCE) {
                printf("Received value: %d\n", set);
            }
        }
    }

    return 0;
}
