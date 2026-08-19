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
/// @file       protocoldefine.h
/// @brief      Firewall application layer protocol structure
/// @details    Firewall application layer protocol structure
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
/// @unit_name=Protocol_Define
/// @unit_description=Firewall application layer definition.
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_KERNAL_MODULE_PROTOCOLDEFINE_H_
#define ARA_FW_KERNAL_MODULE_PROTOCOLDEFINE_H_
#include <linux/types.h>

/// @brief someiptype type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00875
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define PROTOCOL_SOMEIP 0x01

/// @brief dds type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00876
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define PROTOCOL_DDS 0x02
/// @brief doip type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00877
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define PROTOCOL_DOIP 0x03
/// @brief Unknown type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00878
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define PROTOCOL_UNKNOWN 0xff

/**
 *  SOME/IP  Message Type
 */
/// @brief  A request expecting a response (even void)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00879
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_MT_REQUEST 0x00
/// @brief  A fire&forget request
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00880
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_MT_REQUEST_NO_RETURN 0x01
/// @brief  A request of a notification/event callback expecting no response
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00881
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_MT_NOTIFICATION 0x02

/// @brief   The response message
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00882
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_MT_RESPONSE 0x80

/// @brief   The response containing an error
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00883
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_MT_ERROR 0x81

/// @brief    A TP request expecting a response (even void)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00884
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_TP_REQUEST 0x20

/// @brief   A TP fire&forget request
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00885
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_TP_REQUEST_NO_RETURN 0x21

/// @brief   A TP request of a notification/event callback expecting no response
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00886
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_TP_NOTIFICATION 0x22

/// @brief   The TP response message
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00887
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_TP_RESPONSE 0x23
/// @brief   The TP response containing an error
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00888
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_TP_ERROR 0x24

/**
 *  SOME/IP  Return Code
 */
/// @brief   No error occurred
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00889
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_E_OK 0x00

/// @brief    An unspecified error occurred
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00890
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_E_NOT_OK 0x01

/// @brief   The requested Service ID is unknown.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00891
/// @needwork = dda
/// @endcode
#define NSI_E_UNKNOWN_SERVICE 0x02

/// @brief The requested Method ID is unknown. Service ID is known
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00892
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_E_UNKNOWN_METHOD 0x03

/// @brief Service ID and Method ID are known. Application not running.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00893
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_E_NOT_READY 0x04

/// @brief System running the service is not reachable (internal error code
/// only)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00894
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_E_NOT_REACHABLE 0x05

/// @brief A timeout occurred (internal error code only)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00895
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_E_TIMEOUT 0x06

/// @brief Version of SOME/IP protocol not supported
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00896
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_E_WRONG_PROTOCOL_VERSION 0x07

/// @brief Interface version mismatch
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00897
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_E_WRONG_INTERFACE_VERSION 0x08

/// @brief  Deserialization error, so that payload cannot be deserialized.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00898
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_E_MALFORMED_MESSAGE 0x09

/// @brief  An unexpected message type was received
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00899
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define NSI_E_WRONG_MESSAGE_TYPE 0x0a

/**
 * @anchor SOME/IP SD message judgment format
 */

/// @brief  someip sd  service id
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00900
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define SOMEIP_SD_SERVICE_ID 0xffff

/// @brief someip sd  method id
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00901
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define SOMEIP_SD_METHOD_ID 0x8100

/// @brief someip sd  protocol version
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00902
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define SOMEIP_SD_PROTOCOL_VERSION 0x01

/// @brief someip sd  interface version
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00903
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define SOMEIP_SD_INTERFACE_VERSION 0x01

/// @brief someip sd  msg type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00904
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define SOMEIP_SD_MESSAGE_TYPE 0x02

/// @brief someip sd  return code
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00905
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define SOMEIP_SD_RETURN_CODE 0x00

/// @brief SOMEIP SERVICE ID+ METHOD ID + LENGTH  |--2bit--|--2bit--|--4bit--|
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00906
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define SOMEIP_HEADER_LENGTH 0x08

/// @brief SOMEIP SERVICE ID+ METHOD ID + LENGTH+....
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00907
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define SOMEIP_HEADER_LENGTH_ALL 0x10

/// DOIP  |--1bit--|--1bit--|--2bit--|--4bit--|
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00908
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define DOIP_HEADER_LENGTH 8

/// DoIP payload type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00909
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define DOIP_PAYLOAD_DIAGNOSTIC_MESSAGE 0x8001

/// DoIP protocol version
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00910
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define DOIP_PROTOCOL_VERSION_02 0x02

/// DoIP inverse protocol version
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00911
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define DOIP_INVERSE_VERSION_02 0xFD
/// DoIP protocol version (03)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00912
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define DOIP_PROTOCOL_VERSION_03 0x03
/// DoIP inverse protocol version (03)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00913
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define DOIP_INVERSE_VERSION_03 0xFC

/// DoIP type minimum value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00914
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define DOIP_PAYLOAD_MIN_TYPE 0x00
/// DoIP type value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00915
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define DOIP_PAYLOAD_CHECK_TYPE08 0x0008
/// DoIP type value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00916
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define DOIP_PAYLOAD_CHECK_TYPE4001 0x4001
/// DoIP type value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00917
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define DOIP_PAYLOAD_CHECK_TYPE4004 0x4004
/// DoIP type value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00918
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define DOIP_PAYLOAD_CHECK_TYPE8001 0x8001
/// DoIP type value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00919
/// @trace_id_sr=SR_FW_0005
/// @needwork = dda
/// @endcode
#define DOIP_PAYLOAD_CHECK_TYPE8003 0x8003

