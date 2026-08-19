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

/*******************************************************************************
**                                                                            **
**  FILENAME    : Crc.h                                                       **
**                                                                            **
**  Created on  :                                                             **
**  Author      : ShenXu,Yb                                                   **
**  Vendor      :                                                             **
**  DESCRIPTION :                                                             **
**                                                                            **
**  SPECIFICATION(S) :   AUTOSAR classic Platform R19-11                      **
**                                                                            **
*******************************************************************************/
#ifndef CRC_H_
#define CRC_H_
/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*@ req<SWS_Crc_00048> */
#define CRC_VENDOR_ID                   (62u)
#define CRC_MODULE_ID                   (201u)
#define CRC_AR_RELEASE_MAJOR_VERSION    (4u)
#define CRC_AR_RELEASE_MINOR_VERSION    (5u)
#define CRC_AR_RELEASE_REVISION_VERSION (0u)
#define CRC_SW_MAJOR_VERSION            (2u)
#define CRC_SW_MINOR_VERSION            (1u)
#define CRC_SW_PATCH_VERSION            (0u)

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Crc_Cfg.h"

/*@ req<SWS_Crc_00017>,<SWS_Crc_00021>,<SWS_Crc_00011> */
/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
/*
 * Brief               This service returns the version information of
 *                     this module
 * ServiceId           0x04
 * Sync/Async          Synchronous
 * Reentrancy          Reentrant
 * Param-Name[in]      None
 * Param-Name[out]     Versioninfo: Pointer to where to store the version
 * Param-Name[in/out]  None
 * Return              None
 * PreCondition        None
 * CallByAPI           Up layer
 */
/*************************************************************************/
#define Crc_GetVersionInfo(VersionInfo)                                                                                \
    do {                                                                                                               \
        (VersionInfo)->vendorID         = CRC_VENDOR_ID;                                                               \
        (VersionInfo)->moduleID         = CRC_MODULE_ID;                                                               \
        (VersionInfo)->instanceID       = 0u;                                                                          \
        (VersionInfo)->sw_major_version = CRC_SW_MAJOR_VERSION;                                                        \
        (VersionInfo)->sw_minor_version = CRC_SW_MINOR_VERSION;                                                        \
        (VersionInfo)->sw_patch_version = CRC_SW_PATCH_VERSION;                                                        \
        (VersionInfo)->sw_major_version = CRC_SW_MAJOR_VERSION;                                                        \
        (VersionInfo)->sw_minor_version = CRC_SW_MINOR_VERSION;                                                        \
        (VersionInfo)->sw_patch_version = CRC_SW_PATCH_VERSION;                                                        \
    } while (0)

/*******************************************************************************
**                      Global Functions                                      **
*******************************************************************************/
/******************************************************************************/
#if (STD_ON == CRC8_ALGORITHM)
    /*************************************************************************/
    /*
 * Brief               This service makes a CRC8 calculation on Crc_Length
 *                     data bytes, with SAE J1850 parameters
 * ServiceId           0x01
 * Sync/Async          Synchronous
 * Reentrancy          Reentrant
 * Param-Name[in]      Crc_DataPtr:Pointer to start address of data block
 *                     to be calculated.
 *                     Crc_Length:Length of data block to be calculated in bytes.
 *                     Crc_StartValue8:Start value when the algorithm starts.
 *                     Crc_IsFirstCall:
 *                     TRUE: First call in a sequence or individual CRC calculation;
 *                     start from initial value, ignore Crc_StartValue8.
 *                     FALSE:Subsequent call in a call sequence; Crc_StartValue8 is
 *                     interpreted to be the return value of the previous function call.
 * Param-Name[out]     None
 * Param-Name[in/out]  None
 * Return              uint8
 * PreCondition        None
 * CallByAPI           Up layer
 */
    /*************************************************************************/
    /* req<SWS_Crc_00031>*/
    #define CRC_START_SEC_CODE
    #include "Crc_MemMap.h"
extern FUNC(uint8, CRC_CODE) Crc_CalculateCRC8(P2CONST(uint8, AUTOMATIC, CRC_APPL_CONST) Crc_DataPtr,
                                               VAR(uint32, AUTOMATIC) Crc_Length,
                                               VAR(uint8, AUTOMATIC) Crc_StartValue8,
                                               VAR(boolean, AUTOMATIC) Crc_IsFirstCall);
    #define CRC_STOP_SEC_CODE
    #include "Crc_MemMap.h"
#endif /* STD_ON == CRC8_ALGORITHM */

#if (STD_ON == CRC8H2F_ALGORITHM)
    /*************************************************************************/
    /*
 * Brief               This service makes a CRC8 calculation with the
 *                     Polynomial 0x2F on Crc_Length
 * ServiceId           0x05
 * Sync/Async          Synchronous
 * Reentrancy          Reentrant
 * Param-Name[in]      Crc_DataPtr:Pointer to start address of data block
 *                     to be calculated.
 *                     Crc_Length:Length of data block to be calculated in bytes.
 *                     Crc_StartValue8H2F:Start value when the algorithm starts.
 *                     Crc_IsFirstCall:
 *                     TRUE: First call in a sequence or individual CRC calculation;
 *                     start from initial value, ignore Crc_StartValue8.
 *                     FALSE:Subsequent call in a call sequence; Crc_StartValue8 is
 *                     interpreted to be the return value of the previous function call.
 * Param-Name[out]     None
 * Param-Name[in/out]  None
 * Return              uint8
 * PreCondition        None
 * CallByAPI           Up layer
 */
    /*************************************************************************/
    /* req<SWS_Crc_00043>*/
    #define CRC_START_SEC_CODE
    #include "Crc_MemMap.h"
extern FUNC(uint8, CRC_CODE) Crc_CalculateCRC8H2F(P2CONST(uint8, AUTOMATIC, CRC_APPL_CONST) Crc_DataPtr,
                                                  VAR(uint32, AUTOMATIC) Crc_Length,
                                                  VAR(uint8, AUTOMATIC) Crc_StartValue8H2F,
                                                  VAR(boolean, AUTOMATIC) Crc_IsFirstCall);
    #define CRC_STOP_SEC_CODE
    #include "Crc_MemMap.h"
#endif /* STD_ON == CRC8H2F_ALGORITHM */

#if (STD_ON == CRC16_ALGORITHM)
    /*************************************************************************/
    /*
 * Brief               This service makes a CRC16 calculation on Crc_Length
 *                     data bytes.
 * ServiceId           0x02
 * Sync/Async          Synchronous
 * Reentrancy          Reentrant
 * Param-Name[in]      Crc_DataPtr:Pointer to start address of data block
 *                     to be calculated.
 *                     Crc_Length:Length of data block to be calculated in bytes.
 *                     Crc_StartValue16:Start value when the algorithm starts.
 *                     Crc_IsFirstCall:
 *                     TRUE: First call in a sequence or individual CRC calculation;
 *                     start from initial value, ignore Crc_StartValue16.
 *                     FALSE:Subsequent call in a call sequence; Crc_StartValue16 is
 *                     interpreted to be the return value of the previous function call.
 * Param-Name[out]     None
 * Param-Name[in/out]  None
 * Return              uint16
 * PreCondition        None
 * CallByAPI           Up layer
 */
    /*************************************************************************/
    /* req<SWS_Crc_00019>*/
    #define CRC_START_SEC_CODE
    #include "Crc_MemMap.h"
extern FUNC(uint16, CRC_CODE) Crc_CalculateCRC16(P2CONST(uint8, AUTOMATIC, CRC_APPL_CONST) Crc_DataPtr,
                                                 VAR(uint32, AUTOMATIC) Crc_Length,
                                                 VAR(uint16, AUTOMATIC) Crc_StartValue16,
                                                 VAR(boolean, AUTOMATIC) Crc_IsFirstCall);
    #define CRC_STOP_SEC_CODE
    #include "Crc_MemMap.h"
#endif /* STD_ON == CRC16_ALGORITHM */

