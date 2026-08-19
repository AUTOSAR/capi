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
/// @file       log_manager_config_module.h
/// @brief
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_MANAGER_CONFIG_MODULE_H__
#define __LOG_MANAGER_CONFIG_MODULE_H__

#include <rapidjson/document.h>

#include <memory>
#include <string>

#include "DltLogDefines/application_config.h"

namespace ara {
namespace log {
namespace internal {

class LogManagerConfigModule final
{
public:
    bool SearchConfigPath(std::string &appConfig, std::string &globalConfig) const;

    bool ReadGlobalConfig(std::shared_ptr< ApplicationConfig > const &logConfig,
                          std::string const &configPath) const noexcept;

    bool ReadAppConfig(std::shared_ptr< ApplicationConfig > const &logConfig,
                       std::string const &configPath) const noexcept;

    bool ReadAppConfigV1(std::shared_ptr< ApplicationConfig > const &logConfig,
                         std::string const &configPath) const noexcept;

    bool ReadAppConfigV2(std::shared_ptr< ApplicationConfig > const &logConfig,
                         std::string const &configPath) const noexcept;

    bool ParseV1Channel(std::shared_ptr< ApplicationConfig > const &logConfig,
                        rapidjson::Value &channelObj,
                        bool appEnableCompress,
                        bool appPlainText,
                        std::string const &fileEndSuf) const noexcept;

    bool ParseV2Context(std::shared_ptr< ApplicationConfig > const &logConfig,
                        rapidjson::Value &contextObj,
                        bool appEnableCompress,
                        bool appPlainText,
                        std::uint32_t defSingleFileSize,
                        std::uint32_t defFileCount,
                        std::uint32_t defileBuffer,
                        std::uint32_t defDetermFreeCount) const noexcept;

private:
    static std::string NormalizeIdentifier(std::string const &identifier) noexcept;
    static std::string GetCurrentExecutableDir() noexcept;
    static bool FileExists(std::string const &filename) noexcept;
    static bool HasValidSuffix(std::string const &str) noexcept;
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif