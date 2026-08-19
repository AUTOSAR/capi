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
/// @file       future_error_domain.h
/// @brief      The definition and implementation of ara::core::FutureErrorDomain type that derives from ara::core::ErrorDomain and contains the errors equivalent to those defined by std::future_errc.
/// @details
/// @date       2022-06-21
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_FUTURE_ERROR_DOMAIN_H_
#define ARA_CORE_FUTURE_ERROR_DOMAIN_H_

#include <cstdint>

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace ara {
namespace core {

/**
 * @brief Specifies the types of internal errors that can occur upon calling Future::get or Future::GetResult.
 *
 * These definitions are equivalent to the ones from std::future_errc.
 *
 * @uptrace{SWS_CORE_00400, 454c409cd8d545b2659b8dde0064a8cc7e1f1396}
 * @uptrace{SWS_CORE_10900, 3daf455c6e7cf766c561b2a25130f3aee35a73d0}
 * @uptrace{SWS_CORE_10902, 3e0ab1b030e5481dadd92e08015ddcb9f161c0c0}
 * @uptrace{SWS_CORE_10903, f3a4b9876e3b360145a2f97a5f51916c48650f1a}
 * @uptrace{SWS_CORE_10999, 59d020b68015c3dd68ba3aabd01365a5a4c00768}
 */
enum class future_errc : int32_t  // NOLINT
{
    broken_promise            = 101,  // NOLINT ///< the asynchronous task abandoned its shared state
    future_already_retrieved  = 102,  // NOLINT ///< the contents of the shared state were already accessed
    promise_already_satisfied = 103,  // NOLINT ///< attempt to store a value into the shared state twice
    no_state                  = 104,  // NOLINT ///< attempt to access Promise or Future without an associated state
};

/**
 * @brief Exception type thrown by Future and Promise classes.
 *
 * @uptrace{SWS_CORE_00411, 3a2cec59144c3e5079818637f99b260d4ec45e64}
 * @uptrace{SWS_CORE_10910, 3e0dbf4c1340ce8f7e21390a4d08e2cb952c4be9}
 * @uptrace{SWS_CORE_10911, 7652906475f7ffc803fa77abb6143edcd4d3f9ea}
 * @uptrace{SWS_CORE_10999, 59d020b68015c3dd68ba3aabd01365a5a4c00768}
 */
class FutureException : public Exception
{
public:
    /**
     * @brief Construct a new FutureException from an ErrorCode.
     * @param err  the ErrorCode
     *
     * @uptrace{SWS_CORE_00412, b01a27e6117803e5c9d93aa45424fb4449f3899e}
     */
    explicit FutureException(ErrorCode err) noexcept : Exception(std::move(err)) {}
};

/**
 * @brief Error domain for errors originating from classes Future and Promise.
 *
 * @uptrace{SWS_CORE_00011, 8c1f931120c51488e2b531543bd1c15f34d5901b}
 * @uptrace{SWS_CORE_00421, d5a481dcf6d9d7fe1a06b6eb5eaee776f8da5494}
 * @uptrace{SWS_CORE_10400, 0142c62ae23d9207a7fea2aeb181131c5e53b481}
 * @uptrace{SWS_CORE_10930, 8ec9148df6042877e71cbb036f511c175f5ec7ba}
 * @uptrace{SWS_CORE_10931, 4864b93da62f0328c906fbbbbe508b7d0cf54cdb}
 * @uptrace{SWS_CORE_10932, 8c787e2aee47b4e6100972097de83eb6fe9e108f}
 * @uptrace{SWS_CORE_10950, d806419650aa3e31e2207efc4c2ced4a37f9deff}
 * @uptrace{SWS_CORE_10999, 59d020b68015c3dd68ba3aabd01365a5a4c00768}
 */
class FutureErrorDomain final : public ErrorDomain
{
    constexpr static ErrorDomain::IdType kId = 0x8000000000000013;

public:
    /// @brief Alias for the error code value enumeration
    ///
    /// @uptrace{SWS_CORE_00431, 11858e3c0bc82c63c40ce5b4576623206aea4846}
    /// @uptrace{SWS_CORE_10933, 9831758d60afd125a57a3e1f234b0c99950cbacc}
    using Errc = future_errc;

    /// @brief Alias for the exception base class
    ///
    /// @uptrace{SWS_CORE_00432, 90b3a6fe12c2b7f97fcefa16e4e790b4bd6eae27}
    /// @uptrace{SWS_CORE_10934, 1ad643bb41b8713cbf8d8509f7c6b2f8d7c332ea}
    using Exception = FutureException;

    /// @brief Default constructor
    ///
    /// @uptrace{SWS_CORE_00441, 324727a28643bd37973c74de0624f3dcfe4a3689}
    /// @uptrace{SWS_CORE_00013, d790a6a820c1f01e94e12e873ecb0709e5e0ba38}
    constexpr FutureErrorDomain() noexcept : ErrorDomain(kId) {}

    /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error domain.
    /// @returns "Future"
    ///
    /// @uptrace{SWS_CORE_00442, e59a2fd5f3f10fa6dc07496fd917c4c07a92c111}
    /// @uptrace{SWS_CORE_00013, d790a6a820c1f01e94e12e873ecb0709e5e0ba38}
    /// @uptrace{SWS_CORE_10951, 15768dd685697ff3573bca7fb11fc5d5aa5890c6}
    char const* Name() const noexcept override { return "Future"; }

    /// @brief Translate an error code value into a text message.
    /// @param errorCode  the error code value
    /// @returns the text message, never nullptr
    ///
    /// @uptrace{SWS_CORE_00443, 5f252453735286c049e72708c52f66069cd789cc}
    char const* Message(ErrorDomain::CodeType errorCode) const noexcept override
    {
        Errc const code = static_cast< Errc >(errorCode);
        switch (code) {
            case Errc::broken_promise:
                return "broken promise";
            case Errc::future_already_retrieved:
                return "future already retrieved";
            case Errc::promise_already_satisfied:
                return "promise already satisfied";
            case Errc::no_state:
                return "no state associated with this future";
            default:
                return "unknown future error";
        }
    }

    /// @brief Throw the exception type corresponding to the given ErrorCode.
    /// @param errorCode  the ErrorCode instance
    ///
    /// @uptrace{SWS_CORE_00444, a58f5b97e7a9c6018c4fcaefa6a422c6f72075dc}
    void ThrowAsException(ErrorCode const& errorCode) const noexcept(false) override
    {
        /// @uptrace{SWS_CORE_10953, 36d8ef828b0244ac573e536abc463ebab5890f2d}
        internal::ThrowOrTerminate< Exception >(errorCode);
    }
};

namespace internal {
constexpr FutureErrorDomain kFutureErrorDomain;
}  // namespace internal

/**
 * @brief Obtain the reference to the single global FutureErrorDomain instance.
 * @returns reference to the FutureErrorDomain instance
 *
 * @uptrace{SWS_CORE_00480, fe1732f63be070bcff226f09ba5574d86a816624}
 * @uptrace{SWS_CORE_10980, 7b3527e548da813f22785e8be0ecdf00830ad9cb}
 * @uptrace{SWS_CORE_10981, a22ca5477e277b1e1da68c5b8f641ce52d6518b2}
 * @uptrace{SWS_CORE_10982, 81fb283c392bb9e68639bd90ff71373109230314}
 * @uptrace{SWS_CORE_10999, 59d020b68015c3dd68ba3aabd01365a5a4c00768}
 */
constexpr ErrorDomain const& GetFutureErrorDomain() noexcept { return internal::kFutureErrorDomain; }

/**
 * @brief Create a new ErrorCode for FutureErrorDomain with the given support data type and message.
 *
 * @param code  an enumeration value from future_errc
 * @param data  a vendor-defined supplementary value
 * @returns the new ErrorCode instance
 *
 * @uptrace{SWS_CORE_00490, c01b5cd20e1dfbad3abb7ed3d2a85763dab8e38b}
 * @uptrace{SWS_CORE_10990, 20da35eb88503606d9b0a645c0ae8b59090bbfe7}
 * @uptrace{SWS_CORE_10991, df4e2b40a1a0f62208151663589cda3d8b4f2c3f}
 * @uptrace{SWS_CORE_10999, 59d020b68015c3dd68ba3aabd01365a5a4c00768}
 */
constexpr ErrorCode MakeErrorCode(future_errc code, ErrorDomain::SupportDataType data) noexcept
{
    return ErrorCode(static_cast< ErrorDomain::CodeType >(code), GetFutureErrorDomain(), data);
}

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_FUTURE_ERROR_DOMAIN_H_
