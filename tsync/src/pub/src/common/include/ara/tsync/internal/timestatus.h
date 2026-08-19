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
/// @file       timestatus.h
/// @brief      time synchronization status
/// @details
/// @date       2023-01-31
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIME_STATUS_H_
#define ARA_TSYNC_INTERNAL_TIME_STATUS_H_

#include <cstdint>
#include <utility>

#include "isoft/util/type_cast.h"

/// @brief    Local time base status that appears directly after synchronization with the global time base.
//          This variable indicates whether and how the local time base is synchronized to the global master time base. This variable is a bit field of a single state, and not every combination is possible.
//          For example, only when the GLOBAL_TIME_BASE bit is set, TIMEOUT/TIMELEAP_FUTURE/TIMELEAP_PAST/SYNC_TO_GATEWAY can be set.
//          In the following bitmap, 0 is the least significant bit (LSB).
//          The meaning of the value corresponds to the whole byte, for example TIMELEAP_PAST may be 0x00 or 0x20,
//          0x20 binary expansion is 00100000B, the valid bit 1 corresponds to Bit5.
//============================================================================================================
//  Field name          Byte value   BIT bit   Value    Meaning
//------------------------------------------------------------------------------------------------------------
//  TIMEOUT             0x01        0       0x00    No timeout when receiving sync message;
//                                          0x01    Timeout occurred when receiving sync message.
//  SYNC_TO_GATEWAY     0x04        2       0x00    Local time base synchronized with global master time base;
//                                          0x04    Update of local time base is based on gateway under master time base.
//  GLOBAL_TIME_BASE    0x08        3       0x00    Local time base is based only on local time base's reference clock (never synchronized with global time base).
//                                          0x08    Local time base has been synchronized with global time base at least once.
//  TIMELEAP_FUTURE     0x10        4       0x00    No jump to the future when receiving time base.
//                                          0x10    A jump to the future occurred when receiving time base and exceeded the configured threshold.
//  TIMELEAP_PAST       0x20        5       0x00    No jump to the past when receiving time base.
//                                          0x20    A jump to the past occurred when receiving time base and exceeded the configured threshold.
//  RESERVED                        167     0x00    Bits 1,6,7 reserved, fixed to 0.
//=============================================================================================================

namespace ara {
namespace tsync {
namespace internal {

/// @brief time status
class TimeStatus final
{
public:
    /// @brief default constructor
    TimeStatus() noexcept
    {
        *(isoft::util::PointerCast< std::uint8_t, StatBits >(&status_)) = static_cast< std::uint8_t >(0);
    }

    /// @brief default destructor
    ~TimeStatus() = default;

    /// @brief copy constructor
    /// @param st - reference to other object
    TimeStatus(TimeStatus const& st) noexcept { *this = st; }

    /// @brief move constructor
    /// @param st - reference to other object
    TimeStatus(TimeStatus&& st) noexcept { *this = std::move(st); }

    /// @brief operator overload
    /// @param st - other object
    /// @return reference to this object
    TimeStatus& operator=(TimeStatus const& st) & noexcept
    {
        if (this == &st) {
            return *this;
        }
        this->status_ = st.status_;
        return *this;
    }

    /// @brief operator overload
    /// @param st - other object
    /// @return object reference
    TimeStatus& operator=(TimeStatus&& st) & noexcept
    {
        if (this == &st) {
            return *this;
        }
        this->status_ = std::move(st).status_;
        return *this;
    }

    /// @brief set timeout
    /// @param is - whether timeout
    void SetTimeout(bool const is) noexcept
    {
        if (is) {
            status_.timeout = static_cast< std::uint8_t >(1);
        } else {
            status_.timeout = static_cast< std::uint8_t >(0);
        }
    }

    /// @brief check if timeout
    /// @return yes/no
    bool IsTimeout() const noexcept { return static_cast< bool >(status_.timeout); }

    /// @brief set sync to gateway
    /// @param is - yes/no
    void SetSyncToGateway(bool const is) noexcept
    {
        if (is) {
            status_.syncToGW = static_cast< std::uint8_t >(1);
        } else {
            status_.syncToGW = static_cast< std::uint8_t >(0);
        }
    }

    /// @brief check if sync to gateway
    /// @return yes/no
    bool IsSyncToGateway() const noexcept { return static_cast< bool >(status_.syncToGW); }

    /// @brief set sync to global master
    /// @param is - yes/no
    void SetGlobalTimeBase(bool const is) noexcept
    {
        if (is) {
            status_.globalTb = static_cast< std::uint8_t >(1);
        } else {
            status_.globalTb = static_cast< std::uint8_t >(0);
        }
    }

