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
**  FILENAME    : E2E_P02.h                                                   **
**                                                                            **
**  Created on  :                                                             **
**  Author      : YangBo                                                      **
**  Vendor      :                                                             **
**  DESCRIPTION :                                                             **
**                                                                            **
**  SPECIFICATION(S) :   AUTOSAR classic Platform R19-11                      **
**                                                                            **
*******************************************************************************/
#ifndef E2E_P02_H_
#define E2E_P02_H_

#ifdef __cplusplus

extern "C"
{
#endif
/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "E2E.h"
    /*******************************************************************************
    **                      Global Symbols                                        **
    *******************************************************************************/
    /*******************************************************************************
    **                      Global Data Types                                     **
    *******************************************************************************/
    typedef struct
    {
        /*Length of Data, in bits. The value shall be a multiple of 8.*/
        uint16 DataLength;
        /*An array of appropriately chosen Data IDs for protection against masquerading.*/
        P2VAR(uint8, AUTOMATIC, E2E_APPL_DATA) DataIDList;
        /*Initial maximum allowed gap between two counter values of two consecutively received valid Data.*/
        uint8 MaxDeltaCounterInit;
        /*The maximum amount of missing or repeated Data which the receiver does not expect to exceed under
         *  normal communication conditions.*/
        uint8 MaxNoNewOrRepeatedData;
        /*Number of Data required for validating the consistency of the counter that must be received with a
         *  valid counter*/
        uint8 SyncCounterInit;
        /*Offset of the E2E header in the Data[] array in bits.*/
        uint16 Offset;
    } E2E_P02ConfigType;

    typedef struct
    {
        /*Counter to be used for protecting the Data.*/
        uint8 Counter;
    } E2E_P02ProtectStateType;

    typedef enum
    {
        /*OK: The new data has been received according to communication medium, the CRC is
        correct, the Counter is incremented by 1 with respect to the most recent Data received
        with Status _INITIAL, _OK, or _OKSOMELOST. This means that no Data has been lost since
        the last correct data reception.*/
        E2E_P02STATUS_OK = 0x00,
        /*Error: the Check function has been invoked but no new Data is not available since the
        last call, according to communication medium (e.g. RTE, COM). As a result, no E2E checks
        of Data have been consequently executed.*/
        E2E_P02STATUS_NONEWDATA = 0x01,
        /*Error: The data has been received according to communication medium, but the CRC is incorrect.*/
        E2E_P02STATUS_WRONGCRC = 0x02,
        /*NOT VALID: The new data has been received after detection of an unexpected behavior of counter.*/
        E2E_P02STATUS_SYNC = 0x03,
        /*Initial: The new data has been received according to communication medium, the CRC is correct,
        but this is the first Data since the receiver's initialization or reinitialization, so the
        Counter cannot be verified yet.*/
        E2E_P02STATUS_INITIAL = 0x04,
        /*Error: The new data has been received according to communication medium, the CRC is correct,
        but the Counter is identical to the most recent Data received with Status _INITIAL, _OK, or
        _OKSOMELOST.*/
        E2E_P02STATUS_REPEATED = 0x08,
        /*OK: The new data has been received according to communication medium, the CRC is correct, the
        Counter is incremented by DeltaCounter (1 < DeltaCounter=MaxDeltaCounter) with respect to the most
        recent Data received with Status _INITIAL,_OK, or _OKSOMELOST.*/
        E2E_P02STATUS_OKSOMELOST = 0x20,
        /*Error: The new data has been received according to communication medium, the CRC is correct, but
        the Counter Delta is too big (DeltaCounter > MaxDeltaCounter) with respect to the most recent Data
        received with Status _INITIAL, _OK, or _OKSOMELOST.*/
        E2E_P02STATUS_WRONGSEQUENCE = 0x40
    } E2E_P02CheckStatusType;

    typedef struct
    {
        /*Counter of last valid received message.*/
        uint8 LastValidCounter;
        uint8 MaxDeltaCounter;
        /*If true means that no correct data has been yet received after the receiver
         *  initialization or reinitialization.*/
        boolean WaitForFirstData;
        boolean NewDataAvailable;
        /*Number of data (messages) lost since reception of last valid one.*/
        uint8 LostData;
        /*Result of the verification of the Data, determined by the Check function.*/
        E2E_P02CheckStatusType Status;
        /*Number of Data required for validating the consistency of the counter that must
        be received with a valid counter (i.e. counter within the allowed lock-in range)
        after the detection of an unexpected behavior of a received counter.*/
        uint8 SyncCounter;
        /*Amount of consecutive reception cycles in which either (1) there was no new data,
        or (2) when the data was repeated.*/
        uint8 NoNewOrRepeatedDataCounter;
    } E2E_P02CheckStateType;
/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/
/*******************************************************************************
**                      Global Functions                                      **
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
    extern FUNC(Std_ReturnType, E2E_CODE)
        E2E_P02Protect(P2CONST(E2E_P02ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
                       P2VAR(E2E_P02ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
                       P2VAR(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr);
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
    extern FUNC(Std_ReturnType, E2E_CODE)
        E2E_P02ProtectInit(P2VAR(E2E_P02ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr);
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
    extern FUNC(Std_ReturnType, E2E_CODE) E2E_P02Check(P2CONST(E2E_P02ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
                                                       P2VAR(E2E_P02CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
                                                       P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr);
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
    extern FUNC(Std_ReturnType, E2E_CODE)
        E2E_P02CheckInit(P2VAR(E2E_P02CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr);
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
    extern FUNC(E2E_PCheckStatusType, E2E_CODE)
        E2E_P02MapStatusToSM(Std_ReturnType CheckReturn, E2E_P02CheckStatusType Status, boolean ProfileBehavior);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

#ifdef __cplusplus
}

#endif /* end of __cplusplus */

#endif /* E2E_P02_H_ */
