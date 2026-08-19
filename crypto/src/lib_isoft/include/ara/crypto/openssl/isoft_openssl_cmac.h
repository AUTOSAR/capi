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
// https://github.com/openssl/openssl/blob/openssl-3.5.1/include/openssl/cmac.h
//
// Applicable copyright notice for the OpenSSL-derived portions:
// Copyright 2010-2020 The OpenSSL Project Authors. All Rights Reserved.
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
/// @file       isoft_openssl_cmac.h
/// @brief      AutoSar-Crypto configuration
/// @details    Function definitions in the openssl library cmac.h
/// @date       2023-08-14
/// @author     Che Jinzhao
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author       <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Message Authentication Code
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=MessageAuthnCodeCtx
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_OPENSSL_PUHUA_OPENSSL_CAMC_H_
#define ARA_CRYPTO_OPENSSL_PUHUA_OPENSSL_CAMC_H_

#include <ara/crypto/common/isoft_data_type.h>
#include <openssl/evp.h>

namespace ara {
namespace crypto {
namespace openssl {
// PRQA S 2502 QAC /// @qac: false positive
/// @qac Possibly cannot modify [2502]: This name hides a similar kind of declaration.
namespace isoft_def {
// PRQA L:QAC
/// @brief MAC structure data used by openssl
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01573
/// @trace_id_dd=DD_CRYPTO_04092
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=CMAC_CTX_struct
/// @needwork = ad
/// @endcode
struct CMAC_CTX_struct
{
    /// @brief /* Cipher context to use */
    EVP_CIPHER_CTX *cctx;
    /// @brief /* Keys k1 and k2 */
    uint8_t k1[EVP_MAX_BLOCK_LENGTH];
    /// @brief /* Keys k1 and k2 */
    uint8_t k2[EVP_MAX_BLOCK_LENGTH];
    /// @brief /* Temporary block data buffer*/
    uint8_t tbl[EVP_MAX_BLOCK_LENGTH];
    /// @brief /* Last (possibly partial) block */
    uint8_t lastBlock[EVP_MAX_BLOCK_LENGTH];
    /// @brief /* Number of bytes in last block: -1 means context not initialised */
    int32_t nLastBlock;
};
/// @brief Alias for _CMAC_CTX_struct
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03160
/// @trace_id_dd=DD_CRYPTO_06363
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=CMAC_CTX
/// @needwork = ad
/// @endcode
using CMAC_CTX = CMAC_CTX_struct;

/// @brief Allocate and initialize a CMAC algorithm related context structure CMAC_CTX, allocate memory and initialize related fields.
/// @name   CMAC_CTX_new
/// @returns Returns a pointer of type CMAC_CTX. Developers can use this CTX handle for subsequent CMAC operations.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00253
/// @trace_id_dd=DD_CRYPTO_04093
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=CMAC_CTX_new
/// @needwork = dd
/// @endcode
CMAC_CTX *CMAC_CTX_new() noexcept;

/// @brief Clean up the content in the CMAC_CTX type context, release heap-allocated memory such as the key memory block in the CTX, and reset CTX fields to the initial uninitialized state.
/// @name   CMAC_CTX_cleanup
/// @param ctx CMAC algorithm context to be cleaned up
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00253
/// @trace_id_dd=DD_CRYPTO_04094
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=CMAC_CTX_cleanup
/// @needwork = dd
/// @endcode
void CMAC_CTX_cleanup(CMAC_CTX *const ctx) noexcept;

/// @brief Release the memory space occupied by the given CMAC_CTX object, call _CMAC_CTX_cleanup() to clean up the CTX content to prevent the CTX from being used again.
/// @name   CMAC_CTX_free
/// @param ctx CMAC algorithm context to be freed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00253
/// @trace_id_dd=DD_CRYPTO_04095
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=CMAC_CTX_free
/// @needwork = dd
/// @endcode
void CMAC_CTX_free(CMAC_CTX *const ctx) noexcept;

/// @brief Obtain the internal EVP_CIPHER_CTX handle from the given CMAC_CTX object. The EVP_CIPHER_CTX contains the underlying encryption algorithm information used to compute CMAC.
/// @name   CMAC_CTX_get0_cipher_ctx
/// @param ctx CMAC context
/// @returns The function returns a pointer of type EVP_CIPHER_CTX, representing the obtained underlying encryption context.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00253
/// @trace_id_dd=DD_CRYPTO_04096
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=CMAC_CTX_get0_cipher_ctx
/// @needwork = dd
/// @endcode
EVP_CIPHER_CTX *CMAC_CTX_get0_cipher_ctx(CMAC_CTX const *const ctx) noexcept;

/// @brief Copy CMAC context: copy the CMAC context specified by 'in' to the CTX specified by 'out', making 'out' a complete copy of 'in'.
/// @name   CMAC_CTX_copy
/// @param out pointer to the destination CMAC_CTX
/// @param in  source CMAC_CTX to be copied
/// @returns Returns 1 on success, 0 on failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00253
/// @trace_id_dd=DD_CRYPTO_04097
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=CMAC_CTX_copy
/// @needwork = dd
/// @endcode
int32_t CMAC_CTX_copy(CMAC_CTX *const out, CMAC_CTX const *const in) noexcept;

/// @brief Pass parameters to initialize the cmac context
/// @name   CMAC_Init
/// @param ctx pointer to CMAC context
/// @param key CMAC key
/// @param keylen key length
/// @param cipher key algorithm used for message authentication, such as AES
/// @param impl   implementation engine, usually NULL
/// @param piv pointer to initialization vector
/// @returns Returns 1 on success, 0 on failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00253
/// @trace_id_dd=DD_CRYPTO_04098
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=CMAC_Init
/// @needwork = dd
/// @endcode
int32_t CMAC_Init(CMAC_CTX *const ctx,
                  void const *const key,
                  size_t const keylen,
                  EVP_CIPHER const *const cipher,
                  ENGINE *const impl,
                  uint8_t const *const piv) noexcept;

/// @brief This function can be called multiple times to pass data in multiple portions to the CMAC calculation. Finally, CMAC_Final() is needed to complete the collection of all data and calculate the final CMAC value.
/// @name   CMAC_Update
/// @param ctx pointer to CMAC context
/// @param in data buffer to be authenticated
/// @param dlen data length
/// @returns Returns 1 on success, 0 on failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00253
/// @trace_id_dd=DD_CRYPTO_04099
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=CMAC_Update
/// @needwork = dd
/// @endcode
int32_t CMAC_Update(CMAC_CTX *const ctx, void const *const in, size_t dlen) noexcept;

/// @brief Terminate the CMAC operation, calculate the final message authentication code, and output the result to the out buffer.
/// @name   CMAC_Final
/// @param ctx pointer to CMAC context
/// @param out buffer to store the output CMAC value
/// @param poutlen  length of the output CMAC value
/// @returns Returns 1 on success, 0 on failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00253
/// @trace_id_dd=DD_CRYPTO_04100
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=CMAC_Final
/// @needwork = dd
/// @endcode
int32_t CMAC_Final(CMAC_CTX *const ctx, uint8_t *const out, size_t *const poutlen) noexcept;

/// @brief Restore the CMAC context 'ctx' to a state where it can start computing MAC. If ctx is in a suspended state, reset it for subsequent operations.
/// @name   CMAC_resume
/// @param ctx  pointer to CMAC context
/// @returns Returns 1 on success, 0 on failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00253
/// @trace_id_dd=DD_CRYPTO_04101
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=CMAC_resume
/// @needwork = dd
/// @endcode
int32_t CMAC_resume(CMAC_CTX const *const ctx) noexcept;

}  // namespace  isoft_def
}  // namespace openssl
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_OPENSSL_PUHUA_OPENSSL_CAMC_H_