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
/// @file       security_error_domain.h
/// @brief      AutoSar-Crypto Encryption/Decryption common module
/// @details    Error handling for ap-crypto
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr> <td>2021-12-29 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Error Handling
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=MAraCoreInit
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_SECURITY_ERROR_DOMAIN_H_
#define ARA_CRYPTO_SECURITY_ERROR_DOMAIN_H_

#include "ara/core/error_code.h"
#include "ara/core/exception.h"
namespace ara {
namespace crypto {
/// @brief char redefinition
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02124
/// @trace_id_dd=DD_CRYPTO_06396
/// @needwork = dd
/// @endcode
using char8_t = char;  // NOLINT
}  // namespace crypto
}  // namespace ara

namespace ara {
namespace crypto {
//********************************/
/// @brief Enumeration of all security error code values reportable by ara::crypto.
/// @brief Enumeration of all Security Error Code values that may be reported by @c ara::crypto.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10099}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02310}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02121
/// @trace_id_dd=DD_CRYPTO_04889
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=SecurityErrc
/// @needwork = ad
/// @endcode
enum class SecurityErrc : ara::core::ErrorDomain::CodeType
{
    /// @brief Reserved (a multiplier of error class IDs)
    kErrorClass = 0x1000000,
    /// @brief Reserved (a multiplier of error sub-class IDs)
    kErrorSubClass = 0x10000,
    /// @brief Reserved (a multiplier of error sub-sub-class IDs)
    kErrorSubSubClass = 0x100,

    /// @brief ResourceException: Generic resource fault!
    kResourceFault = 1 * kErrorClass,
    /// @brief ResourceException: Specified resource is busy!
    kBusyResource = kResourceFault + 1,
    /// @brief ResourceException: Insufficient capacity of specified resource!
    kInsufficientResource = kResourceFault + 2,
    /// @brief ResourceException: Specified resource was not reserved!
    kUnreservedResource = kResourceFault + 3,
    /// @brief ResourceException: Specified resource has been modified!
    kModifiedResource = kResourceFault + 4,

    /// @brief LogicException: Generic logic fault!
    kLogicFault = 2 * kErrorClass,
    /// @brief InvalidArgumentException: An invalid argument value is provided!
    kInvalidArgument = kLogicFault + 1 * kErrorSubClass,
    /// @brief InvalidArgumentException: Unknown identifier is provided!
    kUnknownIdentifier = kInvalidArgument + 1,
    /// @brief InvalidArgumentException: Insufficient capacity of the output buffer!
    kInsufficientCapacity = kInvalidArgument + 2,
    /// @brief InvalidArgumentException: Invalid size of an input buffer!
    kInvalidInputSize = kInvalidArgument + 3,
    /// @brief InvalidArgumentException: Provided values of arguments are incompatible!
    kIncompatibleArguments = kInvalidArgument + 4,
    /// @brief InvalidArgumentException: Input and output buffers are intersect!
    kInOutBuffersIntersect = kInvalidArgument + 5,
    /// @brief InvalidArgumentException: Provided value is below the lower boundary!
    kBelowBoundary = kInvalidArgument + 6,
    /// @brief InvalidArgumentException: Provided value is above the upper boundary!
    kAboveBoundary = kInvalidArgument + 7,
    /// @brief AuthTagNotValidException: Provided authentication-tag cannot be verified!
    kAuthTagNotValid = kInvalidArgument + 8,
    /// @brief InvalidArgumentException: The provided message data is larger than allowed by the configured context
    /// AlgId
    kIncorrectInputSize = kInvalidArgument + 9,

    /// @brief UnsupportedException: Unsupported request (due to limitations of the implementation)!
    kUnsupported = kInvalidArgument + 1 * kErrorSubSubClass,

