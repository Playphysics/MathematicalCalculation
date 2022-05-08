#include <stdio.h>
#include <windows.h>

#include "Fibonacci.h"
#include "myMatrix.h"
#include "myBigInt.h"

typedef void (*FibFunc_T)(BigInt_T *retPtr, uint32_t elemN);
typedef void (*AddMul_T)(BigInt_T *retPtr, BigInt_CT in1, BigInt_CT in2);

static long long TickFreq;

static void PrtFibTime(FibFunc_T func, BigInt_T *retPtr, uint32_t elemN) {
    const uint32_t maxN = 100000U / elemN + (elemN > 100000U);
    long long t0, t1;
    QueryPerformanceCounter((LARGE_INTEGER *)&t0);

    for (uint32_t n = 0U; n < maxN; ++n) {
        func(retPtr, elemN);
    }
    QueryPerformanceCounter((LARGE_INTEGER *)&t1);
    BigInt_Prt(*retPtr, 8U, "  ");
    printf("Fib executeTime: %7.3fms\n", (t1 - t0) / maxN * 1000.0 / TickFreq);
}

static void PrtMulTime(uint32_t testNum, BigInt_T num[]) {
    const uint32_t maxN = 1000U / testNum + (testNum > 1000U);
    long long t0, t1;
    QueryPerformanceCounter((LARGE_INTEGER *)&t0);
    for (uint32_t n = 0U; n < maxN; ++n) {
        BigInt_Reset(&num[0], 1U, 0);

        for (uint32_t i = 0; i < testNum; ++i) {
            BigInt_Mul(&num[0], num[0], num[2]);
            BigInt_Mul(&num[0], num[0], num[3]);
        }
    }
    QueryPerformanceCounter((LARGE_INTEGER *)&t1);
    BigInt_Prt(num[0], 8U, "  ");
    printf("Mul executeTime: %7.3fs\n", (double)(t1 - t0) / maxN / TickFreq);
}

static void PrtAddTime(uint32_t testNum, BigInt_T num[]) {
    const uint32_t maxN = 100000U / testNum + (testNum > 100000U);
    long long t0, t1;
    QueryPerformanceCounter((LARGE_INTEGER *)&t0);
    for (uint32_t n = 0U; n < maxN; ++n) {
        BigInt_Reset(&num[0], 0U, 0);

        for (uint32_t i = 0U; i < testNum; ++i) {
            BigInt_Add(&num[0], num[0], num[2]);
            BigInt_Add(&num[0], num[0], num[3]);
            BigInt_Add(&num[0], num[0], num[0]);
        }
    }
    QueryPerformanceCounter((LARGE_INTEGER *)&t1);
    BigInt_Prt(num[0], 8U, "  ");
    printf("Add executeTime: %7.3fs\n", (double)(t1 - t0) / maxN / TickFreq);
}

static void PerformanceTest(BigInt_T num[]) {
    printf("\n=== BigInt Add Performance Test ===\n");
    BigInt_Reset(&num[2], 0x1234567887654321U, 0);
    BigInt_Reset(&num[3], 0U,
                 "0x1234567812345678123456781234567812345678123456781234567812345678"
                 "111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEE0000FFFF"
                 "1234567812345678123456781234567812345678123456781234567812345678"
                 "111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEE0000FFFF"
                 "1234567812345678123456781234567812345678123456781234567812345678"
                 "111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEE0000FFFF"
                 "1234567812345678123456781234567812345678123456781234567812345678"
                 "111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEE0000FFFF");

    const uint32_t add_N[] = {10000, 30000, 100000, 150151, 200000};
    for (size_t i = 0; i < sizeof(add_N) / sizeof(add_N[0]); ++i) {
        PrtAddTime(add_N[i], num);
    }

    printf("=== BigInt Mul Performance Test ===\n");
    const uint32_t mul_N[] = {100, 300, 700, 1000};
    for (size_t i = 0; i < sizeof(mul_N) / sizeof(mul_N[0]); ++i) {
        PrtMulTime(mul_N[i], num);
    }

    printf("=== Fibonacci Performance Test ===\n");
    const uint32_t fib_N[] = {1000, 10000, 50000, 100000};
    for (size_t i = 0; i < sizeof(fib_N) / sizeof(fib_N[0]); ++i) {
        PrtFibTime(Fibonacci_add, &num[0], fib_N[i]);
    }
    for (size_t i = 0; i < sizeof(fib_N) / sizeof(fib_N[0]); ++i) {
        PrtFibTime(Fibonacci_Mat, &num[0], fib_N[i]);
    }
}

