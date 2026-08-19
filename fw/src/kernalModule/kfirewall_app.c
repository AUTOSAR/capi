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
/// @file       kfirewall_app.c
/// @brief      Firewall application layer implementation
/// @details    Firewall application layer implementation
/// @date       2025-09-12
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Application layer protocol
/// @interface_level=unit
/// export_level=module
/// @trace_id_sr=SR_FW_0005
/// @unit_name=Kfirewall_app
/// @unit_description=Firewall application layer definition.
/// @endcode
///
/// ================================================================
///
/// clang-format off
///
/// ================================================================

#include <linux/byteorder/generic.h>
#include <linux/init.h>
#include <linux/ip.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/tcp.h>
#include <linux/udp.h>
// clang-format on
#include "protocoldefine.h"
MODULE_LICENSE("GPL");
MODULE_AUTHOR("i-soft");
MODULE_DESCRIPTION("i-soft application firewall");
MODULE_VERSION("0.1");
static DEFINE_RWLOCK(firewall_lock);

/// @brief action  true: 1  false: 0  default: -1
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00537
/// @trace_id_dd=DD_FW_00952
/// @needwork = dd
/// @endcode
static s32 g_Action = -1;
module_param(g_Action, int, 0644);

/// @brief action  true: 1  false: 0  default: -1
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00538
/// @trace_id_dd=DD_FW_00953
/// @needwork = dd
/// @endcode
static s32 g_DefaultAction = -1;
module_param(g_DefaultAction, int, 0644);

// someip protocol Rule
/// @brief someip service id
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00539
/// @trace_id_dd=DD_FW_00954
/// @needwork = dd
/// @endcode
static s32 g_Target_Service_Id_In = -1;
module_param(g_Target_Service_Id_In, int, 0644);

/// @brief clientId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00540
/// @trace_id_dd=DD_FW_00955
/// @needwork = dd
/// @endcode
static s32 g_ClientId_In = -1;
module_param(g_ClientId_In, int, 0644);

/// @brief methodId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00541
/// @trace_id_dd=DD_FW_00956
/// @needwork = dd
/// @endcode
static s32 g_Target_Method_Id_In = -1;
module_param(g_Target_Method_Id_In, int, 0644);

/// @brief length  type bool 1:check  0 :no check
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00542
/// @trace_id_dd=DD_FW_00957
/// @needwork = dd
/// @endcode
static s32 g_Target_Length_Flag_In = -1;
module_param(g_Target_Length_Flag_In, int, 0644);

/// @brief major version.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00543
/// @trace_id_dd=DD_FW_00958
/// @needwork = dd
/// @endcode
static s32 g_Target_Major_Version_In = -1;
module_param(g_Target_Major_Version_In, int, 0644);

/// @brief msg type
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00544
/// @trace_id_dd=DD_FW_00959
/// @needwork = dd
/// @endcode
static s32 g_Target_Msg_Type_In = -1;
module_param(g_Target_Msg_Type_In, int, 0644);

/// @brief protocol version
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00545
/// @trace_id_dd=DD_FW_00960
/// @needwork = dd
/// @endcode
static s32 g_Target_Pro_Version_In = -1;
module_param(g_Target_Pro_Version_In, int, 0644);

/// @brief return code
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00546
/// @trace_id_dd=DD_FW_00961
/// @needwork = dd
/// @endcode
static s32 g_Target_Ret_Code_In = -1;
module_param(g_Target_Ret_Code_In, int, 0644);

// someip sd Rule

/// @brief Entity Type
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00547
/// @trace_id_dd=DD_FW_00962
/// @needwork = dd
/// @endcode
static s32 g_Sd_EntType_In = -1;
module_param(g_Sd_EntType_In, int, 0644);

/// @brief GroupId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00548
/// @trace_id_dd=DD_FW_00963
/// @needwork = dd
/// @endcode
static s32 g_Sd_GroupId_In = -1;
module_param(g_Sd_GroupId_In, int, 0644);

/// @brief MajorVersionMax
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00549
/// @trace_id_dd=DD_FW_00964
/// @needwork = dd
/// @endcode
static s32 g_Sd_MaxMajorVersion_In = -1;
module_param(g_Sd_MaxMajorVersion_In, int, 0644);

/// @brief MajorVersionMin
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00549
/// @trace_id_dd=DD_FW_00964
/// @needwork = dd
/// @endcode
static s32 g_Sd_MinMajorVersion_In = -1;
module_param(g_Sd_MinMajorVersion_In, int, 0644);

/// @brief MajorVersionMin
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00550
/// @trace_id_dd=DD_FW_00965
/// @needwork = dd
/// @endcode
static s32 g_Sd_MinMinorVersion_In = -1;
module_param(g_Sd_MinMinorVersion_In, int, 0644);

/// @brief MajorVersionMax
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00551
/// @trace_id_dd=DD_FW_00966
/// @needwork = dd
/// @endcode
static s32 g_Sd_MaxMinorVersion_In = -1;
module_param(g_Sd_MaxMinorVersion_In, int, 0644);

/// @brief serviceInstance Id
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00552
/// @trace_id_dd=DD_FW_00967
/// @needwork = dd
/// @endcode
static s32 g_Sd_ServiceInstId_In = -1;
module_param(g_Sd_ServiceInstId_In, int, 0644);

/// @brief service Id
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00553
/// @trace_id_dd=DD_FW_00968
/// @needwork = dd
/// @endcode
static s32 g_Sd_ServiceId_In = -1;
module_param(g_Sd_ServiceId_In, int, 0644);

// doip  rule
/// @brief  dest max addr
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00554
/// @trace_id_dd=DD_FW_00969
/// @needwork = dd
/// @endcode
static s32 g_DestMaxAddr_In = -1;
module_param(g_DestMaxAddr_In, int, 0644);

/// @brief  dest min addr
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00555
/// @trace_id_dd=DD_FW_00970
/// @needwork = dd
/// @endcode
static s32 g_DestMinAddr_In = -1;
module_param(g_DestMinAddr_In, int, 0644);

/// @brief  inverProtocolVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00556
/// @trace_id_dd=DD_FW_00971
/// @needwork = dd
/// @endcode
static s32 g_InverProtocolVersion_In = -1;
module_param(g_InverProtocolVersion_In, int, 0644);

/// @brief  PayLoadLen
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00557
/// @trace_id_dd=DD_FW_00972
/// @needwork = dd
/// @endcode
static s32 g_PayLoadLen_In = -1;
module_param(g_PayLoadLen_In, int, 0644);

/// @brief dest g_PayType_In
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00558
/// @trace_id_dd=DD_FW_00973
/// @needwork = dd
/// @endcode
static s32 g_PayType_In = -1;
module_param(g_PayType_In, int, 0644);

/// @brief ProtocolVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00559
/// @trace_id_dd=DD_FW_00974
/// @needwork = dd
/// @endcode
static s32 g_ProtocolVersion_In = -1;
module_param(g_ProtocolVersion_In, int, 0644);

