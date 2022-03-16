
#include <stdio.h>
#include <stdlib.h>
#include "myMatrix.h"

struct Matrix_Impl_RawT {
    size_t row, col;
    BigInt_T data[];
};
typedef struct Matrix_Impl_RawT *Matrix_Impl_T;
typedef const struct Matrix_Impl_RawT *Matrix_Impl_CT;

static inline size_t MatRow(Matrix_CT mat) {
    return ((Matrix_Impl_CT)mat)->row;
}
static inline size_t MatCol(Matrix_CT mat) {
    return ((Matrix_Impl_CT)mat)->col;
}
static inline size_t MatElemNum(Matrix_CT mat) {
    return ((Matrix_Impl_CT)mat)->row * ((Matrix_Impl_CT)mat)->col;
}

static inline BigInt_T MatData(Matrix_CT mat, size_t row, size_t col) {
    const size_t idx = row * ((Matrix_Impl_CT)mat)->col + col;
    return ((Matrix_Impl_CT)mat)->data[idx];
}
static inline BigInt_T MatArrData(Matrix_CT mat, size_t idx) {
    return ((Matrix_Impl_CT)mat)->data[idx];
}
static inline BigInt_T *MatArray(Matrix_T mat) {
    return ((Matrix_Impl_T)mat)->data;
}

static inline int MatSameRow(Matrix_CT mat1, Matrix_CT mat2) {
    return MatRow(mat1) == MatRow(mat2);
}
static inline int MatSameCol(Matrix_CT mat1, Matrix_CT mat2) {
    return MatCol(mat1) == MatCol(mat2);
}
static inline int MatSameDim(Matrix_CT mat1, Matrix_CT mat2) {
    return (MatRow(mat1) == MatRow(mat2)) && (MatCol(mat1) == MatCol(mat2));
}

BigInt_T Mat_GetData(Matrix_CT mat, size_t row, size_t col) {
    return MatData(mat, row, col);
}

void Mat_Prt(Matrix_CT destMat) {
    size_t i, j;

    for (i = 0U; i < MatRow(destMat); ++i) {
        for (j = 0U; j < MatCol(destMat) - 1U; ++j) {
            BigInt_Prt(MatData(destMat, i, j), 2U, " ");
        }
        BigInt_Prt(MatData(destMat, i, j), 2U, "\n");
    }
}

Matrix_T Mat_New(size_t row, size_t col, const uint64_t initData[]) {
    const size_t num = row * col;
    const size_t bufLen = num * sizeof(BigInt_T) + 2U * sizeof(size_t);
    Matrix_Impl_T retMat = malloc(bufLen);

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
        BigInt_Delete(MatArray(destMat)[i]);
    }
    free(destMat);
}

static void Mat_SwapWithSameDim(Matrix_T mat1, Matrix_T mat2) {
    if (!MatSameDim(mat1, mat2)) return;
    const size_t num = MatRow(mat1) * MatCol(mat1);

    for (size_t i = 0U; i < num; ++i) {
        BigInt_Swap(MatArray(mat1) + i, MatArray(mat2) + i);
    }
}

void Mat_Add(Matrix_T retMat, Matrix_CT mat1, Matrix_CT mat2) {
    if (!MatSameDim(retMat, mat1) || !MatSameDim(mat1, mat2)) return;
    const size_t num = MatElemNum(retMat);

    for (size_t i = 0U; i < num; ++i) {
        BigInt_Add(MatArray(retMat)[i], MatArrData(mat1, i), MatArrData(mat2, i));
    }
}

void Mat_Mul(Matrix_T retMat, Matrix_CT mat1, Matrix_CT mat2) {
    if (MatCol(mat1) != MatRow(mat2)) return;
    if (!MatSameRow(retMat, mat1) || !MatSameCol(retMat, mat2)) return;
    Matrix_Impl_T ret = Mat_New(MatRow(retMat), MatCol(retMat), 0);
    BigInt_T temp = BigInt_New(0x00U, 0);

    for (size_t i = 0U; i < ret->row; ++i) {
        for (size_t j = 0U; j < ret->col; ++j) {
            for (size_t k = 0U; k < MatCol(mat1); ++k) {
                const size_t idxRet = i * ret->col + j;
                BigInt_Mul(&temp, MatData(mat1, i, k), MatData(mat2, k, j));
                BigInt_Add(&MatArray(ret)[idxRet], MatArray(ret)[idxRet], temp);
            }
        }
    }
    BigInt_Delete(temp);
    Mat_SwapWithSameDim(retMat, ret);
    Mat_Delete(ret);
}
