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
/// @file       message.h
/// @brief      Execution client communication protocol Message definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/EMS
/// @unit_name=Message
/// @unit_description=The Message between EMS Server and EMS Client.
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_EMS_MESSAGE_H_
#define _ARA_EXEC_INTERNAL_EMS_MESSAGE_H_

#include <cstdint>
#include <string>

#if ARA_EXEC_DEBUG
    #include <iostream>
#endif

namespace ara {
namespace exec {
namespace internal {
namespace ems {

#pragma pack(push, 1)

/// @brief EMS communication protocol message content
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_EM_10001
/// @trace_id_ad=AD_EM_00063
/// @trace_id_dd=DD_EM_00341
/// @needwork = ad
/// @endcode
class Message
{
public:
    /// @brief Execution state definition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_EM_10001
    /// @trace_id_ad=AD_EM_00063
    /// @trace_id_dd=DD_EM_00342
    /// @needwork = dda
    /// @endcode
    enum class State : uint8_t
    {
        kRunning = 1,
        kTerminating,
        kMax
    };

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00063
    /// @trace_id_dd=DD_EM_00343
    /// @needwork = dda
    /// @endcode
    Message() = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00063
    /// @trace_id_dd=DD_EM_00344
    /// @needwork = dda
    /// @endcode
    ~Message() = default;

    /// @brief Disable move constructor
    /// @param other the other Message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00063
    /// @trace_id_dd=DD_EM_00812
    /// @needwork = dda
    /// @endcode
    Message(Message&& other) noexcept = delete;

    /// @brief Disable copy constructor
    /// @param other the other Message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00063
    /// @trace_id_dd=DD_EM_00813
    /// @needwork = dda
    /// @endcode
    Message(Message const& other) noexcept = delete;

    /// @brief Disable move assignment
    /// @param other the other Message
    /// @return New message instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00063
    /// @trace_id_dd=DD_EM_00814
    /// @needwork = dda
    /// @endcode
    Message& operator=(Message&& other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other the other Message
    /// @return New message instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00063
    /// @trace_id_dd=DD_EM_00815
    /// @needwork = dda
    /// @endcode
    Message& operator=(Message const& other) noexcept = delete;

    /// @brief Set the state
    /// @param s State
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00063
    /// @trace_id_dd=DD_EM_00345
    /// @needwork = dda
    /// @endcode
    void SetState(State const& s) noexcept { procState_ = s; }

    /// @brief Get the state
    /// @return State
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00063
    /// @trace_id_dd=DD_EM_00346
    /// @needwork = dda
    /// @endcode
    State GetState() const noexcept { return procState_; }

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept
    {
#if ARA_EXEC_DEBUG
        std::cout << "+++ ems::Message::Debug +++" << std::endl;
        std::cout << "ProcState: ";
        switch (procState_) {
            case State::kRunning: {
                std::cout << "Running" << std::endl;
                break;
            }
            case State::kTerminating: {
                std::cout << "Terminating" << std::endl;
                break;
            }
            default: {
                std::cout << "Unknown" << std::endl;
                break;
            }
        }
        std::cout << "--- ems::Message::Debug ---" << std::endl;
#endif  ///< ARA_EXEC_DEBUG
    }

private:
    /// @brief Process state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00063
    /// @trace_id_dd=DD_EM_00347
    /// @needwork = dda
    /// @endcode
    State procState_;
};  ///< classEMS_MESSAGE
#pragma pack(pop)

}  // namespace ems
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_EMS_MESSAGE_H_
