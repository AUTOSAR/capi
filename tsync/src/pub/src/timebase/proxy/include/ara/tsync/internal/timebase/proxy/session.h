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
/// @file       session.h
/// @brief      time base proxy session management class
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_SESSION_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_SESSION_H_

#include <isoft/ipccpp/client.h>

#include <cstdint>

#include "ara/tsync/internal/timebase/proxy/proxyeventtype.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace proxy {

/// @brief session class
class Session final
{
public:
    /// @brief event type
    using Type = ProxyEventType;
    /// @brief IPC session ID
    using Id = isoft::ipc::IPCSessionId;

    /// @brief constructor with parameters
    /// @param s - session ID
    explicit Session(isoft::ipc::IPCSessionId const s) noexcept : kSid{s}, flags{}
    {
        reinterpret_cast< std::uint8_t* >(&flags)[0] = 0U;
    }

    /// @name kSid
    Id const kSid;

    /// @brief session status flag structure
    struct SessionFlag final
    {
        std::uint8_t sc : 1;   /// StatusChanged
        std::uint8_t ssc : 1;  /// SynchronizationStateChanged
        std::uint8_t lj : 1;   /// LeapJump
        std::uint8_t pm : 1;   /// PrecisionMeasurement
        std::uint8_t vm : 1;   /// ValidationMeasurement
    };
    /// @brief flags status flags
    SessionFlag flags;

    /// @brief operator overload, comparison function
    /// @param lhs - object reference
    /// @param rhs - object reference
    /// @return true - equal
    /// @return false - not equal
    friend bool operator<(Session const& lhs, Session const& rhs) noexcept
    {
        if (lhs.kSid < rhs.kSid) {
            return true;
        }
        return false;
    }

    /// @brief set the flag for the specified type
    /// @param type - type
    /// @param is - yes/no
    void SetTypeFlag(Type const& type, bool const& is) noexcept
    {
        switch (type) {
            case Type::kStatusChanged: {
                flags.sc = static_cast< std::uint8_t >(is);
                break;
            }
            case Type::kSynchronizationStateChanged: {
                flags.ssc = static_cast< std::uint8_t >(is);
                break;
            }
            case Type::kLeapJump: {
                flags.lj = static_cast< std::uint8_t >(is);
                break;
            }
            case Type::kPrecisionMeasurement: {
                flags.pm = static_cast< std::uint8_t >(is);
                break;
            }
            case Type::kValidationMeasurement: {
                flags.vm = static_cast< std::uint8_t >(is);
                break;
            }
            default: {
                break;
            }
        }
    }

    /// @brief get the flag for the specified type
    /// @param type - type
    /// @return yes/no
    bool GetTypeFlag(Type const& type) const noexcept
    {
        bool is{false};
        switch (type) {
            case Type::kStatusChanged: {
                is = static_cast< bool >(flags.sc);
                break;
            }
            case Type::kSynchronizationStateChanged: {
                is = static_cast< bool >(flags.ssc);
                break;
            }
            case Type::kLeapJump: {
                is = static_cast< bool >(flags.lj);
                break;
            }
            case Type::kPrecisionMeasurement: {
                is = static_cast< bool >(flags.pm);
                break;
            }
            case Type::kValidationMeasurement: {
                is = static_cast< bool >(flags.vm);
                break;
            }
            default: {
                is = false;
                break;
            }
        }
        return is;
    }
};

}  // namespace proxy
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_SESSION_H_
