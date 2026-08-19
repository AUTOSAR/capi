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
/// @brief      time base proxy message class
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_MESSAGE_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_MESSAGE_H_

#include <cstdint>

#include "ara/tsync/internal/timebase/proxy/proxyeventtype.h"
#include "ara/tsync/internal/timebase/resource/tbcontext.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace proxy {

#pragma pack(push, 1)

/// @brief time base proxy message definition
class Message final
{
public:
    /// @brief event type
    using Type = ProxyEventType;

    /// @brief common operation enumeration
    enum class Option : std::uint8_t
    {
        kRegister = 11,
        kUnregister,
        kNotify,
    };

    /// @brief verify measurement event message content
    enum class ValidationMeasurementOption : std::uint8_t
    {
        kRegister = 21,
        kUnregister,
        kSetPdelayInitiatorData,
        kSetPdelayResponderData,
        kSetSlaveTimingData,
        kSetMasterTimingData,
    };

public:
    /// @brief constructor
    Message() = default;

    /// @brief destructor
    ~Message() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    Message(Message const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    Message &operator=(Message const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    Message(Message &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    Message &operator=(Message &&) &noexcept = default;

    /// @brief set type
    /// @param type type
    void SetType(Type const type) noexcept { type_ = type; }

    /// @brief get type
    /// @return type
    Type GetType() const noexcept { return type_; }

    /// TODO(person in charge): limit type
    /// @brief get option
    /// @return option
    template < class T >
    T GetOption() const noexcept
    {
        return *static_cast< T const * >(static_cast< const void * >(&opt_));
    }

    /// @brief set option
    /// @param opt - option
    template < class T >
    void SetOption(T const &opt) noexcept
    {
        auto const o{reinterpret_cast< T * >(&opt_)};
        *o = opt;
    }

    /// @brief get option
    /// @return option
    std::uint8_t GetOption() const noexcept { return static_cast< std::uint8_t >(opt_.opt); }

    /// @brief set time base id
    /// @param id - time base id
    void SetTimeBaseId(timebase::resource::TimeBaseId const id) noexcept { timeBaseId_ = id; }

    /// @brief get time base id
    /// @return time base id
    timebase::resource::TimeBaseId GetTimeBaseId() const noexcept { return timeBaseId_; }

private:
    /// @name type_ - message type
    Type type_{Type::kSetTime};
    /// @brief OptUnion - messsage content such as Setime/TimeSync/TimeValidation
    union OptUnion
    {
        Option opt{Option::kNotify};
        ValidationMeasurementOption vmOpt;
    };
    /// @brief opt_ - messsage content such as Setime/TimeSync/TimeValidation
    OptUnion opt_{};

    /// @name timeBaseId_ - time timeBase ID which the message for
    timebase::resource::TimeBaseId timeBaseId_{};

};  // class Message
#pragma pack(pop)

}  // namespace proxy
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_MESSAGE_H_