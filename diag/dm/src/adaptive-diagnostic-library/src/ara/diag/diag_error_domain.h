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
/// @file       diag_error_domain.h
/// @brief      This file provides the definitions of DiagErrorDomain, DiagOfferErrorDomain, DiagReportingErrorDomain and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_ERROR_DOMAIN_DIAG_H_
#define ARA_DIAG_ERROR_DOMAIN_DIAG_H_
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

#include <cstdint>

namespace ara {
namespace diag {

/// @brief Specifies the types of internal errors that can occur upon calling Offer or ReportMonitorAction.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00514}@tracestatus{draft}
enum class DiagErrc : ara::core::ErrorDomain::CodeType
{
    kAlreadyOffered        = 101,  ///< The service is already offered
    kConfigurationMismatch = 102,  ///< monitor configuration does not match dext
    kDebouncingConfigurationInconsistent
    = 103,  ///<  monitor debouncing configuration invalid, e.g. passed threshold larger than failed threshold...
    kReportIgnored   = 104,  ///<  Enable Conditions disabled, OC not started, ...
    kInvalidArgument = 105,  ///<  e.g. kPreFailed with internal debouncing
    kNotOffered      = 106,  ///<  Offer not called before reporting
    kGenericError    = 107,  ///<  generic issue, e.g. connection to DM lost
    kNoSuchDTC       = 108,  ///< No DTC available.
    kBusy            = 109,  ///< Interface is busy with processing.
    kFailed          = 110,  ///< Failed to process.
    kMemoryError     = 111,  ///< A memory error occurred during processing.
    kWrongDtc        = 112,  ///< A wrong DTC number was requested.
    kRejected        = 113,  ///< Requested operation was rejected due to StateManagements/machines internal state.
    kResetTypeNotSupported
    = 114,  ///< The requested Diagnostic reset type is not supported by the Diagnostic Address instance.
    kRequestFailed       = 115,   ///< Diagnostic request could not be performed successfully.
    kServiceNotAvailable = 118,   ///< The call cannot be executed, because essential DM functionality is
                                  ///< currently not available.
    kCustomResetTypeNotSupported  ///< The requested Diagnostic custom reset type is not supported by the Diagnostic
                                  ///< Address instance.
};

/// @brief The DiagOfferErrc enumeration defines the error codes for the DiagErrorDomain.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00559}@tracestatus{draft}
enum class DiagOfferErrc : ara::core::ErrorDomain::CodeType
{
    kAlreadyOffered        = 101,  ///< The service is already offered
    kConfigurationMismatch = 102,  ///<  monitor configuration does not match dext
    kDebouncingConfigurationInconsistent
    = 103,  ///<  monitor debouncing configuration invalid, e.g. passed threshold larger than failed threshold...
};

/// @brief The DiagReportingErrc enumeration defines the error codes for the DiagErrorDomain.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00560}@tracestatus{draft}
enum class DiagReportingErrc : ara::core::ErrorDomain::CodeType
{
    kAlreadyOffered        = 101,  ///< The service is already offered
    kConfigurationMismatch = 102,  ///< monitor configuration does not match dext
    kDebouncingConfigurationInconsistent
    = 103,  ///<  monitor debouncing configuration invalid, e.g. passed threshold larger than failed threshold...
    kReportIgnored   = 104,  ///<  Enable Conditions disabled, OC not started, ...
    kInvalidArgument = 105,  ///<  e.g. kPreFailed with internal debouncing
    kNotOffered      = 106,  ///<  Offer not called before reporting
    kGenericError    = 107   ///<  generic issue, e.g. connection to DM lost
};

/// @brief Exception type thrown by Diag classes.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00515}@tracestatus{draft}
class DiagException : public ara::core::Exception
{
public:
    /// @brief Construct a new DiagException from an ErrorCode.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] err the ErrorCode
    ///
    ///
    /// @traceid{SWS_DM_00516}@tracestatus{draft}
    explicit DiagException(ara::core::ErrorCode err) noexcept : ara::core::Exception(err) {}
};

/// @brief Exception type thrown by Diag Offer classes.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00985}@tracestatus{draft}
class DiagOfferException : public ara::core::Exception
{
public:
    /// @brief Construct a new DiagOfferException from an ErrorCode.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] err the ErrorCode
    ///
    ///
    /// @traceid{SWS_DM_00986}@tracestatus{draft}
    explicit DiagOfferException(ara::core::ErrorCode err) noexcept : ara::core::Exception(err) {}
};

/// @brief Exception type thrown by Diag Reporting classes.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00987}@tracestatus{draft}
class DiagReportingException : public ara::core::Exception
{
public:
    /// @brief Construct a new DiagReportingException from an ErrorCode.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] err the ErrorCode
    ///
    ///
    /// @traceid{SWS_DM_00988}@tracestatus{draft}
    explicit DiagReportingException(ara::core::ErrorCode err) noexcept : ara::core::Exception(err) {}
};

/// @brief Error domain for diagnostic errors.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00517}@tracestatus{draft}
class DiagErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief kId
    static constexpr ara::core::ErrorDomain::IdType kId{0x8000000000000401};

public:
    /// @brief Alias for the error code value enumeration
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00518}@tracestatus{draft}
    using Errc = DiagErrc;

