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
/// @file       exec_error_domain.h
/// @brief      Execution management error domain class
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_EXEC_EXEC_ERROR_DOMAIN_H_
#define ARA_EXEC_EXEC_ERROR_DOMAIN_H_

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace ara {
namespace exec {

/// @brief redefine char
using Char8_t = char;

/// @brief Defines an enumeration class for the Execution Management error codes.
/// @code{.isoft}
/// @trace_id_sws=SWS_EM_02281
/// @needwork = no
/// @endcode
enum class ExecErrc : ara::core::ErrorDomain::CodeType
{
    kNoError            = 0xFF,  ///< have no error  // TODO: Consider using result to solve later
    kGeneralError       = 1,     ///< Some unspecified error occurred
    kInvalidArguments   = 2,     ///< Invalid argument was passed
    kCommunicationError = 3,     ///< Communication error occurred
    kMetaModelError     = 4,     ///< Wrong meta model identifier passed to a function
    kCancelled          = 5,     ///< Transition to the requested Function Group state was cancelled by a newer request
    kFailed             = 6,     ///< Transition to the requested Function Group state failed
    kFailedUnexpectedTerminationOnExit
    = 7,  ///< Unexpected Termination during transition in Processof previous Function Group State happened
    kFailedUnexpectedTerminationOnEnter
    = 8,  ///< Unexpected Termination during transition in Processof target Function Group State happened
    kInvalidTransition = 9,         ///< Transition invalid (e.g. to Terminating when alreadyin Terminating state)
    kAlreadyInState    = 10,        ///< Transition to the requested Function Group statefailed
                                    ///< because it is already in requested state
    kInTransitionToSameState = 11,  ///< Transition to the requested Function Group statefailed because transition to
                                    ///< requested state isalready in progress
    kNoTimeStamp  = 12,             ///< DeterministicClient time stamp information is notavailable
    kCycleOverrun = 13,             ///< Deterministic activation cycle time exceeded
    kIntegrityOrAuthenticityCheckFailed
    = 14,  ///< Integrity or authenticity check for a Process to be spawned in the requested Function Group state failed
};

/// @brief Defines a class for exceptions to be thrown by the Execution Management.
/// @code{.isoft}
/// @trace_id_sws=SWS_EM_02282, 49e8d34a5fe0f95eb1d2188f9e8bbffacabd3477
/// @needwork = no
/// @endcode
class ExecException : public ara::core::Exception  // PRQA S 2659,5215
{
public:
    /// @brief Constructs a new ExecException object containing an error code
    /// @param errorCode The error code.
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02283, 60ef4c51c3dcc4a8b79955c95bc5430cfa5ed1dd
    /// @endcode
    explicit ExecException(ara::core::ErrorCode&& errorCode) noexcept : ara::core::Exception{std::move(errorCode)} {}
};

/// @brief Defines a class representing the Execution Management error domain.
/// @code{.isoft}
/// @trace_id_sws=SWS_EM_02284, dc98a66bb9692b638c168a3248bb3574390d90fb
/// @needwork = no
/// @code{.isoft}
class ExecErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief Key ID for Execution Management error domain.
    static constexpr ErrorDomain::IdType kId{0x8000000000000202ULL};

public:
    /// @brief define execution error
    using Errc = ExecErrc;

    /// @brief define execution exception
    using Exception = ExecException;

    /// @brief Constructs a new ExecErrorDomain object
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02286, f3372473c0fa9bb3b175219bf5ac85ae962317f3
    /// @endcode
    ExecErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}  // NOLINT

    /// @brief Returns a string constant associated with ExecErrorDomain
    /// @return The name of the error domain.
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02287, 1990f0b93d29a5008f7234d867005a60b05a9854
    /// @trace_id_sws=SWS_EM_02292 Name() should return a string ending with nullptr
    /// @code{.isoft}
    Char8_t const* Name() const noexcept final { return "ExecErrorDomain"; }

    /// @brief Returns the message associated with errorCode.
    /// @param errorCode The error code number.
    /// @return The message associated with the error code.
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02288, 68f6ef535fcadc7ce7950aa976b0220ea54cf5e1
    /// @endcode
    Char8_t const* Message(CodeType errorCode) const noexcept final
    {
        Errc const code{static_cast< Errc >(errorCode)};  // PRQA S 4899
        Char8_t const* pMessage{""};                      // PRQA S 2981
        switch (code) {
            case ExecErrc::kGeneralError: {
                pMessage = "Some unspecified error occurred";
                break;
            }
            case ExecErrc::kInvalidArguments: {
                pMessage = "Invalid argument was passed";
                break;
            }
            case ExecErrc::kCommunicationError: {
                pMessage = "Communication error occurred";
                break;
            }
            case ExecErrc::kMetaModelError: {
                pMessage = "Wrong meta model identifier passed to a function";
                break;
            }
            case ExecErrc::kCancelled: {
                pMessage = "Transition to the requested Function Group state was cancelled by a newer request";
                break;
            }
            case ExecErrc::kFailed: {
                pMessage = "Transition to the requested Function Group state failed";
                break;
            }
            case ExecErrc::kFailedUnexpectedTerminationOnExit: {
                pMessage
                    = "Unexpected Termination during transition in Process of previous Function Group State happened";
                break;
            }
            case ExecErrc::kFailedUnexpectedTerminationOnEnter: {
                pMessage
                    = "Unexpected Termination during transition in Process of target Function Group State happened";
                break;
            }
            case ExecErrc::kInvalidTransition: {
                pMessage = "Transition invalid (e.g. to Terminating when alreadyin Terminating state)";
                break;
            }
            case ExecErrc::kAlreadyInState: {
                pMessage
                    = "Transition to the requested Function Group state failed because it is already in requested "
                      "state";
                break;
            }
            case ExecErrc::kInTransitionToSameState: {
                pMessage
                    = "Transition to the requested Function Group state failed because transition to requested state "
                      "is already in progress";
                break;
            }
            case ExecErrc::kNoTimeStamp: {
                pMessage = "DeterministicClient time stamp information is notavailable";
                break;
            }
            case ExecErrc::kCycleOverrun: {
                pMessage = "Deterministic activation cycle time exceeded";
                break;
            }
            case ExecErrc::kIntegrityOrAuthenticityCheckFailed: {
                pMessage
                    = "Integrity or authenticity check for a Process to be spawned in the requested Function Group "
                      "state failed";
                break;
            }
            case ExecErrc::kNoError: {
                pMessage = "Success";
                break;
            }
            default: {
                pMessage = "Unknown error";
                break;
            }
        }
        return pMessage;
    }

    /// @brief Creates a new instance of ExecException from errorCode and throws it as a C++ exception.
    /// @param errorCode The error to throw.
    /// @exception ExecException If the errorCode is not kNoError.
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02289, 6feca1a48b702478c9b3ff555c39f66f3c9f167c
    /// @endcode
    void ThrowAsException(ara::core::ErrorCode const& errorCode) const noexcept(false) final
    {
        ara::core::internal::ThrowOrTerminate< Exception >(errorCode);
    }
};

namespace internal {
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
static ExecErrorDomain const kExecErrorDomain;  // NOLINT
}  // namespace internal

/// @brief Returns a reference to the global ExecErrorDomain object.
/// @return Return a reference to the global ExecErrorDomain object.
/// @code{.isoft}
/// @trace_id_sws=SWS_EM_02290, 22b2bd8fd9228958ed7361a003c91fb86d9a0049
/// @needwork = no
/// @endcode
constexpr ara::core::ErrorDomain const& GetExecErrorDomain() noexcept { return internal::kExecErrorDomain; }

/// @brief Creates an instance of ErrorCode.
/// @param code Error code number.
/// @param data Vendor defined data associated with the error.
/// @return An ErrorCode object.
/// @code{.isoft}
/// @trace_id_sws=SWS_EM_02291, 078696957067d66ffa79cabc0f52c38f8812dc7f
/// @needwork = no
/// @endcode
constexpr ara::core::ErrorCode MakeErrorCode(ara::exec::ExecErrc const code,
                                             ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetExecErrorDomain(), data);
}

}  // namespace exec
}  // namespace ara

#endif  ///< ARA_EXEC_EXEC_ERROR_DOMAIN_H_
