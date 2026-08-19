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
/// @file       manifest_map_config.cpp
/// @brief      AutoSar-Crypto Configuration File: Client Side
/// @details
/// @date       2022-06-28
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Interpret Configuration File
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06004
/// @unit_name=PManifestMapConfig
/// @unit_description=Data structure interpreted from Manifest configuration
/// @endcode
///
/// ================================================================

#include "ara/crypto/manifest/manifest_map_config.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include <functional>

#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/manifest/manifest_crypto_common.h"

//********************************/
namespace manifest = ara::crypto::manifest;
//********************************/ //PORT Mapping
// PRQA S 2024,2025 QAC /// @qac: Cannot be modified within macro definition
MR_HEADER(manifest::PConfig_PortPrototypeMapping)
MR_FIELD(manifest::GetkPortInstance(), stPortInstance)
MR_FIELD(manifest::GetkPortType(), stPortType)
MR_FOOTER
// manifest::kPortToKeySlot
MR_HEADER(manifest::PConfig_MappingPortToKeySlot)
MR_FIELD(manifest::GetkPortInstance(), stPortInstance)
MR_FIELD(manifest::GetkPortType(), stPortType)
MR_FIELD(manifest::GetkKeySlotInstance(), stSlotInstance)
MR_FOOTER
// manifest::kPortToProvider
MR_HEADER(manifest::PConfig_MappingPortToProvider)
MR_FIELD(manifest::GetkPortInstance(), stPortInstance)
MR_FIELD(manifest::GetkPortType(), stPortType)
MR_FIELD(manifest::GetkProviderInstance(), stProviderInstance)
MR_FOOTER
// manifest::kPortToCert
MR_HEADER(manifest::PConfig_MappingPortToCert)
MR_FIELD(manifest::GetkPortInstance(), stPortInstance)
MR_FIELD(manifest::GetkPortType(), stPortType)
MR_FIELD(manifest::GetkCertInstance(), stCertInstance)
MR_FIELD(manifest::GetkWriteAccess(), writeaccess)
MR_FOOTER
// PRQA L:QAC
//********************************/
namespace ara {
namespace crypto {
namespace manifest {
//********************************/
/// @brief Initialize configuration file
/// @param stFileName Configuration file name
/// @return true if init sucess false otherwise
bool PManifestMapConfig::InitManifest(ara::core::StringView const &stFileName) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > const manifestRes{
        isoft::manifestreader::OpenManifest(stFileName)};

    if (!manifestRes.HasValue()) {
        return false;
    }
    isoft::manifestreader::Manifest *const pManifestRes{manifestRes.Value().get()};
    ara::crypto::isoft_def::LogInfo() << "PManifestMapConfig::LoadData ConfigName = " << stFileName.data();

    std::ignore = ReadMapData< PConfig_MappingPortToProvider >(
        pManifestRes, ara::core::String(manifest::GetkPortToProvider()),
        [this](PConfig_MappingPortToProvider const &data) -> void { mapPortToProvider_[data.stPortInstance] = data; });
    std::ignore = ReadMapData< PConfig_MappingPortToKeySlot >(
        pManifestRes, ara::core::String(manifest::GetkPortToKeySlot()),
        [this](PConfig_MappingPortToKeySlot const &data) -> void { mapPortToKeySlot_[data.stPortInstance] = data; });
    std::ignore = ReadMapData< PConfig_MappingPortToCert >(
        pManifestRes, ara::core::String(manifest::GetkPortToCert()),
        [this](PConfig_MappingPortToCert const &data) -> void { mapPortToCert_[data.stPortInstance] = data; });
    return true;
}
//********************************/
namespace {
/// @brief Convert PORT name to Crypto name
/// @tparam T_MAP
/// @param mapData MAP list holding conversion data
/// @param stPort PORT identifier (use Port identifier if '/' is found, otherwise Provider.Shortname)
/// @param pFunc Callback function
/// @return
/// @throws
template < typename T_MAP >
ara::core::String T_TransName_PortToCrypto(
    T_MAP &mapData,
    ara::core::StringView const &stPort,
    std::function< ara::core::String(typename T_MAP::const_iterator const &) > const &pFunc)
{
    if (stPort.empty()) {
        return {""};
    }
    ara::core::String stSlotName(stPort.data(), stPort.size());
    ara::core::StringView::size_type const nFindPos{stPort.find("/")};
    if (ara::core::StringView::npos == nFindPos) {
        return stSlotName;
    }
    typename T_MAP::const_iterator const itFind{mapData.find(stSlotName)};
    if (itFind != mapData.end()) {
        std::ignore = itFind;
        return std::move(pFunc(itFind));
    }
    // If not found finally, return the original text
    return stSlotName;
}
}  // namespace

/// @brief Convert Port identifier to Provider identifier
/// @param stPort PORT identifier
/// @return Provider identifier
ara::core::String PManifestMapConfig::TransName_PortToProvider(ara::core::StringView const &stPort) const noexcept
{
    return T_TransName_PortToCrypto(mapPortToProvider_, stPort,
                                    [](MAP_PortToProvider::const_iterator const &itFind) -> ara::core::String {
                                        return itFind->second.stProviderInstance;
                                    });
}
/// @brief Convert Port identifier to key slot identifier
/// @param stPort PORT identifier
/// @return Key slot identifier
ara::core::String PManifestMapConfig::TransName_PortToSlot(ara::core::StringView const &stPort) const noexcept
{
    return T_TransName_PortToCrypto(mapPortToKeySlot_, stPort,
                                    [](MAP_PortToKeySlot::const_iterator const &itFind) -> ara::core::String {
                                        return itFind->second.stSlotInstance;
                                    });
}
/// @brief Convert Port identifier to certificate identifier
/// @param stPort PORT identifier
/// @return Certificate identifier
ara::core::String PManifestMapConfig::TransName_PortToCert(ara::core::StringView const &stPort) const noexcept
{
    return T_TransName_PortToCrypto(mapPortToCert_, stPort,
                                    [](MAP_PortToCert::const_iterator const &itFind) -> ara::core::String {
                                        return itFind->second.stCertInstance;
                                    });
}
/// @brief Get whether the current certificate is writable
/// @param stPort PORT identifier
/// @return true if can write false otherwise
bool PManifestMapConfig::IsCertPortWriteAccess(ara::core::String const &stPort) const noexcept
{
    ara::crypto::manifest::PManifestMapConfig::MAP_PortToCert::const_iterator const itFind{mapPortToCert_.find(stPort)};
    if (itFind != mapPortToCert_.end()) {
        return itFind->second.writeaccess;
    }
    /// It might also be certinstance here
    for (auto const &at : mapPortToCert_) {
        if (at.second.stCertInstance == stPort) {
            return at.second.writeaccess;
        }
    }

    return false;
}
//********************************/

}  // namespace manifest
}  // namespace crypto
}  // namespace ara