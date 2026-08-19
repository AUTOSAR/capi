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
**  FILENAME    : Crc_MemMap.h                                                **
**                                                                            **
**  Created on  :                                                             **
**  Author      : ShenXu,Yb                                                   **
**  Vendor      :                                                             **
**  DESCRIPTION :                                                             **
**                                                                            **
**  SPECIFICATION(S) :   AUTOSAR classic Platform R19-11                      **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/

#define CRC_MEMMAP_H_AR_MAJOR_VERSION 4U
#define CRC_MEMMAP_H_AR_MINOR_VERSION 5U
#define CRC_MEMMAP_H_AR_PATCH_VERSION 0U
#define CRC_MEMMAP_H_SW_MAJOR_VERSION 2U
#define CRC_MEMMAP_H_SW_MINOR_VERSION 1U
#define CRC_MEMMAP_H_SW_PATCH_VERSION 0U

/*
 * <SIZE> -- BOOLEAN/8BIT/16BIT/32BIT/UNSPECIFIED
 */

/*
 * <MSN>_START_SEC_CODE
 * <MSN>_STOP_SEC_CODE
 * To be used for mapping code to applicaiton block,boot block,external flash etc.
 */

/*
 * <MSN>_START_SEC_CODE_FAST
 * <MSN>_STOP_SEC_CODE_FAST
 * To be used for mapping code to local block,boot block of ISR API.
 */

/*
 * <MSN>_START_SEC_VAR_NO_INIT_<SIZE>
 * <MSN>_STOP_SEC_VAR_NO_INIT_<SIZE>
 * To be used for all global or static variables that are never initialized.
 */

/*
 * <MSN>_START_SEC_VAR_POWER_ON_INIT_<SIZE>
 * <MSN>_STOP_SEC_VAR_POWER_ON_INIT_<SIZE>
 * To be used for all global or static variables that are initialized only after power on reset.
 */

/*
 * <MSN>_START_SEC_VAR_<SIZE>
 * <MSN>_STOP_SEC_VAR_<SIZE>
 * To be used for all global or static variables that are initialized after every reset(the normal case).
 */

/*
 * <MSN>_START_SEC_VAR_FAST_NO_INIT_<SIZE>
 * <MSN>_STOP_SEC_VAR_FAST_NO_INIT_<SIZE>
 * To be used for all global or static variables that have at least one of the following properties:
 *      *accessed bitwise
 *      *frequently used
 *      *high number of accesses in source code
 *  Some platforms allow the use of bit instructions for variables located in this specific RAM area as
 *  well as shorter addressing instructions.This saves code and runtime.
 */

/*
 * <MSN>_START_SEC_VAR_FAST_POWER_ON_INIT_<SIZE>
 * <MSN>_STOP_SEC_VAR_FAST_POWER_ON_INIT_<SIZE>
 * To be used for all global or static variables that have at least one of the following properties:
 *      *accessed bitwise
 *      *frequently used
 *      *high number of accesses in source code
 *  Some platforms allow the use of bit instructions for variables located in this specific RAM area as
 *  well as shorter addressing instructions.This saves code and runtime.
 */

/*
 * <MSN>_START_SEC_VAR_FAST_<SIZE>
 * <MSN>_STOP_SEC_VAR_FAST_<SIZE>
 * To be used for all global or static variables that have at least one of the following properties:
 *      *accessed bitwise
 *      *frequently used
 *      *high number of accesses in source code
 *  Some platforms allow the use of bit instructions for variables located in this specific RAM area as
 *  well as shorter addressing instructions.This saves code and runtime.
 */

/*
 * <MSN>_START_SEC_CONST_<SIZE>
 * <MSN>_STOP_SEC_CONST_<SIZE>
 * To be used for all global or static constants.
 */

/*
 * <MSN>_START_SEC_CONST_FAST_<SIZE>
 * <MSN>_STOP_SEC_CONST_FAST_<SIZE>
 * To be used for all global or static constants in fast segment block.
 */

/*
 * <MSN>_START_CONST_PBCFG
 * <MSN>_STOP_CONST_PBCFG
 * Constants with attributes that show that they reside in one segment for module configuration.
 */

/*
 * <MSN>_START_CONST_PBCFG_ROOT
 * <MSN>_STOP_CONST_PBCFG_ROOT
 * Constants with attributes that show that they reside in one segment for module configuration for root structure.
 */

