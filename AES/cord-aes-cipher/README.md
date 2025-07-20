# 🔐 CORD-AES-CIPHER
[![Software Implementation](https://img.shields.io/badge/Software-Implementation-brightgreen)](https://github.com/packetcord/cord-aes-cipher)
[![AES‑NI Acceleration](https://img.shields.io/badge/Feature-AES--NI‑Acceleration‑Supported-brightgreen)](https://github.com/packetcord/cord-aes-cipher)
[![NEON Acceleration](https://img.shields.io/badge/Feature-NEON‑Acceleration‑Supported-brightgreen)](https://github.com/packetcord/cord-aes-cipher)

Highly configurable AES-128/192/256 cipher implementation to work on any device from small microcontrollers to the latest hardware crypto-accelerated CPUs (Intel/AMD/ARMv8).

> ⚠️ **Note**: This library focuses on the **core AES cipher**, providing a standalone, mode-agnostic engine suitable for integration into higher-level encryption schemes.
Cipher modes such as CBC, CTR, and GCM are about to be implemented separately.

## 🔧 Build Instructions
To compile the example, run the following command in the root directory:

- **For the *pure software* implementation**
```bash
gcc -I includes/ -o main main.c src/aes_cipher.c src/aes_helpers.c
```

- **For the *ARMv8 NEON* accelerated implementation**
```bash
gcc -I includes/ -march=armv8-a+crypto -o main main.c src/aes_cipher.c src/aes_helpers.c
```
Or you can even be a bit more specific with the exact core - an example with Toradex Verdin iMX8M Mini based on Cortex-A53 is given below:
```bash
gcc -I includes/ -mcpu=cortex-a53+crypto -o main main.c src/aes_cipher.c src/aes_helpers.c
```

- **For the *x86-64 AES-NI* (both for AMD and Intel) accelerated implementation**
```bash
gcc -I includes/ -march=native -msse2 -maes -o main main.c src/aes_cipher.c src/aes_helpers.c
```

Toggle the defines inside *aes_cipher.h* to enable the AES-NI or NEON acceleration.
```c
//
// Hardware acceleration
//
#define X86_64_AESNI_ACCEL
#define ARM_NEON_AES_ACCEL
```

**And simply run the example via:**
```bash
./main
```

In case you are intersted in more details related to AES, the library implementation and its fine tuning capabilities, you can continue reading the below lines...

## 🧠 What is AES?

**AES (Advanced Encryption Standard)** is a **block cipher** standardized by NIST in 2001. It encrypts fixed-size blocks of **128 bits (16 bytes)** using keys of varying lengths:
- 🔑 **128-bit key**
- 🔑 **192-bit key**
- 🔑 **256-bit key**

AES performs a series of transformations on the input data through multiple rounds:
- 128-bit key → **10 rounds**
- 192-bit key → **12 rounds**
- 256-bit key → **14 rounds**

Each round consists of the following operations:
1. **Substitute Bytes**
2. **Shift Rows**
3. **Mix Columns**
4. **Add Round Key**

## 🧩 Cipher Modes (CBC, CTR, GCM, etc.)
AES on its own only encrypts individual blocks. **Cipher modes** define how to apply AES to longer messages or to add features like randomness or authentication:

- **CBC (Cipher Block Chaining)** – Adds dependency between blocks.
- **CTR (Counter Mode)** – Converts AES into a stream cipher.
- **GCM (Galois/Counter Mode)** – Adds authentication (AEAD).

> 🧱 This library **does not** implement these modes directly, but is planned to be supported (either within this repo or via separate ones).

## 🔑 Key Expansion

The "Key Size" refers to the original size of the key (the number after the AES abbreviation: AES-**128**, AES-**192** and AES-**256**). It is represented as a matrix of 4 rows and N columns, depending on the key length. 

| **Key Size** | **N rows** | **N columns** |
|----------|--------|----------|
| 128-bit  | 4      | 4        |
| 192-bit  | 4      | 6        |
| 256-bit  | 4      | 8        |

| **AES Version** | **Key Size** | **Number of Rounds (Nr)** | **N rows**  | **N columns**                    | **Expanded Key Size (in bytes)** |
| --------------- | ------------ | ------------------------- |-------------| -------------------------------- | -------------------------------- |
| AES-128         | 128 bits     | 10                        | 4           | 4 × (10 + 1) = **44**            | 44 × 4 = **176 bytes**           |
| AES-192         | 192 bits     | 12                        | 4           | 4 × (12 + 1) = **52**            | 52 × 4 = **208 bytes**           |
| AES-256         | 256 bits     | 14                        | 4           | 4 × (14 + 1) = **60**            | 60 × 4 = **240 bytes**           |

AES operates on a **4×4 byte matrix** called the **state**, which evolves with each round. Each N-th round takes a square matrix of 4x4 (16 bytes) from the expanded key rectangular matrix. The longer key (more AES "bits" configuration) results in a longer expanded key and more rounds respectively (more portions of 4x4 squares to be applied as part of the *Add Round Key* operation).

## ⚙️ Fine Tuning
As mentioned above, the library is designed to be cool enough by providing the flexibility to be fine tuned for small footprint or performance (including utilisation the AES hardware instructions on any popular architecture).

### 🧱 AES key length
The key length is defined by the *AES_KEY_LEN_CONF* macro inside *aes_cipher.h*. The default is set to 128 bit (AES-128):
```c
#define AES_KEY_LEN_CONF 128
```
Possible values are *128*, *192* and *256*.

### 🧱 Mix Columns (refers to the purely software AES, non-hardware accelerated)
The *Mix Columns* and *Inverse Mix Columns* operations could be configured to work either with focus on preserving CPU cycles or memory (Flash or RAM, depending on where we instruct the compiler to store the look-up table array).

The following macros have been exposed accordingly:
```c
#define MIX_COLUMN MIX_COLUMN_MUL
```
and
```c
#define INVERSE_MIX_COLUMN INVERSE_MIX_COLUMN_MUL
```
**MIX_COLUMN** can be set to *MIX_COLUMN_LUT* to use an in-memory pre-calculated look-up table with result values or *MIX_COLUMN_MUL* to use the CPU do the actual multiplication in Galois Field.

**INVERSE_MIX_COLUMN** can similarly be set to *INVERSE_MIX_COLUMN_LUT* or *INVERSE_MIX_COLUMN_MUL*.

### 🧱 CPU vs RAM vs Flash utilisation (quite relevant for microcontrollers)
The AES key is a tricky point when it comes to memory optimisation, especially on microcontrollers with less then 1KB of memory (like the PIC16 series, for example).
As described above, the expanded key is either 176, 208 or even 240 bytes long and even though the "initial" original key could be stored in the flash memory, the expansion happens
at runtime which means that it will end up in the main memory (RAM).

The CORD-AES-CIPHER library comes with the following helper function (inside *aes_helpers.c* and *aes_helpers.h* files):

```c
void print_expanded_key(uint8_t expanded_key[Nb][AES_WORDS]);
```
The example output for the original key:
```c
uint8_t key[Nb * Nk] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
```

After expansion it looks as follows:

```c
//
// print_expanded_key(aes_expanded_key)
//
uint8_t aes_expanded_key[Nb][AES_WORDS] = {{ 0x2B, 0x7E, 0x15, 0x16, 0xA0, 0xFA, 0xFE, 0x17, 0xF2, 0xC2, 0x95, 0xF2, 0x3D, 0x80, 0x47, 0x7D, 0xEF, 0x44, 0xA5, 0x41, 0xD4, 0xD1, 0xC6, 0xF8, 0x6D, 0x88, 0xA3, 0x7A, 0x4E, 0x54, 0xF7, 0x0E, 0xEA, 0xD2, 0x73, 0x21, 0xAC, 0x77, 0x66, 0xF3, 0xD0, 0x14, 0xF9, 0xA8 },
{ 0x28, 0xAE, 0xD2, 0xA6, 0x88, 0x54, 0x2C, 0xB1, 0x7A, 0x96, 0xB9, 0x43, 0x47, 0x16, 0xFE, 0x3E, 0xA8, 0x52, 0x5B, 0x7F, 0x7C, 0x83, 0x9D, 0x87, 0x11, 0x0B, 0x3E, 0xFD, 0x5F, 0x5F, 0xC9, 0xF3, 0xB5, 0x8D, 0xBA, 0xD2, 0x19, 0xFA, 0xDC, 0x21, 0xC9, 0xEE, 0x25, 0x89 },
{ 0xAB, 0xF7, 0x15, 0x88, 0x23, 0xA3, 0x39, 0x39, 0x59, 0x35, 0x80, 0x7A, 0x1E, 0x23, 0x7E, 0x44, 0xB6, 0x71, 0x25, 0x3B, 0xCA, 0xF2, 0xB8, 0xBC, 0xDB, 0xF9, 0x86, 0x41, 0x84, 0xA6, 0x4F, 0xB2, 0x31, 0x2B, 0xF5, 0x60, 0x28, 0xD1, 0x29, 0x41, 0xE1, 0x3F, 0x0C, 0xC8 },
{ 0x09, 0xCF, 0x4F, 0x3C, 0x2A, 0x6C, 0x76, 0x05, 0x73, 0x59, 0xF6, 0x7F, 0x6D, 0x7A, 0x88, 0x3B, 0xDB, 0x0B, 0xAD, 0x00, 0x11, 0xF9, 0x15, 0xBC, 0xCA, 0x00, 0x93, 0xFD, 0x4E, 0xA6, 0xDC, 0x4F, 0x7F, 0x8D, 0x29, 0x2F, 0x57, 0x5C, 0x00, 0x6E, 0xB6, 0x63, 0x0C, 0xA6 }};
```

So, one can first start coding on their PC, expand the key, export it and copy-paste the array (probably by also making it *const*) inside the embedded project. This way the scheduled key may be tuned to end up in the flash memory of the MCU instead of occupying the limited RAM.

> ⚠️ **Note**: The same approach could (or even should) be applied to the above mentioned look-up tables used for the implementations of *Mix Columns* and *Inverse Mix Columns*.

Currently, by default, the look-up tables and other arrays have already been set as *const*, so that they will eventually reside in the flash memory when used on a microcontroller with limited resources:
```c
static const uint8_t round_constants_arr[11];
static const uint8_t sbox[256];
static const uint8_t inverse_sbox[256];
static const uint8_t mul_by_2_lut[256];
static const uint8_t mul_by_3_lut[256];
static const uint8_t mul_by_9_lut[256];
static const uint8_t mul_by_11_lut[256];
static const uint8_t mul_by_13_lut[256];
static const uint8_t mul_by_14_lut[256];
```

### 🧱 Hardware acceleration
Along with the pure software implementation, the library also provides support for hardware acceleration, utilising the specialised AES instructions in the x86-64 and AArch64 architectures.
Inside *aes_cipher.h* there are two defines to enable either of them:

```c
#define X86_64_AESNI_ACCEL
```
```c
#define ARM_NEON_AES_ACCEL
```

> ⚠️ **Note**: AES-NI is enabled by default.

### 📐 X-Major Order (Column or Row)
When working with AES, understanding how the 16-byte input is interpreted internally is essential. AES operates on a 4x4 **state matrix** of bytes, which is typically filled **column by column** — a format known as **column-major order**.

However, some applications may benefit from using **row-major** order, where the matrix is filled left-to-right, row by row.

### 🔍 What’s the difference?
| Format           | Filling Order                     | Description                                       |
|------------------|-----------------------------------|--------------------------------------------------|
| **Column-Major** | Top to bottom, left to right      | Fill column 0 from top to bottom, then column 1, etc. |
| **Row-Major**    | Left to right, top to bottom      | Fill row 0 from left to right, then row 1, etc.      |

---

### 🧪 Example: Visualising the State Matrix
Consider the below 16-byte array:

```cpp
uint8_t buffer[16] = {
  0x00, 0x01, 0x02, 0x03,
  0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B,
  0x0C, 0x0D, 0x0E, 0x0F
};
```

### 📦 Column-Major Representation (Default in AES)
AES fills the state matrix in column-major order by default:
```
| 0x00  0x04  0x08  0x0C |
| 0x01  0x05  0x09  0x0D |
| 0x02  0x06  0x0A  0x0E |
| 0x03  0x07  0x0B  0x0F |
```
This is represented as:
```
Column 0: 0x00, 0x01, 0x02, 0x03
Column 1: 0x04, 0x05, 0x06, 0x07
Column 2: 0x08, 0x09, 0x0A, 0x0B
Column 3: 0x0C, 0x0D, 0x0E, 0x0F
```

### 📦 Row-Major Representation
In row-major order, the state matrix would look like this:
```
| 0x00  0x01  0x02  0x03 |
| 0x04  0x05  0x06  0x07 |
| 0x08  0x09  0x0A  0x0B |
| 0x0C  0x0D  0x0E  0x0F |
```
Similarly, this is represented as:
```
Row 0: 0x00, 0x01, 0x02, 0x03
Row 1: 0x04, 0x05, 0x06, 0x07
Row 2: 0x08, 0x09, 0x0A, 0x0B
Row 3: 0x0C, 0x0D, 0x0E, 0x0F
```

### 🧬 Flexibility in the CORD-AES-CIPHER Library
This AES implementation adheres to the standard column-major memory layout by default. However, for advanced use cases, experimentation, or obfuscation, you can switch to row-major layout easily.

### ✅ Possible Benefits of Layout Switching
- 🔄 **Zero-copy integration** with different data sources and data representations. The library explicitly targets the zero-copy approach - all mapping transformations are done on the key and take place only in the initialisation phase.
- 🚧 **Slightly more obfuscated output** that may slow down naive reverse engineering.
- 🧪 **Custom experimentation** with non-standard AES behavior (as long as transformations remain consistent).

> 💡 Note 1: Changing the layout does not alter the cryptographic strength of AES if used correctly — it simply changes how the internal state is interpreted and manipulated.

> 💡 Note 2: When used with *pure software* implementation, it will provide the same encrypted output. It is also possible to be used in hardware accelearated implementations (AES-NI or NEON), but it will treat the key differently, thus produce different encrypted output. We encourage you to experiment - you may discover interesting representation and obfuscation capabilities as well.

### 🔧 Switch between layouts

The following macro defines the default column-major layout:
```c
#define COLUMN_MAJOR_MODE
```
To switch to row-major layout, change the above to:
```c
#define ROW_MAJOR_MODE
```

### 🚀 **Tune, build, run and enjoy!** 🎉
