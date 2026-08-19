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
/// @file       raw_error_domain.h
/// @brief      Raw data stream error header file
/// @details
/// @date       2021-12-02
/// @author     cuijiusen
/// @version    1.2.0
///
/// ================================================================
///
/// @ref [SWS_CM_11268] -- Definition general ara::com::raw errors
///
/// ================================================================

#ifndef ARA_COM_RAW_RAW_ERROR_DOMAIN_H
#define ARA_COM_RAW_RAW_ERROR_DOMAIN_H
#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace ara {
namespace com {
namespace raw {
/// @brief Raw data stream error enumeration
/// @code{.isoft}
/// export_level=/COM/Raw/RAW
/// @endcode
/// @ref [SWS_CM_12367] -- The RawErrc enumeration defines the error codes for the RawErrorDomain
enum class RawErrc : ara::core::ErrorDomain::CodeType
{
    kUnknownError         = -1,  ///< Unknown/Other Error
    kStreamNotConnected   = 1,   ///< Trying to use a raw data stream without an established connection
    kCommunicationTimeout = 2,   ///< The operation was not successful and timed out
    kConnectionRefused = 3,  ///< The target address was not listening for connections or refused the connection request
    kAddressNotAvailable    = 4,  ///< The specified address is not available from the local machine
    kStreamAlreadyConnected = 5,  ///< The specified connection is already connected
    kConnectionClosedByPeer
    = 6,  ///< Network error. The established connection has been shut down during writing (POSIX EPIPE)
    kPeerUnreachable     = 7,  ///< Network error. The peer is unreachable (POSIX ENETUNREACH)
    kConnectionAborted   = 8,  ///< Network error. The incoming connection was aborted (POSIX ECONNABORTED)
    kInterruptedBySignal = 9,  ///< System error. Operation interrupted by system (POSIX EINTR)
};

/// @brief Raw data stream exception type
/// @code{.isoft}
/// export_level=/COM/Raw/RAW
/// @endcode
/// @ref [SWS_CM_11291]
/// @ref [RS_AP_00130]
/// @ref [RS_AP_00122]
/// @ref [RS_AP_00127]
class RawException : public ara::core::Exception
{
public:
    /// @brief Constructor
    /// @param[in] code Error code
    /// @ref [SWS_CM_11292]
    /// @ref [RS_AP_00120]
    /// @ref [RS_AP_00121]
    /// @ref [RS_AP_00130]
    /// @ref [RS_AP_00132]
    explicit RawException(ara::core::ErrorCode&& errorCode) noexcept : ara::core::Exception(std::move(errorCode)){};
};

/// @brief Raw data stream error domain type
/// @code{.isoft}
/// export_level=/COM/Raw/RAW
/// @endcode
/// @ref [SWS_CM_11293]
/// @ref [RS_AP_00130]
/// @ref [RS_AP_00122]
/// @ref [RS_AP_00127]
/// @domainid{0x8000'0000'0000'1280}
class RawErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief Error domain identifier
    constexpr static ara::core::ErrorDomain::IdType kId{0x8000000000001280};

public:
    /// @brief Type alias -- error type
    using Errc = RawErrc;
    /// @brief Type alias -- exception type
    using Exception = RawException;

    /// @brief Constructor
    /// @ref [SWS_CM_11294]
    /// @ref [RS_AP_00120]
    /// @ref [RS_AP_00130]
    /// @ref [RS_AP_00132]
    constexpr RawErrorDomain() noexcept : ara::core::ErrorDomain(kId){};

    /// @brief Get error domain name
    /// @return char const* "Raw".
    /// @ref [SWS_CM_11295]
    /// @ref [RS_AP_00120]
    /// @ref [RS_AP_00130]
    /// @ref [RS_AP_00132]
    char const* Name() const noexcept override { return "RawError"; };

    /// @brief Get error code related message
    /// @param[in] code Error enumeration
    /// @return char const* Error code related message
    /// @ref [SWS_CM_11296]
    /// @ref [RS_AP_00120]
    /// @ref [RS_AP_00121]
    /// @ref [RS_AP_00130]
    /// @ref [RS_AP_00132]
    char const* Message(CodeType errorCode) const noexcept override
    {
        Errc const code{static_cast< Errc >(errorCode)};
        switch (code) {
            case RawErrc::kStreamNotConnected:  ///< Trying to use a raw data stream without an established connection.
                return "StreamNotConnected";
            case RawErrc::kCommunicationTimeout:  ///< The operation was not successful and timed out.
                return "CommunicationTimeout";
            case RawErrc::kConnectionRefused:  ///< The target address was not listening for connections or refused the
                                               ///< connection request.
                return "ConnectionRefused";
            case RawErrc::kAddressNotAvailable:  ///< The specified address is not available from the local machine.
                return "AddressNotAvailable";
            case RawErrc::kStreamAlreadyConnected:  ///< The specified connection is already connected.
                return "StreamAlreadyConnected";
            case RawErrc::kConnectionClosedByPeer:  ///< Network error. The established connection has been shut down
                                                    ///< during writing (POSIX EPIPE)
                return "ConnectionClosedByPeer";
            case RawErrc::kPeerUnreachable:  ///< Network error. The peer is unreachable (POSIX ENETUNREACH).
                return "PeerUnreachable";
            case RawErrc::kConnectionAborted:  ///< Network error. The incoming connection was aborted (POSIX
                                               ///< ECONNABORTED).
                return "ConnectionAborted";
            case RawErrc::kInterruptedBySignal:  ///< System error. Operation interrupted by system (POSIX EINTR).
                return "InterruptedBySignal";
            default:
                return "Unknown error";
        }
    }
    /// @brief Throw exception
    /// @param[in] code Error code
    /// @ref [SWS_CM_11297] -- Creates a new instance of RawException from errorCode and throws it as a C++ exception
    void ThrowAsException(ara::core::ErrorCode const& code) const noexcept(false) override
    {
        ara::core::internal::ThrowOrTerminate< Exception >(code);
    }
};

namespace internal {
/// @brief Raw data stream error domain
constexpr RawErrorDomain kRawErrorDomain;
}  // namespace internal

/// @brief Get raw data stream error domain
/// @code{.isoft}
/// export_level=/COM/Raw/RAW
/// @endcode
/// @return ara::core::ErrorDomain const& Return a reference to the global RawErrorDomain object.
/// @ref [SWS_CM_11298]
/// @ref [RS_AP_00120]
/// @ref [RS_AP_00130]
/// @ref [RS_AP_00132]
constexpr ara::core::ErrorDomain const& GetRawErrorDomain() noexcept { return internal::kRawErrorDomain; }

/// @brief Generate raw data stream error code
/// @code{.isoft}
/// export_level=/COM/Raw/RAW
/// @endcode
/// @param[in] code Raw data stream error enumeration
/// @param[in] data Custom error data
/// @return ara::core::ErrorCode An ErrorCode object.
/// @ref [SWS_CM_11299]
/// @ref [RS_AP_00120]
/// @ref [RS_AP_00121]
/// @ref [RS_AP_00130]
/// @ref [RS_AP_00132]
constexpr ara::core::ErrorCode MakeErrorCode(ara::com::raw::RawErrc const code,
                                             ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetRawErrorDomain(), data);
}
}  // namespace raw
}  // namespace com
}  // namespace ara
#endif