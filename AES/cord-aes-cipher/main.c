//
// main.c
//

#include "aes_cipher.h"
#include "aes_helpers.h"
#ifdef ARM_NEON_AES_ACCEL
#include <arm_neon.h>
#endif

int main()
{
    printf("AES_EXP_KEY_LEN: %u\n", AES_EXP_KEY_LEN);

    uint8_t key_matrix[Nb][Nk] = { 0x00 };

    uint8_t output[AES_BLK_LEN]  = { 0x00 };

#ifdef ROW_MAJOR_MODE
    uint8_t  input[AES_BLK_LEN]  = { 0x32, 0x88, 0x31, 0xE0, 0x43, 0x5A, 0x31, 0x37, 0xF6, 0x30, 0x98, 0x07, 0xA8, 0x8D, 0xA2, 0x34 };
#else
    uint8_t  input[AES_BLK_LEN]  = { 0x32, 0x43, 0xF6, 0xA8, 0x88, 0x5A, 0x30, 0x8D, 0x31, 0x31, 0x98, 0xA2, 0xE0, 0x37, 0x07, 0x34 };
#endif

#if AES_KEY_LEN_CONF == 128
    uint8_t key[Nb * Nk] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

    uint8_t key_row_major[Nb * Nk] = {
        0x2B, 0x28, 0xAB, 0x09,
        0x7E, 0xAE, 0xF7, 0xCF,
        0x15, 0xD2, 0x15, 0x4F,
        0x16, 0xA6, 0x88, 0x3C
    };
#elif AES_KEY_LEN_CONF == 192
    uint8_t key[Nb * Nk] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C, 
                            0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6 };

    uint8_t key_row_major[Nb * Nk] = {
        0x2B, 0x28, 0xAB, 0x09, 0x2B, 0x28,
        0x7E, 0xAE, 0xF7, 0xCF, 0x7E, 0xAE,
        0x15, 0xD2, 0x15, 0x4F, 0x15, 0xD2,
        0x16, 0xA6, 0x88, 0x3C, 0x16, 0xA6
    };
#elif AES_KEY_LEN_CONF == 256
    uint8_t key[Nb * Nk] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C,
                            0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

    uint8_t key_row_major[Nb * Nk] = {
        0x2B, 0x28, 0xAB, 0x09, 0x2B, 0x28, 0xAB, 0x09,
        0x7E, 0xAE, 0xF7, 0xCF, 0x7E, 0xAE, 0xF7, 0xCF,
        0x15, 0xD2, 0x15, 0x4F, 0x15, 0xD2, 0x15, 0x4F,
        0x16, 0xA6, 0x88, 0x3C, 0x16, 0xA6, 0x88, 0x3C
    };
#endif

#ifdef ROW_MAJOR_MODE
    //
    // You can still keep the key in column-major and do the mapping
    //
    /*
    for (uint8_t n = 0; n < (Nb * Nk); n++)
        key_matrix[n % Nb][n / Nb] = key[n];

    expand_key(key_matrix, aes_expanded_key);
    */

    //
    // Or if the key is also in row-major, just pass it as 1D array
    //
    #ifndef PRE_SCHEDULED_KEY
    expand_key((uint8_t (*)[Nk])key_row_major, aes_expanded_key);
    #endif
#else
    #ifndef PRE_SCHEDULED_KEY
    for (uint8_t n = 0; n < (Nb * Nk); n++)
        key_matrix[n % Nb][n / Nb] = key[n];

    expand_key(key_matrix, aes_expanded_key);

    for (uint8_t n = 0; n < (Nb * Nr) + Nb; n += Nb)
    {
        SWAP_SYMM_OFF_DIAG_ELEMENTS(aes_expanded_key, n);
    }
    #endif
