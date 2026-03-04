#include "util.h"
#include <sys/mman.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BUFF_SIZE (1<<21)
#define L2_WAYS 16
#ifndef STRIDE
#define STRIDE (1<<16)
#endif

// Revert to the configuration that worked (Spacing 16, Base 0)
#define SET_SPACING 16
#define BASE_SET 0

// Inline rdtscp for timing
static inline uint64_t rdtscp(void) {
    uint32_t lo, hi;
    asm volatile("rdtscp" : "=a"(lo), "=d"(hi) :: "rcx");
    return ((uint64_t)hi << 32) | lo;
}

// Linked list node
struct node {
    struct node *next;
    char pad[64 - sizeof(struct node *)]; // Pad to cache line size
};

void *buf;
struct node *sets[256]; // Pointers to the start of each set's list
uint64_t thresholds[256]; // Per-set thresholds

// Shuffle an array of pointers
void shuffle(struct node **array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        struct node *temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// Build shuffled linked list for a set
void build_set(int logical_set_index) {
    char *base = (char *)buf;
    struct node *nodes[L2_WAYS];
    
    // Map logical set index to physical set index with spacing and offset
    int physical_set_index = BASE_SET + (logical_set_index * SET_SPACING);
    
    for (int i = 0; i < L2_WAYS; i++) {
        nodes[i] = (struct node *)(base + physical_set_index * 64 + i * STRIDE);
    }
    
    shuffle(nodes, L2_WAYS);
    
    for (int i = 0; i < L2_WAYS - 1; i++) {
        nodes[i]->next = nodes[i+1];
    }
    nodes[L2_WAYS-1]->next = NULL;
    
    sets[logical_set_index] = nodes[0];
}

// Evict a specific L2 set by traversing the list
void evict_set(int set_index) {
    struct node *curr = sets[set_index];
    while (curr) {
        curr = curr->next;
    }
    // Traverse again to be sure (aggressive eviction)
    curr = sets[set_index];
    while (curr) {
        curr = curr->next;
    }
}

int main(int argc, char **argv)
{
  srand(time(NULL));

  // Allocate a buffer using huge page
  buf = mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
  
  if (buf == (void*) - 1) {
     perror("mmap() error\n");
     exit(EXIT_FAILURE);
  }
  
  *((char *)buf) = 1; // dummy write to trigger page allocation

  // Build sets 0-8
  for (int i = 0; i <= 8; i++) {
      build_set(i);
  }

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
      // 1,000,000 iterations to compensate for stricter receiver
      long duration = 1000000; 
      
      // Sync Pulse: Send Set 8 ONLY for a short burst to wake up receiver
      long sync_duration = 50000;
      for (long k = 0; k < sync_duration; k++) {
          evict_set(8);
      }

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