static void BasicTestFunc(AddMul_T func, BigInt_T *retPtr, BigInt_CT src1, BigInt_CT src2) {
    func(retPtr, src1, src2), BigInt_Prt(*retPtr, 8U, "\n");
    func(retPtr, src2, src1), BigInt_Prt(*retPtr, 8U, "\n");
    func(retPtr, *retPtr, src1), BigInt_Prt(*retPtr, 8U, "\n");
    func(retPtr, *retPtr, src2), BigInt_Prt(*retPtr, 8U, "\n");

    func(retPtr, src1, src2), BigInt_Prt(*retPtr, 8U, "\n");
    func(retPtr, src2, src1), BigInt_Prt(*retPtr, 8U, "\n");
    func(retPtr, src1, *retPtr), BigInt_Prt(*retPtr, 8U, "\n");
    func(retPtr, src2, *retPtr), BigInt_Prt(*retPtr, 8U, "\n");

    func(retPtr, *retPtr, *retPtr), BigInt_Prt(*retPtr, 8U, "\n");
    printf("\n");
}

static void BasicTest(BigInt_T num[]) {
    printf("=== BasicTest ====== BasicTest ====== BasicTest ===\n");
    BasicTestFunc(BigInt_Add, &num[0], num[1], num[2]);
    BasicTestFunc(BigInt_Mul, &num[0], num[1], num[2]);
    BasicTestFunc(BigInt_Sub, &num[0], num[1], num[2]);

    printf("=== LargeTest ====== LargeTest ====== LargeTest ===\n");
    BigInt_Reset(&num[2], 0U, "0x11111111111111111111111111111111");

    BasicTestFunc(BigInt_Add, &num[0], num[1], num[2]);
    BasicTestFunc(BigInt_Mul, &num[0], num[1], num[2]);
    BasicTestFunc(BigInt_Sub, &num[0], num[1], num[2]);

    printf("\n=== FibSmallTest ====== FibSmallTest ====== FibSmallTest ===\n");
    for (uint32_t i = 1U; i <= 10U; ++i) {
        Fibonacci_Mat(&num[3], i), BigInt_Prt(num[3], 8U, " ");
        if (i == 5U) printf("\n");
    }
    printf("\n=== FibLargeTest ====== FibLargeTest ====== FibLargeTest ===\n");
    for (uint32_t i = 1U; i <= 5U; ++i) {
        Fibonacci_Mat(&num[3], i + 300U), BigInt_Prt(num[3], 8U, "\n");
    }
}

// gcc -O3 -Wall .\*.c -o .\mainFunc.exe; .\mainFunc.exe
int main() {
    BigInt_T num[4];
    for (int i = 0; i < 4; ++i) num[i] = BigInt_New(i + 1U, 0);

    QueryPerformanceFrequency((LARGE_INTEGER *)&TickFreq);

    if (0) {
        BasicTest(num);
    }
    if (1) {
        PerformanceTest(num);
    }
    for (int i = 0; i < 4; ++i) BigInt_Delete(num[i]);

    printf("\nBigInt Alloc: %I64u Free: %I64u\n\n", BigInt_mallocCount, BigInt_freeCount);
    return 0;
}
