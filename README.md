# xgf16

Utility to speedup affine multiplications over Mat(GF16).

The prime use case is an operation like
```
  a = sum_ij(b[i] * c[i][j] * d[j])
```
Where a, b, c, d are all square GF16 matrices.

The basic idea is to use uint32_t multiplication as much as possible as this is fast. 
In the first step the 4 bit representation is expanded to a 12 bit representation, 
then multiply and compress back to 4 bits at the end.

For two consecutive multiplications an additional optimization is possible. As bit 10 is 
the highest bit set by the expansion, two consecutive multiplication will use a maximum 
of 31 bits so a 32 bit multiplication is possible. The compression can be delayed after 
all multiplications have been done. This increases the speed of the matrix multiplication. 
This optimization does however requires filtering of the spurious bits introduced by the 
regular multiplication.

Testing
-------
See example.c for an example. 

```
gcc example.c -o example -lcrypto
```
Note: The example uses the OpenSSL library for Shake

TODO
-------
AVX2 optimization.
