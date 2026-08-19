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
/// @file       profiles_01.h
/// @brief      E2E profiles 01 header file
/// @details
/// @date       2023-10-25
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_E2E_PROFILES_01_H
#define ARA_COM_E2E_PROFILES_01_H

#include "isoft/e2e/E2E_P01.h"
#include "profiles.h"

namespace ara {
namespace com {
namespace profile_01 {
/// @brief Profile01 wrapper type
struct Profile01
{
    /// @brief Length type alias
    using LengthType = uint16_t;
    /// @brief Counter type alias
    using CounterType = uint8_t;
    /// @brief Crc type alias
    using CrcType = uint8_t;
    /// @brief DataId type alias
    using IdType = uint16_t;
    /// @brief Idnibble_t type alias
    using IdnibbleType = CounterType;
    /// @brief Type alias--E2E_P01CheckStatusType
    using CheckStatusType = E2E_P01CheckStatusType;
    /// @brief E2E_P01CheckStateType alias
    using CheckStateType = E2E_P01CheckStateType;
    /// @brief E2E_P01ProtectStateType alias
    using ProtectStateType = E2E_P01ProtectStateType;
    /// @brief E2E_P01ConfigType alias
    /// @brief Type alias--E2E_P01ConfigType
    using ConfigType = E2E_P01ConfigType;
    /// @brief E2E_P01ProtectInit method alias
    static constexpr auto&& kE2EProtectInit = E2E_P01ProtectInit;
    /// @brief E2E_P01CheckInit method alias
    static constexpr auto&& kE2ECheckInit = E2E_P01CheckInit;
    /// @brief E2E_P01Protect method alias
    static constexpr auto&& kE2EProtect = E2E_P01Protect;
    /// @brief E2E_P01Check method alias
    /// @brief Checks the Data received using the E2E profile 01.
    static constexpr auto&& kE2ECheck = E2E_P01Check;
    /// @brief E2E_P01MapStatusToSM method alias
    /// @brief The function maps the check status of Profile 01 to a generic check status, which can be used by E2E
    /// state machine check function.
    static constexpr auto&& kE2EMapStatusToSM = E2E_P01MapStatusToSM;
    /// @brief E2E header length
    static constexpr LengthType kHeaderLength{sizeof(CrcType) + sizeof(/*IdnibbleType+*/ CounterType)};
    /// @brief PROFILE name
    /// @return Returns PROFILE_01 name
    static constexpr char const* Name() noexcept { return "PROFILE_01"; };
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
/// @brief Profile01 Protector
using Protector = ara::com::profile::Protector< Profile01, profile::PEvent >;
/// @brief Profile01 Checker
/// @brief Type alias Profile01 Checker wrapper type
using Checker = ara::com::profile::Checker< Profile01, profile::PEvent >;

/// @brief Type alias Profile01::LengthType
using LengthType = Profile01::LengthType;
/// @brief Type alias Profile01::CounterType
using CounterType = Profile01::CounterType;
/// @brief Type alias Profile01::CrcType
using CrcType = Profile01::CrcType;
/// @brief Type alias Profile01::IdType
using IdType = Profile01::IdType;
/// @brief
using IdnibbleType = Profile01::IdnibbleType;
/// @brief Type alias Profile01::ConfigType
using Config = Profile01::ConfigType;
}  // namespace profile_01
namespace profile {
template <>
struct HasOffset< profile_01::Profile01 > : std::false_type
{
};
}  // namespace profile
}  // namespace com
}  // namespace ara
#endif
