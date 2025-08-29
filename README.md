# 🔐 CORD-CRYPTO

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Build System](https://img.shields.io/badge/Build-CMake-brightgreen.svg)](https://cmake.org/)
[![Framework](https://img.shields.io/badge/Part_of-PacketCord.io-blue)](https://github.com/packetcord/packetcord.io)

**Cryptographic Operations Library for Network Applications**

CORD-CRYPTO is the cryptographic component of the [PacketCord.io](https://github.com/packetcord/packetcord.io) framework, providing security primitives and encryption algorithms optimized for network programming applications.

---

## 🧩 Components

### ✅ Implemented Components

#### 🔒 AES Cipher Implementation
**[cord-aes-cipher](https://github.com/packetcord/cord-aes-cipher)** - Hardware-accelerated AES encryption

- **AES-128/192/256** support with configurable key lengths
- **Hardware acceleration** via AES-NI (x86-64) and NEON (ARM)
- **Software fallback** for platforms without hardware acceleration
- **Embedded-friendly** with configurable memory usage
- **Cross-platform** design for servers to microcontrollers

### 🚧 Planned Components

#### 🔐 Cipher Modes
- **AES-CBC** - Cipher Block Chaining mode
- **AES-CTR** - Counter mode for stream-like encryption  
- **AES-ECB** - Electronic Codebook mode
- **AES-GCM** - Galois/Counter Mode with authentication

#### 🌊 Stream Ciphers
- **ChaCha20** - Modern stream cipher
- **ChaCha20-Poly1305** - Authenticated encryption

---

## 🔗 Related Projects

CORD-CRYPTO is part of the **[PacketCord.io](https://github.com/packetcord/packetcord.io)** framework, which comprises three main components:

- **[CORD-FLOW](https://github.com/packetcord/cord-flow)** - High-performance packet flow programming
- **[CORD-CRYPTO](https://github.com/packetcord/cord-crypto)** - Cryptographic operations and security primitives (this repository)
  - **[cord-aes-cipher](https://github.com/packetcord/cord-aes-cipher)** - Hardware-accelerated AES implementation
- **[CORD-CRAFT](https://github.com/packetcord/cord-craft)** - Network packet crafting and generation tools

---

## 🛠️ Build Instructions

### Prerequisites
- **CMake** 3.16 or higher
- **GCC** with C standard support

### Building CORD-CRYPTO
```bash
git clone https://github.com/packetcord/cord-crypto.git
cd cord-crypto
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Building AES Cipher Component
```bash
cd cord-aes-cipher
gcc -I includes/ -o main main.c src/aes_cipher.c src/aes_helpers.c
./main
```

---

## 📁 Repository Structure

```
cord-crypto/
├── CMakeLists.txt                # Main build configuration
├── README.md                     # This file
├── AES/                         # AES cipher modes (planned)
│   ├── CBC/                     # Cipher Block Chaining
│   ├── CTR/                     # Counter mode
│   ├── ECB/                     # Electronic Codebook
│   ├── GCM/                     # Galois/Counter Mode
│   └── cord-aes-cipher/         # Core AES implementation
├── ChaCha/                      # ChaCha20 implementation (planned)
└── Makefile/                    # Build utilities
```

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

---

<div align="center">

**Part of the PacketCord.io Framework**

[![GitHub Stars](https://img.shields.io/github/stars/packetcord/cord-crypto?style=social)](https://github.com/packetcord/cord-crypto)

</div>