    /// @brief InvalidUsageOrderException: Invalid usage order of the interface!
    kInvalidUsageOrder = kLogicFault + 2 * kErrorSubClass,
    /// @brief InvalidUsageOrderException: Context of the interface was not initialized!
    kUninitializedContext = kInvalidUsageOrder + 1,
    /// @brief InvalidUsageOrderException: Data processing was not started yet!
    kProcessingNotStarted = kInvalidUsageOrder + 2,
    /// @brief InvalidUsageOrderException: Data processing was not finished yet!
    kProcessingNotFinished = kInvalidUsageOrder + 3,

    /// @brief RuntimeException: Generic runtime fault!
    kRuntimeFault = 3 * kErrorClass,
    /// @brief RuntimeException: Unsupported serialization format for this object type!
    kUnsupportedFormat = kRuntimeFault + 1,
    /// @brief RuntimeException: Operation is prohibitted due to a risk of a brute force attack!
    kBruteForceRisk = kRuntimeFault + 2,
    /// @brief RuntimeException: The operation violates content restrictions of the target container!
    kContentRestrictions = kRuntimeFault + 3,
    /// @brief RuntimeException: Incorrect reference between objects!
    kBadObjectReference = kRuntimeFault + 4,
    /// @brief RuntimeException: Provided content already exists in the target storage!
    kContentDuplication = kRuntimeFault + 6,

    /// @brief UnexpectedValueException: Unexpected value of an argument is provided!
    kUnexpectedValue = kRuntimeFault + 1 * kErrorSubClass,
    /// @brief UnexpectedValueException: The provided object is incompatible with requested operation or its
    /// configuration!
    kIncompatibleObject = kUnexpectedValue + 1,
    /// @brief UnexpectedValueException: Incomplete state of an argument!
    kIncompleteArgState = kUnexpectedValue + 2,
    /// @brief UnexpectedValueException: Specified container is empty!
    kEmptyContainer = kUnexpectedValue + 3,
    /// @brief kMissingArgumentException: Expected argument, but none provided!
    kMissingArgument = kUnexpectedValue + 4,

    /// @brief BadObjectTypeException: Provided object has unexpected type!
    kBadObjectType = kUnexpectedValue + 1 * kErrorSubSubClass,

    /// @brief UsageViolationException: Violation of allowed usage for the object!
    kUsageViolation = kRuntimeFault + 2 * kErrorSubClass,

    /// @brief AccessViolationException: Access rights violation!
    kAccessViolation = kRuntimeFault + 3 * kErrorSubClass,

    /// @brief Custom error number: Reading IPC data
    kIpcFault = 4 * kErrorClass,
    /// @brief Key slot not initialized: possibly does not exist
    kIpcSlotNotExist = kIpcFault + 1,
    /// @brief IO interface not open: possibly does not exist
    kIpcIoInterfaceNotExist = kIpcFault + 2,
    /// @brief Error occurred while executing IPC save key slot data
    kIpcSlotSaveData = kIpcFault + 3,
    /// @brief Key length is invalid
    kIpcSlotLengthInvalid = kIpcFault + 4,

    /// @brief Custom error number: Function call failure in functions like Init, Update in MAC etc.
    kMacFault = 5 * kErrorClass,
    /// @brief init failed
    kInitFailed = kMacFault + 1,
    /// @brief update failed
    kUpdateFailed = kMacFault + 2,
    /// @brief before ara::core::Intitialize or after ara::core::Deinitialize
    kNotInitialized = 6 * kErrorClass,

    /// @brief Custom error number: Local method failure error value in PSS mode
    kDsvFault = 6 * kErrorClass,
    /// @brief Failed to call RSA_padding_add_PKCS1_PSS method
    kDsvSignerPrivateRsaPss = kDsvFault + 1,
    /// @brief Failed to call RSA_verify_PKCS1_PSS method
    kDsvVerifierPublicRsaPss = kDsvFault + 2,

    /// @brief hsm hardware error
    kDeviceFault = 7 * kErrorClass,
    /// @brief hsm get slotid error
    kGetDeviceSlotId = 7 * kErrorClass + 1,

