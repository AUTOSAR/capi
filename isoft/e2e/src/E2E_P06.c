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
**  FILENAME    : E2E_P06.c                                                   **
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
 * V2.0.1 [2020/9/01] [YangBo] change E2E_P06CheckInit counter init to max
 */
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "E2E_P06.h"
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
 * Protects the array/buffer to be transmitted using the E2E profile 6.
 * This includes checksum calculation, handling of counter.
 * Service ID: 0x2b
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
E2E_P06Protect(
    P2CONST(E2E_P06ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    P2VAR(E2E_P06ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2VAR(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr,
    uint16 Length)
{
    Std_ReturnType Ret;
    uint16 Offset;
    uint16 ComputedCRC;
    uint8 DataID_Hbyte;
    uint8 DataID_Lbyte;

    /*Verify inputs of the protect function return*/
    if ((ConfigPtr != NULL_PTR) && (StatePtr != NULL_PTR) && (DataPtr != NULL_PTR))
    {
        if ((Length >= (ConfigPtr->MinDataLength / 8u)) && (Length <= (ConfigPtr->MaxDataLength / 8u)))
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
        DataID_Hbyte = (uint8)(ConfigPtr->DataID >> 8);
        /*Compute offset[*/
        Offset = ConfigPtr->Offset / 8u;
        /*Write Length*/
        DataPtr[Offset + 2u] = (uint8)(Length >> 8u);
        DataPtr[Offset + 3u] = (uint8)Length;
        /*Write Counter*/
        DataPtr[Offset + 4u] = StatePtr->Counter;
        /*Compute CRC*/
        if (ConfigPtr->Offset > 0u)
        {
            ComputedCRC = Crc_CalculateCRC16(&DataPtr[0], Offset, 0xFFFFu, TRUE);
            ComputedCRC = Crc_CalculateCRC16(&DataPtr[Offset + 2u], Length - Offset - 2u, ComputedCRC, FALSE);
        }
        else
        {
            ComputedCRC = Crc_CalculateCRC16(&DataPtr[Offset + 2u], Length - 2u, 0xFFFFu, TRUE);
        }
        ComputedCRC = Crc_CalculateCRC16(&DataID_Hbyte, 1, ComputedCRC, FALSE);
        ComputedCRC = Crc_CalculateCRC16(&DataID_Lbyte, 1, ComputedCRC, FALSE);
        /*Write CRC*/
        DataPtr[Offset] = (uint8)(ComputedCRC >> 8u);
        DataPtr[Offset + 1u] = (uint8)ComputedCRC;
        /*increment counter*/
        StatePtr->Counter++;
    }

    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * Initializes the protection state.
 * Service ID: 0x2c
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
FUNC(Std_ReturnType, E2E_CODE) E2E_P06ProtectInit(P2VAR(E2E_P06ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr)
{
    Std_ReturnType Ret;

    /*@SWS_E2E_00456*/
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
 * Checks the Data received using the E2E profile 6.
 * This includes CRC calculation, handling of Counter.
 * Service ID: 0x2d
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
E2E_P06Check(
    P2CONST(E2E_P06ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    P2VAR(E2E_P06CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr,
    uint16 Length)
{
    Std_ReturnType Ret;
    boolean NewDataAvailable;
    uint16 Offset;
    uint16 ReceivedLength;
    uint8 ReceivedCounter;
    uint16 ReceivedCRC;
    uint16 ComputedCRC;
    sint8 DeltaCounter;
    uint8 DataID_Hbyte;
    uint8 DataID_Lbyte;

    /*Verify inputs of the check function*/
    NewDataAvailable = FALSE;
    if ((ConfigPtr != NULL_PTR) && (StatePtr != NULL_PTR))
    {
        if (((DataPtr != NULL_PTR) && (Length != 0u)) || ((NULL_PTR == DataPtr) && (0u == Length)))
        {
            if (DataPtr != NULL_PTR)
            {
                if ((Length >= (ConfigPtr->MinDataLength / 8u)) && (Length <= (ConfigPtr->MaxDataLength / 8u)))
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
            /*Compute offset*/
            Offset = ConfigPtr->Offset / 8u;
            /*Read Length*/
            ReceivedLength = (((uint16)DataPtr[Offset + 2u]) << 8u) + (uint16)DataPtr[Offset + 3u];
            /*Read Counter*/
            ReceivedCounter = DataPtr[Offset + 4u];
            /*Read CRC*/
            ReceivedCRC = (((uint16)DataPtr[Offset]) << 8u) + (uint16)DataPtr[Offset + 1u];
            /*Compute CRC*/
            if (ConfigPtr->Offset > 0u)
            {
                ComputedCRC = Crc_CalculateCRC16(&DataPtr[0], Offset, 0xFFFFu, TRUE);
                ComputedCRC = Crc_CalculateCRC16(&DataPtr[Offset + 2u], Length - Offset - 2u, ComputedCRC, FALSE);
            }
            else
            {
                ComputedCRC = Crc_CalculateCRC16(&DataPtr[Offset + 2u], Length - 2u, 0xFFFFu, TRUE);
            }
            ComputedCRC = Crc_CalculateCRC16(&DataID_Hbyte, 1, ComputedCRC, FALSE);
            ComputedCRC = Crc_CalculateCRC16(&DataID_Lbyte, 1, ComputedCRC, FALSE);
        }
        /*Do Checks*/
        if (TRUE == NewDataAvailable)
        {
            if (ReceivedCRC == ComputedCRC)
            {
                if (ReceivedLength == Length)
                {
                    /*taking into wrap around 0xFF*/
                    DeltaCounter = (sint8)ReceivedCounter - (sint8)StatePtr->Counter;
                    if ((DeltaCounter <= (sint8)(ConfigPtr->MaxDeltaCounter)) && (DeltaCounter >= (sint8)0u))
                    {
                        if (DeltaCounter > 0)
                        {
                            if (1 == DeltaCounter)
                            {
                                StatePtr->Status = E2E_P06STATUS_OK;
                            }
                            else
                            {
                                StatePtr->Status = E2E_P06STATUS_OKSOMELOST;
                            }
                        }
                        else
                        {
                            StatePtr->Status = E2E_P06STATUS_REPEATED;
                        }
                    }
                    else
                    {
                        StatePtr->Status = E2E_P06STATUS_WRONGSEQUENCE;
                    }
                    StatePtr->Counter = ReceivedCounter;
                }
                else
                {
                    StatePtr->Status = E2E_P06STATUS_ERROR;
                }
            }
            else
            {
                StatePtr->Status = E2E_P06STATUS_ERROR;
            }
        }
        else
        {
            StatePtr->Status = E2E_P06STATUS_NONEWDATA;
        }
    }

    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * Initializes the check state
 * Service ID: 0x2e
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
FUNC(Std_ReturnType, E2E_CODE) E2E_P06CheckInit(P2VAR(E2E_P06CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr)
{
    Std_ReturnType Ret;

    /*@SWS_E2E_00459*/
    if (NULL_PTR == StatePtr)
    {
        Ret = E2E_E_INPUTERR_NULL;
    }
    else
    {
        StatePtr->Counter = 0xFF;
        StatePtr->Status = E2E_P06STATUS_ERROR;
        Ret = E2E_E_OK;
    }
    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * The function maps the check status of Profile 6 to a generic check status, which can be used by E2E state machine
 * check function. The E2E Profile 6 delivers a more fine-granular status, but this is not relevant for the E2E state
 * machine. Service ID: 0x2f Sync/Async: synchronous Reentrancy: Reentrant Parameters(IN): CheckReturn,Return value of
 * the E2E_P06Check function Status,Status determined by E2E_P06Check function Parameters(INOUT): NA Parameters(OUT): NA
 * Return value: E2E_PCheckStatusType,Profile-independent status of the reception on one single Data in one cycle.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(E2E_PCheckStatusType, E2E_CODE) E2E_P06MapStatusToSM(Std_ReturnType CheckReturn, E2E_P06CheckStatusType Status)
{
    E2E_PCheckStatusType Ret;

    /*@SWS_E2E_00462*/
    if (CheckReturn != E2E_E_OK)
    {
        Ret = E2E_P_ERROR;
    }
    /*@SWS_E2E_00461*/
    else
    {
        switch (Status)
        {
        case E2E_P06STATUS_OK:
        case E2E_P06STATUS_OKSOMELOST:
            Ret = E2E_P_OK;
            break;
        case E2E_P06STATUS_ERROR:
            Ret = E2E_P_ERROR;
            break;
        case E2E_P06STATUS_REPEATED:
            Ret = E2E_P_REPEATED;
            break;
        case E2E_P06STATUS_NONEWDATA:
            Ret = E2E_P_NONEWDATA;
            break;
        case E2E_P06STATUS_WRONGSEQUENCE:
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
