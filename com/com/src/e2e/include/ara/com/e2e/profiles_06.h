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
/// @file       profiles_06.h
/// @brief      E2E profiles 06 header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_E2E_PROFILES_06_H
#define ARA_COM_E2E_PROFILES_06_H

#include "isoft/e2e/E2E_P06.h"
#include "profiles.h"

namespace ara {
namespace com {
namespace profile_06 {

/// @brief Profile06 wrapper type
struct Profile06
{
    /// @brief Alias -- uint16_t
    using LengthType = uint16_t;
    /// @brief Alias -- uint8_t
    using CounterType = uint8_t;
    /// @brief Type alias--uint16_t
    using CrcType = uint16_t;
    /// @brief Type alias--uint16_t
    using IdType = uint16_t;
    /// @brief Type alias--E2E_P06CheckStatusType
    using CheckStatusType = E2E_P06CheckStatusType;
    /// @brief Type alias--E2E_P06CheckStateType
    using CheckStateType = E2E_P06CheckStateType;
    /// @brief Type alias--E2E_P06ProtectStateType
    using ProtectStateType = E2E_P06ProtectStateType;
    /// @brief Type alias--E2E_P06ConfigType
    using ConfigType = E2E_P06ConfigType;

    /// @brief Initializes the protection state.
    static constexpr auto&& kE2EProtectInit = E2E_P06ProtectInit;
    /// @brief Initializes the check state
    static constexpr auto&& kE2ECheckInit = E2E_P06CheckInit;

    /// @brief Protects the array/buffer to be transmitted using the E2E profile 06.
    static constexpr auto&& kE2EProtect = E2E_P06Protect;
    /// @brief Checks the Data received using the E2E profile 06.
    static constexpr auto&& kE2ECheck = E2E_P06Check;

    /// @brief The function maps the check status of Profile 06 to a generic check status, which can be used by E2E
    /// state machine check function.
    static constexpr auto&& kE2EMapStatusToSM = E2E_P06MapStatusToSM;

    /// @brief E2E header length
    static constexpr LengthType kHeaderLength{sizeof(CrcType) + sizeof(LengthType) + sizeof(CounterType)};
    /// @brief PROFILE name
    /// @return Returns Profile06 name
    static constexpr char const* Name() noexcept { return "PROFILE_06"; };
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

/// @brief Type alias Profile06 Protector wrapper type
using Protector = ara::com::profile::Protector< Profile06, profile::PEvent >;
/// @brief Type alias Profile06 Checker wrapper type
using Checker = ara::com::profile::Checker< Profile06, profile::PEvent >;

/// @brief Type alias Profile06::LengthType
using LengthType = Profile06::LengthType;
/// @brief Type alias Profile06::CounterType
using CounterType = Profile06::CounterType;
/// @brief Type alias Profile06::CrcType
using CrcType = Profile06::CrcType;
/// @brief Type alias Profile06::IdType
using IdType = Profile06::IdType;
/// @brief Type alias Profile06::ConfigType
using Config = Profile06::ConfigType;
}  // namespace profile_06
}  // namespace com
}  // namespace ara
#endif
