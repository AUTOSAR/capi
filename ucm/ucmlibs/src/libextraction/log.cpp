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
/// @file       log.cpp
/// @brief      log wrapper
/// @details
/// @date       2024-10-26
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=DefaultLogger
/// @unit_description=Provide logging functionality
/// @endcode
///
/// ================================================================

#include "ara/ucm/internal/extraction/log.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief global default logger
std::unique_ptr< DefaultLogger > DefaultLogger::s_Instance_{nullptr};  // NOLINT
/// @brief once flag
std::once_flag DefaultLogger::s_Once_{};

/// @brief get instance
/// @throws no
/// @return instance
DefaultLogger& DefaultLogger::GetInstance()
{
    if (nullptr == s_Instance_) {
        s_Instance_ = std::make_unique< DefaultLogger >();
    }
    /// std::call_once(once_, [&]{ instance_ = std::make_unique<DefaultLogger>(); });
    return *s_Instance_;
}

/// @brief get global default logger
/// @throws no
/// @return global default logger
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00007
/// @trace_id_dd=DD_UCM_00059
/// @needwork = dd
/// @endcode
ara::log::Logger const& GetDefaultLogger() { return DefaultLogger::GetInstance().GetLogger(); }

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
