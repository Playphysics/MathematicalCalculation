#include "myBigInt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BigInt_Impl_RawT {
    size_t bufferLen;
    size_t dataLen;
    uint32_t data[];
};
typedef struct BigInt_Impl_RawT *BigInt_Impl_T;
typedef const struct BigInt_Impl_RawT *BigInt_Impl_CT;

// === Alloc/Free ====== Alloc/Free ====== Alloc/Free ====== Alloc/Free ===
// === Alloc/Free ====== Alloc/Free ====== Alloc/Free ====== Alloc/Free ===
// === Alloc/Free ====== Alloc/Free ====== Alloc/Free ====== Alloc/Free ===

volatile size_t BigInt_mallocCount, BigInt_freeCount;

static BigInt_Impl_T BigInt_Alloc(size_t dataLen) {
    const size_t mask = 0x0FU;
    const size_t extraSize = 2U * sizeof(size_t);
    const size_t bufLen = (dataLen + extraSize + mask) & ~mask;
    BigInt_Impl_T retPtr = 0;

    if (bufLen > dataLen) {
        BigInt_mallocCount += 1U;
        retPtr = (BigInt_Impl_T)malloc(bufLen);
        retPtr->bufferLen = bufLen - extraSize;
        retPtr->dataLen = 0U;
    }
    return retPtr;
}
static void BigInt_Free(BigInt_Impl_T ptr) {
    BigInt_freeCount += 1U;
    free((void *)ptr);
}

// === convert from/to str ====== convert from/to str ====== convert from/to str ===
// === convert from/to str ====== convert from/to str ====== convert from/to str ===
// === convert from/to str ====== convert from/to str ====== convert from/to str ===

void BigInt_Prt(BigInt_CT ret, size_t maxWidth, const char *end) {
    BigInt_Impl_CT dest = ret;
    size_t u32Size = (dest->dataLen + 3U) >> 2U;

    printf("%8u: 0x", (unsigned int)dest->dataLen);

    if (u32Size == 0U) {
        printf("%08X", 0U);
    } else if (u32Size > maxWidth && maxWidth >= 2U) {
        for (size_t i = 0U; i < maxWidth / 2U - 1U; ++i) {
            printf("%08X ", dest->data[--u32Size]);
        }
        printf("%08X ...", dest->data[--u32Size]);
        u32Size = maxWidth / 2U;
    } else {
        printf("%08X", dest->data[--u32Size]);
    }
    while (u32Size != 0U) {
        printf(" %08X", dest->data[--u32Size]);
    }
    printf("%s", end);
}

static const unsigned char BigInt_StrToNum[] = {
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  // 0 - 15
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  // 16 - 31
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  // 32 - 47
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  37, 37, 37, 37, 37, 37,  // 48 - 63
    37, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,  // 64 - 79
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 37, 37, 37, 37,  // 80 - 95
    37, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,  // 96 - 111
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 37, 37, 37, 37,  // 112 - 127
};

static void BigInt_FromHexStr(uint32_t *dest, const char *hexStr, size_t hexLen) {
    while (hexLen >= 8U) {
        uint32_t temp = (uint32_t)BigInt_StrToNum[hexStr[--hexLen] & 0x7FU];
        for (uint32_t i = 1U; i < 8U; ++i) {
            temp |= (uint32_t)BigInt_StrToNum[hexStr[--hexLen] & 0x7FU] << (i << 2U);
        }
        *dest++ = temp;
    }
    if (hexLen != 0U) {
        uint32_t temp = (uint32_t)BigInt_StrToNum[hexStr[--hexLen] & 0x7FU];
        for (uint32_t i = 1U; i <= hexLen; ++i) {
            temp |= (uint32_t)BigInt_StrToNum[hexStr[hexLen - i] & 0x7FU] << (i << 2U);
        }
        *dest = temp;
    }
}

// === constructor ====== destructors ====== constructor ====== destructors ===
// === constructor ====== destructors ====== constructor ====== destructors ===
// === constructor ====== destructors ====== constructor ====== destructors ===

