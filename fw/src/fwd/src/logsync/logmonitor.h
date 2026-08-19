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
/// @file       logmonitor.h
/// @brief      fw kern.log log processing class
/// @details    fw kern.log log processing class
/// @date       2024-12-17
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/IDSM Log Sync
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0009
/// @unit_name=Log_Monitor
/// @unit_description=Firewall IDSM data synchronization
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_LOG_MONITOR_H_
#define ARA_FW_LOG_MONITOR_H_
#include <ara/core/instance_specifier.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <fstream>
#include <string>
#include <unordered_map>

#include "ara/fw/common/common.h"
#include "ara/fw/internal/cmd_str.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief Log synchronization processing class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00414
/// @trace_id_dd=DD_FW_00613
/// @needwork = ad
/// @endcode
class LogMonitor final
{
public:
    /// @brief Constructor
    /// @param fwManager fw main management class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00415
    /// @trace_id_dd=DD_FW_00614
    /// @needwork = ad
    /// @endcode
    LogMonitor() = default;

    /// @brief Destructor
    /// @exception
    /// @code{.isoft}
    /// @code {.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00416
    /// @trace_id_dd=DD_FW_00615
    /// @needwork = ad
    /// @endcode
    virtual ~LogMonitor() noexcept { file_.close(); };
    /// @brief the move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00417
    /// @trace_id_dd=DD_FW_00616
    /// @needwork = ad
    /// @endcode
    LogMonitor(LogMonitor &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return LogMonitor&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00418
    /// @trace_id_dd=DD_FW_00617
    /// @needwork = ad
    /// @endcode
    LogMonitor &operator=(LogMonitor &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return LogMonitor&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00419
    /// @trace_id_dd=DD_FW_00618
    /// @needwork = ad
    /// @endcode
    LogMonitor &operator=(LogMonitor const &other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00420
    /// @trace_id_dd=DD_FW_00619
    /// @needwork = ad
    /// @endcode
    LogMonitor(LogMonitor const &other) = delete;

public:
    /// @brief Parse log line by line
    /// @param LogLine Log
    /// @return Success/Failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00421
    /// @trace_id_dd=DD_FW_00620
    /// @needwork = ad
    /// @endcode
    bool LogParse(ara::core::String const &logLine) noexcept;

    /// @brief Serialize data
    /// @param vec Data to serialize
    /// @param vecPaylod Serialized data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00422
    /// @trace_id_dd=DD_FW_00621
    /// @needwork = ad
    /// @endcode
    ara::core::Vector< uint8_t > Serialize(ara::core::Vector< ara::core::String > const &vec) noexcept;

    /// @brief Call IDSM interface to send data
    /// @param vecPaylod Serialized data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00423
    /// @trace_id_dd=DD_FW_00622
    /// @needwork = ad
    /// @endcode
    void CallIdsmApi(ara::core::Vector< uint8_t > const &vecPaylod) noexcept;

    /// @brief Execute synchronous log task.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00424
    /// @trace_id_dd=DD_FW_00623
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept;

private:
    /// @brief Determine if within letter range
    /// @param c Character to check
    /// @return Yes/No
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00624
    /// @needwork = dda
    /// @endcode
    bool _isFieldStart(char c) noexcept;
    /// @brief Determine if it is a letter
    /// @param c Character to check
    /// @return Yes/No
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00625
    /// @needwork = dda
    /// @endcode
    bool _isFieldChar(char c) noexcept;
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00626
    /// @needwork = dda
    /// @endcode
    void _saveReadLineNum() const noexcept;
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00627
    /// @needwork = dda
    /// @endcode
    void _loadLastReadLineNum() noexcept;
    /// @brief Parse a single field
    /// @param fieldStr Input value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00628
    /// @needwork = dda
    /// @endcode
    void _parseField(ara::core::String const &fieldStr) noexcept;

    /// @brief Parse app data
    /// @param logLine Read kern.log kernel log line by line
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00629
    /// @needwork = dda
    /// @endcode
    void _parseAppData(ara::core::String const &logLine) noexcept;

private:
    /// @brief  linenumber.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00630
    /// @needwork = dda
    /// @endcode
    int32_t lineNum_{-1};
    /// @brief Kernel file descriptor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00631
    /// @needwork = dda
    /// @endcode
    std::ifstream file_{GetkKernLog()};
    /// @brief Read line number.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00632
    /// @needwork = dda
    /// @endcode
    ara::core::String readLineFileName_{"./fwReadLine"};
    /// @brief Structure to save parsed kern.log log. (udp, tcp, ipv4 ip
    /// datalink.payload)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00633
    /// @needwork = dda
    /// @endcode
    std::unordered_map< ara::core::String, ara::core::String > fields_{};
    /// @brief Structure to save parsed kern.log log. (someip)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00634
    /// @needwork = dda
    /// @endcode
    std::unordered_map< ara::core::String, ara::core::String > someipFields_{};
    /// @brief Structure to save parsed kern.log log. (someip sd )
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00635
    /// @needwork = dda
    /// @endcode
    std::unordered_map< ara::core::String, ara::core::String > someipSdFields_{};
    /// @brief Structure to save parsed kern.log log. (doip )
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00636
    /// @needwork = dda
    /// @endcode
    std::unordered_map< ara::core::String, ara::core::String > doipFields_{};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif
