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
/// @brief      State client communication protocol Message definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/SMS
/// @unit_name=Message
/// @unit_description=The Message between SMS Server and SMS Client.
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_SMS_MESSAGE_H_
#define _ARA_EXEC_INTERNAL_SMS_MESSAGE_H_

#include <ara/core/string.h>

#include <chrono>
#include <cstdint>
#include <cstring>
#include <string>

#include "ara/exec/exec_error_domain.h"

namespace ara {
namespace exec {
namespace internal {
namespace sms {

#pragma pack(push, 1)

/// @brief char redefinition
using Char8_t = char;
/// @brief SMS communication protocol message content
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_EM_10006
/// @trace_id_ad=AD_EM_00075
/// @trace_id_dd=DD_EM_00382
/// @needwork = ad
/// @endcode
class Message
{
public:
 /// @brief Get delimiter
 /// @return Delimiter
    static constexpr Char8_t const *GetkDelimiter() noexcept { return "="; }

 /// @brief Operation code enumeration
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_10006
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00383
    /// @needwork = dda
    /// @endcode
    enum class Operation : uint8_t
    {
        kGetState             = 1,
        kSetState             = 2,
        kNotifyUndefinedState = 3
    };

 /// @brief CallId type definition
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using CallId = uint32_t;

 /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00384
    /// @needwork = dda
    /// @endcode
    Message() noexcept = default;

 /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00385
    /// @needwork = dda
    /// @endcode
    ~Message() noexcept = default;

 /// @brief Disable move construction
    /// @param other the other Message
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Message(Message &&other) noexcept = delete;

 /// @brief Disable copy construction
    /// @param other the other Message
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Message(Message const &other) noexcept = delete;

 /// @brief Disable move assignment
    /// @param other the other Message
 /// @return New message instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Message &operator=(Message &&other) noexcept = delete;

 /// @brief Disable copy assignment
    /// @param other the other Message
 /// @return New message instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Message &operator=(Message const &other) noexcept = delete;

 /// @brief Set call ID
 /// @param id Call ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00386
    /// @needwork = dda
    /// @endcode
    void SetCallId(CallId const &id) noexcept { callId_ = id; }

 /// @brief Get call ID
 /// @return Call ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00387
    /// @needwork = dda
    /// @endcode
    CallId const &GetCallId() const noexcept { return callId_; }

 /// @brief Set operation code
 /// @param t Operation code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00388
    /// @needwork = dda
    /// @endcode
    void SetOperation(Operation const t) noexcept { opration_ = t; }

 /// @brief Get operation code
 /// @return Operation code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00389
    /// @needwork = dda
    /// @endcode
    Operation const &GetOperation() const noexcept { return opration_; }

 /// @brief Set execution error code
 /// @param ee Execution error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00390
    /// @needwork = dda
    /// @endcode
    void SetExecutionError(uint32_t const &ee) noexcept { executionError_ = ee; }

 /// @brief Get execution error code
 /// @return Execution error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00391
    /// @needwork = dda
    /// @endcode
    uint32_t GetExecutionError() const noexcept { return executionError_; }

 /// @brief Set translation error code
 /// @param te Translation error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00392
    /// @needwork = dda
    /// @endcode
    void SetTransitionError(ara::exec::ExecErrc const &te) noexcept { transitionError_ = te; }

 /// @brief Get translation error code
 /// @return Translation error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00393
    /// @needwork = dda
    /// @endcode
    ara::exec::ExecErrc const &GetTransitionError() const noexcept { return transitionError_; }

 /// @brief Get message size
 /// @return Bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00394
    /// @needwork = dda
    /// @endcode
    uint16_t GetSize() const noexcept { return GetSize(payloadSize_); }

 /// @brief Get payload size
 /// @return Bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00395
    /// @needwork = dda
    /// @endcode
    uint16_t GetPayloadSize() const noexcept { return payloadSize_; }

 /// @brief Set payload size
 /// @param size Bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00396
    /// @needwork = dda
    /// @endcode
    void SetPayloadSize(uint16_t const size) noexcept { payloadSize_ = size; }

 /// @brief Set function group state
 /// @param fgName Function group name
 /// @param stateName State name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00397
    /// @needwork = dda
    /// @endcode
    void SetFunctionGroupState(ara::core::StringView const &fgName, ara::core::StringView const &stateName) noexcept
    {
        constexpr uint16_t kDelimiterMaxLength{8U};
        SetPayloadSize(CalculatePayloadSize(fgName, stateName));
        std::ignore = memmove(static_cast< Char8_t * >(payload_), fgName.data(), fgName.size());
        std::ignore = memmove(payload_ + fgName.size(), GetkDelimiter(), strnlen(GetkDelimiter(), kDelimiterMaxLength));
        std::ignore = memmove(payload_ + fgName.size() + strnlen(GetkDelimiter(), kDelimiterMaxLength),
                              stateName.data(), stateName.size());
    }