static void BigInt_ReduceToRealSize(BigInt_Impl_T ret, size_t curSize) {
    uint32_t i, temp;
    size_t retSize = (curSize + 3U) >> 2U;

    while (retSize != 0U) {
        if (ret->data[--retSize] != 0U) break;
    }
    temp = ret->data[retSize];
    for (i = 0U; i < 32U; i += 8U) {
        if (temp & (0xFF000000U >> i)) break;
    }
    ret->dataLen = 4U - (i >> 3U) + (retSize << 2U);
}

BigInt_T BigInt_New(uint64_t num, const char *hexStr) {
    BigInt_Impl_T ret;
    size_t byteNum = sizeof(num);

    if (hexStr == 0 || hexStr[0] != '0' || hexStr[1] == '\0') {
        ret = BigInt_Alloc(byteNum);
        ret->data[0] = (uint32_t)(num >> 0U);
        ret->data[1] = (uint32_t)(num >> 32U);
    } else {
        const size_t hexLen = strlen(hexStr + 2);
        byteNum = (hexLen >> 1U) + (hexLen & 0x01U);
        ret = BigInt_Alloc(byteNum);
        BigInt_FromHexStr(ret->data, hexStr + 2, hexLen);
    }
    BigInt_ReduceToRealSize(ret, byteNum);
    return ret;
}

void BigInt_Delete(BigInt_T ret) {
    BigInt_Free((BigInt_Impl_T)ret);
}

void BigInt_Reset(BigInt_T *retPtr, uint64_t num, const char *hexStr) {
    BigInt_Impl_T ret = *retPtr;
    size_t byteNum = sizeof(num);
    BigInt_Free(ret);

    if (hexStr == 0 || hexStr[0] != '0' || hexStr[1] == '\0') {
        ret = BigInt_Alloc(byteNum);
        ret->data[0] = (uint32_t)(num >> 0U);
        ret->data[1] = (uint32_t)(num >> 32U);
    } else {
        const size_t hexLen = strlen(hexStr + 2);
        byteNum = (hexLen >> 1U) + (hexLen & 0x01U);
        ret = BigInt_Alloc(byteNum);
        BigInt_FromHexStr(ret->data, hexStr + 2, hexLen);
    }
    BigInt_ReduceToRealSize(ret, byteNum);
    *retPtr = ret;
}

void BigInt_Swap(BigInt_T *in1, BigInt_T *in2) {
    BigInt_T temp = *in1;
    *in1 = *in2, *in2 = temp;
}

// === operation ====== operation ====== operation ====== operation ====== operation ===
// === operation ====== operation ====== operation ====== operation ====== operation ===
// === operation ====== operation ====== operation ====== operation ====== operation ===

// return carry
static uint32_t Uint32Add(uint32_t *retPtr, uint32_t in1, uint32_t in2, uint32_t carry) {
    uint32_t temp = in1 + carry;
    carry = temp < carry;

    temp += in2;
    carry |= temp < in2;

    *retPtr = temp;
    return carry;
}

static void BigInt_RawAdd(BigInt_Impl_T ret, BigInt_Impl_CT large, BigInt_Impl_CT small) {
    const size_t smallLen = (small->dataLen + 3U) >> 2U;
    const size_t largeLen = (large->dataLen + 3U) >> 2U;
    uint32_t carry = 0U;

    for (size_t i = 0U; i < smallLen; ++i) {
        carry = Uint32Add(&ret->data[i], large->data[i], small->data[i], carry);
    }
    for (size_t i = smallLen; i < largeLen; ++i) {
        carry = Uint32Add(&ret->data[i], large->data[i], 0U, carry);
    }
    ret->data[largeLen] = carry;
}

