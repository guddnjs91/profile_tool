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

struct op_obj {
    struct timespec start;
    struct timespec end;
    uint64_t range[LATENCY_RANGE_CNT];
    int (*func)(int, int); // function pointer TODO: generalize this
};

void init_op_obj(struct op_obj* op, void *func) {

    memset(&(op->range), 0, sizeof(uint64_t) * LATENCY_RANGE_CNT);
    op->func = func;
}

void calc_time(struct op_obj* op) {
    if (op->start.tv_sec > op->end.tv_sec || 
            (op->start.tv_sec == op->end.tv_sec && op->start.tv_nsec > op->end.tv_nsec)) {
        printf("Error, can't calculate.\n");
        return ;
    } else {
        uint64_t time_diff = (BILLION * (op->end.tv_sec - op->start.tv_sec) + op->end.tv_nsec - op->start.tv_nsec);
        int i = 0;
        for ( ; i < LATENCY_RANGE_CNT - 1; i++) {
            if (time_diff < range_endpoint[i]) {
                __sync_fetch_and_add(&(op->range[i]), 1);                
                break;
            }
            if (i == LATENCY_RANGE_CNT - 1) {
                __sync_fetch_and_add(&(op->range[LATENCY_RANGE_CNT-1]), 1);
            }
        }
    }
}

void print_latency_distr(struct op_obj* op) {
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
                op->range[i]); 
    }
    printf(" %4u%s ~       : %"PRIu64"\n", range_endpoint[i], "ms", op->range[i]);
}

#endif
