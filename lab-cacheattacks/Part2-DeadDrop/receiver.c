#include "util.h"
#include <sys/mman.h>

// L2 Configuration
// Receiver MUST use < 16 ways to avoid self-eviction!
// 12 ways is safe and covers 75% of the set.
#define L2_WAYS_RECEIVER 12 
#define L2_STRIDE 65536
#define CACHE_LINE_SIZE 64

int main(int argc, char **argv)
{
    // Allocate Huge Page
    void *buf = mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, 
                    MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    
    if (buf == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    memset(buf, 1, BUFF_SIZE);

    printf("Please press enter.\n");
    char text_buf[2];
    fgets(text_buf, sizeof(text_buf), stdin);

    printf("Receiver now listening (Mastik-Style Pointer Chasing)...\n");

    while (1) {
        for (int set = 0; set < 256; set++) {
            
            // --- SETUP POINTERS ---
            void **pointers[L2_WAYS_RECEIVER];
            for (int w = 0; w < L2_WAYS_RECEIVER; w++) {
                uint64_t offset = (w * L2_STRIDE) + (set * CACHE_LINE_SIZE);
                pointers[w] = (void **)((char *)buf + offset);
            }
            // Link: 0->1->...->11->0
            for (int w = 0; w < L2_WAYS_RECEIVER; w++) {
                *pointers[w] = pointers[(w + 1) % L2_WAYS_RECEIVER];
            }
            void **start_node = pointers[0];

            // --- CONFIDENCE LOOP ---
            int confidence = 0;
            int required_confidence = 3; 
            
            for (int k = 0; k < required_confidence; k++) {
                // 1. PRIME
                // Traverse the list to load it into L2
                void **p = start_node;
                for (int i = 0; i < L2_WAYS_RECEIVER; i++) {
                    p = (void **)*p;
                }

                // 2. WAIT
                // Wait for sender (2000 cycles)
                uint64_t wait_start = get_time();
                while (get_time() < wait_start + 2000) {}

                // 3. PROBE
                // Traverse and measure
                uint64_t t1 = get_time();
                p = start_node;
                for (int i = 0; i < L2_WAYS_RECEIVER; i++) {
                    p = (void **)*p;
                }
                uint64_t t2 = get_time();
                uint64_t total_time = t2 - t1;

                // 4. THRESHOLD
                // 12 ways * 40 cycles (L2) = 480 cycles
                // 12 ways * DRAM (>200) = > 2400 cycles
                // Threshold: 800 is a safe middle ground.
                if (total_time > 800) {
                    confidence++;
                } else {
                    confidence = 0;
                    break; 
                }
            }

            if (confidence == required_confidence) {
                printf("Received value: %d\n", set);
                fflush(stdout);
                sleep(1);
            }
        }
    }

    return 0;
}