/// @brief src max addr
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00560
/// @trace_id_dd=DD_FW_00975
/// @needwork = dd
/// @endcode
static s32 g_SrcMaxAddr_In = -1;
module_param(g_SrcMaxAddr_In, int, 0644);

/// @brief src min addr
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00561
/// @trace_id_dd=DD_FW_00976
/// @needwork = dd
/// @endcode
static s32 g_SrcMinAddr_In = -1;
module_param(g_SrcMinAddr_In, int, 0644);

/// @brief uds Service Id
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00562
/// @trace_id_dd=DD_FW_00977
/// @needwork = dd
/// @endcode
static s32 g_UdsService_In = -1;
module_param(g_UdsService_In, int, 0644);

// //in:1 out 0
// static s32 g_InOutFlag = -1;
// module_param(g_InOutFlag, int, 0644);

/// @brief someip protocol Rule
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00563
/// @trace_id_dd=DD_FW_00978
/// @needwork = dd
/// @endcode
static s32 g_Target_Service_Id_Out = -1;
module_param(g_Target_Service_Id_Out, int, 0644);

/// @brief clientId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00564
/// @trace_id_dd=DD_FW_00979
/// @needwork = dd
/// @endcode
static s32 g_ClientId_Out = -1;
module_param(g_ClientId_Out, int, 0644);

/// @brief methodId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00565
/// @trace_id_dd=DD_FW_00980
/// @needwork = dd
/// @endcode
static s32 g_Target_Method_Id_Out = -1;
module_param(g_Target_Method_Id_Out, int, 0644);

/// @brief length  type bool 1:check  0 :no check
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00566
/// @trace_id_dd=DD_FW_00981
/// @needwork = dd
/// @endcode
static s32 g_Target_Length_Flag_Out = -1;
module_param(g_Target_Length_Flag_Out, int, 0644);

/// @brief major version.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00567
/// @trace_id_dd=DD_FW_00982
/// @needwork = dd
/// @endcode
static s32 g_Target_Major_Version_Out = -1;
module_param(g_Target_Major_Version_Out, int, 0644);

/// @brief msg type
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00568
/// @trace_id_dd=DD_FW_00983
/// @needwork = dd
/// @endcode
static s32 g_Target_Msg_Type_Out = -1;
module_param(g_Target_Msg_Type_Out, int, 0644);

/// @brief protocol version
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00569
/// @trace_id_dd=DD_FW_00984
/// @needwork = dd
/// @endcode
static s32 g_Target_Pro_Version_Out = -1;
module_param(g_Target_Pro_Version_Out, int, 0644);

/// @brief  return code
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00569
/// @trace_id_dd=DD_FW_00984
/// @needwork = dd
/// @endcode
static s32 g_Target_Ret_Code_Out = -1;
module_param(g_Target_Ret_Code_Out, int, 0644);

// someip sd Rule

/// @brief Entity Type
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00570
/// @trace_id_dd=DD_FW_00985
/// @needwork = dd
/// @endcode
static s32 g_Sd_EntType_Out = -1;
module_param(g_Sd_EntType_Out, int, 0644);

/// @brief GroupId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00571
/// @trace_id_dd=DD_FW_00986
/// @needwork = dd
/// @endcode
static s32 g_Sd_GroupId_Out = -1;
module_param(g_Sd_GroupId_Out, int, 0644);

/// @brief  MajorVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00572
/// @trace_id_dd=DD_FW_00987
/// @needwork = dd
/// @endcode
static s32 g_Sd_MaxMajorVersion_Out = -1;
module_param(g_Sd_MaxMajorVersion_Out, int, 0644);

/// @brief  MajorVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00573
/// @trace_id_dd=DD_FW_00988
/// @needwork = dd
/// @endcode
static s32 g_Sd_MinMajorVersion_Out = -1;
module_param(g_Sd_MinMajorVersion_Out, int, 0644);

/// @brief MinorVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00574
/// @trace_id_dd=DD_FW_00989
/// @needwork = dd
/// @endcode
static s32 g_Sd_MinMinorVersion_Out = -1;
module_param(g_Sd_MinMinorVersion_Out, int, 0644);

/// @brief MinorVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00575
/// @trace_id_dd=DD_FW_00990
/// @needwork = dd
/// @endcode
static s32 g_Sd_MaxMinorVersion_Out = -1;
module_param(g_Sd_MaxMinorVersion_Out, int, 0644);

/// @brief serviceInstance Id
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00576
/// @trace_id_dd=DD_FW_00991
/// @needwork = dd
/// @endcode
static s32 g_Sd_ServiceInstId_Out = -1;
module_param(g_Sd_ServiceInstId_Out, int, 0644);

/// @brief service Id
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00577
/// @trace_id_dd=DD_FW_00992
/// @needwork = dd
/// @endcode
static s32 g_Sd_ServiceId_Out = -1;
module_param(g_Sd_ServiceId_Out, int, 0644);

// doip  rule
/// @brief dest max addr
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00578
/// @trace_id_dd=DD_FW_00993
/// @needwork = dd
/// @endcode
static s32 g_DestMaxAddr_Out = -1;
module_param(g_DestMaxAddr_Out, int, 0644);

/// @brief dest min addr
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00579
/// @trace_id_dd=DD_FW_00994
/// @needwork = dd
/// @endcode
static s32 g_DestMinAddr_Out = -1;
module_param(g_DestMinAddr_Out, int, 0644);

/// @brief  inverProtocolVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00580
/// @trace_id_dd=DD_FW_00995
/// @needwork = dd
/// @endcode
static s32 g_InverProtocolVersion_Out = -1;
module_param(g_InverProtocolVersion_Out, int, 0644);

/// @brief PayLoadLen
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00581
/// @trace_id_dd=DD_FW_00996
/// @needwork = dd
/// @endcode
static s32 g_PayLoadLen_Out = -1;
module_param(g_PayLoadLen_Out, int, 0644);

/// @brief dest g_PayType_In
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00582
/// @trace_id_dd=DD_FW_00997
/// @needwork = dd
/// @endcode
static s32 g_PayType_Out = -1;
module_param(g_PayType_Out, int, 0644);

/// @brief ProtocolVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00583
/// @trace_id_dd=DD_FW_00998
/// @needwork = dd
/// @endcode
static s32 g_ProtocolVersion_Out = -1;
module_param(g_ProtocolVersion_Out, int, 0644);

// doip  rule
/// @brief src max addr
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00584
/// @trace_id_dd=DD_FW_00999
/// @needwork = dd
/// @endcode
static s32 g_SrcMaxAddr_Out = -1;
module_param(g_SrcMaxAddr_Out, int, 0644);

/// @brief src min addr
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00585
/// @trace_id_dd=DD_FW_01000
/// @needwork = dd
/// @endcode
static s32 g_SrcMinAddr_Out = -1;
module_param(g_SrcMinAddr_Out, int, 0644);