    /// @brief Custom error number: CRL related error
    kCRLFault = 8 * kErrorClass,
    /// @brief Failed to call _GenerateCRL method, i.e., unable to generate CRL
    kGenerateCRLFault = kCRLFault + 1,
    /// @brief Failed to call _RevokeCert method, i.e., unable to revoke certificate
    kRevokeCertFault = kCRLFault + 2,
    /// @brief Failed to call kLoadCRLFault method, i.e., unable to load CRL
    kLoadCRLFault = kCRLFault + 3,
};
//********************************/
/// @brief Exception type thrown by CRYPTO errors.
/// @brief Exception type thrown for CRYPTO errors.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_19905}
/// @tracestatus={draft}
/// @uptrace={SWS_CORE_10910}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02122
/// @trace_id_dd=DD_CRYPTO_04890
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=SecurityException
/// @needwork = ad
/// @endcode
class SecurityException : public ara::core::Exception
{
public:
    /// @brief Construct a new SecurityException from an ErrorCode.
    /// @brief Construct a new SecurityException from an ErrorCode.
    /// @param err  the ErrorCode
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_19906}
    /// @tracestatus={draft}
    /// @uptrace={SWS_CORE_10910
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02123
    /// @trace_id_dd=DD_CRYPTO_04891
    /// @needwork = ad
    /// @endcode
    /// @brief AutoSar original text: explicit SecurityException(ara::core::ErrorCode const& err) noexcept : ara::core::Exception{err} {}
    using ara::core::Exception::Exception;

public:
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    ~SecurityException() override = default;

    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    SecurityException& operator=(SecurityException const& other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    SecurityException& operator=(SecurityException&& other) noexcept = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    SecurityException(SecurityException const& other) noexcept = default;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    SecurityException(SecurityException&& other) noexcept = default;
};
//********************************/
//- @class SecurityErrorDomain
/// @brief Security error domain class, provides the interface defined by ara::core::ErrorDomain, such as the name of the security error domain or the message for each error code.
///         This class represents the error domain responsible for all errors potentially reported by the public API in the ara::crypto namespace.
/// @brief Security Error Domain class that provides interfaces as defined by ara::core::ErrorDomain such as
///           a name of the Security Error Domain or messages for each error code.
///          This class represents an error domain responsible for all errors that may be reported by
///          public APIs in @c ara::crypto namespace.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_19900}
/// @tracestatus={draft}
/// @uptrace={RS_AP_00130}
/// @domainid{0x8000'0000'0000'0801}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02124
/// @trace_id_dd=DD_CRYPTO_04892
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=SecurityErrorDomain
/// @needwork = ad
/// @endcode
class SecurityErrorDomain final : public ara::core::ErrorDomain
{
public:
    /// @brief Security error
    /// @brief security error
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_19903}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02310}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03187
    /// @trace_id_dd=DD_CRYPTO_06397
    /// @needwork = ad
    /// @endcode
    using Errc = SecurityErrc;
    /// @brief Alias for the exception base class.
    /// @brief Alias for the exception base class
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_19904}
    /// @tracestatus={draft}
    /// @uptrace={SWS_CORE_10934}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03188
    /// @trace_id_dd=DD_CRYPTO_06398
    /// @needwork = ad
    /// @endcode
    using Exception = SecurityException;
    /// @brief kid
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02124
    /// @trace_id_dd=DD_CRYPTO_04893
    /// @needwork = dd
    /// @endcode
    constexpr static ara::core::ErrorDomain::IdType kId{0x8000000000000801U};

