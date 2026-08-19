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
/// @file       cg_config.h
/// @brief      Cg data stream config header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_INTERNAL_CG_CG_CONFIG_H
#define ARA_COM_INTERNAL_CG_CG_CONFIG_H

#include "ara/com/internal/log/log.h"
#include "ara/core/array.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/map.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "isoft/define.h"
#include "isoft/manifestreader/manifest_node.h"
#include "isoft/manifestreader/manifest_reader.h"

namespace ara {
namespace com {
namespace internal {
namespace cg {
/// @brief
struct CgConfig
{
    /// @brief instanceSpecifier
    ara::core::String instanceSpecifier;
    /// @brief cg interface name
    ara::core::String name;

    /// @brief
    /// @param[in] node
    /// @return
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
    {
        if (isoft::kSuccess != node.Load("instance_specifier", instanceSpecifier)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("name", name)) {
            return isoft::kFailure;
        }
        return isoft::kSuccess;
    }
};

/// @brief
class CgMappingManager final
{
public:
    /// @brief
    /// @param[in] name
    /// @return
    static ara::core::String GetServerInstanceSpecifier(ara::core::String const& name) noexcept
    {
        return Impl::Instance().GetServerInstanceSpecifierImpl(name);
    }
    /// @brief
    /// @param[in] path
    /// @return
    static bool LoadConfig(ara::core::StringView const& path) noexcept { return Impl::Instance().LoadConfigImpl(path); }

private:
    /// @brief
    class Impl final
    {
    public:
        /// @brief
        /// @return
        static Impl& Instance() noexcept
        {
            static Impl s_Instance;
            return s_Instance;
        }
        /// @brief
        /// @param[in] path
        /// @return bool
        bool LoadConfigImpl(ara::core::StringView const& path) noexcept
        {
            if (configured_) {
                ComLogInfo("Cg was already configured, skipping");
                return true;
            }
            auto manifestRes{isoft::manifestreader::OpenManifest(path)};
            if (!manifestRes) {
                return false;
            }
            ara::core::Vector< CgConfig > cnf;
            auto ret{manifestRes.Value()->Load("cg_server_service_instances", cnf)};
            if (ret == isoft::kSuccess) {
                for (auto const& it : cnf) {
                    cgServeceConfigMappings_[it.name] = std::make_shared< CgConfig >(it);
                }
            }
            cnf.clear();
            ret = manifestRes.Value()->Load("cg_client_service_instances", cnf);
            if (ret == isoft::kSuccess) {
                for (auto const& it : cnf) {
                    cgClientConfigMappings_[it.name] = std::make_shared< CgConfig >(it);
                }
            }
            configured_ = true;
            return true;
        }
        /// @brief
        /// @param[in] name
        /// @return
        ara::core::String GetServerInstanceSpecifierImpl(ara::core::String const& name) const noexcept
        {
            ara::core::String ret{};
            auto it{cgServeceConfigMappings_.find(name)};
            if (it != cgServeceConfigMappings_.end()) {
                ret = it->second->instanceSpecifier;
            }
            return ret;
        }
        /// @brief
        /// @param[in] name
        /// @return
        ara::core::String GetClientInstanceSpecifierImpl(ara::core::String const& name) const noexcept
        {
            ara::core::String ret{};
            auto it{cgClientConfigMappings_.find(name)};
            if (it != cgClientConfigMappings_.end()) {
                ret = it->second->instanceSpecifier;
            }
            return ret;
        }

    private:
        /// @brief false config is not load
        bool configured_{false};
        /// @brief server service interface mapping
        ara::core::Map< ara::core::String, std::shared_ptr< CgConfig > > cgServeceConfigMappings_;
        /// @brief client service interface mapping
        ara::core::Map< ara::core::String, std::shared_ptr< CgConfig > > cgClientConfigMappings_;
    };
};
}  // namespace cg
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif