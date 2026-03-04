#include "lib_covert.h"
#include "util.h" // Keep for legacy compatibility if needed
#include <stdio.h>
#include <stdlib.h>

#define L2_WAYS_SENDER 32

int main(int argc, char **argv)
{
    void *buf = allocate_huge_page();
    if (!buf) return 1;

    printf("Please type a message (integer 0-255).\n");

    while (1) {
        char text_buf[128];
        if (fgets(text_buf, sizeof(text_buf), stdin) == NULL) break;
        int secret = atoi(text_buf);
        if (secret < 0 || secret > 255) continue;

        printf("Sending %d...\n", secret);

        // Setup eviction set for the secret
        void **start_node = setup_eviction_set(buf, secret, L2_WAYS_SENDER);

        // Hammer for ~2 seconds (approx 4B cycles)
        hammer_set(start_node, L2_WAYS_SENDER, 4000000000ULL);
        
        printf("Sent.\n");
    }

    return 0;
}
