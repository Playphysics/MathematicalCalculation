#include "Fibonacci.h"
#include "myMatrix.h"

#include <stdlib.h>

void Fibonacci_Mat(BigInt_T *retPtr, uint32_t elemN) {
    Matrix_T mat = Mat_New(2U, 2U, (const uint64_t[]){1U, 1U, 1U, 0U});
    Matrix_T retMat = Mat_New(2U, 2U, (const uint64_t[]){1U, 0U, 1U, 0U});

    if (elemN <= 2U) {
        BigInt_Reset(retPtr, 1U, 0);
    } else {
        for (elemN -= 3U; elemN; elemN >>= 1U) {
            if (elemN & 0x01U) {
                Mat_Mul(retMat, mat, retMat);
            }
            Mat_Mul(mat, mat, mat);
            // Mat_Prt(&retMat);
        }
        BigInt_Add(retPtr, Mat_GetData(retMat, 0U, 0U), Mat_GetData(retMat, 1U, 0U));
    }

    Mat_Delete(mat);
    Mat_Delete(retMat);
}

void Fibonacci_add(BigInt_T *retPtr, uint32_t elemN) {
    BigInt_T p1 = BigInt_New(1U, 0);
    BigInt_T p2 = BigInt_New(1U, 0);
    BigInt_T p3 = BigInt_New(0U, 0);

    if (elemN <= 2U) {
        BigInt_Reset(retPtr, 1U, 0);
    } else {
        for (elemN -= 3U; elemN >= 1U; --elemN) {
            const BigInt_T p = p1;
            BigInt_Add(&p3, p1, p2);
            p1 = p2, p2 = p3, p3 = p;
        }
        BigInt_Add(retPtr, p1, p2);
    }
    BigInt_Delete(p1);
    BigInt_Delete(p2);
    BigInt_Delete(p3);
}

void Factorial_1(BigInt_T *retPtr, uint32_t elemN) {
    const BigInt_T one = BigInt_New(1U, 0);
    BigInt_T num = BigInt_New(0U, 0);
    BigInt_Reset(retPtr, 1U, 0);

    for (uint32_t i = 0U; i < elemN; ++i) {
        BigInt_Add(&num, num, one);
        BigInt_Mul(retPtr, *retPtr, num);
    }
    BigInt_Delete(num);
    BigInt_Delete(one);
}

void Factorial_2(BigInt_T *retPtr, uint32_t elemN) {
    const uint32_t arrLen = elemN;
    BigInt_T *const tempArr = malloc(arrLen * sizeof(BigInt_T));

    for (uint32_t i = 0U; i < arrLen; ++i) {
        tempArr[i] = BigInt_New(i + 1U, 0);
    }

    while (elemN >= 3U) {
        for (uint32_t i = (elemN + 1) / 2U; i < elemN; ++i) {
            const uint32_t destIdx = elemN - 1U - i;
            BigInt_Mul(&tempArr[destIdx], tempArr[destIdx], tempArr[i]);
        }
        elemN = (elemN + 1) / 2U;
    }
    BigInt_Mul(retPtr, tempArr[0], tempArr[1]);

    for (uint32_t i = 0U; i < arrLen; ++i) {
        BigInt_Delete(tempArr[i]);
    }
    free(tempArr);
}