/* @req MEMMAP010 @req MEMMAP004 @req MEMMAP021 @req MEMMAP003 @req MEMMAP018 */
/*=======[M A C R O S]========================================================*/
#if defined(START_WITH_IF)

/*=======[C R C]==============================================================*/

#elif defined(CRC_START_SEC_CODE)
    #undef CRC_START_SEC_CODE
    #define START_SEC_CODE
#elif defined(CRC_STOP_SEC_CODE)
    #undef CRC_STOP_SEC_CODE
    #define STOP_SEC_CODE

#elif defined(CRC_START_SEC_CODE_FAST)
    #undef CRC_START_SEC_CODE_FAST
    #define START_SEC_CODE_FAST
#elif defined(CRC_STOP_SEC_CODE_FAST)
    #undef CRC_STOP_SEC_CODE_FAST
    #define STOP_SEC_CODE_FAST

#elif defined(CRC_START_SEC_VAR_NO_INIT_BOOLEAN)
    #undef CRC_START_SEC_VAR_NO_INIT_BOOLEAN
    #define START_SEC_VAR_NO_INIT_BOOLEAN
#elif defined(CRC_STOP_SEC_VAR_NO_INIT_BOOLEAN)
    #undef CRC_STOP_SEC_VAR_NO_INIT_BOOLEAN
    #define STOP_SEC_VAR_NO_INIT_BOOLEAN

#elif defined(CRC_START_SEC_VAR_NO_INIT_8)
    #undef CRC_START_SEC_VAR_NO_INIT_8
    #define START_SEC_VAR_NO_INIT_8
#elif defined(CRC_STOP_SEC_VAR_NO_INIT_8)
    #undef CRC_STOP_SEC_VAR_NO_INIT_8
    #define STOP_SEC_VAR_NO_INIT_8

#elif defined(CRC_START_SEC_VAR_NO_INIT_16)
    #undef CRC_START_SEC_VAR_NO_INIT_16
    #define START_SEC_VAR_NO_INIT_16
#elif defined(CRC_STOP_SEC_VAR_NO_INIT_16)
    #undef CRC_STOP_SEC_VAR_NO_INIT_16
    #define STOP_SEC_VAR_NO_INIT_16

#elif defined(CRC_START_SEC_VAR_NO_INIT_32)
    #undef CRC_START_SEC_VAR_NO_INIT_32
    #define START_SEC_VAR_NO_INIT_32
#elif defined(CRC_STOP_SEC_VAR_NO_INIT_32)
    #undef CRC_STOP_SEC_VAR_NO_INIT_32
    #define STOP_SEC_VAR_NO_INIT_32

#elif defined(CRC_START_SEC_VAR_NO_INIT_UNSPECIFIED)
    #undef CRC_START_SEC_VAR_NO_INIT_UNSPECIFIED
    #define START_SEC_VAR_NO_INIT_UNSPECIFIED
#elif defined(CRC_STOP_SEC_VAR_NO_INIT_UNSPECIFIED)
    #undef CRC_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
    #define STOP_SEC_VAR_NO_INIT_UNSPECIFIED

#elif defined(CRC_START_SEC_VAR_POWER_ON_INIT_BOOLEAN)
    #undef CRC_START_SEC_VAR_POWER_ON_INIT_BOOLEAN
    #define START_SEC_VAR_POWER_ON_INIT_BOOLEAN
#elif defined(CRC_STOP_SEC_VAR_POWER_ON_INIT_BOOLEAN)
    #undef CRC_STOP_SEC_VAR_POWER_ON_INIT_BOOLEAN
    #define STOP_SEC_VAR_POWER_ON_INIT_BOOLEAN

#elif defined(CRC_START_SEC_VAR_POWER_ON_INIT_8)
    #undef CRC_START_SEC_VAR_POWER_ON_INIT_8
    #define START_SEC_VAR_POWER_ON_INIT_8
#elif defined(CRC_STOP_SEC_VAR_POWER_ON_INIT_8)
    #undef CRC_STOP_SEC_VAR_POWER_ON_INIT_8
    #define STOP_SEC_VAR_POWER_ON_INIT_8

#elif defined(CRC_START_SEC_VAR_POWER_ON_INIT_16)
    #undef CRC_START_SEC_VAR_POWER_ON_INIT_16
    #define START_SEC_VAR_POWER_ON_INIT_16
#elif defined(CRC_STOP_SEC_VAR_POWER_ON_INIT_16)
    #undef CRC_STOP_SEC_VAR_POWER_ON_INIT_16
    #define STOP_SEC_VAR_POWER_ON_INIT_16

