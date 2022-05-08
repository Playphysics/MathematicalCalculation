#include "Fibonacci.h"
#include "myMatrix.h"

void Fibonacci_Mat(BigInt_T *retPtr, uint32_t elemN) {
    Matrix_T mat = Mat_New(2U, 2U, (const uint64_t []){1U, 1U, 1U, 0U});
    Matrix_T retMat = Mat_New(2U, 2U, (const uint64_t []){1U, 0U, 1U, 0U});

    if (elemN <= 2U) {
        BigInt_Reset(retPtr, 1U, 0);
    } else {
        for (elemN -= 3U; elemN; elemN >>= 1U) {
            if (elemN & 0x01U) {
                Mat_Mul(retMat, mat, retMat);
            }
            Mat_Mul(mat, mat, mat);
            //Mat_Prt(&retMat);
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
