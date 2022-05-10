#include "myBigInt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

struct BigInt_Impl_T {
    size_t bufferLen;
    size_t dataLen;
    uint32_t data[];
};

// === Alloc/Free ====== Alloc/Free ====== Alloc/Free ====== Alloc/Free ===
// === Alloc/Free ====== Alloc/Free ====== Alloc/Free ====== Alloc/Free ===
// === Alloc/Free ====== Alloc/Free ====== Alloc/Free ====== Alloc/Free ===

volatile size_t BigInt_mallocCount, BigInt_freeCount;

static BigInt_T BigInt_Alloc(size_t dataLen) {
    const size_t mask = 0x0FU;
    const size_t extraSize = 2U * sizeof(size_t);
    const size_t bufLen = (dataLen + extraSize + mask) & ~mask;
    BigInt_T retPtr = 0;

    if (bufLen > dataLen) {
        BigInt_mallocCount += 1U;
        retPtr = (BigInt_T)malloc(bufLen);
        retPtr->bufferLen = bufLen - extraSize;
        retPtr->dataLen = 0U;
    }
    return retPtr;
}
static void BigInt_Free(BigInt_T ptr) {
    BigInt_freeCount += 1U;
    free((void *)ptr);
}

// === convert from/to str ====== convert from/to str ====== convert from/to str ===
// === convert from/to str ====== convert from/to str ====== convert from/to str ===
// === convert from/to str ====== convert from/to str ====== convert from/to str ===