#elif defined(CRC_START_SEC_VAR_POWER_ON_INIT_32)
    #undef CRC_START_SEC_VAR_POWER_ON_INIT_32
    #define START_SEC_VAR_POWER_ON_INIT_32
#elif defined(CRC_STOP_SEC_VAR_POWER_ON_INIT_32)
    #undef CRC_STOP_SEC_VAR_POWER_ON_INIT_32
    #define STOP_SEC_VAR_POWER_ON_INIT_32

#elif defined(CRC_START_SEC_VAR_POWER_ON_INIT_UNSPECIFIED)
    #undef CRC_START_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
    #define START_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
#elif defined(CRC_STOP_SEC_VAR_POWER_ON_INIT_UNSPECIFIED)
    #undef CRC_STOP_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
    #define STOP_SEC_VAR_POWER_ON_INIT_UNSPECIFIED

#elif defined(CRC_START_SEC_VAR_BOOLEAN)
    #undef CRC_START_SEC_VAR_BOOLEAN
    #define START_SEC_VAR_BOOLEAN
#elif defined(CRC_STOP_SEC_VAR_BOOLEAN)
    #undef CRC_STOP_SEC_VAR_BOOLEAN
    #define STOP_SEC_VAR_BOOLEAN

#elif defined(CRC_START_SEC_VAR_8)
    #undef CRC_START_SEC_VAR_8
    #define START_SEC_VAR_8
#elif defined(CRC_STOP_SEC_VAR_8)
    #undef CRC_STOP_SEC_VAR_8
    #define STOP_SEC_VAR_8

#elif defined(CRC_START_SEC_VAR_16)
    #undef CRC_START_SEC_VAR_16
    #define START_SEC_VAR_16
#elif defined(CRC_STOP_SEC_VAR_16)
    #undef CRC_STOP_SEC_VAR_16
    #define STOP_SEC_VAR_16

#elif defined(CRC_START_SEC_VAR_32)
    #undef CRC_START_SEC_VAR_32
    #define START_SEC_VAR_32
#elif defined(CRC_STOP_SEC_VAR_32)
    #undef CRC_STOP_SEC_VAR_32
    #define STOP_SEC_VAR_32

#elif defined(CRC_START_SEC_VAR_UNSPECIFIED)
    #undef CRC_START_SEC_VAR_UNSPECIFIED
    #define START_SEC_VAR_UNSPECIFIED
#elif defined(CRC_STOP_SEC_VAR_UNSPECIFIED)
    #undef CRC_STOP_SEC_VAR_UNSPECIFIED
    #define STOP_SEC_VAR_UNSPECIFIED

#elif defined(CRC_START_SEC_VAR_NO_INIT_BOOLEAN)
    #undef CRC_START_SEC_VAR_NO_INIT_BOOLEAN
    #define START_SEC_VAR_NO_INIT_BOOLEAN
#elif defined(CRC_STOP_SEC_VAR_NO_INIT_BOOLEAN)
    #undef CRC_STOP_SEC_VAR_NO_INIT_BOOLEAN
    #define STOP_SEC_VAR_NO_INIT_BOOLEAN

#elif defined(CRC_START_SEC_VAR_NO_INIT_8)
    #undef CRC_START_SEC_VAR_NO_INIT_8
    #define START_SEC_VAR_NO_INIT_8
#elif defined(CRC_STOP_SEC_VAR_NO_INIT_8)
    #undef CRC_STOP_SEC_VAR_NO_INIT_8
    #define STOP_SEC_VAR_NO_INIT_8

#elif defined(CRC_START_SEC_VAR_NO_INIT_16)
    #undef CRC_START_SEC_VAR_NO_INIT_16
    #define START_SEC_VAR_NO_INIT_16
#elif defined(CRC_STOP_SEC_VAR_NO_INIT_16)
    #undef CRC_STOP_SEC_VAR_NO_INIT_16
    #define STOP_SEC_VAR_NO_INIT_16

#elif defined(CRC_START_SEC_VAR_NO_INIT_32)
    #undef CRC_START_SEC_VAR_NO_INIT_32
    #define START_SEC_VAR_NO_INIT_32
#elif defined(CRC_STOP_SEC_VAR_NO_INIT_32)
    #undef CRC_STOP_SEC_VAR_NO_INIT_32
    #define STOP_SEC_VAR_NO_INIT_32

