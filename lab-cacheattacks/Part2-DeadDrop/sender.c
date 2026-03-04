#include "util.h"
#include <sys/mman.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

// 2MB Huge Page
#define BUFF_SIZE (1<<21)
#define L2_WAYS 16
// Stride to reach the same set index in the next "color" of the huge page
// 64KB stride
#ifndef STRIDE
#define STRIDE (1<<16)
#endif

void *buf;

// Evict a specific L2 set by accessing congruent addresses
void evict_set(int set_index) {
    char *base = (char *)buf;
    volatile char *addr;
    // Access 16 addresses that map to the same set
    for (int i = 0; i < L2_WAYS; i++) {
        addr = base + set_index * 64 + i * STRIDE;
        *addr; // Read access
    }
}

int main(int argc, char **argv)
{
  // Allocate a buffer using huge page
  buf = mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
  
  if (buf == (void*) - 1) {
     perror("mmap() error\n");
     exit(EXIT_FAILURE);
  }
  
  *((char *)buf) = 1; // dummy write to trigger page allocation

  printf("Please type a message.\n");

  bool sending = true;
  while (sending) {
      char text_buf[128];
      if (fgets(text_buf, sizeof(text_buf), stdin) == NULL) break;

      // Parse integer from input
      int message = atoi(text_buf);
      
      // Basic validation
      if (message < 0 || message > 255) {
          printf("Please enter a value between 0 and 255.\n");
          continue;
      }

      printf("Sending: %d\n", message);

      // Send the message for a duration
      // We use a simple loop count to hold the signal
      // 200000 iterations should be plenty of time for the receiver to notice
      long duration = 200000;
      
      for (long k = 0; k < duration; k++) {
          // 1. Evict Valid Bit (Set 8)
          evict_set(8);
          
          // 2. Evict Data Bits (Sets 0-7)
          for (int i = 0; i < 8; i++) {
              if ((message >> i) & 1) {
                  evict_set(i);
              }
          }
      }
      
      printf("Sent.\n");
  }

  printf("Sender finished.\n");
  return 0;
}