    /// @brief Alias for the exception base class
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00519}@tracestatus{draft}
    using Exception = DiagException;

    /// @brief Default constructor
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00520}@tracestatus{draft}
    constexpr DiagErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

    /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error domain.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return const char*
    /// @throws on overflow
    /// @traceid{SWS_DM_00521}@tracestatus{draft}
    const char* Name() const noexcept override { return "DiagErrorDomain"; }

    /// @brief Translate an error code value into a text message.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] errorCode  the error code value
    /// @returns the text message, never nullptr
    ///
    ///
    /// @traceid{SWS_DM_00522}@tracestatus{draft}
    const char* Message(ara::core::ErrorDomain::CodeType errorCode) const noexcept override
    {
        ara::core::StringView msg{};
        switch (static_cast< Errc >(errorCode)) {
            case Errc::kAlreadyOffered: {
                msg = ara::core::StringView("kAlreadyOffered: The service is already offered");
            } break;
            case Errc::kConfigurationMismatch: {
                msg = ara::core::StringView("kConfigurationMismatch: monitor configuration does not match dext");
            } break;
            case Errc::kDebouncingConfigurationInconsistent: {
                msg = ara::core::StringView(
                    "kDebouncingConfigurationInconsistent: monitor debouncing configuration invalid");
            } break;
            case Errc::kReportIgnored: {
                msg = ara::core::StringView("kReportIgnored: Enable Conditions disabled, OC not started, ...");
            } break;
            case Errc::kInvalidArgument: {
                msg = ara::core::StringView("kInvalidArgument: e.g. kPreFailed with internal debouncing");
            } break;
            case Errc::kNotOffered: {
                msg = ara::core::StringView("kNotOffered: Offer not called before reporting");
            } break;
            case Errc::kGenericError: {
                msg = ara::core::StringView("kGenericError: generic issue, e.g. connection to DM lost");
            } break;
            case Errc::kNoSuchDTC: {
                msg = ara::core::StringView("kNoSuchDTC: No DTC available");
            } break;
            case Errc::kBusy: {
                msg = ara::core::StringView("kBusy: Interface is busy with processing");
            } break;
            case Errc::kFailed: {
                msg = ara::core::StringView("kFailed: Failed to process");
            } break;
            case Errc::kMemoryError: {
                msg = ara::core::StringView("kMemoryError: A memory error occurred during processing");
            } break;
            case Errc::kWrongDtc: {
                msg = ara::core::StringView("kWrongDtc: A wrong DTC number was requested");
            } break;
            case Errc::kRejected: {
                msg = ara::core::StringView(
                    "kRejected: Requested operation was rejected due to StateManagements/machines internal state");
            } break;
            case Errc::kResetTypeNotSupported: {
                msg = ara::core::StringView(
                    "kResetTypeNotSupported: The requested Diagnostic reset type is not supported");
            } break;
            case Errc::kRequestFailed: {
                msg = ara::core::StringView("kRequestFailed: Diagnostic request could not be performed successfully");
            } break;
            case Errc::kCustomResetTypeNotSupported: {
                msg = ara::core::StringView(
                    "kCustomResetTypeNotSupported: The requested Diagnostic custom reset type is not supported");
            } break;
            default: {
                msg = ara::core::StringView("Unknow error");
            } break;
        }
        return msg.data();
    }

    /// @brief Throw the exception type corresponding to the given ErrorCode.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] errorCode  the ErrorCode instance
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00523}@tracestatus{draft}
    void ThrowAsException(ara::core::ErrorCode const& errorCode) const noexcept(false) override
    {
        ara::core::internal::ThrowOrTerminate< Exception >(errorCode);
    }
};