/// @brief uds Service Id
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00586
/// @trace_id_dd=DD_FW_01001
/// @needwork = dd
/// @endcode
static s32 g_UdsService_Out = -1;
module_param(g_UdsService_Out, int, 0644);

/// @brief Verify someip protocol header
/// @param hdr Someip protocol header structure
/// @param dataLen Data length
/// @return Yes/No
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00933
/// @needwork = dda
/// @endcode
static bool Validate_Someip_Header(Someip_Header_t *hdr, unsigned int dataLen)
{
    // someip version number is fixed 0x01
    // Check protocol version
    if (hdr->protocolVersion != SOMEIP_SD_PROTOCOL_VERSION) {
        // pr_info("Invalid someip protocolVersion: 0x%X\n", hdr->protocolVersion);
        return false;
    }

    // Check message type validity, filter all SOME/IP protocol types.
    if ((hdr->messageType != NSI_MT_REQUEST) && (hdr->messageType != NSI_MT_REQUEST_NO_RETURN)
        && (hdr->messageType != NSI_MT_NOTIFICATION) && (hdr->messageType != NSI_MT_RESPONSE)
        && (hdr->messageType != NSI_MT_ERROR) && (hdr->messageType != NSI_TP_REQUEST)
        && (hdr->messageType != NSI_TP_REQUEST_NO_RETURN) && (hdr->messageType != NSI_TP_NOTIFICATION)
        && (hdr->messageType != NSI_TP_RESPONSE) && (hdr->messageType != NSI_TP_ERROR)) {
        // pr_info("Invalid someip messageType: 0x%X\n", hdr->messageType);
        return false;
    }
    // Check return code.
    if ((hdr->returnCode != NSI_E_OK) && (hdr->returnCode != NSI_E_NOT_OK) && (hdr->returnCode != NSI_E_UNKNOWN_SERVICE)
        && (hdr->returnCode != NSI_E_UNKNOWN_METHOD) && (hdr->returnCode != NSI_E_NOT_READY)
        && (hdr->returnCode != NSI_E_NOT_REACHABLE) && (hdr->returnCode != NSI_E_WRONG_PROTOCOL_VERSION)
        && (hdr->returnCode != NSI_E_WRONG_INTERFACE_VERSION) && (hdr->returnCode != NSI_E_MALFORMED_MESSAGE)
        && (hdr->returnCode != NSI_E_WRONG_MESSAGE_TYPE)) {
        // pr_info("Invalid someip returnCode: 0x%X\n", hdr->returnCode);
        return false;
    }

    pr_info("Validate_Someip_Header: the protocol is someip!\n");
    pr_info("SOME/IP packet: Service ID = 0x%X\n", be16_to_cpu(hdr->serviceId));
    pr_info("SOME/IP packet: method ID = 0x%X\n", be16_to_cpu(hdr->methodId));
    pr_info("SOME/IP protocol length: 0x%X\n", be32_to_cpu(hdr->length));
    pr_info("SOME/IP protocol clientId: 0x%X\n", be16_to_cpu(hdr->clientId));
    pr_info("SOME/IP protocol sessionId: 0x%X\n", be16_to_cpu(hdr->sessionId));
    pr_info("SOME/IP protocol protocolVersion: 0x%X\n", hdr->protocolVersion);
    pr_info("SOME/IP protocol interfaceVersion: 0x%X\n", hdr->interfaceVersion);
    pr_info("SOME/IP protocol messageType: 0x%X\n", hdr->messageType);
    pr_info("SOME/IP protocol returnCode: 0x%X\n", hdr->returnCode);
    return true;
}

/// @brief Check SOME/IP protocol header validity
/// @param data Payload data
/// @param dataLen Data length
/// @return Yes/No
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00934
/// @needwork = dda
/// @endcode
static bool Validate_Doip_Header(u8 *data, unsigned int dataLen)
{
    u16 Type;
    u32 payloadLen;
    Doip_Header_t *hdr = NULL;
    hdr                = (Doip_Header_t *)(data);
    if (hdr == NULL || dataLen < sizeof(Doip_Header_t)) {
        return false;
    }
    Type       = be16_to_cpu(hdr->payloadType);
    payloadLen = be32_to_cpu(hdr->payloadLen);

    // Check protocol version and inverse version && check payload type validity,
    // must be within 3 ranges: 0x00~0x008 0x4001~0x4004 0x8001~0x8003 payloadLen
    // length cannot exceed TCP
    // UDP maximum payload length (added TCP/UDP header)
    if (((hdr->protocolVersion == DOIP_PROTOCOL_VERSION_02 && hdr->inverseVersion == DOIP_INVERSE_VERSION_02)
         || (hdr->protocolVersion == DOIP_PROTOCOL_VERSION_03 && hdr->inverseVersion == DOIP_INVERSE_VERSION_03))
        && ((Type >= DOIP_PAYLOAD_MIN_TYPE && Type <= DOIP_PAYLOAD_CHECK_TYPE08)
            || (Type >= DOIP_PAYLOAD_CHECK_TYPE4001 && Type <= DOIP_PAYLOAD_CHECK_TYPE4004)
            || (Type >= DOIP_PAYLOAD_CHECK_TYPE8001 && Type <= DOIP_PAYLOAD_CHECK_TYPE8003))
        && (payloadLen < 65535)) {
        return true;
    }

    return false;
}

/// @brief Check whether it is SOME/IP protocol
//// @param data Payload data
/// @param dataLen Data length
/// @return Yes/No
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00935
/// @needwork = dda
/// @endcode
static bool Is_Someip_Packet(u8 *data, unsigned int dataLen)
{
    Someip_Header_t *hdr = (Someip_Header_t *)data;

    if (hdr == NULL || dataLen < sizeof(Someip_Header_t))
        return false;

    return Validate_Someip_Header(hdr, dataLen);
}

/// @brief // Check whether it is DDS protocol
//// @param data Payload data
/// @param dataLen Data length
/// @return Yes/No
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00936
/// @needwork = dda
/// @endcode
static bool Is_Dds_Packet(const u8 *data, unsigned int dataLen) { return false; }

/// @brief // Check whether it is Doip protocol
//// @param data Payload data
/// @param dataLen Data length
/// @return Yes/No
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00937
/// @needwork = dda
/// @endcode
static bool Is_DoIp_Packet(u8 *data, unsigned int dataLen) { return Validate_Doip_Header(data, dataLen); }

