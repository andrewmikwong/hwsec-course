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

	bool listening = true;
	while (listening) {
		// Sync start time
		uint64_t start = get_time();
		// Align to next large boundary
		start = (start / SLOT_TIME + 10) * SLOT_TIME;

		int received_val = 0;

		for (int i = 0; i < 8; i++) {
			// 1. PRIME: Fill the set with our own data
			while (get_time() < start) {}
			
			for (int w = 0; w < WAY_COUNT; w++) {
				volatile char *p = (char *)buf + (w * STRIDE);
				*p;
			}

			// 2. WAIT: Allow sender to run
			// Wait for 75% of the slot
			while (get_time() < start + (SLOT_TIME * 3 / 4)) {}

			// 3. PROBE: Measure time to access our data again
			uint64_t t1 = get_time();
			for (int w = 0; w < WAY_COUNT; w++) {
				volatile char *p = (char *)buf + (w * STRIDE);
				*p; 
			}
			uint64_t t2 = get_time();
			uint64_t total_time = t2 - t1;

			// 4. DECIDE
			// Threshold tuning: L2 hit ~40 cycles, DRAM ~300 cycles.
			// 12 accesses: Fast ~480, Slow > 600.
			int bit = (total_time > 600) ? 1 : 0;
			
			received_val |= (bit << i);
			// printf("Bit %d: Time %lu -> %d\n", i, total_time, bit);

			start += SLOT_TIME;
		}
		
		if (received_val != 0) {
			printf("Received value: %d\n", received_val);
		}
	}

	printf("Receiver finished.\n");

	return 0;
}
