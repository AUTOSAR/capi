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
/// @file       state_machine.h
/// @brief      E2E state machine header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_E2E_STATE_MACHINE_H
#define ARA_COM_E2E_STATE_MACHINE_H

#include <cstdint>
#include <memory>
#include <mutex>

#include "ara/com/e2e/e2e_error_domain.h"
#include "ara/com/internal/log/log.h"
#include "isoft/e2e/E2E.h"

namespace ara {
namespace com {
namespace e2e {
/// @brief Result of the check of a single sample
/// @ref [SWS_CM_90421] -- ara::com::e2e::ProfileCheckStatus
enum class ProfileCheckStatus : uint8_t
{
    kOk,             ///< The checks of the sample in this cycle were successful (including counter check)
    kRepeated,       ///< sample has a repeated counter
    kWrongSequence,  ///< The checks of the sample in this cycle were successful, with the exception of counter jump,
                     ///< which changed more than the allowed delta
    kError,          ///< Error not related to counters occurred (e.g. wrong crc, wrong length, wrong Data ID)
    kNotAvailable,  ///< No value has been received yet (e.g. during initialization). This is used as the initialization
                    ///< value for the buffer
    kNoNewData,     ///< No new data is available (assuming a sample has already been received since the initialization)
    kCheckDisabled  ///< No E2E check status available. Return value of function GetProfileCheckStatus if
                    ///< EndToEndTransformationComSpecProps.disableEndToEndCheck is set to TRUE
};

/// @brief Enumeration represents in what state is the e2e check of the sample(s) of the event
/// @ref [SWS_CM_90422] -- ara::com::e2e::SMState
enum class SMState : uint8_t
{
    kValid,  ///< Communication of the samples of this event functioning properly according to E2E checks, sample(s) can
             ///< be used
    kNoData,   ///< No data have been received from the publisher at all
    kInit,     ///< Not enough data where the E2E check yielded OK from the publisher is available since the
               ///< initialization, sample(s) cannot be used
    kInvalid,  ///< Too few data where the E2E check yielded OK or to many data where the E2E check yielded ERROR were
               ///< received within the E2E time window – communication of the sample of this event not functioning
               ///< properly, sample(s) cannot be used
    kStateMDisabled  ///< No E2E state machine available. Return value of function GetSMState if
                     ///< EndToEndTransformationComSpecProps.disableEndToEndStateMachine is set to TRUE
};
/// @brief E2E_SMConfigType alias
using Config = E2E_SMConfigType;
/// @brief E2E state machine
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode
class StateMachine
{
    /// @brief E2E_SMConfigType alias
    using SMConfig = E2E_SMConfigType;
    /// @brief EE2E_SMCheckStateType alias
    using SMCheckState = E2E_SMCheckStateType;

public:
    /// @brief Constructor
    /// @param[in] config State machine configuration
    explicit StateMachine(SMConfig const& config) noexcept : config_{config}
    {
        state_.SMState = E2E_SM_NODATA;
        profileStatusWindow_
            = std::shared_ptr< uint8_t >{new uint8_t[config.WindowSizeValid], std::default_delete< uint8_t[] >()};
        state_.ProfileStatusWindow = profileStatusWindow_.get();
        Std_ReturnType retVal{E2E_SMCheckInit(&state_, &config_)};
        ComLogDebug(GenArg(retVal));
    };
    /// @brief Constructor -- deleted
    StateMachine() noexcept = delete;
    /// @brief Copy constructor
    /// @param[in] other Object being copied
    StateMachine(StateMachine const& other) noexcept : config_{other.config_}, state_{other.state_} {};
    /// @brief Assignment operator overload
    /// @param[in] other Reference to the object being assigned
    /// @return
    StateMachine& operator=(StateMachine const& other) noexcept
    {
        if (this != &other) {
            config_ = other.config_;
            state_  = other.state_;
        }
        return *this;
    }
    /// @brief Move constructor
    /// @param[in] other Reference to the object being moved
    StateMachine(StateMachine&& other) noexcept : config_{other.config_}, state_{other.state_} {};

    /// @brief Move assignment operator overload
    /// @param[in] other Reference to the object being moved
    /// @return StateMachine&
    StateMachine& operator=(StateMachine&& other) noexcept
    {
        if (this != &other) {
            config_ = other.config_;
            state_  = other.state_;
        }
        return *this;
    }

    /// @brief Destructor
    ~StateMachine() noexcept = default;

    /// @brief State machine check
    /// @param[in] status Profile check result
    /// @param[out] state State machine check result output
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Check(ara::com::e2e::ProfileCheckStatus const& status,
                                    ara::com::e2e::SMState& state) noexcept
    {
        E2E_PCheckStatusType profileStatus = static_cast< E2E_PCheckStatusType >(status);
        auto retVal                        = E2E_SMCheck(profileStatus, &config_, &state_);
        state                              = CP2AP(state_);
        ComLogDebug("SMCheck", GenArg(retVal), GenArg(profileStatus), GenK2V("E2ESMStateType", state_.SMState),
                    GenK2V("e2e::SMState", state));
        if (retVal == E2E_E_OK) {
            return ara::core::Result< void >::FromValue();
        }
        return ara::core::Result< void >::FromError(ara::com::e2e::E2EErrc::kError);
    };
    /// @brief Convert E2EL library defined SMCheckState to AP definition
    /// @param[in] state E2EL library defined SMCheckState
    /// @return AP definition SMState
    inline ara::com::e2e::SMState CP2AP(SMCheckState state) noexcept
    {
        switch (state.SMState) {
            case E2E_SM_VALID: {
                return ara::com::e2e::SMState::kValid;
            } break;
            case E2E_SM_NODATA: {
                return ara::com::e2e::SMState::kNoData;
            } break;
            case E2E_SM_INIT: {
                return ara::com::e2e::SMState::kInit;
            } break;
            case E2E_SM_INVALID: {
                return ara::com::e2e::SMState::kInvalid;
            } break;
            default:
                return ara::com::e2e::SMState::kInvalid;
                break;
        }
    }

private:
    /// @brief State machine configuration
    SMConfig config_{};
    /// @brief State machine check result status
    SMCheckState state_{};
    /// @brief ProfileStatusWindow
    std::shared_ptr< uint8_t > profileStatusWindow_{};
};

}  // namespace e2e
}  // namespace com
}  // namespace ara
#endif