    /// @brief check if sync to global master
    /// @return yes/no
    bool IsGlobalTimeBase() const noexcept { return static_cast< bool >(status_.globalTb); }

    /// @brief set forward time jump
    void SetTimeLeapFuture() noexcept
    {
        status_.timeLeapFuture = static_cast< std::uint8_t >(1);
        status_.timeLeapPast   = static_cast< std::uint8_t >(0);
    }

    /// @brief check if forward jump
    /// @return yes/no
    bool IsTimeLeapFuture() const noexcept { return static_cast< std::uint8_t >(1) == (status_.timeLeapFuture); }

    /// @brief set backward time jump
    void SetTimeLeapPast() noexcept
    {
        status_.timeLeapFuture = static_cast< std::uint8_t >(0);
        status_.timeLeapPast   = static_cast< std::uint8_t >(1);
    }

    /// @brief check if backward jump
    /// @return yes/no
    bool IsTimeLeapPast() const noexcept { return static_cast< std::uint8_t >(1) == (status_.timeLeapPast); }

    /// @brief set no time jump
    void SetTimeLeapNone() noexcept
    {
        status_.timeLeapFuture = static_cast< std::uint8_t >(0);
        status_.timeLeapPast   = static_cast< std::uint8_t >(0);
    }

    /// @brief check if status is no time jump
    /// @return yes/no
    bool IsTimeLeapNone() const noexcept
    {
        if (static_cast< std::uint8_t >(0) != status_.timeLeapFuture) {
            return false;
        }
        if (static_cast< std::uint8_t >(0) != status_.timeLeapPast) {
            return false;
        }
        return true;
    }

    /// @brief convert to std::uint8_t type
    /// @return std::uint8_t type
    std::uint8_t ToUint8() const noexcept { return *(isoft::util::PointerCast< std::uint8_t, StatBits >(&status_)); }

    /// @brief operator overload, comparison function
    /// @param lhs - object reference
    /// @param rhs - object reference
    /// @return true - equal
    /// @return false - not equal
    friend bool operator==(TimeStatus const& lhs, TimeStatus const& rhs) noexcept
    {
        if (lhs.ToUint8() == rhs.ToUint8()) {
            return true;
        }
        return false;
    }

    /// @brief operator overload, comparison function
    /// @param lhs - object reference
    /// @param rhs - object reference
    /// @return true - not equal
    /// @return false - equal
    friend bool operator!=(TimeStatus const& lhs, TimeStatus const& rhs) noexcept { return !(lhs == rhs); }

    /// @brief set the same synchronization status
    /// @param st - another object
    /// @return yes/no
    void SetSameSyncStatus(TimeStatus const& st) noexcept
    {
        if (this != &st) {
            this->status_.timeout  = st.status_.timeout;
            this->status_.syncToGW = st.status_.syncToGW;
            this->status_.globalTb = st.status_.globalTb;
        }
    }
    /// @brief whether it has the same synchronization status
    /// @param st - another object
    /// @return yes/no
    bool IsSameSyncStatus(TimeStatus const& st) const noexcept
    {
        if ((st.status_.timeout == this->status_.timeout) && (st.status_.syncToGW == this->status_.syncToGW)
            && (st.status_.globalTb == this->status_.globalTb)) {
            return true;
        }
        return false;
    }
    /// @brief set the same time jump status
    /// @param st - another object
    /// @return yes/no
    void SetSameLeapJump(TimeStatus const& st) noexcept
    {
        if (this != &st) {
            this->status_.timeLeapPast   = st.status_.timeLeapPast;
            this->status_.timeLeapFuture = st.status_.timeLeapFuture;
        }
    }

    /// @brief whether it has the same time jump status
    /// @param st - another object
    /// @return yes/no
    bool IsSameLeapJump(TimeStatus const& st) const noexcept
    {
        if ((st.status_.timeLeapPast == this->status_.timeLeapPast)
            && (st.status_.timeLeapFuture == this->status_.timeLeapFuture)) {
            return true;
        }
        return false;
    }

private:
    /// @brief time status bits
    struct StatBits final
    {
        std::uint8_t timeout : 1;
        std::uint8_t reserve1 : 1;
        std::uint8_t syncToGW : 1;
        std::uint8_t globalTb : 1;
        std::uint8_t timeLeapFuture : 1;
        std::uint8_t timeLeapPast : 1;
        std::uint8_t reserve6 : 1;
        std::uint8_t reserve7 : 1;
    };

    /// @name status_ - time status
    StatBits status_{};
};  /// class TimeStatus

}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_TIME_STATUS_H_
