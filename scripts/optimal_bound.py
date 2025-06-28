import math


# Compute m and c such that:# division(n, d) = division(c ∗n, m) and remainder(n, d) = division(remainder(c ∗n, m) ∗d, m) 
# for all n ∈[0, N ]
# condition: 1/d ≤c/m < (1 + 1/N)/d
# 1 <= c * d /m < (1 + 1/N) 
# m <= c * d < m + m //N
# See Daniel Lemire, Colin Bartlett, Owen Kaser,  
# [Integer Division by Constants: Optimal Bounds](https://arxiv.org/abs/2012.12369),  Heliyon 7 (6), 2021
def find_m_c(N, d, m = 1)
    while True:
        c = m//d + (1 if m % d != 0 else 0)
        if c * d < m + (m // (N-(N+1) % d)):
            print(f"m: {m}, c: {c}")
            break
        m *= 2

    print(f"Final m: 2**{math.log(m)/math.log(2)}, c: {c}")
    return m, c

find_m_c(10**15 - 1, 10**4, 2**64)
