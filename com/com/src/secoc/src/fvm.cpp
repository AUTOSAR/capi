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
/// @file       fvm.cpp
/// @brief
/// @details
/// @date       2023-09-04
/// @author     shigang.zan
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/secoc/fvm.h"

#include "ara/com/internal/secoc/secoc_type.h"

namespace ara {
namespace com {
namespace secoc {

/// @brief initialize
/// @code{.isoft}
/// export_level=/COM/Security/SecOC
/// @endcode
/// @return Result object -- empty/value or error
ara::core::Result< void > FVM::Initialize() noexcept { return FVM::Impl::Instance().ImplInitialize(); }

/// @brief get rx freshness value
/// @param[in] secOCFreshnessValueID
/// @param[in] secOCTruncatedFreshnessValue
/// @param[in] secOCAuthVerifyAttempts
/// @return ara::core::Result<FVContainer, SecOcFvmErrc>
ara::core::Result< FVContainer, SecOcFvmErrc > FVM::GetRxFreshness(uint16_t secOCFreshnessValueID,
                                                                   const FVContainer& secOCTruncatedFreshnessValue,
                                                                   uint16_t secOCAuthVerifyAttempts) noexcept
{
    return FVM::Impl::Instance().ImplGetRxFreshness(secOCFreshnessValueID, secOCTruncatedFreshnessValue,
                                                    secOCAuthVerifyAttempts);
}

/// @brief get tx freshness value
/// @param[in] secOCFreshnessValueID
/// @return ara::core::Result<FVContainer, SecOcFvmErrc>
ara::core::Result< FVContainer, SecOcFvmErrc > FVM::GetTxFreshness(uint16_t secOCFreshnessValueID) noexcept
{
    return FVM::Impl::Instance().ImplGetTxFreshness(secOCFreshnessValueID);
}

/// @brief update fv after verification success
/// @param[in] secOCFreshnessValueID freshness Id
/// @param[in] secOCFreshnessValue freshness value which is used to verify pdu
/// @return true / false
/// @details need meet some conditions: 1.Counter mode 2.Use FVM 3.larger than before
bool FVM::VerificationSuccessCallout(uint16_t secOCFreshnessValueID, const FVContainer& secOCFreshnessValue) noexcept
{
    return FVM::Impl::Instance().ImplVerificationSuccessCallout(secOCFreshnessValueID, secOCFreshnessValue);
}
}  // namespace secoc
}  // namespace com
}  // namespace ara
