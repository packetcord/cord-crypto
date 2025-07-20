#include "aes_cipher.h"
#include "cbc.h"

void aes_encrypt_cbc(const uint8_t *in, uint8_t *out, size_t len, const aes_key_t *key, const uint8_t *iv) {
    // Use aes_encrypt_block(...) repeatedly with CBC chaining
}
