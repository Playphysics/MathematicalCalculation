# myBigInt
C语言动态内存分配练习

大整数运算(无符号)，大整数矩阵运算，Fibonacci 数列的 O(log(N)) 算法

## 内存结构：头部 + 柔性数组
* BigInt
包含两个size_t成员（分别记录已分配空间大小、已使用大小），以及一个不定长柔性数组（记录数据，以 uin32_t 为单位）

* Matrix
包含两个size_t成员（分别记录矩阵的行列大小），以及一个不定长柔性数组（记录若干个 BigInt 元素）

## 接口：void *
* BigInt、Matrix的接口类型均为 void \*，不对外开放内部结构

## 算法：朴素算法
* 简介
模拟手工列竖式计算，使用2^32进制；预先分配足够内存，计算完成后再修正实际使用量；字符串转换参考Python，大数相加参考GMP

* BigInt_Add
各个数位依次相加；对于每一次运算，若结果小于其中一个数，则说明有进位，c = a + b, carry = c < a；此方法比转化为 uint64_t 运算更快

* BigInt_Sub
补码运算（取反加一），被减数 + 减数取反 即可，初始进位为1作为“取反加一”的“一”

* BigInt_Mul
模拟手工计算，双重循环依次相乘；过程中转化为uint64_t运算，结果拆分为两个uint32_t数据，分别对应累加到结果即可

* Mat_Add
对应相加，c[i] = a[i] + b[i]

* Mat_Mul
三重循环直接计算，c[i][j] = a[i][k] + b[k][j]

* 斐波那契数列
    1. 朴素算法：a, b = b, a + b
    2. 矩阵运算：使用快速幂算法，计算矩阵的幂，F = M ^ n * E  
    F = [f(n+2), f(n+1)], M = [1, 1; 1, 1], E = [1, 1]

## 基本使用方式

1. 大整数运算
```
    BigInt_T a = BigInt_New(0, 0); // 分配内存并初始化为0
    BigInt_T b = BigInt_New(0x1234, 0); // 分配内存并初始化为0x1234
    // 分配内存并初始化为0x123456789ABCDEF123456789ABCDEF
    // 当第二个参数不为空指针时，将其作为16进制字符串
    BigInt_T c = BigInt_New(0, "0x123456789ABCDEF123456789ABCDEF");

    BigInt_Add(&c, a, b); // c = a + b
    BigInt_Add(&c, c, b); // c += b

    BigInt_Mul(&c, a, b); // c = a * b
    BigInt_Mul(&c, c, b); // c *= b

    // 输出c，最多显示6个uint32_t的16进制数据（只显示首尾，中间使用...代替），输出完成后换行
    BigInt_Prt(c, 6, "\n"); 

    BigInt_Delete(a);
    BigInt_Delete(b);
    BigInt_Delete(c);
```
2. 矩阵运算
```
    Matrix_T a = Mat_New(2, 2, 0); // 分配2*2矩阵的内存并初始化为0
    // 分配2*2矩阵的内存并分别初始化为{1, 2; 3, 4}
    Matrix_T b = Mat_New(2, 2, (const uint64_t []){1, 2, 3, 4});
    // 目前不支持初始化超出uint64_t范围的值
    Matrix_T c = Mat_New(2, 2, (const uint64_t []){0x1234, 2, 3, 4});

    // 与大整数运算不同，此处不使用 &c
    Mat_Add(c, a, b); // c = a + b
    Mat_Add(c, c, b); // c += b

    Mat_Mul(c, a, b); // c = a * b
    Mat_Mul(c, c, b); // c *= b

    // 获取第1行第0列的值，行列从0开始计数
    BigInt_T temp = Mat_GetData(c, 1, 0);

    Mat_Delete(a);
    Mat_Delete(b);
    Mat_Delete(c);
```
