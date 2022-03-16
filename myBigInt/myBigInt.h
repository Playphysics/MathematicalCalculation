#ifndef MY_BIG_INT_H
#define MY_BIG_INT_H

#include <stdint.h>

typedef void *BigInt_T;
typedef const void *BigInt_CT;

extern volatile size_t BigInt_mallocCount, BigInt_freeCount;

extern void BigInt_Prt(BigInt_CT ret, size_t maxWidth, const char *end);

extern BigInt_T BigInt_New(uint64_t num, const char *hexStr);
extern void BigInt_Delete(BigInt_T ret);

extern void BigInt_Reset(BigInt_T *retPtr, uint64_t num, const char *hexStr);
extern void BigInt_Swap(BigInt_T *in1, BigInt_T *in2);

extern void BigInt_Add(BigInt_T *retPtr, BigInt_CT in1, BigInt_CT in2);
extern void BigInt_Sub(BigInt_T *retPtr, BigInt_CT in1, BigInt_CT in2);
extern void BigInt_Mul(BigInt_T *retPtr, BigInt_CT in1, BigInt_CT in2);

#endif
