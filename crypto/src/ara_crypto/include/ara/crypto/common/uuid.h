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
/// @file       uuid.h
/// @brief      AutoSar-Crypto Encryption/Decryption common module
/// @details    Definition of the Globally Unique Identifier (UUID) type.
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <td> <td>2021-12-29 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06001
/// @unit_name=UUID
/// @unit_description=Definition of the Globally Unique Identifier (UUID) type
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_UUID_H_
#define ARA_CRYPTO_UUID_H_

#include <cstdint>
namespace ara {
namespace crypto {
//********************************/
//- @struct Uuid
/// @brief Definition of the Globally Unique Identifier (UUID) type. Independent of the internal definition details of this structure, its size must be 16 bytes, and the entropy of this ID should be close to 128 bits!
/// @brief Regardless of the specific internal definition of the Globally Unique Identifier (UUID) type, the following requirements must be met:
/// @brief 1. The length of the UUID must be 16 bytes; 2. The UUID must have high randomness and uniqueness, close to the almost all possible combinations provided by the theoretical 128-bit binary number.
/// @brief Definition of Universally Unique Identifier (@b UUID) type.
///    Independently from internal definition details of this structure, it's size @b must be 16 bytes and entropy of
///    this ID should be close to 128 bit!
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10400}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02005}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02131
/// @trace_id_dd=DD_CRYPTO_04901
/// @needwork = ad
/// @qac [2198]possibly cannot be modified: This struct explicitly defines member functions.
/// @endcode
// PRQA S 2198 QAC /// @qac: AUTOSAR standard interface
struct Uuid final
// PRQA L:QAC
{
public:
    /// @brief Least significant QWORD.
    /// @trace_id_sws={SWS_CRYPT_10412}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// Less significant QWORD.
    std::uint64_t mQwordLs{0U};
    /// @brief Most significant QWORD.
    /// @trace_id_sws={SWS_CRYPT_10413}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// Most significant QWORD.
    std::uint64_t mQwordMs{0U};

public:
    /// @brief Check whether this identifier is a "Nil UUID" (according to RFC4122).
    /// @brief Check whether this identifier is the "Nil UUID" (according to RFC4122).
    /// @return @c true if this identifier is "Nil" and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10411}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @endcode
    bool IsNil() const noexcept;
};
/// @brief Check whether this identifier is a "Nil UUID" (according to RFC4122).
/// @return true if uuid is nil false otherwise
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02132
/// @trace_id_dd=DD_CRYPTO_04902
/// @needwork = ad
/// @endcode
inline bool Uuid::IsNil() const noexcept { return (0U == mQwordMs) && (0U == mQwordLs); }
//********************************/
/// @brief Comparison operator "equal" for Uuid operands.
/// @brief Comparison operator "equal" for @c Uuid operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @return @c true if a binary representation of @c lhs is equal to @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10451}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02112}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02133
/// @trace_id_dd=DD_CRYPTO_04903
/// @needwork = ad
/// @endcode
inline constexpr bool operator==(Uuid const& lhs, Uuid const& rhs) noexcept
{
    return (lhs.mQwordMs == rhs.mQwordMs) && (lhs.mQwordLs == rhs.mQwordLs);
}
/// @brief Comparison operator "less than" for Uuid operands.
/// @brief Comparison operator "less than" for @c Uuid operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @return @c true if a binary representation of @c lhs is less than @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10452}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02112}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02134
/// @trace_id_dd=DD_CRYPTO_04904
/// @needwork = ad
/// @endcode
inline constexpr bool operator<(Uuid const& lhs, Uuid const& rhs) noexcept
{
    return (lhs.mQwordMs < rhs.mQwordMs) || ((lhs.mQwordMs == rhs.mQwordMs) && (lhs.mQwordLs < rhs.mQwordLs));
}
/// @brief Comparison operator "greater than" for Uuid operands.
/// @brief Comparison operator "greater than" for @c Uuid operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @return @c true if a binary representation of @c lhs is greater than @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10453}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02112}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02135
/// @trace_id_dd=DD_CRYPTO_04905
/// @needwork = ad
/// @endcode
inline constexpr bool operator>(Uuid const& lhs, Uuid const& rhs) noexcept
{
    return (lhs.mQwordMs > rhs.mQwordMs) || ((lhs.mQwordMs == rhs.mQwordMs) && (lhs.mQwordLs > rhs.mQwordLs));
}
/// @brief Comparison operator "not equal" for Uuid operands.
/// @brief Comparison operator "not equal" for @c Uuid operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @return @c true if a binary representation of @c lhs is not equal to @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10454}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02112}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02136
/// @trace_id_dd=DD_CRYPTO_04906
/// @needwork = ad
/// @endcode
inline constexpr bool operator!=(Uuid const& lhs, Uuid const& rhs) noexcept { return !(lhs == rhs); }
/// @brief Comparison operator "less than or equal" for Uuid operands.
/// @brief Comparison operator "less than or equal" for @c Uuid operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @return @c true if a binary representation of @c lhs is less than or equal to @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10455}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02112}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02137
/// @trace_id_dd=DD_CRYPTO_04907
/// @needwork = ad
/// @endcode
inline constexpr bool operator<=(Uuid const& lhs, Uuid const& rhs) noexcept { return !(lhs > rhs); }
/// @brief Comparison operator "greater than or equal" for Uuid operands.
/// @brief Comparison operator "greater than or equal" for @c Uuid operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @return @c true if a binary representation of @c lhs is greater than or equal to @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10456}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02112}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02138
/// @trace_id_dd=DD_CRYPTO_04908
/// @needwork = ad
/// @endcode
inline constexpr bool operator>=(Uuid const& lhs, Uuid const& rhs) noexcept { return !(lhs < rhs); }
//********************************/
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_UUID_H_
