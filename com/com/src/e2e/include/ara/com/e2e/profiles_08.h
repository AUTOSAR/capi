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
/// @file       profiles_08.h
/// @brief      E2E profiles 08 header file
/// @details
/// @date       2023-10-25
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_E2E_PROFILES_08_H
#define ARA_COM_E2E_PROFILES_08_H

#include "isoft/e2e/E2E_P08.h"
#include "profiles.h"

namespace ara {
namespace com {
namespace profile_08 {

/// @brief Profile08 wrapper type
struct Profile08
{
    /// @brief Alias -- uint32_t
    using LengthType = uint32_t;
    /// @brief Alias -- uint32_t
    using CounterType = uint32_t;
    /// @brief Type alias--uint32_t
    using CrcType = uint32_t;
    /// @brief Type alias--uint32_t
    using IdType = uint32_t;

    /// @brief Type alias--E2E_P08CheckStatusType
    using CheckStatusType = E2E_P08CheckStatusType;
    /// @brief Type alias--E2E_P08CheckStateType
    using CheckStateType = E2E_P08CheckStateType;
    /// @brief Type alias--E2E_P08ProtectStateType
    using ProtectStateType = E2E_P08ProtectStateType;
    /// @brief Type alias--E2E_P08ConfigType
    using ConfigType = E2E_P08ConfigType;

    /// @brief Initializes the protection state.
    static constexpr auto&& kE2EProtectInit = E2E_P08ProtectInit;
    /// @brief Initializes the check state
    static constexpr auto&& kE2ECheckInit = E2E_P08CheckInit;

    /// @brief Protects the array/buffer to be transmitted using the E2E profile 08.
    static constexpr auto&& kE2EProtect = E2E_P08Protect;
    /// @brief Checks the Data received using the E2E profile 08.
    static constexpr auto&& kE2ECheck = E2E_P08Check;

    /// @brief The function maps the check status of Profile 08 to a generic check status, which can be used by E2E
    /// state machine check function.
    static constexpr auto&& kE2EMapStatusToSM = E2E_P08MapStatusToSM;

    /// @brief E2E header length
    static constexpr LengthType kHeaderLength{sizeof(CrcType) + sizeof(LengthType) + sizeof(CounterType)
                                              + sizeof(IdType)};
    /// @brief PROFILE name
    /// @return Returns Profile08 name
    static constexpr char const* Name() noexcept { return "PROFILE_08"; };
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

/// @brief Type alias Profile08 Protector wrapper type
using Protector = ara::com::profile::Protector< Profile08, profile::PEvent >;
/// @brief Type alias Profile08 Checker wrapper type
using Checker = ara::com::profile::Checker< Profile08, profile::PEvent >;

/// @brief Type alias Profile08::LengthType
using LengthType = Profile08::LengthType;
/// @brief Type alias Profile08::CounterType
using CounterType = Profile08::CounterType;
/// @brief Type alias Profile08::CrcType
using CrcType = Profile08::CrcType;
/// @brief Type alias Profile08::IdType
using IdType = Profile08::IdType;
/// @brief Type alias Profile08::ConfigType
using Config = Profile08::ConfigType;
}  // namespace profile_08
}  // namespace com
}  // namespace ara
#endif
