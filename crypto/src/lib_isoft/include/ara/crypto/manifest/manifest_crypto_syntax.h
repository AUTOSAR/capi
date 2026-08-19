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
/// @file       manifest_crypto_syntax.h
/// @brief      AutoSar-AP cryptographic module
/// @details    Manifest syntax format involved in ara/per
/// @date       2022-06-28
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
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_MANIFEST_CRYPTO_SYNTAX_H_
#define ARA_CRYPTO_MANIFEST_CRYPTO_SYNTAX_H_

#include "ara/crypto/common/isoft_data_type.h"

namespace ara {
namespace crypto {
namespace manifest {
//********************************/
/// @brief Get the configuration file tag string constant: configuration file name
/// @return ./etc/crypto_manifest.json
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01528
/// @trace_id_dd=DD_CRYPTO_03964
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCryptoManifestFileName() noexcept
{
    return "./etc/crypto_manifest.json";
}
//********************************/ //Key slot
/// @brief Get the configuration file tag string constant: keySlotInstance
/// @return keySlotInstance
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01529
/// @trace_id_dd=DD_CRYPTO_03965
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkKeySlotInstance() noexcept { return "keySlotInstance"; }
/// @brief Get the configuration file tag string constant: keySlot
/// @return keySlot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01530
/// @trace_id_dd=DD_CRYPTO_03966
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkKeySlotData() noexcept { return "keySlot"; }
//********************************/ //keySlotAllowedModification attributes
/// @brief Get the configuration file tag string constant: allowContentTypeChange
/// @return allowContentTypeChange
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01531
/// @trace_id_dd=DD_CRYPTO_03967
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkAllowContentTypeChange() noexcept { return "allowContentTypeChange"; }
/// @brief Get the configuration file tag string constant: exportability
/// @return exportability
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01532
/// @trace_id_dd=DD_CRYPTO_03968
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkExportability() noexcept { return "exportability"; }
/// @brief Get the configuration file tag string constant: maxNumberOfAllowedUpdates
/// @return maxNumberOfAllowedUpdates
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01533
/// @trace_id_dd=DD_CRYPTO_03969
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkMaxNumberOfAllowedUpdates() noexcept
{
    return "maxNumberOfAllowedUpdates";
}
/// @brief Get the configuration file tag string constant: restrictUpdate
/// @return restrictUpdate
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01534
/// @trace_id_dd=DD_CRYPTO_03970
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkRestrictUpdate() noexcept { return "restrictUpdate"; }
// keySlotContentAllowedUsage attributes
/// @brief Get the configuration file tag string constant: allowedKeyslotUsage
/// @return allowedKeyslotUsage
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01535
/// @trace_id_dd=DD_CRYPTO_03971
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkAllowedKeyslotUsage() noexcept { return "allowedKeyslotUsage"; }
//********************************/ // cryptoKeySlot attributes
/// @brief Get the configuration file tag string constant: shortName
/// @return shortName
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01536
/// @trace_id_dd=DD_CRYPTO_03972
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkSlotShortName() noexcept { return "shortName"; }
/// @brief Get the configuration file tag string constant: allocateShadowCopy
/// @return allocateShadowCopy
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01537
/// @trace_id_dd=DD_CRYPTO_03973
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkAllocateShadowCopy() noexcept { return "allocateShadowCopy"; }
/// @brief Get the configuration file tag string constant: cryptoAlgId
/// @return cryptoAlgId
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01538
/// @trace_id_dd=DD_CRYPTO_03974
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCryptoAlgId() noexcept { return "cryptoAlgId"; }
/// @brief Get the configuration file tag string constant: cryptoObjectType
/// @return cryptoObjectType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01539
/// @trace_id_dd=DD_CRYPTO_03975
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCryptoObjectType() noexcept { return "cryptoObjectType"; }
/// @brief Get the configuration file tag string constant: keySlotAllowedModification
/// @return keySlotAllowedModification
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01540
/// @trace_id_dd=DD_CRYPTO_03976
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkKeySlotAllowedModification() noexcept
{
    return "keySlotAllowedModification";
}
/// @brief Get the configuration file tag string constant: keySlotContentAllowedUsage
/// @return keySlotContentAllowedUsage
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01541
/// @trace_id_dd=DD_CRYPTO_03977
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkKeySlotContentAllowedUsage() noexcept
{
    return "keySlotContentAllowedUsage";
}
/// @brief Get the configuration file tag string constant: slotCapacity
/// @return slotCapacity
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01542
/// @trace_id_dd=DD_CRYPTO_03978
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkSlotCapacity() noexcept { return "slotCapacity"; }
/// @brief Get the configuration file tag string constant: slotType
/// @return slotType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01543
/// @trace_id_dd=DD_CRYPTO_03979
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkSlotType() noexcept { return "slotType"; }
//********************************/ //Crypto provider
/// @brief Get the configuration file tag string constant: appName
/// @return appName
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01544
/// @trace_id_dd=DD_CRYPTO_03980
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkAppName() noexcept { return "appName"; }
/// @brief Get the configuration file tag string constant: fileName     // Key slot storage file path
/// @return fileName
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01545
/// @trace_id_dd=DD_CRYPTO_03981
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkFileName() noexcept { return "fileName"; }
/// @brief Get the configuration file tag string constant: providerInstance
/// @return providerInstance
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01546
/// @trace_id_dd=DD_CRYPTO_03982
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkProviderInstance() noexcept { return "providerInstance"; }
/// @brief Get the configuration file tag string constant: keySlots
/// @return keySlots
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01547
/// @trace_id_dd=DD_CRYPTO_03983
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkKeySlotList() noexcept { return "keySlots"; }
//********************************/ //Certificate: client
/// @brief Get the configuration file tag string constant: certificateInstance
/// @return certificateInstance
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01548
/// @trace_id_dd=DD_CRYPTO_03984
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCertInstance() noexcept { return "certificateInstance"; }
/// @brief Get the configuration file tag string constant: cryptoCertificate
/// @return cryptoCertificate
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01549
/// @trace_id_dd=DD_CRYPTO_03985
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCertData() noexcept { return "cryptoCertificate"; }
//********************************/ // cryptoCertificate attributes
/// @brief Get the configuration file tag string constant: isPrivate
/// @return isPrivate
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01550
/// @trace_id_dd=DD_CRYPTO_03986
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCertIsPrivate() noexcept { return "isPrivate"; }
/// @brief Get the configuration file tag string constant: shortName
/// @return shortName
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01551
/// @trace_id_dd=DD_CRYPTO_03987
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCertShortName() noexcept { return "shortName"; }
//********************************/ //Certificate: server side
/// @brief Get the configuration file tag string constant: CryptoServiceCertificate
/// @return CryptoServiceCertificate
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01552
/// @trace_id_dd=DD_CRYPTO_03988
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkkServiceCertData() noexcept { return "CryptoServiceCertificate"; }
/// @brief Get the configuration file tag string constant: algorithmFamily
/// @return algorithmFamily
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01553
/// @trace_id_dd=DD_CRYPTO_03989
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkServiceAlgFamily() noexcept { return "algorithmFamily"; }
/// @brief Get the configuration file tag string constant: format
/// @return format
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01554
/// @trace_id_dd=DD_CRYPTO_03990
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkServiceCertFormat() noexcept { return "format"; }
/// @brief Get the configuration file tag string constant: maximumLength
/// @return maximumLength
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01555
/// @trace_id_dd=DD_CRYPTO_03991
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkServiceMaximumLength() noexcept { return "maximumLength"; }
/// @brief Get the configuration file tag string constant: nextHigherCertificate
/// @return nextHigherCertificate
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01556
/// @trace_id_dd=DD_CRYPTO_03992
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkServiceNextHigherCertificate() noexcept
{
    return "nextHigherCertificate";
}
//********************************/ //Port attributes
/// @brief Get the configuration file tag string constant: portInstance
/// @return portInstance
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01557
/// @trace_id_dd=DD_CRYPTO_03993
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkPortInstance() noexcept { return "portInstance"; }
/// @brief Get the configuration file tag string constant: portType
/// @return portType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01558
/// @trace_id_dd=DD_CRYPTO_03994
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkPortType() noexcept { return "portType"; }
/// @brief Get the configuration file tag string constant: writeAccess
/// @return writeAccess
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01559
/// @trace_id_dd=DD_CRYPTO_03995
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkWriteAccess() noexcept { return "writeAccess"; }
//********************************/ //4 types of mappings
/// @brief Get the configuration file tag string constant: CryptoKeySlots
/// @return CryptoKeySlots
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01560
/// @trace_id_dd=DD_CRYPTO_03996
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCryptoKeySlots() noexcept { return "CryptoKeySlots"; }
/// @brief Get the configuration file tag string constant: CryptoProviders
/// @return CryptoProviders
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01561
/// @trace_id_dd=DD_CRYPTO_03997
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCryptoProviders() noexcept { return "CryptoProviders"; }
/// @brief Get the configuration file tag string constant: CryptoCertificates
/// @return CryptoCertificates
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01562
/// @trace_id_dd=DD_CRYPTO_03998
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCryptoCerts() noexcept { return "CryptoCertificates"; }
/// @brief Get the configuration file tag string constant: CryptoServiceCertificates
/// @return CryptoServiceCertificates
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01563
/// @trace_id_dd=DD_CRYPTO_03999
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCryptoServiceCerts() noexcept { return "CryptoServiceCertificates"; }
/// @brief Get the configuration file tag string constant: CryptoCertificateToCryptoKeySlotMapping
/// @return CryptoCertificateToCryptoKeySlotMapping
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01564
/// @trace_id_dd=DD_CRYPTO_04000
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCertToKeySlot() noexcept
{
    return "CryptoCertificateToCryptoKeySlotMapping";
}
/// @brief Get the configuration file tag string constant: CryptoKeySlotToPortPrototypeMapping
/// @return CryptoKeySlotToPortPrototypeMapping
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01565
/// @trace_id_dd=DD_CRYPTO_04001
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkPortToKeySlot() noexcept
{
    return "CryptoKeySlotToPortPrototypeMapping";
}
/// @brief Get the configuration file tag string constant: CryptoProviderToPortPrototypeMapping
/// @return CryptoProviderToPortPrototypeMapping
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01566
/// @trace_id_dd=DD_CRYPTO_04002
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkPortToProvider() noexcept
{
    return "CryptoProviderToPortPrototypeMapping";
}
/// @brief Get the configuration file tag string constant: CryptoCertificateToPortPrototypeMapping
/// @return CryptoCertificateToPortPrototypeMapping
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01567
/// @trace_id_dd=DD_CRYPTO_04003
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkPortToCert() noexcept
{
    return "CryptoCertificateToPortPrototypeMapping";
}
//********************************/ // TrustMaster
/// @brief Get the configuration file tag string constant: CryptoTrustMasterProcesses
/// @return CryptoTrustMasterProcesses
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01568
/// @trace_id_dd=DD_CRYPTO_04004
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCryptoTrustMasterProcess() noexcept
{
    return "CryptoTrustMasterProcesses";
}
/// @brief Get the configuration file tag string constant: processName
/// @return processName
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01569
/// @trace_id_dd=DD_CRYPTO_04005
/// @trace_id_sr=SR_CRYPTO_06004
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkProcessName() noexcept { return "processName"; }
//********************************/

}  // namespace manifest
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_MANIFEST_CRYPTO_SYNTAX_H_
