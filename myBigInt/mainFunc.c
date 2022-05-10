#include <stdio.h>
#include <windows.h>

#include "Fibonacci.h"
#include "myMatrix.h"
#include "myBigInt.h"

typedef void (*Fib_T)(BigInt_T *retPtr, uint32_t elemN);

static long long TickFreq;

static void PrtFibTime(Fib_T func, BigInt_T *retPtr, uint32_t elemN) {
    long long t0, t1;

    QueryPerformanceCounter((LARGE_INTEGER *)&t0);
    func(retPtr, elemN);
    QueryPerformanceCounter((LARGE_INTEGER *)&t1);

    BigInt_Prt(*retPtr, 10U, " - ");
    printf("%7.3fms\n", (t1 - t0) * 1000.0 / TickFreq);
}

static void PerformanceTest(BigInt_T *num) {
    printf("=== Fibonacci Performance Test ===\n");
    PrtFibTime(Fibonacci_add, num, 100000U); // 181.186
    PrtFibTime(Fibonacci_Mat, num, 100000U); // 34.778

    PrtFibTime(Factorial_1, num, 20000U);
    PrtFibTime(Factorial_2, num, 20000U);
}

// gcc -O3 -Wall .\*.c -o .\mainFunc.exe; .\mainFunc.exe
int main() {
    BigInt_T num = BigInt_New(0U, 0);

    QueryPerformanceFrequency((LARGE_INTEGER *)&TickFreq);

    if (0) BigInt_Test();
    if (1) PerformanceTest(&num);

    BigInt_Delete(num);

    printf("\nBigInt Alloc: %I64u Free: %I64u\n\n", BigInt_mallocCount, BigInt_freeCount);
    return 0;
}
