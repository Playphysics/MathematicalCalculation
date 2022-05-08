
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "myMatrix.h"

struct Matrix_Impl_T {
    size_t row, col;
    BigInt_T data[];
};

static inline size_t MatElemNum(Matrix_CT mat) {
    return mat->row * mat->col;
}
static inline BigInt_T MatData(Matrix_CT mat, size_t row, size_t col) {
    const size_t idx = row * mat->col + col;
    return mat->data[idx];
}

BigInt_T Mat_GetData(Matrix_CT mat, size_t row, size_t col) {
    return MatData(mat, row, col);
}

void Mat_Prt(Matrix_CT destMat) {
    size_t i, j;

    for (i = 0U; i < destMat->row; ++i) {
        for (j = 0U; j < destMat->col - 1U; ++j) {
            BigInt_Prt(MatData(destMat, i, j), 2U, " ");
        }
        BigInt_Prt(MatData(destMat, i, j), 2U, "\n");
    }
}

Matrix_T Mat_New(size_t row, size_t col, const uint64_t initData[]) {
    const size_t num = row * col;
    const size_t bufLen = num * sizeof(BigInt_T) + 2U * sizeof(size_t);
    Matrix_T retMat = malloc(bufLen);

    retMat->row = row, retMat->col = col;

    for (size_t i = 0U; i < num; ++i) {
        const uint64_t init = (initData == 0) ? 0U : initData[i];
        retMat->data[i] = BigInt_New(init, 0);
    }
    return retMat;
}

void Mat_Delete(Matrix_T destMat) {
    const size_t num = MatElemNum(destMat);
    for (size_t i = 0U; i < num; ++i) {
        BigInt_Delete(destMat->data[i]);
    }
    free(destMat);
}

static void Mat_SwapWithSameDim(Matrix_T mat1, Matrix_T mat2) {
    assert(mat1->row == mat2->row && mat1->col == mat2->col);
    const size_t num = mat1->row * mat1->col;

    for (size_t i = 0U; i < num; ++i) {
        BigInt_Swap(mat1->data + i, mat2->data + i);
    }
}

void Mat_Add(Matrix_T retMat, Matrix_CT mat1, Matrix_CT mat2) {
    assert(mat1->row == mat2->row && mat1->col == mat2->col);
    assert(retMat->row == mat2->row && retMat->col == mat2->col);

    const size_t num = MatElemNum(retMat);

    for (size_t i = 0U; i < num; ++i) {
        BigInt_Add(retMat->data + i, mat1->data[i], mat2->data[i]);
    }
}

void Mat_Mul(Matrix_T retMat, Matrix_CT mat1, Matrix_CT mat2) {
    assert(mat1->col == mat2->row);
    assert(retMat->row == mat1->row);
    assert(retMat->col == mat2->col);

    Matrix_T ret = Mat_New(retMat->row, retMat->col, 0);
    BigInt_T temp = BigInt_New(0x00U, 0);

    for (size_t i = 0U; i < ret->row; ++i) {
        for (size_t j = 0U; j < ret->col; ++j) {
            const size_t idxRet = i * ret->col + j;
            for (size_t k = 0U; k < mat1->col; ++k) {
                BigInt_Mul(&temp, MatData(mat1, i, k), MatData(mat2, k, j));
                BigInt_Add(ret->data + idxRet, ret->data[idxRet], temp);
            }
        }
    }
    BigInt_Delete(temp);
    Mat_SwapWithSameDim(retMat, ret);
    Mat_Delete(ret);
}
