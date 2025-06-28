# float_string_generation_benchmark

The goal of this project is to narrowly study the problem of generating
*shortest* number strings (e.g., `1.2122E4`) out of a decimal representation
(`significant * 10**power`). The implicit assumption is that you started
out from a binary floating point numbers (`float` or `double` in C/C++/Java/C#)
and you mapped it to a decimal form.

The project might have applications to more conventional problems such as converting
integer values to decimal representation.


In principle, converting `significant * 10**power` into a decimal string is not difficult.
You can compute `significant % 10` and get the last significant digits, and so forth.
This is the typically right-to-left approach: you write the least significant digit,
and the next least significant digit, and so forht.
You need to determine which way you go to the shortest string (do you write `0.1` or `1E-1`),
but that's not too difficult.

So why is this interesting? It is interesting because we noticed that a highly optimized
function in a Ryu float-to-string implementation was much slower then the implementation
in the  Dragonbox float-to-string implementation.

What is challenging? 

## Knowing where to write

One challenge is that you want to write the characters at the right place
from the start. This is not trivial because you don't known initially how many digits you 
need to write. If you consider the  right-to-left approach, it requires you to start 
writing *somewhere* implying that you know how many digits you have. Thankfully, there
are fast algorithms to count digits:

- Daniel Lemire, "Counting the digits of 64-bit integers," in Daniel Lemire's blog, January 7, 2025, https://lemire.me/blog/2025/01/07/counting-the-digits-of-64-bit-integers/.

Of course, you could write to a buffer and then copy over but that's likely more expensive than
counting the number of digits, at least in some cases.

The Dragonbox float-to-string implementation avoids this problem. The way it does it is that
it writes from left to right. So it writes the most significant digit first !!! It relies
on branching and assumes that the number of digits might be somewhat predictible, which could be true
in practice (or not).

## Storing the characters

Even if you have the digits (e.g., the integer 1) and you know where they should be written, you
still need to do something like:

```c++
buf[index] = '0' + value
```

And, once you figured out where the dot goes, you need to do

```c++
buf[index] = '.'
```

These things add up. So one trick is to compute hundreds instead of tens. And you use a lookup table.
So you have precompted strings from `00`, `01`, `02`, up `99`.
For the dot, you can also avoid having separate store by precomputing the strings
`0.`, `1.`, `2.`,... or something equivalent.

Similarly, for the exponent, you could precompute strings. E.g., you could certainly precompute `e+` and `e-` and
not have two stores.

There might be room for fancier strategies. See

-  Daniel Lemire, "Converting integers to fix-digit representations quickly," in Daniel Lemire's blog, November 18, 2021, https://lemire.me/blog/2021/11/18/converting-integers-to-fix-digit-representations-quickly/.

It is a research question whether AVX-512 can solve this problem. It might. The reason
why AVX-512 might do it is that it supports *masked* stores. So you can safely store
a SIMD register to memory, writing only part of it.
It might also be possible to use SIMD in general if you allow writing beyond the expected
buffer. This might be acceptable in many settings where you can assume that the memory
is overallocated.


## Computing the digit values


This is where the fun mathematics comes in. I give you an integer, how do you compute
quickly the remainder and the quotient? See the following paper.

- Daniel Lemire, Colin Bartlett, Owen Kaser,  [Integer Division by Constants: Optimal Bounds](https://arxiv.org/abs/2012.12369),  Heliyon 7 (6), 2021

Though the math is a bit tricky, we can often brute force a check for the solution.

## Usage

```
cmake -B build
./build/benchmark
```

