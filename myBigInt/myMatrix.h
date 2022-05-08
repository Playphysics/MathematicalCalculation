#ifndef MY_MATRIX_H_
#define MY_MATRIX_H_

#include <stdint.h>
#include "myBigInt.h"

struct Matrix_Impl_T;
typedef struct Matrix_Impl_T *Matrix_T;
typedef const struct Matrix_Impl_T *Matrix_CT;

extern void Mat_Prt(Matrix_CT destMat);
extern Matrix_T Mat_New(size_t row, size_t col, const uint64_t initData[]);
extern void Mat_Delete(Matrix_T destMat);

extern BigInt_T Mat_GetData(Matrix_CT mat, size_t row, size_t col);

extern void Mat_Add(Matrix_T retMat, Matrix_CT mat1, Matrix_CT mat2);
extern void Mat_Mul(Matrix_T retMat, Matrix_CT mat1, Matrix_CT mat2);

#endif