#elif defined(CRC_START_SEC_VAR_NO_INIT_UNSPECIFIED)
    #undef CRC_START_SEC_VAR_NO_INIT_UNSPECIFIED
    #define START_SEC_VAR_NO_INIT_UNSPECIFIED
#elif defined(CRC_STOP_SEC_VAR_NO_INIT_UNSPECIFIED)
    #undef CRC_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
    #define STOP_SEC_VAR_NO_INIT_UNSPECIFIED

#elif defined(CRC_START_SEC_VAR_POWER_ON_INIT_BOOLEAN)
    #undef CRC_START_SEC_VAR_POWER_ON_INIT_BOOLEAN
    #define START_SEC_VAR_POWER_ON_INIT_BOOLEAN
#elif defined(CRC_STOP_SEC_VAR_POWER_ON_INIT_BOOLEAN)
    #undef CRC_STOP_SEC_VAR_POWER_ON_INIT_BOOLEAN
    #define STOP_SEC_VAR_POWER_ON_INIT_BOOLEAN

#elif defined(CRC_START_SEC_VAR_POWER_ON_INIT_8)
    #undef CRC_START_SEC_VAR_POWER_ON_INIT_8
    #define START_SEC_VAR_POWER_ON_INIT_8
#elif defined(CRC_STOP_SEC_VAR_POWER_ON_INIT_8)
    #undef CRC_STOP_SEC_VAR_POWER_ON_INIT_8
    #define STOP_SEC_VAR_POWER_ON_INIT_8

#elif defined(CRC_START_SEC_VAR_POWER_ON_INIT_16)
    #undef CRC_START_SEC_VAR_POWER_ON_INIT_16
    #define START_SEC_VAR_POWER_ON_INIT_16
#elif defined(CRC_STOP_SEC_VAR_POWER_ON_INIT_16)
    #undef CRC_STOP_SEC_VAR_POWER_ON_INIT_16
    #define STOP_SEC_VAR_POWER_ON_INIT_16

#elif defined(CRC_START_SEC_VAR_POWER_ON_INIT_32)
    #undef CRC_START_SEC_VAR_POWER_ON_INIT_32
    #define START_SEC_VAR_POWER_ON_INIT_32
#elif defined(CRC_STOP_SEC_VAR_POWER_ON_INIT_32)
    #undef CRC_STOP_SEC_VAR_POWER_ON_INIT_32
    #define STOP_SEC_VAR_POWER_ON_INIT_32

#elif defined(CRC_START_SEC_VAR_POWER_ON_INIT_UNSPECIFIED)
    #undef CRC_START_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
    #define START_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
#elif defined(CRC_STOP_SEC_VAR_POWER_ON_INIT_UNSPECIFIED)
    #undef CRC_STOP_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
    #define STOP_SEC_VAR_POWER_ON_INIT_UNSPECIFIED

#elif defined(CRC_START_SEC_VAR_BOOLEAN)
    #undef CRC_START_SEC_VAR_BOOLEAN
    #define START_SEC_VAR_BOOLEAN
#elif defined(CRC_STOP_SEC_VAR_BOOLEAN)
    #undef CRC_STOP_SEC_VAR_BOOLEAN
    #define STOP_SEC_VAR_BOOLEAN

#elif defined(CRC_START_SEC_VAR_8)
    #undef CRC_START_SEC_VAR_8
    #define START_SEC_VAR_8
#elif defined(CRC_STOP_SEC_VAR_8)
    #undef CRC_STOP_SEC_VAR_8
    #define STOP_SEC_VAR_8

#elif defined(CRC_START_SEC_VAR_16)
    #undef CRC_START_SEC_VAR_16
    #define START_SEC_VAR_16
#elif defined(CRC_STOP_SEC_VAR_16)
    #undef CRC_STOP_SEC_VAR_16
    #define STOP_SEC_VAR_16

#elif defined(CRC_START_SEC_VAR_32)
    #undef CRC_START_SEC_VAR_32
    #define START_SEC_VAR_32
#elif defined(CRC_STOP_SEC_VAR_32)
    #undef CRC_STOP_SEC_VAR_32
    #define STOP_SEC_VAR_32

#elif defined(CRC_START_SEC_VAR_UNSPECIFIED)
    #undef CRC_START_SEC_VAR_UNSPECIFIED
    #define START_SEC_VAR_UNSPECIFIED
#elif defined(CRC_STOP_SEC_VAR_UNSPECIFIED)
    #undef CRC_STOP_SEC_VAR_UNSPECIFIED
    #define STOP_SEC_VAR_UNSPECIFIED

