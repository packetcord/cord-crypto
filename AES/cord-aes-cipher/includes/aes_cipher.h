//
// aes_cipher.h
//

#include <stdio.h>
#include <stdint.h>
#include <string.h>

//
// Key length
//
#define AES_KEY_LEN_CONF 128

//
// Hardware acceleration
//
#define X86_64_AESNI_ACCEL
// #define ARM_NEON_AES_ACCEL

//
// Pre-scheduled key
//
// #define PRE_SCHEDULED_KEY

//
// Column-major or row-major mode (representation)
//
#define COLUMN_MAJOR_MODE

#ifdef ARM_NEON_AES_ACCEL
#include <arm_neon.h>
#endif

#ifdef X86_64_AESNI_ACCEL
#include <emmintrin.h>
#include <wmmintrin.h>
#endif

#define SWAP(matrix, i1, j1, i2, j2) \
    t = matrix[i1][j1];              \
    matrix[i1][j1] = matrix[i2][j2]; \
    matrix[i2][j2] = t;

#define SWAP_SYMM_OFF_DIAG_ELEMENTS(matrix, offset) \
    uint8_t t = 0;                                  \
    SWAP(matrix, 0, 1 + offset, 1, 0 + offset)      \
    SWAP(matrix, 0, 2 + offset, 2, 0 + offset)      \
    SWAP(matrix, 0, 3 + offset, 3, 0 + offset)      \
    SWAP(matrix, 1, 2 + offset, 2, 1 + offset)      \
    SWAP(matrix, 1, 3 + offset, 3, 1 + offset)      \
    SWAP(matrix, 2, 3 + offset, 3, 2 + offset)

#define MIX_COLUMN_LUT(a, block, i0, i1, i2, i3)                                                   \
    a[0] = block[i0];                                                                              \
    a[1] = block[i1];                                                                              \
    a[2] = block[i2];                                                                              \
    a[3] = block[i3];                                                                              \
    block[i0] =  mul_by_2_lut[a[0]] ^ mul_by_3_lut[a[1]] ^              a[2]  ^              a[3]; \
    block[i1] =               a[0]  ^ mul_by_2_lut[a[1]] ^ mul_by_3_lut[a[2]] ^              a[3]; \
    block[i2] =               a[0]  ^              a[1]  ^ mul_by_2_lut[a[2]] ^ mul_by_3_lut[a[3]];\
    block[i3] =  mul_by_3_lut[a[0]] ^              a[1]  ^              a[2]  ^ mul_by_2_lut[a[3]];

#define MIX_COLUMN_MUL(a, block, i0, i1, i2, i3)                                                   \
    a[0] = block[i0];                                                                              \
    a[1] = block[i1];                                                                              \
    a[2] = block[i2];                                                                              \
    a[3] = block[i3];                                                                              \
    block[i0] = multiply(2U, a[0]) ^ multiply(3U, a[1]) ^              a[2]  ^              a[3];  \
    block[i1] =              a[0]  ^ multiply(2U, a[1]) ^ multiply(3U, a[2]) ^              a[3];  \
    block[i2] =              a[0]  ^              a[1]  ^ multiply(2U, a[2]) ^ multiply(3U, a[3]); \
    block[i3] = multiply(3U, a[0]) ^              a[1]  ^              a[2]  ^ multiply(2U, a[3]);

#define INVERSE_MIX_COLUMN_LUT(a, block, i0, i1, i2, i3)                                               \
    a[0] = block[i0];                                                                                  \
    a[1] = block[i1];                                                                                  \
    a[2] = block[i2];                                                                                  \
    a[3] = block[i3];                                                                                  \
    block[i0] = mul_by_14_lut[a[0]] ^ mul_by_11_lut[a[1]] ^ mul_by_13_lut[a[2]] ^  mul_by_9_lut[a[3]]; \
    block[i1] =  mul_by_9_lut[a[0]] ^ mul_by_14_lut[a[1]] ^ mul_by_11_lut[a[2]] ^ mul_by_13_lut[a[3]]; \
    block[i2] = mul_by_13_lut[a[0]] ^  mul_by_9_lut[a[1]] ^ mul_by_14_lut[a[2]] ^ mul_by_11_lut[a[3]]; \
    block[i3] = mul_by_11_lut[a[0]] ^ mul_by_13_lut[a[1]] ^  mul_by_9_lut[a[2]] ^ mul_by_14_lut[a[3]];

