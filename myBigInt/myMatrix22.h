#ifndef MY_MATRIX_22_H
#define MY_MATRIX_22_H

#include "myBigInt.h"

struct Matrix22_RawT {
    BigInt_T a11, a12;
    BigInt_T a21, a22;
};
typedef struct Matrix22_RawT *Matrix22_T;
typedef const struct Matrix22_RawT *Matrix22_CT;

extern void Mat22_Prt(Matrix22_CT destMat);

extern Matrix22_T Mat22_New(uint32_t a11, uint32_t a12, uint32_t a21, uint32_t a22);
extern void Mat22_Delete(Matrix22_T destMat);
extern void Mat22_Swap(Matrix22_T mat1, Matrix22_T mat2);

extern void Mat22_Add(Matrix22_T ret, Matrix22_CT mat1, Matrix22_CT mat2);
extern void Mat22_Mul(Matrix22_T ret, Matrix22_CT mat1, Matrix22_CT mat2);

#endif
