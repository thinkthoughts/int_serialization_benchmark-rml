import math


# Compute m and c such that:# division(n, d) = division(c ∗n, m) and remainder(n, d) = division(remainder(c ∗n, m) ∗d, m)
# for all n ∈[0, N ]
# condition: 1/d ≤c/m < (1 + 1/N)/d
# 1 <= c * d /m < (1 + 1/N)
# m <= c * d < m + m //N
# See Daniel Lemire, Colin Bartlett, Owen Kaser,
# [Integer Division by Constants: Optimal Bounds](https://arxiv.org/abs/2012.12369),  Heliyon 7 (6), 2021
def find_m_c(N, d, m=1, cmax=2**64):
    while True:
        c = m // d + (1 if m % d != 0 else 0)
        if c >= cmax:
            print(f"Could not find m and c for N={N}, d={d} within cmax={cmax}")
            break
        if c * d < m + (m // (N - (N + 1) % d)):
            print(f"m: {m}, c: {c}")
            break
        m *= 2

    print(f"Final m: 2**{math.log(m) / math.log(2)}, c: {c}")
    return m, c


def mul64(x, c):
    product = c * x
    high = product >> 64
    low = product % (1 << 64)
    return high, low


def push_repeat(mantissa, repeat):
    digits = []
    for _ in range(repeat):
        print(f"mantissa: {mantissa}")
        hundreds, mantissa = mul64(mantissa, 100)
        print(f"hundreds: {hundreds} mantissa: {mantissa}")
        digits.append(hundreds)
    print(f"final digits: {digits}")
    return digits


def recompose(x):
    multcont = 0
    digits = []
    high, low = mul64(x, 118059162071741131)
    multcont += 1
    xdiv10_4 = high >> 6
    print(f"xdiv10_4: {xdiv10_4}")
    digits.append(x - xdiv10_4 * 10000)
    multcont += 2
    xdiv10_8, low = mul64(xdiv10_4, 1844674407370956)
    multcont += 1
    digits.append(push_repeat(low, 2))
    multcont += 2
    xdiv10_16, low = mul64(xdiv10_8, 184467440738)
    multcont += 1
    digits.append(push_repeat(low, 4))
    multcont += 4
    digits.append(xdiv10_16)
    print(f"Total multiplications: {multcont}")
    return digits


x = 92345678901234567
print(recompose(x))

find_m_c(10**17 - 1, 10**8)
find_m_c(10**9 - 1, 10**8, 2**64)

print("----")

find_m_c(10**16 - 1, 10**8)
find_m_c(10**8 - 1, 10**6, 2**64)

print("----")

find_m_c(10**14 - 1, 10**8)
find_m_c(10**6 - 1, 10**4, 2**64)

print("----")

find_m_c(10**13 - 1, 10**8)
find_m_c(10**5 - 1, 10**4, 2**64)

print("----12")
find_m_c(10**12 - 1, 10**6, 2**64)
find_m_c(10**6 - 1, 10**4, 2**64)

print("----")
find_m_c(10**11 - 1, 10**6, 2**64)
find_m_c(10**5 - 1, 10**4, 2**64)

print("----")
find_m_c(10**10 - 1, 10**8, 2**64)

print("----")
find_m_c(10**8 - 1, 10**6, 2**64)

print("----")
find_m_c(10**6 - 1, 10**4, 2**64)