/// @brief Error domain for diagnostic offer errors.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00989}@tracestatus{draft}
class DiagOfferErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief kId
    static constexpr ara::core::ErrorDomain::IdType kId{0x8000000000000403};

public:
    /// @brief Alias for the error code value enumeration
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00990}@tracestatus{draft}
    using Errc = DiagOfferErrc;

    /// @brief Alias for the exception base class
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00991}@tracestatus{draft}
    using Exception = DiagException;

    /// @brief Default constructor
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00992}@tracestatus{draft}
    constexpr DiagOfferErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

    /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error domain.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return const char*
    /// @throws on overflow
    /// @traceid{SWS_DM_00993}@tracestatus{draft}
    const char* Name() const noexcept override { return "DiagOfferErrorDomain"; }

    /// @brief Translate an error code value into a text message.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param errorCode  the error code value
    /// @returns the text message, never nullptr
    ///
    ///
    /// @traceid{SWS_DM_00994}@tracestatus{draft}
    const char* Message(ara::core::ErrorDomain::CodeType errorCode) const noexcept override
    {
        ara::core::StringView msg{};
        switch (static_cast< Errc >(errorCode)) {
            case Errc::kAlreadyOffered: {
                msg = ara::core::StringView("kAlreadyOffered: The service is already offered");
            } break;
            case Errc::kConfigurationMismatch: {
                msg = ara::core::StringView("kConfigurationMismatch: monitor configuration does not match dext");
            } break;
            case Errc::kDebouncingConfigurationInconsistent: {
                msg = ara::core::StringView(
                    "kDebouncingConfigurationInconsistent: monitor debouncing configuration invalid");
            } break;
            default: {
                msg = ara::core::StringView("Unknow error");
            } break;
        }
        return msg.data();
    }

    /// @brief Throw the exception type corresponding to the given ErrorCode.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] errorCode  the ErrorCode instance
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00995}@tracestatus{draft}
    void ThrowAsException(ara::core::ErrorCode const& errorCode) const noexcept(false) override
    {
        ara::core::internal::ThrowOrTerminate< Exception >(errorCode);
    }
};

/// @brief Error domain for diagnostic reporting errors.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00996}@tracestatus{draft}
class DiagReportingErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief kId
    static constexpr ara::core::ErrorDomain::IdType kId{0x8000000000000402};

