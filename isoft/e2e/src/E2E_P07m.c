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
**  FILENAME    : E2E_P07m.c                                                  **
**                                                                            **
**  Created on  :                                                             **
**  Author      : xuhua                                                       **
**  Vendor      :                                                             **
**  DESCRIPTION :                                                             **
**                                                                            **
**  SPECIFICATION(S) :   AUTOSAR classic Platform R20-11                      **
**                                                                            **
*******************************************************************************/
/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/* <VERSION> <DATE> <AUTHOR> <REVISION LOG>
 * V2.0.0 [2023/10/19] [xuhua] Initial Vertion.
 */
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "E2E_P07m.h"
/*******************************************************************************
**                      Imported Compiler Switch Check                        **
*******************************************************************************/

/*******************************************************************************
**                       Version  Check                                       **
*******************************************************************************/

/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/

/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/
static void E2E_P07mCheck_Seqence(
    P2CONST(E2E_P07mConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    P2VAR(E2E_P07mCheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    uint32 ReceivedCounter);
/*******************************************************************************
**                      Global Constant Definitions                           **
*******************************************************************************/

/*******************************************************************************
**                      Global Variable Definitions                           **
*******************************************************************************/

/*******************************************************************************
**                      Private Constant Definitions                          **
*******************************************************************************/

/*******************************************************************************
**                      Private Variable Definitions                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
/**
 * Protects the array/buffer to be transmitted using the E2E profile 7m. 
 * This includes CRC calculation, handling of Counter, Data ID, Message Type,
 *  Message Result, and Source ID.
 * Service ID: 0x4b
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): ConfigPtr,Pointer to static configuration.
 *                 SourceID,A system-unique identifier of the Data Source.
 *                 MessageType,Type of the message (request/response)
 *                 MessageResult,Result of the message (OK/ERROR)
 *                 Length,Length of the data in bytes.
 * Parameters(INOUT): StatePtr,Pointer to port/data communication state.
 *                    DataPtr,Pointer to Data to be transmitted.
 * Parameters(OUT): NA
 * Return value: Std_ReturnType,E2E_E_INPUTERR_NULL E2E_E_INPUTERR_WRONG E2E_E_INTERR E2E_E_OK.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(Std_ReturnType, E2E_CODE) E2E_P07mProtect(
    P2CONST(E2E_P07mConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    P2VAR(E2E_P07mProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    uint32 SourceID,
    Std_MessageTypeType MessageType,
    Std_MessageResultType MessageResult,
    P2VAR(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr,
    uint32 Length)
{
    Std_ReturnType Ret;
    uint32 Offset;
    uint64 ComputedCRC;

    /*Verify inputs of the protect function*/
    if ((ConfigPtr != NULL_PTR) && (StatePtr != NULL_PTR) && (DataPtr != NULL_PTR))
    {
        if ((Length >= (ConfigPtr->MinDataLength / 8u)) && (Length <= (ConfigPtr->MaxDataLength / 8u))
            && (SourceID <= 0xFFFFFFFu)
            && (MessageType >= 0) && (MessageType <= 1)
            && (MessageResult >= 0) && (MessageResult <= 1))
        {
            Ret = E2E_E_OK;
        }
        else
        {
            Ret = E2E_E_INPUTERR_WRONG;
        }
    }
    else
    {
        Ret = E2E_E_INPUTERR_NULL;
    }

    if (E2E_E_OK == Ret)
    {
        /*Compute offset[*/
        Offset = ConfigPtr->Offset / 8u;
        /*Copy 4-byte Length on bytes Data[Offset+8...Offset+11] in Big Endian order*/
        DataPtr[Offset + 8u] = (uint8)(Length >> 24u);
        DataPtr[Offset + 9u] = (uint8)(Length >> 16u);
        DataPtr[Offset + 10u] = (uint8)(Length >> 8u);
        DataPtr[Offset + 11u] = (uint8)Length;
        /*Copy 4-byte State->Counter on bytes Data[Offset+12...Offset+15] in Big Endian order*/
        DataPtr[Offset + 12u] = (uint8)(StatePtr->Counter >> 24u);
        DataPtr[Offset + 13u] = (uint8)(StatePtr->Counter >> 16u);
        DataPtr[Offset + 14u] = (uint8)(StatePtr->Counter >> 8u);
        DataPtr[Offset + 15u] = (uint8)(StatePtr->Counter);
        /*Copy 4-byte Config->DataID to bytes Data[Offset+16...Offset+19] in Big Endian order*/
        DataPtr[Offset + 16u] = (uint8)(ConfigPtr->DataID >> 24u);
        DataPtr[Offset + 17u] = (uint8)(ConfigPtr->DataID >> 16u);
        DataPtr[Offset + 18u] = (uint8)(ConfigPtr->DataID >> 8u);
        DataPtr[Offset + 19u] = (uint8)(ConfigPtr->DataID);
        /*Copy 28-bit SourceID to bytes Data[Offset+20...Offset+23] in Big Endian order*/
        DataPtr[Offset + 20u] = (uint8)(SourceID >> 24u);
        DataPtr[Offset + 21u] = (uint8)(SourceID >> 16u);
        DataPtr[Offset + 22u] = (uint8)(SourceID >> 8u);
        DataPtr[Offset + 23u] = (uint8)(SourceID);
        /*Copy 2-bit MessageType to bits 0 and 1 of byte Data[Offset+20] in Big Endian order*/
        DataPtr[Offset + 20u] = (DataPtr[Offset + 20u] & 0x3F) | ((MessageType & 0x03) << 6u);
        /*Copy 2-bit MessageResult to the bits 2 and 3 of byte Data[Offset+20] in Big Endian order*/
        DataPtr[Offset + 20u] = (DataPtr[Offset + 20u] & 0xCF) | ((MessageResult & 0x03) << 4u);
        /*Compute CRC*/
        if (ConfigPtr->Offset > 0u)
        {
            ComputedCRC = Crc_CalculateCRC64(&DataPtr[0], Offset, 0xFFFFFFFFFFFFFFFFu, TRUE);
            ComputedCRC = Crc_CalculateCRC64(&DataPtr[Offset + 8u], Length - Offset - 8u, ComputedCRC, FALSE);
        }
        else
        {
            ComputedCRC = Crc_CalculateCRC64(&DataPtr[Offset + 8u], Length - 8u, 0xFFFFFFFFFFFFFFFFu, TRUE);
        }
        /*Copy 8-byte local variable CRC on bytes Data[Offset...Offset+7] using big Endian order*/
        DataPtr[Offset] = (uint8)(ComputedCRC >> 56u);
        DataPtr[Offset + 1u] = (uint8)(ComputedCRC >> 48u);
        DataPtr[Offset + 2u] = (uint8)(ComputedCRC >> 40u);
        DataPtr[Offset + 3u] = (uint8)(ComputedCRC >> 32u);
        DataPtr[Offset + 4u] = (uint8)(ComputedCRC >> 24u);
        DataPtr[Offset + 5u] = (uint8)(ComputedCRC >> 16u);
        DataPtr[Offset + 6u] = (uint8)(ComputedCRC >> 8u);
        DataPtr[Offset + 7u] = (uint8)ComputedCRC;
        /*increment counter*/
        StatePtr->Counter++;
    }

    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 * Initializes the protection state.
 * Service ID: 0x4c
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): NA
 * Parameters(INOUT): StatePtr,Pointer to port/data communication state.
 * Parameters(OUT): NA
 * Return value: Std_ReturnType,E2E_E_INPUTERR_NULL E2E_E_OK.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(Std_ReturnType, E2E_CODE)
E2E_P07mProtectInit(P2VAR(E2E_P07mProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr)
{
    Std_ReturnType Ret;
    Ret = E2E_E_OK;

    if (NULL_PTR == StatePtr)
    {
        Ret = E2E_E_INPUTERR_NULL;
    }
    else
    {
        StatePtr->Counter = 0;
    }
    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 * Checks the Data received using the E2E profile 7m. This includes CRC calculation,
handling of Counter, Data ID, Message Type, Message Result, and Source ID.
The function checks only one single data in one cycle, it does not determine/
compute the accumulated state of the communication link.
This function is intended for usage at the data source (i.e., in case of C/S
communication at the client).
 * Service ID: 0x4f
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): ConfigPtr,Pointer to static configuration.
 *                 SourceID,A system-unique identifier of the Data Source.
 *                 MessageType,Type of the message (request/response)
 *                 MessageResult,Result of the message (OK/ERROR)
 *                 DataPtr,Pointer to received data.
 *                 Length,Length of the data in bytes.
 * Parameters(INOUT): StatePtr,Pointer to port/data communication state.
 * Parameters(OUT): NA
 * Return value: Std_ReturnType,E2E_E_INPUTERR_NULL E2E_E_INPUTERR_WRONG E2E_E_INTERR E2E_E_OK.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(Std_ReturnType, E2E_CODE)
E2E_P07mSourceCheck (
    P2CONST(E2E_P07mConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    P2VAR(E2E_P07mCheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    uint32 SourceID,
    Std_MessageTypeType MessageType,
    Std_MessageResultType MessageResult,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr,
    uint32 Length)
{
    Std_ReturnType Ret;
    uint32 Offset;
    uint32 ReceivedLength;
    uint32 ReceivedCounter;
    uint32 ReceivedDataID;
    uint32 ReceivedSourceID;
    uint8  ReceivedMessageType;
    uint8  ReceivedMessageResult;
    uint64 ReceivedCRC;
    uint64 ComputedCRC;
    boolean NewDataAvailable;

    NewDataAvailable = FALSE;
    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr))
    {
        Ret = E2E_E_INPUTERR_NULL;
    }
    else
    {
        if (((DataPtr != NULL_PTR) && (Length != 0u)) || ((NULL_PTR == DataPtr) && (0u == Length)))
        {
            if (NULL_PTR != DataPtr)
            {
                if ((SourceID <= 0xFFFFFFF)
                    && (Length >= (ConfigPtr->MinDataLength / 8u)) && (Length <= (ConfigPtr->MaxDataLength / 8u))
                    && (MessageType >= 0) && (MessageType <= 1)
                    && (MessageResult >= 0) && (MessageResult <= 1))
                {
                    NewDataAvailable = TRUE;
                    Ret = E2E_E_OK;
                }
                else
                {
                    Ret = E2E_E_INPUTERR_WRONG;
                }
            }
            else
            {
                Ret = E2E_E_OK;
            }
        }
        else
        {
            Ret = E2E_E_INPUTERR_WRONG;
        }

        if (E2E_E_OK == Ret)
        {
            if (TRUE == NewDataAvailable)
            {
                /*compute local variable uint16 Offset, which is in [byte]*/
                Offset = ConfigPtr->Offset / 8u;
                /*Copy bytes Data[Offset+8...Offset+11] in Big Endian order to uint32 local variable ReceivedLength*/
                ReceivedLength = (((uint32)DataPtr[Offset + 8u]) << 24u) + (((uint32)DataPtr[Offset + 9u]) << 16u)
                               + (((uint32)DataPtr[Offset + 10u]) << 8u) + (uint32)DataPtr[Offset + 11u];
                /*Copy bytes Data[Offset+12...Offset+15] in Big Endian order on uint32 local variable ReceivedCounter*/
                ReceivedCounter = (((uint32)DataPtr[Offset + 12u]) << 24u) + (((uint32)DataPtr[Offset + 13u]) << 16u)
                                + (((uint32)DataPtr[Offset + 14u]) << 8u) + (uint32)DataPtr[Offset + 15u];
                /*Copy bytes Data[Offset+16...Offset+19] in Big Endian order on uint32 local variable ReceivedDataID*/
                ReceivedDataID = ((uint32)DataPtr[Offset + 16u] << 24u) + ((uint32)DataPtr[Offset + 17u] << 16u)
                                 + ((uint32)DataPtr[Offset + 18u] << 8u) + (uint32)(DataPtr[Offset + 19u]);
                /*Copy 28 least significant bits of Data[Offset+20...Offset+23] in
                Big Endian order on uint32 local variable ReceivedSourceID*/
                ReceivedSourceID = ((uint32)DataPtr[Offset + 20u] << 24u) + ((uint32)DataPtr[Offset + 21u] << 16u)
                                 + ((uint32)DataPtr[Offset + 22u] << 8u) + (uint32)(DataPtr[Offset + 23u]);
                ReceivedSourceID &= 0x0FFFFFFFu;
                /*Copy 2-bit MessageType from the bits 0 and 1 of byte Data[Offset+20]
                 in Big Endian order on uint8 local variable ReceivedMessageType*/
                ReceivedMessageType = (DataPtr[Offset + 20u] & 0xC0) >> 6u;
                /*Copy 2-bit MessageResult from the bits 2 and 3 of byte Data[Offset+20]
                 in Big Endian order on uint8 local variable ReceivedMessageResult*/
                ReceivedMessageResult = (DataPtr[Offset + 20u] & 0x30) >> 4u;
                /*Copy bytes Data[Offset...Offset+7] using big Endian order on 8-byte local variable ReceivedCRC*/
                ReceivedCRC = (((uint64)DataPtr[Offset]) << 56u) + (((uint64)DataPtr[Offset + 1u]) << 48u)
                            + (((uint64)DataPtr[Offset + 2u]) << 40u) + (((uint64)DataPtr[Offset + 3u]) << 32u)
                            + (((uint64)DataPtr[Offset + 4u]) << 24u) + (((uint64)DataPtr[Offset + 5u]) << 16u)
                            + (((uint64)DataPtr[Offset + 6u]) << 8u) + ((uint64)DataPtr[Offset + 7u]);
                /*compute CRC*/
                if (ConfigPtr->Offset > 0u)
                {
                    ComputedCRC = Crc_CalculateCRC64(&DataPtr[0], Offset, 0xFFFFFFFFFFFFFFFFu, TRUE);
                    ComputedCRC = Crc_CalculateCRC64(&DataPtr[Offset + 8u], Length - Offset - 8u, ComputedCRC, FALSE);
                }
                else
                {
                    ComputedCRC = Crc_CalculateCRC64(&DataPtr[Offset + 8u], Length - 8u, 0xFFFFFFFFFFFFFFFFu, TRUE);
                }
                /*Do Checks*/
                if (ReceivedCRC == ComputedCRC
                    && ReceivedDataID == ConfigPtr->DataID
                    && ReceivedSourceID == SourceID
                    && ReceivedMessageType == MessageType
                    && ReceivedMessageResult == MessageResult
                    && ReceivedLength == Length)
                {
                    E2E_P07mCheck_Seqence(ConfigPtr, StatePtr, ReceivedCounter);
                }
                else
                {
                    StatePtr->Status = E2E_P07MSTATUS_ERROR;
                }
            }
            else
            {
                /*Do Checks*/
                StatePtr->Status = E2E_P07MSTATUS_NONEWDATA;
            }
        }
    }

    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 * Checks the Data received using the E2E profile 4m. This includes CRC calculation,
handling of Counter, Data ID, Message Type, Message Result, and Source ID.
The function checks only one single data in one cycle, it does not determine/
compute the accumulated state of the communication link.
This function is intended for usage at the data sink (i.e., in case of C/S
communication at the server).
 * Service ID: 0x4e
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): ConfigPtr,Pointer to static configuration.
 *                 MessageType,Type of the message (request/response)
 *                 MessageResult,Result of the message (OK/ERROR)
 *                 DataPtr,Pointer to received data.
 *                 Length,Length of the data in bytes.
 * Parameters(INOUT): StatePtr,Pointer to port/data communication state.
 * Parameters(OUT): SourceID,A system-unique identifier of the Data Source.
 * Return value: Std_ReturnType,E2E_E_INPUTERR_NULL E2E_E_INPUTERR_WRONG E2E_E_INTERR E2E_E_OK.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(Std_ReturnType, E2E_CODE) E2E_P07mSinkCheck (
    P2CONST(E2E_P07mConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    P2VAR(E2E_P07mCheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2VAR(uint32, AUTOMATIC, E2E_APPL_DATA) SourceID,
    Std_MessageTypeType MessageType,
    Std_MessageResultType MessageResult,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr,
    uint32 Length)
{
    Std_ReturnType Ret;
    uint32 Offset;
    uint32 ReceivedLength;
    uint32 ReceivedCounter;
    uint32 ReceivedDataID;
    uint8  ReceivedMessageType;
    uint8  ReceivedMessageResult;
    uint64 ReceivedCRC;
    uint64 ComputedCRC;
    boolean NewDataAvailable;

    NewDataAvailable = FALSE;
    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr))
    {
        Ret = E2E_E_INPUTERR_NULL;
    }
    else
    {
        if (((DataPtr != NULL_PTR) && (Length != 0u)) || ((NULL_PTR == DataPtr) && (0u == Length)))
        {
            if (NULL_PTR != DataPtr)
            {
                if ((Length >= (ConfigPtr->MinDataLength / 8u)) && (Length <= (ConfigPtr->MaxDataLength / 8u))
                    && (MessageType >= 0) && (MessageType <= 1)
                    && (MessageResult >= 0) && (MessageResult <= 1))
                {
                    NewDataAvailable = TRUE;
                    Ret = E2E_E_OK;
                }
                else
                {
                    Ret = E2E_E_INPUTERR_WRONG;
                }
            }
            else
            {
                Ret = E2E_E_OK;
            }
        }
        else
        {
            Ret = E2E_E_INPUTERR_WRONG;
        }

        if (E2E_E_OK == Ret)
        {
            if (TRUE == NewDataAvailable)
            {
                /*compute local variable uint16 Offset, which is in [byte]*/
                Offset = ConfigPtr->Offset / 8u;
                /*Copy bytes Data[Offset+8...Offset+11] in Big Endian order to uint32 local variable ReceivedLength*/
                ReceivedLength = (((uint32)DataPtr[Offset + 8u]) << 24u) + (((uint32)DataPtr[Offset + 9u]) << 16u)
                               + (((uint32)DataPtr[Offset + 10u]) << 8u) + (uint32)DataPtr[Offset + 11u];
                /*Copy bytes Data[Offset+12...Offset+15] in Big Endian order on uint32 local variable ReceivedCounter*/
                ReceivedCounter = (((uint32)DataPtr[Offset + 12u]) << 24u) + (((uint32)DataPtr[Offset + 13u]) << 16u)
                                + (((uint32)DataPtr[Offset + 14u]) << 8u) + (uint32)DataPtr[Offset + 15u];
                /*Copy bytes Data[Offset+16...Offset+19] in Big Endian order on uint32 local variable ReceivedDataID*/
                ReceivedDataID = ((uint32)DataPtr[Offset + 16u] << 24u) + ((uint32)DataPtr[Offset + 17u] << 16u)
                                 + ((uint32)DataPtr[Offset + 18u] << 8u) + (uint32)(DataPtr[Offset + 19u]);
                /*Copy 28 least significant bits of Data[Offset+20...Offset+23] in
                Big Endian order on uint32 output argument SourceID*/
                *SourceID = ((uint32)DataPtr[Offset + 20u] << 24u) + ((uint32)DataPtr[Offset + 21u] << 16u)
                                 + ((uint32)DataPtr[Offset + 22u] << 8u) + (uint32)(DataPtr[Offset + 23u]);
                *SourceID &= 0x0FFFFFFFu;
                /*Copy 2-bit MessageType from the bits 0 and 1 of byte Data[Offset+20]
                 in Big Endian order on uint8 local variable ReceivedMessageType*/
                ReceivedMessageType = (DataPtr[Offset + 20u] & 0xC0) >> 6u;
                /*Copy 2-bit MessageResult from the bits 2 and 3 of byte Data[Offset+20]
                 in Big Endian order on uint8 local variable ReceivedMessageResult*/
                ReceivedMessageResult = (DataPtr[Offset + 20u] & 0x30) >> 4u;
                /*Copy bytes Data[Offset...Offset+7] using big Endian order on 8-byte local variable ReceivedCRC*/
                ReceivedCRC = (((uint64)DataPtr[Offset]) << 56u) + (((uint64)DataPtr[Offset + 1u]) << 48u)
                            + (((uint64)DataPtr[Offset + 2u]) << 40u) + (((uint64)DataPtr[Offset + 3u]) << 32u)
                            + (((uint64)DataPtr[Offset + 4u]) << 24u) + (((uint64)DataPtr[Offset + 5u]) << 16u)
                            + (((uint64)DataPtr[Offset + 6u]) << 8u) + ((uint64)DataPtr[Offset + 7u]);
                /*compute CRC*/
                if (ConfigPtr->Offset > 0u)
                {
                    ComputedCRC = Crc_CalculateCRC64(&DataPtr[0], Offset, 0xFFFFFFFFFFFFFFFFu, TRUE);
                    ComputedCRC = Crc_CalculateCRC64(&DataPtr[Offset + 8u], Length - Offset - 8u, ComputedCRC, FALSE);
                }
                else
                {
                    ComputedCRC = Crc_CalculateCRC64(&DataPtr[Offset + 8u], Length - Offset - 8u, 0xFFFFFFFFFFFFFFFFu, TRUE);
                }
                /*Do Checks*/
                if (ReceivedCRC == ComputedCRC
                    && ReceivedDataID == ConfigPtr->DataID
                    && ReceivedMessageType == MessageType
                    && ReceivedMessageResult == MessageResult
                    && ReceivedLength == Length)
                {
                    E2E_P07mCheck_Seqence(ConfigPtr, StatePtr, ReceivedCounter);
                }
                else
                {
                    StatePtr->Status = E2E_P07MSTATUS_ERROR;
                }
            }
            else
            {
                /*Do Checks*/
                StatePtr->Status = E2E_P07MSTATUS_NONEWDATA;
            }
        }
    }

    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 * Initializes the check state
 * Service ID: 0x49
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): NA
 * Parameters(INOUT): StatePtr,Pointer to port/data communication state.
 * Parameters(OUT): NA
 * Return value: Std_ReturnType,E2E_E_INPUTERR_NULL E2E_E_OK.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(Std_ReturnType, E2E_CODE) E2E_P07mCheckInit(P2VAR(E2E_P07mCheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr)
{
    uint8 Ret;
    // [SWS_E2E_00597]
    if (NULL_PTR == StatePtr)
    {
        Ret = E2E_E_INPUTERR_NULL;
    }
    else
    {
        StatePtr->Counter = 0xFFFFFFFF;
        StatePtr->Status = E2E_P07MSTATUS_ERROR;
        Ret = E2E_E_OK;
    }
    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 * The function maps the check status of Profile 7m to a generic check status, which
can be used by E2E state machine check function. The E2E Profile 7m delivers a
more fine-granular status, but this is not relevant for the E2E state machine.
 * Service ID: 0x4a
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): CheckReturn,Return value of the E2E_P07mCheck function
 *                 Status,Status determined by E2E_P07mCheck function
 * Parameters(INOUT): NA
 * Parameters(OUT): NA
 * Return value: E2E_PCheckStatusType,Profile-independent status of
 *  the reception on one single Data in one cycle.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(E2E_PCheckStatusType, E2E_CODE) E2E_P07mMapStatusToSM(Std_ReturnType CheckReturn, E2E_P07mCheckStatusType Status)
{
    E2E_PCheckStatusType Ret;

    if (E2E_E_OK != CheckReturn)
    {
        Ret = E2E_P_ERROR;
    }
    else
    {
        switch (Status)
        {
        case E2E_P07MSTATUS_OK:
        case E2E_P07MSTATUS_OKSOMELOST:
            Ret = E2E_P_OK;
            break;
        case E2E_P07MSTATUS_ERROR:
            Ret = E2E_P_ERROR;
            break;
        case E2E_P07MSTATUS_REPEATED:
            Ret = E2E_P_REPEATED;
            break;
        case E2E_P07MSTATUS_NONEWDATA:
            Ret = E2E_P_NONEWDATA;
            break;
        case E2E_P07MSTATUS_WRONGSEQUENCE:
            Ret = E2E_P_WRONGSEQUENCE;
            break;
        default:
            Ret = E2E_P_ERROR;
            break;
        }
    }
    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/*******************************************************************************
**                      Private Function Definitions                          **
*******************************************************************************/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
static void E2E_P07mCheck_Seqence(
    P2CONST(E2E_P07mConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    P2VAR(E2E_P07mCheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    uint32 ReceivedCounter)
{
    sint32 DeltaCounter;
    /*Compute local variable DeltaCounter: ReceivedCounter - State->Counter
     * (taking into wrap around 0xFFFF)*/
    DeltaCounter = (sint16)ReceivedCounter - (sint16)StatePtr->Counter;

    if ((DeltaCounter <= (sint16)ConfigPtr->MaxDeltaCounter) && (DeltaCounter >= (sint16)0))
    {
        if (DeltaCounter > 0)
        {
            if (1 == DeltaCounter)
            {
                StatePtr->Status = E2E_P07MSTATUS_OK;
            }
            else
            {
                StatePtr->Status = E2E_P07MSTATUS_OKSOMELOST;
            }
        }
        else
        {
            StatePtr->Status = E2E_P07MSTATUS_REPEATED;
        }
    }
    else
    {
        StatePtr->Status = E2E_P07MSTATUS_WRONGSEQUENCE;
    }
    StatePtr->Counter = ReceivedCounter;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
