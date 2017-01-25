#ifndef __PROFILE_H__
#define __PROFILE_H__
#include <time.h>
#include <string.h>
#include <stdio.h>

#define LATENCY_RANGE_CNT   (20)
#define BILLION             (1000000000L)

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

uint32_t range_endpoint[LATENCY_RANGE_CNT - 1] = { 100, 500, // ns
    1000, 50000, 100000, 200000, 300000, 400000,
    500000, 600000, 700000, 800000, 900000, // us
    1000000, 5000000, 10000000, 20000000, 50000000, 100000000 }; // ms
uint32_t global_counter; // unique id provider

struct op_obj {
    uint32_t oid; // operation id
    struct timespec start;
    struct timespec end;
    uint64_t range[LATENCY_RANGE_CNT];
};

void init_op_obj(struct op_obj* op) {    
    op->oid = __sync_fetch_and_add(&global_counter, 1);
    memset(&(op->range), 0, sizeof(uint64_t) * LATENCY_RANGE_CNT);
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
        printf(" %4u%s ~ %4u%s: %llu\n", 
                i == 0 ? 0 : 
                 (range_endpoint[i-1] >= 1000000 ? range_endpoint[i-1]/1000000 :
                  (range_endpoint[i-1] >= 1000 ? range_endpoint[i-1]/1000 : range_endpoint[i-1])),
                i == 0 ? "ns" : 
                 (range_endpoint[i-1] >= 1000000 ? "ms" : (range_endpoint[i-1] >= 1000 ? "us" : "ns")),
                range_endpoint[i] >= 1000000 ? range_endpoint[i]/1000000 : (range_endpoint[i] >= 1000 ? range_endpoint[i]/1000 : range_endpoint[i]),
                range_endpoint[i] >= 1000000 ? "ms" : (range_endpoint[i] >= 1000 ? "us" : "ns"),
                op->range[i]); 
    }
    printf(" %4u%s ~       : %llu\n", range_endpoint[i], "ms", op->range[i]);
}

#endif