/// @brief Doip performs special filtering for diagnostic messages
/// @param data Payload data
/// @param len Length
/// @param inOutFlag INPUT/OUTPUT
/// @return Success/Failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00938
/// @needwork = dda
/// @endcode
static int DoIp_Diag_Filter(const Doip_Header_t *data, int len, bool inOutFlag)
{
    Doip_Payload_t *paylodHdr = NULL;
    u16 destAddr              = 0;
    u16 srcAddr               = 0;
    int ruleCheck             = 1;

    // Check length
    if (!data || len < (sizeof(Doip_Payload_t) + sizeof(Doip_Header_t))) {
        return ruleCheck;
    }

    paylodHdr = (Doip_Payload_t *)(data + sizeof(Doip_Header_t));
    destAddr  = be16_to_cpu(paylodHdr->destAddr);
    srcAddr   = be16_to_cpu(paylodHdr->srcAddr);

    pr_info("DoIp_Diag_Filter destAddr:%x\n", destAddr);
    pr_info("DoIp_Diag_Filter srcAddr:%x\n", srcAddr);
    pr_info("DoIp_Diag_Filter uds ServiceId:%x\n", paylodHdr->udsServiceId);

    if (inOutFlag) {
        if ((g_UdsService_In != -1) && (paylodHdr->udsServiceId != g_UdsService_In)) {
            ruleCheck = -1;
        }
    } else {
        if ((g_UdsService_Out != -1) && (paylodHdr->udsServiceId != g_UdsService_Out)) {
            ruleCheck = -1;
        }
    }
#if 0  // Temporarily do not add range processing.
 // User configured destMaxAddr.
    if (((g_DestMaxAddr_In != -1) && (paylodHdr->destAddr > g_DestMaxAddr_In) && (g_InOutFlag == 1))
        || ((g_DestMaxAddr_Out != -1) && (paylodHdr->destAddr > g_DestMaxAddr_Out) && (g_InOutFlag == 0))) {
        ruleCheck = -1;
    }

 // User configured destMinAddr.
    if (((g_DestMinAddr_In != -1) && (paylodHdr->destAddr < g_DestMinAddr_In) && (g_InOutFlag == 1))
        || ((g_DestMinAddr_Out != -1) && (paylodHdr->destAddr < g_DestMinAddr_Out) && (g_InOutFlag == 0))) {
        ruleCheck = -1;
    }

 // User configured srcMaxAddr.
    if (((g_SrcMaxAddr_In != -1) && (paylodHdr->srcAddr > g_SrcMaxAddr_In) && (g_InOutFlag == 1))
        || ((g_SrcMaxAddr_Out != -1) && (paylodHdr->srcAddr > g_SrcMaxAddr_Out) && (g_InOutFlag == 0))) {
        ruleCheck = -1;
    }

 // User configured srcMinAddr.
    if (((g_SrcMinAddr_In != -1) && (paylodHdr->srcAddr < g_SrcMinAddr_In) && (g_InOutFlag == 1))
        || ((g_SrcMinAddr_Out != -1) && (paylodHdr->srcAddr < g_SrcMinAddr_Out) && (g_InOutFlag == 0))) {
        ruleCheck = -1;
    }
#endif
    return ruleCheck;
}

/// @brief Filter a Doip packet
/// @param data Payload data
/// @param len Data length
/// @param inOutFlag INPUT/OUTPUT
/// @return Success/Failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00939
/// @needwork = dda
/// @endcode
static int Doip_Single_Filter(const Doip_Header_t *doiph, int totalLen, bool inOutFlag)
{
    // Get Doip protocol header
    u16 doipType  = 0;
    u32 doipLen   = 0;
    int ruleCheck = 1;

    // Network byte order conversion.
    doipType = be16_to_cpu(doiph->payloadType);
    doipLen  = be32_to_cpu(doiph->payloadLen);
    pr_info("Doip_Filter  ProtocolVersion:0x %X\n", doiph->protocolVersion);
    pr_info("Doip_Filter  InverseProtocolVersion:0x %X\n", doiph->inverseVersion);
    pr_info("Doip_Filter  payloadType:0x %X\n", doipType);
    pr_info("Doip_Filter  payloadLen:0x %X\n", doipLen);

    // User configured type.
    read_lock(&firewall_lock);
    if (inOutFlag) {
        if ((g_PayType_In != -1) && (doipType != g_PayType_In)) {
            ruleCheck = -1;
        }
        if ((g_PayLoadLen_In != -1) && (doipLen != g_PayLoadLen_In)) {
            ruleCheck = -1;
        }
        if ((g_ProtocolVersion_In != -1) && (doiph->protocolVersion != g_ProtocolVersion_In)) {
            ruleCheck = -1;
        }
        if ((g_InverProtocolVersion_In != -1) && (doiph->inverseVersion != g_InverProtocolVersion_In)) {
            ruleCheck = -1;
        }
    } else {
        if ((g_PayType_Out != -1) && (doipType != g_PayType_Out)) {
            ruleCheck = -1;
        }
        if ((g_PayLoadLen_Out != -1) && (doipLen != g_PayLoadLen_Out)) {
            ruleCheck = -1;
        }
        if ((g_ProtocolVersion_Out != -1) && (doiph->protocolVersion != g_ProtocolVersion_Out)) {
            ruleCheck = -1;
        }
        if ((g_InverProtocolVersion_Out != -1) && (doiph->inverseVersion != g_InverProtocolVersion_Out)) {
            ruleCheck = -1;
        }
    }

    // Process dest/src Addr and uds serviceId
    if (doipType == DOIP_PAYLOAD_DIAGNOSTIC_MESSAGE) {
        ruleCheck = DoIp_Diag_Filter(doiph, totalLen, inOutFlag);
    }
    // After comparing field by field, confirm whether all match. If all match,
    // process according to the corresponding action.
    if (ruleCheck == 1) {
        // DF_DROP User sets action to DROP, then after all rules are met, execute
        // DROP. Print protocol header information only in block case.
        if (g_Action == 0) {
            pr_info(
                "FirewallApp-doip-Drop:ProtocolVersion=0x%X InverseProtocolVersion=0x%X PayloadType=0x%X \
                PayloadLen=0x%X Protocol=doip\n",
                doiph->protocolVersion, doiph->inverseVersion, doipType, doipLen);
            ruleCheck = -1;
        }
    }
    // If not match, print log according to defaultaction value
    else if (ruleCheck == -1) {
        // drop
        if (g_DefaultAction == 0) {
            pr_info(
                "FirewallApp-doip-Drop:ProtocolVersion=0x%X InverseProtocolVersion=0x%X PayloadType=0x%X \
                PayloadLen=0x%X Protocol=doip\n",
                doiph->protocolVersion, doiph->inverseVersion, doipType, doipLen);
        }
        // If not match, return ACCEPT
        else if (g_DefaultAction == 1) {
            ruleCheck = 1;
        }
    } else {
    }
    read_unlock(&firewall_lock);
    return ruleCheck;
}