public:
    /// @brief Throw an error code exception
    /// @brief throws exception of error code
    /// @param errorCode  an error code identifier from the @c SecurityErrc enumeration
    /// @throw ???
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_19954}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02310}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02125
    /// @trace_id_dd=DD_CRYPTO_04894
    /// @needwork = ad
    /// @endcode
    void ThrowAsException(ara::core::ErrorCode const& errorCode) const final
    {
        ara::core::internal::ThrowOrTerminate< Exception >(errorCode);
    }
    /// @brief Default constructor
    /// @brief Ctor of the SecurityErrorDomain
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_19902}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02310}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02126
    /// @trace_id_dd=DD_CRYPTO_04895
    /// @needwork = ad
    /// @endcode
    constexpr SecurityErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}
    /// @brief Returns the text "Security"
    /// @brief returns Text "Security"
    /// @returns "Security" text
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_19950}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02310}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02127
    /// @trace_id_dd=DD_CRYPTO_04896
    /// @needwork = ad
    /// @endcode
    char8_t const* Name() const noexcept final { return "Security"; }
    /// @brief Converts an error code value to a text message.
    /// @brief Translate an error code value into a text message.
    /// @param errorCode  an error code identifier from the @c SecurityErrc enumeration
    /// @returns message text of error code
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_19953}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02310}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02128
    /// @trace_id_dd=DD_CRYPTO_04897
    /// @needwork = ad
    /// @endcode
    char8_t const* Message(ara::core::ErrorDomain::CodeType errorCode) const noexcept final
    {
        Errc const kCode{static_cast< Errc >(errorCode)};
        ara::core::StringView stReturn;
        switch (kCode) {
            case Errc::kResourceFault: {
                stReturn = "Generic resource fault!";
            } break;
            case Errc::kBusyResource: {
                stReturn = "Specified resource is busy!";
            } break;
            case Errc::kInsufficientResource: {
                stReturn = "Insufficient capacity of specified resource!";
            } break;
            case Errc::kUnreservedResource: {
                stReturn = "Specified resource was not reserved!";
            } break;

            case Errc::kLogicFault: {
                stReturn = "Generic logic fault!";
            } break;

            case Errc::kInvalidArgument: {
                stReturn = "An invalid argument value is provided!";
            } break;
            case Errc::kUnknownIdentifier: {
                stReturn = "Unknown identifier is provided!";
            } break;
            case Errc::kInsufficientCapacity: {
                stReturn = "Insufficient capacity of the output buffer!";
            } break;
            case Errc::kInvalidInputSize: {
                stReturn = "Invalid size of an input buffer!";
            } break;
            case Errc::kIncompatibleArguments: {
                stReturn = "Provided values of arguments are incompatible!";
            } break;
            case Errc::kInOutBuffersIntersect: {
                stReturn = "Input and output buffers are intersect!";
            } break;
            case Errc::kBelowBoundary: {
                stReturn = "Provided value is below the lower boundary!";
            } break;
            case Errc::kAboveBoundary: {
                stReturn = "Provided value is above the upper boundary!";
            } break;

            case Errc::kUnsupported: {
                stReturn = "Unsupported request (due to limitations of the implementation)!";
            } break;

            case Errc::kInvalidUsageOrder: {
                stReturn = "Invalid usage order of the interface!";
            } break;
            case Errc::kUninitializedContext: {
                stReturn = "Context of the interface was not initialized!";
            } break;
            case Errc::kProcessingNotStarted: {
                stReturn = "Data processing was not started yet!";
            } break;
            case Errc::kProcessingNotFinished: {
                stReturn = "Data processing was not finished yet!";
            } break;

            case Errc::kRuntimeFault: {
                stReturn = "Generic runtime fault!";
            } break;
            case Errc::kUnsupportedFormat: {
                stReturn = "Unsupported serialization format for this object type!";
            } break;
            case Errc::kBruteForceRisk: {
                stReturn = "Operation is prohibitted due to a risk of a brute force attack!";
            } break;
            case Errc::kContentRestrictions: {
                stReturn = "The operation violates content restrictions of the target container!";
            } break;
            case Errc::kBadObjectReference: {
                stReturn = "Incorrect reference between objects!";
            } break;
            case Errc::kContentDuplication: {
                stReturn = "Provided content already exists in the target storage!";
            } break;

            case Errc::kUnexpectedValue: {
                stReturn = "Unexpected value of an argument is provided!";
            } break;
            case Errc::kIncompatibleObject: {
                stReturn = "The provided object is incompatible with requested operation or its configuration!";
            } break;
            case Errc::kIncompleteArgState: {
                stReturn = "Incomplete state of an argument!";
            } break;
            case Errc::kEmptyContainer: {
                stReturn = "Specified container is empty!";
            } break;

            case Errc::kBadObjectType: {
                stReturn = "Provided object has unexpected type!";
            } break;

            case Errc::kUsageViolation: {
                stReturn = "Violation of allowed usage for the object!";
            } break;

            case Errc::kAccessViolation: {
                stReturn = "Access rights violation!";
            } break;
            case Errc::kIpcFault: {
                stReturn = "Custom error number: read IPC data";
            } break;
            case Errc::kIpcSlotNotExist: {
                stReturn = "Custom error number: slot not exist";
            } break;
            case Errc::kIpcIoInterfaceNotExist: {
                stReturn = "the I/O interface is not Open, it may not exist";
            } break;
            case Errc::kIpcSlotSaveData: {
                stReturn = "An error occurred while saving key slot data";
            } break;
            case Errc::kMissingArgument: {
                stReturn = "Expected argument, but none provided!";
            } break;
            case Errc::kInitFailed: {
                stReturn = "Init failed! ";
            } break;
            case Errc::kUpdateFailed: {
                stReturn = "Update failed! ";
            } break;
            case Errc::kIncorrectInputSize: {
                stReturn = "The provided message data is larger than allowed by the configured context AlgId! ";
            } break;
            case Errc::kAuthTagNotValid: {
                stReturn = " AuthTagNotValidException:Provided authentication-tagcannotbeverified";
            } break;
            case Errc::kModifiedResource: {
                stReturn = "ResourceException: Specified resource has been modified!";
            } break;
            default: {
                stReturn = "Unknown error!";
            } break;
        }
        return stReturn.data();
    }

