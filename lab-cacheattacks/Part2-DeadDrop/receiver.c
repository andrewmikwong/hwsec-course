#include "util.h"
#include <sys/mman.h>

int main(int argc, char **argv)
{
	// Allocate huge page
	void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
	
	if (buf == (void*) - 1) {
		perror("mmap() error\n");
		exit(EXIT_FAILURE);
	}

	// Initialize buffer
	memset(buf, 1, BUFF_SIZE);

	printf("Please press enter.\n");

	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);

	printf("Receiver now listening.\n");

	// Sync start time
	uint64_t start = get_time();
	start = (start / SLOT_TIME + 10) * SLOT_TIME;

	bool listening = true;
	while (listening) {
        // --- DEBUG: USE CLFLUSH TO SIMULATE EVICTION ---
        // Instead of Prime+Probe, we will just flush the line and time it.
        // This confirms if we can see ANY signal at all.
        
        // 1. Flush the line (Simulate "Prime" by removing it)
        clflush((ADDR_PTR)buf);
        
        // 2. Wait for Sender
        while (get_time() < start + (SLOT_TIME / 2)) {}
        
        // 3. Measure Reload Time
        CYCLES time = measure_one_block_access_time((ADDR_PTR)buf);

		// 4. DECIDE
		// If time is LOW (< 100), it means Sender ACCESSED it (brought it back to cache).
        // If time is HIGH (> 200), it means Sender did NOTHING (it stayed flushed).
        // Note: This logic is INVERTED compared to Prime+Probe!
        // Fast = Sender Active (1)
        // Slow = Sender Idle (0)
        
		int bit = (time < 200) ? 1 : 0;
		
		if (bit == 1) {
			// Detected Start Bit!
			start += SLOT_TIME;
			int received_val = 0;

			for (int i = 0; i < 1; i++) { // ONLY 1 BIT
                // Flush
                clflush((ADDR_PTR)buf);
                
                // Wait
				while (get_time() < start + (SLOT_TIME / 2)) {}

                // Measure
                time = measure_one_block_access_time((ADDR_PTR)buf);
                
				int data_bit = (time < 200) ? 1 : 0;
				received_val |= (data_bit << i);
                printf("Bit %d: Time %u -> %d\n", i, time, data_bit);

				start += SLOT_TIME;
			}
			printf("Received value: %d\n", received_val);
		} else {
			// Idle
			start += SLOT_TIME;
		}
	}

	printf("Receiver finished.\n");
	return 0;
}
