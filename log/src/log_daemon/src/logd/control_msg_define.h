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
/// @file       control_msg_define.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLogd
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00003,LOG_SR_00009
/// @unit_name = control_msg_define
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#ifndef __control_msg_define__
#define __control_msg_define__
#include <cstdint>
#include <string>

/// @brief Length of ID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02438
/// @trace_id_dd=DD_TSYNC_02837
/// @needwork = ad
/// @endcode
static std::uint8_t const kDlt_Control_Id_Size{4U};

/// @brief Receive type definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02439
/// @trace_id_dd=DD_TSYNC_02838
/// @needwork = ad
/// @endcode
enum class DltReceiverType : std::int32_t
{
    /// @brief DLT_RECEIVE_SOCKET
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02417
    /// @trace_id_dd=DD_TSYNC_02816
    /// @needwork = ad
    /// @endcode
    kDlt_Receive_Socket,

    /// @brief DLT_RECEIVE_UDP_SOCKET
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02418
    /// @trace_id_dd=DD_TSYNC_02817
    /// @needwork = ad
    /// @endcode
    kDlt_Receive_Udp_Socket,

    /// @brief DLT_RECEIVE_FD
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02419
    /// @trace_id_dd=DD_TSYNC_02818
    /// @needwork = ad
    /// @endcode
    kDlt_Receive_Fd
};

/// @brief The structure of the DLT Service Get Log Info.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02440
/// @trace_id_dd=DD_TSYNC_02839
/// @needwork = ad
/// @endcode
struct DltServiceGetLogInfoRequest
{
    /// @brief  service ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01660
    /// @needwork = dda
    /// @endcode
    std::uint32_t serviceId;

    /// @brief  type of request
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01661
    /// @needwork = dda
    /// @endcode
    std::int8_t options;

    /// @brief application id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01662
    /// @needwork = dda
    /// @endcode
    std::uint8_t apid[kDlt_Control_Id_Size];

    /// @brief context id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01663
    /// @needwork = dda
    /// @endcode
    std::uint8_t ctid[kDlt_Control_Id_Size];

    /// @brief  communication interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01664
    /// @needwork = dda
    /// @endcode
    std::uint8_t com[kDlt_Control_Id_Size];
};

/// @brief The structure of the DLT Service Set Log Level.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02441
/// @trace_id_dd=DD_TSYNC_02840
/// @needwork = ad
/// @endcode
struct DltServiceSetLogLevel
{
    /// @brief service ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01666
    /// @needwork = dda
    /// @endcode
    std::uint32_t serviceId;

    /// @brief application id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01667
    /// @needwork = dda
    /// @endcode
    std::uint8_t apid[kDlt_Control_Id_Size];

    /// @brief context id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01668
    /// @needwork = dda
    /// @endcode
    std::uint8_t ctid[kDlt_Control_Id_Size];

    /// @brief  log level to be set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01669
    /// @needwork = dda
    /// @endcode
    std::int8_t logLevel;

    /// @brief  communication interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01670
    /// @needwork = dda
    /// @endcode
    std::uint8_t com[kDlt_Control_Id_Size];
};

/// @brief The structure of the DLT Service Set Default Log Level.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02442
/// @trace_id_dd=DD_TSYNC_02841
/// @needwork = ad
/// @endcode
struct DltServiceSetDefaultLogLevel
{
    /// @brief service ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01671
    /// @needwork = dda
    /// @endcode
    std::uint32_t serviceId;

    /// @brief default log level to be set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01672
    /// @needwork = dda
    /// @endcode
    std::int8_t logLevel;

    /// @brief communication interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01673
    /// @needwork = dda
    /// @endcode
    std::uint8_t com[kDlt_Control_Id_Size];
};

/// @brief
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02443
/// @trace_id_dd=DD_TSYNC_02842
/// @needwork = ad
/// @endcode
struct DltServiceResponse
{
    /// @brief service_id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01674
    /// @needwork = dda
    /// @endcode
    std::uint32_t serviceId;

    /// @brief status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01675
    /// @needwork = dda
    /// @endcode
    std::uint8_t status;
};

/// @brief
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02444
/// @trace_id_dd=DD_TSYNC_02843
/// @needwork = ad
/// @endcode
struct DltServiceGetDefaultLogLevelResponse
{
    /// @brief service_id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01676
    /// @needwork = dda
    /// @endcode
    std::uint32_t serviceId;

    /// @brief status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01677
    /// @needwork = dda
    /// @endcode
    std::uint8_t status;

    /// @brief log_level
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01678
    /// @needwork = dda
    /// @endcode
    std::uint8_t logLevel;
};

/// @brief
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02445
/// @trace_id_dd=DD_TSYNC_02844
/// @needwork = ad
/// @endcode
struct DltServiceGetSoftwareVersion
{
    /// @brief service_id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01679
    /// @needwork = dda
    /// @endcode
    std::uint32_t serviceId;
};

/// @brief
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02446
/// @trace_id_dd=DD_TSYNC_02845
/// @needwork = ad
/// @endcode
struct DltServiceGetSoftwareVersionResponse
{
    /// @brief  service_id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01680
    /// @needwork = dda
    /// @endcode
    std::uint32_t serviceId;

    /// @brief status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01681
    /// @needwork = dda
    /// @endcode
    std::uint8_t status;

    /// @brief length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01682
    /// @needwork = dda
    /// @endcode
    std::uint32_t length;

    /// @brief payload
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01683
    /// @needwork = dda
    /// @endcode
    std::uint8_t* payload;
};

/// @brief The structure is used to organise the receiving of data  including buffer handling.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02447
/// @trace_id_dd=DD_TSYNC_02846
/// @needwork = ad
/// @endcode
struct DltReceiver
{
    /// @brief  bytes received in last receive call
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01684
    /// @needwork = dda
    /// @endcode
    std::uint32_t lastBytesRcvd{0};

    /// @brief received bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01685
    /// @needwork = dda
    /// @endcode
    std::uint32_t bytesRcvd{0};

    /// @brief  total number of received bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01686
    /// @needwork = dda
    /// @endcode
    std::uint32_t totalBytesRcvd{0};

    /// @brief pointer to receiver buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01687
    /// @needwork = dda
    /// @endcode
    std::uint8_t* buffer{nullptr};

    /// @brief pointer to position within receiver buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01688
    /// @needwork = dda
    /// @endcode
    std::uint8_t* buf{nullptr};

    /// @brief pointer to the buffer with partial messages if any
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01689
    /// @needwork = dda
    /// @endcode
    std::uint8_t* backupBuffer{nullptr};

    /// @brief connection handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01690
    /// @needwork = dda
    /// @endcode
    std::uint32_t fd{0};

    /// @brief type of connection handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_00000
    /// @trace_id_dd=DD_TSYNC_02847
    /// @needwork = dda
    /// @endcode
    DltReceiverType type{DltReceiverType::kDlt_Receive_Socket};

    /// @brief size of receiver buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01692
    /// @needwork = dda
    /// @endcode
    std::uint32_t buffersize{0};
};
#endif