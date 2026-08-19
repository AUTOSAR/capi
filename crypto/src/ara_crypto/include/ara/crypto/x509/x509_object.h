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
/// @file       x509_object.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details    Common interface for all objects created by the X.509 Provider.
/// @date       2022-05-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr> <td>2022-05-18 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Components/X.509 Certificate
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=X509Object
/// @unit_description=Common Interface for Certificate Objects
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_X509_OBJECT_H_
#define ARA_CRYPTO_X509_X509_OBJECT_H_

#include "ara/crypto/common/serializable.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
/// @brief X509 Certificate Provider
class X509Provider;
//- @interface X509Object
/// @brief Common interface for all objects created by the X.509 Provider.
/// @brief Common interface of all objects created by X.509 Provider.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_40900}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02652
/// @trace_id_dd=DD_CRYPTO_05486
/// @needwork = ad
/// @endcode
class X509Object : public Serializable
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02653
    /// @trace_id_dd=DD_CRYPTO_05487
    /// @needwork = ad
    /// @endcode
    X509Object() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02654
    /// @trace_id_dd=DD_CRYPTO_05488
    /// @needwork = ad
    /// @endcode
    ~X509Object() noexcept override = default;

public:
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another X509Object to this instance.
    /// @param other Another instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    X509Object& operator=(X509Object const& other) noexcept = delete;
    /// @brief Default move constructor
    /// @brief Move-assign another X509Object to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    X509Object& operator=(X509Object&& other) noexcept = delete;

protected:
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    X509Object(X509Object&& other) noexcept = default;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    X509Object(X509Object const& other) noexcept = default;

public:
    /// @brief Get the reference to the X.509 Provider for this object.
    /// @brief Get a reference to X.509 Provider of this object.
    /// @returns a reference to X.509 Provider instance that provides this object
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40911}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02401}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02659
    /// @trace_id_dd=DD_CRYPTO_05493
    /// @needwork = ad
    /// @endcode
    // PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
    inline virtual X509Provider& MyProvider() const noexcept { return x509Provider_; }
    // PRQA L:QAC

public:  // 2022-05-18 hanjingjing added content
    /// @brief Constructor with parameters
    /// @param x509Provider Certificate provider
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02660
    /// @trace_id_dd=DD_CRYPTO_05494
    /// @needwork = ad
    /// @endcode
    explicit X509Object(X509Provider& x509Provider) noexcept;

private:
    /// @brief Certificate provider
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05495
    /// @needwork = dda
    /// @endcode
    X509Provider& x509Provider_;
};
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_X509_OBJECT_H_
