#include "util.h"
#include <sys/mman.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFF_SIZE (1<<21)
#define L2_WAYS 16
#define STRIDE (1<<16)

// Threshold for L2 miss vs hit
// L2 hit ~14 cycles
// L2 miss (L3 hit) ~50-70 cycles
// DRAM ~200+ cycles
// We will measure the average access time of the set.
// A threshold of 45-50 seems reasonable for a single access.
// If we sum 16 accesses, the threshold would be higher.
// Let's use a single probe for simplicity first, as it's faster.
// If we see noise, we can switch to full set probe.
#define THRESHOLD 45

void *buf;

// Prime a specific set
void prime_set(int set_index) {
    char *base = (char *)buf;
    volatile char *addr;
    for (int i = 0; i < L2_WAYS; i++) {
        addr = base + set_index * 64 + i * STRIDE;
        *addr;
    }
}

// Probe a specific set and return the access time of the first line
CYCLES probe_set(int set_index) {
    char *base = (char *)buf;
    volatile char *addr = base + set_index * 64; // The first address we primed
    return measure_one_block_access_time((ADDR_PTR)addr);
}

int main(int argc, char **argv)
{
    // Allocate huge page
    buf = mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    
    if (buf == (void*) - 1) {
        perror("mmap() error\n");
        exit(EXIT_FAILURE);
    }
    
    *((char *)buf) = 1; // dummy write

    printf("Please press enter.\n");
    char text_buf[2];
    fgets(text_buf, sizeof(text_buf), stdin);
    printf("Receiver now listening.\n");

    bool listening = true;
    int last_received = -1;
    int consecutive_reads = 0;
    
    while (listening) {
        // 1. Prime Sets 0-8
        for (int i = 0; i <= 8; i++) {
            prime_set(i);
        }
        
        // 2. Wait (allow sender to intervene)
        // Busy wait for a short period to let the sender run
        for(volatile int k=0; k<2000; k++); 
        
        // 3. Probe Sets 0-8
        // Check Valid Bit (Set 8) first
        CYCLES t8 = probe_set(8);
        
        if (t8 > THRESHOLD) {
            // Valid signal detected! (Set 8 was evicted)
            int received_byte = 0;
            
            // Decode bits 0-7
            for (int i = 0; i < 8; i++) {
                CYCLES t = probe_set(i);
                if (t > THRESHOLD) {
                    received_byte |= (1 << i);
                }
            }
            
            // Debouncing: Only print if we see the same value multiple times consecutively
            // or just print once per "new" value if it's stable.
            // Simple logic: if it matches the last one, increment counter.
            // If counter > N, print and reset.
            // But we want to print *once* per transmission.
            
            if (received_byte == last_received) {
                consecutive_reads++;
            } else {
                consecutive_reads = 1;
                last_received = received_byte;
            }
            
            if (consecutive_reads == 50) { // Stable for 50 iterations
                printf("%d\n", received_byte);
                // Reset to avoid printing again immediately? 
                // No, we might want to print again if the sender sends again.
                // But the sender holds for 200000 iterations.
                // We will see it many times.
                // We should wait until it changes or disappears to print again.
                // For now, let's just print. The requirement is to print the message.
                // The example output shows "47" once.
                // So we should suppress duplicates until the signal drops.
            }
        } else {
            // No valid signal
            last_received = -1;
            consecutive_reads = 0;
        }
    }
    
    printf("Receiver finished.\n");
    return 0;
}