/// @brief Doip protocol firewall filtering
/// @param data Payload data
/// @param len Data length
/// @param inOutFlag INPUT/OUTPUT
/// @return Success/Failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00940
/// @needwork = dda
/// @endcode
static bool Doip_Filter(u8 *data, int len, bool inOutFlag)
{
    int offset    = 0;
    int ruleCheck = 1;
    u32 payLen    = 0;
    u32 packetLen = 0;
    // pr_info("Doip_Filter in !\n");
    // If length is less than DOIP header, return directly. To avoid malicious
    // modification of length, len length is limited to theoretical maximum
    // TCP/UDP length
    if (len < DOIP_HEADER_LENGTH || !data || len > 65535) {
        return true;
    }

    while (offset + DOIP_HEADER_LENGTH < len) {
        // Safely read message header
        const Doip_Header_t *header = (const Doip_Header_t *)(data + offset);
        payLen                      = be32_to_cpu(header->payloadLen);
        if ((u8 *)header < data) {
            break;
        }
        if ((payLen + DOIP_HEADER_LENGTH) > len) {
            break;
        }

        // Filter each packet
        ruleCheck = Doip_Single_Filter(header, (payLen + DOIP_HEADER_LENGTH), inOutFlag);
        // If there is a need to drop, break.
        if (ruleCheck == -1) {
            break;
        }
        /* Parse payload length */
        // payLen    = be32_to_cpu(header->payloadLen);
        packetLen = DOIP_HEADER_LENGTH + payLen;

        // If length exceeds buffer length, exit.
        if (packetLen + offset > len) {
            break;
        }
        /* Move to next packet */
        offset += packetLen;
    }
    if (ruleCheck == -1) {
        return false;
    }
    return true;
}

/// @brief Someip SD actual filtering
/// @param data Payload data
/// @param inOutFlag INPUT/OUTPUT
/// @return 1: All match (action=accept) 0: All match (action = drop) -1: Match
/// unsuccessful
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00941
/// @needwork = dda
/// @endcode
static int Someip_SD_Entitys_Filter(u8 *data, bool inOutFlag)
{
    Someip_Sd_Entry_t *sdEntity = (Someip_Sd_Entry_t *)data;
    int ruleCheck               = 1;
    if (!data) {
        return ruleCheck;
    }
    // Convert network byte order to host byte order
    sdEntity->u1 = be32_to_cpu(sdEntity->u1);
    sdEntity->u2 = be32_to_cpu(sdEntity->u2);
    sdEntity->u3 = be32_to_cpu(sdEntity->u3);

    pr_info("Someip_SD_Entitys_Filter: get in\n");
    pr_info("SOME/IP SD entitys  entryType: 0x%X\n", sdEntity->type);
    pr_info("SOME/IP SD entitys  major: 0x%X\n", sdEntity->major);
    pr_info("SOME/IP SD entitys  minor: 0x%X\n", sdEntity->minor);
    pr_info("SOME/IP SD entitys  length: 0x%X\n", sdEntity->gid);
    pr_info("SOME/IP SD entitys  inst: 0x%X\n", sdEntity->inst);
    pr_info("SOME/IP SD entitys  serv: 0x%X\n", sdEntity->serv);

    // IN
    if (inOutFlag) {
        if ((g_Sd_EntType_In != -1) && (sdEntity->type != g_Sd_EntType_In)) {
            ruleCheck = -1;
        }
        // Only when type equals 0x06 or 0x07 does groupid have meaning
        if ((sdEntity->type == 0x06 || sdEntity->type == 0x07) && (g_Sd_GroupId_In != -1)
            && (sdEntity->gid != g_Sd_GroupId_In)) {
            ruleCheck = -1;
        }

        if ((g_Sd_ServiceInstId_In != -1) && (sdEntity->inst != g_Sd_ServiceInstId_In)) {
            ruleCheck = -1;
        }
        if ((g_Sd_ServiceId_In != -1) && (sdEntity->serv != g_Sd_ServiceId_In)) {
            ruleCheck = -1;
        }

    } else {
        if ((g_Sd_EntType_Out != -1) && (sdEntity->type != g_Sd_EntType_Out)) {
            ruleCheck = -1;
        }
        // Only when type equals 0x06 or 0x07 does groupid have meaning
        if ((sdEntity->type == 0x06 || sdEntity->type == 0x07) && (g_Sd_GroupId_Out != -1)
            && (sdEntity->gid != g_Sd_GroupId_Out)) {
            ruleCheck = -1;
        }

        if ((g_Sd_ServiceInstId_Out != -1) && (sdEntity->inst != g_Sd_ServiceInstId_Out)) {
            ruleCheck = -1;
        }
        if ((g_Sd_ServiceId_Out != -1) && (sdEntity->serv != g_Sd_ServiceId_Out)) {
            ruleCheck = -1;
        }
    }
// Range comparison not yet supported
#if 0  
    //
    if (((g_Sd_MaxMajorVersion_In != -1) && (sdEntity->major > g_Sd_MaxMajorVersion_In) && (g_InOutFlag == 1))
        || ((g_Sd_MaxMajorVersion_Out != -1) && (sdEntity->major > g_Sd_MaxMajorVersion_Out) && (g_InOutFlag == 0))) {
        //pr_info("SOME/IP SD maxmajor not match!\n");
        ruleCheck = -1;
    }

    if (((g_Sd_MinMajorVersion_In != -1) && (sdEntity->major < g_Sd_MinMajorVersion_In) && (g_InOutFlag == 1))
        || ((g_Sd_MinMajorVersion_Out != -1) && (sdEntity->major < g_Sd_MinMajorVersion_Out) && (g_InOutFlag == 0))) {
        //pr_info("SOME/IP SD minmajor not match!\n");
        ruleCheck = -1;
    }
#endif

    // After comparing field by field, confirm whether all match. If all match,
    // process according to the corresponding action.
    if (ruleCheck == 1) {
        // DF_DROP User sets action to DROP, then after all rules are met, execute
        // DROP. Print protocol header information only in block case.
        if (g_Action == 0) {
            pr_info(
                "FirewallApp-someipsd-Drop:SdServiceId=0x%X SdServiceInstanceId=0x%X  MajorVersion=0x%X EntityType=0x%X \
                MinorVerison=0x%X EventGroupId=0x%X Protocol=someipsd\n",
                sdEntity->serv, sdEntity->inst, sdEntity->major, sdEntity->type, sdEntity->minor, sdEntity->gid);
            ruleCheck = -1;
        }
    }
    // If not match, print log according to defaultaction value.
    else if (ruleCheck == -1) {
        // drop
        if (g_DefaultAction == 0) {
            pr_info(
                "FirewallApp-someipsd-Drop:SdServiceId=0x%X SdServiceInstanceId=0x%X  MajorVersion=0x%X EntityType=0x%X \
                MinorVerison=0x%X EventGroupId=0x%X Protocol=someipsd\n",
                sdEntity->serv, sdEntity->inst, sdEntity->major, sdEntity->type, sdEntity->minor, sdEntity->gid);
        }
        // If not match, return ACCEPT
        else if (g_DefaultAction == 1) {
            ruleCheck = 1;
        }
    } else {
    }
    return ruleCheck;
}

