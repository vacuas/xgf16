// SPDX-License-Identifier: MIT
// 2024 J.A. Leegwater, https://github.com/vacuas/xgf16

#include <stdio.h>
#include <openssl/evp.h>

#define XGF16_N 4
#include "xgf16.h"
typedef uint8_t gf16_bytes_t[XGF16_N2];

#define SEEDLEN 16
#define BYTESLEN 1000 // Should be enough

#define Nb 2
#define Nc 3

void crypto_random(const uint8_t *seed, size_t seedlen,
                   uint8_t *output, size_t outlen)
{
    EVP_MD_CTX *context;
    context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_shake256(), NULL);
    EVP_DigestUpdate(context, seed, seedlen);
    EVP_DigestFinalXOF(context, output, outlen);
    EVP_MD_CTX_free(context);
}

int main()
{

    // Get some random bytes

    uint8_t seed[SEEDLEN] = {0};
    size_t seedlen = SEEDLEN;
    uint8_t random[BYTESLEN] = {0};
    size_t randomlen = BYTESLEN;

    crypto_random(seed, seedlen, random, randomlen);

    gf16_bytes_t A = {0};
    gf16_bytes_t B[Nb] = {0};
    gf16_bytes_t C[Nb][Nc] = {0};
    gf16_bytes_t D[Nc] = {0};

    for (int xn = 0; xn < XGF16_N2; xn++)
    {
        for (int idx1 = 0; idx1 < Nb; idx1++)
        {
            B[idx1][xn] = random[idx1 * XGF16_N2 + xn + 20];
            // printf("%02X ", B[idx1][xn]);
        }
        // printf("\n");
        for (int idx1 = 0; idx1 < Nb; idx1++)
        {
            for (int idx2 = 0; idx2 < Nc; idx2++)
            {
                C[idx1][idx2][xn] = random[idx2 * XGF16_N2 + xn + 200 + 30 * idx1];
                // printf("%02X ", C[idx1][idx2][xn]);
            }
        }
        // printf("\n");
        for (int idx2 = 0; idx2 < Nc; idx2++)
        {
            D[idx2][xn] = random[idx2 * XGF16_N2 + xn + 500];
            // printf("%02X ", D[idx2][xn]);
        }
        // printf("\n");
    }

    // Actual example
    //
    // a = sum(b[i] * c[i][j] * d[j])

    xgf16_matrix_t xB[Nb], xC[Nb][Nc], xD[Nc];

    for (int idx1 = 0; idx1 < Nb; idx1++)
    {
        xgf16_expand(xB[idx1], B[idx1]);
    }
    for (int idx1 = 0; idx1 < Nb; idx1++)
    {
        for (int idx2 = 0; idx2 < Nc; idx2++)
        {
            xgf16_expand(xC[idx1][idx2], C[idx1][idx2]);
        }
    }
    for (int idx2 = 0; idx2 < Nc; idx2++)
    {
        xgf16_expand(xD[idx2], D[idx2]);
    }

    xgf16_matrix_t xA = {0};
    for (int idx1 = 0; idx1 < Nb; idx1++)
    {
        xgf16_matrix_t sum = {0};
        for (int idx2 = 0; idx2 < Nc; idx2++)
        {
            xgf16_affine_mul(sum, xC[idx1][idx2], xD[idx2]);
        }
        xgf16_filter(sum);
        xgf16_affine_mul(xA, xB[idx1], sum);
    }
    xgf16_compress(A, xA);

    printf("Result:\n");
    for (int idx = 0; idx < XGF16_N2; idx++)
    {
        printf("%02X ", A[idx]);
    }
    printf("\n");
}
