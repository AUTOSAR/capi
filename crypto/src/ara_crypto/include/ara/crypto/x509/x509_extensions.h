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
/// @file       x509_extensions.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Components/X.509 Extension Data
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_03003
/// @unit_name=X509Extensions
/// @unit_description=Certificate Extension Information
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_X509_EXTENSIONS_H_
#define ARA_CRYPTO_X509_X509_EXTENSIONS_H_

#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"
#include "ara/crypto/x509/x509_object.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
//- @interface X509Extensions
/// @brief Interface for X.509 extensions.
/// @brief Interface of X.509 Extensions.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_40500}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02578
/// @trace_id_dd=DD_CRYPTO_05384
/// @needwork = ad
/// @endcode
class X509Extensions : public X509Object
{
public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02579
    /// @trace_id_dd=DD_CRYPTO_05385
    /// @needwork = ad
    /// @endcode
    ~X509Extensions() noexcept override;
    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02580
    /// @trace_id_dd=DD_CRYPTO_05386
    /// @needwork = ad
    /// @endcode
    X509Extensions& operator=(X509Extensions const& other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other Another instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02581
    /// @trace_id_dd=DD_CRYPTO_05387
    /// @needwork = ad
    /// @endcode
    X509Extensions& operator=(X509Extensions&& other) noexcept = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02582
    /// @trace_id_dd=DD_CRYPTO_05388
    /// @needwork = ad
    /// @endcode
    X509Extensions(X509Extensions const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02583
    /// @trace_id_dd=DD_CRYPTO_05389
    /// @needwork = ad
    /// @endcode
    X509Extensions(X509Extensions&& other) = delete;

public:
    /// @brief Unique smart pointer for the interface.    //Comment and declaration do not match
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40501}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03229
    /// @trace_id_dd=DD_CRYPTO_06441
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< X509Extensions >;

public:  // X509Extensions interface
    /// @brief Count the number of elements in the sequence.
    /// @brief Count number of elements in the sequence.
    /// @return number of elements in the sequence
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40511}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02584
    /// @trace_id_dd=DD_CRYPTO_05390
    /// @needwork = ad
    /// @endcode
    virtual std::size_t Count() const noexcept;

public:  // Serializable interface
    /// @brief Expose serialization itself.
    /// @brief Serialize itself publicly.
    /// @param formatId  the Crypto Provider specific identifier of the output format
    /// @returns a buffer with the serialized object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less
    /// than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    /// @trace_id_sws={SWS_CRYPT_10711}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02585
    /// @trace_id_dd=DD_CRYPTO_05391
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(FormatId formatId
                                                                             = kFormatDefault) const noexcept override;

public:
    /// @brief Constructor with parameters
    /// @param x509Provider Certificate provider
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02595
    /// @trace_id_dd=DD_CRYPTO_05405
    /// @needwork = ad
    /// @endcode
    explicit X509Extensions(X509Provider& x509Provider) noexcept;
    /// @brief Set X.509 extensions
    /// @param pExtStack X.509 extensions
    /// @return ture if update sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02596
    /// @trace_id_dd=DD_CRYPTO_05406
    /// @needwork = ad
    /// @endcode
    bool UpdateExtensions(X509_EXTENSIONS const* const pExtStack) noexcept;
    /// @brief Set X.509 extensions
    /// @param pX509 Standard format certificate
    /// @return ture if update sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02597
    /// @trace_id_dd=DD_CRYPTO_05407
    /// @needwork = ad
    /// @endcode
    bool UpdateExtensions(X509 const* const pX509) noexcept;
    /// @brief Add extension data by ID
    /// @param nID Extension item ID
    /// @param nCrit Criticality flag
    /// @param stData Extension data: string
    /// @return true if add sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02598
    /// @trace_id_dd=DD_CRYPTO_05408
    /// @needwork = ad
    /// @endcode
    bool AddExtension(int32_t const nID, bool const nCrit, ara::core::String const& stData) noexcept;
    /// @brief Delete extension information
    /// @param nID Extension item ID
    /// @return  true if del sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02599
    /// @trace_id_dd=DD_CRYPTO_05409
    /// @needwork = ad
    /// @endcode
    bool DelExtension(int32_t const nID) noexcept;
    /// @brief Find extension data by ID
    /// @param nID Extension item ID
    /// @return true if find sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02600
    /// @trace_id_dd=DD_CRYPTO_05410
    /// @needwork = ad
    /// @endcode
    bool FindExtension(int32_t const nID) noexcept;

private:
    /// @brief X509_EXTENSIONS pointer
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05413
    /// @needwork = dda
    /// @endcode
    X509_EXTENSIONS* pExtStack_{nullptr};
    /// @brief Construct extension information
    /// @return Standard x509 extension information
    X509_EXTENSION* _createExtension(int const nId, bool const critical, ara::core::String const& stData)
    {
        // Construct extension based on critical_ value
        if (critical) {
            return X509V3_EXT_conf_nid(nullptr, nullptr, nId, ("critical," + stData).c_str());
        }
        return X509V3_EXT_conf_nid(nullptr, nullptr, nId, stData.c_str());
    }
    /// @brief Delete all matching extensions
    /// @param nid Extension item identifier
    /// @return 0 failure >0 total number of deleted extension items
    int _deleteAllExtensionsByNid(int nid)
    {
        if (pExtStack_ == nullptr) {
            return 0;
        }

        int deletedCount = 0;

        // Reverse traversal (to avoid index changes after deletion)
        for (int i = sk_X509_EXTENSION_num(pExtStack_) - 1; i >= 0; i--) {
            X509_EXTENSION* ext = sk_X509_EXTENSION_value(pExtStack_, i);  // NOLINT
            if (ext == nullptr) {
                continue;
            }

            ASN1_OBJECT* obj = X509_EXTENSION_get_object(ext);
            int extNid       = OBJ_obj2nid(obj);

            if (extNid == nid) {
                X509_EXTENSION* deleted = sk_X509_EXTENSION_delete(pExtStack_, i);  // NOLINT
                if (deleted != nullptr) {
                    X509_EXTENSION_free(deleted);
                    deletedCount++;
                }
            }
        }

        return deletedCount;
    }
    /// @brief Find all extensions with the specified NID (return count)
    /// @param nid Extension identifier information
    /// @return Number of extension items
    void _findAllExtensionsByNid(int nid, ara::core::Vector< X509_EXTENSION* >& results)
    {
        if (pExtStack_ == nullptr) {
            return;
        }

        int extCount = sk_X509_EXTENSION_num(pExtStack_);

        // Traverse the stack to find matching extensions
        for (int i = 0; i < extCount; i++) {
            X509_EXTENSION* ext = sk_X509_EXTENSION_value(pExtStack_, i);  // NOLINT
            if (ext == nullptr) {
                continue;
            }

            ASN1_OBJECT* obj = X509_EXTENSION_get_object(ext);
            if (OBJ_obj2nid(obj) == nid) {
                results.push_back(ext);
            }
        }
    }
    /// @brief Clear the extension stack
    void _clearExtensions()
    {
        while (sk_X509_EXTENSION_num(pExtStack_) > 0) {
            X509_EXTENSION* ext = sk_X509_EXTENSION_pop(pExtStack_);  // NOLINT
            X509_EXTENSION_free(ext);
        }
    }
};
//********************************/

}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_X509_EXTENSIONS_H_
