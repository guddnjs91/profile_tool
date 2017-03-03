#ifndef __PROFILE_H__
#define __PROFILE_H__

#define __STDC_FORMAT_MACROS
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define LATENCY_RANGE_CNT   (20)
#define BILLION             (1000000000L)

uint32_t range_endpoint[LATENCY_RANGE_CNT - 1] = { 100, 500, // ns
    1000, 50000, 100000, 200000, 300000, 400000,
    500000, 600000, 700000, 800000, 900000, // us
    1000000, 5000000, 10000000, 20000000, 50000000, 100000000 }; // ms

/* profile structure  */
struct prof {
    struct timespec start;
    struct timespec end;
    uint64_t range[LATENCY_RANGE_CNT];
    int (*func)(int, int); // profiling function pointer TODO: generalize this
};

void prof_create(struct prof* pf, void *func);
int prof(struct prof* pf, int arg1, int arg2); //TODO: generalize this
void calc_time(struct prof* pf);

void prof_create(struct prof* pf, void *func) {

    memset(&(pf->range), 0, sizeof(uint64_t) * LATENCY_RANGE_CNT);
    pf->func = func;
}

int prof(struct prof* pf, int arg1, int arg2) { //TODO: generalize this
    int ret;
    clock_gettime(CLOCK_MONOTONIC, &pf->start);
    ret = pf->func(10, 10); // TODO: generalize this
    clock_gettime(CLOCK_MONOTONIC, &pf->end);
    calc_time(pf);
    return ret;
}

void calc_time(struct prof* pf) {
    if (pf->start.tv_sec > pf->end.tv_sec || 
            (pf->start.tv_sec == pf->end.tv_sec && pf->start.tv_nsec > pf->end.tv_nsec)) {
        printf("Error, can't calculate.\n");
        return ;
    } else {
        uint64_t time_diff = (BILLION * (pf->end.tv_sec - pf->start.tv_sec) + pf->end.tv_nsec - pf->start.tv_nsec);
        int i = 0;
        for ( ; i < LATENCY_RANGE_CNT - 1; i++) {
            if (time_diff < range_endpoint[i]) {
                __sync_fetch_and_add(&(pf->range[i]), 1);                
                break;
            }
            if (i == LATENCY_RANGE_CNT - 1) {
                __sync_fetch_and_add(&(pf->range[LATENCY_RANGE_CNT-1]), 1);
            }
        }
    }
}

void print_latency_distr(struct prof* pf) {
    int i;
    for (i = 0; i < LATENCY_RANGE_CNT - 1 ; i++) {
        printf(" %4u%s ~ %4u%s: %"PRIu64"\n", 
                i == 0 ? 0 : 
                 (range_endpoint[i-1] >= 1000000 ? range_endpoint[i-1]/1000000 :
                  (range_endpoint[i-1] >= 1000 ? range_endpoint[i-1]/1000 : range_endpoint[i-1])),
                i == 0 ? "ns" : 
                 (range_endpoint[i-1] >= 1000000 ? "ms" : (range_endpoint[i-1] >= 1000 ? "us" : "ns")),
                range_endpoint[i] >= 1000000 ? range_endpoint[i]/1000000 : (range_endpoint[i] >= 1000 ? range_endpoint[i]/1000 : range_endpoint[i]),
                range_endpoint[i] >= 1000000 ? "ms" : (range_endpoint[i] >= 1000 ? "us" : "ns"),
                pf->range[i]); 
    }
    printf(" %4u%s ~       : %"PRIu64"\n", range_endpoint[i], "ms", pf->range[i]);
}

#endif
