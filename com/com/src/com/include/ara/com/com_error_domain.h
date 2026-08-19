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
/// @file       com_error_domain.h
/// @brief      Communication error header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================
///
/// @ref [SWS_CM_11265] -- Use of general ara::com errors
/// @ref [SWS_CM_11264] -- Definition general ara::com errors
///
/// ================================================================

#ifndef ARA_COM_COM_ERROR_DOMAIN_H
#define ARA_COM_COM_ERROR_DOMAIN_H

#include "ara/core/error_code.h"
#include "ara/core/exception.h"

namespace ara {
namespace com {
/// @brief Communication error enumeration
/// @code{.isoft}
/// export_level=/COM
/// @endcode
/// @ref [SWS_CM_10432] -- The ComErrc enumeration defines the error codes for the ComErrorDomain
enum class ComErrc : ara::core::ErrorDomain::CodeType
{
    kUnknownError            = -1,  ///< Unknown/Other Error
    kServiceNotAvailable     = 1,   ///< Service is not available
    kMaxSamplesReached       = 2,   ///< Application holds more SamplePtrs than commited in Subscribe()
    kNetworkBindingFailure   = 3,   ///< Local failure has been detected by the network binding
    kGrantEnforcementError   = 4,   ///< Request was refused by Grant enforcement layer
    kPeerIsUnreachable       = 5,   ///< TLS handshake fail
    kFieldValueIsNotValid    = 6,   ///< Field Value is not valid
    kSetHandlerNotSet        = 7,   ///< SetHandler has not been registered
    kUnsetFailure            = 8,   ///< Failure has been detected by unset operation
    kSampleAllocationFailure = 9,   ///< Not Sufficient memory resources can be allocated
    kIllegalUseOfAllocate
    = 10,  ///< The allocation was illegally done via custom allocator (i.e., not via shared memory allocation)
    kServiceNotOffered       = 11,  ///< Service not offered
    kCommunicationLinkError  = 12,  ///< Communication link is broken
    kNoClients               = 13,  ///< No clients connected
    kCommunicationStackError = 14,  ///< Communication Stack Error, e.g. network stack, network binding, or
                                    ///< communication framework reports an error
    kInstanceIDCouldNotBeResolved
    = 15,  ///< ResolveInstanceIDs() failed to resolve InstanceID from InstanceSpecifier, i.e. is not mapped correctly
    kMaxSampleCountNotRealizable   = 16,  ///< Provided maxSampleCount not realizable
    kWrongMethodCallProcessingMode = 17,  ///< Wrong processing mode passed to constructor method call
    kErroneousFileHandle           = 18,  ///< The FileHandle returned from FindServce is corrupt/ service not available
    kCouldNotExecute               = 19,  ///< Command could not be executed in provided Execution Context
};

/// @brief Communication exception type
/// @code{.isoft}
/// export_level=/COM
/// @endcode
/// @ref [SWS_CM_11327] -- Defines a class for exceptions to be thrown by the Communication APIs
class ComException : public ara::core::Exception
{
public:
    /// @brief Constructor
    /// @param[in] code Error code
    /// @ref [SWS_CM_11328] -- Constructs a new ComException object containing an error code
    explicit ComException(ara::core::ErrorCode code) noexcept : ara::core::Exception(code) {}
};

/// @brief Communication error domain type
/// @code{.isoft}
/// export_level=/COM
/// @endcode
/// @ref [SWS_CM_11329] -- Defines a class representing the Communication error domain
class ComErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief Error domain identifier
    /// @ref [SWS_CM_11267] -- General errors domain
    static constexpr ara::core::ErrorDomain::IdType kId{0x8000000000001267};

public:
    /// @brief Type alias -- Error type
    /// @ref [SWS_CM_11336] -- Alias for the error code value enumeration
    using Errc = ComErrc;
    /// @brief Type alias -- Exception type
    /// @ref [SWS_CM_11337] -- Alias for the exception base class
    using Exception = ComException;
    /// @brief Constructor
    /// @ref [SWS_CM_11330] -- Constructs a new ComErrorDomain object
    constexpr ComErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}
    /// @brief Get error domain name
    /// @return char const* ComError
    /// @ref [SWS_CM_11331] -- Returns a string constant associated with ComErrorDomain
    char const* Name() const noexcept override { return "ComError"; }
    /// @brief Get message associated with error code
    /// @param[in] code Error enumeration
    /// @return char const* Message associated with error code
    /// @ref [SWS_CM_11332] -- Returns the message associated with errorCode
    char const* Message(ara::core::ErrorDomain::CodeType code) const noexcept override
    {
        switch (static_cast< Errc >(code)) {
            case Errc::kUnknownError:
                return "Unknown/Other Error";
            case Errc::kServiceNotAvailable:
                return "Service is not available";
            case Errc::kMaxSamplesReached:
                return "Application holds more SamplePtrs than commited in Subscribe()";
            case Errc::kNetworkBindingFailure:
                return "Local failure has been detected by the network binding";
            case Errc::kGrantEnforcementError:
                return "Request was refused by Grant enforcement layer";
            case Errc::kPeerIsUnreachable:
                return "TLS handshake fail";
            case Errc::kFieldValueIsNotValid:
                return "Field Value is not valid";
            case Errc::kSetHandlerNotSet:
                return "SetHandler has not been registered";
            case Errc::kUnsetFailure:
                return "Failure has been detected by unset operation";
            case Errc::kSampleAllocationFailure:
                return "Not Sufficient memory resources can be allocated";
            case Errc::kIllegalUseOfAllocate:
                return "The allocation was illegally done via custom allocator (i.e., not via shared memory "
                       "allocation)";
            case Errc::kServiceNotOffered:
                return "Service not offered";
            case Errc::kCommunicationLinkError:
                return "Communication link is broken";
            case Errc::kNoClients:
                return "No clients connected";
            case Errc::kCommunicationStackError:
                return "Communication Stack Error, e.g. network stack, network binding, or communication framework "
                       "reports an error";
            case Errc::kInstanceIDCouldNotBeResolved:
                return "ResolveInstanceIDs() failed to resolve InstanceID from InstanceSpecifier, i.e. is not mapped "
                       "correctly";
            case Errc::kMaxSampleCountNotRealizable:
                return "Provided maxSampleCount not realizable";
            case Errc::kWrongMethodCallProcessingMode:
                return "Wrong processing mode passed to constructor method call";
            case Errc::kErroneousFileHandle:
                return "The FileHandle returned from FindServce is corrupt/ service not available";
            case Errc::kCouldNotExecute:
                return "Command could not be executed in provided Execution Context";
            default:
                return "Unknown error";
        }
    }
    /// @brief Throw exception
    /// @param[in] code Error code
    /// @ref [SWS_CM_11333] -- Creates a new instance of ComException from errorCode and throws it as a C++ exception
    void ThrowAsException(ara::core::ErrorCode const& code) const noexcept(false) override
    {
        ara::core::internal::ThrowOrTerminate< Exception >(code);
    }
};

namespace internal {
/// @brief Communication error domain
constexpr ComErrorDomain kComErrorDomain;
}  // namespace internal

/// @brief Get communication error domain
/// @code{.isoft}
/// export_level=/COM
/// @endcode
/// @return constexpr ara::core::ErrorDomain const&
/// @ref [SWS_CM_11334] -- Returns a reference to the global ComErrorDomain object
inline constexpr ara::core::ErrorDomain const& GetComErrorDomain() noexcept { return internal::kComErrorDomain; }

/// @brief Generate communication error code
/// @code{.isoft}
/// export_level=/COM
/// @endcode
/// @param[in] code Communication error enumeration
/// @param[in] data Custom error data
/// @return constexpr ara::core::ErrorCode: An ErrorCode object
/// @ref [SWS_CM_11335] -- Creates an instance of ErrorCode
inline constexpr ara::core::ErrorCode MakeErrorCode(ComErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept
{
    return ara::core::ErrorCode{static_cast< ara::core::ErrorDomain::CodeType >(code), GetComErrorDomain(), data};
}
}  // namespace com
}  // namespace ara
#endif
