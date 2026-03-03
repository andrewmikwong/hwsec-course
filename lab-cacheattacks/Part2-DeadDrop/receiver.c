#include "util.h"
#include <sys/mman.h>

// Simple pseudo-random shuffle for receiver
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
	// Allocate huge page
	void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
	
	if (buf == (void*) - 1) {
		perror("mmap() error\n");
		exit(EXIT_FAILURE);
	}

	// Initialize buffer
	memset(buf, 1, BUFF_SIZE);

	// Create an array of indices for random access
	int indices[WAY_COUNT];
	for(int i=0; i<WAY_COUNT; i++) indices[i] = i;

	printf("Please press enter.\n");

	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);

	printf("Receiver now listening.\n");

	// Sync start time
	uint64_t start = get_time();
	start = (start / SLOT_TIME + 10) * SLOT_TIME;

	bool listening = true;
	while (listening) {
		// 1. PRIME (Forward)
		while (get_time() < start) {}
		for (int w = 0; w < WAY_COUNT; w++) {
			volatile char *p = (char *)buf + (indices[w] * STRIDE);
			*p;
		}

		// 2. WAIT (Wait for 50% of slot)
		while (get_time() < start + (SLOT_TIME / 2)) {}

		// 3. PROBE (Backward to avoid self-eviction and confuse prefetcher)
		uint64_t t1 = get_time();
		for (int w = WAY_COUNT - 1; w >= 0; w--) {
			volatile char *p = (char *)buf + (indices[w] * STRIDE);
			*p; 
		}
		uint64_t t2 = get_time();
		uint64_t total_time = t2 - t1;

		// 4. DECIDE
		// Threshold: 750 cycles
		int bit = (total_time > 750) ? 1 : 0;
		
		if (bit == 1) {
			// Detected Start Bit!
			start += SLOT_TIME;
			int received_val = 0;

			for (int i = 0; i < 8; i++) {
				// PRIME (Forward)
				while (get_time() < start) {}
				for (int w = 0; w < WAY_COUNT; w++) {
					volatile char *p = (char *)buf + (indices[w] * STRIDE);
					*p;
				}

				// WAIT
				while (get_time() < start + (SLOT_TIME / 2)) {}

				// PROBE (Backward)
				t1 = get_time();
				for (int w = WAY_COUNT - 1; w >= 0; w--) {
					volatile char *p = (char *)buf + (indices[w] * STRIDE);
					*p; 
				}
				t2 = get_time();
				total_time = t2 - t1;

				int data_bit = (total_time > 750) ? 1 : 0;
				received_val |= (data_bit << i);

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
