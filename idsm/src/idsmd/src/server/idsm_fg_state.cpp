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
/// @file       idsm_fg_state.cpp
/// @brief      Function group state access class
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/function group state pool
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0007
/// @unit_name=FunGrpState
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_fg_state.h"

#include <mutex>

#include "log/idsm_log.h"
namespace ara {
namespace idsm {
/// @brief Unique instance of the function group state pool
std::shared_ptr< FunGrpState > FunGrpState::s_SingleInstance_{nullptr};  // NOLINT
/// @brief Singleton thread-safe mutex lock
std::mutex FunGrpState::s_SigleMutex_{};
/// @brief FunGrpState
/// @return FunGrpState instance
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={reentrant}
/// @endcode
std::shared_ptr< FunGrpState > FunGrpState::GetInstance()
{
    if (s_SingleInstance_ == nullptr) {
        std::unique_lock< std::mutex > const lock{s_SigleMutex_};
        if (s_SingleInstance_ == nullptr) {
            s_SingleInstance_.reset(new FunGrpState);
        }
    }
    return s_SingleInstance_;
}
/// @brief Set function group state
/// @param fgName Function group name
/// @param fgState Function group state
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void FunGrpState::Set(ara::core::String const& fgName, ara::core::String const& fgState)
{
    if (fgMap_.count(fgName) == 0U) {
        std::ignore = fgMap_.insert(std::pair< ara::core::String, ara::core::String >(fgName, fgState));
    } else {
        fgMap_[fgName] = fgState;
    }
}
/// @brief Get function group state
/// @param fgName Function group name
/// @return Function group state
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
ara::core::String FunGrpState::Get(ara::core::String const& fgName)
{
    if (fgMap_.count(fgName) == 0U) {
        LOG_ERROR << "Idsm not found founction group:" << fgName.c_str() << " state.";
        return ara::core::String{""};
    }
    return fgMap_[fgName];
}

}  // namespace idsm
}  // namespace ara