/// @brief SOME/IP protocol header structure
/// @param
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_01005
/// @needwork = dda
/// @endcode
typedef struct __attribute__((packed))
{
    u16 serviceId;
    u16 methodId;
    u32 length;
    u16 clientId;
    u16 sessionId;
    u8 protocolVersion;
    u8 interfaceVersion;
    u8 messageType;
    u8 returnCode;
} Someip_Header_t;

/// @brief Someip Parse structure.
/// @param
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_01006
/// @needwork = dda
/// @endcode
typedef struct __attribute__((packed))
{
    u16 serviceId;
    u16 methodId;
    u32 length;
    u16 clientId;
    u16 sessionId;
    u8 protocolVersion;
    u8 interfaceVersion;
    u8 messageType;
    u8 returnCode;
    s32 action;
} Someip_Header_Parse_t;

/// @brief SOME/IP SD protocol header structure
/// @param
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_01007
/// @needwork = dda
/// @endcode
typedef struct __attribute__((packed))
{
    union
    {
        struct
        {
            uint32_t reserved : 24;
            uint32_t flags : 8;
        };
        uint32_t u;
    };
} Someip_Sd_Header_t;

/// @brief SOME/IP SD protocol header structure
/// @param
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_01008
/// @needwork = dda
/// @endcode
typedef struct __attribute__((packed))
{
    u8 type;
    u8 opt[2];
    union
    {
        struct
        {
            u8 n1 : 4;
            u8 n0 : 4;
        };
        u8 n;
    } optc;

    union
    {
        struct
        {
            u32 inst : 16;
            u32 serv : 16;
        };
        u32 u1;
    };
    union
    {
        struct
        {
            u32 ttl : 24;
            u32 major : 8;
        };
        u32 u2;
    };
    union
    {
        /* PRS_SOMEIPSD_00270
     * PRS_SOMEIPSD_00389
     */
        struct
        {
            u32 gid : 16;
            u32 counter : 4;
            u32 reserved2 : 3;
            u32 init : 1;
            u32 reserved1 : 8;
        };
        /* PRS_SOMEIPSD_00269 */
        u32 minor;
        u32 u3;
    };

} Someip_Sd_Entry_t;

/// @brief SOME/IP SD protocol header structure
/// @param
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_01009
/// @needwork = dda
/// @endcode
typedef struct __attribute__((packed))
{
    u8 type;
    u8 opt[2];
    union
    {
        struct
        {
            u8 n1 : 4;
            u8 n0 : 4;
        };
        u8 n;
    } optc;

    union
    {
        struct
        {
            u32 inst : 16;
            u32 serv : 16;
        };
        u32 u1;
    };
    union
    {
        struct
        {
            u32 ttl : 24;
            u32 major : 8;
        };
        u32 u2;
    };
    union
    {
        /* PRS_SOMEIPSD_00270
     * PRS_SOMEIPSD_00389
     */
        struct
        {
            u32 gid : 16;
            u32 counter : 4;
            u32 reserved2 : 3;
            u32 init : 1;
            u32 reserved1 : 8;
        };
        /* PRS_SOMEIPSD_00269 */
        u32 minor;
        u32 u3;
    };
    s32 action;

} Someip_Sd_Entry_Parse_t;

/// @brief DoIP protocol header structure (based on ISO 13400-2)
/// @brief SOME/IP SD protocol header structure
/// @param
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_01010
/// @needwork = dda
/// @endcode
typedef struct __attribute__((packed))
{
    __u8 protocolVersion;
    __u8 inverseVersion;
    __u16 payloadType;
    __u32 payloadLen;
} Doip_Header_t;

/// @brief DoIP type = 0x8001 message body
/// @brief SOME/IP SD protocol header structure
/// @param
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_01011
/// @needwork = dda
/// @endcode
typedef struct __attribute__((packed))
{
    __u16 srcAddr;     /* Source address */
    __u16 destAddr;    /* Destination address */
    __u8 udsServiceId; /* uds service id */
} Doip_Payload_t;

/// @brief DoIP protocol header structure (based on ISO 13400-2)
/// @brief SOME/IP SD protocol header structure
/// @param
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_01012
/// @needwork = dda
/// @endcode
typedef struct __attribute__((packed))
{
    __u8 protocolVersion;
    __u8 inverseVersion;
    __u16 payloadType;
    __u32 payloadLen;
    s32 action;
} Doip_Header_Parse_t;

/// @brief DoIP type = 0x8001 message body
/// @brief SOME/IP SD protocol header structure
/// @param
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_01013
/// @needwork = dda
/// @endcode
typedef struct __attribute__((packed))
{
    __u16 srcAddr;     /* Source address */
    __u16 destAddr;    /* Destination address */
    __u8 udsServiceId; /* uds service id */
    s32 action;
} Doip_Payload_Parse_t;

/// @brief DDS RTPS protocol header structure (simplified version)
/// @brief SOME/IP SD protocol header structure
/// @param
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_01014
/// @needwork = dda
/// @endcode
typedef struct __attribute__((packed))
{
    u8 magic[4];  // "RTPS"
    u8 version_major;
    u8 version_minor;
    u16 vendor_id;
    u32 guid_prefix[3];
    // Followed by sub-messages
} Dds_Rtps_Header_t;

#endif
