//
// aes_helpers.h
//

#include "aes_cipher.h"
#ifdef ARM_NEON_AES_ACCEL
#include <arm_neon.h>
void neon_get_keys_foreach_round_column_major_u8x16(uint8_t expanded_key[Nb][AES_WORDS], uint8x16_t encr_round_keys[Nr + 1], uint8x16_t decr_round_keys[Nr + 1]);
void neon_get_keys_foreach_round_row_major_u8x16(uint8_t expanded_key[Nb][AES_WORDS], uint8x16_t encr_round_keys[Nr + 1], uint8x16_t decr_round_keys[Nr + 1]);
#endif

#ifdef X86_64_AESNI_ACCEL
#include <emmintrin.h>
#include <wmmintrin.h>
void aesni_get_keys_foreach_round_column_major(uint8_t expanded_key[Nb][AES_WORDS], __m128i encr_round_keys[Nr + 1], __m128i decr_round_keys[Nr + 1]);
void aesni_get_keys_foreach_round_row_major(uint8_t expanded_key[Nb][AES_WORDS], __m128i encr_round_keys[Nr + 1], __m128i decr_round_keys[Nr + 1]);
#endif

void print_expanded_key(uint8_t expanded_key[Nb][AES_WORDS]);
uint8_t round_const(uint8_t ri);
void generate_round_constants(uint8_t *rc_array, uint8_t nth_constant);
void print_round_key_column_major(uint8_t expanded_key[Nb][AES_WORDS], uint8_t round);
void print_round_key_row_major(uint8_t expanded_key[Nb][AES_WORDS], uint8_t round);
void get_round_key_column_major(uint8_t expanded_key[Nb][AES_WORDS], uint8_t round, uint8_t current_round_key[AES_BLK_LEN]);
void get_round_key_row_major(uint8_t expanded_key[Nb][AES_WORDS], uint8_t round, uint8_t current_round_key[AES_BLK_LEN]);
void get_keys_foreach_round_column_major(uint8_t expanded_key[Nb][AES_WORDS], uint8_t nth_round_expanded_key[Nr + 1][AES_BLK_LEN]);
void get_keys_foreach_round_row_major(uint8_t expanded_key[Nb][AES_WORDS], uint8_t nth_round_expanded_key[Nr + 1][AES_BLK_LEN]);