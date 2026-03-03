#include "util.h"
#include <sys/mman.h>

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
              // Send '1': ACCESS THE MEMORY
              // This is Flush+Reload style: Victim accesses the line.
              // We just need to touch it once to bring it into cache.
              // But let's hammer it a bit to be sure.
              uint64_t end_hammer = start + (SLOT_TIME * 9 / 10);
              while (get_time() < end_hammer) {
                  volatile char *p = (char *)buf;
                  *p; // Just read the first byte
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