/// @brief SOMEIP SD protocol filtering (processed as only one someipsd message
/// per someip message)
/// @param data Payload data
/// @param dataLen Data length
/// @param inOutFlag  INPUT/OUTPUT
/// @return Filter success/failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00942
/// @needwork = dda
/// @endcode
static int Someip_Sd_Filter(u8 *data, int dataLen, bool inOutFlag)
{
    Someip_Sd_Header_t *sdHdr;
    Someip_Header_t *hdr;
    int ruleCheck     = 1;
    int entLen        = 0;
    u32 *entityLenPtr = NULL;
    u8 *ents          = NULL;
    // Number of Entities. If error is not due to filtering issues, return true
    // directly.
    int entitys = 0;
    sdHdr       = (Someip_Sd_Header_t *)(data + sizeof(Someip_Header_t));
    hdr         = (Someip_Header_t *)data;

    pr_info("Someip_Sd_Filter: get in\n");
    pr_info("SOME/IP SD Header packet: Service ID = 0x%04x\n", be16_to_cpu(hdr->serviceId));
    pr_info("SOME/IP SD Header packet: method ID = 0x%04x\n", be16_to_cpu(hdr->methodId));
    pr_info("SOME/IP SD Header protocol length: 0x%02X\n", be32_to_cpu(hdr->length));
    pr_info("SOME/IP SD Header protocol clientId: 0x%02X\n", be16_to_cpu(hdr->clientId));
    pr_info("SOME/IP SD Header protocol sessionId: 0x%02X\n", be16_to_cpu(hdr->sessionId));
    pr_info("SOME/IP SD Header protocol protocolVersion: 0x%X\n", hdr->protocolVersion);
    pr_info("SOME/IP SD Header protocol interfaceVersion: 0x%X\n", hdr->interfaceVersion);
    pr_info("SOME/IP SD Header protocol messageType: 0x%X\n", hdr->messageType);
    pr_info("SOME/IP SD Header protocol returnCode: 0x%X\n", hdr->returnCode);

    if (sdHdr == NULL) {
        return ruleCheck;
    }

    sdHdr->u = be32_to_cpu(sdHdr->u);

    // check len
    if (dataLen < (sizeof(Someip_Header_t) + sizeof(Someip_Sd_Header_t))) {
        return ruleCheck;
    }
    // Get entitys length
    entityLenPtr = (u32 *)(data + sizeof(Someip_Header_t) + sizeof(Someip_Sd_Header_t));
    entLen       = be32_to_cpu(*entityLenPtr);

    // len verify. If verification fails, do not filter, return true directly.
    entitys = entLen / sizeof(Someip_Sd_Entry_t);

    if (entLen != (entitys * sizeof(Someip_Sd_Entry_t)) || entLen == 0) {
        return ruleCheck;
    }

    // buf pointer offset to Entity Array header position.
    ents = (u8 *)(data + sizeof(Someip_Header_t) + sizeof(Someip_Sd_Header_t) + sizeof(u32));
    // loop check Entity, each entity size 16.
    while (entLen > 0) {
        int ret = 1;
        ret     = Someip_SD_Entitys_Filter(ents, inOutFlag);
        // For dropped packets, break out of loop
        if (ret == -1) {
            return 1;
        }
        entLen -= sizeof(Someip_Sd_Entry_t);
        ents += sizeof(Someip_Sd_Entry_t);
    }
    return ruleCheck;
}

