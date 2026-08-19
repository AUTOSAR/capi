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
/// @file       nmmsg.h
/// @brief      NM message management
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @interface_level=/NetworkManager/protcl
/// @module_path=/NetworkManager/protcl
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00001,SRS_NM_00002
/// @unit_name=NmMsg
/// @unit_description=NM message management
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_NMMSG_H_
#define _ARA_NM_NMMSG_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <memory>

#include "common/common.h"
#include "config/include/configure.h"
#include "ethersocket.h"
#include "utils/include/utils.h"
namespace ara {
namespace nm {
namespace internal {
/// @brief Callback function type for handling received NM messages
/// @param pMsgBuffer -  NM message pointer
/// @param msgLenth -  Message length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100000
/// @trace_id_dd=DD_NM_00918
/// @needwork = ad
/// @endcode
using ValidMsgProcesHandler = std::function< void(std::uint8_t const nodeId,
                                                  bool const repeatMessageBitIndication,
                                                  ara::core::Vector< std::uint16_t > const &passiveValidPnGroup) >;

/// @brief ethernet communicator state machine
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100099
/// @trace_id_dd=DD_NM_00844
/// @needwork = ad
/// @endcode
class NmMsg final
{
public:
    /// @brief constructor of NmMsg.
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00687
    /// @needwork = dda
    /// @endcode
    NmMsg() = default;

    /// @brief copy constructor is forbidden.
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00688
    /// @needwork = dda
    /// @endcode
    NmMsg(NmMsg const &other) = delete;

    /// @brief copy operator is forbidden.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00689
    /// @needwork = dda
    /// @endcode
    NmMsg &operator=(NmMsg const &other) = delete;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00690
    /// @needwork = dda
    /// @endcode
    NmMsg(NmMsg &&other) = default;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00691
    /// @needwork = dda
    /// @endcode
    NmMsg &operator=(NmMsg &&other) = default;

    /// @brief destructor of NmMsg.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00692
    /// @needwork = dda
    /// @endcode
    ~NmMsg() = default;

    /// @brief DeInit of NmMsg.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00693
    /// @needwork = dda
    /// @endcode
    void DeInit() noexcept;

    /// @brief Encapsulate and send NM messages.
    /// @param detectNode Whether it is node detection
    /// @param pnGroup   The set of partial networks currently in use by this node
    /// @param userDataNMState   The value of NMState
    /// @param activeWakeUpSet Whether to set the activeWakeUp bit in CBV
    /// @returns  kNmOperOK ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00694
    /// @needwork = dda
    /// @endcode
    std::int32_t SendNmMessage(bool const detectNode,
                               ara::core::Vector< std::uint16_t > const &pnGroup,
                               std::uint8_t const userDataNMState,
                               bool const activeWakeUpSet) noexcept;

    /// @brief content initialization.
    /// @param connectorName  ethernet communicator name
    /// @param pConfigRefTmp      json configure reference
    /// @returns               0 ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00695
    /// @needwork = dda
    /// @endcode
    std::int32_t Init(ara::core::String const &connectorName,
                      std::shared_ptr< Configure > const &pConfigRefTmp) noexcept;

    /// @brief Register NM message processing callback interface.
    /// @param msgCallBack receive message callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00696
    /// @needwork = dda
    /// @endcode
    void RegistMsgProceHandler(ValidMsgProcesHandler const &msgCallBack) noexcept { msgCallBack_ = msgCallBack; };

private:
    /// @brief process of received nm message.
    /// @param buffer message buffer
    /// @param bufferLen buffer length
    /// @returns kNmOperOK ok
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00008 SWS_ANM_00019 SWS_ANM_00025 SWS_ANM_00051
    /// SWS_ANM_00081 SWS_ANM_00089 SWS_ANM_00040 SWS_ANM_00055 SWS_ANM_00085}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00697
    /// @needwork = dda
    /// @endcode
    NmOperCode _recvNmMessage(std::uint8_t const buffer[], std::uint32_t const bufferLen) noexcept;

private:
    /// @brief connectorName_
    /// ethernet communication name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00698
    /// @needwork = dda
    /// @endcode
    ara::core::String connectorName_{};

    /// @brief pEtherUdpNmconfig_
    /// pointer of configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00699
    /// @needwork = dda
    /// @endcode
    NmEthernetUdpNmNodeConfig const *pEtherUdpNmconfig_{nullptr};

    /// @brief pSndBuffer_
    /// message send buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00700
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< std::uint8_t[] > pSndBuffer_{nullptr};

    /// @brief pEtherSocket_
    /// pointer of ethernet socket handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00701
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< EtherSocket > pEtherSocket_{nullptr};

    /// @brief configPnGroup_
    /// collection of configured pn
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00702
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< std::uint16_t > configPnGroup_{};

    /// @brief pConfigRef_
    /// message send buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00703
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< Configure > pConfigRef_{nullptr};

    /// @brief msgCallBack_
    /// message process callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00704
    /// @needwork = dda
    /// @endcode
    ValidMsgProcesHandler msgCallBack_{};

    /// @brief detectNode_
    /// Whether to start node detection. Once started, when an NM message is received and the PN part is empty, report node information to the upper layer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00705
    /// @needwork = dda
    /// @endcode
    bool detectNode_{false};
};

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  //_ARA_NM_NMMSG_H_