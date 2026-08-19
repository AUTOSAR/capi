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
/// @file       profiles_07.h
/// @brief      E2E profiles 07 header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_E2E_PROFILES_07_H
#define ARA_COM_E2E_PROFILES_07_H

#include "isoft/e2e/E2E_P07.h"
#include "profiles.h"

namespace ara {
namespace com {
namespace profile_07 {

/// @brief Profile07 wrapper type
struct Profile07
{
    /// @brief Alias -- uint32_t
    using LengthType = uint32_t;
    /// @brief Alias -- uint32_t
    using CounterType = uint32_t;
    /// @brief Type alias--uint64_t
    using CrcType = uint64_t;
    /// @brief Type alias--uint32_t
    using IdType = uint32_t;
    /// @brief Type alias--E2E_P07CheckStatusType
    using CheckStatusType = E2E_P07CheckStatusType;
    /// @brief Type alias--E2E_P07CheckStateType
    using CheckStateType = E2E_P07CheckStateType;
    /// @brief Type alias--E2E_P07ProtectStateType
    using ProtectStateType = E2E_P07ProtectStateType;
    /// @brief Type alias--E2E_P07ConfigType
    using ConfigType = E2E_P07ConfigType;

    /// @brief Initializes the protection state.
    static constexpr auto&& kE2EProtectInit = E2E_P07ProtectInit;
    /// @brief Initializes the check state
    static constexpr auto&& kE2ECheckInit = E2E_P07CheckInit;

    /// @brief Protects the array/buffer to be transmitted using the E2E profile 07.
    static constexpr auto&& kE2EProtect = E2E_P07Protect;
    /// @brief Checks the Data received using the E2E profile 07.
    static constexpr auto&& kE2ECheck = E2E_P07Check;

    /// @brief The function maps the check status of Profile 07 to a generic check status, which can be used by E2E
    /// state machine check function.
    static constexpr auto&& kE2EMapStatusToSM = E2E_P07MapStatusToSM;

    /// @brief E2E header length
    static constexpr LengthType kHeaderLength{sizeof(CrcType) + sizeof(LengthType) + sizeof(CounterType)
                                              + sizeof(IdType)};
    /// @brief PROFILE name
    /// @return Returns Profile07 name
    static constexpr char const* Name() noexcept { return "PROFILE_07"; };
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

/// @brief Type alias Profile07 Protector wrapper type
using Protector = ara::com::profile::Protector< Profile07, profile::PEvent >;
/// @brief Type alias Profile07 Checker wrapper type
using Checker = ara::com::profile::Checker< Profile07, profile::PEvent >;

/// @brief Type alias Profile07::LengthType
using LengthType = Profile07::LengthType;
/// @brief Type alias Profile07::CounterType
using CounterType = Profile07::CounterType;
/// @brief Type alias Profile07::CrcType
using CrcType = Profile07::CrcType;
/// @brief Type alias Profile07::IdType
using IdType = Profile07::IdType;
/// @brief Type alias Profile07::ConfigType
using Config = Profile07::ConfigType;
}  // namespace profile_07
}  // namespace com
}  // namespace ara
#endif
