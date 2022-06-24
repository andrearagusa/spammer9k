/*
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain
*/

#include <stdint.h>

#pragma once

#define SHA1_DIGEST_LENGTH 20

typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    unsigned char buffer[64];
} SHA1_CTX;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
void SHA1Transform(uint32_t state[5], const unsigned char buffer[64]);
void SHA1Init(SHA1_CTX* context);
void SHA1Update(SHA1_CTX* context, const unsigned char* data, uint32_t len);
void SHA1Final(SHA1_CTX* context, unsigned char digest[20]);
#ifdef __cplusplus
}
#endif // __cplusplus
