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
/// @file       isoft_volatile_trusted_container.h
/// @brief      AutoSar-Crypto encryption and decryption common module
/// @details    Trusted container
/// @date       2022-06-07
/// @author     Zheng Chang
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PVolatileTrustedContainer
/// @unit_description=Smart buffer
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_VOLATILE_TRUSTED_CONTAINER_H_
#define ARA_CRYPTO_KEYS_PUHUA_VOLATILE_TRUSTED_CONTAINER_H_

#include "ara/crypto/common/isoft_io_interface_mem_trust.h"
#include "ara/crypto/common/volatile_trusted_container.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
//- @interface VolatileTrustedContainer
/// @brief The explicit interface of volatile Trusted Container, used to buffer CryptoAPI objects in RAM.
/// This class represents a "smart buffer" because it provides access to IOInterface, which can be used to query metadata of the buffer contents.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00004
/// @trace_id_dd=DD_CRYPTO_00172
/// @needwork = ad
/// @endcode
class PVolatileTrustedContainer : public VolatileTrustedContainer
{
public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00004
    /// @trace_id_dd=DD_CRYPTO_06237
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PVolatileTrustedContainer >;

public:
    /// @brief Parameterized constructor
    /// @param capacity capacity of the trusted list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00173
    /// @needwork = dda
    /// @endcode
    explicit PVolatileTrustedContainer(std::size_t const capacity) noexcept;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00174
    /// @needwork = dda
    /// @endcode
    ~PVolatileTrustedContainer() noexcept override;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00175
    /// @needwork = dda
    /// @endcode
    PVolatileTrustedContainer &operator=(PVolatileTrustedContainer const &other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00176
    /// @needwork = dda
    /// @endcode
    PVolatileTrustedContainer &operator=(PVolatileTrustedContainer &&other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00177
    /// @needwork = dda
    /// @endcode
    PVolatileTrustedContainer(PVolatileTrustedContainer &&other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00178
    /// @needwork = dda
    /// @endcode
    PVolatileTrustedContainer(PVolatileTrustedContainer const &other) noexcept = delete;

public:  // VolatileTrustedContainer interface
    /// @brief Retrieve the IOInterface used to import/export objects into/from this container.
    /// @name   GetIOInterface
    /// @returns IOInterface instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00179
    /// @needwork = dda
    /// @endcode
    IOInterface &GetIOInterface() const noexcept override;

private:
    /// @brief Capacity size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00180
    /// @needwork = dda
    /// @endcode
    std::size_t capacity_;
    /// @brief IOInterface pointer of type Mem_Trust
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00181
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem_Trust *ioInterface_;
};

}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_VOLATILE_TRUSTED_CONTAINER_H_