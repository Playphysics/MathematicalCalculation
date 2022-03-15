from time import time

def PrtHex(N, endStr='\n'):
    prtStr = hex(N)
    prtLen = len(prtStr)

    print('%6d' % ((prtLen - 1) // 2), end=' ')
    if prtLen > 36:
        print(prtStr[:18], '... ', prtStr[-16:], end=endStr)
    else: print(prtStr, end=endStr)

def Fib(N):
    a, b = 1, 1
    for _ in range(N - 1):
        a, b = b, a + b
    return a

def PrtFibTime(N):
    cycleNum = 100000 // N
    t0 = time()
    for _ in range(cycleNum): ret = Fib(N)
    dt = time() - t0;
    PrtHex(ret, '  ')
    print(dt / cycleNum * 1000)
    

a = 0x1234567887654321
b = int(\
    "0x1234567812345678123456781234567812345678123456781234567812345678"
    "111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEE0000FFFF"
    "1234567812345678123456781234567812345678123456781234567812345678"
    "111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEE0000FFFF"
    "1234567812345678123456781234567812345678123456781234567812345678"
    "111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEE0000FFFF"
    "1234567812345678123456781234567812345678123456781234567812345678"
    "111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEE0000FFFF"
    , 16)

def PrtAddTime(N):
    cycleNum = 100000 // N + int(N > 100000)
    t0 = time()
    for _ in range(cycleNum):
        c = 0
        for i in range(N): c += a; c += b; c += c

    PrtHex(c, '  ')
    print((time() - t0) / cycleNum)

def PrtMulTime(N):
    cycleNum = 1000 // N + int(N > 1000)
    t0 = time()
    for _ in range(cycleNum):
        c = 1
        for i in range(N): c *= a; c *= b

    PrtHex(c, '  ')
    print((time() - t0) / cycleNum)

for i in range(1, 6):
    PrtHex(Fib(i + 300))
print("==================================\n");

for i in [10000, 30000, 100000, 150151, 200000]:
    PrtAddTime(i)
print("==================================\n");

for i in [100, 300, 700, 1000]:
    PrtMulTime(i)
print("==================================\n");

for i in [1000, 10000, 50000, 100000]:
    PrtFibTime(i)
