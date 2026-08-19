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
/// @file       isoft_per_redd_calculate.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Operation class for calculating redundancy results used by the PER persistence module
/// @date       2023-08-01
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Encryption Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2023-08-01 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/crypto/isoft_per_redd_calculate.h"

#include <ara/core/result.h>
#ifdef ARA_WITH_CRYPTO
    #include <ara/crypto/common/entry_point.h>
    #include <ara/crypto/cryp/crypto_provider.h>
    #include <ara/crypto/cryp/hash_function_ctx.h>
#endif

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Create a working context
/// @return Created hash encryption context
bool PReddAlgorithm_Base::_BuildCtx(PReddData_CheckSum const &checkSum) noexcept
{
#ifdef ARA_WITH_CRYPTO
    ara::core::InstanceSpecifier const isCryptoName{"isoft"};
    ara::crypto::cryp::CryptoProvider::Uptr pCryptoProvider{ara::crypto::LoadCryptoProvider(isCryptoName)};
    ara::crypto::cryp::CryptoProvider::AlgId eAlgId{
        pCryptoProvider->ConvertToAlgId(T_StringView(checkSum.stAlgorithmFamily))};
    ara::core::Result< ara::crypto::cryp::HashFunctionCtx::Uptr > autoCtx{
        pCryptoProvider->CreateHashFunctionCtx(eAlgId)};
    if (false == autoCtx.HasValue()) {
        return false;
    }
    pHashCtx_ = std::move(std::move(autoCtx).Value());
    return true;
#else
    std::ignore = checkSum;
    return false;
#endif
}
/// @brief Reset all data
void PReddAlgorithm_Base::Reset() noexcept
{
#ifdef ARA_WITH_CRYPTO
    ara::crypto::cryp::HashFunctionCtx *pAlgCtx{_GetAlgorithmCtx()};
    if (nullptr != pAlgCtx) {
        std::ignore = pAlgCtx->Start();
    }
#endif
    return;
}
/// @brief Get the AlgID of the algorithm
/// @return
uint64_t PReddAlgorithm_Base::GetAlgID() const noexcept
{
#ifdef ARA_WITH_CRYPTO
    if (!pHashCtx_) {
        return 0U;
    }
    return static_cast< uint64_t >(pHashCtx_->GetCryptoPrimitiveId()->GetPrimitiveId());
#else
    return 0U;
#endif
}
/// @brief Calculate Redd data
/// @param pBSrcData
/// @param nSrcLen
/// @return
bool PReddAlgorithm_Base::CalculateReddData(uint8_t const *const pBSrcData, uint32_t const nSrcLen) noexcept
{
#ifdef ARA_WITH_CRYPTO
    if (!pHashCtx_) {
        return false;
    }
    ara::crypto::ReadOnlyMemRegion readMem{pBSrcData, nSrcLen};
    ara::core::Result< void > resultUpdate{pHashCtx_->Update(readMem)};
    if (false == resultUpdate.HasValue()) {
        return false;
    }
    return true;
#else
    std::ignore = pBSrcData;
    std::ignore = nSrcLen;
    return false;
#endif
}
/// @brief Return the execution result
/// @return Vector of uint8_t type
ara::core::Vector< uint8_t > PReddAlgorithm_Base::GetResult() const noexcept
{
    ara::core::Vector< uint8_t > vecReturn;
#ifdef ARA_WITH_CRYPTO
    ara::core::Result< ara::core::Vector< ara::core::Byte > > resultDigest{pHashCtx_->Finish()};
    if (false == resultDigest.HasValue()) {
        return vecReturn;
    }
    for (auto const &byte : resultDigest.Value()) {
        vecReturn.push_back(static_cast< uint8_t >(byte));
    }
#endif
    return vecReturn;
}
//********************************/
PReddAlgorithm_Crc::PReddAlgorithm_Crc(PReddDataCrc const &reddConfig) { _BuildCtx(reddConfig); }
/// @brief Get the Redd type supported by the derived class
/// @return
EReddType PReddAlgorithm_Crc::GetReddType() const noexcept { return EReddType::kCrc; }
//***************/
PReddAlgorithm_Hash::PReddAlgorithm_Hash(PReddDataHash const &reddConfig)
{
    _BuildCtx(reddConfig);
    // 2023-08-02 Generate a fixed initialization vector
    if (reddConfig.nInitVecLen > 0) {
        for (uint16_t i = 0; i < reddConfig.nInitVecLen; i++) {
            vecInit_.push_back('a' + i % kInt16_26U);
        }
    }
}
/// @brief Get the Redd type supported by the derived class
/// @return
EReddType PReddAlgorithm_Hash::GetReddType() const noexcept { return EReddType::kHash; }
/// @brief Reset all data
void PReddAlgorithm_Hash::Reset() noexcept
{
#ifdef ARA_WITH_CRYPTO
    ara::crypto::cryp::HashFunctionCtx *pAlgCtx{_GetAlgorithmCtx()};
    if (nullptr != pAlgCtx) {
        if (false == vecInit_.empty()) {
            ara::crypto::ReadOnlyMemRegion initMem{vecInit_.data(), vecInit_.size()};
            ara::core::Result< void > const resultStart{pAlgCtx->Start(initMem)};
            // Redundant error-configured initialization vector
            if (false == resultStart.HasValue()) {
                if (resultStart.Error() == ara::crypto::SecurityErrorDomain::Errc::kUnsupported) {
                    std::ignore = pAlgCtx->Start();
                }
            }
        } else {
            std::ignore = pAlgCtx->Start();
        }
    }
#endif
    return;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
