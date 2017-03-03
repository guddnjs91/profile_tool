#include "ut0profile.h"

int funcA(int, int);
int funcB(int, int);
void* (*funcptr)(void*, void*);

int main(int argc, char* argv[]) {

    int a = 10, b = 10;
    int retA, retB;
    struct prof opA, opB;
    
    prof_create(&opA, funcA);
    prof_create(&opB, funcB);
    
    // "( ret (*) args ) func_addr" => represents function name
    funcptr = (void* (*) (void*, void*)) funcA;
    //retA = (int) funcptr(a, b);
    //retA = opA.func(a, b);
    retA = prof(&opA, a, b);
    printf("%d\n", retA);

    //retB = funcB(a, b);
    //retB = opB.func(a, b);
    retB = prof(&opB, a, b);
    printf("%d\n", retB);

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
