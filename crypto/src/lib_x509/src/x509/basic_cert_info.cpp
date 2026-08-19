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
/// @file       basic_cert_info.cpp
/// @brief      AutoSar-Crypto Certificate management module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate component/X.509 certificate
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=BasicCertInfo
/// @unit_description=Basic certificate information
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/basic_cert_info.h"

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
/// @brief "Basic certificate information" interface.
//********************************/
/// @brief Get the length of the hash algorithm
/// @param nIdHashType Hash type
/// @returns  Hash algorithm length
uint32_t BasicCertInfo::TransHashLen(int32_t const nIdHashType) noexcept
{
    uint32_t nHashLen{0U};
    switch (nIdHashType)  // There may be many types here
    {
        case NID_md5:
        case NID_md5WithRSAEncryption: {
            nHashLen = kInt_32U;
        } break;
        case NID_sha1:
        case NID_sha1WithRSAEncryption: {
            nHashLen = kInt_20U;
        } break;
        case NID_sha224:
        case NID_sha224WithRSAEncryption: {
            nHashLen = kInt_28U;
        } break;
        case NID_sha256:
        case NID_sha256WithRSAEncryption: {
            nHashLen = kInt_32U;
        } break;
        case NID_sha384:
        case NID_sha384WithRSAEncryption: {
            nHashLen = kInt_48U;
        } break;
        case NID_sha512:
        case NID_sha512WithRSAEncryption: {
            nHashLen = kInt_64U;
        } break;
        case NID_sm3: {
            nHashLen = kInt_32U;
        } break;
        default: {
        } break;
    }
    return nHashLen;
}
/// @brief Openssl.nid => CryptoAlgId
/// @param nAlgOid OpenSSL nid
/// @return Algorithm ID
CryptoAlgId BasicCertInfo::Trans2AlgId(uint32_t const nAlgOid) noexcept
{
    /// @brief Declaration of ara::crypto::cryp::isoft_def::EPhCtxTypeID
    using EPhCtxTypeID = ara::crypto::cryp::isoft_def::EPhCtxTypeID;
    EPhCtxTypeID nAlgId{EPhCtxTypeID::kUnDefine};
    switch (nAlgOid) {
        case NID_sha1WithRSAEncryption: {
            nAlgId = EPhCtxTypeID::kHashSha1;
        } break;
        case NID_sha224WithRSAEncryption:
        case NID_ecdsa_with_SHA224: {
            nAlgId = EPhCtxTypeID::kHashSha2_224;
        } break;
        case NID_sha256WithRSAEncryption:
        case NID_ecdsa_with_SHA256: {
            nAlgId = EPhCtxTypeID::kHashSha2_256;
        } break;
        case NID_sha384WithRSAEncryption:
        case NID_ecdsa_with_SHA384: {
            nAlgId = EPhCtxTypeID::kHashSha2_384;
        } break;
        case NID_sha512WithRSAEncryption:
        case NID_ecdsa_with_SHA512: {
            nAlgId = EPhCtxTypeID::kHashSha2_512;
        } break;
        case NID_md5WithRSAEncryption: {
            nAlgId = EPhCtxTypeID::kHashMd5;
        } break;
        default: {
        } break;
    }
    return static_cast< CryptoAlgId >(nAlgId);
}
/// @brief CryptoAlgId => Openssl.nid
/// @param algId Cryptographic primitive ID, algorithm ID
/// @return Cryptographic algorithm combination ID
uint32_t BasicCertInfo::Trans2Nid(CryptoAlgId const algId) noexcept
{
    /// @brief Declaration of ara::crypto::cryp::isoft_def::EPhCtxTypeID
    using EPhCtxTypeID = ara::crypto::cryp::isoft_def::EPhCtxTypeID;
    EPhCtxTypeID const nAlgId{static_cast< EPhCtxTypeID >(algId)};
    uint32_t nNid{0U};
    switch (nAlgId) {
        case EPhCtxTypeID::kHashSha1: {
            nNid = static_cast< uint32_t >(NID_sha1WithRSAEncryption);
        } break;
        case EPhCtxTypeID::kHashSha2_256: {
            nNid = static_cast< uint32_t >(NID_sha256WithRSAEncryption);
        } break;
        case EPhCtxTypeID::kHashSha2_512: {
            nNid = static_cast< uint32_t >(NID_sha512WithRSAEncryption);
        } break;
        case EPhCtxTypeID::kHashMd5: {
            nNid = static_cast< uint32_t >(NID_md5WithRSAEncryption);
        } break;
        default: {
            nNid = static_cast< uint32_t >(NID_undef);
        } break;
    }
    return nNid;
}

}  // namespace x509
}  // namespace crypto
}  // namespace ara
