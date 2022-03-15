
#include <stdio.h>
#include <stdlib.h>
#include "myMatrix22.h"
#include "myBigInt.h"

void Mat22_Prt(Matrix22_CT destMat) {
    BigInt_Prt(destMat->a11, 2U, " ");
    BigInt_Prt(destMat->a12, 2U, "\n");
    BigInt_Prt(destMat->a21, 2U, " ");
    BigInt_Prt(destMat->a22, 2U, "\n");
}

Matrix22_T Mat22_New(uint32_t a11, uint32_t a12, uint32_t a21, uint32_t a22) {
    Matrix22_T retMat = malloc(sizeof(*retMat));
    retMat->a11 = BigInt_New(a11, 0);
    retMat->a12 = BigInt_New(a12, 0);
    retMat->a21 = BigInt_New(a21, 0);
    retMat->a22 = BigInt_New(a22, 0);
    return retMat;
}

void Mat22_Delete(Matrix22_T destMat) {
    BigInt_Delete(destMat->a11);
    BigInt_Delete(destMat->a12);
    BigInt_Delete(destMat->a21);
    BigInt_Delete(destMat->a22);
    free(destMat);
}

void Mat22_Swap(Matrix22_T mat1, Matrix22_T mat2) {
    BigInt_Swap(&mat1->a11, &mat2->a11);
    BigInt_Swap(&mat1->a12, &mat2->a12);
    BigInt_Swap(&mat1->a21, &mat2->a21);
    BigInt_Swap(&mat1->a22, &mat2->a22);
}

void Mat22_Add(Matrix22_T retMat, Matrix22_CT mat1, Matrix22_CT mat2) {
    BigInt_Add(&retMat->a11, mat1->a11, mat2->a11);
    BigInt_Add(&retMat->a12, mat1->a12, mat2->a12);
    BigInt_Add(&retMat->a21, mat1->a21, mat2->a21);
    BigInt_Add(&retMat->a22, mat1->a22, mat2->a22);
}

void Mat22_Mul(Matrix22_T retMat, Matrix22_CT mat1, Matrix22_CT mat2) {
    Matrix22_T ret = Mat22_New(0U, 0U, 0U, 0U);  // maybe retMat == mat1 or mat2
    BigInt_T temp1 = BigInt_New(0x00U, 0);
    BigInt_T temp2 = BigInt_New(0x00U, 0);

    BigInt_Mul(&temp1, mat1->a11, mat2->a11);
    BigInt_Mul(&temp2, mat1->a12, mat2->a21);
    BigInt_Add(&ret->a11, temp1, temp2);

    BigInt_Mul(&temp1, mat1->a11, mat2->a12);
    BigInt_Mul(&temp2, mat1->a12, mat2->a22);
    BigInt_Add(&ret->a12, temp1, temp2);

    BigInt_Mul(&temp1, mat1->a21, mat2->a11);
    BigInt_Mul(&temp2, mat1->a22, mat2->a21);
    BigInt_Add(&ret->a21, temp1, temp2);

    BigInt_Mul(&temp1, mat1->a21, mat2->a12);
    BigInt_Mul(&temp2, mat1->a22, mat2->a22);
    BigInt_Add(&ret->a22, temp1, temp2);

    BigInt_Delete(temp1);
    BigInt_Delete(temp2);

    Mat22_Swap(retMat, ret);
    Mat22_Delete(ret);
}
