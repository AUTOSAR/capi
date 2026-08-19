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
/// @file       sync.h
/// @brief
/// @details
/// @date       2021-10-18
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_INTERNAL_PTP_SYNC_MESSAGE_H_
#define _ARA_TSYNC_INTERNAL_PTP_SYNC_MESSAGE_H_

#include <chrono>
#include <cstdint>
#include <iomanip>

#include "ara/tsync/internal/ptp/message/header.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

#pragma pack(push, 1)
/// @brief Sync
class Sync final : public Header
{
    /// @brief kFlagFieldByte0 constant declaration
    static constexpr std::uint8_t kFlagFieldByte0{0x2U};

    /// @brief kFlagFieldByte1 constant declaration
    static constexpr std::uint8_t kFlagFieldByte1{0x8U};

public:
    /// @brief constructor
    Sync() = default;

    /// @brief destructor
    ~Sync() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    Sync(Sync const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    Sync &operator=(Sync const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    Sync(Sync &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    Sync &operator=(Sync &&) &noexcept = default;

    /// @brief initialize
    void Init() noexcept
    {
        SetTransportSpecific();
        SetType(Type::kSync);
        SetMessageLength(sizeof(*this));
        // set twoStepFlag
        SetFlagField(kFlagFieldByte0, kFlagFieldByte1);
        SetControlField(ControlFieldType::kCtrlSync);
    }

    /// @brief set initial timestamp
    /// @param ts timestamp
    void SetOriginTimeStamp(std::chrono::nanoseconds const &ts) noexcept { originTimestamp_.FromChrono(ts); }

    /// @brief get initial timestamp
    /// @returns timestamp
    std::chrono::nanoseconds GetOriginTimeStamp() const noexcept
    {
        return std::move(std::chrono::nanoseconds(originTimestamp_.ToChrono().count()));
    }

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        Header::HeaderDebug();
        std::int32_t const tsNsec{static_cast< std::int32_t >(GetOriginTimeStamp().count() % 1000000000)};
        std::cout << "OriginTimeStamp:" << std::dec
                  << std::chrono::duration_cast< std::chrono::seconds >(GetOriginTimeStamp()).count() << ".";
        std::cout << std::right << std::setw(kTS_NUM_INT_9) << std::setfill(kTS_FILL_0) << std::dec << tsNsec
                  << std::endl;
#endif
    }

private:
    /// @name originTimestamp_
    internal::TimeValue originTimestamp_{};

};  // class Sync
#pragma pack(pop)

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  //_ARA_TSYNC_INTERNAL_PTP_SYNC_MESSAGE_H_