#define INVERSE_MIX_COLUMN_MUL(a, block, i0, i1, i2, i3)                                               \
    a[0] = block[i0];                                                                                  \
    a[1] = block[i1];                                                                                  \
    a[2] = block[i2];                                                                                  \
    a[3] = block[i3];                                                                                  \
    block[i0] = multiply(14U, a[0]) ^ multiply(11U, a[1]) ^ multiply(13U, a[2]) ^ multiply( 9U, a[3]); \
    block[i1] = multiply( 9U, a[0]) ^ multiply(14U, a[1]) ^ multiply(11U, a[2]) ^ multiply(13U, a[3]); \
    block[i2] = multiply(13U, a[0]) ^ multiply( 9U, a[1]) ^ multiply(14U, a[2]) ^ multiply(11U, a[3]); \
    block[i3] = multiply(11U, a[0]) ^ multiply(13U, a[1]) ^ multiply( 9U, a[2]) ^ multiply(14U, a[3]);

#define XTIME(x) (((x) << 1) ^ (((x) & 0x80U) ? 0x1BU : 0x00U))

#define MIX_COLUMN MIX_COLUMN_MUL
#define INVERSE_MIX_COLUMN INVERSE_MIX_COLUMN_MUL

#if AES_KEY_LEN_CONF == 128
    #define Nk 4
    #define Nr 10
#elif AES_KEY_LEN_CONF == 192
    #define Nk 6
    #define Nr 12
#elif AES_KEY_LEN_CONF == 256
    #define Nk 8
    #define Nr 14
#endif

#define Nb              4
#define AES_WORDS       (Nb * (Nr + 1))
#define AES_EXP_KEY_LEN (AES_WORDS * Nb)
#define AES_KEY_LEN     (AES_KEY_LEN_CONF / 8)
#define AES_BLK_LEN     16

extern uint8_t aes_expanded_key[Nb][AES_WORDS];
static const uint8_t round_constants_arr[11];
static const uint8_t sbox[256];
static const uint8_t inverse_sbox[256];
static const uint8_t mul_by_2_lut[256];
static const uint8_t mul_by_3_lut[256];
static const uint8_t mul_by_9_lut[256];
static const uint8_t mul_by_11_lut[256];
static const uint8_t mul_by_13_lut[256];
static const uint8_t mul_by_14_lut[256];

void expand_key(uint8_t K[Nb][Nk], uint8_t W[Nb][AES_WORDS]);
uint8_t multiply(uint8_t x, uint8_t y);
void aes_mix_columns(uint8_t state[AES_BLK_LEN]);
void aes_inverse_mix_columns(uint8_t state[AES_BLK_LEN]);
void aes_shift_rows(uint8_t arr[AES_BLK_LEN]);
void aes_inverse_shift_rows(uint8_t arr[AES_BLK_LEN]);
void aes_add_round_key(uint8_t *state, uint8_t round);
void aes_sub_bytes(uint8_t *state);
void aes_inverse_sub_bytes(uint8_t *state);
void aes_encrypt_initial_round(uint8_t *state);
void aes_decrypt_initial_round(uint8_t *state);
void aes_encrypt_round(uint8_t *state, uint8_t round);
void aes_decrypt_round(uint8_t *state, uint8_t round);
void aes_encrypt_final_round(uint8_t *state);
void aes_decrypt_final_round(uint8_t *state);
void aes_encrypt_block(uint8_t *block);
void aes_decrypt_block(uint8_t *block);

#ifdef ARM_NEON_AES_ACCEL
void neon_aes_encrypt(const uint8_t *input, uint8_t *output, const uint8x16_t *encr_round_key);
void neon_aes_decrypt(const uint8_t *input, uint8_t *output, const uint8x16_t *decr_round_key);
void neon_aes_encrypt_single_buffer(uint8_t *state, const uint8x16_t *encr_round_key);
void neon_aes_decrypt_single_buffer(uint8_t *state, const uint8x16_t *decr_round_key);
#endif

#ifdef X86_64_AESNI_ACCEL
void aesni_encrypt(uint8_t *block, const __m128i *encr_round_key);
void aesni_decrypt(uint8_t *block, const __m128i *decr_round_key);
#endif