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
**  FILENAME    : E2E_P01.h                                                   **
**                                                                            **
**  Created on  :                                                             **
**  Author      : YangBo                                                      **
**  Vendor      :                                                             **
**  DESCRIPTION :                                                             **
**                                                                            **
**  SPECIFICATION(S) :   AUTOSAR classic Platform R19-11                      **
**                                                                            **
*******************************************************************************/
#ifndef E2E_P01_H_
#define E2E_P01_H_

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
    /*E2E P01 DataID mode*/
    typedef enum
    {
        /*Two bytes are included in the CRC (double ID configuration) This is used in
        E2E variant 1A.*/
        E2E_P01_DATAID_BOTH,
        /*One of the two bytes byte is included, alternating high and low byte,
        depending on parity of the counter (alternating ID configuration).*/
        E2E_P01_DATAID_ALT,
        /*Only the low byte is included, the high byte is never used.*/
        E2E_P01_DATAID_LOW,
        /*The low byte is included in the implicit CRC calculation, the low nibble of the
        high byte is transmitted along with the data (i.e. it is explicitly included), the
        high nibble of the high byte is not used. This is applicable for the IDs up to 12 bits.
        This is used in E2E variant 1C.*/
        E2E_P01_DATAID_NIBBLE,
        /*used for functional safety without DataId*/
        E2E_P01_DATAID_NOUSED
    } E2E_P01DataIDMode;

    /*Configuration of transmitted Data (Data Element or I-PDU),for E2E Profile 1.
    For each transmitted Data, there is an instance of this typedef.*/
    typedef struct
    {
        /*Bit offset of Counter in MSB first order. In variants 1A and 1B,
        CounterOffset is 8. The offset shall be a multiple of 4.*/
        uint16 CounterOffset;
        /*Bit offset of CRC (i.e. since *Data) in MSB first order. In variants 1A and
        1B, CRCOffset is 0. The offset shall be a multiple of 8.*/
        uint16 CRCOffset;
        /*A unique identifier, for protection against masquerading.*/
        uint16 DataID;
        /*Bit offset of the low nibble of the high byte of Data ID. This parameter is
        used by E2E Library only if DataIDMode = E2E_P01_DATAID_NIBBLE (otherwise it is
        ignored by E2ELibrary).For DataIDMode different than E2E_P01_DATAID_NIBBLE,
        DataIDNibbleOffset shall be initialized to 0 (even if it is ignored by E2E Library).*/
        uint16 DataIDNibbleOffset;
        /*Inclusion mode of ID in CRC computation (both bytes,alternating, or low byte only of
        ID included).*/
        E2E_P01DataIDMode DataIDMode;
        /*Length of data, in bits.The value shall be a multiple of 8 and shall be ' 240.*/
        uint16 DataLength;
        /*Initial maximum allowed gap between two counter values of two consecutively received
        valid Data.*/
        uint8 MaxDeltaCounterInit;
        /*The maximum amount of missing or repeated Data which the receiver does not expect to
        exceed under normal communication conditions.*/
        uint8 MaxNoNewOrRepeatedData;
        /*Number of Data required for validating the consistency of the counter that must be received
        with a valid counter (i.e. counter within the allowed lock-in range) after the detection of an
        unexpected behavior of a received counter.*/
        uint8 SyncCounterInit;
    } E2E_P01ConfigType;

    typedef struct
    {
        /*Counter to be used for protecting the next Data.*/
        uint8 Counter;
    } E2E_P01ProtectStateType;

    typedef enum
    {
        /*OK: The new data has been received according to communication medium,the CRC is
        correct, the Counter is incremented by 1 with respect to the most recent Data
        received with Status _INITIAL, _OK, or _OKSOMELOST. This means that no Data has been
        lost since the last correct data reception.*/
        E2E_P01STATUS_OK = 0x00,
        /*Error: the Check function has been invoked but no new Data is not available since
        the last call, according to communication medium (e.g. RTE, COM). As a result, no E2E
        checks of Data have been consequently executed.*/
        E2E_P01STATUS_NONEWDATA = 0x01,
        /*Error: The data has been received according to communication medium, but 1. the CRC
        is incorrect (applicable for all E2E Profile 1 configurations) or 2. the low nibble of
        the high byte of Data ID is incorrect (applicable only for E2E Profile 1 with
        E2E_P01DataIDMode = E2E_P01_DATAID_NIBBLE).*/
        E2E_P01STATUS_WRONGCRC = 0x02,
        /*NOT VALID: The new data has been received after detection of an unexpected behavior of
        counter. The data has a correct CRC and a counter within the expected range with respect
        to the most recent Data received, but the determined continuity check for the counter is
        not finalized yet.*/
        E2E_P01STATUS_SYNC = 0x03,
        /*Initial: The new data has been received according to communication medium, the CRC is
        correct, but this is the first Data since the receiver's initialization or reinitialization,
        so the Counter cannot be verified yet.*/
        E2E_P01STATUS_INITIAL = 0x04,
        /*Error: The new data has been received according to communication medium, the CRC is correct,
        but the Counter is identical to the most recent Data received with Status _INITIAL, _OK, or
        _OKSOMELOST.*/
        E2E_P01STATUS_REPEATED = 0x08,
        /*OK: The new data has been received according to communication medium, the CRC is correct,
        the Counter is incremented by DeltaCounter (1 < DeltaCounter = MaxDeltaCounter) with respect
        to the most recent Data received with Status _INITIAL, _OK, or _OKSOMELOST.*/
        E2E_P01STATUS_OKSOMELOST = 0x20,
        /*Error: The new data has been received according to communication medium, the CRC is correct
        , but the Counter Delta is too big (DeltaCounter > MaxDeltaCounter) with respect to the most
        recent Data received with Status _INITIAL, _OK, or _OKSOMELOST.*/
        E2E_P01STATUS_WRONGSEQUENCE = 0x40
    } E2E_P01CheckStatusType;

    typedef struct
    {
        /*Counter value most recently received.*/
        uint8 LastValidCounter;
        /*MaxDeltaCounter specifies the maximum allowed difference between two
        counter values of consecutively received valid messages.*/
        uint8 MaxDeltaCounter;
        /*If true means that no correct data (with correct Data ID and CRC) has been yet received after
        the receiver initialization or reinitialization.*/
        boolean WaitForFirstData;
        /*Indicates to E2E Library that a new data is available for Library to be checked. This attribute
        is set by the E2E Library caller, and not by the E2E Library.*/
        boolean NewDataAvailable;
        /*Number of data (messages) lost since reception of last valid one. This attribute is set only if
        Status equals E2E_P01STATUS_OK or E2E_P01STATUS_OKSOMELOST.*/
        uint8 LostData;
        /*Result of the verification of the Data, determined by the Check function.*/
        E2E_P01CheckStatusType Status;
        /*Number of Data required for validating the consistency of the counter that must be received with
        a valid counter (i.e. counter within the allowed lock-in range) after the detection of an unexpected
        behavior of a received counter.*/
        uint8 SyncCounter;
        /*Amount of consecutive reception cycles in which either (1) there was no new data, or (2) when the
        data was repeated.*/
        uint8 NoNewOrRepeatedDataCounter;
    } E2E_P01CheckStateType;
/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/
/*******************************************************************************
**                      Global Functions                                      **
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
    extern FUNC(Std_ReturnType, E2E_CODE)
        E2E_P01Protect(P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
                       P2VAR(E2E_P01ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
                       P2VAR(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr);
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
    extern FUNC(Std_ReturnType, E2E_CODE)
        E2E_P01ProtectInit(P2VAR(E2E_P01ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr);
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
    extern FUNC(Std_ReturnType, E2E_CODE) E2E_P01Check(P2CONST(E2E_P01ConfigType, AUTOMATIC, E2E_APPL_DATA) Config,
                                                       P2VAR(E2E_P01CheckStateType, AUTOMATIC, E2E_APPL_DATA) State,
                                                       P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) Data);
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
    extern FUNC(Std_ReturnType, E2E_CODE)
        E2E_P01CheckInit(P2VAR(E2E_P01CheckStateType, AUTOMATIC, E2E_APPL_DATA) State);
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
    extern FUNC(E2E_PCheckStatusType, E2E_CODE)
        E2E_P01MapStatusToSM(Std_ReturnType CheckReturn, E2E_P01CheckStatusType Status, boolean ProfileBehavior);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

#ifdef __cplusplus
}

#endif /* end of __cplusplus */
#endif