 /// @brief Get function group state
 /// @param fgName Function group name
 /// @param stateName Function group state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00398
    /// @needwork = dda
    /// @endcode
    void GetFunctionGroupState(ara::core::String &fgName, ara::core::String &stateName) const noexcept
    {
        std::ignore = fgName;
        std::ignore = stateName;
        ara::core::String const delim{GetkDelimiter()};
        ara::core::StringView const fullSV{static_cast< Char8_t const * >(payload_),
                                           static_cast< size_t >(payloadSize_)};
        std::size_t const pos{fullSV.find(delim)};
        if (pos != ara::core::StringView::npos) {
            fgName    = fullSV.substr(0U, pos);
            stateName = fullSV.substr(pos + delim.length(), payloadSize_ - (pos + delim.length()));
        } else {
            fgName = fullSV;
        }
    }

 /// @brief Generate a CallId
    /// @return callId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00399
    /// @needwork = dda
    /// @endcode
    static CallId GenCallId() noexcept
    {
        uint32_t const kLast4Byte{0xFFFFFFFFU};
        return static_cast< uint32_t >(std::chrono::steady_clock::now().time_since_epoch().count()) & kLast4Byte;
    }

 /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept
    {
#if ARA_EXEC_DEBUG
        std::cout << "+++ sms::Message::Debug +++" << std::endl;
        std::cout << "MsgOperation: ";
        switch (opration_) {
            case Operation::kGetState: {
                std::cout << "GetState" << std::endl;
                break;
            }
            case Operation::kSetState: {
                std::cout << "SetState" << std::endl;
                break;
            }
            case Operation::kNotifyUndefinedState: {
                std::cout << "UndefinedStateNotify" << std::endl;
                break;
            }
            default: {
                std::cout << "Unknown" << std::endl;
                break;
            }
        }

        ara::core::StringView const sv{static_cast< Char8_t const * >(payload_), static_cast< size_t >(payloadSize_)};
        std::cout << "FgState: " << sv << std::endl;
        std::cout << "--- sms::Message::Debug ---" << std::endl;
#endif  ///< ARA_EXEC_DEBUG
    }

public:
 /// @brief Calculate total message size based on payload size
 /// @param payloadSize Payload size
 /// @return Bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00400
    /// @needwork = dda
    /// @endcode
    static uint16_t GetSize(uint16_t const payloadSize) noexcept
    {
        return static_cast< uint16_t >(__builtin_offsetof(Message, payload_) + payloadSize);
    }

 /// @brief Calculate payload size
 /// @param fgName Function group name
 /// @param stateName State name
 /// @return Bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00401
    /// @needwork = dda
    /// @endcode
    static uint16_t CalculatePayloadSize(ara::core::StringView const &fgName,
                                         ara::core::StringView const &stateName) noexcept
    {
        ara::core::String const delim{GetkDelimiter()};
        return static_cast< uint16_t >(fgName.size() + delim.length() + stateName.size());
    }

 /// @brief Calculate message size
 /// @param fgName Function group name
 /// @param stateName State name
 /// @return Bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00402
    /// @needwork = dda
    /// @endcode
    static uint16_t CalculateSize(ara::core::StringView const &fgName, ara::core::StringView const &stateName) noexcept
    {
        return GetSize(CalculatePayloadSize(fgName, stateName));
    }

private:
 /// @brief Call ID, used to distinguish each call
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00403
    /// @needwork = dda
    /// @endcode
    uint32_t callId_{0U};

 /// @brief Operation code, e.g., set state, get state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00404
    /// @needwork = dda
    /// @endcode
    Operation opration_{Operation::kGetState};

 /// @brief Error code when function group state fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00405
    /// @needwork = dda
    /// @endcode
    uint32_t executionError_{0U};

 /// @brief Reason for function group state failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00406
    /// @needwork = dda
    /// @endcode
    ara::exec::ExecErrc transitionError_{ara::exec::ExecErrc::kNoError};

 /// @brief Payload size, calculated based on function group and state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00407
    /// @needwork = dda
    /// @endcode
    uint16_t payloadSize_{0U};

 /// @brief Payload content, storing function group name and state name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00075
    /// @trace_id_dd=DD_EM_00408
    /// @needwork = dda
    /// @endcode
    Char8_t payload_[1]{0};  // PRQA S 4151 # the -Wpedantic forbiden zero-array, so increase 1

};  ///< class SMS_MESSAGE
#pragma pack(pop)

}  // namespace sms
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_SMS_MESSAGE_H_
