#include <stdio.h>
#include <windows.h>

#include "Fibonacci.h"
#include "myMatrix.h"
#include "myMatrix22.h"
#include "myBigInt.h"

typedef void (*FibFunc_T)(BigInt_T *retPtr, uint32_t elemN);
typedef void (*AddMul_T)(BigInt_T *retPtr, BigInt_CT in1, BigInt_CT in2);

static long long TickFreq;

void PrtFibTime(FibFunc_T func, BigInt_T *retPtr, uint32_t elemN) {
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

void PrtMulTime(uint32_t testNum, Matrix22_T mat) {
    const uint32_t maxN = 1000U / testNum + (testNum > 1000U);
    long long t0, t1;
    QueryPerformanceCounter((LARGE_INTEGER *)&t0);
    for (uint32_t n = 0U; n < maxN; ++n) {
        BigInt_Reset(&mat->a11, 1U, 0);

        for (uint32_t i = 0; i < testNum; ++i) {
            BigInt_Mul(&mat->a11, mat->a11, mat->a21);
            BigInt_Mul(&mat->a11, mat->a11, mat->a22);
        }
    }
    QueryPerformanceCounter((LARGE_INTEGER *)&t1);
    BigInt_Prt(mat->a11, 8U, "  ");
    printf("Mul executeTime: %7.3fs\n", (double)(t1 - t0) / maxN / TickFreq);
}

void PrtAddTime(uint32_t testNum, Matrix22_T mat) {
    const uint32_t maxN = 100000U / testNum + (testNum > 100000U);
    long long t0, t1;
    QueryPerformanceCounter((LARGE_INTEGER *)&t0);
    for (uint32_t n = 0U; n < maxN; ++n) {
        BigInt_Reset(&mat->a11, 0U, 0);

        for (uint32_t i = 0U; i < testNum; ++i) {
            BigInt_Add(&mat->a11, mat->a11, mat->a21);
            BigInt_Add(&mat->a11, mat->a11, mat->a22);
            BigInt_Add(&mat->a11, mat->a11, mat->a11);
        }
    }
    QueryPerformanceCounter((LARGE_INTEGER *)&t1);
    BigInt_Prt(mat->a11, 8U, "  ");
    printf("Add executeTime: %7.3fs\n", (double)(t1 - t0) / maxN / TickFreq);
}

static void PerformanceTest(Matrix22_T mat) {
    printf("\n=== BigInt Add Performance Test ===\n");
    BigInt_Reset(&mat->a21, 0x1234567887654321U, 0);
    BigInt_Reset(&mat->a22, 0U,
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
        PrtAddTime(add_N[i], mat);
    }

    printf("=== BigInt Mul Performance Test ===\n");
    const uint32_t mul_N[] = {100, 300, 700, 1000};
    for (size_t i = 0; i < sizeof(mul_N) / sizeof(mul_N[0]); ++i) {
        PrtMulTime(mul_N[i], mat);
    }

    printf("=== Fibonacci Performance Test ===\n");
    const uint32_t fib_N[] = {1000, 10000, 50000, 100000};
    for (size_t i = 0; i < sizeof(fib_N) / sizeof(fib_N[0]); ++i) {
        PrtFibTime(Fibonacci_add, &mat->a11, fib_N[i]);
    }
    for (size_t i = 0; i < sizeof(fib_N) / sizeof(fib_N[0]); ++i) {
        PrtFibTime(Fibonacci_Mat, &mat->a11, fib_N[i]);
    }
    for (size_t i = 0; i < sizeof(fib_N) / sizeof(fib_N[0]); ++i) {
        PrtFibTime(Fibonacci, &mat->a11, fib_N[i]);
    }
}

void AddMulBasic(AddMul_T func, BigInt_T *retPtr, BigInt_CT src1, BigInt_CT src2) {
    func(retPtr, src1, src2), BigInt_Prt(*retPtr, 8U, "  ");
    func(retPtr, *retPtr, src1), BigInt_Prt(*retPtr, 8U, "  ");
    func(retPtr, src2, *retPtr), BigInt_Prt(*retPtr, 8U, "  ");
    func(retPtr, *retPtr, *retPtr), BigInt_Prt(*retPtr, 8U, "\n");
}

void AddMulLarge(AddMul_T func, BigInt_T *retPtr, BigInt_CT src1, BigInt_CT src2) {
    func(retPtr, *retPtr, *retPtr), BigInt_Prt(*retPtr, 8U, "\n");
    func(retPtr, src1, src2), BigInt_Prt(*retPtr, 8U, "\n");
    func(retPtr, *retPtr, src1), BigInt_Prt(*retPtr, 8U, "\n");
    func(retPtr, src2, *retPtr), BigInt_Prt(*retPtr, 8U, "\n");
    func(retPtr, *retPtr, *retPtr), BigInt_Prt(*retPtr, 8U, "\n");
    printf("\n");
}

static void BasicTest(Matrix22_T mat) {
    printf("=== BasicTest ====== BasicTest ====== BasicTest ===\n");
    AddMulBasic(BigInt_Add, &mat->a11, mat->a12, mat->a21);
    AddMulBasic(BigInt_Mul, &mat->a11, mat->a12, mat->a21);
    AddMulBasic(BigInt_Sub, &mat->a11, mat->a12, mat->a21);

    printf("=== LargeTest ====== LargeTest ====== LargeTest ===\n");
    BigInt_Reset(&mat->a21, 0U, "0x11111111111111111111111111111111");

    AddMulLarge(BigInt_Add, &mat->a11, mat->a12, mat->a21);
    AddMulLarge(BigInt_Mul, &mat->a11, mat->a12, mat->a21);
    AddMulLarge(BigInt_Sub, &mat->a11, mat->a12, mat->a21);

    printf("\n=== FibSmallTest ====== FibSmallTest ====== FibSmallTest ===\n");
    for (uint32_t i = 1U; i <= 5U; ++i) {
        Fibonacci_Mat(&mat->a22, i), BigInt_Prt(mat->a22, 8U, " ");
    }
    printf("\n");
    for (uint32_t i = 1U; i <= 5U; ++i) {
        Fibonacci_Mat(&mat->a22, i + 5U), BigInt_Prt(mat->a22, 8U, " ");
    }
    printf("\n=== FibLargeTest ====== FibLargeTest ====== FibLargeTest ===\n");
    for (uint32_t i = 1U; i <= 5U; ++i) {
        Fibonacci_Mat(&mat->a22, i + 300U), BigInt_Prt(mat->a22, 8U, "\n");
    }
}

int main() {
    Matrix22_T mat = Mat22_New(1U, 1U, 1U, 0U);

    QueryPerformanceFrequency((LARGE_INTEGER *)&TickFreq);

    if (1) {
        BasicTest(mat);
    }
    if (1) {
        PerformanceTest(mat);
    }
    Mat22_Delete(mat);

    printf("\nBigInt Alloc: %I64u Free: %I64u\n\n", BigInt_mallocCount, BigInt_freeCount);
    return 0;
}