#endif

    uint8_t  ref_input[AES_BLK_LEN] = { 0 };
    printf("\n\nPlain:");
    for (uint8_t i = 0; i < AES_BLK_LEN; i++)
    {
        if ( i % 4 == 0 )
            printf("\n");
        printf("0x%02X ", input[i]);
        ref_input[i] = input[i];
    }

    //
    // Pure-SW implementation
    //
    printf("\n\nPure-SW implementation test...\n");

    aes_encrypt_block(input);
    printf("\nEncrypted:");
    for (uint8_t i = 0; i < AES_BLK_LEN; i++)
    {
        if ( i % 4 == 0 )
            printf("\n");
        printf("0x%02X ", input[i]);
    }

    aes_decrypt_block(input);
    printf("\n\nDecrypted:");
    for (uint8_t i = 0; i < AES_BLK_LEN; i++)
    {
        if ( i % 4 == 0 )
            printf("\n");
        printf("0x%02X ", input[i]);
    }

    for (uint8_t i = 0; i < AES_BLK_LEN; i++)
    {
        if (input[i] != ref_input[i])
        {
            printf("\n\nPure-SW implementation FAILED!\n");
            return 0;
        }
    }

    printf("\n\nPure-SW implementation SUCCESS!\n");

#ifdef ARM_NEON_AES_ACCEL
    //
    // ARMv8 NEON AES accelerated implementation
    //
    printf("\n\nARM NEON acceleration test...\n");

    for (uint8_t n = 0; n < (Nb * Nk); n++)
        key_matrix[n % Nb][n / Nb] = key[n];

    expand_key(key_matrix, aes_expanded_key);

    uint8x16_t encr_round_keys[Nr + 1];
    uint8x16_t decr_round_keys[Nr + 1];
    neon_get_keys_foreach_round_column_major_u8x16(aes_expanded_key, encr_round_keys, decr_round_keys);

    printf("\nPlain:");
    for (uint8_t i = 0; i < AES_BLK_LEN; i++)
    {
        if ( i % 4 == 0 )
            printf("\n");
        printf("0x%02X ", input[i]);
    }

    neon_aes_encrypt(input, output, encr_round_keys);
    printf("\n\nNEON accel. encrypted:");
    for (uint8_t i = 0; i < AES_BLK_LEN; i++)
    {
        if ( i % 4 == 0 )
            printf("\n");
        printf("0x%02X ", output[i]);
    }

    neon_aes_decrypt(output, input, decr_round_keys);
    printf("\n\nNEON accel. decrypted:");
    for (uint8_t i = 0; i < AES_BLK_LEN; i++)
    {
        if ( i % 4 == 0 )
            printf("\n");
        printf("0x%02X ", input[i]);
    }

    for (uint8_t i = 0; i < AES_BLK_LEN; i++)
    {
        if (input[i] != ref_input[i])
        {
            printf("\n\nNEON accel. implementation FAILED!\n");
            return 0;
        }
    }

    printf("\n\nNEON accel. implementation SUCCESS!\n");
#endif

#ifdef X86_64_AESNI_ACCEL
    printf("\nx86-64 AES-NI acceleration test...\n");

    for (uint8_t n = 0; n < (Nb * Nk); n++)
        key_matrix[n % Nb][n / Nb] = key[n];

    expand_key(key_matrix, aes_expanded_key); // Or maybe rewrite it to utilise _mm_aeskeygenassist_si128, but we'll skip this for now since it won't affect the encrypt/decrypt operations performance.

    __m128i encr_round_keys[Nr + 1];
    __m128i decr_round_keys[Nr + 1];

    aesni_get_keys_foreach_round_column_major(aes_expanded_key, encr_round_keys, decr_round_keys);

    aesni_encrypt(input, encr_round_keys);
    printf("\nAES-NI accel. encrypted:");
    for (uint8_t i = 0; i < AES_BLK_LEN; i++)
    {
        if ( i % 4 == 0 )
            printf("\n");
        printf("0x%02X ", input[i]);
    }

    aesni_decrypt(input, decr_round_keys);
    printf("\n\nAES-NI accel. decrypted:");
    for (uint8_t i = 0; i < AES_BLK_LEN; i++)
    {
        if ( i % 4 == 0 )
            printf("\n");
        printf("0x%02X ", input[i]);
    }

    printf("\n\nAES-NI accel. implementation SUCCESS!\n");
#endif

    return 0;
}