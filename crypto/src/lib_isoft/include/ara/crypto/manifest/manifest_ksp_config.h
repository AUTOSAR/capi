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
/// @file       manifest_ksp_config.h
/// @brief      AutoSar-Crypto configuration file: server side
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
/// @unit_name=PManifestKspConfig
/// @unit_description=Data structure after parsing the crypto_manifest.json file used by the encryption module
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_MANIFEST_KSP_CONFIG_H_
#define ARA_CRYPTO_MANIFEST_KSP_CONFIG_H_

#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>
#include <isoft/manifestreader/tps_enumeration.h>

#include <functional>

#include "ara/core/instance_specifier.h"
#include "ara/core/map.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"
#include "ara/crypto/keys/key_slot_content_props.h"
#include "ara/crypto/keys/key_slot_prototype_props.h"
#include "ara/crypto/manifest/manifest_crypto_data.h"
#include "ara/crypto/manifest/manifest_crypto_syntax.h"

namespace ara {
namespace crypto {
namespace manifest {
//********************************/
/// @brief Configuration file used by key manager (cryptod)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01570
/// @trace_id_dd=DD_CRYPTO_04006
/// @needwork = ad
/// @endcode
class PManifestKspConfig
{
public:
    /// @brief MAP list after reading the configuration file: CertToKeySlot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01570
    /// @trace_id_dd=DD_CRYPTO_06353
    /// @needwork = dd
    /// @endcode
    using MAP_CertToKeySlot = ara::core::Map< ara::core::String, PConfig_MappingCertToKeySlot >;
    /// @brief MAP list after reading the configuration file: CryptoKeySlot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01570
    /// @trace_id_dd=DD_CRYPTO_06354
    /// @needwork = dd
    /// @endcode
    using MAP_CryptoKeySlot = ara::core::Map< ara::core::String, PConfig_CryptoKeySlot >;
    /// @brief MAP list after reading the configuration file: CryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01570
    /// @trace_id_dd=DD_CRYPTO_06355
    /// @needwork = dd
    /// @endcode
    using MAP_CryptoProvider = ara::core::Map< ara::core::String, PConfig_CryptoProvider >;
    /// @brief MAP list after reading the configuration file: CryptoCertificate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01570
    /// @trace_id_dd=DD_CRYPTO_06356
    /// @needwork = dd
    /// @endcode
    using MAP_CryptoCertificate = ara::core::Map< ara::core::String, PConfig_CryptoCertificate >;
    /// @brief MAP list after reading the configuration file: ServiceCertDb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01570
    /// @trace_id_dd=DD_CRYPTO_06357
    /// @needwork = dd
    /// @endcode
    using MAP_ServiceCertDb = ara::core::Map< ara::core::String, PConfig_ServiceCertificate >;
    /// @brief VECTOR list after reading the configuration file: trusted container
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01570
    /// @trace_id_dd=DD_CRYPTO_06358
    /// @needwork = dd
    /// @endcode
    using VECTOR_CryptoTrustMasterProcess = ara::core::Vector< PConfig_CryptoTrustMasterProcess >;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04007
    /// @needwork = dda
    /// @endcode
    PManifestKspConfig() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04008
    /// @needwork = dda
    /// @endcode
    virtual ~PManifestKspConfig() noexcept = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04009
    /// @needwork = dda
    /// @endcode
    PManifestKspConfig(PManifestKspConfig const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04010
    /// @needwork = dda
    /// @endcode
    PManifestKspConfig(PManifestKspConfig &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04011
    /// @needwork = dda
    /// @endcode
    PManifestKspConfig &operator=(PManifestKspConfig const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04012
    /// @needwork = dda
    /// @endcode
    PManifestKspConfig &operator=(PManifestKspConfig &&other) = delete;
    /// @brief Initialize the configuration file
    /// @param stFileName configuration file
    /// @return true if has init manisfest sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04013
    /// @needwork = dda
    /// @endcode
    bool InitManifest(ara::core::StringView const &stFileName) noexcept;

public:
    //@brief Find the key slot by name (use identifier if '/' is found, otherwise Shortname)
    /// @param stSlotName key slot name
    /// @return PConfig_CryptoKeySlot instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04014
    /// @needwork = dda
    /// @endcode
    PConfig_CryptoKeySlot const *FindKeySlotByName(ara::core::StringView const &stSlotName) const noexcept;
    /// @brief Find the certificate slot by name (use identifier if '/' is found, otherwise Shortname)
    /// @param stCertName certificate name
    /// @return PConfig_CryptoKeySlot instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04015
    /// @needwork = dda
    /// @endcode
    PConfig_CryptoKeySlot const *FindCertSlotByName(ara::core::StringView const &stCertName) const noexcept;
    /// @brief Find Key Slot By Cert Slot Name
    /// @param stCertName certificate name
    /// @return certificate fqn
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04016
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::crypto::manifest::PConfig_InstanceKeySlot > FindKeySlotByCertSlotName(
        ara::core::StringView const &stCertName) const noexcept;
    /// @brief Find the certificate by name (use identifier if '/' is found, otherwise Shortname)
    /// @param stCertName certificate name
    /// @return pointer to PConfig_CryptoCertificate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04017
    /// @needwork = dda
    /// @endcode
    PConfig_CryptoCertificate const *FindCertByName(ara::core::StringView const &stCertName) const noexcept;
    //@brief Find the key slot by key slot identifier
    /// @param stSlotIns key slot identifier: string format
    /// @param stCryptoAlgID cryptographic primitive ID: string format
    /// @return KeySlotPrototypeProps instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04018
    /// @needwork = dda
    /// @endcode
    keys::KeySlotPrototypeProps::Uptr FindSlotPropsByName(ara::core::StringView const &stSlotIns,
                                                          ara::core::String &stCryptoAlgID) const noexcept;
    /// @brief Assemble key slot properties
    /// @param pFindSlot key slot object
    /// @return KeySlotPrototypeProps instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04019
    /// @needwork = dda
    /// @endcode
    static keys::KeySlotPrototypeProps::Uptr AssembleSlotProps(PConfig_CryptoKeySlot const *const pFindSlot) noexcept;
    /// @brief Get the crypto provider to which the key slot belongs by name
    /// @param stSlotName key slot name
    /// @return the crypto provider to which the key slot belongs
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04020
    /// @needwork = dda
    /// @endcode
    ara::core::String FindCryptoProviderBySlotName(ara::core::StringView const &stSlotName) const noexcept;
    /// @brief Determine whether a process is a TrustMaster by process name
    /// @param stProcessName process name
    /// @return true if trustmaster process false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04021
    /// @needwork = dda
    /// @endcode
    bool IsTrustMaster(ara::core::StringView const &stProcessName) const noexcept;
    /// @brief Find the corresponding slot name by certificate name
    /// @param stCertName certificate name
    /// @return key slot port name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04022
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > FindCertSlotNamesByName(
        ara::core::StringView const &stCertName) const noexcept;

private:
    /// @brief MAP list of key slots
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04023
    /// @needwork = dda
    /// @endcode
    MAP_CryptoKeySlot mapKeySlot_;
    /// @brief MAP list of crypto providers
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04024
    /// @needwork = dda
    /// @endcode
    MAP_CryptoProvider mapCryptoProvider_;
    /// @brief MAP list of certificates
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04025
    /// @needwork = dda
    /// @endcode
    MAP_CryptoCertificate mapCryptoCert_;
    /// @brief MAP list of server certificates
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04026
    /// @needwork = dda
    /// @endcode
    MAP_ServiceCertDb mapServiceCert_;
    /// @brief Mapping from Cert to KeySlot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04027
    /// @needwork = dda
    /// @endcode
    MAP_CertToKeySlot mapCertToKeySlot_;
    /// @brief Store trustmaster processes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04028
    /// @needwork = dda
    /// @endcode
    VECTOR_CryptoTrustMasterProcess vectorCryptoTrusterMasterProcess_;

private:
};
//********************************/
}  // namespace manifest
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_MANIFEST_KSP_CONFIG_H_
