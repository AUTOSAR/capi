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
**  FILENAME    : E2E.h                                                       **
**                                                                            **
**  Created on  :                                                             **
**  Author      : YangBo                                                      **
**  Vendor      :                                                             **
**  DESCRIPTION :                                                             **
**                                                                            **
**  SPECIFICATION(S) :   AUTOSAR classic Platform R19-11                      **
**                                                                            **
*******************************************************************************/
#ifndef E2E_H_
#define E2E_H_
#ifdef __cplusplus

extern "C"
{
#endif
/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/* <VERSION> <DATE> <AUTHOR> <REVISION LOG>
 * V2.0.0 [2020/8/17] [YangBo] Initial Vertion.
 * V2.0.1 [2021/8/24] [Wanglili] Functional safety profile01 compatible when dataId
 * is not used.
 * V2.0.2 [2023/2/08] [Jian.Jiang] Code walkthrough, in the conditional statement (==),
 * constants must be placed on the left.
 */
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Crc.h"
#include "Std_Types.h"
/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
/*=======[V E R S I O N  I N F O R M A T I O N]===============================*/
/* @req<SWS_E2E_00327> */
#define E2E_VENDOR_ID                   (62u)
#define E2E_MODULE_ID                   (207u)
#define E2E_AR_RELEASE_MAJOR_VERSION    (4u)
#define E2E_AR_RELEASE_MINOR_VERSION    (5u)
#define E2E_AR_RELEASE_REVISION_VERSION (0u)
#define E2E_SW_MAJOR_VERSION            (2u)
#define E2E_SW_MINOR_VERSION            (0u)
#define E2E_SW_PATCH_VERSION            (2u)

/**
 * Returns the version information of this module.
 * Service ID: 0x14
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): NA
 * Parameters(INOUT): NA
 * Parameters(OUT): VersionInfo,Pointer to where to store the version information of this module.
 * Return value: NA
 *
 */
#define E2E_GetVersionInfo(VersionInfo)                                                                                \
    do {                                                                                                               \
        (VersionInfo)->vendorID         = E2E_VENDOR_ID;                                                               \
        (VersionInfo)->moduleID         = E2E_MODULE_ID;                                                               \
        (VersionInfo)->instanceID       = 0u;                                                                          \
        (VersionInfo)->sw_major_version = E2E_SW_MAJOR_VERSION;                                                        \
        (VersionInfo)->sw_minor_version = E2E_SW_MINOR_VERSION;                                                        \
        (VersionInfo)->sw_patch_version = E2E_SW_PATCH_VERSION;                                                        \
    } while (0)

    typedef uint8 E2E_PCheckStatusType;
/*OK: the checks of the Data in this cycle were successful (including counter check).*/
#define E2E_P_OK (E2E_PCheckStatusType)0x00u
/*Data has a repeated counter.*/
#define E2E_P_REPEATED (E2E_PCheckStatusType)0x01u
/*The checks of the Data in this cycle were successful, with the exception of counter
jump, which changed more than the allowed delta.*/
#define E2E_P_WRONGSEQUENCE (E2E_PCheckStatusType)0x02u
/*Error not related to counters occurred (e.g. wrong crc, wrong length, wrong Data ID)
or the return of the check function was not OK.*/
#define E2E_P_ERROR (E2E_PCheckStatusType)0x03u
/*No value has been received yet (e.g. during initialization). This is used as the initialization
value for the buffer, it is not returned by any E2E profile.*/
#define E2E_P_NOTAVAILABLE (E2E_PCheckStatusType)0x04u
/*No new data is available.*/
#define E2E_P_NONEWDATA (E2E_PCheckStatusType)0x05u

    typedef uint8 E2E_SMStateType;
/*Communication functioning properly according to E2E, data can be used.*/
#define E2E_SM_VALID (E2E_SMStateType)0x00u
/*State before E2E_SMCheckInit() is invoked, data cannot be used.*/
#define E2E_SM_DEINIT (E2E_SMStateType)0x01u
/*No data from the sender is available since the initialization, data cannot be used.*/
#define E2E_SM_NODATA (E2E_SMStateType)0x02u
/*There has been some data received since startup, but it is not yet possible use it,
data cannot be used.*/
#define E2E_SM_INIT (E2E_SMStateType)0x03u
/*Communication not functioning properly, data cannot be used.*/
#define E2E_SM_INVALID (E2E_SMStateType)0x04u
/*At least one pointer parameter is a NULL pointer*/
#define E2E_E_INPUTERR_NULL (E2E_SMStateType)0x13u
/*At least one input parameter is erroneous, e.g. out of range*/
#define E2E_E_INPUTERR_WRONG (E2E_SMStateType)0x17u
/*An internal library error has occurred (e.g. error detected by program
flow monitoring, violated invariant or postcondition)*/
#define E2E_E_INTERR (E2E_SMStateType)0x19u
/*Function completed successfully*/
#define E2E_E_OK (E2E_SMStateType)0x00u
/*Function executed in wrong state*/
#define E2E_E_WRONGSTATE (E2E_SMStateType)0x1Au
    /******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/
    typedef struct
    {
        /*Size of the monitoring window for the state machine.*/
        uint8 WindowSizeValid;
        /*Minimal number of checks in which ProfileStatus equal to E2E_P_OK was determined
    within the last WindowSize checks (for the state E2E_SM_INIT) required to change to
    state E2E_SM_VALID.*/
        uint8 MinOkStateInit;
        /*Maximal number of checks in which ProfileStatus equal to E2E_P_ERROR was determined,
    within the last WindowSize checks (for the state E2E_SM_INIT).*/
        uint8 MaxErrorStateInit;
        /*Minimal number of checks in which ProfileStatus equal to E2E_P_OK was determined within
    the last WindowSize checks (for the state E2E_SM_VALID) required to keep in state E2E_SM_VALID.*/
        uint8 MinOkStateValid;
        /*Maximal number of checks in which ProfileStatus equal to E2E_P_ERROR was determined, within the last
    WindowSize checks (for the state E2E_SM_VALID).*/
        uint8 MaxErrorStateValid;
        /*Minimum number of checks in which ProfileStatus equal to E2E_P_OK was determined within the last
    WindowSize checks (for the state E2E_SM_INVALID) required to change to state E2E_SM_VALID.*/
        uint8 MinOkStateInvalid;
        /*Maximal number of checks in which ProfileStatus equal to E2E_P_ERROR was determined, within the last
    WindowSize checks (for the state E2E_SM_INVALID).*/
        uint8 MaxErrorStateInvalid;
        /*Size of the monitoring windows for the state machine during state INIT.*/
        uint8 WindowSizeInit;
        /*Size of the monitoring window for the state machine during state INVALID*/
        uint8 WindowSizeInvalid;
        /*Clear monitoring window data on transition to state INVALID.*/
        boolean ClearToInvalid;
        /*Restrict/allow tranistion from states INIT/NODATA to INVALID state.*/
        boolean transitToInvalidExtended;
    } E2E_SMConfigType;

    typedef struct
    {
        /*Pointer to an array, in which the ProfileStatus-es of the last E2Echecks are stored.
    The array size shall be WindowSize*/
        P2VAR(uint8, AUTOMATIC, E2E_APPL_DATA) ProfileStatusWindow;
        /*index in the array, at which the next ProfileStatus is to be written.*/
        uint8 WindowTopIndex;
        /*Count of checks in which ProfileStatus equal to E2E_P_OK was determined, within the last
    WindowSize checks.*/
        uint8 OkCount;
        /*Count of checks in which ProfileStatus equal to E2E_P_ERROR was determined,within the
    last WindowSize checks.*/
        uint8 ErrorCount;
        /*The current state in the state machine.*/
        E2E_SMStateType SMState;
    } E2E_SMCheckStateType;

