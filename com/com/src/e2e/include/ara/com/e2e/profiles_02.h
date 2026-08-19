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
/// @file       profiles_02.h
/// @brief      E2E profiles 02 header file
/// @details
/// @date       2023-10-30
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_E2E_PROFILES_02_H
#define ARA_COM_E2E_PROFILES_02_H

#include "isoft/e2e/E2E_P02.h"
#include "profiles.h"

namespace ara {
namespace com {
namespace profile_02 {

/// @brief Profile02 wrapper type
struct Profile02
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
    using IdlistType = ara::core::Array< uint8_t, 16 >;  // NOLINT -- TODO[magic-numbers]
    /// @brief Type alias--E2E_P02CheckStatusType
    using CheckStatusType = E2E_P02CheckStatusType;
    /// @brief Type alias--E2E_P02CheckStateType
    using CheckStateType = E2E_P02CheckStateType;
    /// @brief Type alias--E2E_P02ProtectStateType
    using ProtectStateType = E2E_P02ProtectStateType;
    /// @brief Type alias--E2E_P02ConfigType
    using ConfigType = E2E_P02ConfigType;

    /// @brief Initializes the protection state.
    static constexpr auto&& kE2EProtectInit = E2E_P02ProtectInit;
    /// @brief Initializes the check state
    static constexpr auto&& kE2ECheckInit = E2E_P02CheckInit;

    /// @brief Protects the array/buffer to be transmitted using the E2E profile 02.
    static constexpr auto&& kE2EProtect = E2E_P02Protect;
    /// @brief Checks the Data received using the E2E profile 02.
    static constexpr auto&& kE2ECheck = E2E_P02Check;

    /// @brief The function maps the check status of Profile 02 to a generic check status, which can be used by E2E
    /// state machine check function.
    static constexpr auto&& kE2EMapStatusToSM = E2E_P02MapStatusToSM;

    /// @brief E2E header length
    static constexpr LengthType kHeaderLength{sizeof(CrcType) + sizeof(/*4 bit unused+*/ CounterType)};
    /// @brief PROFILE name
    /// @return Returns Profile02 name
    static constexpr char const* Name() noexcept { return "PROFILE_02"; };
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

/// @brief Type alias Profile02 Protector wrapper type
using Protector = ara::com::profile::Protector< Profile02, profile::PEvent >;
/// @brief Type alias Profile02 Checker wrapper type
using Checker = ara::com::profile::Checker< Profile02, profile::PEvent >;

/// @brief Type alias Profile02::LengthType
using LengthType = Profile02::LengthType;
/// @brief Type alias Profile02::CounterType
using CounterType = Profile02::CounterType;
/// @brief Type alias Profile02::CrcType
using CrcType = Profile02::CrcType;
/// @brief Type alias Profile02::IdType
using IdType = Profile02::IdType;
/// @brief Type alias Profile02::IdlistType
using IdlistType = Profile02::IdlistType;
/// @brief Type alias Profile02::ConfigType
using Config = Profile02::ConfigType;
}  // namespace profile_02
namespace profile {
template <>
struct HasDataID< profile_02::Profile02 > : std::false_type
{
};
}  // namespace profile
}  // namespace com
}  // namespace ara
#endif
