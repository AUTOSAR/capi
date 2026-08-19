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
**  FILENAME    : E2E_P05.c                                                   **
**                                                                            **
**  Created on  :                                                             **
**  Author      : YangBo                                                      **
**  Vendor      :                                                             **
**  DESCRIPTION :                                                             **
**                                                                            **
**  SPECIFICATION(S) :   AUTOSAR classic Platform R19-11                      **
**                                                                            **
*******************************************************************************/
/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/* <VERSION> <DATE> <AUTHOR> <REVISION LOG>
 * V2.0.0 [2020/8/17] [YangBo] Initial Vertion.
 * V2.0.1 [2020/9/01] [YangBo] change E2E_P05CheckInit counter init to max
 */
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "E2E_P05.h"
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
 * Protects the array/buffer to be transmitted using the E2E profile 5.
 * This includes checksum calculation, handling of counter.
 * Service ID: 0x26
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): ConfigPtr,Pointer to static configuration.
 *                 Length,Length of the data in bytes.
 * Parameters(INOUT): StatePtr,Pointer to port/data communication state.
 *                    DataPtr,Pointer to Data to be transmitted.
 * Parameters(OUT): NA
 * Return value: Std_ReturnType,E2E_E_INPUTERR_NULL E2E_E_INPUTERR_WRONG E2E_E_INTERR E2E_E_OK.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(Std_ReturnType, E2E_CODE)
E2E_P05Protect(
    P2CONST(E2E_P05ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    P2VAR(E2E_P05ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2VAR(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr,
    uint16 Length)
{
    Std_ReturnType Ret;
    uint16 Offset;
    uint16 ComputedCRC;
    uint8 DataID_Hbyte;
    uint8 DataID_Lbyte;

    /*Verify inputs of the protect function*/
    if ((ConfigPtr != NULL_PTR) && (StatePtr != NULL_PTR) && (DataPtr != NULL_PTR))
    {
        if (Length == (ConfigPtr->DataLength / 8u))
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
        DataID_Lbyte = (uint8)(ConfigPtr->DataID);
        DataID_Hbyte = (uint8)(ConfigPtr->DataID >> 8u);
        /*Compute Offset*/
        Offset = ConfigPtr->Offset / 8u;
        DataPtr[Offset + 2u] = StatePtr->Counter;
        /*Compute CRC*/
        if ((ConfigPtr->Offset) > 0u)
        {
            ComputedCRC = Crc_CalculateCRC16(&DataPtr[0], (uint32)Offset, 0xFFFFu, TRUE);
            ComputedCRC = Crc_CalculateCRC16(&DataPtr[Offset + 2u], (uint32)(Length - Offset - 2u), ComputedCRC, FALSE);
        }
        else
        {
            ComputedCRC = Crc_CalculateCRC16(&DataPtr[Offset + 2u], (uint32)(Length - 2u), 0xFFFFu, TRUE);
        }
        ComputedCRC = Crc_CalculateCRC16(&DataID_Lbyte, (uint32)1, ComputedCRC, FALSE);
        ComputedCRC = Crc_CalculateCRC16(&DataID_Hbyte, (uint32)1, ComputedCRC, FALSE);
        /*Copy 2-byte local variable CRC on bytes Data[Offset+0...Offset+1] using Little Endian order*/
        DataPtr[Offset] = (uint8)ComputedCRC;
        DataPtr[Offset + 1u] = (uint8)(ComputedCRC >> 8u);
        StatePtr->Counter++;
    }

    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * Initializes the protection state.
 * Service ID: 0x27
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
FUNC(Std_ReturnType, E2E_CODE) E2E_P05ProtectInit(P2VAR(E2E_P05ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr)
{
    Std_ReturnType Ret;

    if (NULL_PTR == StatePtr)
    {
        Ret = E2E_E_INPUTERR_NULL;
    }
    else
    {
        StatePtr->Counter = 0;
        Ret = E2E_E_OK;
    }

    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * Checks the Data received using the E2E profile 5.
 * This includes CRC calculation, handling of Counter.
 * Service ID: 0x28
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): ConfigPtr,Pointer to static configuration.
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
E2E_P05Check(
    P2CONST(E2E_P05ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    P2VAR(E2E_P05CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr,
    uint16 Length)
{
    Std_ReturnType Ret;
    boolean NewDataAvailable;
    uint16 Offset;
    uint8 ReceivedCounter;
    uint16 ReceivedCRC;
    uint16 ComputedCRC;
    sint8 DeltaCounter;
    uint8 DataID_Hbyte;
    uint8 DataID_Lbyte;

    /*CRCVerify inputs of the check function*/
    NewDataAvailable = FALSE;
    if ((ConfigPtr != NULL_PTR) && (StatePtr != NULL_PTR))
    {
        if (((DataPtr != NULL_PTR) && (Length != 0u)) || ((NULL_PTR == DataPtr) && (0u == Length)))
        {
            if (DataPtr != NULL_PTR)
            {
                if (Length == (ConfigPtr->DataLength / 8u))
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
    }
    else
    {
        Ret = E2E_E_INPUTERR_NULL;
    }

    if (E2E_E_OK == Ret)
    {
        DataID_Lbyte = (uint8)(ConfigPtr->DataID);
        DataID_Hbyte = (uint8)(ConfigPtr->DataID >> 8u);
        if (TRUE == NewDataAvailable)
        {
            /*Compute Offset*/
            Offset = ConfigPtr->Offset / 8u;
            ReceivedCounter = DataPtr[Offset + 2u];
            /*Copy bytes Data[Offset+0...Offset+1] using Li ttle Endian order on 2-byte local variable ReceivedCRC*/
            ReceivedCRC = (uint16)DataPtr[Offset] + (((uint16)DataPtr[Offset + 1u]) << 8u);
            /*Compute CRC*/
            if ((ConfigPtr->Offset) > 0u)
            {
                ComputedCRC = Crc_CalculateCRC16(&DataPtr[0], (uint32)Offset, 0xFFFFu, TRUE);
                ComputedCRC =
                    Crc_CalculateCRC16(&DataPtr[Offset + 2u], (uint32)(Length - Offset - 2u), ComputedCRC, FALSE);
            }
            else
            {
                ComputedCRC = Crc_CalculateCRC16(&DataPtr[Offset + 2u], (uint32)(Length - 2u), 0xFFFFu, TRUE);
            }
            ComputedCRC = Crc_CalculateCRC16(&DataID_Lbyte, (uint32)1, ComputedCRC, FALSE);
            ComputedCRC = Crc_CalculateCRC16(&DataID_Hbyte, (uint32)1, ComputedCRC, FALSE);
        }
        /*Do Checks*/
        if (TRUE == NewDataAvailable)
        {
            if (ReceivedCRC == ComputedCRC)
            {
                /*taking into wrap around 0xFF*/
                DeltaCounter = (sint8)ReceivedCounter - (sint8)StatePtr->Counter;
                if ((DeltaCounter <= (sint8)ConfigPtr->MaxDeltaCounter) && (DeltaCounter >= 0))
                {
                    if (DeltaCounter > 0)
                    {
                        if (1 == DeltaCounter)
                        {
                            StatePtr->Status = E2E_P05STATUS_OK;
                        }
                        else
                        {
                            StatePtr->Status = E2E_P05STATUS_OKSOMELOST;
                        }
                    }
                    else
                    {
                        StatePtr->Status = E2E_P05STATUS_REPEATED;
                    }
                }
                else
                {
                    StatePtr->Status = E2E_P05STATUS_WRONGSEQUENCE;
                }
                StatePtr->Counter = ReceivedCounter;
            }
            else
            {
                StatePtr->Status = E2E_P05STATUS_ERROR;
            }
        }
        else
        {
            StatePtr->Status = E2E_P05STATUS_NONEWDATA;
        }
    }

    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * Initializes the check state
 * Service ID: 0x29
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): NA
 * Parameters(INOUT): State,Pointer to port/data communication state.
 * Parameters(OUT): NA
 * Return value: Std_ReturnType,E2E_E_INPUTERR_NULL E2E_E_OK.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(Std_ReturnType, E2E_CODE) E2E_P05CheckInit(P2VAR(E2E_P05CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr)
{
    Std_ReturnType Ret;

    /*@SWS_E2E_00451*/
    if (NULL_PTR == StatePtr)
    {
        Ret = E2E_E_INPUTERR_NULL;
    }
    else
    {
        StatePtr->Counter = 0xFF;
        StatePtr->Status = E2E_P05STATUS_ERROR;
        Ret = E2E_E_OK;
    }
    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * The function maps the check status of Profile 5 to a generic check status, which can be used by E2E state machine
 * check function. The E2E Profile 5 delivers a more fine-granular status, but this is not relevant for the E2E state
 * machine. Service ID: 0x2a Sync/Async: synchronous Reentrancy: Reentrant Parameters(IN): CheckReturn,Return value of
 * the E2E_P05Check function Status,Status determined by E2E_P05Check function Parameters(INOUT): NA Parameters(OUT): NA
 * Return value: E2E_PCheckStatusType,Profile-independent status of the reception on one single Data in one cycle.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(E2E_PCheckStatusType, E2E_CODE) E2E_P05MapStatusToSM(Std_ReturnType CheckReturn, E2E_P05CheckStatusType Status)
{
    E2E_PCheckStatusType Ret;

    /*@SWS_E2E_00454*/
    if (E2E_E_OK != CheckReturn)
    {
        Ret = E2E_P_ERROR;
    }
    /*@SWS_E2E_00453*/
    else
    {
        switch (Status)
        {
        case E2E_P05STATUS_OK:
        case E2E_P05STATUS_OKSOMELOST:
            Ret = E2E_P_OK;
            break;
        case E2E_P05STATUS_ERROR:
            Ret = E2E_P_ERROR;
            break;
        case E2E_P05STATUS_REPEATED:
            Ret = E2E_P_REPEATED;
            break;
        case E2E_P05STATUS_NONEWDATA:
            Ret = E2E_P_NONEWDATA;
            break;
        case E2E_P05STATUS_WRONGSEQUENCE:
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
