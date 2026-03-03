#include "util.h"
#include <sys/mman.h>

// L2 Configuration for Skylake-SP
// Size: 1MB
// Associativity: 16-way
// Line Size: 64 bytes
// Sets: 1MB / 64B / 16 = 1024 sets
// Stride to hit same set in next way: 64KB (65536 bytes)

#define L2_WAYS 16
#define L2_STRIDE 65536 // 64KB
#define CACHE_LINE_SIZE 64
#define NUM_SETS 1024

int main(int argc, char **argv)
{
    // Allocate Huge Page (2MB)
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

    printf("Receiver now listening (Scanning 256 Sets)...\n");

    volatile char *base = (char *)buf;

    while (1) {
        // We scan sets 0-255 (since message is 0-255)
        // For each set, we Prime+Probe.
        
        for (int set = 0; set < 256; set++) {
            // 1. PRIME
            // Access all 16 ways for this set
            for (int w = 0; w < L2_WAYS; w++) {
                uint64_t offset = (w * L2_STRIDE) + (set * CACHE_LINE_SIZE);
                volatile char *p = base + offset;
                *p;
            }

            // 2. WAIT
            // Wait a little bit for sender to potentially evict us
            // 1000 cycles is enough for sender to do a few accesses
            uint64_t wait_start = get_time();
            while (get_time() < wait_start + 2000) {}

            // 3. PROBE
            uint64_t t1 = get_time();
            for (int w = 0; w < L2_WAYS; w++) {
                uint64_t offset = (w * L2_STRIDE) + (set * CACHE_LINE_SIZE);
                volatile char *p = base + offset;
                *p;
            }
            uint64_t t2 = get_time();
            uint64_t total_time = t2 - t1;

            // 4. DECIDE
            // Threshold: L2 hit ~40 cycles * 16 ways = 640 cycles.
            // DRAM miss ~300 cycles.
            // If sender evicted us, total_time will be > (640 + MissPenalty)
            // Let's use 1000 as a safe threshold.
            if (total_time > 1000) {
                // Found a hot set!
                // But wait, is it noise? Let's double check.
                // Re-Prime and Re-Probe immediately.
                
                // PRIME AGAIN
                for (int w = 0; w < L2_WAYS; w++) {
                    uint64_t offset = (w * L2_STRIDE) + (set * CACHE_LINE_SIZE);
                    volatile char *p = base + offset;
                    *p;
                }
                while (get_time() < wait_start + 2000) {} // Wait again
                
                // PROBE AGAIN
                t1 = get_time();
                for (int w = 0; w < L2_WAYS; w++) {
                    uint64_t offset = (w * L2_STRIDE) + (set * CACHE_LINE_SIZE);
                    volatile char *p = base + offset;
                    *p;
                }
                t2 = get_time();
                
                if ((t2 - t1) > 1000) {
                    // Confirmed!
                    printf("Received value: %d (Time: %lu)\n", set, total_time);
                    fflush(stdout);
                    // Wait a bit to avoid spamming the same number
                    sleep(1);
                }
            }
        }
    }

    return 0;
}
