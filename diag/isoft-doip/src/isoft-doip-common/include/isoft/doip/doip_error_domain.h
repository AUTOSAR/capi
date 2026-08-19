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
/// @file       doip_error_domain.h
/// @brief      This file defines the error information structure related to the DoIP error domain.
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_ERROR_DOMAIN_H_
#define ISOFT_DOIP_ERROR_DOMAIN_H_
#include "ara/core/error_code.h"
#include "ara/core/exception.h"

namespace isoft {
namespace doip {

/// @brief DOIP error code
enum class DoipErrorCode : ara::core::ErrorDomain::CodeType
{
    kDoipInternalErr                             = 1,   /// Internal implementation object null pointer
    kDoipEidInputErr                             = 2,   /// Invalid EID input
    kDoipVinInputErr                             = 3,   /// Invalid VIN input
    kDoipUdsInputErr                             = 4,   /// Invalid UDS input
    kDoipConnectTcpErr                           = 5,   /// TCP connection failed
    kDoipConnectUcpErr                           = 6,   /// UCP connection error
    kDoipActivateErr                             = 7,   /// DoIP routing activation status error
    kDoipDiagMessageCdErr                        = 8,   /// Frequent transmission of diagnostic messages
    kDoipActivateTcpErr                          = 9,   /// Failed to activate TCP
    kDoipActivateUdpErr                          = 10,  /// Failed to activate UDP
    kDoipListenVehicleAnnouncementErr            = 11,  /// Listening for vehicle announcement
    kDoipDisconnectErr                           = 12,  /// Connection disconnected
    kDoipRoutingActivationResponseDeserializeErr = 13,  /// Routing activation message parsing error
    kDoipRoutingActivationFail                   = 14,  /// Routing activation failed
    kDoipPeerRefuseConnect                       = 15,  /// Peer refused connection
    kDoipSendError                               = 16,  /// Data sending failed
    kDoipRecvieveError                           = 17,  /// Data receiving failed
    kDoipRecvieveNack                            = 18,  /// Received diagnostic NACK
};

/// @brief Exception handling class
class DoipException : public ara::core::Exception
{
public:
    explicit DoipException(ara::core::ErrorCode err) noexcept : ara::core::Exception(err) {}
};

/// @brief Error domain
class DoipErrorDomain final : public ara::core::ErrorDomain
{
    static constexpr ara::core::ErrorDomain::IdType kId{0xC000000000000003};

public:
    /// @brief DoIP error code alias
    using Errc = DoipErrorCode;
    /// @brief DoIP exception handling alias
    using Exception = DoipException;
    /// @brief Constructor
    /// @exception
    constexpr DoipErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}
    /// @brief Get DoIP error code name
    /// @return char const* DoipError
    /// @exception
    char const* Name() const noexcept override { return "DoipError"; }
    /// @brief Get DoIP error code description
    /// @param[in] code: The error code number
    /// @return char const* The message associated with the error code
    /// @exception
    char const* Message(ara::core::ErrorDomain::CodeType code) const noexcept override
    {
        ara::core::StringView msg{};
        switch (static_cast< Errc >(code)) {
            case DoipErrorCode::kDoipInternalErr: {
                msg = ara::core::StringView("Doip Internal Err");
            } break;
            case DoipErrorCode::kDoipEidInputErr: {
                msg = ara::core::StringView("Doip eid input Err");
            } break;
            case DoipErrorCode::kDoipVinInputErr: {
                msg = ara::core::StringView("Doip vin input Err");
            } break;
            case DoipErrorCode::kDoipUdsInputErr: {
                msg = ara::core::StringView("Doip uds input Err");
            } break;
            case DoipErrorCode::kDoipConnectTcpErr: {
                msg = ara::core::StringView("Doip tcp connect Err");
            } break;
            case DoipErrorCode::kDoipConnectUcpErr: {
                msg = ara::core::StringView("Doip udp connect Err");
            } break;
            case DoipErrorCode::kDoipActivateErr: {
                msg = ara::core::StringView("Doip activate Err");
            } break;
            case DoipErrorCode::kDoipActivateTcpErr: {
                msg = ara::core::StringView("Doip activate tcp Err");
            } break;
            case DoipErrorCode::kDoipActivateUdpErr: {
                msg = ara::core::StringView("Doip activate udp Err");
            } break;
            case DoipErrorCode::kDoipListenVehicleAnnouncementErr: {
                msg = ara::core::StringView("Doip listen vehicle announcement Err");
            } break;
            case DoipErrorCode::kDoipDisconnectErr: {
                msg = ara::core::StringView("Doip disconnect Err");
            } break;
            case DoipErrorCode::kDoipDiagMessageCdErr: {
                msg = ara::core::StringView("Doip DiagMessage cd Err");
            } break;
            case DoipErrorCode::kDoipRoutingActivationResponseDeserializeErr: {
                msg = ara::core::StringView("Doip routing reopnse deserialize error");
            } break;
            case DoipErrorCode::kDoipRoutingActivationFail: {
                msg = ara::core::StringView("Doip peer routing fail");
            } break;
            case DoipErrorCode::kDoipPeerRefuseConnect: {
                msg = ara::core::StringView("Doip peer refuse connect");
            } break;
            default: {
                msg = ara::core::StringView("Doip Unknown Err");
            } break;
        }

        return msg.data();
    }
    /// @brief Throw exception
    /// @param[in] code Exception error code
    void ThrowAsException(ara::core::ErrorCode const& code) const noexcept(false) override
    {
        ara::core::internal::ThrowOrTerminate< Exception >(code);
    }
};

namespace internal {
static DoipErrorDomain const kDoipErrorDomain;
}  // namespace internal

/// @brief : Returns the global DoipErrorDomain object.
/// @returns : The global DoipErrorDomain object.
constexpr ara::core::ErrorDomain const& GetDoipDomain() noexcept { return internal::kDoipErrorDomain; }

/// @brief Creates an error code.
/// @param code Error code number.
/// @param data Vendor defined data associated with the error.
/// @return An ErrorCode object.
constexpr ara::core::ErrorCode MakeErrorCode(DoipErrorCode const code,
                                             ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetDoipDomain(), data);
}

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_ERROR_DOMAIN_H_