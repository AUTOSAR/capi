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
/// @file       core_error_domain.h
/// @brief      The definition and implementation of ara::core::CoreErrorDomain type that derives from
/// @details    ara::core::ErrorDomain and contains the errors that can originate from within the CORE Functional Cluster.
/// @date       2022-06-21
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_CORE_ERROR_DOMAIN_H_
#define ARA_CORE_CORE_ERROR_DOMAIN_H_

#include <cerrno>

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace ara {
namespace core {

/// @brief An enumeration with errors that can occur within this Functional Cluster
///
/// @uptrace{SWS_CORE_05200, e335c1bf88a743aeba47205f71ccaebfedd7837a}
/// @uptrace{SWS_CORE_10900, 3daf455c6e7cf766c561b2a25130f3aee35a73d0}
/// @uptrace{SWS_CORE_10901, 332efd5fe2b3e4caec01a7b13d12630a0a50a161}
/// @uptrace{SWS_CORE_10902, 3e0ab1b030e5481dadd92e08015ddcb9f161c0c0}
/// @uptrace{SWS_CORE_10903, f3a4b9876e3b360145a2f97a5f51916c48650f1a}
/// @uptrace{SWS_CORE_10999, 59d020b68015c3dd68ba3aabd01365a5a4c00768}
enum class CoreErrc : ErrorDomain::CodeType
{
    kInvalidArgument           = 22,   ///< invalid argument
    kInvalidMetaModelShortname = 137,  ///< given string is not a valid model element shortname
    kInvalidMetaModelPath      = 138,  ///< missing or invalid path to model element
};

/// @brief Exception type thrown for CORE errors.
///
/// @uptrace{SWS_CORE_05211, 6b40401b6cc596428f44107de03ebaaf0d23bf8d}
/// @uptrace{SWS_CORE_10910, 3e0dbf4c1340ce8f7e21390a4d08e2cb952c4be9}
/// @uptrace{SWS_CORE_10911, 7652906475f7ffc803fa77abb6143edcd4d3f9ea}
/// @uptrace{SWS_CORE_10999, 59d020b68015c3dd68ba3aabd01365a5a4c00768}
class CoreException : public Exception
{
public:
    /**
     * @brief Construct a new CoreException from an ErrorCode.
     * @param err  the ErrorCode
     *
     * @uptrace{SWS_CORE_05212, f31d7abbdf343ee1ae8a11ec3a3d385cbbeb36da}
     */
    explicit CoreException(ErrorCode err) noexcept : Exception(err) {}
};

/// @brief An error domain for ara::core errors.
///
/// @uptrace{SWS_CORE_00011, 8c1f931120c51488e2b531543bd1c15f34d5901b}
/// @uptrace{SWS_CORE_05221, 538ece338b7dd0d075d47a34c6c004cd4d59a6f5}
/// @uptrace{SWS_CORE_10400, 0142c62ae23d9207a7fea2aeb181131c5e53b481}
/// @uptrace{SWS_CORE_10930, 8ec9148df6042877e71cbb036f511c175f5ec7ba}
/// @uptrace{SWS_CORE_10931, 4864b93da62f0328c906fbbbbe508b7d0cf54cdb}
/// @uptrace{SWS_CORE_10932, 8c787e2aee47b4e6100972097de83eb6fe9e108f}
/// @uptrace{SWS_CORE_10950, d806419650aa3e31e2207efc4c2ced4a37f9deff}
/// @uptrace{SWS_CORE_10999, 59d020b68015c3dd68ba3aabd01365a5a4c00768}
class CoreErrorDomain final : public ErrorDomain
{
    constexpr static ErrorDomain::IdType kId = 0x8000000000000014;

public:
    /// @brief Alias for the error code value enumeration
    ///
    /// @uptrace{SWS_CORE_05231, cbc97baeb641245a26e2116268ec79e91adad3a3}
    /// @uptrace{SWS_CORE_10933, 9831758d60afd125a57a3e1f234b0c99950cbacc}
    using Errc = CoreErrc;

    /// @brief Alias for the exception base class
    ///
    /// @uptrace{SWS_CORE_05232, 975eef28bfc3232ffc21051c99647c512e83069a}
    /// @uptrace{SWS_CORE_10934, 1ad643bb41b8713cbf8d8509f7c6b2f8d7c332ea}
    using Exception = CoreException;

    /// @brief Default constructor
    ///
    /// @uptrace{SWS_CORE_05241, d58f187ed597e27e3db3d490a28bdbf50cc5a9f4}
    /// @uptrace{SWS_CORE_00014, 4448596ec421fc7980b549e3473b417cff14324a}
    constexpr CoreErrorDomain() noexcept : ErrorDomain(kId) {}

    /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error domain.
    /// @returns "Core"
    ///
    /// @uptrace{SWS_CORE_05242, c50e3baf0a82dfca2ef68558eab69705d03eb630}
    /// @uptrace{SWS_CORE_00014, 4448596ec421fc7980b549e3473b417cff14324a}
    /// @uptrace{SWS_CORE_10951, 15768dd685697ff3573bca7fb11fc5d5aa5890c6}
    char const* Name() const noexcept override { return "Core"; }

    /// @brief Translate an error code value into a text message.
    /// @param errorCode  the error code value
    /// @returns the text message, never nullptr
    ///
    /// @uptrace{SWS_CORE_05243, 4e6aa85d836086353c9273796103eec7a2c59e14}
    char const* Message(ErrorDomain::CodeType errorCode) const noexcept override
    {
        Errc const code = static_cast< Errc >(errorCode);
        switch (code) {
            case Errc::kInvalidArgument:
                return "Invalid argument";
            case Errc::kInvalidMetaModelShortname:
                return "Invalid meta model shortname";
            case Errc::kInvalidMetaModelPath:
                return "Invalid meta model path";
            default:
                return "Unknown error";
        }
    }

    /// @brief Throw the exception type corresponding to the given ErrorCode.
    /// @param errorCode  the ErrorCode instance
    ///
    /// @uptrace{SWS_CORE_05244, 04e6694764e2cad009f923154e5516fad9549986}
    void ThrowAsException(ErrorCode const& errorCode) const noexcept(false) override
    {
        /// @uptrace{SWS_CORE_10953, 36d8ef828b0244ac573e536abc463ebab5890f2d}
        internal::ThrowOrTerminate< Exception >(errorCode);
    }
};

namespace internal {
constexpr CoreErrorDomain kCoreErrorDomain;
}  // namespace internal

/// @brief Return a reference to the global CoreErrorDomain.
/// @returns the CoreErrorDomain
///
/// @uptrace{SWS_CORE_05280, 4c8e15250f7089acb4952d258e89fc6a2dccea39}
/// @uptrace{SWS_CORE_10980, 7b3527e548da813f22785e8be0ecdf00830ad9cb}
/// @uptrace{SWS_CORE_10981, a22ca5477e277b1e1da68c5b8f641ce52d6518b2}
/// @uptrace{SWS_CORE_10982, 81fb283c392bb9e68639bd90ff71373109230314}
/// @uptrace{SWS_CORE_10999, 59d020b68015c3dd68ba3aabd01365a5a4c00768}
constexpr ErrorDomain const& GetCoreErrorDomain() noexcept { return internal::kCoreErrorDomain; }

/// @brief Create a new ErrorCode within CoreErrorDomain.
///
/// This function is used internally by constructors of ErrorCode. It is usually not
/// used directly by users.
///
/// @param code  the CoreErrorDomain-specific error code value
/// @param data  optional vendor-specific error data
/// @returns a new ErrorCode instance
///
/// @uptrace{SWS_CORE_05290, f2b33f86557b9f9abd1d47b26dab2fcc41adedbb}
/// @uptrace{SWS_CORE_10990, 20da35eb88503606d9b0a645c0ae8b59090bbfe7}
/// @uptrace{SWS_CORE_10991, df4e2b40a1a0f62208151663589cda3d8b4f2c3f}
/// @uptrace{SWS_CORE_10999, 59d020b68015c3dd68ba3aabd01365a5a4c00768}
constexpr ErrorCode MakeErrorCode(CoreErrc code, ErrorDomain::SupportDataType data) noexcept
{
    return ErrorCode(static_cast< ErrorDomain::CodeType >(code), GetCoreErrorDomain(), data);
}

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_CORE_ERROR_DOMAIN_H_
