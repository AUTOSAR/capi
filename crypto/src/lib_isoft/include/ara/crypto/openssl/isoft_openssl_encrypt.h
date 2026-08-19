// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------
//
// OpenSSL source attribution
//
// Portions of this file are derived from OpenSSL source code.
// OpenSSL 3.5.1 is used as the reference version for source attribution.
//
// Reference upstream sources:
// - https://github.com/openssl/openssl/blob/openssl-3.5.1/providers/implementations/kem/rsa_kem.c
// - https://github.com/openssl/openssl/blob/openssl-3.5.1/crypto/evp/e_des3.c
// - https://github.com/openssl/openssl/blob/openssl-3.5.1/crypto/buffer/buffer.c
//
// Applicable copyright notices for the OpenSSL-derived portions:
// - rsa_kem.c: Copyright 2020-2025 The OpenSSL Project Authors. All Rights Reserved.
// - e_des3.c: Copyright 1995-2024 The OpenSSL Project Authors. All Rights Reserved.
// - buffer.c: Copyright 1995-2020 The OpenSSL Project Authors. All Rights Reserved.
//
// The OpenSSL-derived portions are licensed under the Apache License 2.0.
// A copy of the license is provided in:
// LICENSES/LICENSE-Apache-2.0.txt
//
// The OpenSSL-derived portions have been modified for integration into the
// CAPI C++ and AUTOSAR-specific codebase.
//
// Nothing in the AUTOSAR notice above is intended to restrict the rights
// granted under the Apache License 2.0 for the OpenSSL-derived portions.
//
// --------------------------------------------------------------------------

/// ================================================================
///
/// File description:
/// ----------------
/// @file       isoft_openssl_encrypt.h
/// @brief
/// @details
/// @date       2024-07-15
/// @author     Che Jinzhao
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CRYPTO_PUHUA_OPENSSL_API_H_
#define ARA_CRYPTO_PUHUA_OPENSSL_API_H_

// Define macro for determining version of kInt_1.kInt_1.X
#define PUHUA_OPENSSL_IS_11X (OPENSSL_VERSION_NUMBER >= 0x10100000L && OPENSSL_VERSION_NUMBER < 0x20000000L)
// 0x30500010
#include <openssl/aes.h>
#include <openssl/crypto.h>
#include <openssl/des.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/modes.h>
#include <openssl/opensslv.h>
#include <openssl/pem.h>
#include <openssl/rand.h>  // NOLINT
#include <openssl/rsa.h>
#include <openssl/sha.h>

#include <cstring>

#if !PUHUA_OPENSSL_IS_11X
    #include <openssl/core_names.h>
    #include <openssl/param_build.h>
    #include <openssl/provider.h>
#endif
#include <openssl/ec.h>
#include <openssl/rsa.h>

#include "ara/core/vector.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/isoft_generate_key_data.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"  // NOLINT

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_06485
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
#define PH_BIO_pending(b) static_cast< int32_t >(BIO_ctrl(b, BIO_CTRL_PENDING, 0, nullptr))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_06486
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
#define PH_BIO_get_mem_ptr(b, pp)                                                                                      \
    BIO_ctrl(b, BIO_C_GET_BUF_MEM_PTR, 0, static_cast< char8_t * >(static_cast< void * >(pp)))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_06487
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
#define PH_BIO_set_close(b, c) static_cast< int32_t >(BIO_ctrl(b, BIO_CTRL_SET_CLOSE, (c), nullptr))

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03005
/// @trace_id_ad=AD_CRYPTO_01434
/// @needwork = dd
/// @endcode
#define PH_EVP_PKEY_assign_RSA(pkey, rsa)                                                                              \
    EVP_PKEY_assign((pkey), EVP_PKEY_RSA, static_cast< ara::crypto::char8_t * >(static_cast< void * >((rsa))))

// Further encapsulate type conversion macro, automatically select the correct conversion method based on version
#if PUHUA_OPENSSL_IS_11X
    #define I2D_CAST(func) reinterpret_cast< int32_t (*)(void *, u_char **) >(func)
#else
    #define I2D_CAST(func) reinterpret_cast< int32_t (*)(const void *, u_char **) >(func)
#endif

namespace ara {
namespace crypto {

#if PUHUA_OPENSSL_IS_11X
/// @brief Copy ECC public key (without private key)
/// @param src Source ECC key (must contain the public key)
/// @return A new EVP_PKEY containing only the public key, or NULL on failure
EVP_PKEY *Evp_pkey_ecc_dup_pub(const EVP_PKEY *src) noexcept;
/// @brief Copy ECC private key (including the corresponding public key)
/// @param src Source ECC key (must contain the private key)
/// @return A new EVP_PKEY containing both private key and public key, or NULL on failure
EVP_PKEY *Evp_pkey_ecc_dup_priv(const EVP_PKEY *src) noexcept;
/// @brief rsa sve_recover
/// @param rsa pointer to RSA resource
/// @param out output buffer
/// @param outlen length of output buffer
/// @param in input data buffer
/// @param inlen input data length
/// @return 0 sucess，fail otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00167
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
int32_t Rsasve_recover_evp(
    EVP_PKEY *const pkey, u_char *const out, size_t *const outlen, u_char const *const in, size_t const inlen) noexcept;
/// @brief rsa sve_gen_rand_bytes
/// @param rsaPub pointer to RSA public key resource
/// @param out output buffer
/// @param outlen length of output buffer
/// @return 0 sucess，fail otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00168
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
int32_t Rsasve_gen_rand_bytes(RSA *const rsaPub, u_char *const out, int32_t const outlen) noexcept;
/// @brief rsa sve_generate
/// @param rsa pointer to RSA resource
/// @param out output buffer
/// @param outlen length of output buffer
/// @param secret secret seed
/// @param secretlen length of secret seed
/// @return 0 sucess，fail otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00169
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
int32_t Rsasve_generate_evp(EVP_PKEY *const pkey,
                            u_char *const out,
                            size_t *const outlen,
                            u_char *const secret,
                            size_t *const secretlen) noexcept;
#else
/// @brief Generate a random number z (kInt_1 < z < n-1) compliant with NIST.SP.800-56Br2 standard
/// @param pkey EVP_PKEY type key (must be of RSA type)
/// @param out  output buffer
/// @param outlen output length (must equal the RSA modulus length)
/// @return kInt_1 on success, 0 on failure
int32_t Rsasve_gen_rand_bytes_evp(EVP_PKEY *pkey, u_char *out, int32_t outlen) noexcept;
/// @brief rsasve_generate
/// @param rsa pointer to RSA resource
/// @param out output buffer
/// @param outlen length of output buffer
/// @param secret secret seed
/// @param secretlen length of secret seed
/// @return 0 fail 1 sucess
int32_t Rsasve_generate_evp(EVP_PKEY *const pkey,
                            u_char *const out,
                            size_t *const outlen,
                            u_char *const secret,
                            size_t *const secretlen) noexcept;
/// @brief rsasve_recover_evp
/// @param rsa pointer to RSA resource
/// @param out output buffer
/// @param outlen length of output buffer
/// @param in input data buffer
/// @param inlen Input data length
int32_t Rsasve_recover_evp(
    EVP_PKEY *const pkey, u_char *const out, size_t *const outlen, u_char const *const in, size_t const inlen) noexcept;
/// @brief General encryption/decryption function for EVP version
/// @param in Input data (length must be an integer multiple of AES_BLOCK_SIZE)
/// @param out Output buffer (must be pre-allocated with sufficient space)
/// @param key Key data (length must be 16/24/32 bytes, corresponding to AES-128/192/256)
/// @param cipher Encryption algorithm
/// @param enc 1=encryption, 0=decryption
/// @param update_len Encrypted length
/// @param setoffset Whether to set offset
/// @param num Tracks the processed bit count (0-7)
void Encrypt_evp_common(u_char const *input,
                        u_char *output,
                        u_char const *key,
                        EVP_CIPHER const *cipher,
                        const int32_t enc,
                        int32_t length,
                        u_char *ivec = nullptr,
                        int32_t *num = nullptr) noexcept;
/// @brief Custom AES-ECB encryption function, does not use AES_KEY
/// @param in Input data (length must be an integer multiple of AES_BLOCK_SIZE)
/// @param out Output buffer (must be pre-allocated with sufficient space)
/// @param key Key data (length must be 16/24/32 bytes, corresponding to AES-128/192/256)
/// @param keyLen Key length (16/24/32)
/// @param enc 1=encryption, 0=decryption
void AES_ecb_encrypt_evp(u_char const *in, u_char *out, u_char const *key, int32_t keyLen, const int32_t enc) noexcept;
/// @brief Custom AES-CBC encryption function, EVP implementation, does not use AES_KEY
/// @param in Input data block (8 bytes)
/// @param out Output buffer (8 bytes)
/// @param length Data length (must be a multiple of 8)
/// @param key Key data (16/24/32 bytes, corresponding to AES-128/192/256)
/// @param keyLen Key length (16/24/32)
/// @param ivec IV (input: initial IV, output: updated IV)
/// @param enc 1=encryption, 0=decryption
void AES_cbc_encrypt_evp(u_char const *in,
                         u_char *out,
                         size_t length,
                         u_char const *key,
                         int32_t keyLen,
                         u_char *ivec,
                         const int32_t enc) noexcept;
/// @brief DES-ECB encryption/decryption function (EVP implementation, does not rely on DES-specific types)
/// @param input Input data block (8 bytes)
/// @param output Output buffer (8 bytes)
/// @param key First key (8 bytes)
/// @param enc 1=encryption, 0=decryption
void DES_ecb_encrypt_evp(u_char const *input, u_char *output, u_char const *key, int32_t enc) noexcept;
/// @brief DES-NCBC encryption/decryption function (EVP implementation, does not rely on DES-specific types). DES-NCBC does not handle padding.
/// @param input Input data block (8 bytes)
/// @param output Output buffer (8 bytes)
/// @param key First key (8 bytes)
/// @param ivec IV (input: initial IV, output: updated IV)
/// @param enc 1=encryption, 0=decryption
void DES_ncbc_encrypt_evp(
    u_char const *input, u_char *output, int64_t length, u_char const *key, u_char *ivec, int32_t enc) noexcept;
/// @brief 3DES-ECB encryption/decryption function (EVP implementation, does not rely on DES-specific types)
/// @param input Input data block (8 bytes)
/// @param output Output buffer (8 bytes)
/// @param key1 First key (8 bytes)
/// @param key2 Second key (8 bytes)
/// @param key3 Third key (8 bytes)
/// @param enc 1=encryption, 0=decryption
void DES_ecb3_encrypt_evp(u_char const *input,
                          u_char *output,
                          u_char const *key1,
                          u_char const *key2,
                          u_char const *key3,
                          int32_t enc) noexcept;
/// @brief AES-CFB1 encryption/decryption function (EVP implementation, does not rely on AES_KEY)
/// @param in: Input data
/// @param out: Output buffer (must be pre-allocated with sufficient space)
/// @param length: Input data length (bytes)
/// @param key: Key data (16/24/32 bytes, corresponding to AES-128/192/256)
/// @param keyLen: Key length (16/24/32)
/// @param ivec: Initialization vector (16 bytes, updated internally)
/// @param num: Tracks the processed bit count (0-7)
/// @param enc: 1=encryption, 0=decryption
void AES_cfb1_encrypt_evp(u_char const *in,
                          u_char *out,
                          int64_t length,
                          u_char const *key,
                          int32_t keyLen,
                          u_char *ivec,
                          int32_t *num,
                          const int32_t enc) noexcept;
/// @brief EVP version of AES_cfb8_encrypt (fully equivalent)
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param length Data length (bytes)
/// @param key AES key (16 bytes, including parity bits)
/// @param keyLen AES key length
/// @param ivec Initialization vector (8 bytes), input/output
/// @param num Indicates the number of bytes already used in the current keystream block
/// @param enc Encryption mode: 1 (DES_ENCRYPT) or 0 (DES_DECRYPT)
void AES_cfb8_encrypt_evp(u_char const *in,
                          u_char *out,
                          size_t length,
                          u_char const *key,
                          int32_t keyLen,
                          u_char *ivec,
                          int32_t *num,
                          const int32_t enc) noexcept;
/// @brief EVP version of AES_cfb128_encrypt (fully equivalent)
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param length Data length (bytes)
/// @param key AES key (16 bytes, including parity bits)
/// @param keyLen AES key length
/// @param ivec Initialization vector (8 bytes), input/output
/// @param num Indicates the number of bytes already used in the current keystream block
/// @param enc Encryption mode: 1 (DES_ENCRYPT) or 0 (DES_DECRYPT)
void AES_cfb128_encrypt_evp(u_char const *in,
                            u_char *out,
                            size_t length,
                            u_char const *key,
                            int32_t keyLen,
                            u_char *ivec,
                            int32_t *num,
                            const int32_t enc) noexcept;
/// @brief EVP version of AES_ofb128_encrypt (fully equivalent)
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param length Data length (bytes)
/// @param key AES key (16 bytes, including parity bits)
/// @param keyLen AES key length
/// @param ivec Initialization vector (8 bytes), input/output
/// @param num Indicates the number of bytes already used in the current keystream block
void AES_ofb128_encrypt_evp(u_char const *in,
                            u_char *out,
                            size_t length,
                            u_char const *key,
                            int32_t keyLen,
                            u_char *ivec,
                            int32_t *num) noexcept;
/// @brief EVP version of DES_cfb_encrypt (fully equivalent)
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param numbits Number of feedback bits: 1, 8, or 64
/// @param length Data length (bytes)
/// @param key DES key (8 bytes, including parity bits)
/// @param ivec Initialization vector (8 bytes), input/output
void DES_cfb_encrypt_evp(u_char const *in,
                         u_char *out,
                         int32_t numbits,
                         int64_t length,
                         u_char const *key,
                         u_char *ivec,
                         int32_t const enc) noexcept;
/// @brief Equivalent to DES_cfb64_encrypt
/// @param in Input data
/// @param out Output data
/// @param length Data length (bytes)
/// @param key DES key (8 bytes)
/// @param ivec Initialization vector (8 bytes), input/output
/// @param num Indicates the number of bytes already used in the current keystream block
void DES_cfb64_encrypt_evp(u_char const *in,
                           u_char *out,
                           int64_t length,
                           u_char const *key,
                           u_char *ivec,
                           int32_t *num,
                           int32_t const enc) noexcept;
/// @brief Equivalent to DES_ofb_encrypt(in, out, numbits, length, schedule, ivec)
/// @param in Input data
/// @param out Output data
/// @param numbits Number of feedback bits: 1 or 8
/// @param length Data length (bytes)
/// @param key DES key (8 bytes)
/// @param ivec Initialization vector (8 bytes), input/output
void DES_ofb_encrypt_evp(
    u_char const *in, u_char *out, int32_t numbits, int64_t length, u_char const *key, u_char *ivec) noexcept;
/// @brief DES OFB64 mode encryption/decryption (equivalent to DES_ofb64_encrypt)
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param length Data length (bytes)
/// @param key DES key (8 bytes)
/// @param ivec Initialization vector (8 bytes) Input: initial IV Output: updated keystream block (can be used for next call)
/// @param num Indicates the number of bytes already used in the current keystream block
void DES_ofb64_encrypt_evp(
    u_char const *in, u_char *out, int64_t length, u_char const *key, u_char *ivec, int32_t *num) noexcept;
/// @brief 3DES EDE CFB mode encryption/decryption (three independent key version) equivalent to DES_ede3_cfb_encrypt()
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param numbits Number of feedback bits (recommended 8 or 64)
/// @param length Data length (bytes)
/// @param key1 First DES key (8 bytes)
/// @param key2 Second DES key (8 bytes)
/// @param key3 Third DES key (8 bytes)
/// @param ivec Initialization vector (8 bytes) Input: initial IV Output: updated feedback register (for next call)
/// @param enc Encryption mode: 1 (DES_ENCRYPT) or 0 (DES_DECRYPT)
void DES_ede3_cfb_encrypt_evp(u_char const *in,
                              u_char *out,
                              int32_t numbits,
                              int64_t length,
                              u_char const *key1,
                              u_char const *key2,
                              u_char const *key3,
                              u_char *ivec,
                              int32_t enc) noexcept;
/// @brief 3DES EDE CFB64 mode encryption/decryption (three-key interface, supports state continuation) DES_ede3_cfb64_encrypt()
/// @param in Input data
/// @param out Output data
/// @param length Data length (bytes)
/// @param k1 First DES key (8 bytes)
/// @param k2 Second DES key (8 bytes)
/// @param k3 Third DES key (8 bytes)
/// @param ivec IV (8 bytes), input/output
/// @param num Current byte offset (0~7), input/output
/// @param enc Encryption mode: 1 (DES_ENCRYPT) or 0 (DES_DECRYPT)
void DES_ede3_cfb64_encrypt_evp(u_char const *in,
                                u_char *out,
                                int64_t length,
                                u_char const *k1,
                                u_char const *k2,
                                u_char const *k3,
                                u_char *ivec,
                                int32_t *num,
                                int32_t enc) noexcept;
/// @brief Equivalent to DES_ede3_ofb64_encrypt() 3DES EDE OFB64 mode encryption/decryption (supports state continuation). OFB mode uses the same logic for encryption and decryption, no enc parameter needed.
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param length Data length (bytes)
/// @param k1 First DES key (8 bytes)
/// @param k2 Second DES key (8 bytes)
/// @param k3 Third DES key (8 bytes)
/// @param ivec Initialization vector (8 bytes) Input: current IV Output: updated keystream block (new IV)
/// @param num Current byte offset (0~7), input/output parameter for multi-call continuous encryption/decryption
void DES_ede3_ofb64_encrypt_evp(u_char const *in,
                                u_char *out,
                                int64_t length,
                                u_char const *k1,
                                u_char const *k2,
                                u_char const *k3,
                                u_char *ivec,
                                int32_t *num) noexcept;

#endif
/// @brief EVP version of DES_ede3_cbc_encrypt
/// @param input Input data (plaintext or ciphertext)
/// @param output Output data (ciphertext or plaintext)
/// @param length Data length (must be a multiple of 8)
/// @param ks1 First DES key (8 bytes)
/// @param ks2 Second DES key (8 bytes)
/// @param ks3 Third DES key (8 bytes)
/// @param ivec IV (input: initial IV, output: updated IV)
/// @param enc 1=encryption, 0=decryption
void DES_ede3_cbc_encrypt_evp(u_char const *input,
                              u_char *output,
                              int64_t length,
                              void *ks1,
                              void *ks2,
                              void *ks3,
                              u_char *ivec,
                              int32_t enc) noexcept;
/// @brief Use EVP encryption series functions
/// @param in Input
/// @param out Output
/// @param original_key Key
/// @param key_bits Key length
void AES_encrypt_evp(u_char const *in, u_char *out, u_char const *originalKey, int32_t keyBits) noexcept;
/// @brief Use EVP decryption series functions
/// @param in Input
/// @param out Output
/// @param original_key Key
/// @param key_bits Key length
void AES_decrypt_evp(u_char const *in, u_char *out, u_char const *originalKey, int32_t keyBits) noexcept;
/// @brief Get EC_KEY from memory
/// @param bio ECC key memory data
/// @return EVP_PKEY
EVP_PKEY *PEM_read_bio_EC_PUBKEY_Compatible(BIO *bio) noexcept;
/// @brief Get EC key length
/// @param pkey Key containing ECC key
/// @return Key length, 0 on failure
int32_t Get_EccKey_Size(EVP_PKEY *pkey) noexcept;
/// @brief Get RSA key length
/// @param pkey Key containing RSA key
/// @return 0 on failure, key length on success
int32_t Get_RsaKey_Size(EVP_PKEY *pkey) noexcept;
/// @brief Perform ECDH key exchange using new EVP API
/// @param shared_secret Shared secret data
/// @param secret_len Shared secret data length
/// @param peer_key Peer key
/// @param my_key My key
/// @return 0 on failure, 1 on success
int32_t Ecdh_key_exchange(u_char *sharedSecret, size_t *secretLen, EVP_PKEY *peerKey, EVP_PKEY *myKey) noexcept;
/// @brief Generate RSA key pair (using default public exponent 65537)
/// @param bits Key length (2048 or 4096)
/// @return Generated EVP_PKEY pointer, returns nullptr on failure
EVP_PKEY *Generate_rsa_evp_key(int32_t bits) noexcept;
/// @brief Generate ECDSA signature (using EVP interface)
/// @param priv_key Private key
/// @param data Input data
/// @param data_len Input data length
/// @param signature Signature data
/// @param sig_len Signature data length
/// @return 0 on failure, 1 on success
int32_t Ecdsa_sign(EVP_PKEY *privKey, u_char const *data, size_t dataLen, u_char *signature, size_t *sigLen) noexcept;
/// @brief Verify ECDSA signature (replacement for ECDSA_verify)
/// @param pub_key Public key
/// @param data Input data
/// @param data_len Input data length
/// @param signature Signature data
/// @param sig_len Signature data length
/// @return 0 on failure, 1 on success
int32_t Ecdsa_verify(
    EVP_PKEY *pubKey, u_char const *data, size_t dataLen, u_char const *signature, size_t sigLen) noexcept;
/// @brief Implement PSS-padded signature using EVP interface (replacement for RSA_padding_add_PKCS1_PSS + private key encryption)
/// @param pkey EVP_PKEY object containing RSA private key
/// @param out Output buffer (signature result)
/// @param outlen Output buffer size / actual signature length
/// @param digest Hash algorithm (e.g., EVP_sha256())
/// @param salt_len PSS salt length (usually equal to hash length)
/// @param in Input data (data to be signed)
/// @param inlen Input data length
/// @return 1 on success, 0 on failure
int32_t RSA_padding_add_PKCS1_PSS_evp(EVP_PKEY *pkey,
                                      u_char *out,
                                      size_t *outlen,
                                      const EVP_MD *digest,
                                      int32_t saltLen,
                                      u_char const *in,
                                      size_t inlen) noexcept;
/// @brief Verify PSS signature (corresponding to signature verification flow)
/// @param pkey EVP_PKEY object containing RSA public key
/// @param sig Signature data
/// @param siglen Signature data length
/// @param digest Hash algorithm
/// @param salt_len Salt length
/// @param in Digest data
/// @param inlen Digest data length
/// @return 0 on failure, 1 on success
int32_t RSA_verify_PKCS1_PSS_evp(EVP_PKEY *pkey,
                                 u_char const *in,
                                 size_t inlen,
                                 const EVP_MD *digest,
                                 int32_t saltLen,
                                 u_char const *sig,
                                 size_t siglen) noexcept;
/// @brief Implement RSA public key encryption using EVP_PKEY interface (replacement for RSA_public_encrypt)
/// @param pkey EVP_PKEY object containing RSA public key
/// @param out Output buffer (encryption result)
/// @param in Input plaintext data
/// @param inlen Input data length
/// @param padding Padding mode (compatible with RSA_public_encrypt)
/// @return Encrypted length on success, -1 on failure
int32_t Rsa_public_encrypt_evp(EVP_PKEY *pkey, u_char *out, u_char const *in, size_t inlen, int32_t padding) noexcept;
/// @brief Implement RSA private key decryption using EVP_PKEY interface (replacement for RSA_private_decrypt)
/// @param pkey EVP_PKEY object containing RSA private key
/// @param out Output buffer (decryption result)
/// @param in Input ciphertext data
/// @param inlen Input data length
/// @param padding Padding mode (must match the one used for encryption)
/// @return Decrypted length on success, -1 on failure
int32_t Rsa_private_decrypt_evp(EVP_PKEY *pkey, u_char *out, u_char const *in, size_t inlen, int32_t padding) noexcept;
/// @brief Manually implement RSA private key signature (compatible with RSA_private_encrypt)
/// @param in Input data (plaintext)
/// @param in_len Input length
/// @param out Output buffer (stores signature result)
/// @param pkey EVP_PKEY private key (must be RSA type)
/// @param padding Padding mode (RSA_PKCS1_PADDING or RSA_NO_PADDING)
/// @return Signature length on success, -1 on failure
int32_t Rsa_private_encrypt_evp(const u_char *in, size_t inLen, u_char *out, EVP_PKEY *pkey, int32_t padding) noexcept;
/// @brief Manually implement RSA public key decryption (compatible with the signature logic of RSA_private_encrypt)
/// @param in Input data (ciphertext or signature result)
/// @param in_len Input length (must equal RSA key length)
/// @param out Output buffer (stores recovered plaintext)
/// @param pkey EVP_PKEY public key (must be RSA type)
/// @param padding Padding mode (RSA_PKCS1_PADDING or RSA_NO_PADDING)
/// @return Plaintext length on success, -1 on failure
int32_t Rsa_public_decrypt_evp(const u_char *in, size_t inLen, u_char *out, EVP_PKEY *pkey, int32_t padding) noexcept;
/// @brief Select the corresponding elliptic curve based on platform and key length (128/192/256 bits)
/// @param security_bits Key length (only supports 128, 192, 256)
/// @return Curve name, returns null on failure
char const *Get_curve_name(int32_t keyBits, cryp::isoft_def::EccForPlatform eccForPlatform) noexcept;
/// @brief Generate ECC key according to specified security strength
/// @param security_bits Security strength (128/192/256)
/// @return Generated EVP_PKEY pointer, returns NULL on failure
EVP_PKEY *Generate_ecc_key(int32_t const keyLen,
                           cryp::isoft_def::EccForPlatform platform = cryp::isoft_def::EccForPlatform::kPuhua) noexcept;
/// @brief Check RSA key validity
/// @param pkey RSA key
/// @return 1: key valid, 0: key invalid, -1: error during check (e.g., memory allocation failure, context error)
int32_t Rsa_check_key_evp(EVP_PKEY *pkey) noexcept;
/// @brief AES general wrapping function
/// @param cipher Wrapping algorithm
/// @param init_func Initialization function
/// @param update_func Update function
/// @param final_func Finalization function
/// @param key Key
/// @param iv_or_icv Initialization vector
/// @param out Output result
/// @param in Input
/// @param inlen Input length
/// @param min_inlen Minimum input length
/// @param custom_check Length check function
/// @return Output length, 0 indicates failure
size_t Aes_wrap_operation_evp(
    const EVP_CIPHER *cipher,
    int32_t (*initFunc)(EVP_CIPHER_CTX *, const EVP_CIPHER *, ENGINE *, u_char const *, u_char const *),
    int32_t (*updateFunc)(EVP_CIPHER_CTX *, u_char *, int32_t *, u_char const *, int32_t),
    int32_t (*finalFunc)(EVP_CIPHER_CTX *, u_char *, int32_t *),
    u_char const *key,
    u_char const *ivOrIcv,
    u_char *out,
    u_char const *in,
    size_t inlen,
    size_t minInlen,
    int32_t (*customCheck)(size_t)) noexcept;
/// @brief Unpadded wrapping: custom length check
/// @param inlen Length
/// @return true if valid false invalid
int32_t Check_wrap_length(size_t inlen) noexcept;
/// @brief Unpadded unwrapping: custom length check (multiple of 16 bytes and ≥24)
/// @param inlen Length
/// @return true if valid false invalid
int32_t Check_unwrap_length(size_t inlen) noexcept;
/// @brief Unpadded wrapping implementation
/// @param key Key
/// @param iv Initialization vector
/// @param out Output
/// @param in Input
/// @param inlen Input length
/// @return Output length, 0 indicates failure
size_t Aes_128_wrap_evp(u_char const *key, u_char const *iv, u_char *out, u_char const *in, size_t inlen) noexcept;
/// @brief Unpadded unwrapping implementation
/// @param key Key
/// @param iv Initialization vector
/// @param out Output
/// @param in Input
/// @param inlen Input length
/// @return Output length, 0 indicates failure
size_t Aes_128_unwrap_evp(u_char const *key, u_char const *iv, u_char *out, u_char const *in, size_t inlen) noexcept;
/// @brief Padded wrapping implementation (no custom check, any length ≥1)
/// @param key Key
/// @param icv Initialization vector
/// @param out Output
/// @param in Input
/// @param inlen Input length
/// @return Output length, 0 indicates failure
size_t Aes_128_wrap_pad_evp(u_char const *key, u_char const *icv, u_char *out, u_char const *in, size_t inlen) noexcept;
/// @brief Padded unwrapping implementation (no custom check, any length ≥16)
/// @param key Key
/// @param icv Initialization vector
/// @param out Output
/// @param in Input
/// @param inlen Input length
/// @return Output length, 0 indicates failure
size_t Aes_128_unwrap_pad_evp(
    u_char const *key, u_char const *icv, u_char *out, u_char const *in, size_t inlen) noexcept;
/// @brief AES wrapping: key_unpad
/// @param key AES_KEY key resource
/// @param iv Initialization vector
/// @param out Output buffer
/// @param in Input data buffer
/// @param inlen Input data length
/// @return 0 sucess，fail otherwise
std::size_t AES_wrap_key_unpad(u_char const *key,
                               u_char const *const iv,
                               u_char *const out,
                               u_char const *const in,
                               uint32_t const inlen,
                               size_t const keyLen = 0) noexcept;
/// @brief AES unwrapping: key_unpad
/// @param key AES_KEY key resource
/// @param iv Initialization vector
/// @param out Output buffer
/// @param in Input data buffer
/// @param inlen Input data length
/// @return 0 sucess，fail otherwise
std::size_t AES_unwrap_key_unpad(u_char const *key,
                                 u_char const *const iv,
                                 u_char *const out,
                                 u_char const *const in,
                                 uint32_t const inlen,
                                 size_t const keyLen = 0) noexcept;
/// @brief AES wrapping: key_pad
/// @param key AES_KEY key resource
/// @param iv Initialization vector
/// @param out Output buffer
/// @param in Input data buffer
/// @param inlen Input data length
/// @return 0 sucess，fail otherwise
std::size_t AES_wrap_key_pad(u_char const *key,
                             u_char const *const iv,
                             u_char *const out,
                             u_char const *const in,
                             uint32_t const inlen,
                             size_t const keyLen = 0) noexcept;
/// @brief AES unwrapping: key_pad
/// @param key AES_KEY key resource
/// @param iv Initialization vector
/// @param out Output buffer
/// @param in Input data buffer
/// @param inlen Input data length
/// @return 0 sucess，fail otherwise
std::size_t AES_unwrap_key_pad(u_char const *key,
                               u_char const *const iv,
                               u_char *const out,
                               u_char const *const in,
                               uint32_t const inlen,
                               size_t const keyLen = 0) noexcept;
/// @brief Reverse buffer content
/// @param out Output buffer
/// @param in Input data buffer
/// @param size Input data length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00157
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
void Reverse_Buff(uint8_t *out, uint8_t const *const in, size_t const size) noexcept;
/// @brief DES unwrapping: key
/// @param ks1 DES key 1
/// @param ks2 DES key 2
/// @param ks3 DES key 3
/// @param out Output buffer
/// @param in Input data buffer
/// @param inl Input data length
/// @return 0 sucess，fail otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00160
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
int32_t DES_unwrap_key(
    void *ks1, void *ks2, void *ks3, uint8_t *const out, uint8_t const *in, size_t const inl) noexcept;
/// @brief DES wrapping: key
/// @param ks1 DES key 1
/// @param ks2 DES key 2
/// @param ks3 DES key 3
/// @param out Output buffer
/// @param in Input data buffer
/// @param inl Input data length
/// @return 0 sucess，fail otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00161
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
int32_t DES_wrap_key(
    void *ks1, void *ks2, void *ks3, uint8_t *const out, uint8_t const *const in, size_t const inl) noexcept;
/// @brief Simulate big number increment by 1
/// @param counter Counter
/// @param len Length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00158
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
inline void Increment_counter(uint8_t *const counter, int32_t const len) noexcept
{
    uint32_t carry{1U};  // Initial carry is 1 because we want to add 1 to the array
    for (int32_t i{len - kInt_1}; i >= 0; --i) {
        uint32_t const sum{*(counter + i) + carry};
        *(counter + i) = static_cast< uint8_t >(
            sum & 0xFFU);   // Keep only the lower 8 bits as the new value of the current element
        carry = sum >> 8U;  // Check if there is a carry (actually checks if sum > 255)
        if (carry == 0U) {
            break;
        }
    }

    if (carry != 0U) {
        std::ignore = memset(counter, 0, static_cast< size_t >(len));
    }
}
/// @brief AES string encryption
/// @param plaintext Plaintext data
/// @param startOffset Encryption start offset
/// @param plaintextLen Plaintext data length
/// @param iv Initialization vector
/// @param ciphertext Ciphertext data
/// @param aesKey AES key
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00159
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
void Aes_ctr_encrypt(uint8_t const *const plaintext,
                     int32_t const startOffset,
                     int32_t const plaintextLen,
                     uint8_t const *const iv,
                     uint8_t *const ciphertext,
                     const u_char *originalKey,
                     int32_t keyBits) noexcept;
/// @brief Private key decryption, replacement for RSA_private_decrypt
/// @param flen Input length
/// @param from Input content
/// @param to Output content
/// @param rsa Private key
/// @param padding Whether to use padding
/// @return -1 on failure
int32_t Rsa_private_decrypt_evp(int32_t flen, const u_char *from, u_char *to, RSA *rsa, int32_t padding) noexcept;
/// @brief Replacement for rsa_public_encrypt, public key encryption
/// @param flen Input length
/// @param from Input content
/// @param to Output content
/// @param rsa Public key
/// @param padding Padding
/// @return -1 on failure
int32_t Rsa_public_encrypt_evp(int32_t flen, const u_char *from, u_char *to, RSA *rsa, int32_t padding) noexcept;
/// @brief Copy an RSA key
/// @param src
/// @param pri true: copy private key, false: copy public key
/// @return RSA key
EVP_PKEY *Evp_pkey_dup(EVP_PKEY *src, bool pri = true) noexcept;
/// @brief Convert the corresponding key data into the corresponding asymmetric structure (both public and private keys), such as RSA or ECC. The passed key type and method must match.
/// @param pKeyData Key data
/// @param keySize Key length
/// @param pemReadFun PEM format read function
/// @param derReadFun DER format read function
/// @return Public/private key object pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03027
/// @trace_id_dd=DD_CRYPTO_06013
/// @needwork = ad
/// @endcode
template < typename T >
static T *TransformKeyDataToRsaOrEccStruct(
    uint8_t const *const pKeyData,
    std::size_t const keySize,
    std::function< T *(BIO *, T **, pem_password_cb *, void *) > const &pemReadFun,
    std::function< T *(BIO *, T **) > const &derReadFun) noexcept
{
    if ((pKeyData == nullptr) || (keySize == 0U)) {
        return nullptr;
    }
    /// Two BIOs are used here because
    /// If the data is in PEM format, calling d2i_RSA_PUBKEY_bio and then PEM_read_bio_RSAPublicKey still cannot generate an RSA object, so they are separated
    BIO *const pKeyBioPem{BIO_new_mem_buf(pKeyData, static_cast< int32_t >(keySize))};
    BIO *const pKeyBioDer{BIO_new_mem_buf(pKeyData, static_cast< int32_t >(keySize))};

    void *pAsmkeyPair{nullptr};
    pAsmkeyPair
        = static_cast< void * >(pemReadFun(pKeyBioPem, reinterpret_cast< T ** >(&pAsmkeyPair), nullptr, nullptr));
    if (pAsmkeyPair
        == nullptr) {  /// Indicates that the data currently stored in the slot may be in DER format, not PEM format
        pAsmkeyPair = static_cast< void * >(derReadFun(pKeyBioDer, reinterpret_cast< T ** >(&pAsmkeyPair)));
    }

    std::ignore = BIO_free(pKeyBioDer);
    std::ignore = BIO_free(pKeyBioPem);
    return static_cast< T * >(static_cast< void * >(pAsmkeyPair));
}
/// @brief Convert the corresponding key data into the corresponding asymmetric structure (both public and private keys), such as RSA or ECC. The passed key type and method must match.
/// @param pKeyData Key data
/// @param keySize Key length
/// @param rsa_or_ecc   true rsa_key false ecc_key
/// @param pri_or_pub   true private_key false public_key
/// @return EVP_PKEY type key
inline EVP_PKEY *TransfromToEvpKey(uint8_t const *const pKeyData,
                                   std::size_t const keySize,
                                   bool rsaOrEcc,
                                   bool priOrPub) noexcept
{
    if ((pKeyData == nullptr) || (keySize == 0U)) {
        return nullptr;
    }

    EVP_PKEY *pAsmkeyPair{nullptr};

#if PUHUA_OPENSSL_IS_11X
    EVP_PKEY *pkey = EVP_PKEY_new();
    if (rsaOrEcc) {
        RSA *pRsaKey = nullptr;
        if (priOrPub) {
            /// OpenSSL 1.1.1 supports both PKCS#8 and PKCS#1
            pRsaKey = TransformKeyDataToRsaOrEccStruct< RSA >(pKeyData, keySize, &PEM_read_bio_RSAPrivateKey,
                                                              &d2i_RSAPrivateKey_bio);
            if (pRsaKey == nullptr) {
                /// Most likely will not reach here
                pAsmkeyPair = TransformKeyDataToRsaOrEccStruct< EVP_PKEY >(pKeyData, keySize, &PEM_read_bio_PrivateKey,
                                                                           &d2i_PrivateKey_bio);
            }
        } else {
            pRsaKey = TransformKeyDataToRsaOrEccStruct< RSA >(pKeyData, keySize, &PEM_read_bio_RSAPublicKey,
                                                              &d2i_RSAPublicKey_bio);
            if (pRsaKey == nullptr) {
                /// Will reach here
                pAsmkeyPair = TransformKeyDataToRsaOrEccStruct< EVP_PKEY >(pKeyData, keySize, &PEM_read_bio_PUBKEY,
                                                                           &d2i_PUBKEY_bio);
            }
        }
        if (pAsmkeyPair != nullptr) {
            EVP_PKEY_free(pkey);
            return pAsmkeyPair;
        }

        if (EVP_PKEY_assign_RSA(pkey, pRsaKey) != 1) {  // NOLINT
            EVP_PKEY_free(pkey);
            RSA_free(pRsaKey);  // Need to free if association fails
        }
    } else {
        EC_KEY *pEcKey = nullptr;
        if (priOrPub) {  /// OpenSSL 1.1.1 supports both PKCS#8 and PKCS#1
            pEcKey = TransformKeyDataToRsaOrEccStruct< EC_KEY >(pKeyData, keySize, &PEM_read_bio_ECPrivateKey,
                                                                &d2i_ECPrivateKey_bio);
            if (pEcKey == nullptr) {
                /// Most likely will not reach here
                pAsmkeyPair = TransformKeyDataToRsaOrEccStruct< EVP_PKEY >(pKeyData, keySize, &PEM_read_bio_PrivateKey,
                                                                           &d2i_PrivateKey_bio);
            }
        } else {
            pEcKey = TransformKeyDataToRsaOrEccStruct< EC_KEY >(pKeyData, keySize, &PEM_read_bio_EC_PUBKEY,
                                                                &d2i_EC_PUBKEY_bio);
            if (pEcKey == nullptr) {
                /// Most likely will not reach here
                pAsmkeyPair = TransformKeyDataToRsaOrEccStruct< EVP_PKEY >(pKeyData, keySize, &PEM_read_bio_PUBKEY,
                                                                           &d2i_PUBKEY_bio);
            }
        }
        if (pAsmkeyPair != nullptr) {
            EVP_PKEY_free(pkey);
            return pAsmkeyPair;
        }
        if (EVP_PKEY_assign_EC_KEY(pkey, pEcKey) != 1) {  // NOLINT
            // Association failed
            EVP_PKEY_free(pkey);
            EC_KEY_free(pEcKey);  // If eckey is no longer needed
        }
    }
    pAsmkeyPair = Evp_pkey_dup(pkey, priOrPub);
#else
    std::ignore = rsaOrEcc;
    if (priOrPub) {
        pAsmkeyPair = TransformKeyDataToRsaOrEccStruct< EVP_PKEY >(pKeyData, keySize, &PEM_read_bio_PrivateKey,
                                                                   &d2i_PrivateKey_bio);
    } else {
        pAsmkeyPair
            = TransformKeyDataToRsaOrEccStruct< EVP_PKEY >(pKeyData, keySize, &PEM_read_bio_PUBKEY, &d2i_PUBKEY_bio);
    }
#endif

    return pAsmkeyPair;
}

}  // namespace crypto
}  // namespace ara

#endif