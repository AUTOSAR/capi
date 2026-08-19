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
/// @file       crypto_object_uid.h
/// @brief      AutoSar-Crypto Encryption/Decryption common module
/// @details    Definition of the Crypto Object Unique Identifier (COUID) type.
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </td> <td>2021-12-29 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06001
/// @unit_name=UUID
/// @unit_description=Crypto Object Unique Identifier (COUID)
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYPTO_OBJECT_UID_H_
#define ARA_CRYPTO_CRYPTO_OBJECT_UID_H_

#include <cstdint>

#include "ara/crypto/common/uuid.h"

namespace ara {
namespace crypto {
//********************************/
/// @brief Definition of the Crypto Object Unique Identifier (COUID) type.
/// @brief Definition of Crypto Object Unique Identifier (@b COUID) type.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10100}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02005}
/// @uptrace={RS_CRYPTO_02006}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02109
/// @trace_id_dd=DD_CRYPTO_04877
/// @needwork = ad
/// @qac [2198]possibly cannot be modified: This struct explicitly defines member functions.
/// @endcode
// PRQA S 2198 QAC /// @qac: AUTOSAR standard interface
struct CryptoObjectUid final
// PRQA L:QAC
{
public:
    /// @brief UUID of the generator that created this COUID. This UUID can be associated with an HSM, physical host/ECU, or virtual machine.
    /// @brief UUID of a generator that has produced this COUID. This UUID can be associated with HSM, physical host/ECU
    /// or VM.
    /// @trace_id_sws={SWS_CRYPT_10101}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    Uuid mGeneratorUid{};
    /// @brief Sequence value of a stable timer or simple counter, indicating the version of the corresponding crypto object.
    /// @brief Sequential value of a steady timer or simple counter, representing version of correspondent Crypto
    /// Object.
    /// @trace_id_sws={SWS_CRYPT_10102}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    std::uint64_t mVersionStamp{0U};

public:
    /// @brief Check whether this identifier was generated before the identifier provided by the parameter.
    /// @brief Check whether this identifier was generated earlier than the one provided by the argument.
    /// @param anotherId another identifier for the comparison
    /// @return @c true if this identifier was generated earlier than the @c anotherId
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10112}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Reentrant}
    /// @endcode
    constexpr bool HasEarlierVersionThan(CryptoObjectUid const& anotherId) const noexcept;
    /// @brief Check whether this identifier was generated after the identifier provided by the parameter.
    /// @brief Check whether this identifier was generated later than the one provided by the argument.
    /// @param anotherId  another identifier for the comparison
    /// @return @c true if this identifier was generated later than the @c anotherId
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10113}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Reentrant}
    /// @endcode
    constexpr bool HasLaterVersionThan(CryptoObjectUid const& anotherId) const noexcept;
    /// @brief Check whether this identifier shares a common source with the identifier provided by the parameter.
    /// @brief Check whether this identifier has a common source with the one provided by the argument.
    /// @param anotherId  another identifier for the comparison
    /// @return @c true if both identifiers has common source (identical value of the @c mGeneratorUid field)
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10111}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Reentrant}
    /// @endcode
    constexpr bool HasSameSourceAs(CryptoObjectUid const& anotherId) const noexcept;
    /// @brief Check whether this identifier is "Nil".
    /// @brief Check whether this identifier is "Nil".
    /// @return @c true if this identifier is "Nil" and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10114}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Reentrant}
    /// @endcode
    bool IsNil() const noexcept;
    /// @brief Check whether the generator identifier of this object is "Nil".
    /// @brief Check whether this object's generator identifier is "Nil".
    /// @return  @c true if this identifier is "Nil" and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10115}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Reentrant}
    /// @endcode
    bool SourceIsNil() const noexcept;
};
//********************************/
/// @brief Check whether this identifier shares a common source with the identifier provided by the parameter.
/// @param anotherId Another crypto object unique identifier
/// @return true if HasSameSource and @c false otherwise
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02110
/// @trace_id_dd=DD_CRYPTO_04878
/// @needwork = ad
/// @endcode
inline constexpr bool CryptoObjectUid::HasSameSourceAs(CryptoObjectUid const& anotherId) const noexcept
{
    return mGeneratorUid == anotherId.mGeneratorUid;
}
/// @brief Check whether this identifier was generated before the identifier provided by the parameter.
/// @param anotherId Another crypto object unique identifier
/// @return true if HasEarlierVersion and @c false otherwise
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02111
/// @trace_id_dd=DD_CRYPTO_04879
/// @needwork = ad
/// @endcode
inline constexpr bool CryptoObjectUid::HasEarlierVersionThan(CryptoObjectUid const& anotherId) const noexcept
{
    return HasSameSourceAs(anotherId) && (mVersionStamp < anotherId.mVersionStamp);
}
/// @brief Check whether this identifier was generated after the identifier provided by the parameter.
/// @param anotherId Another crypto object unique identifier
/// @return true if HasLaterVersion and @c false otherwise
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02112
/// @trace_id_dd=DD_CRYPTO_04880
/// @needwork = ad
/// @endcode
inline constexpr bool CryptoObjectUid::HasLaterVersionThan(CryptoObjectUid const& anotherId) const noexcept
{
    return HasSameSourceAs(anotherId) && (mVersionStamp > anotherId.mVersionStamp);
}
/// @brief Check if empty
/// @return true if empty and @c false otherwise
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02113
/// @trace_id_dd=DD_CRYPTO_04881
/// @needwork = ad
/// @endcode
inline bool CryptoObjectUid::IsNil() const noexcept { return mGeneratorUid.IsNil() && (0U == mVersionStamp); }
/// @brief Check if empty
/// @return  true if empty and @c false otherwise
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02114
/// @trace_id_dd=DD_CRYPTO_04882
/// @needwork = ad
/// @endcode
inline bool CryptoObjectUid::SourceIsNil() const noexcept { return mGeneratorUid.IsNil() && (0U == mVersionStamp); }
//********************************/
/// @brief Comparison operator "equal" for CryptoObjectUid operands.
/// @brief Comparison operator "equal" for @c CryptoObjectUid operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @return @c true if all members' values of @c lhs is equal to @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10150}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02005}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02115
/// @trace_id_dd=DD_CRYPTO_04883
/// @needwork = ad
/// @endcode
inline constexpr bool operator==(CryptoObjectUid const& lhs, CryptoObjectUid const& rhs) noexcept
{
    return lhs.HasSameSourceAs(rhs) && (lhs.mVersionStamp == rhs.mVersionStamp);
}
/// @brief Comparison operator "less than" for CryptoObjectUid operands.
/// @brief Comparison operator "less than" for @c CryptoObjectUid operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @return @c true if a binary representation of @c lhs is less than @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10151}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02005}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02116
/// @trace_id_dd=DD_CRYPTO_04884
/// @needwork = ad
/// @qac [3293]possibly cannot be modified: This comparison predicate does not appear to provide a strict weak ordering.
/// @endcode
// PRQA S 3293 QAC /// @qac: AUTOSAR standard interface
inline constexpr bool operator<(CryptoObjectUid const& lhs, CryptoObjectUid const& rhs) noexcept
// PRQA L:QAC
{
    return lhs.HasEarlierVersionThan(rhs);
}
/// @brief Comparison operator "greater than" for CryptoObjectUid operands.
/// @brief Comparison operator "greater than" for @c CryptoObjectUid operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @return @c true if a binary representation of @c lhs is greater than @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10152}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02005}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02117
/// @trace_id_dd=DD_CRYPTO_04885
/// @needwork = ad
/// @qac [3293]possibly cannot be modified: This comparison predicate does not appear to provide a strict weak ordering.
/// @endcode
// PRQA S 3293 QAC /// @qac: AUTOSAR standard interface
inline constexpr bool operator>(CryptoObjectUid const& lhs, CryptoObjectUid const& rhs) noexcept
// PRQA L:QAC
{
    return lhs.HasLaterVersionThan(rhs);
}
/// @brief Comparison operator "not equal" for CryptoObjectUid operands.
/// @brief Comparison operator "not equal" for @c CryptoObjectUid operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @return @c true if at least one member of @c lhs has a value not equal to correspondent member of @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10153}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02005}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02118
/// @trace_id_dd=DD_CRYPTO_04886
/// @needwork = ad
/// @endcode
inline constexpr bool operator!=(CryptoObjectUid const& lhs, CryptoObjectUid const& rhs) noexcept
{
    return !(lhs == rhs);
}
/// @brief Comparison operator "less than or equal" for CryptoObjectUid operands.
/// @brief Comparison operator "less than or equal" for @c CryptoObjectUid operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @return @c true if a binary representation of @c lhs is less than or equal to @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10154}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02005}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02119
/// @trace_id_dd=DD_CRYPTO_04887
/// @needwork = ad
/// @endcode
inline constexpr bool operator<=(CryptoObjectUid const& lhs, CryptoObjectUid const& rhs) noexcept
{
    return lhs.HasSameSourceAs(rhs) && (lhs.mVersionStamp <= rhs.mVersionStamp);
}
/// @brief Comparison operator "greater than or equal" for CryptoObjectUid operands.
/// @brief Comparison operator "greater than or equal" for @c CryptoObjectUid operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @return @c true if a binary representation of @c lhs is greater than or equal to @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10155}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02005}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02120
/// @trace_id_dd=DD_CRYPTO_04888
/// @needwork = ad
/// @endcode
inline constexpr bool operator>=(CryptoObjectUid const& lhs, CryptoObjectUid const& rhs) noexcept
{
    return lhs.HasSameSourceAs(rhs) && (lhs.mVersionStamp >= rhs.mVersionStamp);
}
//********************************/
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYPTO_OBJECT_UID_H_
