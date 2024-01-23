// SPDX-License-Identifier: MIT
// 2024 J.A. Leegwater, https://github.com/vacuas/xgf16

#include <stdint.h>
#ifndef XGF16_H
#define XGF16_H

#ifndef XGF16_N
#define XGF16_N 4
#endif

#define XGF16_N2 (XGF16_N * XGF16_N)

typedef uint32_t xgf16_matrix_t[XGF16_N2];

/**
 * GF[x]/(x^4+x+1) reduction
 */
static inline uint32_t xgf16_reduce(uint32_t idx)
{
    uint32_t res, upper;

    res = idx & 0x49249249; // Octal 0o11111111111
    upper = idx >> 12;
    res = res ^ upper ^ (upper << 3);
    upper = res >> 12;
    res = res ^ upper ^ (upper << 3);
    upper = res >> 12;
    res = res ^ upper ^ (upper << 3);

    return res & 0x249;
}

/**
 * Conversion 4 bit -> 32 bit representation
 */
static inline uint32_t xgf16_from_byte(uint8_t idx)
{
    uint32_t middle = idx | idx << 4;
    return (middle & 0x41) | ((middle << 2) & 0x208);
}

/**
 * Conversion 32 bit -> 4 bit representation
 */
static inline uint8_t xgf16_to_byte(uint32_t idx)
{
    uint32_t res = xgf16_reduce(idx);
    res = res | (res >> 4);
    return (res & 0x5) | ((res >> 2) & 0xa);
}

/**
 * Matrix conversion 4 bit -> 32 bit representation
 */
static inline void xgf16_expand(xgf16_matrix_t dest, const uint8_t *source)
{
    for (size_t idx = 0; idx < XGF16_N2; idx++)
    {
        dest[idx] = xgf16_from_byte(source[idx]);
    }
}

/**
 * Matrix conversion 32 bit -> 4 bit representation
 */
static inline void xgf16_compress(uint8_t *dest, const xgf16_matrix_t source)
{
    for (size_t idx = 0; idx < XGF16_N2; idx++)
    {
        dest[idx] = xgf16_to_byte(source[idx]);
    }
}

/**
 * Removes stray bits intruced by using regular mutiplication instead of GF16 multiplication
 */
static inline void xgf16_filter(xgf16_matrix_t a)
{
    for (int i = 0; i < XGF16_N2; ++i)
    {
        a[i] = a[i] & 0x49249249;
    }
}

/**
 * Multiplication of GF16 Matrices: a += b * c
 */
static inline void xgf16_mul(xgf16_matrix_t a, const xgf16_matrix_t b, const xgf16_matrix_t c)
{
    for (int i = 0; i < XGF16_N; ++i)
    {
        for (int j = 0; j < XGF16_N; ++j)
        {
            uint32_t sum = 0;
            for (int k = 0; k < XGF16_N; ++k)
            {
                sum ^= b[(i * XGF16_N) + k] * c[(k * XGF16_N) + j];
            }
            a[(i * XGF16_N) + j] = sum;
        }
    }
}

/**
 * Affine multiplication of GF16 Matrices: a += b * c
 */
static inline void xgf16_affine_mul(xgf16_matrix_t a, const xgf16_matrix_t b, const xgf16_matrix_t c)
{
    for (int i = 0; i < XGF16_N; ++i)
    {
        for (int j = 0; j < XGF16_N; ++j)
        {
            for (int k = 0; k < XGF16_N; ++k)
            {
                a[(i * XGF16_N) + j] ^= b[(i * XGF16_N) + k] * c[(k * XGF16_N) + j];
            }
        }
    }
}

#endif
