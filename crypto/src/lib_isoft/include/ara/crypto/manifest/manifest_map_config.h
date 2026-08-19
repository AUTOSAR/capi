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
/// @file       manifest_map_config.h
/// @brief      AutoSar-Crypto configuration file: client side
/// @details    Data structure after parsing the crypto_manifest.json file used by the Puhua encryption module
/// @date       2022-06-28
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
/// @module_path=/CRYPTO/Reusable Functions/Parse Configuration File
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06004
/// @unit_name=PManifestMapConfig
/// @unit_description=Data structure after parsing Manifest configuration
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_MANIFEST_MAP_CONFIG_H_
#define ARA_CRYPTO_MANIFEST_MAP_CONFIG_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>
#include <isoft/manifestreader/tps_enumeration.h>

#include <functional>

#include "ara/crypto/keys/key_slot_content_props.h"
#include "ara/crypto/keys/key_slot_prototype_props.h"
#include "ara/crypto/manifest/manifest_crypto_data.h"
#include "ara/crypto/manifest/manifest_crypto_syntax.h"

namespace ara {
namespace crypto {
namespace manifest {

/// @brief Data structure after parsing Manifest configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01514
/// @trace_id_dd=DD_CRYPTO_03947
/// @needwork = ad
/// @endcode
class PManifestMapConfig
{
public:
    /// @brief Map list from Port to KeySlot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01514
    /// @trace_id_dd=DD_CRYPTO_06350
    /// @needwork = dd
    /// @endcode
    using MAP_PortToKeySlot = ara::core::Map< ara::core::String, PConfig_MappingPortToKeySlot >;
    /// @brief Map list from Port to Provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01514
    /// @trace_id_dd=DD_CRYPTO_06351
    /// @needwork = dd
    /// @endcode
    using MAP_PortToProvider = ara::core::Map< ara::core::String, PConfig_MappingPortToProvider >;
    /// @brief Map list from Port to Cert
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01514
    /// @trace_id_dd=DD_CRYPTO_06352
    /// @needwork = dd
    /// @endcode
    using MAP_PortToCert = ara::core::Map< ara::core::String, PConfig_MappingPortToCert >;

public:
    /// @brief the constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01515
    /// @trace_id_dd=DD_CRYPTO_03948
    /// @needwork = ad
    /// @endcode
    PManifestMapConfig() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01516
    /// @trace_id_dd=DD_CRYPTO_03949
    /// @needwork = ad
    /// @endcode
    virtual ~PManifestMapConfig() = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01517
    /// @trace_id_dd=DD_CRYPTO_03950
    /// @needwork = ad
    /// @endcode
    PManifestMapConfig(PManifestMapConfig const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01518
    /// @trace_id_dd=DD_CRYPTO_03951
    /// @needwork = ad
    /// @endcode
    PManifestMapConfig(PManifestMapConfig &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01519
    /// @trace_id_dd=DD_CRYPTO_03952
    /// @needwork = ad
    /// @endcode
    PManifestMapConfig &operator=(PManifestMapConfig const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01520
    /// @trace_id_dd=DD_CRYPTO_03953
    /// @needwork = ad
    /// @endcode
    PManifestMapConfig &operator=(PManifestMapConfig &&other) = delete;

    /// @brief Init Manifest
    /// @param stFileName configuration file
    /// @return true if has init manifest sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01521
    /// @trace_id_dd=DD_CRYPTO_03954
    /// @needwork = ad
    /// @endcode
    bool InitManifest(ara::core::StringView const &stFileName) noexcept;  // Initialize the configuration file

public:
    /// @brief Convert PORT identifier to key slot identifier
    /// @param stPort PORT identifier (use PORT identifier if '/' is found, otherwise KeySlot.Shortname)
    /// @return key slot fqn
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01522
    /// @trace_id_dd=DD_CRYPTO_03955
    /// @needwork = ad
    /// @endcode
    ara::core::String TransName_PortToSlot(ara::core::StringView const &stPort) const noexcept;
    /// @brief Convert PORT identifier to certificate identifier
    /// @param stPort PORT identifier (use PORT identifier if '/' is found, otherwise Cert.Shortname)
    /// @return certificate fqn
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01523
    /// @trace_id_dd=DD_CRYPTO_03956
    /// @needwork = ad
    /// @endcode
    ara::core::String TransName_PortToCert(ara::core::StringView const &stPort) const noexcept;
    /// @brief Convert PORT identifier to provider identifier
    /// @param stPort PORT identifier (use PORT identifier if '/' is found, otherwise Provider.Shortname)
    /// @return crypto provider fqn
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01524
    /// @trace_id_dd=DD_CRYPTO_03957
    /// @needwork = ad
    /// @endcode
    ara::core::String TransName_PortToProvider(ara::core::StringView const &stPort) const noexcept;
    /// @brief Check whether the certificate slot is writable
    /// @param stPort PORT identifier (use PORT identifier if '/' is found, otherwise Provider.Shortname)
    /// @return true if can write false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01525
    /// @trace_id_dd=DD_CRYPTO_03958
    /// @needwork = ad
    /// @endcode
    bool IsCertPortWriteAccess(ara::core::String const &stPort) const noexcept;

private:
    /// @brief Parse CryptoProviderToPortPrototypeMapping
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03959
    /// @needwork = dda
    /// @endcode
    MAP_PortToProvider mapPortToProvider_;
    /// @brief Parse CryptoKeySlotToPortPrototypeMapping
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03960
    /// @needwork = dda
    /// @endcode
    MAP_PortToKeySlot mapPortToKeySlot_;
    /// @brief Parse CryptoCertificateToPortPrototypeMapping
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03961
    /// @needwork = dda
    /// @endcode
    MAP_PortToCert mapPortToCert_;

private:
};

}  // namespace manifest

}  // namespace crypto

}  // namespace ara

#endif  // ARA_CRYPTO_MANIFEST_MAP_CONFIG_H_
