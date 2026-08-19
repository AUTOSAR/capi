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
/// @file       transfer_managment.h
/// @brief      This file provides transfer_managment functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/transfer_managment
/// @interface_level=software
/// @trace_id_sr=766d2813-362b-4d96-8423-c03d79e6c0f3
/// @unit_name=TransferManagment
/// @unit_description=This file provides transfer_managment functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICREQUESTUPLOAD_H_
#define ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICREQUESTUPLOAD_H_
#include <memory>

#include "download.h"
#include "file_transfer.h"
#include "isoft/uds/authentication_management/authentication_state.h"
#include "isoft/uds/common.h"
#include "isoft/uds/data_management/environmental_condition_model.h"
#include "isoft/uds/security_access_management/security_level.h"
#include "isoft/uds/session_management/session_model.h"
#include "upload.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Download request configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01324
/// @trace_id_dd=DD_UDS_02616
/// @needwork = ad
/// @endcode
struct RequestDownload : public ServiceConfigBase
{
    /// @brief Maximum block length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01011
    /// @trace_id_dd=DD_UDS_02125
    /// @needwork = ad
    /// @endcode
    std::uint32_t maxNumberOfBlockLength{0U};
    /// @brief Download interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01012
    /// @trace_id_dd=DD_UDS_02126
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< DownloadInterface > interfacePtr{nullptr};
    /// @brief Access permission
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01013
    /// @trace_id_dd=DD_UDS_02127
    /// @needwork = ad
    /// @endcode
    EnableAuth accessPermissionAuth{};
    /// @brief Access session ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01014
    /// @trace_id_dd=DD_UDS_02128
    /// @needwork = ad
    /// @endcode
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access security level ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01015
    /// @trace_id_dd=DD_UDS_02129
    /// @needwork = ad
    /// @endcode
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Access environmental condition ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01016
    /// @trace_id_dd=DD_UDS_02130
    /// @needwork = ad
    /// @endcode
    EnvironmentalConditionId accessPermissionEnvCondition{0};
    /// @brief P4 timer in 0x36 service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01017
    /// @trace_id_dd=DD_UDS_02131
    /// @needwork = ad
    /// @endcode
    std::uint32_t p4ServerMax_0x36{0U};
    /// @brief P4 timer in 0x37 service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01018
    /// @trace_id_dd=DD_UDS_02132
    /// @needwork = ad
    /// @endcode
    std::uint32_t p4ServerMax_0x37{0U};
};

/// @brief File transfer request configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01325
/// @trace_id_dd=DD_UDS_02617
/// @needwork = ad
/// @endcode
struct RequestFileTransfer : public ServiceConfigBase
{
    /// @brief Maximum block length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork /// @endcode
    /// @endcode
    std::uint32_t maxNumberOfBlockLength{0U};
    /// @brief File transfer interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01019
    /// @trace_id_dd=DD_UDS_02133
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< FileTransferInterface > interfacePtr{nullptr};
    /// @brief Access authentication permission
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01020
    /// @trace_id_dd=DD_UDS_02134
    /// @needwork = ad
    /// @endcode
    EnableAuth accessPermissionAuth{};
    /// @brief Access session ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01021
    /// @trace_id_dd=DD_UDS_02135
    /// @needwork = ad
    /// @endcode
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access security level ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01022
    /// @trace_id_dd=DD_UDS_02136
    /// @needwork = ad
    /// @endcode
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Access environmental condition ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01023
    /// @trace_id_dd=DD_UDS_02137
    /// @needwork = ad
    /// @endcode
    EnvironmentalConditionId accessPermissionEnvCondition{0};
    /// @brief P4 timer in 0x36 service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01024
    /// @trace_id_dd=DD_UDS_02138
    /// @needwork = ad
    /// @endcode
    std::uint32_t p4ServerMax_0x36{0U};
    /// @brief P4 timer in 0x37 service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01025
    /// @trace_id_dd=DD_UDS_02139
    /// @needwork = ad
    /// @endcode
    std::uint32_t p4ServerMax_0x37{0U};
};

/// @brief Upload request configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01326
/// @trace_id_dd=DD_UDS_02618
/// @needwork = ad
/// @endcode
struct RequestUpload : public ServiceConfigBase
{
    /// @brief Maximum block length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01026
    /// @trace_id_dd=DD_UDS_02140
    /// @needwork = ad
    /// @endcode
    std::uint32_t maxNumberOfBlockLength{0U};
    /// @brief Upload interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01027
    /// @trace_id_dd=DD_UDS_02141
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< UploadInterface > interfacePtr{nullptr};
    /// @brief Access  authentication permission
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01028
    /// @trace_id_dd=DD_UDS_02142
    /// @needwork = ad
    /// @endcode
    EnableAuth accessPermissionAuth{};
    /// @brief Access session ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01029
    /// @trace_id_dd=DD_UDS_02143
    /// @needwork = ad
    /// @endcode
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access security level ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01030
    /// @trace_id_dd=DD_UDS_02144
    /// @needwork = ad
    /// @endcode
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Access environmental condition ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01031
    /// @trace_id_dd=DD_UDS_02145
    /// @needwork = ad
    /// @endcode
    EnvironmentalConditionId accessPermissionEnvCondition{0};
    /// @brief P4 timer in 0x36 service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01032
    /// @trace_id_dd=DD_UDS_02146
    /// @needwork = ad
    /// @endcode
    std::uint32_t p4ServerMax_0x36{0U};
    /// @brief P4 timer in 0x37 service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01033
    /// @trace_id_dd=DD_UDS_02147
    /// @needwork = ad
    /// @endcode
    std::uint32_t p4ServerMax_0x37{0U};
};

/// @brief Transfer management configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01327
/// @trace_id_dd=DD_UDS_02619
/// @needwork = ad
/// @endcode
struct TransferManagment
{
    /// @brief Download request configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01034
    /// @trace_id_dd=DD_UDS_02148
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< RequestDownload > requestDownload{nullptr};
    /// @brief Upload request configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01035
    /// @trace_id_dd=DD_UDS_02149
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< RequestUpload > requestUpload{nullptr};
    /// @brief File transfer request configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01036
    /// @trace_id_dd=DD_UDS_02150
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< RequestFileTransfer > requestFileTransfer{nullptr};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  ///ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICREQUESTUPLOAD_H_