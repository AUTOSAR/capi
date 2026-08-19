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

/// ================================================================
///
/// File description:
/// ----------------
/// @file       serializable.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption common module
/// @details
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Decryption/Signature Storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Serializable
/// @unit_description=Interface for serializable objects
/// @endcode
///
/// ================================================================

#include "ara/crypto/common/serializable.h"

#include "ara/crypto/openssl/isoft_openssl_encrypt.h"
#include "openssl/bio.h"

namespace ara {
namespace crypto {

/// @brief Export function
/// @param format Format type ID
/// @param pFun Callback function
/// @return  Data content in the corresponding format
ara::core::Result< ara::core::Vector< ara::core::Byte > > Serializable::ExportPublic_Fun(
    FormatId const format, std::function< int32_t(BIO *, FunctionId const) > const &pFun) noexcept
{
    BIO *const pPublic{BIO_new(BIO_s_mem())};
    int32_t ret{0};
    switch (format) {
        case kFormatRawValueOnly:
        case kFormatDerEncoded: {
            ret = pFun(pPublic, kFuncDer);
        } break;
        case kFormatPemEncoded: {
            ret = pFun(pPublic, kFuncPem);
        } break;
        case kFormatDefault: {
            ret = pFun(pPublic, kFuncDer);
            if (ret == 0) {
                ret = pFun(pPublic, kFuncPem);
            }
        } break;
        default: {
        } break;
    }
    ara::core::Vector< ara::core::Byte > vecData;
    if (ret >= 1) {
        int32_t const nNeedLen{PH_BIO_pending(pPublic)};
        if (nNeedLen > 0) {
            /// @error: SecurityErrorDomain::kInsufficientCapacity
            /// @details if(output.empty() == false), but it’s capacity is less than required
            try {
                vecData.resize(static_cast< std::size_t >(nNeedLen));
            } catch (std::bad_alloc &) {
                return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                    SecurityErrorDomain::Errc::kInsufficientCapacity);
            }
            std::ignore = BIO_read(pPublic, vecData.data(), nNeedLen);
        }
    }
    std::ignore = BIO_free(pPublic);
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecData);
}
/// @brief Check if the FormatID is supported
/// @param nFormatID Certificate encoding format
/// @param bCheckForWrite Whether to check the format ID for write support
/// @return true if is valid formatId false otherwise
bool Serializable::IsValidFormatID(FormatId const nFormatID, bool const bCheckForWrite) noexcept
{
    if (bCheckForWrite) {
        return (nFormatID == Serializable::kFormatDerEncoded) || (nFormatID == Serializable::kFormatPemEncoded)
               || (nFormatID == Serializable::kFormatDefault);
    }
    /// Verify nFormatID is within the enumeration definition {Serializable::kFormatDefault, Serializable::kFormatPemEncoded}
    return (nFormatID == Serializable::kFormatDefault) || (nFormatID == Serializable::kFormatRawValueOnly)
           || (nFormatID == Serializable::kFormatDerEncoded) || (nFormatID == Serializable::kFormatPemEncoded);
}
}  // namespace crypto
}  // namespace ara