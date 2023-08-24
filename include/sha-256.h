#ifndef SHA256_H_
#define SHA256_H_


#include <stddef.h>


#define SHA256_BLOCK_SIZE 32


typedef struct {
  unsigned char data[64];
  unsigned int data_len;
  unsigned long long bit_len;
  unsigned int state[8];
} sha256_ctx;


void sha256_init(sha256_ctx *ctx);
void sha256_transform(sha256_ctx *ctx, const unsigned char *data);
void sha256_update(sha256_ctx *ctx, const unsigned char *data, size_t len);
void sha256_out(sha256_ctx *ctx, unsigned char *hash);
void sha256_compute(unsigned char *string, unsigned char *hash);


#endif  // SHA256_H_
