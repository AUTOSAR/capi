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
/// @file       volatile_trusted_container.h
/// @brief      AutoSar-Crypto Encryption/Decryption common module
/// @details    Memory-lifetime trusted container for buffering CryptoAPI objects in RAM.
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr> <td>2021-12-29 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PVolatileTrustedContainer
/// @unit_description=Smart Buffer Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_VOLATILE_TRUSTED_CONTAINER_H_
#define ARA_CRYPTO_VOLATILE_TRUSTED_CONTAINER_H_

#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/io_interface.h"

namespace ara {
namespace crypto {
//********************************/
//- @interface VolatileTrustedContainer
/// @brief This explicit interface of volatile Trusted Container for buffering CryptoAPI objects in RAM.
///         This class represents a "smart buffer" as it provides access to an IOInterface that can be used to query metadata of the buffer's content.
/// @brief This explicit interface of a volatile Trusted Container is used for buffering CryptoAPI objects in RAM.
///       This class represents a "smart buffer" in that it provides access to the IOInterface, which can be used for
///       querying meta-data of the buffer content.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10850}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02086
/// @trace_id_dd=DD_CRYPTO_04850
/// @needwork = ad
/// @endcode
class VolatileTrustedContainer
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10852}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02086
    /// @trace_id_dd=DD_CRYPTO_06391
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< VolatileTrustedContainer >;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02087
    /// @trace_id_dd=DD_CRYPTO_04851
    /// @needwork = ad
    /// @endcode
    VolatileTrustedContainer() = default;
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10851}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02088
    /// @trace_id_dd=DD_CRYPTO_04852
    /// @needwork = ad
    /// @endcode
    virtual ~VolatileTrustedContainer() noexcept = default;
    /// @brief Retrieve the IOInterface used for importing/exporting objects into/from this container.
    /// @brief Retrieve the IOInterface used for importing/exporting objects into this container
    /// @returns a reference to the IOInterface of this container
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10853}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02089
    /// @trace_id_dd=DD_CRYPTO_04853
    /// @needwork = ad
    /// @endcode
    virtual IOInterface& GetIOInterface() const noexcept = 0;

public:
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another VolatileTrustedContainer to this instance.
    /// @param other Another instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30206}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04854
    /// @needwork = dda
    /// @endcode
    VolatileTrustedContainer& operator=(VolatileTrustedContainer const& other) = delete;
    /// @brief Default move constructor
    /// @brief Move-assign another VolatileTrustedContainer to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30207}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04855
    /// @needwork = dda
    /// @endcode
    VolatileTrustedContainer& operator=(VolatileTrustedContainer&& other) = delete;

public:
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04856
    /// @needwork = dda
    /// @endcode
    VolatileTrustedContainer(VolatileTrustedContainer&& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04857
    /// @needwork = dda
    /// @endcode
    VolatileTrustedContainer(VolatileTrustedContainer const& other) = delete;

public:
};
//********************************/
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_VOLATILE_TRUSTED_CONTAINER_H_
