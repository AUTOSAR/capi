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
**  FILENAME    : E2E_P05.h                                                   **
**                                                                            **
**  Created on  :                                                             **
**  Author      : YangBo                                                      **
**  Vendor      :                                                             **
**  DESCRIPTION :                                                             **
**                                                                            **
**  SPECIFICATION(S) :   AUTOSAR classic Platform R19-11                      **
**                                                                            **
*******************************************************************************/
#ifndef E2E_P05_H_
#define E2E_P05_H_

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
    /*Configuration of transmitted Data (Data Element or I-PDU), for E2E Profile 5.
     * For each transmitted Data, there is an instance of this typedef.*/
    typedef struct
    {
        /*Bit offset of the first bit of the E2E header from the beginning of the Data
        Array (bit numbering: bit 0 is the least important).*/
        uint16 Offset;
        /*Length of data, in bits.The value shall be = 4096*8 (4kB) and shall be ' 3*8*/
        uint16 DataLength;
        /*A system-unique identifier of the Data*/
        uint16 DataID;
        /*Maximum allowed gap between two counter values of two consecutively received
        valid Data.*/
        uint8 MaxDeltaCounter;
    } E2E_P05ConfigType;

    /*State of the sender for a Data protected with E2E Profile 5.*/
    typedef struct
    {
        /*Counter to be used for protecting the next Data.*/
        uint8 Counter;
    } E2E_P05ProtectStateType;

    /*Status of the reception on one single Data in one cycle, protected with E2E Profile5.*/
    typedef enum
    {
        /*OK: the checks of the Data in this cycle were successful (including counter
        check, which was incremented by 1).*/
        E2E_P05STATUS_OK = 0x00,
        /*Error: the Check function has been invoked but no new Data is not available
        since the last call, according to communication medium (e.g. RTE,COM). As a result,
        no E2E checks of Data have been consequently executed.*/
        E2E_P05STATUS_NONEWDATA = 0x01,
        /*Error: error not related to counters occurred (e.g. wrong crc, wrong length).*/
        E2E_P05STATUS_ERROR = 0x07,
        /*Error: the checks of the Data in this cycle were successful, with the exception of
        the repetition.*/
        E2E_P05STATUS_REPEATED = 0x08,
        /*OK: the checks of the Data in this cycle were successful (including counter check,
        which was incremented within the allowed configured delta).*/
        E2E_P05STATUS_OKSOMELOST = 0x20,
        /*Error: the checks of the Data in this cycle were successful, with the exception of
        counter jump, which changed more than the allowed delta*/
        E2E_P05STATUS_WRONGSEQUENCE = 0x40
    } E2E_P05CheckStatusType;

    /*State of the reception on one single Data protected with E2E Profile 5.*/
    typedef struct
    {
        /*Result of the verification of the Data in this cycle, determined by the Check function.*/
        E2E_P05CheckStatusType Status;
        /*Counter of the data in previous cycle.It is initialized with 0.*/
        uint8 Counter;
    } E2E_P05CheckStateType;

/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/
/*******************************************************************************
**                      Global Functions                                      **
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
    extern FUNC(Std_ReturnType, E2E_CODE)
        E2E_P05Protect(P2CONST(E2E_P05ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
                       P2VAR(E2E_P05ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
                       P2VAR(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr,
                       uint16 Length);
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
    extern FUNC(Std_ReturnType, E2E_CODE)
        E2E_P05ProtectInit(P2VAR(E2E_P05ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr);
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
    extern FUNC(Std_ReturnType, E2E_CODE) E2E_P05Check(P2CONST(E2E_P05ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
                                                       P2VAR(E2E_P05CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
                                                       P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr,
                                                       uint16 Length);
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
    extern FUNC(Std_ReturnType, E2E_CODE)
        E2E_P05CheckInit(P2VAR(E2E_P05CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr);
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
    extern FUNC(E2E_PCheckStatusType, E2E_CODE)
        E2E_P05MapStatusToSM(Std_ReturnType CheckReturn, E2E_P05CheckStatusType Status);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
#ifdef __cplusplus
}

#endif /* end of __cplusplus */
#endif /*E2E_P05_H_ */
