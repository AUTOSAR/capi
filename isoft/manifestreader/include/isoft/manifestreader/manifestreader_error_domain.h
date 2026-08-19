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
/// @file       manifestreader_error_domain.h
/// @brief
/// @details
/// @date       2021-04-14
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_MANIFESTREADER_INCLUDE_PUBLIC_ISOFT_MANIFESTREADER_MANIFESTREADER_ERROR_DOMAIN_H_
#define ISOFT_MANIFESTREADER_INCLUDE_PUBLIC_ISOFT_MANIFESTREADER_MANIFESTREADER_ERROR_DOMAIN_H_

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace isoft {
namespace manifestreader {

enum class ManifestReaderErrc : ara::core::ErrorDomain::CodeType
{
    kUnknown              = -1,  ///< Placeholder for investigation
    kOpenFile             = 1,   ///< Error while opening the manifest file.
    kParsing              = 2,   ///< Error while parsing manifest with rapidjson.
    kKeyNotFound          = 3,   ///< Key not found
    kInvalidTypeRequested = 4,   ///< Attempt to receive data with valid key but of other type
    kInvalidChecker       = 5,   ///< check failed
};

class ManifestReaderException : public ara::core::Exception
{
public:
    explicit ManifestReaderException(ara::core::ErrorCode err) noexcept : ara::core::Exception(err) {}
};

class ManifestReaderErrorDomain final : public ara::core::ErrorDomain
{
    constexpr static ara::core::ErrorDomain::IdType kId = 0xC000000000000002;

public:
    using Errc      = ManifestReaderErrc;
    using Exception = ManifestReaderException;

    constexpr ManifestReaderErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

    /// @brief Return the "shortname" of this error domain.
    char const* Name() const noexcept override { return "ApdManifestReader"; }

    char const* Message(ara::core::ErrorDomain::CodeType errorCode) const noexcept override
    {
        Errc const code = static_cast< Errc >(errorCode);
        switch (code) {
            default:
                return "Unknown error";
        }
    }

    void ThrowAsException(ara::core::ErrorCode const& errorCode) const noexcept(false) override
    {
        ara::core::internal::ThrowOrTerminate< Exception >(errorCode);
    }
};

namespace internal {
constexpr ManifestReaderErrorDomain g_ManifestReaderErrorDomain;
}

inline constexpr ara::core::ErrorDomain const& GetManifestReaderErrorDomain() noexcept
{
    return internal::g_ManifestReaderErrorDomain;
}

constexpr ara::core::ErrorCode MakeErrorCode(ManifestReaderErrc code,
                                             ara::core::ErrorDomain::SupportDataType data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetManifestReaderErrorDomain(),
                                data);
}

}  // namespace manifestreader
}  // namespace isoft

#endif  // ISOFT_MANIFESTREADER_INCLUDE_PUBLIC_ISOFT_MANIFESTREADER_MANIFESTREADER_ERROR_DOMAIN_H_
