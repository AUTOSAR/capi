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
/// @file       log_manager_logger_module.h
/// @brief
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_MANAGER_LOGGER_MODULE_H__
#define __LOG_MANAGER_LOGGER_MODULE_H__

#include <map>
#include <memory>
#include <string>

#include "DltLogDefines/application_config.h"
#include "DltLogDefines/dlt_sinkers_config.h"
#include "DltLogDefines/logger_scope_data.h"
#include "DltSinkers/log_sinker_interface.h"
#include "common.h"

namespace ara {
namespace log {
namespace internal {

class LogManagerLoggerModule final
{
public:
    void CreateFileSinker(std::shared_ptr< ApplicationConfig > const &logConfig,
                          std::map< std::string, std::shared_ptr< IlogSinker > > &fileSinkerMap,
                          std::shared_ptr< DLTV2::DltSinkersConfig > const &fileConfig,
                          LoggerDataPtr &logScopedData) const noexcept;

    void InitializeLoggerScopeData(std::shared_ptr< ApplicationConfig > const &logConfig,
                                   LoggerDataPtr &logScopedData,
                                   std::string const &cid,
                                   std::string const &des,
                                   ara::log::LogLevel const &level) const noexcept;
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif