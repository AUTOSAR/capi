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
/// @file       error_domain.h
/// @brief      The definition and implementation of ErrorDomain type that constitutes a base class for error domain implementations.
/// @details
/// @date       2022-06-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_ERROR_DOMAIN_H_
#define ARA_CORE_ERROR_DOMAIN_H_

#include <cstdint>
#include <exception>

namespace ara {
namespace core {

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif

// forward declaration
class ErrorCode;

///
/// @brief Encapsulation of an error domain.
///        An error domain is the controlling entity for ErrorCode’s error code values, and defines the
///        mapping of such error code values to textual representations.
///        This class is a literal type, and subclasses are strongly advised to be literal types as well.
///
/// @uptrace{SWS_CORE_00110, bc1caeec282c5b60a85a3d03f7cf075b3912aafd}
/// @uptrace{SWS_CORE_10400, 0142c62ae23d9207a7fea2aeb181131c5e53b481}
class ErrorDomain
{
public:
    /// @uptrace{SWS_CORE_00121, 02bf5b86ec1837463027b482ebb0035478e40ef2}
    using IdType = std::uint64_t;
    /// @uptrace{SWS_CORE_00122, e7b550a3a4f0e6aac81d3b00aabcf5a58e919909}
    using CodeType = std::int32_t;
    /// @uptrace{SWS_CORE_00123, 73df17f65774d10478eeda2c825dba253c0ad709}
    using SupportDataType = std::int32_t;

    /// @uptrace{SWS_CORE_00131, 7ed6bb98d73a81537f9248234c84853cb63dfb59}
    ErrorDomain(ErrorDomain const&) = delete;
    /// @uptrace{SWS_CORE_00132, d3e51bea0616672dd1f5c81e2b4c0345ab6fcf48}
    ErrorDomain(ErrorDomain&&) = delete;
    /// @uptrace{SWS_CORE_00133, 9ec5be7720475e767cadae1417ee2c7e9e6c085c}
    ErrorDomain& operator=(ErrorDomain const&) = delete;
    /// @uptrace{SWS_CORE_00134, af29c8a9416a9bd8a719ac3cd94c394fedc7de4d}
    ErrorDomain& operator=(ErrorDomain&&) = delete;

    /// @uptrace{SWS_CORE_00152, f78d2da6be39cc6925f4b06e8d65d124f3428dd4}
    virtual char const* Name() const noexcept = 0;

    /// @uptrace{SWS_CORE_00153, 66f49cc16c41cf89bf498ce8f8af695da69c0219}
    virtual char const* Message(CodeType errorCode) const noexcept = 0;

    /**
     * @brief Throws the given errorCode as Exception
     * @param errorCode error code to be thrown
     * @remark if ARA_NO_EXCEPTIONS is defined, this function call will terminate.
     * @uptrace{SWS_CORE_00154, f5bfa0f796f8e9d298b555b02fdcf6c2fc94eeae}
     */
    [[noreturn]] virtual void ThrowAsException(ErrorCode const& errorCode) const noexcept(false) = 0;

    /// @uptrace{SWS_CORE_00151, 19a5449ead75d53fb71cf6982558690a03e6ff07}
    /// @uptrace{SWS_CORE_10952, 2348e876930b4180a3fb56225f68e7d7c797bed8}
    constexpr IdType Id() const noexcept { return kId; }

    /// @uptrace{SWS_CORE_00137, 2133aaa0d4f34c600a6826a7c0760368ba545a0d}
    constexpr bool operator==(ErrorDomain const& other) const noexcept { return kId == other.kId; }

    /// @uptrace{SWS_CORE_00138, 079432af24d8980708611cd338eee61898aedd42}
    constexpr bool operator!=(ErrorDomain const& other) const noexcept { return kId != other.kId; }

protected:
    /// @uptrace{SWS_CORE_00135, 6260cbf4a54ba37d696753e3fb55956c9f7377fa}
    constexpr explicit ErrorDomain(IdType id) noexcept : kId(id) {}

    /// @brief Destructor
    ///
    /// This dtor is non-virtual (and trivial) so that this class is a literal type.
    /// While we do have virtual functions, we never need polymorphic destruction.
    ///
    /// @uptrace{SWS_CORE_00136, 4f6d6695ecf94a86e3e9c5ba51226cee3002783f}
    ~ErrorDomain() = default;

private:
    IdType const kId;
};

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_ERROR_DOMAIN_H_
