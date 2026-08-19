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
// Reference upstream source:
// https://github.com/openssl/openssl/blob/openssl-3.5.1/crypto/cmac/cmac.c
//
// Applicable copyright notice for the OpenSSL-derived portions:
// Copyright 2010-2024 The OpenSSL Project Authors. All Rights Reserved.
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
/// @file       isoft_openssl_cmac.cpp
/// @brief      AutoSar-Crypto Configuration
/// @details
/// @date       2023-08-14
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Message Authentication Code
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=MessageAuthnCodeCtx
/// @endcode
///
/// ================================================================

#include "ara/crypto/openssl/isoft_openssl_cmac.h"

#include <openssl/err.h>

#include <cstring>
#include <tuple>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

#define BUF_SIZE_MAX_CMC 2048
namespace ara {
namespace crypto {
namespace openssl {
namespace isoft_def {

/// @brief Generate subkeys required by CMAC algorithm based on input key L
/// @param k1 Pointer to buffer holding subkey k1
/// @param l  Input CMAC master key
/// @param bl Key length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00253
/// @trace_id_dd=DD_CRYPTO_06646
/// @needwork = dd
/// @endcode
static void MakeKeyN(uint8_t *const k1, uint8_t const *const l, int32_t const bl) noexcept
{
    int32_t i{0};
    uint8_t c{*(l + 0)};
    uint8_t const carry{static_cast< decltype(c) >(c >> 7U)};
    uint8_t cnext{0U};

    // Copy the first bl bytes of master key l directly to k1 as subkey k1
    for (i = 0; i < bl - 1; i++) {
        cnext     = *(l + i + 1);
        *(k1 + i) = static_cast< decltype(c) >(c << 1U) | static_cast< decltype(c) >((cnext >> ara::crypto::kInt_7U));
        c         = cnext;
    }

    /* If MSB set fixup with R */
    uint8_t tempValue{0U};
    if (bl == ara::crypto::kInt_16) {
        tempValue = kInt8_0x87U;
    } else {
        tempValue = kInt8_0x1bU;
    }
    *(k1 + i) = static_cast< decltype(c) >(c << 1U)
                ^ static_cast< decltype(c) >((static_cast< decltype(c) >(0U - carry))
                                             & static_cast< decltype(c) >(tempValue));
}

/// @brief Allocate and initialize a CMAC algorithm-related context structure CMAC_CTX, allocate memory, and initialize related fields.
/// @returns Returns a pointer of type CMAC_CTX. Developers can use this CTX handle for subsequent CMAC operations.
CMAC_CTX *CMAC_CTX_new() noexcept
{
    CMAC_CTX *const ctx{static_cast< CMAC_CTX * >(OPENSSL_malloc(sizeof(CMAC_CTX)))};

    if (nullptr == ctx) {
        CRYPTOerr(CRYPTO_F_CMAC_CTX_NEW, ERR_R_MALLOC_FAILURE);  // NOLINT
        return nullptr;
    }
    ctx->cctx = EVP_CIPHER_CTX_new();
    if (ctx->cctx == nullptr) {
        OPENSSL_free(ctx);
        return nullptr;
    }
    ctx->nLastBlock = -1;
    return ctx;
}

/// @brief Clean up the contents of the CMAC_CTX type context, release heap-allocated memory blocks like keys inside CTX, reset CTX fields to initial uninitialized state.
/// @param ctx CMAC algorithm context to clean up
void CMAC_CTX_cleanup(CMAC_CTX *const ctx) noexcept
{
    std::ignore = EVP_CIPHER_CTX_reset(ctx->cctx);
    OPENSSL_cleanse(static_cast< void * >(ctx->tbl), static_cast< size_t >(EVP_MAX_BLOCK_LENGTH));
    if (ctx->tbl[0U] == 0U) {
    }
    OPENSSL_cleanse(static_cast< void * >(ctx->k1), static_cast< size_t >(EVP_MAX_BLOCK_LENGTH));
    if (ctx->k1[0U] == 0U) {
    }
    OPENSSL_cleanse(static_cast< void * >(ctx->k2), static_cast< size_t >(EVP_MAX_BLOCK_LENGTH));
    if (ctx->k2[0U] == 0U) {
    }
    OPENSSL_cleanse(static_cast< void * >(ctx->lastBlock), static_cast< size_t >(EVP_MAX_BLOCK_LENGTH));
    if (ctx->lastBlock[0U] == 0U) {
    }
    ctx->nLastBlock = -1;
}

/// @brief Release the memory space occupied by the given CMAC_CTX object, call _CMAC_CTX_cleanup() to clean up CTX contents, prevent CTX from being reused.
/// @param ctx CMAC algorithm context to free
void CMAC_CTX_free(CMAC_CTX *const ctx) noexcept
{
    if (ctx == nullptr) {
        return;
    }
    CMAC_CTX_cleanup(ctx);
    EVP_CIPHER_CTX_free(ctx->cctx);
    OPENSSL_free(ctx);
}

/// @brief Get the internal EVP_CIPHER_CTX handle from the given CMAC_CTX object. EVP_CIPHER_CTX contains underlying encryption algorithm info used for CMAC calculation.
/// @param ctx CMAC context
/// @returns Return value is a pointer of type EVP_CIPHER_CTX, representing the retrieved underlying encryption context.
EVP_CIPHER_CTX *CMAC_CTX_get0_cipher_ctx(CMAC_CTX const *const ctx) noexcept { return ctx->cctx; }

/// @brief Copy CMAC context: Copy the CMAC context specified by 'in' to the CTX specified by 'out', making 'out' a complete copy of 'in'.
/// @param out Destination CMAC_CTX pointer for copying
/// @param in  Source CMAC_CTX to copy
/// @returns Return 1 on success, 0 on failure
int32_t CMAC_CTX_copy(CMAC_CTX *const out, CMAC_CTX const *const in) noexcept
{
    int32_t bl{0};
    if (in->nLastBlock == -1) {
        return 0;
    }
    if (EVP_CIPHER_CTX_copy(out->cctx, in->cctx) == 0) {
        return 0;
    }
    bl = EVP_CIPHER_CTX_block_size(in->cctx);
    std::ignore
        = memcpy(static_cast< void * >(out->k1), static_cast< void const * >(in->k1), static_cast< size_t >(bl));
    if (out->k1[0U] == 0U) {
    }
    std::ignore
        = memcpy(static_cast< void * >(out->k2), static_cast< void const * >(in->k2), static_cast< size_t >(bl));
    if (out->k2[0U] == 0U) {
    }
    std::ignore
        = memcpy(static_cast< void * >(out->tbl), static_cast< void const * >(in->tbl), static_cast< size_t >(bl));
    if (out->tbl[0U] == 0U) {
    }
    std::ignore = memcpy(static_cast< void * >(out->lastBlock), static_cast< void const * >(in->lastBlock),
                         static_cast< size_t >(bl));
    if (out->lastBlock[0U] == 0U) {
    }
    out->nLastBlock = in->nLastBlock;
    return 1;
}

/// @brief Pass parameters to initialize cmac context
/// @param ctx Pointer to CMAC context
/// @param key CMAC key
/// @param keylen Key length
/// @param cipher Key algorithm used for message authentication, e.g., AES
/// @param impl Implementation engine, usually NULL
/// @param piv Pointer to initialization vector
/// @returns Return 1 on success, 0 on failure
int32_t CMAC_Init(CMAC_CTX *const ctx,
                  void const *const key,
                  size_t const keylen,
                  EVP_CIPHER const *const cipher,
                  ENGINE *const impl,
                  uint8_t const *const piv) noexcept
{
#if PUHUA_OPENSSL_IS_11X
    /* All zeros means restart */
    if ((key == nullptr) && (cipher == nullptr) && (impl == nullptr) && (keylen == 0U)) {
        /* Not initialised */
        if (ctx->nLastBlock == -1) {
            return 0;
        }
        if (EVP_EncryptInit_ex(ctx->cctx, nullptr, nullptr, nullptr, piv) == 0) {
            return 0;
        }
        std::ignore
            = memset(static_cast< void * >(ctx->tbl), 0, static_cast< size_t >(EVP_CIPHER_CTX_block_size(ctx->cctx)));
        if (ctx->tbl[0U] == 0U) {
        }
        ctx->nLastBlock = 0;
        return 0;
    }
    // Initialize an EVP_CIPHER_CTX context for symmetric encryption.
    if (cipher != nullptr) {
        if (EVP_EncryptInit_ex(ctx->cctx, cipher, impl, nullptr, nullptr) == 0) {
            return 0;
        }
    }

    /* Non-nullptr key means initialisation complete */
    if (nullptr != key) {
        int32_t bl{0};
        if (EVP_CIPHER_CTX_cipher(ctx->cctx)
            == nullptr) {  // Get symmetric encryption algorithm info used by the context.
            return 0;
        }
        if (EVP_CIPHER_CTX_set_key_length(ctx->cctx, static_cast< int32_t >(keylen)) == 0) {  // Set key length
            return 0;
        }
        uint8_t const *const pKey{static_cast< uint8_t const * >(key)};
        if (EVP_EncryptInit_ex(ctx->cctx, nullptr, nullptr, pKey, piv) == 0) {
            return 0;
        }
        bl = EVP_CIPHER_CTX_block_size(
            ctx->cctx);  // Get the block size of the symmetric encryption algorithm associated with the specified EVP_CIPHER_CTX context.
        // EVP_Cipher is the basic interface structure implementing symmetric encryption algorithms
        if (EVP_Cipher(ctx->cctx, static_cast< uint8_t * >(ctx->tbl), piv, static_cast< uint32_t >(bl)) == 0) {
            return 0;
        }
        MakeKeyN(static_cast< uint8_t * >(ctx->k1), static_cast< uint8_t const * >(ctx->tbl), bl);
        MakeKeyN(static_cast< uint8_t * >(ctx->k2), static_cast< uint8_t const * >(ctx->k1), bl);
        OPENSSL_cleanse(static_cast< void * >(ctx->tbl), static_cast< size_t >(bl));
        if (ctx->tbl[0U] == 0U) {
        }
        /* Reset context again ready for first data block */
        if (EVP_EncryptInit_ex(ctx->cctx, nullptr, nullptr, nullptr, piv) == 0) {
            return 0;
        }
        /* Zero tbl so resume works */
        std::ignore = memset(static_cast< void * >(ctx->tbl), 0, static_cast< size_t >(bl));
        if (ctx->tbl[0U] == 0U) {
        }
        ctx->nLastBlock = 0;
    }
    return 1;
#else
    int blockLen{0};

    /* All zeros means restart */
    if (key == nullptr && cipher == nullptr && impl == nullptr && keylen == 0) {
        /* Not initialised */
        if (ctx->nLastBlock == -1) {
            return 0;
        }
        if (EVP_EncryptInit_ex2(ctx->cctx, nullptr, nullptr, piv, nullptr) == 0) {
            return 0;
        }
        blockLen = EVP_CIPHER_CTX_get_block_size(ctx->cctx);
        if (blockLen == 0) {
            return 0;
        }
        memset(ctx->tbl, 0, blockLen);
        ctx->nLastBlock = 0;
        return 1;
    }
    /* Initialise context */
    if (cipher != nullptr) {
        /* Ensure we can't use this ctx until we also have a key */
        ctx->nLastBlock = -1;
        if (impl != nullptr) {
            if (EVP_EncryptInit_ex(ctx->cctx, cipher, impl, nullptr, nullptr) == 0) {
                return 0;
            }
        } else {
            if (EVP_EncryptInit_ex2(ctx->cctx, cipher, nullptr, nullptr, nullptr) == 0) {
                ERR_print_errors_fp(stderr);  // Output full error stack to standard error stream
                return 0;
            }
        }
    }
    /* Non-nullptr key means initialisation complete */
    if (key != nullptr) {
        int bl{0};

        /* If anything fails then ensure we can't use this ctx */
        ctx->nLastBlock = -1;
        if (EVP_CIPHER_CTX_get0_cipher(ctx->cctx) == nullptr) {
            return 0;
        }
        if (EVP_CIPHER_CTX_set_key_length(ctx->cctx, static_cast< int32_t >(keylen)) <= 0) {
            return 0;
        }
        if (EVP_EncryptInit_ex2(ctx->cctx, nullptr, static_cast< uint8_t const * >(key), piv, nullptr) == 0) {
            return 0;
        }
        if ((bl = EVP_CIPHER_CTX_get_block_size(ctx->cctx)) < 0) {
            return 0;
        }
        if (EVP_Cipher(ctx->cctx, ctx->tbl, piv, bl) <= 0) {
            return 0;
        }
        MakeKeyN(ctx->k1, ctx->tbl, bl);
        MakeKeyN(ctx->k2, ctx->k1, bl);
        OPENSSL_cleanse(ctx->tbl, bl);
        /* Reset context again ready for first data block */
        if (EVP_EncryptInit_ex2(ctx->cctx, nullptr, nullptr, piv, nullptr) == 0) {
            return 0;
        }
        /* Zero tbl so resume works */
        memset(ctx->tbl, 0, bl);
        ctx->nLastBlock = 0;
    }
    return 1;
#endif
}

/// @brief This function can be called multiple times to feed data into CMAC calculation in chunks. Finally, CMAC_Final() is needed to terminate data collection and calculate the final CMAC value.
/// @param ctx CMAC context pointer
/// @param in Buffer of data to be authenticated
/// @param dlen Data length
/// @returns Return 1 on success, 0 on failure
int32_t CMAC_Update(CMAC_CTX *const ctx, void const *const in, size_t dlen) noexcept
{
#if PUHUA_OPENSSL_IS_11X
    uint8_t const *data{static_cast< uint8_t const * >(in)};
    size_t bl{0U};
    if (ctx->nLastBlock == -1) {
        return 0;
    }
    if (dlen == 0U) {
        return 0;
    }
    bl = static_cast< size_t >(EVP_CIPHER_CTX_block_size(ctx->cctx));
    /* Copy into partial block if we need to */
    if (ctx->nLastBlock > 0) {
        size_t nleft{0U};
        nleft = bl - static_cast< size_t >(ctx->nLastBlock);
        if (dlen < nleft) {
            nleft = dlen;
        }
        std::ignore = memcpy(ctx->lastBlock + ctx->nLastBlock, data, nleft);
        if (ctx->lastBlock[0U] == 0U) {
        }
        dlen -= nleft;
        ctx->nLastBlock += static_cast< int32_t >(nleft);
        /* If no more to process return */
        if (dlen == 0U) {
            return 0;
        }
        data += nleft;
        /* Else not final block so encrypt it */
        if (EVP_Cipher(ctx->cctx, static_cast< uint8_t * >(ctx->tbl), static_cast< uint8_t const * >(ctx->lastBlock),
                       static_cast< uint32_t >(bl))
            == 0) {
            return 0;
        }
    }
    /* Encrypt all but one of the complete blocks left */
    while (dlen > bl) {
        if (EVP_Cipher(ctx->cctx, static_cast< uint8_t * >(ctx->tbl), data, static_cast< uint32_t >(bl)) == 0) {
            return 0;
        }
        dlen -= bl;
        data += bl;
    }
    /* Copy any data left to last block buffer */
    std::ignore = memcpy(static_cast< void * >(ctx->lastBlock), data, dlen);
    if (ctx->lastBlock[0U] == 0U) {
    }
    ctx->nLastBlock = static_cast< int32_t >(dlen);
    return 1;
#else
    const u_char *data = static_cast< const u_char * >(in);
    int bl{0};
    size_t maxBurstBlocks{0U};
    size_t cipherBlocks{0U};
    u_char buf[BUF_SIZE_MAX_CMC];

    if (ctx->nLastBlock == -1) {
        return 0;
    }
    if (dlen == 0) {
        return 1;
    }
    if ((bl = EVP_CIPHER_CTX_get_block_size(ctx->cctx)) == 0) {
        return 0;
    }
    /* Copy into partial block if we need to */
    if (ctx->nLastBlock > 0) {
        size_t nleft{0};

        nleft = bl - ctx->nLastBlock;
        if (dlen < nleft) {
            nleft = dlen;
        }
        memcpy(ctx->lastBlock + ctx->nLastBlock, data, nleft);
        dlen -= nleft;
        ctx->nLastBlock += static_cast< int32_t >(nleft);
        /* If no more to process return */
        if (dlen == 0) {
            return 1;
        }
        data += nleft;
        /* Else not final block so encrypt it */
        if (EVP_Cipher(ctx->cctx, ctx->tbl, ctx->lastBlock, bl) <= 0) {
            return 0;
        }
    }
    /* Encrypt all but one of the complete blocks left */

    maxBurstBlocks = BUF_SIZE_MAX_CMC / bl;
    cipherBlocks   = (dlen - 1) / bl;
    if (maxBurstBlocks == 0) {
        /*
            * When block length is greater than local buffer size,
            * use ctx->tbl as cipher output.
            */
        while (dlen > static_cast< size_t >(bl)) {
            if (EVP_Cipher(ctx->cctx, ctx->tbl, data, bl) <= 0) {
                return 0;
            }
            dlen -= bl;
            data += bl;
        }
    } else {
        while (cipherBlocks > maxBurstBlocks) {
            if (EVP_Cipher(ctx->cctx, buf, data, maxBurstBlocks * bl) <= 0) {
                return 0;
            }
            dlen -= maxBurstBlocks * bl;
            data += maxBurstBlocks * bl;
            cipherBlocks -= maxBurstBlocks;
        }
        if (cipherBlocks > 0) {
            if (EVP_Cipher(ctx->cctx, buf, data, cipherBlocks * bl) <= 0) {
                return 0;
            }
            dlen -= cipherBlocks * bl;
            data += cipherBlocks * bl;
            memcpy(ctx->tbl, &buf[(cipherBlocks - 1) * bl], bl);
        }
    }
    /* Copy any data left to last block buffer */
    memcpy(ctx->lastBlock, data, dlen);
    ctx->nLastBlock = static_cast< int32_t >(dlen);
    return 1;
#endif
}

/// @brief Terminate CMAC operation, calculate the final message authentication code, and output the result to the 'out' buffer.
/// @param ctx CMAC context pointer
/// @param out Buffer holding the output CMAC value
/// @param poutlen Length of the output CMAC value
/// @returns Return 1 on success, 0 on failure
int32_t CMAC_Final(CMAC_CTX *const ctx, uint8_t *const out, size_t *const poutlen) noexcept
{
#if PUHUA_OPENSSL_IS_11X
    int32_t i{0};
    int32_t bl{0};
    int32_t lb{0};
    if (ctx->nLastBlock == -1) {
        return 0;
    }
    bl       = EVP_CIPHER_CTX_block_size(ctx->cctx);
    *poutlen = static_cast< size_t >(bl);
    if (out == nullptr) {
        return 0;
    }
    lb = ctx->nLastBlock;
    /* Is last block complete? */
    if (lb == bl) {
        for (i = 0; i < bl; i++) {
            *(out + i) = ctx->lastBlock[i] ^ ctx->k1[i];  // NOLINT
        }
    } else {
        ctx->lastBlock[lb] = kInt8_0x80U;  // NOLINT
        if (bl - lb > 1) {
            int32_t const res{bl - lb - 1};
            std::ignore = memset(ctx->lastBlock + lb + 1, 0, static_cast< size_t >(res));
        }
        for (i = 0; i < bl; i++) {
            *(out + i) = ctx->lastBlock[i] ^ ctx->k2[i];  // NOLINT
        }
    }
    if (EVP_Cipher(ctx->cctx, out, out, static_cast< uint32_t >(bl)) == 0) {
        OPENSSL_cleanse(out, static_cast< size_t >(bl));
        return 0;
    }
    return 1;
#else
    int i{0};
    int bl{0};
    int lb{0};

    if (ctx->nLastBlock == -1) {
        return 0;
    }
    if ((bl = EVP_CIPHER_CTX_get_block_size(ctx->cctx)) == 0) {
        return 0;
    }
    if (poutlen != nullptr) {
        *poutlen = static_cast< size_t >(bl);
    }
    if (out == nullptr) {
        return 1;
    }
    lb = ctx->nLastBlock;
    /* Is last block complete? */
    if (lb == bl) {
        for (i = 0; i < bl; i++) {
            out[i] = ctx->lastBlock[i] ^ ctx->k1[i];
        }
    } else {
        ctx->lastBlock[lb] = kInt8_0x80U;
        if (bl - lb > 1) {
            memset(ctx->lastBlock + lb + 1, 0, bl - lb - 1);
        }
        for (i = 0; i < bl; i++) {
            out[i] = ctx->lastBlock[i] ^ ctx->k2[i];
        }
    }
    if (EVP_Cipher(ctx->cctx, out, out, bl) <= 0) {
        OPENSSL_cleanse(out, bl);
        return 0;
    }
    return 1;
#endif
}

/// @brief Restore CMAC context 'ctx' to a state ready to start calculating MAC. If ctx is paused, reset it for subsequent operations.
/// @param ctx CMAC context pointer
/// @returns Return 1 on success, 0 on failure
int32_t CMAC_resume(CMAC_CTX const *const ctx) noexcept
{
    if (ctx->nLastBlock == -1) {
        return 0;
    }
    /*
     * The buffer "tbl" contains the last fully encrypted block which is the
     * last IV (or all zeroes if no last encrypted block). The last block has
     * not been modified since CMAC_final(). So reinitialising using the last
     * decrypted block will allow CMAC to continue after calling
     * CMAC_Final().
     */
    return EVP_EncryptInit_ex(ctx->cctx, nullptr, nullptr, nullptr, static_cast< uint8_t const * >(ctx->tbl));
}

}  // namespace  isoft_def
}  // namespace openssl
}  // namespace crypto
}  // namespace ara
