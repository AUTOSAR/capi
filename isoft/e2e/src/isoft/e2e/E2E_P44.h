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
**  FILENAME    : E2E_P44.h                                                   **
**                                                                            **
**  Created on  :                                                             **
**  Author      : xuhua                                                       **
**  Vendor      :                                                             **
**  DESCRIPTION :                                                             **
**                                                                            **
**  SPECIFICATION(S) :   AUTOSAR classic Platform R20-11                      **
**                                                                            **
*******************************************************************************/
#ifndef E2E_P44_H_
#define E2E_P44_H_

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
        /*A system-unique identifier of the Data.*/
        uint32 DataID;
        /*Bit offset of the first bit of the E2E header from the beginning of the Data
        (bit numbering: bit 0 is the least important). The offset shall be a multiple
        of 8 and 0 <= Offset <= MaxDataLength-(12*8). Example: If Offset equals
        8, then the high byte of the E2E Length (16 bit) is written to Byte 1, the
        low Byte is written to Byte 2.*/
        uint32 Offset;
        /*Minimal length of Data, in bits. E2E checks that Length is >= MinData
        Length*/
        uint32 MinDataLength;
        /*Maximal length of Data, in bits. E2E checks that DataLength is <= Max
        DataLength.*/
        uint32 MaxDataLength;
        /*Maximum allowed gap between two counter values of two consecutively
        received valid Data. For example, if the receiver gets Data with counter 1
        and MaxDeltaCounter is 3, then at the next reception the receiver can
        accept Counters with values 2, 3 or 4*/
        uint16 MaxDeltaCounter;
    } E2E_P44ConfigType;

    typedef struct
    {
        /*Counter to be used for protecting the next Data.*/
        uint16 Counter;
    } E2E_P44ProtectStateType;

    typedef enum
    {
        /*OK: the checks of the Data in this cycle were successful (including counter
        check, which was incremented by 1).*/
        E2E_P44STATUS_OK = 0x00,
        /*Error: the Check function has been invoked but no new Data is not available
        since the last call, according to communication medium (e.g. RTE,COM). As a result,
        no E2E checks of Data have been consequently executed.*/
        E2E_P44STATUS_NONEWDATA = 0x01,
        /*Error: error not related to counters occurred (e.g. wrong crc, wrong length,
        wrong options, wrong Data ID).*/
        E2E_P44STATUS_ERROR = 0x07,
        /*Error: the checks of the Data in this cycle were successful, with the exception of
        the repetition.*/
        E2E_P44STATUS_REPEATED = 0x08,
        /*OK: the checks of the Data in this cycle were successful (including counter check,
        which was incremented within the allowed configured delta).*/
        E2E_P44STATUS_OKSOMELOST = 0x20,
        /*Error: the checks of the Data in this cycle were successful, with the exception of
        counter jump, which changed more than the allowed delta*/
        E2E_P44STATUS_WRONGSEQUENCE = 0x40
    } E2E_P44CheckStatusType;

    typedef struct
    {
        /*Result of the verification of the Data in this cycle, determined by the
        Check function.*/
        E2E_P44CheckStatusType Status;
        /*Counter of the data in previous cycle.It is initialized with 0.*/
        uint16 Counter;
    } E2E_P44CheckStateType;

/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/
/*******************************************************************************
**                      Global Functions                                      **
*******************************************************************************/
/**
 * Protects the array/buffer to be transmitted using the E2E profile 44.
 * This includes checksum calculation, handling of counter and Data ID.
 * Service ID: 0x50
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
        E2E_P44Protect(P2CONST(E2E_P44ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
                       P2VAR(E2E_P44ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
                       P2VAR(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr,
                       uint16 Length);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 * Initializes the protection state.
 * Service ID: 0x51
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
        E2E_P44ProtectInit(P2VAR(E2E_P44ProtectStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 * Checks the Data received using the E2E profile 44.
 * This includes CRC calculation, handling of Counter and Data ID.
 * Service ID: 0x53
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
    extern FUNC(Std_ReturnType, E2E_CODE) E2E_P44Check(P2CONST(E2E_P44ConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
                                                       P2VAR(E2E_P44CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
                                                       P2CONST(uint8, AUTOMATIC, E2E_APPL_DATA) DataPtr,
                                                       uint16 Length);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 * Initializes the check state
 * Service ID: 0x55
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
        E2E_P44CheckInit(P2VAR(E2E_P44CheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 * The function maps the check status of Profile 44 to a generic check status, which
 * can be used by E2E state machine check function. The E2E Profile 44 delivers a
 * more fine-granular status, but this is not relevant for the E2E state machine.
 * Service ID: 0x56 
 * Sync/Async: synchronous 
 * Reentrancy: Reentrant
 * Parameters(IN): CheckReturn,Return value of
 * the E2E_P44Check function Status,Status determined by E2E_P44Check function Parameters(INOUT): NA 
 * Parameters(OUT): NA
 * Return value: E2E_PCheckStatusType,Profile-independent status of the reception on one single Data in one cycle.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
    extern FUNC(E2E_PCheckStatusType, E2E_CODE)
        E2E_P44MapStatusToSM(Std_ReturnType CheckReturn, E2E_P44CheckStatusType Status);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

#ifdef __cplusplus
}

#endif /* end of __cplusplus */
#endif /*E2E_P44_H_ */
