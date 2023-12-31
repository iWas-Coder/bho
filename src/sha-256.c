#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sha-256.h>


#define ROTATE_LEFT(a, b) ((a << b) | (a >> (32 - b)))
#define ROTATE_RIGHT(a, b) ((a >> b) | (a << (32 - b)))
#define CH(x, y, z) ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define EP0(x) (ROTATE_RIGHT(x, 2) ^ ROTATE_RIGHT(x, 13) ^ ROTATE_RIGHT(x, 22))
#define EP1(x) (ROTATE_RIGHT(x, 6) ^ ROTATE_RIGHT(x, 11) ^ ROTATE_RIGHT(x, 25))
#define SIG0(x) (ROTATE_RIGHT(x, 7) ^ ROTATE_RIGHT(x, 18) ^ (x >> 3))
#define SIG1(x) (ROTATE_RIGHT(x, 17) ^ ROTATE_RIGHT(x, 19) ^ (x >> 10))


static const unsigned int k[SHA256_CHUNK_SIZE] = {
  0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
  0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
  0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
  0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
  0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
  0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
  0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
  0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};


void sha256_init(sha256_ctx* ctx) {
  ctx->data_len = 0;
  ctx->bit_len  = 0;
  ctx->state[0] = 0x6a09e667;
  ctx->state[1] = 0xbb67ae85;
  ctx->state[2] = 0x3c6ef372;
  ctx->state[3] = 0xa54ff53a;
  ctx->state[4] = 0x510e527f;
  ctx->state[5] = 0x9b05688c;
  ctx->state[6] = 0x1f83d9ab;
  ctx->state[7] = 0x5be0cd19;
}


void sha256_transform(sha256_ctx *ctx, const unsigned char *data) {
  unsigned int a, b, c, d, e, f, g, h, i, j, t1, t2, m[SHA256_CHUNK_SIZE];

  // Data blocks preparation
  for (i = 0, j = 0; i < pow(SHA256_DIGEST_ALIGN, 2); ++i, j += SHA256_DIGEST_ALIGN) {
    m[i] = (data[j] << 24)     |
           (data[j + 1] << 16) |
           (data[j + 2] << 8)  |
           (data[j + 3]);
  }
  for (; i < SHA256_CHUNK_SIZE; ++i) {
    // PROF: 20% of time spends here
    m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];
  }

  // Saving context's state to local variables
  a = ctx->state[0];
  b = ctx->state[1];
  c = ctx->state[2];
  d = ctx->state[3];
  e = ctx->state[4];
  f = ctx->state[5];
  g = ctx->state[6];
  h = ctx->state[7];

  // Main transformation
  for (i = 0; i < SHA256_CHUNK_SIZE; ++i) {
    // Intermediate computations
    // PROF: 15% of time spends here
    t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
    t2 = EP0(a) + MAJ(a, b, c);
    // Update local variables
    h = g;
    g = f;
    f = e;
    e = d + t1;
    d = c;
    c = b;
    b = a;
    a = t1 + t2;
  }

  // Update context's state with computed values in local variables
  ctx->state[0] += a;
  ctx->state[1] += b;
  ctx->state[2] += c;
  ctx->state[3] += d;
  ctx->state[4] += e;
  ctx->state[5] += f;
  ctx->state[6] += g;
  ctx->state[7] += h;
}


void sha256_update(sha256_ctx *ctx, const unsigned char *data, size_t len) {
  unsigned int i;

  for (i = 0; i < len; ++i) {
    ctx->data[ctx->data_len] = data[i];
    ++(ctx->data_len);
    if (ctx->data_len == SHA256_CHUNK_SIZE) {
      sha256_transform(ctx, ctx->data);
      ctx->bit_len += 512;
      ctx->data_len = 0;
    }
  }
}


void sha256_out(sha256_ctx *ctx, unsigned char *hash) {
  unsigned int i;
  i = ctx->data_len;

  if (ctx->data_len < 56) {
    ctx->data[i++] = 0x80;
    while (i < 56) {
      ctx->data[i++] = 0x00;
    }
  }
  else {
    ctx->data[i++] = 0x80;
    while (i < SHA256_CHUNK_SIZE) {
      ctx->data[i++] = 0x00;
    }
    sha256_transform(ctx, ctx->data);
    memset(ctx->data, 0, 56);
  }

  ctx->bit_len += ctx->data_len * 8;
  ctx->data[63] = ctx->bit_len;
  ctx->data[62] = ctx->bit_len >> 8;
  ctx->data[61] = ctx->bit_len >> 16;
  ctx->data[60] = ctx->bit_len >> 24;
  ctx->data[59] = ctx->bit_len >> 32;
  ctx->data[58] = ctx->bit_len >> 40;
  ctx->data[57] = ctx->bit_len >> 48;
  ctx->data[56] = ctx->bit_len >> 56;
  sha256_transform(ctx, ctx->data);

  for (i = 0; i < SHA256_DIGEST_ALIGN; ++i) {
    hash[i]      = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 4]  = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 8]  = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
  }
}


void sha256_compute(unsigned char *string, unsigned char *hash) {
  sha256_ctx ctx;

  sha256_init(&ctx);
  sha256_update(&ctx, string, strlen((char*) string));
  sha256_out(&ctx, hash);
}


void sha256_file_compute(FILE *fd, unsigned char *hash) {
  sha256_ctx ctx;
  size_t bytes_read;
  unsigned char buf[FILE_COMPUTE_BUF_SIZE];

  sha256_init(&ctx);
  while ((bytes_read = fread(buf, 1, sizeof(buf), fd)) > 0) {
    sha256_update(&ctx, buf, bytes_read);
  }
  sha256_out(&ctx, hash);
}


void sha256_print(unsigned char *hash) {
  for (int i = 0; i < SHA256_DIGEST_SIZE; ++i) {
    printf("%02x", hash[i]);
  }
  printf("\n");
}
