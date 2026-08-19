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
/// @file       isoft_openssl_encrypt.cpp
/// @brief
/// @details
/// @date       2024-07-15
/// @author     Che Jinzhao
/// @version    1.2.0
///
/// ================================================================

#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {

#if PUHUA_OPENSSL_IS_11X
/// @brief Copy ECC public key (without private key)
/// @param src Source ECC key (must contain public key)
/// @return New EVP_PKEY containing only public key on success, nullptr on failure
EVP_PKEY *Evp_pkey_ecc_dup_pub(EVP_PKEY *src) noexcept
{
    if (src == nullptr || EVP_PKEY_id(src) != EVP_PKEY_EC) {
        fprintf(stderr, "Source key is invalid or not ECC type\n");
        return nullptr;
    }

    // Extract EC_KEY and core data from source key
    const EC_KEY *srcEc      = EVP_PKEY_get0_EC_KEY(src);
    const EC_GROUP *srcRroup = EC_KEY_get0_group(srcEc);
    const EC_POINT *srcPub   = EC_KEY_get0_public_key(srcEc);
    if (srcEc == nullptr || srcRroup == nullptr || srcPub == nullptr) {
        fprintf(stderr, "Source key public key data incomplete\n");
        return nullptr;
    }

    // Create new EC_GROUP (copy curve parameters)
    EC_GROUP *dupGroup = EC_GROUP_dup(srcRroup);
    if (dupGroup == nullptr) {
        fprintf(stderr, "Failed to copy curve parameters\n");
        return nullptr;
    }

    // Create new EC_POINT (copy public key point)
    EC_POINT *dupPub = EC_POINT_dup(srcPub, dupGroup);
    if (dupPub == nullptr) {
        fprintf(stderr, "Failed to copy public key point\n");
        EC_GROUP_free(dupGroup);
        return nullptr;
    }

    // Create new EC_KEY (public key only)
    EC_KEY *dupEc = EC_KEY_new();
    if (dupEc == nullptr) {
        fprintf(stderr, "Failed to create EC_KEY\n");
        EC_POINT_free(dupPub);
        EC_GROUP_free(dupGroup);
        return nullptr;
    }

    // Bind curve and public key to new EC_KEY
    if (EC_KEY_set_group(dupEc, dupGroup) != 1 || EC_KEY_set_public_key(dupEc, dupPub) != 1) {
        fprintf(stderr, "Failed to bind public key to EC_KEY\n");
        EC_KEY_free(dupEc);
        EC_POINT_free(dupPub);
        EC_GROUP_free(dupGroup);
        return nullptr;
    }

    // Release temporary resources (EC_KEY takes ownership of group and pub)
    EC_GROUP_free(dupGroup);
    EC_POINT_free(dupPub);

    // Wrap as EVP_PKEY
    EVP_PKEY *dupPkey = EVP_PKEY_new();
    if (dupPkey == nullptr || EVP_PKEY_set1_EC_KEY(dupPkey, dupEc) != 1) {
        fprintf(stderr, "Failed to bind EC_KEY to EVP_PKEY\n");
        EVP_PKEY_free(dupPkey);
        EC_KEY_free(dupEc);
        return nullptr;
    }
    EC_KEY_free(dupEc);  // Release local reference

    return dupPkey;
}
/// @brief Copy ECC private key (including corresponding public key)
/// @param src Source ECC key (must contain private key)
/// @return New EVP_PKEY containing private and public key on success, nullptr on failure
EVP_PKEY *Evp_pkey_ecc_dup_priv(EVP_PKEY *src) noexcept
{
    if (src == nullptr || EVP_PKEY_id(src) != EVP_PKEY_EC) {
        fprintf(stderr, "Source key is invalid or not ECC type\n");
        return nullptr;
    }

    // Extract EC_KEY and core data from source key
    const EC_KEY *srcEc = EVP_PKEY_get0_EC_KEY(src);

    const EC_GROUP *srcGroup = EC_KEY_get0_group(srcEc);
    const EC_POINT *srcPub   = EC_KEY_get0_public_key(srcEc);
    const BIGNUM *srcPriv    = EC_KEY_get0_private_key(srcEc);
    if (srcGroup == nullptr || srcPub == nullptr || srcPriv == nullptr) {
        fprintf(stderr, "Source key private key data incomplete\n");
        return nullptr;
    }

    // Copy curve parameters, public key point, private key
    EC_GROUP *dupGroup = EC_GROUP_dup(srcGroup);
    EC_POINT *dupPub   = EC_POINT_dup(srcPub, srcGroup);
    BIGNUM *dupPriv    = BN_dup(srcPriv);
    if (dupGroup == nullptr || dupPub == nullptr || dupPriv == nullptr) {
        fprintf(stderr, "Failed to copy private key data\n");
        EC_GROUP_free(dupGroup);
        EC_POINT_free(dupPub);
        BN_free(dupPriv);
        return nullptr;
    }

    // Create new EC_KEY (containing private and public key)
    EC_KEY *dupEc = EC_KEY_new();
    if (dupEc == nullptr) {
        fprintf(stderr, "Failed to create EC_KEY\n");
        EC_GROUP_free(dupGroup);
        EC_POINT_free(dupPub);
        BN_free(dupPriv);
        return nullptr;
    }

    // Bind curve, public key, private key to new EC_KEY
    if (EC_KEY_set_group(dupEc, dupGroup) != 1 || EC_KEY_set_public_key(dupEc, dupPub) != 1
        || EC_KEY_set_private_key(dupEc, dupPriv) != 1) {
        fprintf(stderr, "Failed to bind private key to EC_KEY\n");
        EC_KEY_free(dupEc);
        EC_GROUP_free(dupGroup);
        EC_POINT_free(dupPub);
        BN_free(dupPriv);
        return nullptr;
    }

    // Release temporary resources
    EC_GROUP_free(dupGroup);
    EC_POINT_free(dupPub);
    BN_free(dupPriv);

    // Wrap as EVP_PKEY
    EVP_PKEY *dupPkey = EVP_PKEY_new();
    if (dupPkey == nullptr || EVP_PKEY_set1_EC_KEY(dupPkey, dupEc) != 1) {
        fprintf(stderr, "Failed to bind EC_KEY to EVP_PKEY\n");
        EVP_PKEY_free(dupPkey);
        EC_KEY_free(dupEc);
        return nullptr;
    }
    EC_KEY_free(dupEc);  // Release local reference

    return dupPkey;
}
/// @brief rsa sve_recover
/// @param rsa RSA resource pointer
/// @param out Output buffer
/// @param outlen Output buffer length
/// @param in Input data buffer
/// @param inlen Input data length
/// @return 0 sucess，fail otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00167
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
int32_t Rsasve_recover_evp(
    EVP_PKEY *const pkey, u_char *const out, size_t *const outlen, u_char const *const in, size_t const inlen) noexcept
{
    size_t nlen{0U};

    // Check if key type is RSA
    if (EVP_PKEY_base_id(pkey) != EVP_PKEY_RSA) {
        return 0;
    }

    // Get RSA structure from EVP_PKEY
    RSA *rsa = EVP_PKEY_get0_RSA(pkey);
    if (rsa == nullptr) {
        return 0;
    }

    /* Step (kInt_1): get the byte length of n */
    nlen = static_cast< size_t >(RSA_size(rsa));

    if (out == nullptr) {
        if (nlen == 0U) {
            return 0;
        }
        *outlen = nlen;
        return kInt_1;
    }

    /* Step (2): check the input ciphertext 'inlen' matches the nlen */
    if (inlen != nlen) {
        return 0;
    }
    /* Step (3): out  RSADP n,d, in */
    int32_t const ret{RSA_private_decrypt(static_cast< int32_t >(inlen), in, out, rsa, RSA_NO_PADDING)};
    *outlen = static_cast< size_t >(ret);
    return ret;
}
/// @brief rsa sve_gen_rand_bytes
/// @param rsaPub RSA resource pointer
/// @param out Output buffer
/// @param outlen Output buffer length
/// @return 0 sucess，fail otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00168
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
int32_t Rsasve_gen_rand_bytes(RSA *const rsaPub, u_char *const out, int32_t const outlen) noexcept
{
    int32_t ret{0};
    BN_CTX *bnctx{nullptr};
    BIGNUM *z{nullptr};
    BIGNUM *nminus3{nullptr};

    bnctx = BN_CTX_new();
    if (bnctx == nullptr) {
        return 0;
    }

    /*
     * Generate a random in the range kInt_1 < z < (n – kInt_1).
     * Since BN_priv_rand_range_ex() returns a value in range 0 <= r < max
     * We can achieve this by adding 2.. but then we need to subtract 3 from
     * the upper bound i.e: 2 + (0 <= r < (n - 3))
     */
    BN_CTX_start(bnctx);
    nminus3 = BN_CTX_get(bnctx);
    z       = BN_CTX_get(bnctx);
    do {
        if (z == nullptr) {
            ret = 0;
            break;
        }
        if (BN_copy(nminus3, RSA_get0_n(rsaPub)) == nullptr) {
            ret = 0;
            break;
        }
        if (BN_sub_word(nminus3, kInt_3U) == 0) {
            ret = 0;
            break;
        }
        if (BN_priv_rand_range(z, nminus3) == 0) {
            ret = 0;
            break;
        }
        if (BN_add_word(z, kInt_2U) == 0) {
            ret = 0;
            break;
        }
        if (BN_bn2binpad(z, out, outlen) != outlen) {
            ret = 0;
            break;
        }
        ret = kInt_1;
    } while (false);

    BN_CTX_end(bnctx);
    BN_CTX_free(bnctx);
    return ret;
}
/// @brief rsa sve_generate
/// @param rsa RSA resource pointer
/// @param out Output buffer
/// @param outlen Output buffer length
/// @param secret Secret seed
/// @param secretlen Secret seed length
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
                            size_t *const secretlen) noexcept
{
    int32_t ret{0};
    size_t nlen{0U};

    // Check if key type is RSA
    if (EVP_PKEY_base_id(pkey) != EVP_PKEY_RSA) {
        return 0;
    }

    // Get RSA structure from EVP_PKEY
    RSA *rsa = EVP_PKEY_get0_RSA(pkey);
    if (rsa == nullptr) {
        return 0;
    }

    nlen = static_cast< std::size_t >(RSA_size(rsa));

    if (out == nullptr) {
        if (nlen == 0U) {
            return 0;
        }
        if ((outlen == nullptr) && (secretlen == nullptr)) {
            return 0;
        }
        if (outlen != nullptr) {
            *outlen = nlen;
        }
        if (secretlen != nullptr) {
            *secretlen = nlen;
        }
        return kInt_1;
    }

    /*
     * Step (2): Generate a random byte string z of nlen bytes where
     *            kInt_1 < z < n - kInt_1
     */
    if (Rsasve_gen_rand_bytes(rsa, secret, static_cast< int32_t >(nlen)) == 0) {
        return 0;
    }

    ret = RSA_public_encrypt(static_cast< int32_t >(nlen), secret, out, rsa, RSA_NO_PADDING);
    if (ret != 0) {
        ret = kInt_1;
        if (outlen != nullptr) {
            *outlen = nlen;
        }
        if (secretlen != nullptr) {
            *secretlen = nlen;
        }
    } else {
        OPENSSL_cleanse(secret, nlen);
    }
    return ret;
}
#else
/// @brief Generate random number z conforming to NIST.SP.800-56Br2 standard (1 < z < n-1)
/// @param pkey EVP_PKEY key (must be RSA type)
/// @param out Output buffer
/// @param outlen Output length (must equal RSA modulus length)
/// @return 1 on success, 0 on failure
int32_t Rsasve_gen_rand_bytes_evp(EVP_PKEY *pkey, u_char *out, int32_t outlen) noexcept
{
    // Get key type and verify it is RSA
    if (EVP_PKEY_base_id(pkey) != EVP_PKEY_RSA) {
        return 0;
    }

    // Get RSA modulus n via EVP parameter
    BIGNUM *n = BN_new();
    if (n == nullptr) {
        return 0;
    }

    // Use EVP_PKEY_get_bn_param to get modulus n
    if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_N, &n) <= 0) {
        BN_free(n);
        return 0;
    }

    // Verify output length matches modulus length
    int32_t nBytes = BN_num_bytes(n);
    if (outlen != nBytes) {
        BN_free(n);
        return 0;
    }

    // Try getting libctx via temporary ctx (more compatible way)
    OSSL_LIB_CTX *libctx = nullptr;
    EVP_PKEY_CTX *tmpCtx = EVP_PKEY_CTX_new(pkey, nullptr);
    libctx               = EVP_PKEY_CTX_get0_libctx(tmpCtx);  // Use EVP_PKEY_CTX_get0_libctx
    EVP_PKEY_CTX_free(tmpCtx);

    // Create secure BN context (using obtained libctx or NULL)
    BN_CTX *bnCtx = BN_CTX_secure_new_ex(libctx);
    if (bnCtx == nullptr) {
        BN_free(n);
        return 0;
    }

    // Initialize temporary BIGNUM
    BN_CTX_start(bnCtx);
    BIGNUM *nminus3 = BN_CTX_get(bnCtx);
    BIGNUM *z       = BN_CTX_get(bnCtx);

    // Check if temporary BIGNUM allocation succeeded
    if (nminus3 == nullptr || z == nullptr) {
        BN_CTX_end(bnCtx);
        BN_CTX_free(bnCtx);
        BN_free(n);
        return 0;
    }

    // Calculate nminus3 = n - 3
    if (BN_copy(nminus3, n) == nullptr || BN_sub_word(nminus3, 3) == 0) {
        BN_CTX_end(bnCtx);
        BN_CTX_free(bnCtx);
        BN_free(n);
        return 0;
    }

    // Generate random number 0 <= z < nminus3
    if (BN_priv_rand_range_ex(z, nminus3, 0, bnCtx) == 0) {
        BN_CTX_end(bnCtx);
        BN_CTX_free(bnCtx);
        BN_free(n);
        return 0;
    }

    // Adjust range to 2 <= z < n-1 (i.e., 1 < z < n-1)
    if (BN_add_word(z, 2) == 0) {
        BN_CTX_end(bnCtx);
        BN_CTX_free(bnCtx);
        BN_free(n);
        return 0;
    }

    // Convert BIGNUM to binary and pad into output buffer
    if (BN_bn2binpad(z, out, outlen) != outlen) {
        BN_CTX_end(bnCtx);
        BN_CTX_free(bnCtx);
        BN_free(n);
        return 0;
    }

    // Clean up resources
    BN_CTX_end(bnCtx);
    BN_CTX_free(bnCtx);
    BN_free(n);
    return kInt_1;
}
/// @brief rsasve_generate
/// @param rsa RSA resource pointer
/// @param out Output buffer
/// @param outlen Output buffer length
/// @param secret Secret seed
/// @param secretlen Secret seed length
/// @return 0 fail 1 sucess
int32_t Rsasve_generate_evp(EVP_PKEY *const pkey,
                            u_char *const out,
                            size_t *const outlen,
                            u_char *const secret,
                            size_t *const secretlen) noexcept
{
    // Check key type (RSA only)
    if (EVP_PKEY_base_id(pkey) != EVP_PKEY_RSA) {
        return 0;
    }

    // Get RSA key length (bytes)
    size_t nlen = static_cast< size_t >(EVP_PKEY_size(pkey));
    if (nlen == 0) {
        return 0;
    }

    // Case of getting length only
    if (out == nullptr) {
        if (outlen != nullptr) {
            *outlen = nlen;
        }
        if (secretlen != nullptr) {
            *secretlen = nlen;
        }
        return 0;
    }

    // Generate random number z conforming to NIST standard (1 < z < n-1)
    if (Rsasve_gen_rand_bytes_evp(pkey, secret, static_cast< int32_t >(nlen)) == 0) {
        return 0;
    }

    // Create encryption context
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (ctx == nullptr) {
        OPENSSL_cleanse(secret, nlen);
        return 0;
    }

    // Initialize RSA public key encryption (no padding mode)
    if (EVP_PKEY_encrypt_init(ctx) <= 0 || EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_NO_PADDING) <= 0) {
        OPENSSL_cleanse(secret, nlen);
        EVP_PKEY_CTX_free(ctx);
        return 0;
    }

    // Perform public key encryption
    size_t encryptLen = nlen;
    if (EVP_PKEY_encrypt(ctx, out, &encryptLen, secret, nlen) <= 0) {
        OPENSSL_cleanse(secret, nlen);
        EVP_PKEY_CTX_free(ctx);
        return 0;
    }

    // Success handling
    if (outlen != nullptr) {
        *outlen = encryptLen;
    }
    if (secretlen != nullptr) {
        *secretlen = nlen;
    }

    // Clean up resources
    EVP_PKEY_CTX_free(ctx);
    return kInt_1;
}
/// @brief rsasve_recover_evp
/// @param rsa RSA resource pointer
/// @param out Output buffer
/// @param outlen Output buffer length
/// @param in Input data buffer
/// @param inlen Input data length
int32_t Rsasve_recover_evp(
    EVP_PKEY *const pkey, u_char *const out, size_t *const outlen, u_char const *const in, size_t const inlen) noexcept
{
    // Check if key type is RSA
    if (EVP_PKEY_base_id(pkey) != EVP_PKEY_RSA) {
        return 0;
    }

    // Get RSA modulus length (bytes)
    size_t nlen = static_cast< size_t >(EVP_PKEY_size(pkey));
    if (nlen == 0) {
        return 0;
    }

    // Case of getting output length only (when out is nullptr)
    if (out == nullptr) {
        return 0;
    }

    // Verify input length matches modulus length (NIST standard requirement)
    if (inlen != nlen) {
        return 0;
    }

    // Create private key decryption context
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (ctx == nullptr) {
        return 0;
    }

    // Initialize private key decryption operation (no padding mode, consistent with original function)
    if (EVP_PKEY_decrypt_init(ctx) <= 0 || EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_NO_PADDING) <= 0) {
    } else {
        // Perform private key decryption (corresponding to RSADP operation in original function)
        size_t decryptLen = nlen;
        if (EVP_PKEY_decrypt(ctx, out, &decryptLen, in, inlen) > 0) {
            *outlen = decryptLen;
        }
    }

    // Clean up resources
    EVP_PKEY_CTX_free(ctx);
    return 1;
}
/// @brief Generic EVP encrypt/decrypt function
/// @param in Input data (length must be multiple of AES_BLOCK_SIZE)
/// @param out Output buffer (pre-allocated with sufficient space)
/// @param key Key data (length must be 16/24/32 bytes, corresponding to AES-128/192/256)
/// @param cipher Encryption algorithm
/// @param enc 1=Encrypt, 0=Decrypt
/// @param update_len Encryption length
/// @param setoffset Whether to set offset
/// @param num Track processed bit count (0-7)
void Encrypt_evp_common(u_char const *input,
                        u_char *output,
                        u_char const *key,
                        EVP_CIPHER const *cipher,
                        const int32_t enc,
                        int32_t length,
                        u_char *ivec,
                        int32_t *num) noexcept
{
    if (input == nullptr || output == nullptr || cipher == nullptr || key == nullptr) {
        return;
    }

    // Create and initialize EVP context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        return;
    }

    // Initialize encrypt/decrypt context: use DES-CBC algorithm, specify key, IV, and direction
    if (EVP_CipherInit_ex(ctx, cipher, nullptr, key, ivec, enc) <= 0) {
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    // Disable padding
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    int32_t outLen = 0;
    // Perform actual encrypt/decrypt operation
    if (EVP_CipherUpdate(ctx, output, &outLen, input, static_cast< int32_t >(length)) <= 0) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    // Verify output length matches input length (since padding disabled, lengths should be equal)
    if (outLen != length) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    // Finalization (Final produces no extra data when padding disabled, but must be called)
    int32_t finalLen = 0;
    if (EVP_CipherFinal_ex(ctx, output + outLen, &finalLen) <= 0) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    // Get updated bit offset
    if (num != nullptr) {
        *num = EVP_CIPHER_CTX_get_num(ctx);
    }

    if (ivec != nullptr) {
        if (EVP_CIPHER_CTX_get_updated_iv(ctx, ivec, EVP_CIPHER_CTX_get_iv_length(ctx)) <= 0) {
            ERR_print_errors_fp(stderr);
        }
    }
    // Release context
    EVP_CIPHER_CTX_free(ctx);
}
/// @brief Custom AES-ECB encrypt/decrypt function, does not use AES_KEY
/// @param in Input data (length must be multiple of AES_BLOCK_SIZE)
/// @param out Output buffer (pre-allocated with sufficient space)
/// @param key Key data (length must be 16/24/32 bytes, corresponding to AES-128/192/256)
/// @param keyLen Key length (16/24/32)
/// @param enc 1=Encrypt, 0=Decrypt
void AES_ecb_encrypt_evp(u_char const *in, u_char *out, u_char const *key, int32_t keyLen, const int32_t enc) noexcept
{
    if (in == nullptr || out == nullptr || key == nullptr) {
        return;
    }
    // Select corresponding AES-ECB algorithm based on key length
    const EVP_CIPHER *cipher = nullptr;
    switch (keyLen) {
        case kInt_16:  // AES-kInt_128
            cipher = EVP_aes_128_ecb();
            break;
        case kInt_24:  // AES-kInt_192
            cipher = EVP_aes_192_ecb();
            break;
        case kInt_32:  // AES-kInt_256
            cipher = EVP_aes_256_ecb();
            break;
        default:  // Invalid key length, return directly
            break;
    }
    if (cipher == nullptr) {
        return;
    }
    return Encrypt_evp_common(in, out, key, cipher, enc, AES_BLOCK_SIZE);
}
/// @brief Custom AES-CBC encrypt/decrypt function, EVP implementation, does not use AES_KEY
/// @param in Input data block (8 bytes)
/// @param out Output buffer (8 bytes)
/// @param length Data length (must be multiple of 8)
/// @param key Key data (16/24/32 bytes, corresponding to AES-128/192/256)
/// @param keyLen Key length (16/24/32)
/// @param ivec IV (input: initial IV, output: updated IV)
/// @param enc enc: 1=Encrypt, 0=Decrypt
void AES_cbc_encrypt_evp(u_char const *in,
                         u_char *out,
                         size_t length,
                         u_char const *key,
                         int32_t keyLen,
                         u_char *ivec,
                         const int32_t enc) noexcept
{
    if (in == nullptr || out == nullptr || key == nullptr || ivec == nullptr || length <= 0 || length % kInt_16 != 0) {
        return;
    }
    // Select corresponding AES-CBC algorithm based on key length
    const EVP_CIPHER *cipher = nullptr;
    switch (keyLen) {
        case kInt_16:  // AES-kInt_128
            cipher = EVP_aes_128_cbc();
            break;
        case kInt_24:  // AES-kInt_192
            cipher = EVP_aes_192_cbc();
            break;
        case kInt_32:  // AES-kInt_256
            cipher = EVP_aes_256_cbc();
            break;
        default:  // Invalid key length, return directly
            break;
    }
    if (cipher == nullptr) {
        return;
    }
    return Encrypt_evp_common(in, out, key, cipher, enc, static_cast< int32_t >(length), ivec);
}
/// @brief DES-ECB encrypt/decrypt function (EVP implementation, not dependent on DES specific types)
/// @param input Input data block (8 bytes)
/// @param output Output buffer (8 bytes)
/// @param key First key (8 bytes)
/// @param enc enc: 1=Encrypt, 0=Decrypt
void DES_ecb_encrypt_evp(u_char const *input, u_char *output, u_char const *key, int32_t enc) noexcept
{
    if (input == nullptr || output == nullptr || key == nullptr) {
        return;
    }
    return Encrypt_evp_common(input, output, key, EVP_des_ecb(), enc, kInt_8);
}
/// @brief DES-NCBC encrypt/decrypt function (EVP implementation, not dependent on DES specific types) DES-NCBC does not handle padding
/// @param input Input data block (8 bytes)
/// @param output Output buffer (8 bytes)
/// @param key First key (8 bytes)
/// @param ivec IV (input: initial IV, output: updated IV)
/// @param enc enc: 1=Encrypt, 0=Decrypt
void DES_ncbc_encrypt_evp(
    u_char const *input, u_char *output, int64_t length, u_char const *key, u_char *ivec, int32_t enc) noexcept
{
    if (input == nullptr || output == nullptr || key == nullptr || ivec == nullptr || length <= 0
        || length % kInt_8 != 0) {
        return;
    }

    return Encrypt_evp_common(input, output, key, EVP_des_cbc(), enc, static_cast< int32_t >(length), ivec);
}
/// @brief 3DES-ECB encrypt/decrypt function (EVP implementation, not dependent on DES specific types)
/// @param input Input data block (8 bytes)
/// @param output Output buffer (8 bytes)
/// @param key1 First key (8 bytes)
/// @param key2 Second key (8 bytes)
/// @param key3 Third key (8 bytes)
/// @param enc enc: 1=Encrypt, 0=Decrypt
void DES_ecb3_encrypt_evp(u_char const *input,
                          u_char *output,
                          u_char const *key1,
                          u_char const *key2,
                          u_char const *key3,
                          int32_t enc) noexcept
{
    // Parameter validation
    if (input == nullptr || output == nullptr || key1 == nullptr || key2 == nullptr || key3 == nullptr) {
        return;
    }
    // Combine three 8-byte keys into 24-byte 3DES key
    u_char key[kInt_24];
    for (int32_t i = 0; i < kInt_8; i++) {
        key[i] = key1[i];
    }
    for (int32_t i = 0; i < kInt_8; i++) {
        key[i + kInt_8] = key2[i];
    }
    for (int32_t i = 0; i < kInt_8; i++) {
        key[i + kInt_16] = key3[i];
    }

    return Encrypt_evp_common(input, output, key, EVP_des_ede3_ecb(), enc, kInt_8);
}