/// @brief Someip single packet filtering
/// @param hrd someip protocol header
/// @param data Payload data
/// @param dataLen Data length
/// @param inOutFlag  INPUT/OUTPUT
/// @return 1: All match (action=accept) 0: All match (action = drop) -1: Match
/// unsuccessful
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00943
/// @needwork = dda
/// @endcode
static int SomeIp_SinglePacket_Filter(const Someip_Header_t *hdr, u8 *data, int dataLen, bool inOutFlag)
{
    int ruleCheck = 1;

    // If the incoming someip packet data is incomplete, return true directly,
    // firewall does not filter.
    if ((be32_to_cpu(hdr->length) + 8) != dataLen) {
        return ruleCheck;
    }
    pr_info("SomeIp_SinglePacket_Filter: get in\n");
    pr_info("SOME/IP packet: Service ID = 0x%04x\n", be16_to_cpu(hdr->serviceId));
    pr_info("SOME/IP packet: method ID = 0x%04x\n", be16_to_cpu(hdr->methodId));
    pr_info("SOME/IP protocol length: 0x%02X\n", be32_to_cpu(hdr->length));
    pr_info("SOME/IP protocol clientId: 0x%02X\n", be16_to_cpu(hdr->clientId));
    pr_info("SOME/IP protocol sessionId: 0x%02X\n", be16_to_cpu(hdr->sessionId));
    pr_info("SOME/IP protocol protocolVersion: 0x%X\n", hdr->protocolVersion);
    pr_info("SOME/IP protocol interfaceVersion: 0x%X\n", hdr->interfaceVersion);
    pr_info("SOME/IP protocol messageType: 0x%X\n", hdr->messageType);
    pr_info("SOME/IP protocol returnCode: 0x%X\n", hdr->returnCode);

    read_lock(&firewall_lock);
    // true input
    if (inOutFlag) {
        // User configured serviceid and no match.
        if ((g_Target_Service_Id_In != -1) && (g_Target_Service_Id_In != be16_to_cpu(hdr->serviceId))) {
            ruleCheck = -1;
        }
        // User configured clientId and no match.
        if ((g_ClientId_In != -1) && (g_ClientId_In != be16_to_cpu(hdr->clientId))) {
            ruleCheck = -1;
        }
        // User configured MajorVersion and no match.
        if ((g_Target_Major_Version_In != -1) && (g_Target_Major_Version_In != hdr->interfaceVersion)) {
            ruleCheck = -1;
        }
        // User configured messageType and no match.
        if ((g_Target_Msg_Type_In != -1) && (g_Target_Msg_Type_In != hdr->messageType)) {
            ruleCheck = -1;
        }
        // User configured methodid and no match.
        if ((g_Target_Method_Id_In != -1) && (g_Target_Method_Id_In != be16_to_cpu(hdr->methodId))) {
            ruleCheck = -1;
        }
        // User configured protocol Version and no match.
        if ((g_Target_Pro_Version_In != -1) && (g_Target_Pro_Version_In != hdr->protocolVersion)) {
            ruleCheck = -1;
        }
        // User configured returncode and no match.
        if ((g_Target_Ret_Code_In != -1) && (g_Target_Ret_Code_In != hdr->returnCode)) {
            ruleCheck = -1;
        }
        // Length verification flag.
        if (g_Target_Length_Flag_In == 1) {
            if (dataLen != (be32_to_cpu(hdr->length) + 8)) {
                ruleCheck = -1;
            }
        }
    } else {
        // User configured serviceid and no match.
        if ((g_Target_Service_Id_Out != -1) && (g_Target_Service_Id_Out != be16_to_cpu(hdr->serviceId))) {
            ruleCheck = -1;
        }
        // User configured clientId and no match.
        if ((g_ClientId_Out != -1) && (g_ClientId_Out != be16_to_cpu(hdr->clientId))) {
            ruleCheck = -1;
        }
        // User configured MajorVersion and no match.
        if ((g_Target_Major_Version_Out != -1) && (g_Target_Major_Version_Out != hdr->interfaceVersion)) {
            ruleCheck = -1;
        }
        // User configured messageType and no match.
        if ((g_Target_Msg_Type_Out != -1) && (g_Target_Msg_Type_Out != hdr->messageType)) {
            ruleCheck = -1;
        }
        // User configured methodid and no match.
        if ((g_Target_Method_Id_Out != -1) && (g_Target_Method_Id_Out != be16_to_cpu(hdr->methodId))) {
            ruleCheck = -1;
        }
        // User configured protocol Version and no match.
        if ((g_Target_Pro_Version_Out != -1) && (g_Target_Pro_Version_Out != hdr->protocolVersion)) {
            ruleCheck = -1;
        }
        // User configured returncode and no match.
        if ((g_Target_Ret_Code_Out != -1) && (g_Target_Ret_Code_Out != hdr->returnCode)) {
            ruleCheck = -1;
        }
        // Length verification flag.
        if (g_Target_Length_Flag_Out == 1) {
            if (dataLen != (be32_to_cpu(hdr->length) + 8)) {
                ruleCheck = -1;
            }
        }
    }

    // SD type message
    if ((be16_to_cpu(hdr->serviceId) == SOMEIP_SD_SERVICE_ID) && (be16_to_cpu(hdr->methodId) == SOMEIP_SD_METHOD_ID)
        && (hdr->protocolVersion == SOMEIP_SD_PROTOCOL_VERSION)
        && (hdr->interfaceVersion == SOMEIP_SD_INTERFACE_VERSION) && (hdr->messageType == SOMEIP_SD_MESSAGE_TYPE)
        && (hdr->returnCode == SOMEIP_SD_RETURN_CODE)) {
        ruleCheck = Someip_Sd_Filter(data, dataLen, inOutFlag);
    }

    // After comparing field by field, confirm whether all match. If all match,
    // process according to the corresponding action.
    if (ruleCheck == 1) {
        // DF_DROP User sets action to DROP, then after all rules are met, execute
        // DROP. Print protocol header information only in block case.
        if (g_Action == 0) {
            pr_info(
                "FirewallApp-someip-Drop:ServiceId=0x%X methodId=0x%X length:0x%X clientId=0x%X \
                protocolVersion=0x%X interfaceVersion=0x%X messageType=0x%X returnCode=0x%X Protocol=someip\n",
                be16_to_cpu(hdr->serviceId), be16_to_cpu(hdr->methodId), be32_to_cpu(hdr->length),
                be16_to_cpu(hdr->clientId), hdr->protocolVersion, hdr->interfaceVersion, hdr->messageType,
                hdr->returnCode);
            ruleCheck = -1;
        }
    }
    // If not match, print log according to defaultaction value.
    else if (ruleCheck == -1) {
        // drop
        if (g_DefaultAction == 0) {
            pr_info(
                "FirewallApp-someip-Drop:ServiceId=0x%X methodId=0x%X length:0x%X clientId=0x%X \
                protocolVersion=0x%X interfaceVersion=0x%X messageType=0x%X returnCode=0x%X Protocol=someip\n",
                be16_to_cpu(hdr->serviceId), be16_to_cpu(hdr->methodId), be32_to_cpu(hdr->length),
                be16_to_cpu(hdr->clientId), hdr->protocolVersion, hdr->interfaceVersion, hdr->messageType,
                hdr->returnCode);
        }
        // If not match, return ACCEPT
        else if (g_DefaultAction == 1) {
            ruleCheck = 1;
        }
    } else {
    }
    read_unlock(&firewall_lock);
    return ruleCheck;
}

/// @brief Someip protocol filtering
/// @param data Payload data
/// @param dataLen Data length
/// @param inOutFlag  INPUT/OUTPUT
/// @return ture/false
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00944
/// @needwork = dda
/// @endcode
static bool Someip_Filter(u8 *data, int dataLen, bool inOutFlag)
{
    // Take first packet to calculate length
    int offset = 0;
    // Receive single packet processing
    int ruleCheck = 0;
    // Loop someip body len
    int payLen = 0;
    // Each packet length
    int packetLen = 0;

    if (!data || dataLen > 65535 || dataLen < SOMEIP_HEADER_LENGTH_ALL) {
        return true;
    }

    while (offset + SOMEIP_HEADER_LENGTH_ALL <= dataLen) {
        // Safely read message header
        const Someip_Header_t *header = (const Someip_Header_t *)(data + offset);

        if (!header) {
            break;
        }
        payLen    = be32_to_cpu(header->length);
        packetLen = SOMEIP_HEADER_LENGTH_ALL + payLen;
        // Filter each packet
        ruleCheck = SomeIp_SinglePacket_Filter(header, data, packetLen, inOutFlag);
        // If there is a drop, return directly, no need to continue looping.
        if (ruleCheck == -1) {
            return false;
        }

        // If length exceeds buffer length, exit.
        if (packetLen + offset > dataLen) {
            break;
        }
        /* Move to next packet */
        offset += packetLen;
    }
    return true;
}

/// @brief Application layer protocol actual filtering processing
/// @param data Payload data
/// @param dataLen Data length
/// @param protocolType Protocol type
/// @param inOutFlag   INPUT/OUTPUT
/// @return NF_ACCEPT/NF_DROP
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00945
/// @needwork = dda
/// @endcode
static bool Filter_By_Protocol(u8 *data, unsigned int dataLen, int protocolType, bool inOutFlag)
{
    bool filterCheck;
    filterCheck = true;
    // Record protocol type
    // pr_info("Filtering %d bytes for protocol type %d\n", dataLen,
    // protocolType);
    switch (protocolType) {
        case PROTOCOL_SOMEIP:
            filterCheck = Someip_Filter(data, dataLen, inOutFlag);
            pr_info("Filter_By_Protocol Someip_Filter return vaule:%d\n", filterCheck);
            break;
        case PROTOCOL_DDS:
            /* code */
            break;
        case PROTOCOL_DOIP:
            filterCheck = Doip_Filter(data, dataLen, inOutFlag);
            pr_info("Filter_By_Protocol Doip_Filter return vaule:%d\n", filterCheck);
            break;

        case PROTOCOL_UNKNOWN:
            /* code */
            break;
        default:
            break;
    }

    return filterCheck;
}

