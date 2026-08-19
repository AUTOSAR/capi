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
/// @file       profiles_11.h
/// @brief      E2E profiles 11 header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_E2E_PROFILES_11_H
#define ARA_COM_E2E_PROFILES_11_H

#include "isoft/e2e/E2E_P11.h"
#include "profiles.h"

namespace ara {
namespace com {
namespace profile_11 {

/// @brief Profile11 wrapper type
struct Profile11
{
    /// @brief Alias -- uint16_t
    using LengthType = uint16_t;
    /// @brief Alias -- uint8_t
    using CounterType = uint8_t;
    /// @brief Type alias--uint8_t
    using CrcType = uint8_t;
    /// @brief Type alias--uint16_t
    using IdType = uint16_t;
    /// @brief
    using IdnibbleType = CounterType;
    /// @brief Type alias--E2E_P11CheckStatusType
    using CheckStatusType = E2E_P11CheckStatusType;
    /// @brief Type alias--E2E_P11CheckStateType
    using CheckStateType = E2E_P11CheckStateType;
    /// @brief Type alias--E2E_P11ProtectStateType
    using ProtectStateType = E2E_P11ProtectStateType;
    /// @brief Type alias--E2E_P11ConfigType
    using ConfigType = E2E_P11ConfigType;

    /// @brief Initializes the protection state.
    static constexpr auto&& kE2EProtectInit = E2E_P11ProtectInit;
    /// @brief Initializes the check state
    static constexpr auto&& kE2ECheckInit = E2E_P11CheckInit;

    /// @brief Protects the array/buffer to be transmitted using the E2E profile 11.
    static constexpr auto&& kE2EProtect = E2E_P11Protect;
    /// @brief Checks the Data received using the E2E profile 11.
    static constexpr auto&& kE2ECheck = E2E_P11Check;

    /// @brief The function maps the check status of Profile 11 to a generic check status, which can be used by E2E
    /// state machine check function.
    static constexpr auto&& kE2EMapStatusToSM = E2E_P11MapStatusToSM;

    /// @brief E2E header length
    static constexpr LengthType kHeaderLength{sizeof(CrcType) + sizeof(/*IdnibbleType+*/ CounterType)};
    /// @brief PROFILE name
    /// @return Returns Profile11 name
    static constexpr char const* Name() noexcept { return "PROFILE_11"; };
    /// @brief Parse length field according to PROFILE definition (not implemented)
    /// @param[in] dataPtr E2E protected content
    /// @param[in] offset E2E header offset
    /// @return Returns length field
    static LengthType ReceivedCounter(uint8_t const* dataPtr, uint8_t const offset) noexcept
    {
        std::ignore = dataPtr;
        std::ignore = offset;
        return 0;
    };
};

/// @brief Type alias Profile112 Protector wrapper type
using Protector = ara::com::profile::Protector< Profile11, profile::PEvent >;
/// @brief Type alias Profile11 Checker wrapper type
using Checker = ara::com::profile::Checker< Profile11, profile::PEvent >;

/// @brief Type alias Profile11::LengthType
using LengthType = Profile11::LengthType;
/// @brief Type alias Profile11::CounterType
using CounterType = Profile11::CounterType;
/// @brief Type alias Profile11::CrcType
using CrcType = Profile11::CrcType;
/// @brief Type alias Profile11::IdType
using IdType = Profile11::IdType;
/// @brief Type alias Profile11::IdnibbleType
using IdnibbleType = Profile11::IdnibbleType;

/// @brief Type alias Profile11::ConfigType
using Config = Profile11::ConfigType;
}  // namespace profile_11
namespace profile {
template <>
struct HasOffset< profile_11::Profile11 > : std::false_type
{
};
}  // namespace profile
}  // namespace com
}  // namespace ara
#endif