#elif defined(CRC_START_SEC_CONST_BOOLEAN)
    #undef CRC_START_SEC_CONST_BOOLEAN
    #define START_SEC_CONST_BOOLEAN
#elif defined(CRC_STOP_SEC_CONST_BOOLEAN)
    #undef CRC_STOP_SEC_CONST_BOOLEAN
    #define STOP_SEC_CONST_BOOLEAN

#elif defined(CRC_START_SEC_CONST_8)
    #undef CRC_START_SEC_CONST_8
    #define START_SEC_CONST_8
#elif defined(CRC_STOP_SEC_CONST_8)
    #undef CRC_STOP_SEC_CONST_8
    #define STOP_SEC_CONST_8

#elif defined(CRC_START_SEC_CONST_16)
    #undef CRC_START_SEC_CONST_16
    #define START_SEC_CONST_16
#elif defined(CRC_STOP_SEC_CONST_16)
    #undef CRC_STOP_SEC_CONST_16
    #define STOP_SEC_CONST_16

#elif defined(CRC_START_SEC_CONST_32)
    #undef CRC_START_SEC_CONST_32
    #define START_SEC_CONST_32
#elif defined(CRC_STOP_SEC_CONST_32)
    #undef CRC_STOP_SEC_CONST_32
    #define STOP_SEC_CONST_32

#elif defined(CRC_START_SEC_CONST_UNSPECIFIED)
    #undef CRC_START_SEC_CONST_UNSPECIFIED
    #define START_SEC_CONST_UNSPECIFIED
#elif defined(CRC_STOP_SEC_CONST_UNSPECIFIED)
    #undef CRC_STOP_SEC_CONST_UNSPECIFIED
    #define STOP_SEC_CONST_UNSPECIFIED

#elif defined(CRC_START_SEC_CONST_BOOLEAN)
    #undef CRC_START_SEC_CONST_BOOLEAN
    #define START_SEC_CONST_BOOLEAN
#elif defined(CRC_STOP_SEC_CONST_BOOLEAN)
    #undef CRC_STOP_SEC_CONST_BOOLEAN
    #define STOP_SEC_CONST_BOOLEAN

#elif defined(CRC_START_SEC_CONST_8)
    #undef CRC_START_SEC_CONST_8
    #define START_SEC_CONST_8
#elif defined(CRC_STOP_SEC_CONST_8)
    #undef CRC_STOP_SEC_CONST_8
    #define STOP_SEC_CONST_8

#elif defined(CRC_START_SEC_CONST_16)
    #undef CRC_START_SEC_CONST_16
    #define START_SEC_CONST_16
#elif defined(CRC_STOP_SEC_CONST_16)
    #undef CRC_STOP_SEC_CONST_16
    #define STOP_SEC_CONST_16

#elif defined(CRC_START_SEC_CONST_32)
    #undef CRC_START_SEC_CONST_32
    #define START_SEC_CONST_32
#elif defined(CRC_STOP_SEC_CONST_32)
    #undef CRC_STOP_SEC_CONST_32
    #define STOP_SEC_CONST_32

#elif defined(CRC_START_SEC_CONST_UNSPECIFIED)
    #undef CRC_START_SEC_CONST_UNSPECIFIED
    #define START_SEC_CONST_UNSPECIFIED
#elif defined(CRC_STOP_SEC_CONST_UNSPECIFIED)
    #undef CRC_STOP_SEC_CONST_UNSPECIFIED
    #define STOP_SEC_CONST_UNSPECIFIED

#elif defined(CRC_START_SEC_CONST_PBCFG)
    #undef CRC_START_SEC_CONST_PBCFG
    #define START_SEC_CONST_PBCFG
#elif defined(CRC_STOP_SEC_CONST_PBCFG)
    #undef CRC_STOP_SEC_CONST_PBCFG
    #define STOP_SEC_CONST_PBCFG

#elif defined(CRC_START_SEC_CONFIG_DATA)
    #undef CRC_START_SEC_CONFIG_DATA
    #define START_SEC_CONFIG_DATA
#elif defined(CRC_STOP_SEC_CONFIG_DATA)
    #undef CRC_STOP_SEC_CONFIG_DATA
    #define STOP_SEC_CONFIG_DATA

#endif /* START_WITH_IF */

#include "MemMap.h"
/*=======[E N D   O F   F I L E]==============================================*/