#if (STD_ON == CRC32_ALGORITHM)
    /*************************************************************************/
    /*
 * Brief               This service makes a CRC32 calculation on Crc_Length
 *                     data bytes.
 * ServiceId           0x03
 * Sync/Async          Synchronous
 * Reentrancy          Reentrant
 * Param-Name[in]      Crc_DataPtr: Pointer to start address of data block
 *                     to be calculated.
 *                     Crc_Length: Length of data block to be calculated in bytes.
 *                     Crc_StartValue32: Start value when the algorithm starts.
 *                     Crc_IsFirstCall:
 *                     TRUE: First call in a sequence or individual CRC calculation;
 *                     start from initial value, ignore Crc_StartValue32.
 *                     FALSE: Subsequent call in a call sequence; Crc_StartValue32 is
 *                     interpreted to be the return value of the previous function call.
 * Param-Name[out]     None
 * Param-Name[in/out]  None
 * Return              uint16
 * PreCondition        None
 * CallByAPI           Up layer
 */
    /*************************************************************************/
    /* req<SWS_Crc_00020>*/
    #define CRC_START_SEC_CODE
    #include "Crc_MemMap.h"
extern FUNC(uint32, CRC_CODE) Crc_CalculateCRC32(P2CONST(uint8, AUTOMATIC, CRC_APPL_CONST) Crc_DataPtr,
                                                 VAR(uint32, AUTOMATIC) Crc_Length,
                                                 VAR(uint32, AUTOMATIC) Crc_StartValue32,
                                                 VAR(boolean, AUTOMATIC) Crc_IsFirstCall);
    #define CRC_STOP_SEC_CODE
    #include "Crc_MemMap.h"
#endif /* STD_ON == CRC32_ALGORITHM */

#if (STD_ON == CRC32P4_ALGORITHM)
    /*************************************************************************/
    /*
 * Brief               This service makes a CRC32 calculation on Crc_Length
 *                     data bytes.
 * ServiceId           0x06
 * Sync/Async          Synchronous
 * Reentrancy          Reentrant
 * Param-Name[in]      Crc_DataPtr: Pointer to start address of data block
 *                     to be calculated.
 *                     Crc_Length: Length of data block to be calculated in bytes.
 *                     Crc_StartValue32: Start value when the algorithm starts.
 *                     Crc_IsFirstCall:
 *                     TRUE: First call in a sequence or individual CRC calculation;
 *                     start from initial value, ignore Crc_StartValue32.
 *                     FALSE: Subsequent call in a call sequence; Crc_StartValue32 is
 *                     interpreted to be the return value of the previous function call.
 * Param-Name[out]     None
 * Param-Name[in/out]  None
 * Return              uint16
 * PreCondition        None
 * CallByAPI           Up layer
 */
    /*************************************************************************/
    /* req<SWS_Crc_00058>*/
    #define CRC_START_SEC_CODE
    #include "Crc_MemMap.h"
extern FUNC(uint32, CRC_CODE) Crc_CalculateCRC32P4(P2CONST(uint8, AUTOMATIC, CRC_APPL_CONST) Crc_DataPtr,
                                                   VAR(uint32, AUTOMATIC) Crc_Length,
                                                   VAR(uint32, AUTOMATIC) Crc_StartValue32,
                                                   VAR(boolean, AUTOMATIC) Crc_IsFirstCall);
    #define CRC_STOP_SEC_CODE
    #include "Crc_MemMap.h"
#endif /* STD_ON == CRC32P4_ALGORITHM */

#if (STD_ON == CRC64_ALGORITHM)
    #define CRC_START_SEC_CODE
    #include "Crc_MemMap.h"
extern FUNC(uint64, CRC_CODE) Crc_CalculateCRC64(P2CONST(uint8, AUTOMATIC, CRC_APPL_CONST) Crc_DataPtr,
                                                 VAR(uint64, AUTOMATIC) Crc_Length,
                                                 VAR(uint64, AUTOMATIC) Crc_StartValue64,
                                                 VAR(boolean, AUTOMATIC) Crc_IsFirstCall);
    #define CRC_STOP_SEC_CODE
    #include "Crc_MemMap.h"
#endif /* STD_ON == CRC64_ALGORITHM */

#endif /* CRC_H_ */
