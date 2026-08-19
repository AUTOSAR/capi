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
/// @file       configure.h
/// @brief      Configuration file management
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/config
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=Configure
/// @unit_description=Configuration file management
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_CONFIGURE_CONFIGURE_H_
#define _ARA_NM_CONFIGURE_CONFIGURE_H_
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/define.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include "nmdefaultpara.h"
#include "nmethernetudpnmnodeconfig.h"
#include "nmlogichandle.h"
#include "nmpnhandle.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief network-management configuration class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100052
/// @trace_id_dd=DD_NM_00791
/// @needwork = ad
/// @endcode
class Configure final
{
public:
    /// @brief constructor of Configure.
    /// @throws  Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00297
    /// @needwork = dda
    /// @endcode
    Configure() = default;

    /// @brief copy constructor is default.
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00298
    /// @needwork = dda
    /// @endcode
    Configure(Configure const &other) = default;

    /// @brief copy operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00299
    /// @needwork = dda
    /// @endcode
    Configure &operator=(Configure const &other) = default;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00300
    /// @needwork = dda
    /// @endcode
    Configure(Configure &&other) = default;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00301
    /// @needwork = dda
    /// @endcode
    Configure &operator=(Configure &&other) = default;

    /// @brief Destructor of NmLogicHandleConfig.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00302
    /// @needwork = dda
    /// @endcode
    ~Configure() = default;

public:
    /// @brief load network-management configuration
    /// @param manifestPath configuration file
    /// @returns 0 ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00303
    /// @needwork = dda
    /// @endcode
    std::int32_t Load(ara::core::String const &manifestPath) noexcept;

    /// @brief get all logic network configuration
    /// @returns configuration reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00304
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< NmLogicHandleConfig > const &GetAllLoicHandle() const noexcept { return logicHandleGroup_; }

    /// @brief get all partial network configuration
    /// @returns configuration reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00305
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< NmPnHandleConfig > const &GetAllPn() const noexcept { return pnGroup_; }

    /// @brief get all ethernet connector configuration
    /// @returns configuration reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00306
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< NmEthernetUdpNmNodeConfig > const &GetAllEther() const noexcept
    {
        return ethernetUdpNmNodeGroup_;
    }

    /// @brief get single ethernet connector configuration
    /// @param connectorName network connector name
    /// @returns configuration reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00307
    /// @needwork = dda
    /// @endcode
    NmEthernetUdpNmNodeConfig const *GetEtherConfig(ara::core::String const &connectorName) noexcept;

    /// @brief Get diagnostic DID instance configuration
    /// @returns Diagnostic DID instance configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00308
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetDidInstance() const noexcept { return defaultPara_.didInstance; }

    /// @brief Get diagnostic monitor instance configuration
    /// @returns Diagnostic monitor instance configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00309
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetDiagMonitorId() const noexcept { return defaultPara_.diagMonitorId; }

    /// @brief Get diagnostic opertation cycle instance configuratio
    /// @returns opertation cycle instance configuratio
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00309
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetDiagOperCycleInstance() const noexcept
    {
        return defaultPara_.diagOperationCycleInstance;
    }

    /// @brief get partial network configurations of ethernet connector
    /// @param connectorName ethernet connector name
    /// @param configPnGroup partial network configuration reference
    /// @returns true valid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00310
    /// @needwork = dda
    /// @endcode
    bool GetPnsOfEthernet(ara::core::String const &connectorName,
                          ara::core::Vector< std::uint16_t > &configPnGroup) noexcept;

    /// @brief get pn offset in nm message
    /// @returns defaultNmPnInfoOffset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00311
    /// @needwork = dda
    /// @endcode
    std::uint32_t GetDefaultNmPnInfoOffset() const noexcept
    {
        return static_cast< std::uint32_t >(defaultPara_.defaultNmPnInfoOffset);
    }

    /// @brief get PnResetTime for pn state-machine
    /// @returns defaultNmPnResetTime
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00312
    /// @needwork = dda
    /// @endcode
    double GetDefaultNmPnResetTime() const noexcept { return defaultPara_.defaultNmPnResetTime; }

    /// @brief get PncPrepareSleepTime for pn state-machine
    /// @returns defaultPncPrepareSleepTime
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00313
    /// @needwork = dda
    /// @endcode
    double GetDefaultPncPrepareSleepTime() const noexcept { return defaultPara_.defaultPncPrepareSleepTime; }

private:
    /// @brief existence-check of partial network
    /// @param pnHandle partial network name
    /// @param connectList connector list
    /// @returns true valid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00314
    /// @needwork = dda
    /// @endcode
    bool _getPnEtherConnectorsList(ara::core::String const &pnHandle,
                                   ara::core::Vector< ara::core::String > &connectList) noexcept;

    /// @brief existence-check of partial network
    /// @param pnHandle partial network name
    /// @returns true valid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00315
    /// @needwork = dda
    /// @endcode
    bool _checkPnExist(ara::core::String const &pnHandle) noexcept;

    /// @brief if ethernet connector belong to pn
    /// @param connectorName ethernet connector name
    /// @returns true belong
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00316
    /// @needwork = dda
    /// @endcode
    bool _belongToPn(ara::core::String const &connectorName) const noexcept;

    /// @brief existence-check of ethernet-connector
    /// @param connectorName ethernet-connector name
    /// @returns true valid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00317
    /// @needwork = dda
    /// @endcode
    bool _checkEtherConnectorExist(ara::core::String const &connectorName) noexcept;

    /// @brief check configuration
    /// @returns 0 valid
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00066}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00318
    /// @needwork = dda
    /// @endcode
    std::int32_t _checkConfig() noexcept;

    /// @brief check configuration
    /// @returns 0 valid
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00066}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00319
    /// @needwork = dda
    /// @endcode
    std::int32_t _checkLNConfig() noexcept;

    /// @brief check configuration
    /// @returns 0 valid
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00066}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00320
    /// @needwork = dda
    /// @endcode
    std::int32_t _checkPNConfig() noexcept;

    /// @brief validation-check of ethernet-connector array
    /// @returns true valid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00321
    /// @needwork = dda
    /// @endcode
    bool _setDefaultAndCheckEtherConfig() noexcept;

    /// @brief validation-check of ethernet-connector array
    /// @param ref ethernet-connector reference
    /// @returns true valid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00322
    /// @needwork = dda
    /// @endcode
    bool _checketherTimerConfig(NmEthernetUdpNmNodeConfig const &ref) const noexcept;

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00323
    /// @needwork = dda
    /// @endcode
    void _debug() const noexcept;

private:
    /// @brief defaultPara_
    /// default parameter.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00324
    /// @needwork = dda
    /// @endcode
    NmDefaultPara defaultPara_;

    /// @brief logicHandleGroup_
    /// all logic network configurations.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00325
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< NmLogicHandleConfig > logicHandleGroup_;

    /// @brief pnGroup_
    /// all partial network configurations.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00326
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< NmPnHandleConfig > pnGroup_;

    /// @brief ethernetUdpNmNodeGroup_
    /// all ethernet connector configurations.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00327
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< NmEthernetUdpNmNodeConfig > ethernetUdpNmNodeGroup_;

};  // class Configure
}  // namespace internal
}  // namespace nm
}  // namespace ara
#endif /* _ARA_NM_CONFIGURE_CONFIGURE_H_ */
