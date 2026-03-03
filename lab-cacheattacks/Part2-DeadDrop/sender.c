#include "util.h"
#include <sys/mman.h>

int main(int argc, char **argv)
{
  // Allocate a buffer using huge page
  // See the handout for details about hugepage management
  void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
  
  if (buf == (void*) - 1) {
     perror("mmap() error\n");
     exit(EXIT_FAILURE);
  }

  // Initialize buffer to ensure allocation
  memset(buf, 1, BUFF_SIZE);

  printf("Please type a message (integer 0-255).\n");

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
      // Align to next large boundary to sync with receiver
      start = (start / SLOT_TIME + 10) * SLOT_TIME; 

      // Send 8 bits
      for (int i = 0; i < 8; i++) {
          int bit = (secret >> i) & 1;
          
          // Wait for the start of the slot
          while (get_time() < start) {}
          
          if (bit) {
              // Send '1': Hammer the eviction set to evict receiver
              // We access WAY_COUNT lines at STRIDE intervals
              // This targets the specific cache set determined by offset 0
              uint64_t end_hammer = start + (SLOT_TIME / 2);
              while (get_time() < end_hammer) {
                  for (int w = 0; w < WAY_COUNT; w++) {
                      // Access memory
                      volatile char *p = (char *)buf + (w * STRIDE);
                      *p; 
                  }
              }
          } else {
              // Send '0': Do nothing (idle)
          }
          
          start += SLOT_TIME;
      }
      printf("Sent: %d\n", secret);
  }

  printf("Sender finished.\n");
  return 0;
}
