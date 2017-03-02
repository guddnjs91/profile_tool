#include "ut0profile.h"

int funcA(int, int);
int funcB(int, int);
void* (*funcptr)(void*, void*);

int main(int argc, char* argv[]) {

    int a = 10, b = 10;
    int retA, retB;
    struct op_obj opA, opB;
    
    init_op_obj(&opA, funcA);
    init_op_obj(&opB, funcB);
    
    clock_gettime(CLOCK_MONOTONIC, &opA.start);
    // "( ret (*) args ) func_addr" => represents function name
    funcptr = (void* (*) (void*, void*)) funcA;
    //retA = (int) funcptr(a, b);
    retA = opA.func(a, b);
    printf("%d\n", retA);
    clock_gettime(CLOCK_MONOTONIC, &opA.end);
    calc_time(&opA);

    clock_gettime(CLOCK_MONOTONIC, &opB.start);
    //retB = funcB(a, b);
    retB = opB.func(a, b);
    printf("%d\n", retB);
    clock_gettime(CLOCK_MONOTONIC, &opB.end);
    calc_time(&opB);

    printf("opA\n");
    print_latency_distr(&opA);
    printf("opB\n");
    print_latency_distr(&opB);

    return 0;
}

int funcA(int x, int y) {
    return x + y;
}

int funcB(int x, int y) {
    int i;
    for (i = 0; i < 10000; i++) {
        x += y;
    }
    return x + y;
}