/// @brief AES-CFB1 encrypt/decrypt function (EVP implementation, not dependent on AES_KEY)
/// @param in Input data
/// @param out Output buffer (pre-allocated with sufficient space)
/// @param length Input data length (bytes)
/// @param key Key data (16/24/32 bytes, corresponding to AES-128/192/256)
/// @param keyLen Key length (16/24/32)
/// @param ivec Initialization vector (16 bytes, updated internally)
/// @param num Track processed bit count (0-7)
/// @param enc 1=Encrypt, 0=Decrypt
void AES_cfb1_encrypt_evp(u_char const *in,
                          u_char *out,
                          int64_t length,
                          u_char const *key,
                          int32_t keyLen,
                          u_char *ivec,
                          int32_t *num,
                          const int32_t enc) noexcept
{
    // Parameter validation
    if (in == nullptr || out == nullptr || key == nullptr || ivec == nullptr || num == nullptr || length <= 0) {
        return;
    }

    const EVP_CIPHER *cipher = nullptr;
    switch (keyLen) {
        case kInt_16:
            cipher = EVP_aes_128_cfb1();
            break;
        case kInt_24:
            cipher = EVP_aes_192_cfb1();
            break;
        case kInt_32:
            cipher = EVP_aes_256_cfb1();
            break;
        default:
            break;
    }

    return Encrypt_evp_common(in, out, key, cipher, enc, static_cast< int32_t >(length), ivec, num);
}
/// @brief EVP version of AES_cfb8_encrypt (completely equivalent)
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param length Data length (bytes)
/// @param key AES key (16 bytes, including parity bits)
/// @param keyLen AES key length
/// @param ivec Initialization vector (8 bytes), input/output
/// @param num Indicates number of bytes used in current keystream block
/// @param enc Encryption mode: 1 (DES_ENCRYPT) or 0 (DES_DECRYPT)
void AES_cfb8_encrypt_evp(u_char const *in,
                          u_char *out,
                          size_t length,
                          u_char const *key,
                          int32_t keyLen,
                          u_char *ivec,
                          int32_t *num,
                          const int32_t enc) noexcept
{
    // Parameter validation
    if (in == nullptr || out == nullptr || key == nullptr || ivec == nullptr || num == nullptr || length <= 0) {
        return;
    }

    const EVP_CIPHER *cipher = nullptr;
    switch (keyLen) {
        case kInt_16:
            cipher = EVP_aes_128_cfb8();
            break;
        case kInt_24:
            cipher = EVP_aes_192_cfb8();
            break;
        case kInt_32:
            cipher = EVP_aes_256_cfb8();
            break;
        default:
            break;
    }

    return Encrypt_evp_common(in, out, key, cipher, enc, static_cast< int32_t >(length), ivec, num);
}
/// @brief EVP version of AES_cfb128_encrypt (completely equivalent)
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param length Data length (bytes)
/// @param key AES key (16 bytes, including parity bits)
/// @param keyLen AES key length
/// @param ivec Initialization vector (8 bytes), input/output
/// @param num Indicates number of bytes used in current keystream block
/// @param enc Encryption mode: 1 (DES_ENCRYPT) or 0 (DES_DECRYPT)
void AES_cfb128_encrypt_evp(u_char const *in,
                            u_char *out,
                            size_t length,
                            u_char const *key,
                            int32_t keyLen,
                            u_char *ivec,
                            int32_t *num,
                            const int32_t enc) noexcept
{
    // Parameter validation
    if (in == nullptr || out == nullptr || key == nullptr || ivec == nullptr || num == nullptr || length <= 0) {
        return;
    }

    const EVP_CIPHER *cipher = nullptr;
    switch (keyLen) {
        case kInt_16:
            cipher = EVP_aes_128_cfb128();
            break;
        case kInt_24:
            cipher = EVP_aes_192_cfb128();
            break;
        case kInt_32:
            cipher = EVP_aes_256_cfb128();
            break;
        default:
            break;
    }

    return Encrypt_evp_common(in, out, key, cipher, enc, static_cast< int32_t >(length), ivec, num);
}
/// @brief EVP version of AES_ofb128_encrypt (completely equivalent)
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param length Data length (bytes)
/// @param key AES key (16 bytes, including parity bits)
/// @param keyLen AES key length
/// @param ivec Initialization vector (8 bytes), input/output
/// @param num Indicates the number of bytes used in the current keystream block
void AES_ofb128_encrypt_evp(u_char const *in,
                            u_char *out,
                            size_t length,
                            u_char const *key,
                            int32_t keyLen,
                            u_char *ivec,
                            int32_t *num) noexcept
{
    // Parameter validation
    if (in == nullptr || out == nullptr || key == nullptr || ivec == nullptr || num == nullptr || length <= 0) {
        return;
    }

    const EVP_CIPHER *cipher = nullptr;
    switch (keyLen) {
        case kInt_16:
            cipher = EVP_aes_128_ofb();
            break;
        case kInt_24:
            cipher = EVP_aes_192_ofb();
            break;
        case kInt_32:
            cipher = EVP_aes_256_ofb();
            break;
        default:
            break;
    }

    return Encrypt_evp_common(in, out, key, cipher, kInt_1, static_cast< int32_t >(length), ivec, num);
}
/// @brief EVP version of DES_cfb_encrypt (completely equivalent)
/// @param in        Input data (plaintext or ciphertext)
/// @param out       Output data (ciphertext or plaintext)
/// @param numbits   Feedback bit count: 1, 8, or 64
/// @param length    Data length (bytes)
/// @param key       DES key (8 bytes, including parity bits)
/// @param ivec      Initialization vector (8 bytes), input/output
void DES_cfb_encrypt_evp(u_char const *in,
                         u_char *out,
                         int32_t numbits,
                         int64_t length,
                         u_char const *key,
                         u_char *ivec,
                         int32_t const enc) noexcept
{
    // Parameter validation
    if (length <= 0 || in == nullptr || out == nullptr || key == nullptr || ivec == nullptr
        || (numbits != kInt_1 && numbits != kInt_8 && numbits != kInt_64)) {
        return;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        return;
    }

    // Select corresponding CFB mode based on numbits
    const EVP_CIPHER *cipher = nullptr;
    switch (numbits) {
        case kInt_1:
            cipher = EVP_des_cfb1();
            break;
        case kInt_8:
            cipher = EVP_des_cfb8();  // or EVP_des_cfb()
            break;
        case kInt_64:
            cipher = EVP_des_cfb64();
            break;
        default:
            break;
    }

    return Encrypt_evp_common(in, out, key, cipher, enc, static_cast< int32_t >(length), ivec);
}
/// @brief Equivalent to DES_cfb64_encrypt
/// @param in Input data
/// @param out Output data
/// @param length Data length (bytes)
/// @param key DES key (8 bytes)
/// @param ivec Initialization vector (8 bytes), input/output
/// @param num Indicates the number of bytes used in the current keystream block
void DES_cfb64_encrypt_evp(u_char const *in,
                           u_char *out,
                           int64_t length,
                           u_char const *key,
                           u_char *ivec,
                           int32_t *num,
                           int32_t const enc) noexcept
{
    // Parameter validation
    if (length <= 0 || in == nullptr || out == nullptr || key == nullptr || ivec == nullptr) {
        return;
    }

    return Encrypt_evp_common(in, out, key, EVP_des_cfb64(), enc, static_cast< int32_t >(length), ivec, num);
}
/// @brief Equivalent to DES_ofb_encrypt(in, out, numbits, length, schedule, ivec)
/// @param in      Input data
/// @param out     Output data
/// @param numbits Feedback bit count: 1 or 8
/// @param length  Data length (bytes)
/// @param key    DES key (8 bytes)
/// @param ivec    Initialization vector (8 bytes), input/output
void DES_ofb_encrypt_evp(
    u_char const *in, u_char *out, int32_t numbits, int64_t length, u_char const *key, u_char *ivec) noexcept
{
    // Parameter validation
    if (length <= 0 || in == nullptr || out == nullptr || key == nullptr || ivec == nullptr || numbits != kInt_8) {
        return;
    }

    return Encrypt_evp_common(in, out, key, EVP_des_ofb(), kInt_1, static_cast< int32_t >(length), ivec);
}
/// @brief DES OFB64 mode encryption/decryption (equivalent to DES_ofb64_encrypt)
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param length Data length (bytes)
/// @param key DES key (8 bytes)
/// @param ivec Initialization vector (8 bytes) Input: initial IV Output: updated keystream block (can be used for next call)
/// @param num Indicates the number of bytes used in the current keystream block
void DES_ofb64_encrypt_evp(
    u_char const *in, u_char *out, int64_t length, u_char const *key, u_char *ivec, int32_t *num) noexcept
{
    // Parameter validation
    if (length <= 0 || in == nullptr || out == nullptr || key == nullptr || ivec == nullptr || num == nullptr) {
        return;
    }

    if (*num < 0 || *num > kInt_7) {
        *num = 0;  // Reset illegal state
    }

    return Encrypt_evp_common(in, out, key, EVP_des_ofb(), kInt_1, static_cast< int32_t >(length), ivec, num);
}
/// @brief 3DES EDE CFB mode encryption/decryption (triple independent key version) equivalent to DES_ede3_cfb_encrypt()
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param numbits Feedback bit count (recommended 8 or 64)
/// @param length Data length (bytes)
/// @param key1 First DES key (8 bytes)
/// @param key2 Second DES key (8 bytes)
/// @param key3 Third DES key (8 bytes)
/// @param ivec Initialization vector (8 bytes) Input: initial IV Output: updated feedback register (used for next call)
/// @param enc Encryption mode: 1 (DES_ENCRYPT) or 0 (DES_DECRYPT)
void DES_ede3_cfb_encrypt_evp(u_char const *in,
                              u_char *out,
                              int32_t numbits,
                              int64_t length,
                              u_char const *key1,
                              u_char const *key2,
                              u_char const *key3,
                              u_char *ivec,
                              int32_t enc) noexcept
{
    // Parameter validation
    if (key1 == nullptr || key2 == nullptr || key3 == nullptr || ivec == nullptr || in == nullptr || out == nullptr
        || length <= 0) {
        return;
    }

    if (numbits != kInt_8 && numbits != kInt_64) {
        // Only CFB8 and CFB64 are supported
        return;
    }
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        return;
    }
    // Select cipher based on numbits
    const EVP_CIPHER *cipher = nullptr;
    switch (numbits) {
        case kInt_8:
            cipher = EVP_des_ede3_cfb8();  // 3DES-CFB8
            break;
        case kInt_64:
            cipher = EVP_des_ede3_cfb64();  // 3DES-CFB64
            break;
        default:
            break;
    }
    if (cipher == nullptr) {
        return;
    }

    // Construct 24-byte key: K1 || K2 || K3
    u_char tdesKey[kInt_24];
    memcpy(tdesKey, key1, kInt_8);
    memcpy(tdesKey + kInt_8, key2, kInt_8);
    memcpy(tdesKey + kInt_16, key3, kInt_8);

    Encrypt_evp_common(in, out, tdesKey, EVP_des_ofb(), enc, static_cast< int32_t >(length), ivec);
    // Clear temporary key (security consideration)
    memset(tdesKey, 0, sizeof(tdesKey));
}
/// @brief 3DES EDE CFB64 mode encryption/decryption (triple key interface, supports state continuation) DES_ede3_cfb64_encrypt()
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
                                int32_t enc) noexcept
{
    if (length <= 0 || in == nullptr || out == nullptr || k1 == nullptr || k2 == nullptr || k3 == nullptr
        || ivec == nullptr || num == nullptr) {
        return;
    }

    // Combine keys
    u_char key[kInt_24];
    memcpy(key + 0 * kInt_8, k1, kInt_8);
    memcpy(key + kInt_1 * kInt_8, k2, kInt_8);
    memcpy(key + 2 * kInt_8, k3, kInt_8);

    Encrypt_evp_common(in, out, key, EVP_des_ede3_cfb64(), enc, static_cast< int32_t >(length), ivec, num);

    OPENSSL_cleanse(key, 24);  // Clear sensitive data
}
/// @brief Equivalent to DES_ede3_ofb64_encrypt() 3DES EDE OFB64 mode encryption/decryption (supports state continuation). OFB mode uses the same logic for encryption/decryption; no enc parameter is needed.
/// @param in Input data (plaintext or ciphertext)
/// @param out Output data (ciphertext or plaintext)
/// @param length Data length (bytes)
/// @param k1 First DES key (8 bytes)
/// @param k2 Second DES key (8 bytes)
/// @param k3 Third DES key (8 bytes)
/// @param ivec Initialization vector (8 bytes) Input: current IV Output: updated keystream block (new IV)
/// @param num Current byte offset (0~7), input/output parameter for continuous encrypt/decrypt across multiple calls
void DES_ede3_ofb64_encrypt_evp(u_char const *in,
                                u_char *out,
                                int64_t length,
                                u_char const *k1,
                                u_char const *k2,
                                u_char const *k3,
                                u_char *ivec,
                                int32_t *num) noexcept
{
    // Parameter validation
    if (length <= 0 || in == nullptr || out == nullptr || k1 == nullptr || k2 == nullptr || k3 == nullptr
        || ivec == nullptr || num == nullptr) {
        return;
    }

    // Combine keys
    u_char key[kInt_24];
    memcpy(key + 0 * kInt_8, k1, kInt_8);
    memcpy(key + kInt_1 * kInt_8, k2, kInt_8);
    memcpy(key + 2 * kInt_8, k3, kInt_8);

    return Encrypt_evp_common(in, out, key, EVP_des_ede3_ofb(), kInt_1, static_cast< int32_t >(length), ivec, num);
}
#endif
/// @brief EVP version of DES_ede3_cbc_encrypt
/// @param input Input data (plaintext or ciphertext)
/// @param output Output data (ciphertext or plaintext)
/// @param length Data length (must be a multiple of 8)
/// @param ks1 First DES key (8 bytes)
/// @param ks2 Second DES key (8 bytes)
/// @param ks3 Third DES key (8 bytes)
/// @param ivec IV (Input: initial IV, Output: updated IV)
/// @param enc 1=Encrypt, 0=Decrypt
void DES_ede3_cbc_encrypt_evp(u_char const *input,
                              u_char *output,
                              int64_t length,
                              void *ks1,
                              void *ks2,
                              void *ks3,
                              u_char *ivec,
                              int32_t enc) noexcept
{
    // Parameter validation
    if (input == nullptr || output == nullptr || ivec == nullptr || length <= 0 || length % kInt_8 != 0) {
        return;
    }

#if PUHUA_OPENSSL_IS_11X
    return DES_ede3_cbc_encrypt(static_cast< uint8_t const * >(input), static_cast< uint8_t * >(output),
                                static_cast< int64_t >(length), static_cast< DES_key_schedule * >(ks1),
                                static_cast< DES_key_schedule * >(ks2), static_cast< DES_key_schedule * >(ks3),
                                static_cast< DES_cblock * >(static_cast< void * >(ivec)), enc);
#else
    // Construct 24-byte 3DES key
    u_char key[kInt_24];
    memcpy(key, ks1, kInt_8);
    memcpy(key + kInt_8, ks2, kInt_8);
    memcpy(key + kInt_16, ks3, kInt_8);

    return Encrypt_evp_common(input, output, key, EVP_des_ede3_cbc(), enc, static_cast< int32_t >(length), ivec);
#endif
}
/// @brief Use EVP encryption series functions
/// @param in Input
/// @param out Output
/// @param originalKey Key
/// @param keyBits Key length in bits
void AES_encrypt_evp(u_char const *in, u_char *out, u_char const *originalKey, int32_t keyBits) noexcept
{
    if (in == nullptr || out == nullptr || originalKey == nullptr) {
        return;
    }

    const EVP_CIPHER *cipher = nullptr;
    switch (keyBits) {
        case kInt_128:
            cipher = EVP_aes_128_ecb();
            break;
        case kInt_192:
            cipher = EVP_aes_192_ecb();
            break;
        case kInt_256:
            cipher = EVP_aes_256_ecb();
            break;
        default:
            break;
    }

    if (cipher == nullptr) {
        return;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        return;
    }

    // Key difference 1: Encryption initialization function
    int32_t success = EVP_EncryptInit_ex(ctx, cipher, nullptr, originalKey, nullptr);
    success &= EVP_CIPHER_CTX_set_padding(ctx, 0);

    if (success != 0) {
        int32_t outLen{0};
        // Key difference 2: Encryption data processing function
        success = EVP_EncryptUpdate(ctx, out, &outLen, in, kInt_16);
        success &= (outLen == kInt_16);  // NOLINT
    }

    EVP_CIPHER_CTX_free(ctx);
}
/// @brief Use EVP decryption series functions
/// @param in Input
/// @param out Output
/// @param originalKey Key
/// @param keyBits Key length in bits
void AES_decrypt_evp(u_char const *in, u_char *out, u_char const *originalKey, int32_t keyBits) noexcept
{
    if (in == nullptr || out == nullptr || originalKey == nullptr) {
        return;
    }

    const EVP_CIPHER *cipher = nullptr;
    switch (keyBits) {
        case kInt_128:
            cipher = EVP_aes_128_ecb();
            break;
        case kInt_192:
            cipher = EVP_aes_192_ecb();
            break;
        case kInt_256:
            cipher = EVP_aes_256_ecb();
            break;
        default:
            break;
    }

    if (cipher == nullptr) {
        return;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        return;
    }

    // Key difference 1: Decryption initialization function
    int32_t success = EVP_DecryptInit_ex(ctx, cipher, nullptr, originalKey, nullptr);
    success &= EVP_CIPHER_CTX_set_padding(ctx, 0);

    if (success != 0) {
        int32_t outLen{0};
        // Key difference 2: Decryption data processing function
        success = EVP_DecryptUpdate(ctx, out, &outLen, in, kInt_16);
        success &= (outLen == kInt_16);  // NOLINT
    }

    EVP_CIPHER_CTX_free(ctx);
}
/// @brief Get EC_KEY from memory
/// @param bio Memory data containing ECC key
/// @return EVP_PKEY
EVP_PKEY *PEM_read_bio_EC_PUBKEY_Compatible(BIO *bio) noexcept
{
#if PUHUA_OPENSSL_IS_11X
    EC_KEY *ecKey = PEM_read_bio_EC_PUBKEY(bio, nullptr, nullptr, nullptr);

    EVP_PKEY *pkey = EVP_PKEY_new();
    if (pkey == nullptr || EVP_PKEY_set1_EC_KEY(pkey, ecKey) != kInt_1) {
        EVP_PKEY_free(pkey);
        EC_KEY_free(ecKey);
        return nullptr;
    }

    // Release EC_KEY (EVP_PKEY already holds the reference)
    EC_KEY_free(ecKey);
    return pkey;
#else
    EVP_PKEY *pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    if (pkey == nullptr) {
        return nullptr;
    }

    // Verify key type
    int32_t keytype = EVP_PKEY_get_id(pkey);
    if (keytype != EVP_PKEY_EC) {
        EVP_PKEY_free(pkey);
        return nullptr;
    }

    // Use new parameter API to verify it is an EC key
    char groupName[kInt_64] = {0};
    size_t groupNameLen     = 0;

    if (EVP_PKEY_get_utf8_string_param(pkey, OSSL_PKEY_PARAM_GROUP_NAME, groupName, sizeof(groupName) - kInt_1,
                                       &groupNameLen)
        <= 0) {
        // Not an EC key or failed to get parameters
        EVP_PKEY_free(pkey);
        return nullptr;
    }
    return pkey;
#endif
}
/// @brief Get eckey length
/// @param pkey Key containing ecckey
/// @return Key length, 0 on failure
int32_t Get_EccKey_Size(EVP_PKEY *pkey) noexcept
{
#if PUHUA_OPENSSL_IS_11X
    if (pkey == nullptr || EVP_PKEY_id(pkey) != EVP_PKEY_EC) {
        return 0;
    }
#else
    if (pkey == nullptr || EVP_PKEY_get_id(pkey) != EVP_PKEY_EC) {
        return 0;
    }
#endif
    // return EVP_PKEY_bits(pkey);
    return EVP_PKEY_size(pkey);
}
/// @brief Get rsakey length
/// @param pkey Key containing RSA
/// @return Key length, 0 on failure
int32_t Get_RsaKey_Size(EVP_PKEY *pkey) noexcept
{
#if PUHUA_OPENSSL_IS_11X
    if (pkey == nullptr || EVP_PKEY_id(pkey) != EVP_PKEY_RSA) {
        return 0;
    }
#else
    if (pkey == nullptr || EVP_PKEY_get_id(pkey) != EVP_PKEY_RSA) {
        return 0;
    }
#endif
    // return EVP_PKEY_bits(pkey);
    return EVP_PKEY_size(pkey);
}
/// @brief Perform ECDH key exchange using new EVP API
/// @param sharedSecret Shared secret data
/// @param secretLen Shared secret data length
/// @param peerKey Peer public key
/// @param myKey Local private/public key pair
/// @return 0 on failure, 1 on success
int32_t Ecdh_key_exchange(u_char *sharedSecret, size_t *secretLen, EVP_PKEY *peerKey, EVP_PKEY *myKey) noexcept
{
    if (sharedSecret == nullptr) {
        return 0;
    }
    // Initialize output parameters
    *secretLen = 0;

    // Create EVP_PKEY_CTX context
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(myKey, nullptr);
    if (ctx == nullptr) {
        return 0;
    }

    // Initialize key exchange
    if (EVP_PKEY_derive_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return 0;
    }

    // Set peer public key
    if (EVP_PKEY_derive_set_peer(ctx, peerKey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return 0;
    }

    // First get the length of the shared secret
    if (EVP_PKEY_derive(ctx, nullptr, secretLen) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return 0;
    }

    // Calculate shared secret
    if (EVP_PKEY_derive(ctx, sharedSecret, secretLen) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return 0;
    }

    // Completed successfully, release context and return
    EVP_PKEY_CTX_free(ctx);
    return kInt_1;
}
/// @brief Generate RSA key pair (using default public exponent 65537)
/// @param bits bits Key length (2048 or 4096)
/// @return Generated EVP_PKEY pointer, returns nullptr on failure
EVP_PKEY *Generate_rsa_evp_key(int32_t bits) noexcept
{
#if PUHUA_OPENSSL_IS_11X
    EVP_PKEY *pkey = nullptr;
    RSA *rsa       = nullptr;

    // kInt_1. Create RSA structure and generate key pair
    rsa = RSA_new();
    if (rsa == nullptr) {
        return nullptr;
    }

    // 2. Generate RSA key pair (specify key length and public key exponent)
    BIGNUM *bne = BN_new();
    if (bne == nullptr) {
        RSA_free(rsa);
        return nullptr;
    }

    // Set public key exponent (usually 65537)
    if (BN_set_word(bne, RSA_F4) != kInt_1) {
        BN_free(bne);
        RSA_free(rsa);
        return nullptr;
    }

    // Generate key pair (bits is the key length)
    if (RSA_generate_key_ex(rsa, bits, bne, nullptr) != kInt_1) {
        BN_free(bne);
        RSA_free(rsa);
        return nullptr;
    }
    BN_free(bne);  // Public key exponent has been used, release it

    // 3. Encapsulate RSA into EVP_PKEY
    pkey = EVP_PKEY_new();
    if (pkey == nullptr) {
        RSA_free(rsa);
        return nullptr;
    }

    if (EVP_PKEY_set1_RSA(pkey, rsa) != kInt_1) {
        EVP_PKEY_free(pkey);
        RSA_free(rsa);
        return nullptr;
    }

    // Release underlying RSA structure (EVP_PKEY already holds a reference via set1)
    RSA_free(rsa);
    return pkey;
#else
    // Create RSA key generation context
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (ctx == nullptr) {
        return nullptr;
    }

    // Initialize key generation
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return nullptr;
    }

    // Only set key length (use default value 65537 for public exponent)
    OSSL_PARAM params[2];
    params[0]      = OSSL_PARAM_construct_int(OSSL_PKEY_PARAM_RSA_BITS, &bits);
    params[kInt_1] = OSSL_PARAM_construct_end();

    // Apply parameters to context
    if (EVP_PKEY_CTX_set_params(ctx, params) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return nullptr;
    }

    // Generate RSA key pair
    EVP_PKEY *pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return nullptr;
    }

    // Release context
    EVP_PKEY_CTX_free(ctx);
    return pkey;
