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
/// @file       profiles_22.h
/// @brief      E2E profiles 22 header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_E2E_PROFILES_22_H
#define ARA_COM_E2E_PROFILES_22_H

#include "isoft/e2e/E2E_P22.h"
#include "profiles.h"

namespace ara {
namespace com {
namespace profile_22 {

/// @brief Profile22 wrapper type
struct Profile22
{
    /// @brief Alias -- uint16_t
    using LengthType = uint16_t;
    /// @brief Alias -- uint8_t
    using CounterType = uint8_t;
    /// @brief Type alias--uint8_t
    using CrcType = uint8_t;
    /// @brief Type alias--uint8_t
    using IdType = uint8_t;
    /// @brief Type alias--ara::core::Array<uint8_t, 16>
    using IdlistType = ara::core::Array< uint8_t,
                                         16 >;  // NOLINT -- TODO[magic-numbers]
    /// @brief Type alias--E2E_P22CheckStatusType
    using CheckStatusType = E2E_P22CheckStatusType;
    /// @brief Type alias--E2E_P22CheckStateType
    using CheckStateType = E2E_P22CheckStateType;
    /// @brief Type alias--E2E_P22ProtectStateType
    using ProtectStateType = E2E_P22ProtectStateType;
    /// @brief Type alias--E2E_P22ConfigType
    using ConfigType = E2E_P22ConfigType;

    /// @brief Initializes the protection state.
    static constexpr auto&& kE2EProtectInit = E2E_P22ProtectInit;
    /// @brief Initializes the check state
    static constexpr auto&& kE2ECheckInit = E2E_P22CheckInit;

    /// @brief Protects the array/buffer to be transmitted using the E2E profile 22.
    static constexpr auto&& kE2EProtect = E2E_P22Protect;
    /// @brief Checks the Data received using the E2E profile 22.
    static constexpr auto&& kE2ECheck = E2E_P22Check;

    /// @brief The function maps the check status of Profile 22 to a generic check status, which can be used by E2E
    /// state machine check function.
    static constexpr auto&& kE2EMapStatusToSM = E2E_P22MapStatusToSM;

    /// @brief E2E header length
    static constexpr LengthType kHeaderLength{sizeof(CrcType) + sizeof(/*4 bit unused+*/ CounterType)};
    /// @brief PROFILE name
    /// @return Returns Profile22 name
    static constexpr char const* Name() noexcept { return "PROFILE_22"; };
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

/// @brief Type alias Profile222 Protector wrapper type
using Protector = ara::com::profile::Protector< Profile22, profile::PEvent >;
/// @brief Type alias Profile22 Checker wrapper type
using Checker = ara::com::profile::Checker< Profile22, profile::PEvent >;

/// @brief Type alias Profile22::LengthType
using LengthType = Profile22::LengthType;
/// @brief Type alias Profile22::CounterType
using CounterType = Profile22::CounterType;
/// @brief Type alias Profile22::CrcType
using CrcType = Profile22::CrcType;
/// @brief Type alias Profile22::IdType
using IdType = Profile22::IdType;
/// @brief Type alias Profile22::IdlistType
using IdlistType = Profile22::IdlistType;
/// @brief Type alias Profile22::ConfigType
using Config = Profile22::ConfigType;
}  // namespace profile_22
namespace profile {
template <>
struct HasDataID< profile_22::Profile22 > : std::false_type
{
};
}  // namespace profile
}  // namespace com
}  // namespace ara
#endif
