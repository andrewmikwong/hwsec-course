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

int main(int argc, char **argv)
{
    // Allocate Huge Page (2MB)
    // This covers the entire 1MB L2 cache twice over.
    void *buf = mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, 
                    MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    
    if (buf == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    memset(buf, 1, BUFF_SIZE);

    printf("Please type a message (integer 0-255).\n");

    while (1) {
        char text_buf[128];
        if (fgets(text_buf, sizeof(text_buf), stdin) == NULL) break;
        int secret = atoi(text_buf);
        if (secret < 0 || secret > 255) {
            printf("Value must be 0-255\n");
            continue;
        }

        printf("Sending %d... (Press Ctrl+C to stop or enter new number)\n", secret);

        // Continuous Transmission
        // We just hammer the specific set corresponding to 'secret'
        // We don't need sync. Receiver will scan and find the hot set.
        
        // We use a volatile pointer to force memory access
        volatile char *base = (char *)buf;
        
        // Run for a while (or until next input, but here we block)
        // In a real chat, we'd check for input non-blocking. 
        // For this lab demo, we can just run for a fixed time or forever.
        // Let's run for a "long enough" time to ensure receiver catches it, 
        // then ask for next input. Actually, the prompt implies we send once.
        // But without sync, we should send continuously for a bit.
        
        uint64_t start_time = get_time();
        while (get_time() < start_time + 2000000000) { // Run for ~1 second
            for (int w = 0; w < L2_WAYS; w++) {
                // Calculate address that maps to Set 'secret'
                // Offset = (Way * Stride) + (SetIndex * LineSize)
                uint64_t offset = (w * L2_STRIDE) + (secret * CACHE_LINE_SIZE);
                
                // Access
                volatile char *p = base + offset;
                *p;
            }
        }
        printf("Sent.\n");
    }

    return 0;
}
