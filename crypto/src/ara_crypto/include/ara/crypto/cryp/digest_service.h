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
/// @file       digest_service.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Extended metadata service for generating context digests.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </td> <td>2021-12-21 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Decryption/Service Module
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=DigestService
/// @unit_description=Extended metadata service for generating context digests base class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_DIGEST_SERVICE_H_
#define ARA_CRYPTO_CRYP_DIGEST_SERVICE_H_

#include "ara/core/result.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/block_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Extended metadata service for generating context digests.
/// @brief Extension meta-information service for digest producing contexts.
/// @interface DigestService
/// @AUTOSAR_SWS {SWS_CRYPT_29010}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02167
/// @trace_id_dd=DD_CRYPTO_04938
/// @needwork = ad
/// @endcode
class DigestService : public BlockService
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29011}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03189
    /// @trace_id_dd=DD_CRYPTO_06400
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< DigestService >;

public:
    /// @brief Compare the calculated digest with the expected value.
    ///         The entire digest value is kept in the context until the next call to Start(), so any part of it can be verified or extracted again.
    ///         if(full_digest_size <= offset) || (expected.size() == 0) then return false; Else comparison_size =
    ///         min(expected.size(), (full_digest_size - offset)) bytes.
    ///         This method can be implemented "inline" after the ara::core::memcmp() function is standardized.
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished
    ///                by a call of the @c Finish() method
    /// @error: SecurityErrorDomain::kBruteForceRisk  if the buffered digest belongs to a MAC/HMAC/AE/AEAD
    ///                context, which was initialized by a key without @c kAllowSignature permission, but actual
    ///                size of requested digest is less than 8 bytes (it is a protection from the brute-force attack)
    /// @trace_id_sws={SWS_CRYPT_29013}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02168
    /// @trace_id_dd=DD_CRYPTO_04939
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool >
    /// @brief Compare the calculated digest against an expected value.
    ///       Entire digest value is kept in the context up to next call @c Start(), therefore any its part can be
    ///       verified again or extracted.
    ///       If <tt>(full_digest_size <= offset) || (expected.size() == 0)</tt> then return @c false;
    ///       else <tt>comparison_size = min(expected.size(), (full_digest_size - offset))</tt> bytes.
    ///       This method can be implemented as "inline" after standartization of function @c ara::core::memcmp().
    /// @param expected  the memory region containing an expected digest value
    /// @param offset  position of the first byte in calculated digest for the comparison starting
    /// @return @c true if the expected bytes sequence is identical to first bytes of calculated digest
    Compare(ReadOnlyMemRegion const &expected, std::size_t offset = 0U) const noexcept = 0;
    /// @brief Get the output digest size.
    /// @brief Get the output digest size.
    /// @return size of the full output from this digest-function in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29012}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02169
    /// @trace_id_dd=DD_CRYPTO_04940
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetDigestSize() const noexcept = 0;
    /// @brief Check the current status of stream processing: whether it is complete.
    /// @brief Check current status of the stream processing: finished or no.
    /// @return @c true if a previously started stream processing was finished by a call of the @c Finish() or @c
    /// FinishBytes() methods
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29015}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02170
    /// @trace_id_dd=DD_CRYPTO_04941
    /// @needwork = ad
    /// @endcode
    virtual bool IsFinished() const noexcept = 0;
    /// @brief Check the current status of stream processing: whether it has been started.
    /// @brief Check current status of the stream processing: started or no.
    /// @return @c true if the processing was start by a call of the @c Start() methods and was not finished yet
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29014}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02171
    /// @trace_id_dd=DD_CRYPTO_04942
    /// @needwork = ad
    /// @endcode
    virtual bool IsStarted() const noexcept = 0;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02172
    /// @trace_id_dd=DD_CRYPTO_04943
    /// @needwork = ad
    /// @endcode
    DigestService() = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02173
    /// @trace_id_dd=DD_CRYPTO_04944
    /// @needwork = ad
    /// @endcode
    ~DigestService() override = default;
    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @returns *this
    /// @throws  Function execution exception
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02174
    /// @trace_id_dd=DD_CRYPTO_04945
    /// @needwork = ad
    /// @endcode
    DigestService &operator=(DigestService const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another instance of this class
    /// @returns *this
    /// @throws  Function execution exception
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02175
    /// @trace_id_dd=DD_CRYPTO_04946
    /// @needwork = ad
    /// @endcode
    DigestService &operator=(DigestService &&other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02176
    /// @trace_id_dd=DD_CRYPTO_04947
    /// @needwork = ad
    /// @endcode
    DigestService(DigestService &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02177
    /// @trace_id_dd=DD_CRYPTO_04948
    /// @needwork = ad
    /// @endcode
    DigestService(DigestService const &other) noexcept = delete;
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_DIGEST_SERVICE_H_
