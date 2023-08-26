#ifndef SHA256_H_
#define SHA256_H_


#include <stddef.h>
#include <stdio.h>


#define SHA256_CHUNK_SIZE 64
#define SHA256_DIGEST_SIZE 32
#define SHA256_DIGEST_ALIGN 4
#define FILE_COMPUTE_BUF_SIZE 1024


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
void sha256_file_compute(FILE *fd, unsigned char *hash);
void sha256_print(unsigned char *hash);


#endif  // SHA256_H_
