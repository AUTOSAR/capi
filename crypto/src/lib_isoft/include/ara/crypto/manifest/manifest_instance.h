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
/// @file       manifest_instance.h
/// @brief      AutoSar-Crypto configuration
/// @details
/// @date       2021-09-13
/// @author     hanjingjing
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
/// @module_path=/CRYPTO/Reusable Functions/Parse Configuration File
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06004
/// @unit_name=PManifestKspConfig
/// @unit_description=Singleton class of configuration file parser
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_MANIFEST_CRYPTO_INSTANCE_H_
#define ARA_CRYPTO_MANIFEST_CRYPTO_INSTANCE_H_

#include <memory>

#include "ara/crypto/manifest/manifest_map_config.h"

namespace ara {
namespace crypto {
namespace manifest {
//********************************/
/// @brief Json configuration parser
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01571
/// @trace_id_dd=DD_CRYPTO_04029
/// @needwork = ad
/// @endcode
class PManifestInstance
{
public:
    /// @brief Get the Instance pointer
    /// @return Instance pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04030
    /// @needwork = dda
    /// @endcode
    static PManifestMapConfig *Get() noexcept;
    /// @brief Deinitialization operation
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04031
    /// @needwork = dda
    /// @endcode
    static void Deinitialize() noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04032
    /// @needwork = dda
    /// @endcode
    virtual ~PManifestInstance() = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04033
    /// @needwork = dda
    /// @endcode
    PManifestInstance(PManifestInstance const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04034
    /// @needwork = dda
    /// @endcode
    PManifestInstance(PManifestInstance &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04035
    /// @needwork = dda
    /// @endcode
    PManifestInstance &operator=(PManifestInstance const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04036
    /// @needwork = dda
    /// @endcode
    PManifestInstance &operator=(PManifestInstance &&other) = delete;
};
//********************************/
}  // namespace manifest
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_MANIFEST_CRYPTO_INSTANCE_H_
