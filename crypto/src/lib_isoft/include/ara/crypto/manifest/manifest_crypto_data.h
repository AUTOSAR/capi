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
/// @file       manifest_crypto_data.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Data structures after parsing crypto_manifest.json file used by Puhua encryption module
/// @date       2022-06-28
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-06-28  <td>1.0.0    <td>Chang Zheng  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Parse Configuration Files
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06004
/// @unit_name=PManifestKspConfig
/// @unit_description=Data structures after parsing crypto_manifest.json file used by encryption module
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_MANIFEST_CYPTO_DATA_H_
#define ARA_CRYPTO_MANIFEST_CYPTO_DATA_H_
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"

namespace ara {
namespace crypto {
namespace manifest {
//********************************/ // Key Slot
/// @brief Json config: Key slot allow modification properties
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01498
/// @trace_id_dd=DD_CRYPTO_03884
/// @needwork = ad
/// @endcode
struct PConfig_KeySlotAllowedModification
{
public:
    /// @brief Whether context type modification is allowed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03885
    /// @needwork = dda
    /// @endcode
    bool allowContentTypeChange;
    /// @brief Whether export is allowed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03886
    /// @needwork = dda
    /// @endcode
    bool exportability;
    /// @brief Maximum modification count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03887
    /// @needwork = dda
    /// @endcode
    std::uint32_t maxNumberOfAllowedUpdates;
    /// @brief Restrict updates
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03888
    /// @needwork = dda
    /// @endcode
    bool restrictUpdate;
};
/// @brief Json config: Key slot usage scope
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01499
/// @trace_id_dd=DD_CRYPTO_03889
/// @needwork = ad
/// @endcode
struct PConfig_KeySlotContentAllowedUsage
{
public:
    /// @brief Key slot usage scope
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03890
    /// @needwork = dda
    /// @endcode
    ara::core::String allowedKeyslotUsage;
};
/// @brief Json config: Key slot data
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01500
/// @trace_id_dd=DD_CRYPTO_03891
/// @needwork = ad
/// @endcode
struct PConfig_KeySlotData
{
public:
    /// @brief Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03892
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName;
    /// @brief Whether shadow copy allocation is allowed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03893
    /// @needwork = dda
    /// @endcode
    bool allocateShadowCopy;
    /// @brief Crypto primitive ID of supported encryption algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03894
    /// @needwork = dda
    /// @endcode
    ara::core::String cryptoAlgId;
    /// @brief Type of key material
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03895
    /// @needwork = dda
    /// @endcode
    ara::core::String cryptoObjectType;
    /// @brief Allow modification item configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03896
    /// @needwork = dda
    /// @endcode
    PConfig_KeySlotAllowedModification keySlotAllowedModification;
    /// @brief Usage scope
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03897
    /// @needwork = dda
    /// @endcode
    PConfig_KeySlotContentAllowedUsage keySlotContentAllowedUsage;
    /// @brief Upper limit of allocated storage capacity
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03898
    /// @needwork = dda
    /// @endcode
    std::uint32_t slotCapacity;
    /// @brief Key slot type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03899
    /// @needwork = dda
    /// @endcode
    ara::core::String slotType;
    /// @brief Only valid when slotType == "APPLICATION"
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03900
    /// @needwork = dda
    /// @endcode
    ara::core::String stAppName;
    /// @brief Path to file where slot information is saved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03901
    /// @needwork = dda
    /// @endcode
    ara::core::String stFileName;
};
//***************/
/// @brief Json config: Key slot information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01501
/// @trace_id_dd=DD_CRYPTO_03902
/// @needwork = ad
/// @endcode
struct PConfig_CryptoKeySlot
{
public:
    /// @brief Key slot identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03903
    /// @needwork = dda
    /// @endcode
    ara::core::String stKeySlotInstance;
    /// @brief Key slot data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03904
    /// @needwork = dda
    /// @endcode
    PConfig_KeySlotData keySlotData;
};
//********************************/
/// @brief Json config: Key slot instance
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01502
/// @trace_id_dd=DD_CRYPTO_03905
/// @needwork = ad
/// @endcode
struct PConfig_InstanceKeySlot
{
public:
    /// @brief Key slot identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03906
    /// @needwork = dda
    /// @endcode
    ara::core::String stKeySlotInstance;
};
/// @brief Json config: Crypto provider
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01503
/// @trace_id_dd=DD_CRYPTO_03907
/// @needwork = ad
/// @endcode
struct PConfig_CryptoProvider
{
public:
    /// @brief Crypto provider name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03908
    /// @needwork = dda
    /// @endcode
    ara::core::String stProviderInstance;
    /// @brief List of key slots managed by the crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03909
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PConfig_InstanceKeySlot > vecKeySlot;
};
//********************************/
/// @brief Json config: Certificate information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01504
/// @trace_id_dd=DD_CRYPTO_03912
/// @needwork = ad
/// @endcode
struct PConfig_CertificateData
{
public:
    /// @brief Certificate name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03913
    /// @needwork = dda
    /// @endcode
    ara::core::String stShortName{};
    /// @brief Whether private
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03914
    /// @needwork = dda
    /// @endcode
    bool isPrivate{false};
};
/// @brief Json config: Certificate configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01505
/// @trace_id_dd=DD_CRYPTO_03915
/// @needwork = ad
/// @endcode
struct PConfig_CryptoCertificate
{
public:
    /// @brief Certificate identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03916
    /// @needwork = dda
    /// @endcode
    ara::core::String stCertInstance;
    /// @brief Certificate information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03917
    /// @needwork = dda
    /// @endcode
    PConfig_CertificateData certData;
};
//********************************/  // Service Certificate
/// @brief Json config: Service-side certificate information: Determined by configuration within the specific module/App using Crypto on 2023-01-09
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01506
/// @trace_id_dd=DD_CRYPTO_03918
/// @needwork = ad
/// @endcode
struct PConfig_ServiceCertificateData
{
public:
    /// @brief Encryption algorithm family
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03919
    /// @needwork = dda
    /// @endcode
    ara::core::String stAlgorithmFamily;
    /// @brief Format
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03920
    /// @needwork = dda
    /// @endcode
    ara::core::String stFormat;
    /// @brief Maximum length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03921
    /// @needwork = dda
    /// @endcode
    uint16_t nMaximumLength{0};
    /// @brief Parent certificate identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03922
    /// @needwork = dda
    /// @endcode
    ara::core::String stNextHigherCertificate;
};
/// @brief Json configuration: certificate configuration on the Service side
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01507
/// @trace_id_dd=DD_CRYPTO_03923
/// @needwork = ad
/// @endcode
struct PConfig_ServiceCertificate
{
public:
    /// @brief Certificate identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03924
    /// @needwork = dda
    /// @endcode
    ara::core::String stCertInstance;
    /// @brief Certificate data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03925
    /// @needwork = dda
    /// @endcode
    PConfig_ServiceCertificateData certData;
};
/// @brief Json configuration: trusted environment
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01508
/// @trace_id_dd=DD_CRYPTO_03926
/// @needwork = ad
/// @endcode
struct PConfig_CryptoTrustMasterProcess
{
public:
    /// @brief Trusted process name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03927
    /// @needwork = dda
    /// @endcode
    ara::core::String stProcessName;
};
//********************************/ //4 types of mappings
/// @brief Json configuration: PORT mapping information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01509
/// @trace_id_dd=DD_CRYPTO_03928
/// @needwork = ad
/// @endcode
struct PConfig_PortPrototypeMapping
{
public:
    /// @brief PORT identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03929
    /// @needwork = dda
    /// @endcode
    ara::core::String stPortInstance;
    /// @brief PORT type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03930
    /// @needwork = dda
    /// @endcode
    ara::core::String stPortType;
};
//***************/
/// @brief Json configuration: PORT to key slot mapping
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01510
/// @trace_id_dd=DD_CRYPTO_03931
/// @needwork = ad
/// @endcode
struct PConfig_MappingPortToKeySlot
{
public:
    /// @brief PORT identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03932
    /// @needwork = dda
    /// @endcode
    ara::core::String stPortInstance;
    /// @brief PORT type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03933
    /// @needwork = dda
    /// @endcode
    ara::core::String stPortType;
    /// @brief Key slot identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03934
    /// @needwork = dda
    /// @endcode
    ara::core::String stSlotInstance;
};
//***************/
/// @brief Json configuration: PORT to crypto provider mapping
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01511
/// @trace_id_dd=DD_CRYPTO_03935
/// @needwork = ad
/// @endcode
struct PConfig_MappingPortToProvider
{
public:
    /// @brief PORT identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03936
    /// @needwork = dda
    /// @endcode
    ara::core::String stPortInstance;
    /// @brief PORT type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03937
    /// @needwork = dda
    /// @endcode
    ara::core::String stPortType;
    /// @brief Crypto provider identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03938
    /// @needwork = dda
    /// @endcode
    ara::core::String stProviderInstance;
};
//***************/
/// @brief Json configuration: PORT to certificate mapping
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01512
/// @trace_id_dd=DD_CRYPTO_03939
/// @needwork = ad
/// @endcode
struct PConfig_MappingPortToCert
{
public:
    /// @brief PORT identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03940
    /// @needwork = dda
    /// @endcode
    ara::core::String stPortInstance;
    /// @brief PORT type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03941
    /// @needwork = dda
    /// @endcode
    ara::core::String stPortType;
    /// @brief Certificate identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03942
    /// @needwork = dda
    /// @endcode
    ara::core::String stCertInstance;
    /// @brief Whether writable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03943
    /// @needwork = dda
    /// @endcode
    bool writeaccess;
};
//***************/
/// @brief Json configuration: certificate to key slot mapping
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01513
/// @trace_id_dd=DD_CRYPTO_03944
/// @needwork = ad
/// @endcode
struct PConfig_MappingCertToKeySlot
{
public:
    /// @brief Certificate identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03945
    /// @needwork = dda
    /// @endcode
    ara::core::String stCertInstance;
    /// @brief Key slot list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03946
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PConfig_InstanceKeySlot > vecKeySlot;
};
//********************************/
}  // namespace manifest
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_MANIFEST_CYPTO_DATA_H_
