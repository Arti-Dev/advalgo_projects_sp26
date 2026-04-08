# Assume test case format where first number is the number of test cases, and in each test case there is the
# precision and number of stream elements, and then the stream elements themselves.
# Output for each test case is the estimate of the distinct elements in the stream according to HyperLogLog

import hashlib
import math
import sys


class HyperLogLog:
    def __init__(self, p):
        self.p = p
        self.m = 1 << p
        self.reg = [0] * self.m

    def add(self, value):
        x = int.from_bytes(hashlib.sha1(str(value).encode()).digest()[:8], "big")
        j = x >> (64 - self.p)
        wbits = 64 - self.p
        w = x & ((1 << wbits) - 1)
        if w == 0:
            r = wbits + 1
        else:
            r = (wbits - w.bit_length()) + 1
        if r > self.reg[j]:
            self.reg[j] = r

    def estimate(self):
        m = self.m
        if m == 16:
            a = 0.673
        elif m == 32:
            a = 0.697
        elif m == 64:
            a = 0.709
        else:
            a = 0.7213 / (1 + 1.079 / m)

        z = sum(2.0 ** (-r) for r in self.reg)
        e = a * m * m / z

        v = self.reg.count(0)
        if e <= 2.5 * m and v > 0:
            return m * math.log(m / v)

        two64 = float(1 << 64)
        if e > two64 / 30.0:
            return -two64 * math.log(1.0 - e / two64)

        return e


def main():
    if len(sys.argv) > 1:
        t = open(sys.argv[1], "r", encoding="utf-8").read().split()
    else:
        t = sys.stdin.read().split()
    if not t:
        return

    i = 0
    tc = int(t[i])
    i += 1

    out = []
    for _ in range(tc):
        p = int(t[i])
        n = int(t[i + 1])
        i += 2

        h = HyperLogLog(p)
        for _ in range(n):
            h.add(t[i])
            i += 1

        out.append(str(int(round(h.estimate()))))

    sys.stdout.write("\n".join(out))


if __name__ == "__main__":
    main()