/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/
/*******************************************************************************
**                      Global Functions                                      **
*******************************************************************************/
/**
 * Checks the communication channel. It determines if the data can be used for safety-related application,
 * based on history of checks performed by a corresponding E2E_P0XCheck() function.
 * Service ID: 0x30
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): ProfileStatus,Profile-independent status of the reception on one single Data in one cycle.
 *                 ConfigPtr,Pointer to static configuration.
 * Parameters(INOUT): StatePtr,Pointer to port/data communication state.
 * Parameters(OUT): NA
 * Return value: Std_ReturnType,E2E_E_INPUTERR_NULL E2E_E_INPUTERR_WRONG E2E_E_INTERR E2E_E_OK E2E_E_WRONGSTATE.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
    extern FUNC(Std_ReturnType, E2E_CODE) E2E_SMCheck(E2E_PCheckStatusType ProfileStatus,
                                                      P2CONST(E2E_SMConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr,
                                                      P2VAR(E2E_SMCheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
/**
 * Initializes the state machine.
 * Service ID: 0x31
 * Sync/Async: synchronous
 * Reentrancy: Reentrant
 * Parameters(IN): ConfigPtr,Pointer to configuration of the state machine.
 * Parameters(INOUT): StatePtr,Pointer to port/data communication state.
 * Parameters(OUT): NA
 * Return value: Std_ReturnType,E2E_E_INPUTERR_NULL E2E_E_OK.
 *
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
    extern FUNC(Std_ReturnType, E2E_CODE)
        E2E_SMCheckInit(P2VAR(E2E_SMCheckStateType, AUTOMATIC, E2E_APPL_DATA) StatePtr,
                        P2CONST(E2E_SMConfigType, AUTOMATIC, E2E_APPL_DATA) ConfigPtr);
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
#ifdef __cplusplus
}

#endif /* end of __cplusplus */
#endif