void BigInt_Add(BigInt_T *retPtr, BigInt_CT in1, BigInt_CT in2) {
    BigInt_Impl_CT large, small;
    BigInt_Impl_T ret = *retPtr;
    size_t retSize;

    if (((BigInt_Impl_CT)in1)->dataLen >= ((BigInt_Impl_CT)in2)->dataLen) {
        large = (BigInt_Impl_CT)in1, small = (BigInt_Impl_CT)in2;
    } else {
        large = (BigInt_Impl_CT)in2, small = (BigInt_Impl_CT)in1;
    }
    retSize = large->dataLen + sizeof(large->data[0]);
    if (ret->bufferLen < retSize) ret = BigInt_Alloc(retSize);

    BigInt_RawAdd(ret, large, small);
    BigInt_ReduceToRealSize(ret, large->dataLen + 1U);

    if (ret != *retPtr) {
        BigInt_Free((BigInt_Impl_T)*retPtr);
        *retPtr = ret;
    }
}

static void BigInt_RawSub(BigInt_Impl_T ret, BigInt_Impl_CT in1, BigInt_Impl_CT in2) {
    const size_t in1Len = (in1->dataLen + 3U) >> 2U;
    const size_t in2Len = (in2->dataLen + 3U) >> 2U;
    size_t i, smallSize = (in1Len < in2Len) ? in1Len : in2Len;
    uint32_t carry = 1U;

    for (i = 0U; i < smallSize; ++i) {
        carry = Uint32Add(&ret->data[i], in1->data[i], ~in2->data[i], carry);
    }
    for (i = smallSize; i < in1Len && carry == 0U; ++i) {
        carry = Uint32Add(&ret->data[i], in1->data[i], 0xFFFFFFFFU, carry);
    }
    for (i = smallSize; i < in2Len; ++i) {
        carry = Uint32Add(&ret->data[i], 0U, ~in2->data[i], carry);
    }
}

void BigInt_Sub(BigInt_T *retPtr, BigInt_CT in1, BigInt_CT in2) {
    BigInt_Impl_T ret = *retPtr;
    size_t retSize = ((BigInt_Impl_CT)in1)->dataLen;

    if (((BigInt_Impl_CT)in1)->dataLen < ((BigInt_Impl_CT)in2)->dataLen) {
        retSize = ((BigInt_Impl_CT)in2)->dataLen;
    }
    if (ret->bufferLen < retSize) ret = BigInt_Alloc(retSize);

    BigInt_RawSub(ret, in1, in2);
    BigInt_ReduceToRealSize(ret, retSize);

    if (ret != *retPtr) {
        BigInt_Free((BigInt_Impl_T)*retPtr);
        *retPtr = ret;
    }
}

static void BigInt_RawMul(BigInt_Impl_T ret, BigInt_Impl_CT n1, BigInt_Impl_CT n2) {
    const size_t n1Len = (n1->dataLen + 3U) >> 2U;
    const size_t n2Len = (n2->dataLen + 3U) >> 2U;

    for (size_t i = 0U; i < n1Len; ++i) {
        uint32_t *const dest = &ret->data[i], carry = 0U;
        const uint32_t num1 = n1->data[i];
        // if (num1 == 0U) continue;

        for (size_t j = 0U; j < n2Len; ++j) {
            uint64_t temp = (uint64_t)num1 * n2->data[j];
            temp += (uint64_t)dest[j] + carry;
            dest[j] = (uint32_t)(temp >> 0U);
            carry = (uint32_t)(temp >> 32U);
        }
        dest[n2Len] = carry;
    }
}
void BigInt_Mul(BigInt_T *retPtr, BigInt_CT in1, BigInt_CT in2) {
    BigInt_Impl_CT n1 = (BigInt_Impl_T)in1, n2 = (BigInt_Impl_T)in2;
    const size_t retSize = n1->dataLen + n2->dataLen + sizeof(n1->data[0]);

    BigInt_Impl_T ret = BigInt_Alloc(retSize);
    (void)memset(ret->data, 0, ret->bufferLen);

    BigInt_RawMul(ret, n1, n2);
    BigInt_ReduceToRealSize(ret, retSize);

    BigInt_Free((BigInt_Impl_T)*retPtr);
    *retPtr = ret;
}
