#include "util.h"
#include <sys/mman.h>

// L2 Configuration
#define L2_WAYS_RECEIVER 8 
#define L2_STRIDE 65536
#define CACHE_LINE_SIZE 64

// Serializing timer
static inline uint64_t get_time_serializing() {
    uint64_t a, d;
    unsigned int aux;
    asm volatile("rdtscp" : "=a"(a), "=d"(d), "=c"(aux) : : "memory");
    return (d << 32) | a;
}

// Simple pseudo-random shuffle
// Fisher-Yates shuffle
void shuffle(int *array, size_t n) {
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

int main(int argc, char **argv)
{
    // Disable buffering
    setbuf(stdout, NULL);

    // Allocate Huge Page
    void *buf = mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, 
                    MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    
    if (buf == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    memset(buf, 1, BUFF_SIZE);

    // Prepare random scan order
    int scan_order[256];
    for(int i=0; i<256; i++) scan_order[i] = i;
    // shuffle(scan_order, 256); // Shuffle once at start

    printf("Please press enter.\n");
    char text_buf[2];
    fgets(text_buf, sizeof(text_buf), stdin);

    printf("Receiver now listening.\n");

    while (1) {
        // Shuffle EVERY loop to kill prefetcher patterns completely
        shuffle(scan_order, 256);

        for (int i = 0; i < 256; i++) {
            int set = scan_order[i];
            
            // --- SETUP POINTERS ---
            void **pointers[L2_WAYS_RECEIVER];
            for (int w = 0; w < L2_WAYS_RECEIVER; w++) {
                uint64_t offset = (w * L2_STRIDE) + (set * CACHE_LINE_SIZE);
                pointers[w] = (void **)((char *)buf + offset);
            }
            // Link: 0->1->...->7->0
            for (int w = 0; w < L2_WAYS_RECEIVER; w++) {
                *pointers[w] = pointers[(w + 1) % L2_WAYS_RECEIVER];
            }
            void **start_node = pointers[0];

            // --- CONFIDENCE LOOP ---
            int confidence = 0;
            int required_confidence = 10; 
            
            for (int k = 0; k < required_confidence; k++) {
                // 1. PRIME
                void **p = start_node;
                for (int j = 0; j < L2_WAYS_RECEIVER; j++) {
                    p = (void **)*p;
                }

                // 2. WAIT
                // Wait for sender (2000 cycles)
                uint64_t wait_start = get_time_serializing();
                while (get_time_serializing() < wait_start + 2000) {}

                // 3. PROBE
                uint64_t t1 = get_time_serializing();
                p = start_node;
                for (int j = 0; j < L2_WAYS_RECEIVER; j++) {
                    p = (void **)*p;
                }
                uint64_t t2 = get_time_serializing();
                uint64_t total_time = t2 - t1;

                // 4. THRESHOLD
                // 8 ways * 40 cycles = 320 cycles.
                // Threshold 800 is very safe.
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
            }
        }
    }

    return 0;
}
