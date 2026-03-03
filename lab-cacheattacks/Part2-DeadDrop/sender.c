#include "util.h"
#include <sys/mman.h>

// L2 Configuration
#define L2_WAYS_SENDER 20   // Hammer hard (overflow the set)
#define L2_STRIDE 65536     // 64KB
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

    printf("Please type a message (integer 0-255).\n");

    while (1) {
        char text_buf[128];
        if (fgets(text_buf, sizeof(text_buf), stdin) == NULL) break;
        int secret = atoi(text_buf);
        if (secret < 0 || secret > 255) continue;

        printf("Sending %d... (Press Ctrl+C to stop)\n", secret);

        // --- MASTIK-STYLE POINTER CHASING SETUP ---
        // We construct a circular linked list inside the buffer
        // that hops through the eviction set.
        
        void **pointers[L2_WAYS_SENDER];
        for (int w = 0; w < L2_WAYS_SENDER; w++) {
            uint64_t offset = (w * L2_STRIDE) + (secret * CACHE_LINE_SIZE);
            pointers[w] = (void **)((char *)buf + offset);
        }

        // Link them up: 0->1->2...->19->0
        for (int w = 0; w < L2_WAYS_SENDER; w++) {
            *pointers[w] = pointers[(w + 1) % L2_WAYS_SENDER];
        }

        // --- ATTACK LOOP ---
        // Traverse the list infinitely. 
        // CPU must load p to find the next p.
        void **p = pointers[0];
        
        // Run for ~1 second
        uint64_t start = get_time();
        while (get_time() < start + 2000000000) {
            // Unroll slightly for performance
            p = (void **)*p;
            p = (void **)*p;
            p = (void **)*p;
            p = (void **)*p;
            p = (void **)*p;
        }
        printf("Sent.\n");
    }

    return 0;
}
