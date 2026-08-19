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
/// @file       exception.h
/// @brief      The definition and implementation of ara::core::Exception type that constitutes the base type for all exception types defined by the Adaptive Platform.
/// @details
/// @date       2022-06-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_EXCEPTIONS_H
#define ARA_CORE_EXCEPTIONS_H

#include <exception>
#include <ostream>
#include <system_error>
#include <utility>

#include "ara/core/error_code.h"

namespace ara {
namespace core {

///
/// @brief Base type for all AUTOSAR exception types.
///
/// @uptrace{SWS_CORE_00601, be210d5a6b1032badcea526ed5b523c4fe93cfa7}
class Exception : public std::exception
{
private:
    ErrorCode const kErrorCode;

public:
    /// @uptrace{SWS_CORE_00611, 1eaae4781c1cd4222528289b9a1adb577845dc03}
    explicit Exception(ErrorCode err) noexcept : kErrorCode(std::move(err)) {}

    /// @uptrace{SWS_CORE_00613, acacd31a46488d3f3bc3042f08e6edadd00d9f82}
    ErrorCode const& Error() const noexcept { return kErrorCode; }

    /// @uptrace{SWS_CORE_00612, f06f333e88d932c101d60f977d8fcc6550915e4a}
    char const* what() const noexcept override { return std::exception::what(); }

public:
    ~Exception() override = default;
    /// @brief Copy constructor
    /// @param[in] object Exception&
    Exception(Exception const& object) = default;
    /// @brief Copy the assignment function
    /// @param[in] object Exception&
    /// @return Exception&
    /// @uptrace{SWS_CORE_00614, 64d8a3ccc8be6a4f4433398b3ab204684d05992a}
    Exception& operator=(Exception const& object) = delete;
    /// @briefMove constructor
    /// @param[in] object Exception&& object
    Exception(Exception&& object) noexcept = default;
    /// @brief Move operation
    /// @param[in] object Exception&& object
    /// @return Exception&
    Exception& operator=(Exception&& object) noexcept = delete;
};

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_EXCEPTIONS_H