void BigInt_Prt(BigInt_CT ret, size_t maxWidth, const char *end) {
    size_t u32Size = (ret->dataLen + 3U) >> 2U;

    printf("%8u: 0x", (unsigned int)ret->dataLen);

    if (u32Size == 0U) {
        printf("%08X", 0U);
    } else if (u32Size > maxWidth && maxWidth >= 2U) {
        for (size_t i = 0U; i < maxWidth / 2U - 1U; ++i) {
            printf("%08X ", ret->data[--u32Size]);
        }
        printf("%08X ...", ret->data[--u32Size]);
        u32Size = maxWidth / 2U;
    } else {
        printf("%08X", ret->data[--u32Size]);
    }
    while (u32Size != 0U) {
        printf(" %08X", ret->data[--u32Size]);
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

static void BigInt_ReduceToRealSize(BigInt_T ret, size_t curSize) {
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
    BigInt_T ret;
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
    BigInt_Free(ret);
}

void BigInt_Reset(BigInt_T *retPtr, uint64_t num, const char *hexStr) {
    BigInt_Free(*retPtr);
    *retPtr = BigInt_New(num, hexStr);
}

void BigInt_Swap(BigInt_T *in1, BigInt_T *in2) {
    BigInt_T temp = *in1;
    *in1 = *in2, *in2 = temp;
}
void BigInt_CSwap(BigInt_CT *in1, BigInt_CT *in2) {
    BigInt_CT temp = *in1;
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

static void BigInt_RawAdd(BigInt_T ret, BigInt_CT large, BigInt_CT small) {
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
    BigInt_T ret = *retPtr;
    size_t retSize;

    if (in1->dataLen < in2->dataLen) BigInt_CSwap(&in1, &in2);

    retSize = in1->dataLen + sizeof(in1->data[0]);
    if (ret->bufferLen < retSize) ret = BigInt_Alloc(retSize);

    BigInt_RawAdd(ret, in1, in2);
    BigInt_ReduceToRealSize(ret, in1->dataLen + 1U);

    if (ret != *retPtr) {
        BigInt_Free(*retPtr);
        *retPtr = ret;
    }
}

static void BigInt_RawSub(BigInt_T ret, BigInt_CT in1, BigInt_CT in2) {
    const size_t in1Len = (in1->dataLen + 3U) >> 2U;
    const size_t in2Len = (in2->dataLen + 3U) >> 2U;
    size_t i, smallSize = (in1Len < in2Len) ? in1Len : in2Len;
    uint32_t carry = 1U;

    for (i = 0U; i < smallSize; ++i) {
        carry = Uint32Add(&ret->data[i], in1->data[i], ~in2->data[i], carry);
    }
    for (i = smallSize; i < in1Len; ++i) {
        carry = Uint32Add(&ret->data[i], in1->data[i], 0xFFFFFFFFU, carry);
    }
    for (i = smallSize; i < in2Len; ++i) {
        carry = Uint32Add(&ret->data[i], 0U, ~in2->data[i], carry);
    }
}

void BigInt_Sub(BigInt_T *retPtr, BigInt_CT in1, BigInt_CT in2) {
    BigInt_T ret = *retPtr;
    size_t retSize = in1->dataLen;

    if (in1->dataLen < in2->dataLen) retSize = in2->dataLen;
    if (ret->bufferLen < retSize) ret = BigInt_Alloc(retSize);

    BigInt_RawSub(ret, in1, in2);
    BigInt_ReduceToRealSize(ret, retSize);

    if (ret != *retPtr) {
        BigInt_Free(*retPtr);
        *retPtr = ret;
    }
}

static void BigInt_RawMul(BigInt_T ret, BigInt_CT n1, BigInt_CT n2) {
    const size_t n1Len = (n1->dataLen + 3U) >> 2U;
    const size_t n2Len = (n2->dataLen + 3U) >> 2U;

    for (size_t i = 0U; i < n1Len; ++i) {
        uint32_t *const dest = &ret->data[i], carry = 0U;
        const uint32_t num1 = n1->data[i];
        if (num1 == 0U) continue;

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
    const size_t retSize = in1->dataLen + in2->dataLen + sizeof(in1->data[0]);

    BigInt_T ret = BigInt_Alloc(retSize);
    (void)memset(ret->data, 0, ret->bufferLen);

    BigInt_RawMul(ret, in1, in2);
    BigInt_ReduceToRealSize(ret, retSize);

    BigInt_Free(*retPtr);
    *retPtr = ret;
}

int BigInt_Cmp(BigInt_CT in1, BigInt_CT in2) {
    if (in1->dataLen > in2->dataLen) return 1;
    if (in1->dataLen < in2->dataLen) return -1;

    size_t u32Size = (in1->dataLen + 3U) >> 2U;
    while (u32Size-- > 0U) {
        if (in1->data[u32Size] > in2->data[u32Size]) return 1;
        if (in1->data[u32Size] < in2->data[u32Size]) return -1;
    }
    return 0;
}

// === Test ====== Test ====== Test ====== Test ====== Test ===
// === Test ====== Test ====== Test ====== Test ====== Test ===
// === Test ====== Test ====== Test ====== Test ====== Test ===

#define BigInt_TestOne(func, in1, in2, expectStr)                     \
    func(&num, in1, in2);                                             \
    BigInt_Reset(&expect, 0U, expectStr);                             \
    if (BigInt_Cmp(num, expect) != 0) {                               \
        printf("\n=== TEST ERROR ====== TEST ERROR ===\nline: ");     \
        printf("%d %s with (%s, %s)\n", __LINE__, #func, #in1, #in2); \
        printf("expect:\n%s\nThe result:\n", expectStr);              \
        BigInt_Prt(num, 10U, "\n\n"), exit(-1);                       \
    }

static void BigInt_TestAdd(BigInt_CT zero, BigInt_CT one, BigInt_CT FF_U64, BigInt_CT F0_8) {
    BigInt_T num = BigInt_New(0U, 0);
    BigInt_T expect = BigInt_New(0U, 0);

    BigInt_TestOne(BigInt_Add, zero, zero, "0x0");
    BigInt_TestOne(BigInt_Add, one, one, "0x2");
    BigInt_TestOne(BigInt_Add, FF_U64, FF_U64, "0x1fffffffffffffffe");
    BigInt_TestOne(BigInt_Add, F0_8, F0_8,
                   "0x1fdb974130eca8643fdb974130eca8643fdb974130eca8643fdb974130eca8643fdb974130eca"
                   "8643fdb974130eca8643fdb974130eca8643fdb974130eca8642");

    BigInt_TestOne(BigInt_Add, zero, one, "0x1");
    BigInt_TestOne(BigInt_Add, one, zero, "0x1");
    BigInt_TestOne(BigInt_Add, FF_U64, F0_8,
                   "0xfedcba0987654321fedcba0987654321fedcba0987654321fedcba0987654321fedcba0987654"
                   "321fedcba0987654321fedcba0987654322fedcba0987654320");
    BigInt_TestOne(BigInt_Add, F0_8, FF_U64,
                   "0xfedcba0987654321fedcba0987654321fedcba0987654321fedcba0987654321fedcba0987654"
                   "321fedcba0987654321fedcba0987654322fedcba0987654320");

    BigInt_Delete(expect);
    BigInt_Delete(num);

    printf("%s passed all the tests\n", __func__);
}
static void BigInt_TestSub(BigInt_CT zero, BigInt_CT one, BigInt_CT FF_U64, BigInt_CT F0_8) {
    BigInt_T num = BigInt_New(0U, 0);
    BigInt_T expect = BigInt_New(0U, 0);

    BigInt_TestOne(BigInt_Sub, zero, zero, "0x0");
    BigInt_TestOne(BigInt_Sub, one, one, "0x0");
    BigInt_TestOne(BigInt_Sub, FF_U64, FF_U64, "0x0");
    BigInt_TestOne(BigInt_Sub, F0_8, F0_8, "0x0");

    BigInt_TestOne(BigInt_Sub, zero, one, "0xffffffff");
    BigInt_TestOne(BigInt_Sub, one, zero, "0x1");
    BigInt_TestOne(BigInt_Sub, FF_U64, F0_8,
                   "0x12345f6789abcde012345f6789abcde012345f6789abcde012345f6789abcde012345f6789abc"
                   "de012345f6789abcde012345f6789abcdf012345f6789abcde");
    BigInt_TestOne(BigInt_Sub, F0_8, FF_U64,
                   "0xfedcba0987654321fedcba0987654321fedcba0987654321fedcba0987654321fedcba0987654"
                   "321fedcba0987654321fedcba0987654320fedcba0987654322");

    BigInt_Delete(expect);
    BigInt_Delete(num);

    printf("%s passed all the tests\n", __func__);
}
static void BigInt_TestMul(BigInt_CT zero, BigInt_CT one, BigInt_CT FF_U64, BigInt_CT F0_8) {
    BigInt_T num = BigInt_New(0U, 0);
    BigInt_T expect = BigInt_New(0U, 0);

    BigInt_TestOne(BigInt_Mul, zero, zero, "0x0");
    BigInt_TestOne(BigInt_Mul, one, one, "0x1");
    BigInt_TestOne(BigInt_Mul, FF_U64, FF_U64, "0xfffffffffffffffe0000000000000001");
    BigInt_TestOne(
        BigInt_Mul, F0_8, F0_8,
        "0xfdbabf7b303f805cca74202338234af9972d80cb4007159663e6e17347eae03330a0421b4fceaacffd59a2c3"
        "57b2756cca13036b5f96400996cc6413677a0aa4681045c50eded4899b56e51d06fb09ecce9d8474ff173f5001"
        "e423ccf73374b3352ac324ef4faa166871627ce76bdf799bb801d4df8814dccefea12cd7a44a41");

    BigInt_TestOne(BigInt_Mul, zero, one, "0x0");
    BigInt_TestOne(BigInt_Mul, one, zero, "0x0");
    BigInt_TestOne(BigInt_Mul, FF_U64, F0_8,
                   "0xfedcba0987654320fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                   "fffffffffffffffffffffffffffffffffffffffffffffffffff012345f6789abcdf");
    BigInt_TestOne(BigInt_Mul, F0_8, FF_U64,
                   "0xfedcba0987654320fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                   "fffffffffffffffffffffffffffffffffffffffffffffffffff012345f6789abcdf");

    BigInt_Delete(expect);
    BigInt_Delete(num);

    printf("%s passed all the tests\n", __func__);
}
static void BigInt_TestCmp(BigInt_CT zero, BigInt_CT one, BigInt_CT FF_U64, BigInt_CT F0_8) {
    BigInt_T num = BigInt_New(0U, 0);

    assert(BigInt_Cmp(zero, zero) == 0);
    assert(BigInt_Cmp(one, one) == 0);
    assert(BigInt_Cmp(FF_U64, FF_U64) == 0);
    assert(BigInt_Cmp(F0_8, F0_8) == 0);

    assert(BigInt_Cmp(zero, one) != 0);
    assert(BigInt_Cmp(zero, FF_U64) != 0);
    assert(BigInt_Cmp(zero, F0_8) != 0);
    assert(BigInt_Cmp(one, FF_U64) != 0);
    assert(BigInt_Cmp(one, F0_8) != 0);
    assert(BigInt_Cmp(FF_U64, F0_8) != 0);

    BigInt_Delete(num);
    printf("%s passed all the tests\n", __func__);
}

void BigInt_Test(void) {
    BigInt_T zero = BigInt_New(0U, 0);
    BigInt_T one = BigInt_New(1U, 0);
    BigInt_T FF = BigInt_New(UINT64_MAX, 0);

    const char *const ptr =
        "0xFEDCBA0987654321FEDCBA0987654321FEDCBA0987654321FEDCBA0987654321F"
        "EDCBA0987654321FEDCBA0987654321FEDCBA0987654321FEDCBA0987654321";
    BigInt_T F0_8 = BigInt_New(0U, ptr);

    BigInt_TestCmp(zero, one, FF, F0_8);
    BigInt_TestAdd(zero, one, FF, F0_8);
    BigInt_TestSub(zero, one, FF, F0_8);
    BigInt_TestMul(zero, one, FF, F0_8);

    BigInt_Delete(zero);
    BigInt_Delete(one);
    BigInt_Delete(FF);
    BigInt_Delete(F0_8);
}
