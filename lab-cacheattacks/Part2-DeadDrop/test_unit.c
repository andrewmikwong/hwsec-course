#include "lib_covert.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// ANSI Colors
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

void test_shuffle() {
    printf("Testing shuffle()... ");
    int arr[10];
    for(int i=0; i<10; i++) arr[i] = i;
    
    int original[10];
    memcpy(original, arr, sizeof(arr));
    
    shuffle(arr, 10);
    
    // Check if order changed (probabilistic, but highly likely)
    bool changed = false;
    for(int i=0; i<10; i++) {
        if (arr[i] != original[i]) changed = true;
    }
    
    // Check checksum (elements should be same)
    int sum = 0;
    for(int i=0; i<10; i++) sum += arr[i];
    
    assert(sum == 45); // 0+1+...+9 = 45
    
    if (changed) printf(GREEN "PASS" RESET "\n");
    else printf(RED "FAIL (Order didn't change?)" RESET "\n");
}

void test_huge_page() {
    printf("Testing allocate_huge_page()... ");
    void *buf = allocate_huge_page();
    if (buf) {
        // Try writing to it
        ((char*)buf)[0] = 'A';
        ((char*)buf)[BUFF_SIZE-1] = 'Z';
        printf(GREEN "PASS" RESET "\n");
    } else {
        printf(RED "FAIL (NULL)" RESET "\n");
    }
}

void test_linked_list() {
    printf("Testing setup_eviction_set()... ");
    void *buf = allocate_huge_page();
    if (!buf) { printf(RED "SKIP (No HugePage)" RESET "\n"); return; }
    
    int ways = 5;
    void **start = setup_eviction_set(buf, 0, ways);
    
    // Traverse and verify cycle
    void **p = start;
    int count = 0;
    for(int i=0; i<ways; i++) {
        p = (void **)*p;
        count++;
    }
    
    // After 'ways' steps, we should be back at start
    if (p == start && count == ways) {
        printf(GREEN "PASS" RESET "\n");
    } else {
        printf(RED "FAIL (Cycle broken)" RESET "\n");
    }
}

void test_timing() {
    printf("Testing get_time_serializing()... ");
    uint64_t t1 = get_time_serializing();
    uint64_t t2 = get_time_serializing();
    
    if (t2 > t1) {
        printf(GREEN "PASS (Delta: %lu)" RESET "\n", t2 - t1);
    } else {
        printf(RED "FAIL (Not monotonic)" RESET "\n");
    }
}

int main() {
    printf("=== UNIT TESTS ===\n");
    test_shuffle();
    test_huge_page();
    test_linked_list();
    test_timing();
    printf("==================\n");
    return 0;
}
