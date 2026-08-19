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
/// @file       serializable.h
/// @brief      AutoSar-Crypto Encryption/Decryption common module
/// @details    Interface for serializable objects.
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Signature Storage
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Serializable
/// @unit_description=Interface for serializable objects
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_SERIALIZABLE_H_
#define ARA_CRYPTO_SERIALIZABLE_H_

#include "ara/core/result.h"
#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/common/security_error_domain.h"
#include "openssl/bio.h"

namespace ara {
namespace crypto {
//********************************/
//- @interface Serializable
/// @brief Serializable object interface.
/// @brief Interface for serializable objects.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10700}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02105}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02079
/// @trace_id_dd=DD_CRYPTO_04833
/// @needwork = ad
/// @endcode
class Serializable
{
public:
    /// @brief Container type for encoding format identifiers.
    /// @brief A container type for the encoding format identifiers.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10701}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @uptrace={RS_CRYPTO_02302}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03184
    /// @trace_id_dd=DD_CRYPTO_06389
    /// @needwork = ad
    /// @endcode
    using FormatId = std::uint32_t;
    /// @brief Container type for encoding format identifiers.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03184
    /// @trace_id_dd=DD_CRYPTO_06390
    /// @needwork = dd
    /// @endcode
    using FunctionId = std::uint32_t;

public:
    /// @brief Default serialization format.
    /// @brief Default serialization format.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10750}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @uptrace={RS_CRYPTO_02302}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02079
    /// @trace_id_dd=DD_CRYPTO_04834
    /// @needwork = dd
    /// @endcode
    static FormatId const kFormatDefault = 0U;
    /// @brief Export only the raw values of the object.
    /// @brief Export only raw value of an object.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10751}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @uptrace={RS_CRYPTO_02302}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02079
    /// @trace_id_dd=DD_CRYPTO_04835
    /// @needwork = dd
    /// @endcode
    static FormatId const kFormatRawValueOnly = 1U;
    /// @brief Export the DER encoded value of the object.
    /// @brief Export DER-encoded value of an object.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10752}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @uptrace={RS_CRYPTO_02302}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02079
    /// @trace_id_dd=DD_CRYPTO_04836
    /// @needwork = dd
    /// @endcode
    static FormatId const kFormatDerEncoded = 2U;
    /// @brief Export the PEM encoded value of the object.
    /// @brief Export PEM-encoded value of an object.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10753}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @uptrace={RS_CRYPTO_02302}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02079
    /// @trace_id_dd=DD_CRYPTO_04837
    /// @needwork = dd
    /// @endcode
    static FormatId const kFormatPemEncoded = 3U;
    /// @brief kFuncDer
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02079
    /// @trace_id_dd=DD_CRYPTO_04838
    /// @needwork = dd
    /// @endcode
    static FunctionId const kFuncDer = 0U;
    /// @brief kFuncPem
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02079
    /// @trace_id_dd=DD_CRYPTO_04839
    /// @needwork = dd
    /// @endcode
    static FunctionId const kFuncPem = 1U;

public:
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10710}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @uptrace={RS_CRYPTO_02302}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02080
    /// @trace_id_dd=DD_CRYPTO_04840
    /// @needwork = ad
    /// @endcode
    virtual ~Serializable() noexcept = default;
    /// @brief Expose serialization itself.
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less
    /// than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object type
    /// @trace_id_sws={SWS_CRYPT_10711}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02084
    /// @trace_id_dd=DD_CRYPTO_04844
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @brief Serialize itself publicly.
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @returns a buffer with the serialized object
    ExportPublicly(FormatId formatId = kFormatDefault) const noexcept = 0;

protected:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork =dda
    /// @endcode
    Serializable() = default;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork =dda
    /// @endcode
    Serializable(Serializable const &other) noexcept = default;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork =dda
    /// @endcode
    Serializable(Serializable &&other) = default;

public:
    /// @brief Expose serialization itself. This method sets the size of the output container based on the actually saved values!
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the output buffer is less than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    /// @trace_id_sws={SWS_CRYPT_10712}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02085
    /// @trace_id_dd=DD_CRYPTO_04845
    /// @needwork = ad
    /// @endcode
    template < typename Alloc = VENDOR_IMPLEMENTATION_DEFINED >
    ara::core::Result< ByteVector< Alloc > >
    /// @brief Serialize itself publicly.
    ///     This method sets the size of the output container according to actually saved value!
    /// @tparam Alloc custom allocator type of the output container
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @returns  pre-reserved managed container for the serialization output
    // PRQA S 2135 QAC /// @qac: AUTOSAR standard interface
    ExportPublicly(FormatId formatId = kFormatDefault) const noexcept
    // PRQA L:QAC
    {
        auto result = ExportPublicly(formatId);
        if (false == result.HasValue()) {
            return ara::core::Result< ByteVector< Alloc > >::FromError(result.Error());
        }

        ByteVector< Alloc > output;
        try {
            output.resize(result.Value().size());
        } catch (std::bad_alloc &) {
            return ara::core::Result< ByteVector< Alloc > >::FromError(
                SecurityErrorDomain::Errc::kInsufficientCapacity);
        }

        memcpy(core::data(output), result.Value().data(), result.Value().size());
        return ara::core::Result< ByteVector< Alloc > >::FromValue(output);
    }

protected:
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another Serializable to this instance.
    /// @param other Another instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30204}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04846
    /// @needwork = dda
    /// @endcode
    Serializable &operator=(Serializable const &other) noexcept = default;
    /// @brief Default move assignment operator
    /// @brief Move-assign another Serializable to this instance.
    /// @param other Another instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30205}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04847
    /// @needwork = dda
    /// @endcode
    Serializable &operator=(Serializable &&other) noexcept = default;

public:
    /// @brief Export function
    /// @param format Export format
    /// @param pFun Callback function: executes the specific logic
    /// @return Data content in the corresponding format
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04848
    /// @needwork = dda
    /// @endcode
    static ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublic_Fun(
        FormatId const format, std::function< int32_t(BIO *, FunctionId const) > const &pFun) noexcept;
    /// @brief Check if the FormatID is supported
    /// @param nFormatID Certificate encoding format
    /// @param bCheckForWrite Whether to check the format ID for write support
    /// @return true if is valid formatId false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04849
    /// @needwork = dda
    /// @endcode
    static bool IsValidFormatID(FormatId const nFormatID, bool const bCheckForWrite) noexcept;
};
//********************************/
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_SERIALIZABLE_H_
