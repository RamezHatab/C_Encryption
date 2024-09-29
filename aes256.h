#include <stddef.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    uint8_t iv[16];
} iv_blk;

typedef struct {
    uint8_t key[32];
    uint8_t ek[32];
    iv_blk blk;
} aes_ctx;


void aes_init(aes_ctx *ctx, uint8_t *key);
void encrypt(aes_ctx *ctx, uint8_t *buf);

void init_ctr(aes_ctx *ctx, iv_blk *blk);
void ctr_encrypt(aes_ctx *ctx, uint8_t *buf, size_t sz);
