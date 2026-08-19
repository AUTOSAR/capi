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
**  FILENAME    : E2E_P01.c                                                   **
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
 * V2.0.1 [2020/9/01] [YangBo] change E2E_P01CheckInit WaitForFirstData = TRUE
 * V2.0.2 [2021/5/11] [wanglili]
 *    1.In the nibble mode in E2E_P01Check, the logic change of the comparison
 *    between ReceivedDataIDNibble and the configured dataid
 */
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "E2E.h"
#include "E2E_P01.h"

/*******************************************************************************
**                      Imported Compiler Switch Check                        **
*******************************************************************************/

/*******************************************************************************
**                       Version  Check                                       **
*******************************************************************************/

/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/
#define E2E_P01_MAX_COUNTER_VALUE 14u

#define E2E_P01_Min(a, b)         (((a) <= (b)) ? (a) : (b))
/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
static FUNC(Std_ReturnType, E2E_CODE)
    E2E_P01CheckDataIDMode(P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_CONST) ConfigPtr);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
static FUNC(Std_ReturnType, E2E_CODE) E2E_P01ProtectVerifyInputs(
    P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_CONST) ConfigPtr,
    P2CONST(E2E_P01ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
static FUNC(Std_ReturnType, E2E_CODE) E2E_P01CheckVerifyInputs(
    P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_CONST) ConfigPtr,
    P2CONST(E2E_P01CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(uint8, E2E_CODE)
E2E_P01CalculateCRC8(
    P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    uint8 Counter,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
static void E2E_P01Check_Seqence(
    P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_DATA) Config,
    P2VAR(E2E_P01CheckStateType, AUTOMATIC, E2E_APPL_DATA) State,
    uint8 ReceivedCounter);
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
 * Protects the array/buffer to be transmitted using the E2E profile 1.
 * This includes checksum calculation, handling of counter and Data ID.
 * Service ID: 0x01
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
E2E_P01Protect(
    P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    P2VAR(E2E_P01ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2VAR(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr)
{
    uint8 CRC;
    uint8 Ret = E2E_P01ProtectVerifyInputs(ConfigPtr, StatePtr, DataPtr);

    if (E2E_E_OK == Ret)
    {
        /*Write the counter in the Data, at the configured CounterOffset. The counter goes either into low
        nibble (left branch) or high nibble (right branch) of Data. Note that the nibble next to Counter
        may be used by application.*/
        if (0u == (ConfigPtr->CounterOffset % 8u))
        {
            *(DataPtr + (ConfigPtr->CounterOffset / 8u)) =
                (*(DataPtr + (ConfigPtr->CounterOffset / 8u)) & 0xF0u) | (StatePtr->Counter & 0x0Fu);
        }
        else
        {
            *(DataPtr + (ConfigPtr->CounterOffset / 8u)) =
                (*(DataPtr + (ConfigPtr->CounterOffset / 8u)) & 0x0Fu) | ((uint8)(StatePtr->Counter << 4u));
        }
        /*Write the low nibble of high byte of Data ID - only for E2E_P01_DATAID_NIBBLE configuration.*/
        if (E2E_P01_DATAID_NIBBLE == ConfigPtr->DataIDMode)
        {
            if (0u == ((ConfigPtr->DataIDNibbleOffset) % 8u))
            {
                *(DataPtr + (uint8)(ConfigPtr->DataIDNibbleOffset / 8u)) =
                    (*(DataPtr + (uint8)(ConfigPtr->DataIDNibbleOffset / 8u)) & 0xF0u)
                    | ((uint8)(ConfigPtr->DataID >> 8u) & 0x0Fu);
            }
            else
            {
                *(DataPtr + (uint8)(ConfigPtr->DataIDNibbleOffset / 8u)) =
                    (*(DataPtr + (uint8)(ConfigPtr->DataIDNibbleOffset / 8u)) & 0x0Fu)
                    | ((uint8)(ConfigPtr->DataID >> 4u) & 0xF0u);
            }
        }

        CRC = E2E_P01CalculateCRC8(ConfigPtr, StatePtr->Counter, DataPtr);
        /*CRC is written to the Data at configured location.*/
        *(DataPtr + (ConfigPtr->CRCOffset / 8u)) = CRC;
        StatePtr->Counter = (StatePtr->Counter + 1u) % 15u;
    }
    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * Initializes the protection state.
 * Service ID: 0x1b
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
E2E_P01ProtectInit(P2VAR(E2E_P01ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr)
{
    Std_ReturnType Ret;
    Ret = E2E_E_OK;

    /*@SWS_E2E_00386*/
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
 * Checks the Data received using the E2E profile 1.
 * This includes CRC calculation, handling of Counter and Data ID.
 * Service ID: 0x02
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): Config,Pointer to static configuration.
 *                 Data,Pointer to received data.
 * Parameters(INOUT): State,Pointer to port/data communication state.
 * Parameters(OUT): NA
 * Return value: Std_ReturnType,E2E_E_INPUTERR_NULL E2E_E_INPUTERR_WRONG E2E_E_INTERR E2E_E_OK.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(Std_ReturnType, E2E_CODE)
E2E_P01Check(
    P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_DATA) Config,
    P2VAR(E2E_P01CheckStateType, AUTOMATIC, E2E_APPL_DATA) State,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) Data)
{
    Std_ReturnType Ret = E2E_P01CheckVerifyInputs(Config, State, Data);

    if (E2E_E_OK == Ret)
    {
        uint8 ReceivedCounter;
        uint8 ReceivedCRC;
        uint8 ReceivedDataIDNibble;
        uint8 CalculatedCRC;
        State->MaxDeltaCounter = E2E_P01_Min(((State->MaxDeltaCounter) + 1u), (14u));

        if (TRUE == State->NewDataAvailable)
        {
            if (0u == (Config->CounterOffset % 8u))
            {
                ReceivedCounter = *(Data + (Config->CounterOffset / 8u)) & 0x0Fu;
            }
            else
            {
                ReceivedCounter = (*(Data + (Config->CounterOffset / 8u)) >> 4u);
            }
        }
        else
        {
            if (State->NoNewOrRepeatedDataCounter < 14u)
            {
                State->NoNewOrRepeatedDataCounter++;
            }
            State->Status = E2E_P01STATUS_NONEWDATA;
            return Ret;
        }
        if (ReceivedCounter >= 15u)
        {
            Ret = E2E_E_INPUTERR_WRONG;
            return Ret;
        }
        ReceivedCRC = *(Data + (Config->CRCOffset / 8u));

        if (E2E_P01_DATAID_NIBBLE == Config->DataIDMode)
        {
            if (0u == (Config->DataIDNibbleOffset % 8u))
            {
                /*Read low nibble of high byte of Data ID from Data.*/
                ReceivedDataIDNibble = *(Data + (Config->DataIDNibbleOffset / 8u)) & 0x0Fu;
            }
            else
            {
                ReceivedDataIDNibble = (*(Data + (Config->DataIDNibbleOffset / 8u)) >> 4u);
            }
        }
        CalculatedCRC = E2E_P01CalculateCRC8(Config, ReceivedCounter, Data);

        if (ReceivedCRC == CalculatedCRC)
        {
            if ((E2E_P01_DATAID_NIBBLE == Config->DataIDMode)
                && (ReceivedDataIDNibble != ((Config->DataID >> 8u) & 0x0Fu)))
            {
                State->Status = E2E_P01STATUS_WRONGCRC;
            }
            else
            {
                /*Check if any correct data has already be received.*/
                if (TRUE == State->WaitForFirstData)
                {
                    State->WaitForFirstData = FALSE;
                    State->MaxDeltaCounter = Config->MaxDeltaCounterInit;
                    State->LastValidCounter = ReceivedCounter;
                    State->Status = E2E_P01STATUS_INITIAL;
                }
                else
                {
                    E2E_P01Check_Seqence(Config, State, ReceivedCounter);
                }
            }
        }
        else
        {
            State->Status = E2E_P01STATUS_WRONGCRC;
        }
    }
    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * Initializes the check state
 * Service ID: 0x1c
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
FUNC(Std_ReturnType, E2E_CODE)
E2E_P01CheckInit(P2VAR(E2E_P01CheckStateType, AUTOMATIC, E2E_APPL_DATA) State)
{
    Std_ReturnType Ret;

    Ret = E2E_E_OK;
    /*@SWS_E2E_00389*/
    if (NULL_PTR == State)
    {
        Ret = E2E_E_INPUTERR_NULL;
    }
    else
    {
        State->LastValidCounter = 0u;
        State->MaxDeltaCounter = 0u;
        State->WaitForFirstData = TRUE;
        State->NewDataAvailable = TRUE;
        State->LostData = 0u;
        State->Status = E2E_P01STATUS_NONEWDATA;
        State->NoNewOrRepeatedDataCounter = 0u;
        State->SyncCounter = 0u;
    }
    return Ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 * The function maps the check status of Profile 1 to a generic check status, which can be used by E2E state machine
 * check function. The E2E Profile 1 delivers a more fine-granular status, but this is not relevant for the E2E state
 * machine. Service ID: 0x1d Sync/Async: synchronous Reentrancy: Reentrant Parameters(IN): CheckReturn,Return value of
 * the E2E_P01Check function Status,Status determined by E2E_P01Check function profileBehavior,FALSE: check has the
 * legacy behavior, before R4.2 TRUE: check behaves like new P4/P5/P6 profiles introduced in R4.2 Parameters(INOUT): NA
 * Parameters(OUT): NA
 * Return value: E2E_PCheckStatusType,Profile-independent status of the reception on one single Data in one cycle.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
FUNC(E2E_PCheckStatusType, E2E_CODE)
E2E_P01MapStatusToSM(Std_ReturnType CheckReturn, E2E_P01CheckStatusType Status, boolean ProfileBehavior)
{
    E2E_PCheckStatusType Ret;

    /*@SWS_E2E_00384*/
    if (E2E_E_OK != CheckReturn)
    {
        Ret = E2E_P_ERROR;
    }
    /*@SWS_E2E_00383*/
    else if ((boolean)1 == ProfileBehavior)
    {
        switch (Status)
        {
        case E2E_P01STATUS_OK:
        case E2E_P01STATUS_OKSOMELOST:
        case E2E_P01STATUS_SYNC:
            Ret = E2E_P_OK;
            break;
        case E2E_P01STATUS_WRONGCRC:
            Ret = E2E_P_ERROR;
            break;
        case E2E_P01STATUS_REPEATED:
            Ret = E2E_P_REPEATED;
            break;
        case E2E_P01STATUS_NONEWDATA:
            Ret = E2E_P_NONEWDATA;
            break;
        case E2E_P01STATUS_WRONGSEQUENCE:
        case E2E_P01STATUS_INITIAL:
            Ret = E2E_P_WRONGSEQUENCE;
            break;
        default:
            Ret = E2E_P_ERROR;
            break;
        }
    }
    /*@SWS_E2E_00476*/
    else
    {
        switch (Status)
        {
        case E2E_P01STATUS_OK:
        case E2E_P01STATUS_OKSOMELOST:
        case E2E_P01STATUS_INITIAL:
            Ret = E2E_P_OK;
            break;
        case E2E_P01STATUS_WRONGCRC:
            Ret = E2E_P_ERROR;
            break;
        case E2E_P01STATUS_REPEATED:
            Ret = E2E_P_REPEATED;
            break;
        case E2E_P01STATUS_NONEWDATA:
            Ret = E2E_P_NONEWDATA;
            break;
        case E2E_P01STATUS_WRONGSEQUENCE:
        case E2E_P01STATUS_SYNC:
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
static FUNC(Std_ReturnType, E2E_CODE)
    E2E_P01CheckDataIDMode(P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_CONST) ConfigPtr)
{
    Std_ReturnType ret;

    if (E2E_P01_DATAID_ALT == ConfigPtr->DataIDMode)
    {
        ret = E_OK;
    }
    else if (E2E_P01_DATAID_BOTH == ConfigPtr->DataIDMode)
    {
        ret = E_OK;
    }
    else if (E2E_P01_DATAID_LOW == ConfigPtr->DataIDMode)
    {
        ret = E_OK;
    }
    else if (E2E_P01_DATAID_NIBBLE == ConfigPtr->DataIDMode)
    {
        ret = E_OK;
    }
    else
    {
        ret = E_NOT_OK;
    }

    return ret;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
static FUNC(Std_ReturnType, E2E_CODE) E2E_P01ProtectVerifyInputs(
    P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_CONST) ConfigPtr,
    P2CONST(E2E_P01ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr)
{
    Std_ReturnType ret = E2E_E_INPUTERR_NULL;

    /* Check input pointer parameter for NULL_PTR */
    if ((ConfigPtr != NULL_PTR) && (StatePtr != NULL_PTR) && (DataPtr != NULL_PTR))
    {
        /* Check if value of Counter is within valid range */
        if (StatePtr->Counter > E2E_P01_MAX_COUNTER_VALUE)
        {
            ret = E2E_E_INPUTERR_WRONG;
        }
        else if (E_NOT_OK == E2E_P01CheckDataIDMode(ConfigPtr))
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
static FUNC(Std_ReturnType, E2E_CODE) E2E_P01CheckVerifyInputs(
    P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_CONST) ConfigPtr,
    P2CONST(E2E_P01CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr)
{
    Std_ReturnType ret = E2E_E_INPUTERR_NULL;

    /* Check input parameter for NULL_PTR */
    if ((ConfigPtr != NULL_PTR) && (StatePtr != NULL_PTR) && (DataPtr != NULL_PTR))
    {
        /* Check if counter values are within valid range */
        if ((StatePtr->LastValidCounter > E2E_P01_MAX_COUNTER_VALUE)
            || (StatePtr->MaxDeltaCounter > E2E_P01_MAX_COUNTER_VALUE)
            || (StatePtr->SyncCounter > ConfigPtr->SyncCounterInit)
            || (StatePtr->NoNewOrRepeatedDataCounter > E2E_P01_MAX_COUNTER_VALUE))
        {
            ret = E2E_E_INPUTERR_WRONG;
        }
        else if (E_NOT_OK == E2E_P01CheckDataIDMode(ConfigPtr))
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
FUNC(uint8, E2E_CODE)
E2E_P01CalculateCRC8(
    P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
    uint8 Counter,
    P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr)
{
    uint8 CRC = 0;
    uint8 DataID_Hbyte;
    uint8 DataID_Lbyte;
    uint8 Data_0 = 0;

    DataID_Hbyte = (uint8)((ConfigPtr->DataID) >> 8u);
    DataID_Lbyte = (uint8)(ConfigPtr->DataID);
    /*Compute CRC over DataID, depending on DataIDMode setting.The first invocation of Crc_CalculateCRC8() is done with
      start value 0xFF.The CalculateCRC8() is XORing the start value provided by the caller (equal 0xFF) with 0xFF,
      resulting with actual internal start value equal to 0x00.*/
    switch (ConfigPtr->DataIDMode)
    {
    case E2E_P01_DATAID_BOTH:
        CRC = Crc_CalculateCRC8(&DataID_Lbyte, 1, 0xFF, FALSE);
        CRC = Crc_CalculateCRC8(&DataID_Hbyte, 1, CRC, FALSE);
        break;
    case E2E_P01_DATAID_LOW:
        CRC = Crc_CalculateCRC8(&DataID_Lbyte, 1, 0xFF, FALSE);
        break;
    case E2E_P01_DATAID_ALT:
        if (0u == (Counter % 2u))
        {
            CRC = Crc_CalculateCRC8(&DataID_Lbyte, 1, 0xFF, FALSE);
        }
        else
        {
            CRC = Crc_CalculateCRC8(&DataID_Hbyte, 1, 0xFF, FALSE);
        }
        break;
    case E2E_P01_DATAID_NIBBLE:
        CRC = Crc_CalculateCRC8(&DataID_Lbyte, 1, 0xFF, FALSE);
        CRC = Crc_CalculateCRC8(&Data_0, 1, CRC, FALSE);
        break;
    default:
        /*nothing to do*/
        break;
    }
    /*Compute CRC over the area before the CRC (if CRC is not the first byte)*/
    if ((ConfigPtr->CRCOffset >= 8u) && (E2E_P01_DATAID_NOUSED != ConfigPtr->DataIDMode))
    {
        CRC = Crc_CalculateCRC8(DataPtr, (ConfigPtr->CRCOffset / 8u), CRC, FALSE);
    }
    else if ((0u == ConfigPtr->CRCOffset) && (E2E_P01_DATAID_NOUSED != ConfigPtr->DataIDMode))
    {
        /*nothing to do*/
    }
    else
    {
        CRC = Crc_CalculateCRC8(DataPtr, (ConfigPtr->CRCOffset / 8u), 0xFF, FALSE);
    }
    /*Compute the area after CRC, if CRC is not the last byte. Start with the byte after CRC,
      finish with the last byte of Data.*/
    if ((ConfigPtr->CRCOffset / 8u) < ((ConfigPtr->DataLength / 8u) - 1u))
    {
        CRC = Crc_CalculateCRC8(
            &DataPtr[((ConfigPtr->CRCOffset / 8u) + 1u)],
            (((ConfigPtr->DataLength) / 8u) - (ConfigPtr->CRCOffset / 8u) - 1u),
            CRC,
            FALSE);
    }
    CRC = CRC ^ 0xFFu;

    return CRC;
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
static void E2E_P01Check_Seqence(
    P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_DATA) Config,
    P2VAR(E2E_P01CheckStateType, AUTOMATIC, E2E_APPL_DATA) State,
    uint8 ReceivedCounter)
{
    uint8 DeltaCounter;

    /*Compute the delta, taking into account the overflow.*/
    if (ReceivedCounter >= State->LastValidCounter)
    {
        DeltaCounter = ReceivedCounter - State->LastValidCounter;
    }
    else
    {
        DeltaCounter = 15u + ReceivedCounter - State->LastValidCounter;
    }
    /*The previous and the current data have correct CRC, verify the counter.*/
    if (0u == DeltaCounter)
    {
        if (State->NoNewOrRepeatedDataCounter < 14u)
        {
            State->NoNewOrRepeatedDataCounter++;
        }
        State->Status = E2E_P01STATUS_REPEATED;
    }
    else if (1u == DeltaCounter)
    {
        State->MaxDeltaCounter = Config->MaxDeltaCounterInit;
        State->LastValidCounter = ReceivedCounter;
        State->LostData = 0u;
        if (State->NoNewOrRepeatedDataCounter <= Config->MaxNoNewOrRepeatedData)
        {
            State->NoNewOrRepeatedDataCounter = 0u;
            if (State->SyncCounter > 0u)
            {
                State->SyncCounter--;
                State->Status = E2E_P01STATUS_SYNC;
            }
            else
            {
                State->Status = E2E_P01STATUS_OK;
            }
        }
        else
        {
            State->NoNewOrRepeatedDataCounter = 0u;
            State->SyncCounter = Config->SyncCounterInit;
            State->Status = E2E_P01STATUS_SYNC;
        }
    }
    else if ((DeltaCounter <= State->MaxDeltaCounter))
    {
        State->MaxDeltaCounter = Config->MaxDeltaCounterInit;
        State->LastValidCounter = ReceivedCounter;
        State->LostData = (DeltaCounter - 1u);
        if (State->NoNewOrRepeatedDataCounter <= Config->MaxNoNewOrRepeatedData)
        {
            State->NoNewOrRepeatedDataCounter = 0u;
            if (State->SyncCounter > 0u)
            {
                State->SyncCounter--;
                State->Status = E2E_P01STATUS_SYNC;
            }
            else
            {
                State->Status = E2E_P01STATUS_OKSOMELOST;
            }
        }
        else
        {
            State->NoNewOrRepeatedDataCounter = 0u;
            State->SyncCounter = Config->SyncCounterInit;
            State->Status = E2E_P01STATUS_SYNC;
        }
    }
    else
    {
        State->NoNewOrRepeatedDataCounter = 0u;
        State->SyncCounter = Config->SyncCounterInit;
        if (State->SyncCounter > 0u)
        {
            State->MaxDeltaCounter = Config->MaxDeltaCounterInit;
            State->LastValidCounter = ReceivedCounter;
        }
        State->Status = E2E_P01STATUS_WRONGSEQUENCE;
    }
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
