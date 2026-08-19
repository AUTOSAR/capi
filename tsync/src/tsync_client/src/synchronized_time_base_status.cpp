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
/// @file       synchronized_time_base_status.cpp
/// @brief      time base status private implementation class
/// @details
/// @date       2023-02-08
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/synchronized_time_base_status.h"

#include <ara/core/vector.h>

#include "ara/tsync/synchronized_time_base_status_private_impl.h"

namespace ara {
namespace tsync {

/// @brief constructor
SynchronizedTimeBaseStatus::SynchronizedTimeBaseStatus() noexcept : privateImpl{}
{
    privateImpl = std::make_unique< SynchronizedTimeBaseStatusPrivateImpl >();
    if (nullptr == privateImpl) {
        return;
    }
}

/// @brief destructor
SynchronizedTimeBaseStatus::~SynchronizedTimeBaseStatus() noexcept { privateImpl = nullptr; }

/// @brief    move constructor
/// @param    tbs object reference
/// @traceid  {SWS_TS_01057, 20-11}
SynchronizedTimeBaseStatus::SynchronizedTimeBaseStatus(SynchronizedTimeBaseStatus &&tbs) noexcept
    : privateImpl{std::move(tbs.privateImpl)}
{
}

/// @brief    copy constructor
/// @param    tbs object reference
/// @traceid  {SWS_TS_01058, 20-11}
SynchronizedTimeBaseStatus::SynchronizedTimeBaseStatus(SynchronizedTimeBaseStatus const &tbs) noexcept : privateImpl{}
{
    privateImpl = std::make_unique< SynchronizedTimeBaseStatusPrivateImpl >();
    if (nullptr == privateImpl) {
        return;
    }
    SynchronizedTimeBaseStatusPrivateImpl *const thisPri{this->privateImpl.get()};
    SynchronizedTimeBaseStatusPrivateImpl *const tbsPri{tbs.privateImpl.get()};
    if (thisPri != nullptr) {
        if (tbsPri != nullptr) {
            *thisPri = *tbsPri;
        }
    }
}

/// @brief    move assignment operator overload
/// @param    tbs object reference
/// @returns  object reference
/// @traceid  {SWS_TS_01059, 20-11}
SynchronizedTimeBaseStatus &SynchronizedTimeBaseStatus::operator=(SynchronizedTimeBaseStatus &&tbs) &noexcept
{
    if (this == &tbs) {
        return *this;
    }

    this->privateImpl = std::move(tbs.privateImpl);

    return *this;
}

/// @brief    copy assignment operator overload
/// @param    tbs object reference
/// @returns  object reference
/// @traceid  {SWS_TS_01060, 20-11}
SynchronizedTimeBaseStatus &SynchronizedTimeBaseStatus::operator=(SynchronizedTimeBaseStatus const &tbs) noexcept
{
    if (this == &tbs) {
        return *this;
    }

    SynchronizedTimeBaseStatusPrivateImpl *const thisPri{this->privateImpl.get()};
    SynchronizedTimeBaseStatusPrivateImpl *const tbsPri{tbs.privateImpl.get()};

    if (thisPri != nullptr) {
        if (tbsPri != nullptr) {
            *thisPri = *tbsPri;
        }
    }

    return *this;
}

/// @brief    get creation time
/// @returns  creation time
/// @traceid  {SWS_TS_01055, 20-11}
ara::tsync::Timestamp SynchronizedTimeBaseStatus::GetCreationTime() const noexcept { return privateImpl->creationTime; }

/// @brief    get synchronization status
/// @returns  synchronization status enumeration value.
/// @traceid  {SWS_TS_01053, 20-11}
SynchronizationStatus SynchronizedTimeBaseStatus::GetSynchronizationStatus() const noexcept
{
    return privateImpl->status;
}

/// @brief    get time jump mode, only includes jumps that occurred before the previous object was created.
/// @returns  time jump status enumeration value.
/// @traceid  {SWS_TS_01054, 20-11}
LeapJump SynchronizedTimeBaseStatus::GetLeapJump() const noexcept { return privateImpl->timeLeapJump; }

/// @traceid  {SWS_TS_010056, 20-11}
/// @traceid  {SWS_TS_00120, 20-11}
/// @brief    get user data. If there is no user data, the function should return an empty vector.
/// @returns get user data
ara::core::Span< ara::core::Byte const > SynchronizedTimeBaseStatus::GetUserData() const noexcept
{
    return privateImpl->userData.ToByteSpan();
}

}  // namespace tsync
}  // namespace ara
