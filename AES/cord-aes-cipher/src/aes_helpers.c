//
// aes_helpers.c
//

#include "aes_helpers.h"

#ifdef PRE_SCHEDULED_KEY_EXPORT
void print_expanded_key(uint8_t expanded_key[Nb][AES_WORDS])
{
    printf("uint8_t aes_expanded_key[Nb][AES_WORDS] = {");
    for (uint8_t i = 0; i < Nb; i++)
    {
        printf("{ ");
        for (uint8_t j = 0; j < AES_WORDS; j++)
        {
            printf("0x%02X", expanded_key[i][j]);
            if (j == (AES_WORDS - 1)) printf(" ");
            else printf(", ");
        }
        if (i == (Nb - 1)) printf("}");
        else printf("},\n");
    }
    printf("};");
}
#endif

#ifdef RC_GENERATE_FUNCTIONS
uint8_t round_const(uint8_t ri)
{
    if (ri == 0) // Value [0] should never be used, RCs start from [1]
        return 0;
    else if (ri <= 1)
        return 1;
    else
        if (round_const(ri - 1) < 0x80)
            return 2 * round_const(ri - 1);
        else
            return (2 * round_const(ri - 1)) ^ 0x11B;
}

void generate_round_constants(uint8_t *rc_array, uint8_t nth_constant)
{
    for (uint8_t n = 1; n < nth_constant; n++)
        rc_array[n] = round_const(n);
    
    return;
}
#endif

void get_round_key_column_major(uint8_t expanded_key[Nb][AES_WORDS], uint8_t round, uint8_t current_round_key[AES_BLK_LEN])
{
    uint8_t n = 0;

    if (round > Nr)
        return;

    for (uint8_t j = Nb * round; j < (Nb * round) + Nb; j++)
        for (uint8_t i = 0; i < Nb; i++)
            current_round_key[n++] = expanded_key[i][j];
}

void get_round_key_row_major(uint8_t expanded_key[Nb][AES_WORDS], uint8_t round, uint8_t current_round_key[AES_BLK_LEN])
{
    uint8_t n = 0;

    if (round > Nr)
        return;

    for (uint8_t i = 0; i < Nb; i++)
        for (uint8_t j = Nb * round; j < (Nb * round) + Nb; j++)
            current_round_key[n++] = expanded_key[i][j];
}

#ifdef ARM_NEON_AES_ACCEL
void neon_get_keys_foreach_round_column_major_u8x16(uint8_t expanded_key[Nb][AES_WORDS], uint8x16_t encr_round_keys[Nr + 1], uint8x16_t decr_round_keys[Nr + 1])
{
    uint8_t temp[AES_BLK_LEN];

    for (uint8_t n = 0; n <= Nr; n++)
    {
        get_round_key_column_major(expanded_key, n, temp);
        encr_round_keys[n] = vld1q_u8(temp);
    }

    decr_round_keys[0] = encr_round_keys[Nr];
    decr_round_keys[Nr] = encr_round_keys[0];

    for (uint8_t n = 1; n < Nr; n++)
    {
        decr_round_keys[n] = vaesimcq_u8(encr_round_keys[Nr - n]);
    }
}

void neon_get_keys_foreach_round_row_major_u8x16(uint8_t expanded_key[Nb][AES_WORDS], uint8x16_t encr_round_keys[Nr + 1], uint8x16_t decr_round_keys[Nr + 1])
{
    uint8_t temp[AES_BLK_LEN];

    for (uint8_t n = 0; n <= Nr; n++)
    {
        get_round_key_row_major(expanded_key, n, temp);
        encr_round_keys[n] = vld1q_u8(temp);
    }

    decr_round_keys[0] = encr_round_keys[Nr];
    decr_round_keys[Nr] = encr_round_keys[0];

    for (uint8_t n = 1; n < Nr; n++)
    {
        decr_round_keys[n] = vaesimcq_u8(encr_round_keys[Nr - n]);
    }
}
#endif

#ifdef X86_64_AESNI_ACCEL
void aesni_get_keys_foreach_round_column_major(uint8_t expanded_key[Nb][AES_WORDS], __m128i encr_round_keys[Nr + 1], __m128i decr_round_keys[Nr + 1])
{
    uint8_t temp[AES_BLK_LEN];

    for (uint8_t n = 0; n <= Nr; n++)
    {
        get_round_key_column_major(expanded_key, n, temp);
        encr_round_keys[n] = _mm_loadu_si128((__m128i *)temp);
    }

    decr_round_keys[0] = encr_round_keys[Nr];
    decr_round_keys[Nr] = encr_round_keys[0];

    for (uint8_t n = 1; n < Nr; n++)
    {
        decr_round_keys[n] = _mm_aesimc_si128(encr_round_keys[Nr - n]);
    }
}

void aesni_get_keys_foreach_round_row_major(uint8_t expanded_key[Nb][AES_WORDS], __m128i encr_round_keys[Nr + 1], __m128i decr_round_keys[Nr + 1])
{
    uint8_t temp[AES_BLK_LEN];

    for (uint8_t n = 0; n <= Nr; n++)
    {
        get_round_key_row_major(expanded_key, n, temp);
        encr_round_keys[n] = _mm_loadu_si128((__m128i *)temp);
    }

    decr_round_keys[0] = encr_round_keys[Nr];
    decr_round_keys[Nr] = encr_round_keys[0];

    for (uint8_t n = 1; n < Nr; n++)
    {
        decr_round_keys[n] = _mm_aesimc_si128(encr_round_keys[Nr - n]);
    }
}
#endif

void print_round_key_column_major(uint8_t expanded_key[Nb][AES_WORDS], uint8_t round)
{
    if (round > Nr)
        return;

    printf("Round %u key (default, column major): \n", round);
    for (uint8_t j = Nb * round; j < (Nb * round) + Nb; j++)
        for (uint8_t i = 0; i < Nb; i++)
            printf("0x%02X\n", expanded_key[i][j]);
}

void print_round_key_row_major(uint8_t expanded_key[Nb][AES_WORDS], uint8_t round)
{
    if (round > Nr)
        return;

    printf("Round %u key (row major): \n", round);
    for (uint8_t i = 0; i < Nb; i++)
        for (uint8_t j = Nb * round; j < (Nb * round) + Nb; j++)
            printf("0x%02X\n", expanded_key[i][j]);
}

void get_keys_foreach_round_column_major(uint8_t expanded_key[Nb][AES_WORDS], uint8_t nth_round_expanded_key[Nr + 1][AES_BLK_LEN])
{
    for (uint8_t n = 0; n < Nr + 1; n++)
        get_round_key_column_major(expanded_key, n, nth_round_expanded_key[n]);
}

void get_keys_foreach_round_row_major(uint8_t expanded_key[Nb][AES_WORDS], uint8_t nth_round_expanded_key[Nr + 1][AES_BLK_LEN])
{
    for (uint8_t n = 0; n < Nr + 1; n++)
        get_round_key_row_major(expanded_key, n, nth_round_expanded_key[n]);
}