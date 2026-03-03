#include "util.h"
#include <sys/mman.h>

// Simple pseudo-random shuffle for sender
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
  // Allocate a buffer using huge page
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

  printf("Please type a message (integer 0 or 1).\n");

  bool sending = true;
  while (sending) {
      char text_buf[128];
      if (fgets(text_buf, sizeof(text_buf), stdin) == NULL) {
          sending = false;
          continue;
      }

      int secret = atoi(text_buf);
      printf("Sending %d...\n", secret);

      // Synchronize start
      uint64_t start = get_time();
      start = (start / SLOT_TIME + 10) * SLOT_TIME; 

      // Send 2 bits: Start Bit (1) + 1 Data Bit
      for (int i = -1; i < 1; i++) { // ONLY 1 BIT
          int bit;
          if (i == -1) bit = 1; // Start Bit
          else bit = (secret >> i) & 1;
          
          while (get_time() < start) {}
          
          if (bit) {
              // Send '1': Hammer the eviction set
              // Use randomized access pattern to confuse prefetcher
              uint64_t end_hammer = start + (SLOT_TIME * 9 / 10);
              while (get_time() < end_hammer) {
                  // Forward pass
                  for (int k = 0; k < WAY_COUNT; k++) {
                      volatile char *p = (char *)buf + (indices[k] * STRIDE);
                      *p; 
                  }
                  // Backward pass
                  for (int k = WAY_COUNT - 1; k >= 0; k--) {
                      volatile char *p = (char *)buf + (indices[k] * STRIDE);
                      *p; 
                  }
              }
          } else {
              // Send '0': Idle
          }
          
          start += SLOT_TIME;
      }
      printf("Sent: %d\n", secret);
  }

  printf("Sender finished.\n");
  return 0;
}