public:
};
//********************************/
namespace internal {
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02129
/// @trace_id_dd=DD_CRYPTO_04898
/// @needwork = dd
/// @endcode
constexpr SecurityErrorDomain kSpecificErrorsErrorDomain;
}  // namespace internal
/// @brief Returns a reference to the global CryptoErrorDomain.
/// @brief Return a reference to the global CryptoErrorDomain.
/// @returns the CryptoErrorDomain
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_19952}
/// @tracestatus={draft}
/// @uptrace={SWS_CORE_10980}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02129
/// @trace_id_dd=DD_CRYPTO_04899
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=GetCryptoErrorDomain
/// @needwork = ad
/// @endcode
constexpr ara::core::ErrorDomain const& GetCryptoErrorDomain() noexcept { return internal::kSpecificErrorsErrorDomain; }
//********************************/
/// @brief Generates an error code instance from the security error domain. The returned ErrorCode instance always references the SecurityErrorDomain.
/// @brief Makes Error Code instances from the Security Error Domain.
///  The returned @c ErrorCode instance always references to @c SecurityErrorDomain.
/// @param code  an error code identifier from the @c SecurityErrc enumeration
/// @param data supplementary data for the error description
/// @returns an instance of @c ErrorCode created according the arguments
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_19951}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02310}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02130
/// @trace_id_dd=DD_CRYPTO_04900
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=MakeErrorCode
/// @needwork = ad
/// @endcode
constexpr ara::core::ErrorCode MakeErrorCode(SecurityErrorDomain::Errc const code,
                                             ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    // TODO(username):
    return ara::core::ErrorCode{static_cast< ara::core::ErrorDomain::CodeType >(code), GetCryptoErrorDomain(), data};
}
//********************************/
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_SECURITY_ERROR_DOMAIN_H_