public:
    /// @brief Alias for the error code value enumeration
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00997}@tracestatus{draft}
    using Errc = DiagReportingErrc;

    /// @brief Alias for the exception base class
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00998}@tracestatus{draft}
    using Exception = DiagReportingException;

    /// @brief Default constructor
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00999}@tracestatus{draft}
    constexpr DiagReportingErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

    /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error domain.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return const char*
    /// @throws on overflow
    /// @traceid{SWS_DM_01000}@tracestatus{draft}
    const char* Name() const noexcept override { return "DiagReportingErrorDomain"; }

    /// @brief Translate an error code value into a text message.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param errorCode  the error code value
    /// @returns the text message, never nullptr
    ///
    ///
    /// @traceid{SWS_DM_01001}@tracestatus{draft}
    const char* Message(ara::core::ErrorDomain::CodeType errorCode) const noexcept override
    {
        ara::core::StringView msg{};
        switch (static_cast< Errc >(errorCode)) {
            case Errc::kAlreadyOffered: {
                msg = ara::core::StringView("kAlreadyOffered: The service is already offered");
            } break;
            case Errc::kConfigurationMismatch: {
                msg = ara::core::StringView("kConfigurationMismatch: monitor configuration does not match dext");
            } break;
            case Errc::kDebouncingConfigurationInconsistent: {
                msg = ara::core::StringView(
                    "kDebouncingConfigurationInconsistent: monitor debouncing configuration invalid");
            } break;
            case Errc::kReportIgnored: {
                msg = ara::core::StringView("kReportIgnored: Enable Conditions disabled, OC not started, ...");
            } break;
            case Errc::kInvalidArgument: {
                msg = ara::core::StringView("kInvalidArgument: e.g. kPreFailed with internal debouncing");
            } break;
            case Errc::kNotOffered: {
                msg = ara::core::StringView("kNotOffered: Offer not called before reporting");
            } break;
            case Errc::kGenericError: {
                msg = ara::core::StringView("kGenericError: generic issue, e.g. connection to DM lost");
            } break;
            default: {
                msg = ara::core::StringView("Unknow error");
            } break;
        }
        return msg.data();
    }

    /// @brief Throw the exception type corresponding to the given ErrorCode.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param errorCode  the ErrorCode instance
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_01002}@tracestatus{draft}
    void ThrowAsException(ara::core::ErrorCode const& errorCode) const noexcept(false) override
    {
        ara::core::internal::ThrowOrTerminate< Exception >(errorCode);
    }
};

namespace api {
constexpr DiagErrorDomain kG_DiagDomain;
constexpr DiagOfferErrorDomain kG_DiagOfferDomain;
constexpr DiagReportingErrorDomain kG_DiagReportingDomain;
}  // namespace api

/// @brief Obtain the reference to the single global DiagErrorDomain instance.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return reference to the DiagErrorDomain instance
///
///
/// @traceid{SWS_DM_00524}@tracestatus{draft}
constexpr ara::core::ErrorDomain const& GetDiagDomain() noexcept { return api::kG_DiagDomain; }

/// @brief Obtain the reference to the single global DiagOfferErrorDomain instance.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return reference to the DiagOfferErrorDomain instance
///
///
/// @traceid{SWS_DM_01003}@tracestatus{draft}
constexpr ara::core::ErrorDomain const& GetDiagOfferDomain() noexcept { return api::kG_DiagOfferDomain; }

/// @brief Obtain the reference to the single global DiagReportingErrorDomain instance.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return reference to the DiagReportingErrorDomain instance
///
///  @traceid{SWS_DM_01004}@tracestatus{draft}
constexpr ara::core::ErrorDomain const& GetDiagReportingDomain() noexcept { return api::kG_DiagReportingDomain; }

/// @brief Create a new ErrorCode for DiagErrorDomain with the given support data type.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param code an enumeration value from future_errc
/// @param data a vendor-defined supplementary value
/// @return the new ErrorCode instance
///
///
/// @traceid{SWS_DM_00525}@tracestatus{draft}
constexpr ara::core::ErrorCode MakeErrorCode(DiagErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetDiagDomain(), data);
}

/// @brief Create a new ErrorCode for DiagOfferErrorDomain with the given support data type.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param code an enumeration value from future_errc
/// @param data a vendor-defined supplementary value
/// @return the new ErrorCode instance
///
///
/// @traceid{SWS_DM_01005}@tracestatus{draft}
constexpr ara::core::ErrorCode MakeErrorCode(DiagOfferErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetDiagOfferDomain(), data);
}

/// @brief Create a new ErrorCode for DiagReportingErrorDomain with the given support data type.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param code an enumeration value from future_errc
/// @param data a vendor-defined supplementary value
/// @returnthe new ErrorCode instance
/// @return ara::core::ErrorCode
///
/// @traceid{SWS_DM_01006}@tracestatus{draft}
constexpr ara::core::ErrorCode MakeErrorCode(DiagReportingErrc code,
                                             ara::core::ErrorDomain::SupportDataType data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetDiagReportingDomain(), data);
}

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_DIAG_ERROR_DOMAIN_H_