/// @brief Check protocol in TCP/UDP packets
/// @param skb skb structure data
/// @param iph ip protocol header
/// @param inOutFlag INPUT/OUTPUT
/// @return NF_ACCEPT/NF_DROP
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00946
/// @needwork = dda
/// @endcode
static int Check_Packet_Protocol(struct sk_buff *skb, struct iphdr *iph, bool inOutFlag)
{
    unsigned int totalLen   = 0;
    unsigned int payloadLen = 0;
    u8 *payloadData         = NULL;
    int protocolType        = PROTOCOL_UNKNOWN;

    // Calculate transport layer protocol, payload protocol
    if (iph->protocol == IPPROTO_TCP) {  // Calculate TCP payload position and length
                                         // If parsing tcp protocol header fails, do not filter
        if (!(tcp_hdr(skb))) {
            pr_info("Failed to  get tcp_hdr\n");
            return NF_ACCEPT;
        }
        if (skb->len < (iph->ihl * 4 + sizeof(struct tcphdr))) {
            // Packet incomplete, unable to parse TCP header
            return NF_ACCEPT;
        }

        totalLen    = ntohs(iph->tot_len);
        payloadLen  = totalLen - (iph->ihl * 4) - ((tcp_hdr(skb)->doff * 4));
        payloadData = (u8 *)tcp_hdr(skb) + (tcp_hdr(skb)->doff * 4);
    } else if (iph->protocol == IPPROTO_UDP) {
        // If parsing udp protocol header fails, do not filter
        if (!(udp_hdr(skb))) {
            pr_info("Failed to  get udp_hdr\n");
            return NF_ACCEPT;
        }
        if (skb->len < (iph->ihl * 4 + sizeof(struct udphdr))) {
            // Packet incomplete, unable to parse UDP header
            return NF_ACCEPT;
        }
        // pr_info("parser UDP patcket!\n");
        // Calculate UDP payload position and length
        totalLen    = ntohs(iph->tot_len);
        payloadLen  = totalLen - (iph->ihl * 4) - sizeof(struct udphdr);
        payloadData = (u8 *)udp_hdr(skb) + sizeof(struct udphdr);
    }
    // Application layer uses other transport protocol, do not process.
    else {
        return NF_ACCEPT;
    }

    // someip
    if (Is_Someip_Packet(payloadData, payloadLen)) {
        protocolType = PROTOCOL_SOMEIP;
    }
    // doip
    else if (Is_DoIp_Packet(payloadData, payloadLen)) {
        protocolType = PROTOCOL_DOIP;
    }
    // dds
    else if (Is_Dds_Packet(payloadData, payloadLen)) {
        protocolType = PROTOCOL_DDS;
    } else {
        protocolType = PROTOCOL_UNKNOWN;
    }

    // Perform protocol-specific filtering
    if (!Filter_By_Protocol(payloadData, payloadLen, protocolType, inOutFlag)) {
        // pr_info("Packet block by protocol filter\n");
        return NF_DROP;
    }
    return NF_ACCEPT;
}

/// @brief Netfilter hook function (INPUT)
/// @param priv User-defined structure data
/// @param skb skb structure
/// @param state hook state
/// @return NF_ACCEPT/NF_DROP
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00947
/// @needwork = dda
/// @endcode
static unsigned int App_Protocol_Filter_Input(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct iphdr *iph;
    u32 len;
    // Ensure packet is linear and can be safely accessed
    if (!pskb_may_pull(skb, sizeof(struct iphdr))) {
        pr_info("Failed to pskb_may_pull skb\n");
        return NF_ACCEPT;
    }
    iph = ip_hdr(skb);
    // If IP protocol is not ipv4 and IP header length is less than minimum 5.
    // Currently only supports IPV4
    if (iph->ihl < 5 || iph->version != 4) {
        return NF_ACCEPT;
    }
    // If IP protocol header data is non-linear, return directly.
    if (!pskb_may_pull(skb, iph->ihl * 4)) {
        return NF_ACCEPT;
    }
    iph = ip_hdr(skb);
    // IP header length verification.
    len = ntohs(iph->tot_len);
    // Verify skd data length, if too small return directly.
    if (skb->len < len) {
        return NF_ACCEPT;
    } else if (len < (iph->ihl * 4)) {
        return NF_ACCEPT;
    }

    // Determine protocol type
    return Check_Packet_Protocol(skb, iph, true);
}

/// @brief Netfilter hook function (OUTPUT)
/// @param priv User-defined structure data
/// @param skb skb structure
/// @param state hook state
/// @return NF_ACCEPT/NF_DROP
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00948
/// @needwork = dda
/// @endcode
static unsigned int App_Protocol_Filter_Output(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct iphdr *iph;
    u32 len;
    // Ensure packet is linear and can be safely accessed
    if (!pskb_may_pull(skb, sizeof(struct iphdr))) {
        pr_info("Failed to linearize skb\n");
        return NF_ACCEPT;
    }
    iph = ip_hdr(skb);
    // If IP protocol header data is non-linear, return directly.
    if (!pskb_may_pull(skb, iph->ihl * 4)) {
        return NF_ACCEPT;
    }
    iph = ip_hdr(skb);
    // IP header length verification.
    len = ntohs(iph->tot_len);
    // Verify skd data length, if too small return directly.
    if (skb->len < len) {
        return NF_ACCEPT;
    } else if (len < (iph->ihl * 4)) {
        return NF_ACCEPT;
    }

    // Determine protocol type
    return Check_Packet_Protocol(skb, iph, false);
}

/// @brief Netfilter hook structure
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00589
/// @trace_id_dd=DD_FW_01004
/// @needwork = dd
/// @endcode
static struct nf_hook_ops g_Nfho[] = {
    // input hook point
    {
        .hook     = App_Protocol_Filter_Input,
        .pf       = PF_INET,
        .hooknum  = NF_INET_LOCAL_IN,
        .priority = NF_IP_PRI_SECURITY,
    },
    // output hook point
    {
        .hook     = App_Protocol_Filter_Output,
        .pf       = PF_INET,
        .hooknum  = NF_INET_LOCAL_OUT,
        .priority = NF_IP_PRI_SECURITY,
    },
};
/// @brief Kernel module initialization function
/// @param
/// @return int
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00949
/// @needwork = dda
/// @endcode
static int __init ApFilter_Filter_Init(void)
{
    int ret = nf_register_net_hooks(&init_net, g_Nfho, ARRAY_SIZE(g_Nfho));
    // Register hook INPUT
    if (ret < 0) {
        pr_warn("input/output register failed!\n");
        return ret;
    }
    pr_info("ap firewall kernel module loaded succefully!\n");
    return 0;
}

/// @brief Kernel module exit function
/// @param
/// @return int
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00950
/// @needwork = dda
/// @endcode
static void __exit ApFilter_Filter_Exit(void)
{
    nf_unregister_net_hooks(&init_net, g_Nfho, ARRAY_SIZE(g_Nfho));
    pr_info("ap firewall kernel module  unloaded\n");
}

module_init(ApFilter_Filter_Init);
module_exit(ApFilter_Filter_Exit);
