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
/// @file       fg_info_config.h
/// @brief      Read configuration files related to function group information
/// @details
/// @date       2024-04-30
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Config
/// @unit_name=FGInfoConfig
/// @interface_level=uint
/// @unit_description=Read configuration files related to function group information
/// @trace_id_sr=SR_SM_00101，SR_SM_00106
/// @endcode
///
/// ================================================================

#ifndef FG_INFO_CONFIG_H_
#define FG_INFO_CONFIG_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/log/logger.h>
#include <isoft/manifestreader/manifest_reader.h>

namespace ara {
namespace sm {
namespace config {

/// @brief Simple function group info struct
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_00101
/// @trace_id_ad=AD_SM_08028
/// @trace_id_dd=DD_SM_08060
/// @needwork = ad
/// @endcode
struct FGInfoSimple
{
    /// @brief Function group's FQN
    core::String fgFQN;

    /// @brief The states managed by function group
    core::Vector< core::String > fgStates;
};

/// @brief The class for parsing the config files and getting funtion groups' info
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_00101，SR_SM_00106
/// @trace_id_ad=AD_SM_08029
/// @trace_id_dd=DD_SM_08061
/// @needwork = ad
/// @endcode
class FGInfoConfig
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00101，SR_SM_00106
    /// @trace_id_ad=AD_SM_00024
    /// @trace_id_dd=DD_SM_00024
    /// @needwork = ad
    /// @endcode
    FGInfoConfig() noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00101，SR_SM_00106
    /// @trace_id_ad=AD_SM_00025
    /// @trace_id_dd=DD_SM_00025
    /// @needwork = ad
    /// @endcode
    ~FGInfoConfig() noexcept;

    /// @brief deleted copy constructor function
    /// @param other The FGInfoConfig instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00101，SR_SM_00106
    /// @trace_id_ad=AD_SM_00026
    /// @trace_id_dd=DD_SM_00026
    /// @needwork = ad
    /// @endcode
    FGInfoConfig(FGInfoConfig const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The FGInfoConfig instance to be copyed
    /// @return the assigned FGInfoConfig instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00101，SR_SM_00106
    /// @trace_id_ad=AD_SM_00027
    /// @trace_id_dd=DD_SM_00027
    /// @needwork = ad
    /// @endcode
    FGInfoConfig& operator=(FGInfoConfig const& other) = delete;

    /// @brief Move constructor function
    /// @param other The FGInfoConfig instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00101，SR_SM_00106
    /// @trace_id_ad=AD_SM_00028
    /// @trace_id_dd=DD_SM_00028
    /// @needwork = ad
    /// @endcode
    FGInfoConfig(FGInfoConfig&& other) = default;

    /// @brief Move assignment function
    /// @param other The FGInfoConfig instance to be moved
    /// @return the assigned FGInfoConfig instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00101，SR_SM_00106
    /// @trace_id_ad=AD_SM_00029
    /// @trace_id_dd=DD_SM_00029
    /// @needwork = ad
    /// @endcode
    FGInfoConfig& operator=(FGInfoConfig&& other) = delete;

    /// @brief Get all funtion groups' info list
    /// @return funtion groups' info list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00101
    /// @trace_id_ad=AD_SM_00030
    /// @trace_id_dd=DD_SM_00030
    /// @needwork = ad
    /// @endcode
    core::Vector< FGInfoSimple > const& GetAllFGInfoList() const noexcept;

    /// @brief Get the mapping of physical address to function group FQN
    /// @return Mapping of physical address to function group FQN
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00106
    /// @trace_id_ad=AD_SM_00031
    /// @trace_id_dd=DD_SM_00031
    /// @needwork = ad
    /// @endcode
    core::Map< int32_t, core::Vector< core::String > > const& GetPhysicalAddr2FGFQNs() const noexcept;

    /// @brief Get the mapping of functional address to function group FQN
    /// @return Mapping of functional address to function group FQN
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00106
    /// @trace_id_ad=AD_SM_00032
    /// @trace_id_dd=DD_SM_00032
    /// @needwork = ad
    /// @endcode
    core::Map< int32_t, core::Vector< core::String > > const& GetFunctionalAddr2FGFQNs() const noexcept;

    /// @brief Get the FQN of MachineFG
    /// @return FQN of MachineFG
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00101
    /// @trace_id_ad=AD_SM_00033
    /// @trace_id_dd=DD_SM_00033
    /// @needwork = ad
    /// @endcode
    core::String GetMachineFgFQN() const noexcept;

    /// @brief Load all funtion groups' info list
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00101，SR_SM_00106
    /// @trace_id_ad=AD_SM_00034
    /// @trace_id_dd=DD_SM_00034
    /// @needwork = ad
    /// @endcode
    bool Load() noexcept;

private:
    /// @brief Software cluster information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00106
    /// @trace_id_ad=AD_SM_08029
    /// @trace_id_dd=DD_SM_08062
    /// @needwork = dd
    /// @endcode
    struct SwclInfo
    {
        /// @brief Software cluster name
        core::String name;

        /// @brief Software cluster version number
        core::String version;

        /// @brief Physical address to which the software cluster belongs
        int32_t physicalDiagnosticAddress{};

        /// @brief Set of functional addresses to which the software cluster belongs
        core::Vector< int32_t > functionalDiagnosticAddresses;
    };

    /// @brief Load software cluster information
    /// @param path Software cluster configuration file path
    /// @return List of software cluster information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00101，SR_SM_00106
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08063
    /// @needwork = dda
    /// @endcode
    core::Vector< SwclInfo > _loadSwclsInfo(core::String const& path) noexcept;

    /// @brief Load function group information
    /// @param path fgSet configuration file path
    /// @param diganosticAddress Physical address
    /// @param functionalAddresses Set of functional addresses
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00101
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08064
    /// @needwork = dda
    /// @endcode
    void _loadFGInfoSimple(core::String const& path,
                           int32_t const& diganosticAddress,
                           core::Vector< int32_t > const& functionalAddresses) noexcept;

    /// @brief Print software cluster information
    /// @param info Software cluster information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00106
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08065
    /// @needwork = dda
    /// @endcode
    void _printSwclInfo(FGInfoConfig::SwclInfo const& info) const noexcept;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00101，SR_SM_00106
    /// @trace_id_ad=   AD_SM_00000
    /// @trace_id_dd=DD_SM_08066
    /// @needwork = dda
    /// @endcode
    log::Logger& log_;

    /// @brief List of function group information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00101
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08067
    /// @needwork = dda
    /// @endcode
    core::Vector< FGInfoSimple > fgInfoList_;

    /// @brief Mapping of physical address to function group FQN
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00106
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08068
    /// @needwork = dda
    /// @endcode
    core::Map< int32_t, core::Vector< core::String > > physicalAddr2FgFQNs_;

    /// @brief Mapping of functional address to function group FQN
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00106
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08069
    /// @needwork = dda
    /// @endcode
    core::Map< int32_t, core::Vector< core::String > > functionalAddr2FgFQNs_;

    /// @brief FQN of MachineFG
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00101
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08070
    /// @needwork = dda
    /// @endcode
    core::String machineFgFQN_;
};

}  // namespace config
}  // namespace sm
}  // namespace ara

#endif  // FG_INFO_CONFIG_H_
