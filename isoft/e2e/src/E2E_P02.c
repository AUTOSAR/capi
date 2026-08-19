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
**  FILENAME    : E2E_P02.c                                                   **
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
 * V1.0.0 [2020/8/17] [YangBo] Initial Vertion.
 * V1.0.1 [2020/9/01] [YangBo] change E2E_P02CheckInit WaitForFirstData = TRUE
 */
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "E2E.h"
#include "E2E_P02.h"
/*******************************************************************************
**                      Imported Compiler Switch Check                        **
*******************************************************************************/

/*******************************************************************************
**                       Version  Check                                       **
*******************************************************************************/

/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/
#define E2E_P02_MAX_COUNTER_VALUE 15u
/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/

#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
static FUNC(Std_ReturnType, E2E_CODE) E2E_P02ProtectVerifyInputs(
    P2CONST(E2E_P02ConfigType, AUTOMATIC, E2E_APPL_CONST) ConfigPtr,
    P2CONST(E2E_P02ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
static FUNC(Std_ReturnType, E2E_CODE) E2E_P02CheckVerifyInputs(
    P2CONST(E2E_P02ConfigType, AUTOMATIC, E2E_APPL_CONST) ConfigPtr,
    P2CONST(E2E_P02CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
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
 * Protects the array/buffer to be transmitted using the E2E profile 2.
 * This includes checksum calculation, handling of sequence counter and Data ID.
 * Service ID: 0x03
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): ConfigPtr,Pointer to static configuration.
 * Parameters(INOUT): StatePtr,Pointer to port/data communication state.
 *                    DataPtr,Pointer to Data to be transmitted.
 * Parameters(OUT): NA
 * Return value: Std_ReturnType,E2E_E_INPUTERR_NULL E2E_E_INPUTERR_WRONG E2E_E_INTERR E2E_E_OK.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(Std_ReturnType, E2E_CODE)
E2E_P02Protect(
    P2CONST(E2E_P02ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    P2VAR(E2E_P02ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2VAR(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr)
{
    uint8 Ret = E2E_P02ProtectVerifyInputs(ConfigPtr, StatePtr, DataPtr);
    if (E2E_E_OK == Ret)
    {
        uint8 DataID;
        uint8 CRC;

        if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr) || (NULL_PTR == DataPtr))
        {
            Ret = E2E_E_INPUTERR_NULL;
        }
        else
        {
            if (StatePtr->Counter < 15u)
            {
                StatePtr->Counter++;
            }
            else
            {
                StatePtr->Counter = 0;
            }
            *(DataPtr + 1u) = (*(DataPtr + 1u) & 0xF0u) | (StatePtr->Counter);
            DataID = ConfigPtr->DataIDList[StatePtr->Counter];
            CRC = Crc_CalculateCRC8H2F(&DataPtr[1], (uint32)((ConfigPtr->DataLength / 8u) - 1u), 0xFF, TRUE);
            CRC = Crc_CalculateCRC8H2F(&DataID, 1, CRC, FALSE);
            DataPtr[0] = CRC;
        }
    }

    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * Initializes the protection state.
 * Service ID: 0x1e
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
FUNC(Std_ReturnType, E2E_CODE) E2E_P02ProtectInit(P2VAR(E2E_P02ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr)
{
    uint8 Ret;
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
/*Caculate the status of received counter*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
static FUNC(void, E2E_CODE) AnalysisDeltaCounter(
    sint8 DeltaCounter,
    P2VAR(E2E_P02CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2CONST(E2E_P02ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    uint8 ReceivedCounter)
{
    if (0 == DeltaCounter)
    {
        if (StatePtr->NoNewOrRepeatedDataCounter < 15u)
        {
            StatePtr->NoNewOrRepeatedDataCounter++;
        }
        StatePtr->Status = E2E_P02STATUS_REPEATED;
    }
    else if (1 == DeltaCounter)
    {
        StatePtr->MaxDeltaCounter = ConfigPtr->MaxDeltaCounterInit;
        StatePtr->LastValidCounter = ReceivedCounter;
        StatePtr->LostData = 0;
        if (StatePtr->NoNewOrRepeatedDataCounter <= ConfigPtr->MaxNoNewOrRepeatedData)
        {
            StatePtr->NoNewOrRepeatedDataCounter = 0;
            if (StatePtr->SyncCounter > 0u)
            {
                StatePtr->SyncCounter--;
                StatePtr->Status = E2E_P02STATUS_SYNC;
            }
            else
            {
                StatePtr->Status = E2E_P02STATUS_OK;
            }
        }
        else
        {
            StatePtr->NoNewOrRepeatedDataCounter = 0;
            StatePtr->SyncCounter = ConfigPtr->SyncCounterInit;
            StatePtr->Status = E2E_P02STATUS_SYNC;
        }
    }
    else if ((1 < DeltaCounter) && ((uint8)DeltaCounter <= StatePtr->MaxDeltaCounter))
    {
        StatePtr->MaxDeltaCounter = ConfigPtr->MaxDeltaCounterInit;
        StatePtr->LastValidCounter = ReceivedCounter;
        StatePtr->LostData = ((uint8)DeltaCounter - 1u);
        if (StatePtr->NoNewOrRepeatedDataCounter <= ConfigPtr->MaxNoNewOrRepeatedData)
        {
            StatePtr->NoNewOrRepeatedDataCounter = 0;
            if (StatePtr->SyncCounter > 0u)
            {
                StatePtr->SyncCounter--;
                StatePtr->Status = E2E_P02STATUS_SYNC;
            }
            else
            {
                StatePtr->Status = E2E_P02STATUS_OKSOMELOST;
            }
        }
        else
        {
            StatePtr->NoNewOrRepeatedDataCounter = 0;
            StatePtr->SyncCounter = ConfigPtr->SyncCounterInit;
            StatePtr->Status = E2E_P02STATUS_SYNC;
        }
    }
    else
    {
        StatePtr->NoNewOrRepeatedDataCounter = 0;
        StatePtr->SyncCounter = ConfigPtr->SyncCounterInit;
        if (StatePtr->SyncCounter > 0u)
        {
            StatePtr->MaxDeltaCounter = ConfigPtr->MaxDeltaCounterInit;
            StatePtr->LastValidCounter = ReceivedCounter;
            StatePtr->Status = E2E_P02STATUS_WRONGSEQUENCE;
        }
    }
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * Checks the Data received using the E2E profile 2.
 * This includes CRC calculation, handling of Counter and Data ID.
 * Service ID: 0x04
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): ConfigPtr,Pointer to static configuration.
 *                 DataPtr,Pointer to received data.
 * Parameters(INOUT): StatePtr,Pointer to port/data communication state.
 * Parameters(OUT): NA
 * Return value: Std_ReturnType,E2E_E_INPUTERR_NULL E2E_E_INPUTERR_WRONG E2E_E_INTERR E2E_E_OK.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(Std_ReturnType, E2E_CODE)
E2E_P02Check(
    P2CONST(E2E_P02ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    P2VAR(E2E_P02CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr)
{
    Std_ReturnType Ret = E2E_P02CheckVerifyInputs(ConfigPtr, StatePtr, DataPtr);
    if (E2E_E_OK == Ret)
    {
        uint8 ReceivedCounter;
        uint8 DataID;
        uint8 CalcualtedCRC;
        sint8 DeltaCounter;

        if (StatePtr->MaxDeltaCounter < 15u)
        {
            StatePtr->MaxDeltaCounter++;
        }

        if (TRUE == StatePtr->NewDataAvailable)
        {
            ReceivedCounter = (*(DataPtr + 1u) & 0xFu);
            DataID = ConfigPtr->DataIDList[ReceivedCounter];
            CalcualtedCRC = Crc_CalculateCRC8H2F(&DataPtr[1], (uint32)((ConfigPtr->DataLength / 8u) - 1u), 0xFF, TRUE);
            CalcualtedCRC = Crc_CalculateCRC8H2F(&DataID, 1, CalcualtedCRC, FALSE);
            /*checksum ok*/
            if (CalcualtedCRC == DataPtr[0])
            {
                if (TRUE == StatePtr->WaitForFirstData)
                {
                    StatePtr->WaitForFirstData = FALSE;
                    StatePtr->MaxDeltaCounter = ConfigPtr->MaxDeltaCounterInit;
                    StatePtr->LastValidCounter = ReceivedCounter;
                    StatePtr->Status = E2E_P02STATUS_INITIAL;
                }
                else
                {
                    /*caculate delta counter*/
                    DeltaCounter = (sint8)ReceivedCounter - (sint8)StatePtr->LastValidCounter;
                    if (DeltaCounter < 0)
                    {
                        DeltaCounter = DeltaCounter + 16;
                    }
                    AnalysisDeltaCounter(DeltaCounter, StatePtr, ConfigPtr, ReceivedCounter);
                }
            }
            else
            {
                StatePtr->Status = E2E_P02STATUS_WRONGCRC;
            }
        }
        else
        {
            if (StatePtr->NoNewOrRepeatedDataCounter < 15u)
            {
                StatePtr->NoNewOrRepeatedDataCounter++;
            }
            StatePtr->Status = E2E_P02STATUS_NONEWDATA;
        }
    }

    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * Initializes the check state
 * Service ID: 0x1f
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
FUNC(Std_ReturnType, E2E_CODE) E2E_P02CheckInit(P2VAR(E2E_P02CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr)
{
    uint8 Ret;

    Ret = E2E_E_OK;
    /*@SWS_E2E_00392*/
    if (NULL_PTR == StatePtr)
    {
        Ret = E2E_E_INPUTERR_NULL;
    }
    else
    {
        StatePtr->LastValidCounter = 0;
        StatePtr->MaxDeltaCounter = 0;
        StatePtr->WaitForFirstData = TRUE;
        StatePtr->NewDataAvailable = TRUE;
        StatePtr->LostData = 0;
        StatePtr->Status = E2E_P02STATUS_NONEWDATA;
        StatePtr->NoNewOrRepeatedDataCounter = 0;
        StatePtr->SyncCounter = 0;
    }
    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * The function maps the check status of Profile 2 to a generic check status, which can be used by E2E state machine
 * check function. The E2E Profile 2 delivers a more fine-granular status, but this is not relevant for the E2E state
 * machine. Service ID: 0x20 Sync/Async: synchronous Reentrancy: Reentrant Parameters(IN): CheckReturn,Return value of
 * the E2E_P01Check function Status,Status determined by E2E_P01Check function profileBehavior,FALSE: check has the
 * legacy behavior, before R4.2 TRUE: check behaves like new P4/P5/P6 profiles introduced in R4.2 Parameters(INOUT): NA
 * Parameters(OUT): NA
 * Return value: E2E_PCheckStatusType,Profile-independent status of the reception on one single Data in one cycle.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(E2E_PCheckStatusType, E2E_CODE)
E2E_P02MapStatusToSM(Std_ReturnType CheckReturn, E2E_P02CheckStatusType Status, boolean ProfileBehavior)
{
    E2E_PCheckStatusType Ret;

    /*@SWS_E2E_00381*/
    if (CheckReturn != E2E_E_OK)
    {
        Ret = E2E_P_ERROR;
    }
    /*@SWS_E2E_00380*/
    else if ((boolean)1 == ProfileBehavior)
    {
        switch (Status)
        {
        case E2E_P02STATUS_OK:
        case E2E_P02STATUS_OKSOMELOST:
        case E2E_P02STATUS_SYNC:
            Ret = E2E_P_OK;
            break;
        case E2E_P02STATUS_WRONGCRC:
            Ret = E2E_P_ERROR;
            break;
        case E2E_P02STATUS_REPEATED:
            Ret = E2E_P_REPEATED;
            break;
        case E2E_P02STATUS_NONEWDATA:
            Ret = E2E_P_NONEWDATA;
            break;
        case E2E_P02STATUS_WRONGSEQUENCE:
        case E2E_P02STATUS_INITIAL:
            Ret = E2E_P_WRONGSEQUENCE;
            break;
        default:
            Ret = E2E_P_ERROR;
            break;
        }
    }
    /*@SWS_E2E_00477*/
    else
    {
        switch (Status)
        {
        case E2E_P02STATUS_OK:
        case E2E_P02STATUS_OKSOMELOST:
        case E2E_P02STATUS_INITIAL:
            Ret = E2E_P_OK;
            break;
        case E2E_P02STATUS_WRONGCRC:
            Ret = E2E_P_ERROR;
            break;
        case E2E_P02STATUS_REPEATED:
            Ret = E2E_P_REPEATED;
            break;
        case E2E_P02STATUS_NONEWDATA:
            Ret = E2E_P_NONEWDATA;
            break;
        case E2E_P02STATUS_WRONGSEQUENCE:
        case E2E_P02STATUS_SYNC:
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
static FUNC(Std_ReturnType, E2E_CODE) E2E_P02ProtectVerifyInputs(
    P2CONST(E2E_P02ConfigType, AUTOMATIC, E2E_APPL_CONST) ConfigPtr,
    P2CONST(E2E_P02ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr)
{
    Std_ReturnType ret = E2E_E_INPUTERR_NULL;

    /* Check if input pointer parameters are not NULL */
    if ((ConfigPtr != NULL_PTR) && (StatePtr != NULL_PTR) && (DataPtr != NULL_PTR))
    {
        /* Check if Counter is within valid range */
        if (StatePtr->Counter > E2E_P02_MAX_COUNTER_VALUE)
        {
            ret = E2E_E_INPUTERR_WRONG;
        }
        else
        {
            ret = E2E_E_OK;
        }
    }
    return ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
static FUNC(Std_ReturnType, E2E_CODE) E2E_P02CheckVerifyInputs(
    P2CONST(E2E_P02ConfigType, AUTOMATIC, E2E_APPL_CONST) ConfigPtr,
    P2CONST(E2E_P02CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr)
{
    Std_ReturnType ret = E2E_E_INPUTERR_NULL;

    /* Check if input pointer parameters are valid and if state parameter are within range */
    if ((ConfigPtr != NULL_PTR) && (StatePtr != NULL_PTR) && (DataPtr != NULL_PTR))
    {
        if ((StatePtr->LastValidCounter > E2E_P02_MAX_COUNTER_VALUE)
            || (StatePtr->MaxDeltaCounter > E2E_P02_MAX_COUNTER_VALUE)
            || (StatePtr->SyncCounter > ConfigPtr->SyncCounterInit)
            || (StatePtr->NoNewOrRepeatedDataCounter > E2E_P02_MAX_COUNTER_VALUE))
        {
            ret = E2E_E_INPUTERR_WRONG;
        }
        else
        {
            ret = E2E_E_OK;
        }
    }
    return ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
