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
/// @file       config_manager.h
/// @brief      parse PHM config and privide conf to other modules.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/ConfigManager
/// @unit_description=parse PHM config and privide conf to other modules.
/// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
/// SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
/// @unit_name=ConfigParser
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_CONFIG_MANAGER_H_
#define ARA_PHM_INTERNAL_CONFIG_MANAGER_H_

#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <mutex>

#include "ara/phm/internal/config_struct.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief ConfigManager, parse PHM config and privide conf to other modules.
/// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
/// @needwork = ad
class ConfigManager final
{
public:
    /// @brief Creation of ConfigManager, shared singleton.
    /// @param configPath the path of phm conf.
    /// @return shared ptr of ConfigManager
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
    ///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
    /// @needwork = ad
    static std::shared_ptr< ConfigManager > GetInstanceShared(ara::core::String const& configPath)
    {
        LOG_INFO << "config path " << configPath.c_str();
        static std::shared_ptr< ConfigManager > s_Instance{nullptr};
        if (s_Instance.get() == nullptr) {
            static std::once_flag s_Flag{};
            std::call_once(s_Flag, [&configPath]() { s_Instance.reset(new ConfigManager(configPath)); });
            std::ignore = s_Flag;
        }
        return s_Instance;
    }

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
    ///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
    /// @needwork = ad
    ConfigManager(ConfigManager& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return ConfigManager& The copied object.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
    ///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
    /// @needwork = ad
    ConfigManager& operator=(ConfigManager const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
    ///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
    /// @needwork = ad
    ConfigManager(ConfigManager&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
    ///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
    /// @needwork = ad
    ConfigManager& operator=(ConfigManager const&& obj) = delete;

    /// @brief Destructor of ConfigManager
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
    ///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
    /// @needwork = ad
    ~ConfigManager() = default;

    /// @brief Load the conf file and parse it.
    /// @return 0, success; < 0, fail.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
    ///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
    /// @needwork = ad
    int32_t Parse() noexcept;

    /// @brief Returns the conf of checkpoint.
    /// @return the conf of checkpoint.
    /// @trace_id_sr=SR_PHM_02001
    /// @needwork = ad
    ara::core::Vector< CheckpointConf > const& GetCheckpointConf() const noexcept;

    /// @brief Returns the conf of alive supervision.
    /// @return the conf of alive supervision.
    /// @trace_id_sr=SR_PHM_02002
    /// @needwork = ad
    ara::core::Vector< AliveSupervisionConf > const& GetAliveSupervisionConf() const noexcept;

    /// @brief Returns the conf of deadline supervision.
    /// @return the conf of deadline supervision.
    /// @trace_id_sr=SR_PHM_02003
    /// @needwork = ad
    ara::core::Vector< DeadlineSupervisionConf > const& GetDeadlineSupervisionConf() const noexcept;

    /// @brief Returns the conf of logical supervision.
    /// @return the conf of logical supervision.
    /// @trace_id_sr=SR_PHM_02004
    /// @needwork = ad
    ara::core::Vector< LogicalSupervisionConf > const& GetLogicalSupervisionConf() const noexcept;

    /// @brief Returns the conf of local supervision
    /// @return the conf of local supervision.
    /// @trace_id_sr=SR_PHM_02005
    /// @needwork = ad
    ara::core::Vector< LocalSupervisionConf > const& GetLocalSupervisionConf() const noexcept;

    /// @brief Returns the conf of global supervision
    /// @return the conf of global supervision.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = ad
    ara::core::Vector< GlobalSupervisionConf > const& GetGlobalSupervisionConf() const noexcept;

    /// @brief Returns the conf of fg supervision conf.
    /// @return the conf of fg supervision conf.
    /// @trace_id_sr=SR_PHM_02010
    /// @needwork = ad
    ara::core::Vector< FgSupervisionModeConf > const& GetSupervisionModeConf() const noexcept;

    /// @brief Returns the conf of health channel.
    /// @return the conf of health channel
    /// @trace_id_sr=SR_PHM_02008
    /// @needwork = ad
    ara::core::Vector< HealthChannelEntityConf > const& GetHealthChannelConf() const noexcept;

    /// @brief Returns the conf of health channel supervision.
    /// @return the conf of health channel supervision.
    /// @trace_id_sr=SR_PHM_02009
    /// @needwork = ad
    ara::core::Vector< HealthChannelSupervisionConf > const& GetHealthChannelSupervisionConf() const noexcept;

    /// @brief Returns the conf of watchdog
    /// @return the conf of watchdog.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
    ///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
    /// @needwork = ad
    WatchdogConf const& GetWatchdogConf() const noexcept;

    /// @brief Returns the conf of recovery notification mapping.
    /// @return the conf of recovery notification mappint.
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = ad
    ara::core::Vector< RecoveryNotificationPortMappingConf > const& GetRecoveryNotificationMap() const noexcept;

private:
    /// @brief Constructor of ConfigManager.
    /// @param configPath the path of conf.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
    /// @needwork = dda
    explicit ConfigManager(ara::core::String configPath) noexcept
        : kConfigPath(std::move(configPath))
        , checkpointConf_{}
        , aliveSupervisionConf_{}
        , deadlineSupervisionConf_{}
        , logicalSupervisionConf_{}
        , localSupervisionConf_{}
        , globalSupervisionConf_{}
        , supervisionModeConf_{}
        , healthChannelConf_{}
        , healthChannelSupervisionConf_{}
        , watchdogConf_{}
        , recoveryNotificationMapConf_{} {};

    /// @brief Parse phm contribute.
    /// @param contributeNode The json node.
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
    ///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
    /// @needwork = dda
    int32_t _parsePhmContribute(isoft::manifestreader::ManifestNode const& contributeNode) noexcept;

    /// @brief Parse conf of checkpoint.
    /// @param manifestNode The json node.
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_02001
    /// @needwork = dda
    int32_t _parseCheckpoint(isoft::manifestreader::ManifestNode const& manifestNode) noexcept;

    /// @brief Parse conf of alive supervision.
    /// @param manifestNode The json node.
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_02002
    /// @needwork = dda
    int32_t _parseAliveSupervision(isoft::manifestreader::ManifestNode const& manifestNode) noexcept;

    /// @brief Parse conf of deadline supervision.
    /// @param manifestNode The json node.
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_02003
    /// @needwork = dda
    int32_t _parseDeadlineSupervision(isoft::manifestreader::ManifestNode const& manifestNode) noexcept;

    /// @brief Parse conf of logical supervision.
    /// @param manifestNode The json node.
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_02004
    /// @needwork = dda
    int32_t _parseLogicalSupervision(isoft::manifestreader::ManifestNode const& manifestNode) noexcept;

    /// @brief Parse conf of local supervision.
    /// @param manifestNode The json node.
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_02005
    /// @needwork = dda
    int32_t _parseLocalSupervision(isoft::manifestreader::ManifestNode const& manifestNode) noexcept;

    /// @brief Parse conf of global supervision.
    /// @param manifestNode The json node.
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    int32_t _parseGlobalSupervision(isoft::manifestreader::ManifestNode const& manifestNode) noexcept;

    /// @brief Parse conf of supervision mode.
    /// @param manifestNode The json node.
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_02010
    /// @needwork = dda
    int32_t _parseSupervisionMode(isoft::manifestreader::ManifestNode const& manifestNode) noexcept;

    /// @brief Parse conf of health channel.
    /// @param manifestNode The json node.
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_02008
    /// @needwork = dda
    int32_t _parseHealthChannel(isoft::manifestreader::ManifestNode const& manifestNode) noexcept;

    /// @brief Parse conf of health channel supervision.
    /// @param manifestNode The json node.
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_02009
    /// @needwork = dda
    int32_t _parseHealthChannelSupervision(isoft::manifestreader::ManifestNode const& manifestNode) noexcept;

    /// @brief Parse conf of recovery notification mapping.
    /// @param manifestNode The json node.
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    int32_t _parseRecoveryNotificationPortMapping(isoft::manifestreader::ManifestNode const& manifestNode) noexcept;

    /// @brief Parse conf of watchdog.
    /// @param manifestNode The json node.
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    int32_t _parseWatchdogConf(isoft::manifestreader::ManifestNode const& manifestNode) noexcept;

private:
    /// @brief the path of conf.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    ara::core::String const kConfigPath;

    /// @brief the conf of checkpoint.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< CheckpointConf > checkpointConf_;

    /// @brief the conf of alive supervision.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< AliveSupervisionConf > aliveSupervisionConf_;

    /// @brief the conf of deadline supervision.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< DeadlineSupervisionConf > deadlineSupervisionConf_;

    /// @brief the conf of logical supervision.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< LogicalSupervisionConf > logicalSupervisionConf_;

    /// @brief the conf of local supervision.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< LocalSupervisionConf > localSupervisionConf_;

    /// @brief the conf of global supervision.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< GlobalSupervisionConf > globalSupervisionConf_;

    /// @brief the conf of supervision mode.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< FgSupervisionModeConf > supervisionModeConf_;

    /// @brief the conf of health channel.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< HealthChannelEntityConf > healthChannelConf_;

    /// @brief the conf of health channel supervision.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< HealthChannelSupervisionConf > healthChannelSupervisionConf_;

    /// @brief the conf of watchdog
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    WatchdogConf watchdogConf_;

    /// @brief the conf of recovery notification.
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< RecoveryNotificationPortMappingConf > recoveryNotificationMapConf_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_CONFIG_MANAGER_H_
