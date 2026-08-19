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
/// @file       profiles_04m.h
/// @brief      E2E profiles 04mm header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_E2E_PROFILES_04mM_H
#define ARA_COM_E2E_PROFILES_04mM_H

#include "isoft/e2e/E2E_P04m.h"
#include "profiles.h"

namespace ara {
namespace com {
namespace profile_04m {

/// @brief Profile04m wrapper type
struct Profile04m
{
    /// @brief Alias -- uint16_t
    using LengthType = uint16_t;
    /// @brief Alias -- uint16_t
    using CounterType = uint16_t;
    /// @brief Type alias--uint32_t
    using CrcType = uint32_t;
    /// @brief Type alias--uint32_t
    using IdType = uint32_t;
    /// @brief Type alias--E2E_P04mCheckStatusType
    using CheckStatusType = E2E_P04mCheckStatusType;
    /// @brief Type alias--E2E_P04mCheckStateType
    using CheckStateType = E2E_P04mCheckStateType;
    /// @brief Type alias--E2E_P04mProtectStateType
    using ProtectStateType = E2E_P04mProtectStateType;
    /// @brief Type alias--E2E_P04mConfigType
    using ConfigType = E2E_P04mConfigType;

    /// @brief Initializes the protection state.
    static constexpr auto&& kE2EProtectInit = E2E_P04mProtectInit;
    /// @brief Initializes the check state
    static constexpr auto&& kE2ECheckInit = E2E_P04mCheckInit;

    /// @brief Protects the array/buffer to be transmitted using the E2E profile 04m.
    static constexpr auto&& kE2EProtect = E2E_P04mProtect;
    /// @brief Checks the Data received using the E2E profile 04m.This function is intended for usage at the data source
    /// (i.e., in case of C/S communication at the client).
    static constexpr auto&& kE2ESourceCheck = E2E_P04mSourceCheck;
    /// @brief Checks the Data received using the E2E profile 04m.This function is intended for usage at the data sink
    /// (i.e., in case of C/S communication at the server).
    static constexpr auto&& kE2ESinkCheck = E2E_P04mSinkCheck;

    /// @brief The function maps the check status of Profile 04m to a generic check status, which can be used by E2E
    /// state machine check function.
    static constexpr auto&& kE2EMapStatusToSM = E2E_P04mMapStatusToSM;

    /// @brief E2E header length
    static constexpr LengthType kHeaderLength{sizeof(LengthType) + sizeof(CounterType) + sizeof(IdType)
                                              + sizeof(CrcType) + 4};
    /// @brief PROFILE name
    /// @return Returns Profile04m name
    static constexpr char const* Name() noexcept { return "PROFILE_04m"; };
    /// @brief Parse length field according to PROFILE definition
    /// @param[in] dataPtr E2E protected content
    /// @param[in] offset E2E header offset
    /// @return Returns length field
    static LengthType ReceivedCounter(uint8_t const* dataPtr, uint8_t const offset) noexcept
    {
        constexpr uint32_t kShift8{1 * CHAR_BIT};
        return static_cast< uint16_t >(dataPtr[offset + 2] << kShift8) + static_cast< uint16_t >(dataPtr[offset + 3]);
    };
};

/// @brief Type alias Profile04m Protector wrapper type
using Protector = ara::com::profile::Protector< Profile04m, profile::PMethod >;
/// @brief Type alias Profile04m Checker wrapper type
using Checker = ara::com::profile::Checker< Profile04m, profile::PMethod >;

/// @brief Type alias Profile04m::LengthType
using LengthType = Profile04m::LengthType;
/// @brief Type alias Profile04m::CounterType
using CounterType = Profile04m::CounterType;
/// @brief Type alias Profile04m::CrcType
using CrcType = Profile04m::CrcType;
/// @brief Type alias Profile04m::IdType
using IdType = Profile04m::IdType;
/// @brief Type alias Profile04m::ConfigType
using Config = Profile04m::ConfigType;
}  // namespace profile_04m
}  // namespace com
}  // namespace ara
#endif
