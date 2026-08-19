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
/// @file       isoft_volatile_trusted_container.cpp
/// @brief      AutoSar-Crypto encryption/decryption common module
/// @details
/// @date       2022-06-07
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/ReuseFunction/ReuseFunctionModule
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=SR_CRYPTO_06005
/// @unit_description=Smart buffer
/// @endcode
///
/// ================================================================

#include "ara/crypto/common/isoft_volatile_trusted_container.h"

#include "ara/crypto/common/isoft_io_interface_mem.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
// 2022-08-30 hanjingjing sets PIoInterface_Mem default to Session object
/// @brief Parameterized constructor
/// @param capacity capacity
PVolatileTrustedContainer::PVolatileTrustedContainer(std::size_t const capacity) noexcept
    : VolatileTrustedContainer{}  // NOLINT
    , capacity_{capacity}
    , ioInterface_{new PIoInterface_Mem_Trust()}
{
    std::ignore = ioInterface_->SetCapacity(static_cast< uint32_t >(capacity_));
    std::ignore = ioInterface_->CommitSaveAction();
}

/// @brief Destructor
PVolatileTrustedContainer::~PVolatileTrustedContainer() noexcept
{
    if (ioInterface_ != nullptr) {
        delete ioInterface_;
        ioInterface_ = nullptr;
    }
}

/// @brief Get IO interface
/// @name  GetIOInterface
/// @returns  IOInterface reference
IOInterface& PVolatileTrustedContainer::GetIOInterface() const noexcept { return *ioInterface_; }
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara