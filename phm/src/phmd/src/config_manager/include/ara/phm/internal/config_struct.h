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
/// @file       config_struct.h
/// @brief      Class to store the conf of PHM.
/// @details
/// @date       2024-06-06
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/ConfigManager
/// @unit_description=Class to store the conf of PHM.
/// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
/// SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
/// @unit_name=ConfigStruct
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_CONFIG_STRUCT_H_
#define ARA_PHM_INTERNAL_CONFIG_STRUCT_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/define.h>
#include <isoft/manifestreader/manifest_reader.h>

#include "ara/phm/internal/phm_log.h"

/// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
/// @needwork = no
#define MANIFEST_RET_CHECK(ret, fieldName)                                                                             \
    if (isoft::kSuccess != (ret)) {                                                                                    \
        LOG_WARN << (fieldName) << " parse error, ret:" << (ret);                                                      \
        return (ret);                                                                                                  \
    }
namespace ara {
namespace phm {
namespace internal {

/// @brief The min timeout of os watchdog.
/// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
/// @needwork = no
int32_t const kOsWatchdogMinTimeoutMs{8000};

/// @brief Conf of checkpoint.
/// @trace_id_sr=SR_PHM_02001
/// @needwork = ad
class CheckpointConf final
{
public:
    /// @brief fqn of checkopoint.
    /// @trace_id_sr=SR_PHM_02001
    /// @needwork = dda
    ara::core::String fqn;

    /// @brief name of checkopoint.
    /// @trace_id_sr=SR_PHM_02001
    /// @needwork = dda
    ara::core::String shortName;

    /// @brief the interface of checkpoint.
    /// @trace_id_sr=SR_PHM_02001
    /// @needwork = dda
    ara::core::String identifier;

    /// @brief the process name this checkpoint belongs to.
    /// @trace_id_sr=SR_PHM_02001
    /// @needwork = dda
    ara::core::String processName;

    /// @brief checkpoint id.
    /// @trace_id_sr=SR_PHM_02001
    /// @needwork = dda
    uint32_t checkpointId{0U};

    /// @brief Standard usage of json parser.
    /// @param node json node.
    /// @return int32_t 0 success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02001
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of alive supervision.
/// @trace_id_sr=SR_PHM_02002
/// @needwork = ad
class AliveSupervisionConf final
{
public:
    /// @brief fqn of alive supervision.
    /// @trace_id_sr=SR_PHM_02002
    /// @needwork = dda
    ara::core::String fqn;

    /// @brief name of alive supervision.
    /// @trace_id_sr=SR_PHM_02002
    /// @needwork = dda
    ara::core::String shortName;

    /// @brief checkpoint's fqn referenced.
    /// @trace_id_sr=SR_PHM_02002
    /// @needwork = dda
    ara::core::String checkpointFqn;

    /// @brief the timer period for PHM to check alive supervision status.
    /// @trace_id_sr=SR_PHM_02002
    /// @needwork = dda
    double aliveRefrenceCycle{0};

    /// @brief expected alive indications when check alive supervision.
    /// @trace_id_sr=SR_PHM_02002
    /// @needwork = dda
    uint32_t expectedAliveIndications{0U};

    /// @brief min deviation allowed of indications.
    /// @trace_id_sr=SR_PHM_02002
    /// @needwork = dda
    uint32_t maxMargin{0U};

    /// @brief max deviation allowed of indications.
    /// @trace_id_sr=SR_PHM_02002
    /// @needwork = dda
    uint32_t minMargin{0U};

    /// @brief Standard usage of json parser
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02002
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of deadline supervision.
/// @trace_id_sr=SR_PHM_02003
/// @needwork = ad
class DeadlineSupervisionConf final
{
public:
    /// @brief fqn of deadline supervision.
    /// @trace_id_sr=SR_PHM_02003
    /// @needwork = dda
    ara::core::String fqn;

    /// @brief name of deadline supervision.
    /// @trace_id_sr=SR_PHM_02003
    /// @needwork = dda
    ara::core::String shortName;

    /// @brief checkpoint transition referenced.
    /// @trace_id_sr=SR_PHM_02003
    /// @needwork = dda
    ara::core::Vector< ara::core::String > checkpointTransition;

    /// @brief max deadline allowed.
    /// @trace_id_sr=SR_PHM_02003
    /// @needwork = dda
    double maxDeadline{0};

    /// @brief min deadline allowed.
    /// @trace_id_sr=SR_PHM_02003
    /// @needwork = dda
    double minDeadline{0};

    /// @brief Standard usage of json parser.
    /// @param node json node.
    /// @return int32_t  success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02003
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of checkpoint trannsition.
/// @trace_id_sr=SR_PHM_02003,SR_PHM_02004
/// @needwork = ad
class TransitionConf final
{
public:
    /// @brief name of checkpoint transition.
    /// @trace_id_sr=SR_PHM_02003,SR_PHM_02004
    /// @needwork = dda
    ara::core::String shortName;

    /// @brief source checkpoint fqn of checkpoint transition.
    /// @trace_id_sr=SR_PHM_02003,SR_PHM_02004
    /// @needwork = dda
    ara::core::String sourceCheckpoint;

    /// @brief target checkpoint fqn of checkpoint transition.
    /// @trace_id_sr=SR_PHM_02003,SR_PHM_02004
    /// @needwork = dda
    ara::core::String targetCheckpoint;

    /// @brief Standard usage of json parser.
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02003,SR_PHM_02004
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of logical supervision.
/// @trace_id_sr=SR_PHM_02004
/// @needwork = ad
class LogicalSupervisionConf final
{
public:
    /// @brief fqn of logical supervision.
    /// @trace_id_sr=SR_PHM_02004
    /// @needwork = dda
    ara::core::String fqn;

    /// @brief name of logical supervision.
    /// @trace_id_sr=SR_PHM_02004
    /// @needwork = dda
    ara::core::String shortName;

    /// @brief initial checkpoint referenced by logical supervision.
    /// @trace_id_sr=SR_PHM_02004
    /// @needwork = dda
    ara::core::Vector< ara::core::String > initialCheckpoint;

    /// @brief final checkpoint referenced by logical supervision.
    /// @trace_id_sr=SR_PHM_02004
    /// @needwork = dda
    ara::core::Vector< ara::core::String > finalCheckpoint;

    /// @brief checkpoint transition referenced by logical supervision.
    /// @trace_id_sr=SR_PHM_02004
    /// @needwork = dda
    ara::core::Vector< TransitionConf > transition;

    /// @brief Standard usage of json parser.
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02004
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of local supervision.
/// @trace_id_sr=SR_PHM_02004
/// @needwork = ad
class LocalSupervisionConf final
{
public:
    /// @brief name of local supervision.
    /// @trace_id_sr=SR_PHM_02005
    /// @needwork = dda
    ara::core::String shortName;

    /// @brief fqn of local supervision.
    /// @trace_id_sr=SR_PHM_02005
    /// @needwork = dda
    ara::core::String fqn;

    /// @brief failed supervision cycles allowed.
    /// @trace_id_sr=SR_PHM_02005
    /// @needwork = dda
    uint32_t failedSupervisionCyclesTolerance{0U};

    /// @brief alive supervisions referenced by local supervision.
    /// @trace_id_sr=SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< ara::core::String > aliveSupervision;

    /// @brief deadline supervisions referenced by local supervision.
    /// @trace_id_sr=SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< ara::core::String > deadlineSupervision;

    /// @brief logical supervisions referenced by local supervision.
    /// @trace_id_sr=SR_PHM_02005
    /// @needwork = dda
    ara::core::Vector< ara::core::String > logicalSupervision;

    /// @brief Standard usage of json parser.
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02005
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of global supervision.
/// @trace_id_sr=SR_PHM_02006
/// @needwork = ad
class GlobalSupervisionInfo final
{
public:
    /// @brief name of global supervision.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    ara::core::String shortName;

    /// @briefperiod of timer driving to determine the status of global supervision.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    double supervisionCycle{0};

    /// @brief expired supervision cycles allowed.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    uint32_t expiredSupervisonCycleTolerance{0U};

    /// @brief local supervisions referenced by global supervision.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    ara::core::Vector< ara::core::String > localSupervision;

    /// @brief Standard usage of json parser.
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of recovery.
/// @trace_id_sr=SR_PHM_02007
/// @needwork = ad
class RecoverNotificationConf final
{
public:
    /// @brief timeout of recovery.
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    double recoveryNotificationTimeout{0};

    /// @brief max retry of recovery.
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    uint32_t recoveryNotificationRetry{0U};

    /// @brief Standard usage of json parser
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of global supervision recovery.
/// @trace_id_sr=SR_PHM_02007
/// @needwork = ad
class GlobalSupervisionReoveryInfo final
{
public:
    /// @brief name of global supervision recovery.
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    ara::core::String shortName;

    /// @brief model id.
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    ara::core::String metaModelIdentifier;

    /// @brief global supervision referenced.
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    ara::core::String globalSupervision;

    /// @brief recovery conf.
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    RecoverNotificationConf recoveryNotification;

    /// @brief Standard usage of json parser.
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Total conf of global supervision,
/// composed of GlobalSupervisionInfo and GlobalSupervisionReoveryInfo.
/// @trace_id_sr=SR_PHM_02006
/// @needwork = ad
class GlobalSupervisionConf final
{
public:
    /// @brief name of global supervision.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    ara::core::String shortName;

    /// @brief model id.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    ara::core::String metaModelIdentifier;

    /// @brief time period to determine the status of global supervision.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    double supervisionCycle{0};

    /// @brief expired supervision cycle allowed.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    uint32_t expiredSupervisionCycleTolerance{0U};

    /// @brief max retry times.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    uint32_t recoveryNotificationRetry{0U};

    /// @brief recover timeout.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    double recoveryNotificationTimeout{0};

    /// @brief local supervisions referenced by global supervision.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    ara::core::Vector< ara::core::String > localSupervision;

    /// @brief whether this global supervision is supervised.
    /// @trace_id_sr=SR_PHM_02006
    /// @needwork = dda
    bool isSupervised{false};
};

/// @brief Conf of process.
/// @trace_id_sr=SR_PHM_02006
/// @needwork = ad
class ProcessInfoConf final
{
public:
    /// @brief name of process.
    /// @trace_id_sr=SR_PHM_02010
    /// @needwork = dda
    ara::core::String processName;

    /// @brief execution error code of process.
    /// @trace_id_sr=SR_PHM_02010
    /// @needwork = dda
    int32_t executionError{0};

    /// @brief Standard usage of json parser.
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02010
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of function group state.
/// @trace_id_sr=SR_PHM_02010
/// @needwork = ad
class OneFgStateConf final
{
public:
    /// @brief state of function group.
    /// @trace_id_sr=SR_PHM_02010
    /// @needwork = dda
    ara::core::String stateName;

    /// @brief phm supervisions fqn activated in this function group state.
    /// @trace_id_sr=SR_PHM_02010
    /// @needwork = dda
    ara::core::Vector< ara::core::String > phmSupervision;

    /// @brief process info of this function group state.
    /// @trace_id_sr=SR_PHM_02010
    /// @needwork = dda
    ara::core::Vector< ProcessInfoConf > processInfo;

    /// @brief Standard usage of json parser.
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02010
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};
/// @brief Conf of function group supervision mode.
/// @trace_id_sr=SR_PHM_02010
/// @needwork = ad
class FgSupervisionModeConf final
{
public:
    /// @brief name of function group.
    /// @trace_id_sr=SR_PHM_02010
    /// @needwork = dda
    ara::core::String fqn;

    /// @brief contaner of OneFgStateConf.
    /// @trace_id_sr=SR_PHM_02010
    /// @needwork = dda
    ara::core::Vector< OneFgStateConf > fgStateConf;

    /// @brief Standard usage of json parser
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02010
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of health status.
/// @trace_id_sr=SR_PHM_02008
/// @needwork = ad
class HealthStatusConf final
{
public:
    /// @brief name of health status.
    /// @trace_id_sr=SR_PHM_02008
    /// @needwork = dda
    ara::core::String shortName;

    /// @brief id of health status.
    /// @trace_id_sr=SR_PHM_02008,
    /// @needwork = dda
    uint32_t statusId{0U};

    /// @brief
    /// @brief
    /// @trace_id_sr=SR_PHM_02008
    /// @needwork = dda
    bool triggersRecoveryNotification{false};

    /// @brief Standard usage of json parser.
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02008
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of health channel.
/// @trace_id_sr=SR_PHM_02008
/// @needwork = ad
class HealthChannelEntityConf final
{
public:
    /// @brief name of health channel.
    /// @trace_id_sr=SR_PHM_02008
    /// @needwork = dda
    ara::core::String shortName;

    /// @brief container of health status belongs to this health channel.
    /// @trace_id_sr=SR_PHM_02008
    /// @needwork = dda
    ara::core::Vector< HealthStatusConf > healthStatus;

    /// @brief Standard usage of json parser
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02008
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of health channel supervision.
/// @trace_id_sr=SR_PHM_02009
/// @needwork = ad
class HealthChannelSupervisionConf final
{
public:
    /// @brief name of health channel.
    /// @trace_id_sr=SR_PHM_02009
    /// @needwork = dda
    ara::core::String shortName;

    /// @brief model id.
    /// @trace_id_sr=SR_PHM_02009
    /// @needwork = dda
    ara::core::String metaModelIdentifier;

    /// @brief identifier of health channel.
    /// @trace_id_sr=SR_PHM_02009
    /// @needwork = dda
    ara::core::String hcInterfaceIdentifier;

    /// @brief  interface of health channel
    /// @trace_id_sr=SR_PHM_02009
    /// @needwork = dda
    ara::core::String phmHealthChannelInterface;

    /// @brief conf of recover.
    /// @trace_id_sr=SR_PHM_02009
    /// @needwork = dda
    RecoverNotificationConf recoveryNotification;

    /// @brief Standard usage of json parser
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02009
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief Conf of Watchdog.
/// @trace_id_sr=SR_PHM_02007
/// @needwork = ad
class WatchdogConf final
{
public:
    /// @brief watchdog dev.
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    ara::core::String osWatchdog;

    /// @brief timeout of watchdog.
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    int32_t watchdogTimeoutMs;

    /// @brief
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    bool enable;

    /// @brief Standard usage of json parser.
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    static int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

/// @brief conf of recovery notification mapping.
/// @trace_id_sr=SR_PHM_02007
/// @needwork = ad
class RecoveryNotificationPortMappingConf final
{
public:
    /// @brief instance specifier
    /// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
    /// @needwork = dda
    ara::core::String instanceSpecifier;

    /// @brief health channel id
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    ara::core::String healthChannelId;

    /// @brief Standard usage of json parser.
    /// @param node json node.
    /// @return int32_t success; other failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    /// @trace_id_sr=SR_PHM_02007
    /// @needwork = dda
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_CONFIG_STRUCT_H_