#endif
}

/// @brief Generate ECDSA signature (using EVP interface)
/// @param privKey Private key
/// @param data Input data
/// @param dataLen Input data length
/// @param signature Signature data
/// @param sigLen Signature data length
/// @return 0 failure, 1 success
int32_t Ecdsa_sign(EVP_PKEY *privKey, u_char const *data, size_t dataLen, u_char *signature, size_t *sigLen) noexcept
{
    if (privKey == nullptr || data == nullptr || signature == nullptr || dataLen == 0) {
        return 0;
    }

    // Check if public key is of ECC type (ensure ECDSA verification)
    if (EVP_PKEY_id(privKey) != EVP_PKEY_EC) {
        return 0;
    }

    EVP_MD_CTX *mdCtx = EVP_MD_CTX_new();
    if (mdCtx == nullptr) {
        return 0;
    }

    // Initialize signature context (using SHA-kInt_256 hash algorithm)
    if (EVP_DigestSignInit(mdCtx, nullptr, EVP_sha256(), nullptr, privKey) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // Input data to be signed
    if (EVP_DigestSignUpdate(mdCtx, data, dataLen) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // Generate signature
    int32_t ret = EVP_DigestSignFinal(mdCtx, signature, sigLen);
    EVP_MD_CTX_free(mdCtx);

    return ret > 0 ? kInt_1 : 0;
}
/// @brief Verify ECDSA signature (replacement for ECDSA_verify)
/// @param pubKey Public key
/// @param data Digest data
/// @param dataLen Digest data length
/// @param signature Signature data
/// @param sigLen Signature data length
/// @return 0 failure, 1 success
int32_t Ecdsa_verify(
    EVP_PKEY *pubKey, u_char const *data, size_t dataLen, u_char const *signature, size_t sigLen) noexcept
{
    // Check that input parameters are not null
    if (pubKey == nullptr || data == nullptr || signature == nullptr || dataLen == 0 || sigLen == 0) {
        return 0;
    }

    // Check if public key is of ECC type (ensure ECDSA verification)
    if (EVP_PKEY_id(pubKey) != EVP_PKEY_EC) {
        return 0;
    }

    EVP_MD_CTX *mdCtx = EVP_MD_CTX_new();
    if (mdCtx == nullptr) {
        return 0;
    }

    // Initialize verification context (use same hash algorithm SHA-256 as signing)
    if (EVP_DigestVerifyInit(mdCtx, nullptr, EVP_sha256(), nullptr, pubKey) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // Input data to be verified
    if (EVP_DigestVerifyUpdate(mdCtx, data, dataLen) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // Perform verification (returns 1 for success, 0 for failure, -1 for error)
    int32_t ret = EVP_DigestVerifyFinal(mdCtx, signature, sigLen);

    EVP_MD_CTX_free(mdCtx);

    return ret;
}
/// @brief Implement PSS padding signing using EVP interface (replacement for RSA_padding_add_PKCS1_PSS + private key encryption)
/// @param pkey EVP_PKEY object containing RSA private key
/// @param out  Output buffer (signature result)
/// @param outlen Output buffer size / actual signature length
/// @param digest Hash algorithm (e.g., EVP_sha256())
/// @param saltLen PSS salt length (usually same as hash length)
/// @param in  Input data (data to be signed)
/// @param inlen Input data length
/// @return Returns 1 on success, 0 on failure
int32_t RSA_padding_add_PKCS1_PSS_evp(EVP_PKEY *pkey,
                                      u_char *out,
                                      size_t *outlen,  // NOLINT
                                      const EVP_MD *digest,
                                      int32_t saltLen,
                                      u_char const *in,
                                      size_t inlen) noexcept
{
    // Parameter check
    if (pkey == nullptr || out == nullptr || outlen == nullptr || digest == nullptr || in == nullptr) {
        return 0;
    }
    // Check if key type is RSA
    if (EVP_PKEY_base_id(pkey) != EVP_PKEY_RSA) {
        return 0;
    }
#if PUHUA_OPENSSL_IS_11X
    std::ignore = inlen;
    // Get RSA structure from EVP_PKEY
    RSA *rsa = EVP_PKEY_get0_RSA(pkey);
    if (rsa == nullptr) {
        return 0;
    }
    return RSA_padding_add_PKCS1_PSS(rsa, out, in, digest, saltLen);
#else
    // Create signing context
    EVP_MD_CTX *mdCtx = EVP_MD_CTX_new();
    if (mdCtx == nullptr) {
        return 0;
    }

    // Initialize signing operation
    if (EVP_DigestSignInit(mdCtx, nullptr, digest, nullptr, pkey) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // Set PSS padding parameters (corresponds to RSA_padding_add_PKCS1_PSS functionality)
    EVP_PKEY_CTX *pkeyCtx = EVP_MD_CTX_get_pkey_ctx(mdCtx);
    if (pkeyCtx == nullptr) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // First set padding mode to PSS (critical step!)
    if (EVP_PKEY_CTX_set_rsa_padding(pkeyCtx, RSA_PKCS1_PSS_PADDING) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // Set PSS salt length (corresponds to saltLen parameter of RSA_padding_add_PKCS1_PSS)
    if (EVP_PKEY_CTX_set_rsa_pss_saltlen(pkeyCtx, saltLen) <= 0) {
        ERR_print_errors_fp(stderr);
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // Update data to be signed
    if (EVP_DigestSignUpdate(mdCtx, in, inlen) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // Finalize signature (automatically handles PSS padding and private key encryption)
    if (EVP_DigestSignFinal(mdCtx, out, outlen) <= 0) {
        ERR_print_errors_fp(stderr);
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // Clean up resources
    EVP_MD_CTX_free(mdCtx);
    return kInt_1;
#endif
}
/// @brief Verify PSS signature (corresponding to signature verification flow)
/// @param pkey EVP_PKEY object containing RSA public key
/// @param sig  Signature data
/// @param siglen Signature data length
/// @param digest Encryption algorithm
/// @param saltLen Salt length
/// @param in  Digest data
/// @param inlen Digest data length
/// @return 0 failure, 1 success
int32_t RSA_verify_PKCS1_PSS_evp(EVP_PKEY *pkey,
                                 u_char const *in,
                                 size_t inlen,
                                 const EVP_MD *digest,
                                 int32_t saltLen,
                                 u_char const *sig,
                                 size_t siglen) noexcept
{
    if (pkey == nullptr || sig == nullptr || digest == nullptr || in == nullptr) {
        return 0;
    }

    // Check if key type is RSA
    if (EVP_PKEY_base_id(pkey) != EVP_PKEY_RSA) {
        return 0;
    }
#if PUHUA_OPENSSL_IS_11X
    std::ignore = siglen;
    std::ignore = inlen;
    // Get RSA structure from EVP_PKEY
    RSA *rsa = EVP_PKEY_get0_RSA(pkey);
    if (rsa == nullptr) {
        return 0;
    }
    return RSA_verify_PKCS1_PSS(rsa, in, digest, sig, saltLen);
#else
    EVP_MD_CTX *mdCtx = EVP_MD_CTX_new();
    if (mdCtx == nullptr) {
        return 0;
    }

    if (EVP_DigestVerifyInit(mdCtx, nullptr, digest, nullptr, pkey) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // Set PSS parameters for verification side (must match signing side)
    EVP_PKEY_CTX *pkeyCtx = EVP_MD_CTX_get_pkey_ctx(mdCtx);
    if (pkeyCtx == nullptr) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // First set padding mode to PSS (critical step!)
    if (EVP_PKEY_CTX_set_rsa_padding(pkeyCtx, RSA_PKCS1_PSS_PADDING) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    if (EVP_PKEY_CTX_set_rsa_pss_saltlen(pkeyCtx, saltLen) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    if (EVP_DigestVerifyUpdate(mdCtx, in, inlen) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    // Verify signature
    int32_t ret = EVP_DigestVerifyFinal(mdCtx, sig, siglen);
    if (ret <= 0) {
        ERR_print_errors_fp(stderr);
        EVP_MD_CTX_free(mdCtx);
        return 0;
    }

    EVP_MD_CTX_free(mdCtx);
    return kInt_1;
#endif
}
/// @brief Implement RSA public key encryption using EVP_PKEY interface (replacement for RSA_public_encrypt)
/// @param pkey EVP_PKEY object containing RSA public key
/// @param out  Output buffer (encryption result)
/// @param in   Input plaintext data
/// @param inlen Input data length
/// @param padding Padding mode (compatible with RSA_public_encrypt)
/// @return Returns encryption length on success, -1 on failure
int32_t Rsa_public_encrypt_evp(EVP_PKEY *pkey, u_char *out, u_char const *in, size_t inlen, int32_t padding) noexcept
{
    // Parameter validity check
    if (pkey == nullptr || out == nullptr || in == nullptr) {
        return -1;
    }

    // Verify key type is RSA
    if (EVP_PKEY_base_id(pkey) != EVP_PKEY_RSA) {
        return -1;
    }

    // Create encryption context
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (ctx == nullptr) {
        return -1;
    }

    // Initialize public key encryption operation
    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }

    // Set padding mode (maintain full compatibility with RSA_public_encrypt)
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }

    size_t outlen{static_cast< size_t >(Get_RsaKey_Size(pkey))};
    // Perform public key encryption
    if (EVP_PKEY_encrypt(ctx, out, &outlen, in, inlen) <= 0) {
        ERR_print_errors_fp(stderr);
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }

    // Success handling
    int32_t ret = static_cast< int32_t >(outlen);
    EVP_PKEY_CTX_free(ctx);
    return ret;
}
/// @brief Implement RSA private key decryption using EVP_PKEY interface (replacement for RSA_private_decrypt)
/// @param pkey EVP_PKEY object containing RSA private key
/// @param out  Output buffer (decryption result)
/// @param in   Input ciphertext data
/// @param inlen Input data length
/// @param padding Padding mode (must match encryption)
/// @return Returns decryption length on success, -1 on failure
int32_t Rsa_private_decrypt_evp(EVP_PKEY *pkey, u_char *out, u_char const *in, size_t inlen, int32_t padding) noexcept
{
    // Parameter validity check
    if (pkey == nullptr || out == nullptr || in == nullptr) {
        return -1;
    }

    // Verify key type is RSA
    if (EVP_PKEY_base_id(pkey) != EVP_PKEY_RSA) {
        return -1;
    }

    // Create decryption context
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (ctx == nullptr) {
        return -1;
    }

    // Initialize private key decryption operation
    if (EVP_PKEY_decrypt_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }

    // Set padding mode (must match the mode used during encryption)
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    size_t outlen{inlen};
    // Perform private key decryption
    if (EVP_PKEY_decrypt(ctx, out, &outlen, in, inlen) <= 0) {
        ERR_print_errors_fp(stderr);
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }

    // Success handling
    int32_t ret = static_cast< int32_t >(outlen);
    EVP_PKEY_CTX_free(ctx);
    return ret;
}
/// @brief Manually implement RSA private key signing (compatible with RSA_private_encrypt)
/// @param in Input data (plaintext)
/// @param inLen Input length
/// @param out Output buffer (store signature result)
/// @param pkey EVP_PKEY private key (must be RSA type)
/// @param padding Padding mode (RSA_PKCS1_PADDING or RSA_NO_PADDING)
/// @return Returns signature length on success, -1 on failure
int32_t Rsa_private_encrypt_evp(const u_char *in, size_t inLen, u_char *out, EVP_PKEY *pkey, int32_t padding) noexcept
{
    // Parameter check (unchanged)
    if (in == nullptr || out == nullptr || pkey == nullptr
        || (padding != RSA_PKCS1_PADDING && padding != RSA_NO_PADDING)) {
        return -1;
    }
    if (EVP_PKEY_id(pkey) != EVP_PKEY_RSA) {
        return -1;
    }

    // Get RSA parameters (unchanged)
    BIGNUM *n = nullptr;
    BIGNUM *d = nullptr;
#if !PUHUA_OPENSSL_IS_11X
    if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_N, &n) == 0
        || EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_D, &d) == 0) {
        if (n != nullptr) {
            BN_free(n);
        }
        return -1;
    }
#else
    const RSA *rsa = EVP_PKEY_get0_RSA(pkey);
    if (rsa == nullptr) {
        return -1;
    }
    n = BN_dup(RSA_get0_n(rsa));
    d = BN_dup(RSA_get0_d(rsa));
    if (n == nullptr || d == nullptr) {
        if (n != nullptr) {
            BN_free(n);
        }
        if (d != nullptr) {
            BN_free(d);
        }
        return -1;
    }
#endif

    const int32_t rsaSize = BN_num_bytes(n);
    if (padding == RSA_NO_PADDING && inLen > static_cast< size_t >(rsaSize)) {
        BN_free(n);
        BN_free(d);
        return -1;
    }

    // Fix 1: Explicitly ensure at least 8 bytes for PKCS#1 padding
    if (padding == RSA_PKCS1_PADDING) {
        if (inLen > static_cast< size_t >(rsaSize) - kInt_11U) {  // 11 = 3-byte header + 8-byte padding
            BN_free(n);
            BN_free(d);
            return -1;
        }
    }

    // Construct padding data
    u_char *paddedData = static_cast< u_char * >(OPENSSL_malloc(rsaSize));
    if (paddedData == nullptr) {
        BN_free(n);
        BN_free(d);
        return -1;
    }

    if (padding == RSA_PKCS1_PADDING) {
        // Fix 2: Strictly follow PKCS#1 signature padding format
        paddedData[0] = 0x00;
        paddedData[1] = 0x01;
        size_t padLen = rsaSize - inLen - 3;                  // 3 = 00 01 00
        padLen        = (padLen < kInt_8) ? kInt_8 : padLen;  // Force at least 8 bytes of padding
        memset(paddedData + 2, kInt_FF, padLen);
        paddedData[2 + padLen] = 0x00;
        memcpy(paddedData + 3 + padLen, in, inLen);

    } else {
        // No padding: prepend leading zeros
        memset(paddedData, 0, rsaSize - inLen);
        memcpy(paddedData + rsaSize - inLen, in, inLen);
    }

    // Convert to BIGNUM and check m < n
    BIGNUM *m = BN_bin2bn(paddedData, rsaSize, nullptr);
    OPENSSL_free(paddedData);
    if (m == nullptr || BN_cmp(m, n) >= 0) {
        BN_free(m);
        BN_free(n);
        BN_free(d);
        return -1;
    }

    // Calculate signature s = m^d mod n
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *s   = BN_new();
    if (ctx == nullptr || s == nullptr || BN_mod_exp(s, m, d, n, ctx) == 0) {
        BN_free(m);
        BN_free(n);
        BN_free(d);
        BN_free(s);
        BN_CTX_free(ctx);
        return -1;
    }

    // Fix 3: Strictly handle output length
    const int32_t sLen = BN_bn2binpad(s, out, rsaSize);
    BN_free(m);
    BN_free(n);
    BN_free(d);
    BN_free(s);
    BN_CTX_free(ctx);

    return (sLen == rsaSize) ? rsaSize : -1;
}
/// @brief Manually implement RSA public key decryption (compatible with RSA_private_encrypt signing logic)
/// @param in Input data (ciphertext or signature result)
/// @param inLen Input length (must equal RSA key length)
/// @param out Output buffer (store recovered plaintext)
/// @param pkey EVP_PKEY public key (must be RSA type)
/// @param padding Padding mode (RSA_PKCS1_PADDING or RSA_NO_PADDING)
/// @return Returns plaintext length on success, -1 on failure
int32_t Rsa_public_decrypt_evp(const u_char *in, size_t inLen, u_char *out, EVP_PKEY *pkey, int32_t padding) noexcept
{
    // Parameter check
    if (in == nullptr || out == nullptr || pkey == nullptr
        || (padding != RSA_PKCS1_PADDING && padding != RSA_NO_PADDING)) {
        return -1;
    }

    // Check key type
    if (EVP_PKEY_id(pkey) != EVP_PKEY_RSA) {
        return -1;
    }

    // Get RSA public key parameters (n, e)
    BIGNUM *n = nullptr;
    BIGNUM *e = nullptr;
#if !PUHUA_OPENSSL_IS_11X  // OpenSSL 3.0+
    if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_N, &n) == 0
        || EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_E, &e) == 0) {
        return -1;
    }
#else  // OpenSSL 1.1.1
    const RSA *rsa = EVP_PKEY_get0_RSA(pkey);
    if (rsa == nullptr) {
        return -1;
    }
    n = BN_dup(RSA_get0_n(rsa));
    e = BN_dup(RSA_get0_e(rsa));
    if (n == nullptr || e == nullptr) {
        if (n != nullptr) {
            BN_free(n);
        }
        if (e != nullptr) {
            BN_free(e);
        }
        return -1;
    }
#endif

    // Check input length equals RSA key length
    const int32_t rsaSize = BN_num_bytes(n);
    if (inLen != static_cast< size_t >(rsaSize)) {
        BN_free(n);
        BN_free(e);
        return -1;
    }

    // Convert input data to BIGNUM (c)
    BIGNUM *c = BN_bin2bn(in, static_cast< int32_t >(inLen), nullptr);
    if (c == nullptr) {
        BN_free(n);
        BN_free(e);
        return -1;
    }

    // Check c < n
    if (BN_cmp(c, n) >= 0) {
        BN_free(c);
        BN_free(n);
        BN_free(e);
        return -1;
    }

    // Calculate m = c^e mod n
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *m   = BN_new();
    if (ctx == nullptr || m == nullptr || BN_mod_exp(m, c, e, n, ctx) == 0) {
        if (c != nullptr) {
            BN_free(c);
        }
        if (n == nullptr) {
            BN_free(n);
        }
        if (e == nullptr) {
            BN_free(e);
        }
        if (m == nullptr) {
            BN_free(m);
        }
        if (ctx == nullptr) {
            BN_CTX_free(ctx);
        }
        return -1;
    }

    // Convert m to byte array (fixed length rsa_size, prepend leading zeros)
    const int32_t mLen = BN_bn2binpad(m, out, rsaSize);
    if (mLen != rsaSize) {
        BN_free(c);
        BN_free(n);
        BN_free(e);
        BN_free(m);
        BN_CTX_free(ctx);
        return -1;
    }

    // Handle padding modes
    int32_t plainLen = rsaSize;
    if (padding == RSA_PKCS1_PADDING) {
        // PKCS#1 v1.5 signature padding format: 0x00 0x01 [0xFF...] 0x00 [data]
        if (rsaSize < kInt_11) {  // Padding requires at least 11 bytes
            BN_free(c);
            BN_free(n);
            BN_free(e);
            BN_free(m);
            BN_CTX_free(ctx);
            return -1;
        }

        // Check padding header: 0x00 0x01
        if (out[0] != 0x00 || out[1] != 0x01) {
            BN_free(c);
            BN_free(n);
            BN_free(e);
            BN_free(m);
            BN_CTX_free(ctx);
            return -1;
        }

        // Check padding bytes: must all be 0xFF
        size_t pos = 2;
        while (pos < static_cast< size_t >(rsaSize) && out[pos] == kInt_FF) {
            pos++;
        }

        // Check delimiter: 0x00
        if (pos >= static_cast< size_t >(rsaSize) || out[pos] != 0x00) {
            BN_free(c);
            BN_free(n);
            BN_free(e);
            BN_free(m);
            BN_CTX_free(ctx);
            return -1;
        }

        pos++;  // Skip delimiter
        plainLen = rsaSize - static_cast< int32_t >(pos);
        if (plainLen == 0) {  // Data cannot be empty
            BN_free(c);
            BN_free(n);
            BN_free(e);
            BN_free(m);
            BN_CTX_free(ctx);
            return -1;
        }

        // Move data to buffer head
        memmove(out, out + pos, plainLen);
    } else if (padding == RSA_NO_PADDING) {
        // No padding: return all bytes directly (may include leading zeros)
        plainLen = rsaSize;
    }

    // Clean up resources
    BN_free(c);
    BN_free(n);
    BN_free(e);
    BN_free(m);
    BN_CTX_free(ctx);

    return plainLen;
}
/// @brief Select corresponding elliptic curve based on platform and key length (128/kInt_192/256 bits)
/// @param security_bits Key length (only supports 128, 192, 256)
/// @return Curve name, returns null on failure
char const *Get_curve_name(int32_t keyBits, cryp::isoft_def::EccForPlatform eccForPlatform) noexcept
{
    int32_t eccNameIndex{-1};
    const char *curveName[] = {"secp128r1", "secp192k1", "secp256k1", "brainpoolP256r1"};
    switch (eccForPlatform) {
        case cryp::isoft_def::EccForPlatform::kPuhua: {
            switch (keyBits) {
                case kInt_128:
                    eccNameIndex = 0;
                    break;
                case kInt_192:
                    eccNameIndex = 1;
                    break;
                case kInt_256:
                    eccNameIndex = 2;
                    break;
                default:
                    break;
            }
            break;
        }
        // Currently nxp only supports 256
        case cryp::isoft_def::EccForPlatform::kNxp: {
            if (keyBits == kInt_256) {
                eccNameIndex = 3;
            }
            break;
        }
        // Currently bst only supports 256
        case cryp::isoft_def::EccForPlatform::kBst: {
            if (keyBits == kInt_256) {
                eccNameIndex = 2;
            }
            break;
        }
        default:
            break;
    }
    if (eccNameIndex < 0 || eccNameIndex > 3) {
        return nullptr;
    }
    return curveName[eccNameIndex];
}
/// @brief Generate ECC key according to specified security strength
/// @param security_bits Security strength (128/kInt_192/256)
/// @return Generated EVP_PKEY pointer, returns NULL on failure
EVP_PKEY *Generate_ecc_key(int32_t const keyLen, cryp::isoft_def::EccForPlatform platform) noexcept
{
#if PUHUA_OPENSSL_IS_11X
    const char *curveName = Get_curve_name(keyLen, platform);
    if (curveName == nullptr || strlen(curveName) == 0) {
        return nullptr;
    }

    EVP_PKEY *pkey    = nullptr;
    EC_KEY *ecKey     = nullptr;
    EC_GROUP *ecGroup = nullptr;
    int32_t curveNid{0};

    // kInt_1. Resolve NID by name
    curveNid = OBJ_sn2nid(curveName);
    if (curveNid == NID_undef) {
        // Try alias resolution (some curves have multiple names)
        curveNid = OBJ_ln2nid(curveName);
    }
    if (curveNid == NID_undef) {
        return nullptr;
    }

    // 2. Create EC_KEY structure
    ecKey = EC_KEY_new();
    if (ecKey == nullptr) {
        return nullptr;
    }

    // 3. Create curve group via NID
    ecGroup = EC_GROUP_new_by_curve_name(curveNid);
    if (ecGroup == nullptr) {
        EC_KEY_free(ecKey);
        return nullptr;
    }

    // 4. Associate curve group with EC_KEY
    if (EC_KEY_set_group(ecKey, ecGroup) != kInt_1) {
        EC_GROUP_free(ecGroup);
        EC_KEY_free(ecKey);
        return nullptr;
    }
    EC_GROUP_free(ecGroup);

    // 5. Generate key pair
    if (EC_KEY_generate_key(ecKey) != kInt_1) {
        EC_KEY_free(ecKey);
        return nullptr;
    }

    // 6. Encapsulate as EVP_PKEY
    pkey = EVP_PKEY_new();
    if (pkey == nullptr) {
        EC_KEY_free(ecKey);
        return nullptr;
    }

    if (EVP_PKEY_set1_EC_KEY(pkey, ecKey) != kInt_1) {
        EVP_PKEY_free(pkey);
        EC_KEY_free(ecKey);
        return nullptr;
    }

    EC_KEY_free(ecKey);
    return pkey;
#else
    // Get corresponding curve
    char const *curveName = Get_curve_name(keyLen, platform);
    if (curveName == nullptr) {
        return nullptr;
    }
    // Create ECC key generation context
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (ctx == nullptr) {
        return nullptr;
    }

    // Initialize key generation
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return nullptr;
    }

    // Set elliptic curve parameters
    OSSL_PARAM params[2];
    params[0]
        = OSSL_PARAM_construct_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME, const_cast< char * >(curveName), 0);  //NOLINT
    params[kInt_1] = OSSL_PARAM_construct_end();

    if (EVP_PKEY_CTX_set_params(ctx, params) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return nullptr;
    }

    // Generate ECC key pair
    EVP_PKEY *pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        ERR_print_errors_fp(stderr);
        EVP_PKEY_CTX_free(ctx);
        return nullptr;
    }

    // Release context and return key
    EVP_PKEY_CTX_free(ctx);
    return pkey;
#endif
}
/// @brief Check RSA key validity
/// @param pkey RSA key
/// @return 1: Key valid, 0: Key invalid, -1: Error during check (e.g., memory allocation failure, context error)
int32_t Rsa_check_key_evp(EVP_PKEY *pkey) noexcept
{
    // Check if input key is null
    if (pkey == nullptr) {
        return -1;
    }

    // Check if it is an RSA key
    if (EVP_PKEY_id(pkey) != EVP_PKEY_RSA) {
        return 0;  // Not an RSA key, return invalid
    }

    // Create context
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (ctx == nullptr) {
        return -1;  // Context creation failed
    }

    // Directly call check function (no separate init function needed)
    int32_t ret = EVP_PKEY_check(ctx);

    // Release resources
    EVP_PKEY_CTX_free(ctx);

    return ret;
}
/// @brief Generic AES wrapper/unwrapper function
/// @param cipher Wrapper algorithm
/// @param init_func Initialization algorithm
/// @param update_func Update algorithm
/// @param final_func  Result algorithm
/// @param key Secret key
/// @param ivOrIcv Initialization vector
/// @param out Output result
/// @param in  Input
/// @param inlen Input length
/// @param minInlen Minimum input length
/// @param customCheck Length check function
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
    int32_t (*customCheck)(size_t)) noexcept
{
    // Basic parameter validation
    if (key == nullptr || out == nullptr || in == nullptr || inlen < minInlen) {
        return 0;
    }

    // Custom validation (e.g., whether length is a multiple of 16)
    if (customCheck != nullptr && customCheck(inlen) == 0) {
        return 0;
    }

    // Create context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        return 0;
    }

    // Execute encrypt/decrypt flow
    int32_t outlen1 = 0;
    int32_t outlen2 = 0;
    int32_t success = 0;
    if (initFunc(ctx, cipher, nullptr, key, ivOrIcv) != 0
        && updateFunc(ctx, out, &outlen1, in, static_cast< int32_t >(inlen)) != 0
        && finalFunc(ctx, out + outlen1, &outlen2) != 0) {
        success = kInt_1;
    }

    // Clean up resources
    EVP_CIPHER_CTX_free(ctx);

    return success != 0 ? static_cast< size_t >(outlen1 + outlen2) : 0;
}
/// @brief No-padding wrap: custom length check
/// @param inlen Length
/// @return true if valid false invalid
int32_t Check_wrap_length(size_t inlen) noexcept
{
    if (inlen >= kInt_16) {
        return 1;
    }
    return 0;
}
/// @brief No-padding unwrap: custom length check (multiple of 16 bytes and ≥24)
/// @param inlen Length
/// @return true if valid false invalid
int32_t Check_unwrap_length(size_t inlen) noexcept
{
    if (inlen >= kInt_24) {
        return 1;
    }
    return 0;
}
/// @brief No-padding wrap implementation
/// @param key Key
/// @param iv  Initialization vector
/// @param out Output
/// @param in  Input
/// @param inlen Input length
/// @return Output length, 0 indicates failure
size_t Aes_128_wrap_evp(u_char const *key, u_char const *iv, u_char *out, u_char const *in, size_t inlen) noexcept
{
    return Aes_wrap_operation_evp(EVP_aes_128_wrap(), EVP_EncryptInit_ex, EVP_EncryptUpdate, EVP_EncryptFinal_ex, key,
                                  iv, out, in, inlen, kInt_16, Check_wrap_length);
}
/// @brief No-padding unwrap implementation
/// @param key Key
/// @param iv Initialization vector
/// @param out Output
/// @param in Input
/// @param inlen Input length
/// @return Output length, 0 indicates failure
size_t Aes_128_unwrap_evp(u_char const *key, u_char const *iv, u_char *out, u_char const *in, size_t inlen) noexcept
{
    return Aes_wrap_operation_evp(EVP_aes_128_wrap(), EVP_DecryptInit_ex, EVP_DecryptUpdate, EVP_DecryptFinal_ex, key,
                                  iv, out, in, inlen, kInt_24, Check_unwrap_length);
}
/// @brief With-padding wrap implementation (no custom check, any length ≥1)
/// @param key Key
/// @param icv Initialization vector
/// @param out Output
/// @param in Input
/// @param inlen Input length
/// @return Output length, 0 indicates failure
size_t Aes_128_wrap_pad_evp(u_char const *key, u_char const *icv, u_char *out, u_char const *in, size_t inlen) noexcept
{
    return Aes_wrap_operation_evp(EVP_aes_128_wrap_pad(), EVP_EncryptInit_ex, EVP_EncryptUpdate, EVP_EncryptFinal_ex,
                                  key, icv, out, in, inlen, kInt_1, nullptr);  // inlen≥1 is sufficient
}
/// @brief With-padding unwrap implementation (no custom check, any length ≥16)
/// @param key Key
/// @param icv Initialization vector
/// @param out Output
/// @param in Input
/// @param inlen Input length
/// @return Output length, 0 indicates failure
size_t Aes_128_unwrap_pad_evp(
    u_char const *key, u_char const *icv, u_char *out, u_char const *in, size_t inlen) noexcept
{
    return Aes_wrap_operation_evp(EVP_aes_128_wrap_pad(), EVP_DecryptInit_ex, EVP_DecryptUpdate, EVP_DecryptFinal_ex,
                                  key, icv, out, in, inlen, kInt_16, nullptr);
}
/// @brief AES wrap: key_unpad
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
                               size_t const keyLen) noexcept
{
// PRQA S 3048,3049 QAC /// @qac: This expression is a cast to or from a pointer to function type.
#if PUHUA_OPENSSL_IS_11X
    AES_KEY aeskey;
    AES_set_encrypt_key(key, static_cast< int32_t >(keyLen * kInt_8U), &aeskey);
    return CRYPTO_128_wrap(&aeskey, iv, out, in, static_cast< std::size_t >(inlen),
                           reinterpret_cast< block128_f >(&AES_encrypt));
#else
    std::ignore = keyLen;
    return Aes_128_wrap_evp(key, iv, out, in, static_cast< std::size_t >(inlen));
#endif
}
/// @brief AES unwrap: key_unpad
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
                                 size_t const keyLen) noexcept
{
// PRQA S 3048,3049 QAC /// @qac: This expression is a cast to or from a pointer to function type.
#if PUHUA_OPENSSL_IS_11X
    AES_KEY aeskey;
    AES_set_decrypt_key(key, static_cast< int32_t >(keyLen * kInt_8U), &aeskey);
    return CRYPTO_128_unwrap(&aeskey, iv, out, in, static_cast< std::size_t >(inlen),
                             reinterpret_cast< block128_f >(&AES_decrypt));
#else
    std::ignore = keyLen;
    return Aes_128_unwrap_evp(key, iv, out, in, static_cast< std::size_t >(inlen));
#endif
    // PRQA L:QAC
}
/// @brief AES wrap: key_pad
/// @param key AES_KEY key resource
/// @param iv Initialization vector
/// @param out Output buffer
/// @param in Input data buffer
/// @param inlen input data length
/// @return 0 sucess，fail otherwise
std::size_t AES_wrap_key_pad(u_char const *key,
                             u_char const *const iv,
                             u_char *const out,
                             u_char const *const in,
                             uint32_t const inlen,
                             size_t const keyLen) noexcept
{
// PRQA S 3048,3049 QAC /// @qac: This expression is a cast to or from a pointer to function type.
#if PUHUA_OPENSSL_IS_11X
    AES_KEY aeskey;
    AES_set_encrypt_key(key, static_cast< int32_t >(keyLen * kInt_8U), &aeskey);
    return CRYPTO_128_wrap_pad(&aeskey, iv, out, in, static_cast< std::size_t >(inlen),
                               reinterpret_cast< block128_f >(&AES_encrypt));
#else
    std::ignore = keyLen;
    return Aes_128_wrap_pad_evp(key, iv, out, in, static_cast< std::size_t >(inlen));
#endif
    // PRQA L:QAC
}
/// @brief AES unwrap: key_pad
/// @param key AES_KEY key resource
/// @param iv initialization vector
/// @param out output buffer
/// @param in input data buffer
/// @param inlen input data length
/// @return 0 sucess，fail otherwise
std::size_t AES_unwrap_key_pad(u_char const *key,
                               u_char const *const iv,
                               u_char *const out,
                               u_char const *const in,
                               uint32_t const inlen,
                               size_t const keyLen) noexcept
{
// PRQA S 3048,3049 QAC /// @qac: This expression is a cast to or from a pointer to function type.
#if PUHUA_OPENSSL_IS_11X
    AES_KEY aeskey;
    AES_set_decrypt_key(key, static_cast< int32_t >(keyLen * kInt_8U), &aeskey);
    return CRYPTO_128_unwrap_pad(&aeskey, iv, out, in, static_cast< std::size_t >(inlen),
                                 reinterpret_cast< block128_f >(&AES_decrypt));
#else
    std::ignore = keyLen;
    return Aes_128_unwrap_pad_evp(key, iv, out, in, static_cast< std::size_t >(inlen));
#endif
    // PRQA L:QAC
}
/// @brief Reverse BUFF content
/// @param out output buffer
/// @param in input data buffer
/// @param size input data length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00157
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
void Reverse_Buff(uint8_t *out, uint8_t const *const in, size_t const size) noexcept
{
    if (nullptr == out) {
        return;
    }
    size_t i{0U};
    if (nullptr != in) {
        for (i = 0U; i < size; ++i) {
            *(out + size - kInt_1 - i) = *(in + i);
        }
    } else {
        uint8_t *q{nullptr};
        uint8_t c{kInt8_0U};
        q = out + size - kInt_1;
        for (i = 0U; i < size / kInt_2U; ++i) {
            c  = *q;
            *q = *out;
            q--;
            *out = c;
            out++;
        }
    }
}
/// @brief DES unwrap: key
/// @param ks1 DES key 1
/// @param ks2 DES key 2
/// @param ks3 DES key 3
/// @param out output buffer
/// @param in input data buffer
/// @param inl input data length
/// @return 0 sucess，fail otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00160
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
int32_t DES_unwrap_key(
    void *ks1, void *ks2, void *ks3, uint8_t *const out, uint8_t const *in, size_t const inl) noexcept
{
    ara::core::Vector< uint8_t > vecIcv;
    vecIcv.resize(kInt_8U);
    uint8_t *const icv{vecIcv.data()};

    ara::core::Vector< uint8_t > vecIv;
    vecIv.resize(kInt_8U);
    uint8_t *const iv{vecIv.data()};

    ara::core::Vector< uint8_t > vecSha1tmp;
    vecSha1tmp.resize(static_cast< size_t >(SHA_DIGEST_LENGTH));
    uint8_t *const sha1tmp{vecSha1tmp.data()};

    ara::core::Vector< uint8_t > vecWorkiv;
    vecWorkiv.resize(kInt_16U);
    uint8_t *const workiv{vecWorkiv.data()}; /* working iv */

    uint8_t const wrapIv[kInt_8U]{0x4aU, 0xddU, 0xa2U, 0x2cU, 0x79U, 0xe8U, 0x21U, 0x05U};
    int32_t rv{-1};
    if (inl < kInt_24U) {
        return -1;
    }
    if (out == nullptr) {
        return static_cast< int32_t >(inl) - static_cast< int32_t >(kInt_16U);
    }

    std::ignore = memcpy(workiv, static_cast< void const * >(wrapIv), kInt_8U);
    /* Decrypt first block which will end up as icv */
    DES_ede3_cbc_encrypt_evp(static_cast< uint8_t const * >(in), static_cast< uint8_t * >(icv),
                             static_cast< int64_t >(kInt_8), static_cast< u_char * >(ks1), static_cast< u_char * >(ks2),
                             static_cast< u_char * >(ks3), static_cast< u_char * >(static_cast< void * >(workiv)), 0);
    /* Decrypt central blocks */
    /*
     * If decrypting in place move whole output along a block so the next
     * des_ede_cbc_cipher is in place.
     */
    if (out == in) {
        std::ignore = memmove(out, out + kInt_8, inl - kInt_8U);
        in -= kInt_8;
    }

    DES_ede3_cbc_encrypt_evp(in + kInt_8, out, static_cast< int64_t >(inl) - static_cast< int64_t >(kInt_16),
                             static_cast< u_char * >(ks1), static_cast< u_char * >(ks2), static_cast< u_char * >(ks3),
                             static_cast< u_char * >(static_cast< void * >(workiv)), 0);

    /* Decrypt final block which will be IV */
    DES_ede3_cbc_encrypt_evp(in + inl - kInt_8, static_cast< uint8_t * >(iv), static_cast< int64_t >(kInt_8),
                             static_cast< u_char * >(ks1), static_cast< u_char * >(ks2), static_cast< u_char * >(ks3),
                             static_cast< u_char * >(static_cast< void * >(workiv)), 0);

    /* Reverse order of everything */
    Reverse_Buff(static_cast< uint8_t * >(icv), nullptr, kInt_8U);
    Reverse_Buff(static_cast< uint8_t * >(out), nullptr, inl - kInt_16U);
    Reverse_Buff(static_cast< uint8_t * >(workiv), static_cast< uint8_t * >(iv), kInt_8U);

    /* Decrypt again using new IV */
    DES_ede3_cbc_encrypt_evp(out, out, static_cast< int64_t >(inl) - static_cast< int64_t >(kInt_16),
                             static_cast< u_char * >(ks1), static_cast< u_char * >(ks2), static_cast< u_char * >(ks3),
                             static_cast< u_char * >(static_cast< void * >(workiv)), 0);
    DES_ede3_cbc_encrypt_evp(static_cast< uint8_t const * >(icv), static_cast< uint8_t * >(icv),
                             static_cast< int64_t >(kInt_8), static_cast< u_char * >(ks1), static_cast< u_char * >(ks2),
                             static_cast< u_char * >(ks3), static_cast< u_char * >(static_cast< void * >(workiv)), 0);
    /* Work out SHA1 hash of first portion */
    std::ignore = SHA1(static_cast< uint8_t const * >(out), inl - kInt_16U, static_cast< uint8_t * >(sha1tmp));

    if (CRYPTO_memcmp(static_cast< void const * >(sha1tmp), static_cast< void const * >(icv), kInt_8U) == 0) {
        rv = static_cast< int32_t >(inl) - kInt_16;
    }
    OPENSSL_cleanse(static_cast< void * >(icv), kInt_8U);
    OPENSSL_cleanse(static_cast< void * >(sha1tmp), static_cast< size_t >(SHA_DIGEST_LENGTH));
    OPENSSL_cleanse(static_cast< void * >(iv), kInt_8U);
    OPENSSL_cleanse(static_cast< void * >(workiv), kInt_8U);
    if (rv == -1) {
        OPENSSL_cleanse(out, inl - kInt_16U);
    }

    return rv;
}
/// @brief DES wrap: key
/// @param ks1 DES key 1
/// @param ks2 DES key 2
/// @param ks3 DES key 3
/// @param out output buffer
/// @param in input data buffer
/// @param inl input data length
/// @return 0 sucess，fail otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00161
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
int32_t DES_wrap_key(
    void *ks1, void *ks2, void *ks3, uint8_t *const out, uint8_t const *const in, size_t const inl) noexcept
{
    ara::core::Vector< uint8_t > vecSha1tmp;
    vecSha1tmp.resize(static_cast< size_t >(SHA_DIGEST_LENGTH));
    uint8_t *const sha1tmp{vecSha1tmp.data()};

    ara::core::Vector< uint8_t > vecWorkiv;
    vecWorkiv.resize(kInt_16U);
    uint8_t *const workiv{vecWorkiv.data()}; /* working iv */

    uint8_t const wrapIv[kInt_8U]{0x4aU, 0xddU, 0xa2U, 0x2cU, 0x79U, 0xe8U, 0x21U, 0x05U};
    if (out == nullptr) {
        return static_cast< int32_t >(inl) + kInt_16;
    }
    /* Copy input to output buffer + kInt_8 so we have space for IV */
    std::ignore = memmove(out + kInt_8, in, inl);

    /* Work out ICV */
    std::ignore = SHA1(static_cast< uint8_t const * >(in), inl, static_cast< uint8_t * >(sha1tmp));

    std::ignore = memcpy(static_cast< void * >(out + inl + kInt_8), static_cast< void const * >(sha1tmp), kInt_8U);
    OPENSSL_cleanse(static_cast< void * >(sha1tmp), static_cast< size_t >(SHA_DIGEST_LENGTH));
    /* Generate random IV */
    if (RAND_bytes(static_cast< uint8_t * >(workiv), kInt_8) <= 0) {
        return -1;
    }
    std::ignore = memcpy(static_cast< void * >(out), static_cast< void const * >(workiv), kInt_8U);

    /* Encrypt everything after IV in place */
    DES_ede3_cbc_encrypt_evp(static_cast< uint8_t const * >(out + kInt_8), static_cast< uint8_t * >(out + kInt_8),
                             static_cast< int64_t >(inl) + static_cast< int64_t >(kInt_8), static_cast< u_char * >(ks1),
                             static_cast< u_char * >(ks2), static_cast< u_char * >(ks3),
                             static_cast< u_char * >(static_cast< void * >(workiv)), kInt_1);

    Reverse_Buff(out, nullptr, inl + kInt_16U);

    std::ignore = memcpy(static_cast< void * >(workiv), static_cast< void const * >(wrapIv), kInt_8U);
    DES_ede3_cbc_encrypt_evp(out, out, static_cast< int64_t >(inl) + static_cast< int64_t >(kInt_16),
                             static_cast< u_char * >(ks1), static_cast< u_char * >(ks2), static_cast< u_char * >(ks3),
                             static_cast< u_char * >(static_cast< void * >(workiv)), kInt_1);

    return static_cast< int32_t >(inl) + kInt_16;
}
/// @brief AES string encryption
/// @param plaintext plaintext data
/// @param startOffset encryption position offset
/// @param plaintextLen plaintext data length
/// @param iv initialization vector
/// @param ciphertext ciphertext data
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
                     int32_t keyBits) noexcept
{
    std::ignore = plaintext;
    /*counter*/
    ara::crypto::internal::PAutoBuff ctr{static_cast< uint32_t >(AES_BLOCK_SIZE)};
    ctr.ResetData();
    uint8_t *const pData{ctr.Data()};
    /*result of counter and key encryption*/
    ara::crypto::internal::PAutoBuff streamBlock{static_cast< uint32_t >(AES_BLOCK_SIZE)};
    streamBlock.ResetData();
    uint8_t *const pDataEncrypt{streamBlock.Data()};
    std::ignore = memcpy(pData, iv, static_cast< size_t >(AES_BLOCK_SIZE));

    int32_t const blockOffset{startOffset / AES_BLOCK_SIZE}; /*calculate which block to start from*/
    int32_t withinBlockOffset{startOffset % AES_BLOCK_SIZE}; /*calculate which position within the block to start from*/
    int32_t outputIndex{0};                                  /*output plaintext index*/
    int32_t remainingLen{plaintextLen};                      /*data length to be processed*/

    // // //
    // Adjust the counter value so that its value is different for each 16-byte block and consistent with the decryption rules; functionally equivalent to the commented code below but with fewer operations
    for (int32_t i{0}; i < blockOffset; i++) {
        Increment_counter(pData, AES_BLOCK_SIZE);
    }

    // Initial counter value encryption
    AES_encrypt_evp(pData, pDataEncrypt, originalKey, keyBits);

    // Encrypt specified length of data starting from the given offset
    while (true) {
        if (remainingLen <= 0) {
            break;
        }
        int32_t const bytesToProcess{std::min(AES_BLOCK_SIZE - withinBlockOffset, remainingLen)};
        for (int32_t i{0}; i < bytesToProcess; ++i) {
            *(ciphertext + outputIndex) = *(plaintext + outputIndex) ^ *(pDataEncrypt + withinBlockOffset + i);
            outputIndex++;
        }
        withinBlockOffset = 0;          /*next block starts from the first byte*/
        remainingLen -= bytesToProcess; /*update remaining data length*/

        // Update counter value and encrypted counter
        if (remainingLen > 0) {
            Increment_counter(pData, AES_BLOCK_SIZE);
            AES_encrypt_evp(pData, pDataEncrypt, originalKey, keyBits);
        }
    }
}
/// @brief Private key decryption, replacement for RSA_private_decrypt
/// @param flen input length
/// @param from input content
/// @param to   output content
/// @param rsa  private key
/// @param padding whether to pad
/// @return -1 on failure
int32_t Rsa_private_decrypt_evp(int32_t flen, const u_char *from, u_char *to, RSA *rsa, int32_t padding) noexcept
{
    // Create EVP_PKEY and assign RSA key
    EVP_PKEY *pkey = EVP_PKEY_new();
    if (pkey == nullptr || EVP_PKEY_assign_RSA(pkey, rsa) != kInt_1) {  // NOLINT
        if (pkey != nullptr) {
            EVP_PKEY_free(pkey);
        }
        return -1;
    }

    // Create decryption context
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (ctx != nullptr) {
        EVP_PKEY_free(pkey);
        return -1;
    }

    int32_t result = -1;

    // Perform decryption operation
    if (EVP_PKEY_decrypt_init(ctx) > 0 && EVP_PKEY_CTX_set_rsa_padding(ctx, padding) > 0) {
        size_t outlen = flen;
        if (EVP_PKEY_decrypt(ctx, to, &outlen, from, flen) > 0) {
            result = static_cast< int32_t >(outlen);
        }
    }

    // Clean up resources
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);

    return result;
}
/// @brief Replacement for rsa_public_encrypt, public key encryption
/// @param flen input length
/// @param from input content
/// @param to   output content
/// @param rsa  public key
/// @param padding padding
/// @return -1 on failure
int32_t Rsa_public_encrypt_evp(int32_t flen, const u_char *from, u_char *to, RSA *rsa, int32_t padding) noexcept
{
    EVP_PKEY *pkey = EVP_PKEY_new();
    if (pkey == nullptr) {
        return -1;
    }

    // Assign RSA key to EVP_PKEY
    if (EVP_PKEY_assign_RSA(pkey, rsa) != kInt_1) {  // NOLINT
        EVP_PKEY_free(pkey);
        return -1;
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (ctx == nullptr) {
        EVP_PKEY_free(pkey);
        return -1;
    }

    int32_t result = -1;

    // Initialize encryption operation
    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return -1;
    }

    // Set padding mode
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return -1;
    }

    size_t outlen = flen;  // Initial output buffer size
    if (EVP_PKEY_encrypt(ctx, to, &outlen, from, flen) > 0) {
        result = static_cast< int32_t >(outlen);  // Return actual encrypted data length
    }

    // Clean up resources
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);

    return result;
}
/// @brief Copy a key (RSA or ECC)
/// @param src EVP_PKEY type key
/// @return key
EVP_PKEY *Evp_pkey_dup(EVP_PKEY *src, bool pri) noexcept
{
    if (src == nullptr) {
        return nullptr;
    }
    // Create new EVP_PKEY
    EVP_PKEY *dst = EVP_PKEY_new();
    if (dst == nullptr) {
        return nullptr;
    }

#if PUHUA_OPENSSL_IS_11X
    // Check if source key is RSA type
    if (EVP_PKEY_id(src) == EVP_PKEY_RSA) {
        // Extract RSA structure from EVP_PKEY
        RSA *rsaSrc = EVP_PKEY_get0_RSA(src);
        if (rsaSrc == nullptr) {
            return nullptr;
        }
        RSA *rsaDup = nullptr;
        // Copy RSA structure
        if (pri) {
            rsaDup = RSAPrivateKey_dup(rsaSrc);
        } else {
            rsaDup = RSAPublicKey_dup(rsaSrc);
        }
        if (rsaDup == nullptr) {
            EVP_PKEY_free(dst);
            return nullptr;
        }
        // Associate copied RSA structure with new EVP_PKEY
        if (EVP_PKEY_assign_RSA(dst, rsaDup) != kInt_1) {  // NOLINT
            EVP_PKEY_free(dst);
            RSA_free(rsaDup);
            return nullptr;
        }
        return dst;
    }

    if (EVP_PKEY_id(src) == EVP_PKEY_EC) {  // Check if source key is ECC type
        if (pri) {
            return Evp_pkey_ecc_dup_priv(src);
        }
        return Evp_pkey_ecc_dup_pub(src);
    }
    return nullptr;
#else
    std::ignore = pri;
    return EVP_PKEY_dup(src);
#endif
}

}  // namespace crypto
}  // namespace ara