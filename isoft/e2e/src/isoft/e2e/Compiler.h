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

/*============================================================================*/
/*
 *  
 *  @file       <Compiler.h>
 *  @brief      <Briefly describe file(one line)>
 *  
 *  <Compiler: TASKING    MCU:TC17XX>
 *  
 *  @author     <chen xue hua>
 *  @date       <2013-02-27>
 */
/*============================================================================*/
/* @req COMPILER004 @req COMPILER047 @req COMPILER050 @req COMPILER042 */
#ifndef COMPILER_H
    #define COMPILER_H

    /*=======[R E V I S I O N   H I S T O R Y]====================================*/
    /*  <VERSION>    <DATE>    <AUTHOR>    <REVISION LOG>
 *  V1.0.0       20130227  chenxuehua  Initial version
 *  V1.0.1       20140815  chenxuehua  Add MICRO "FAR" and "NEAR"
 */
    /*============================================================================*/

    /*=======[V E R S I O N  I N F O R M A T I O N]===============================*/
    #define COMPILER_VENDOR_ID        0
    #define COMPILER_MODULE_ID        0
    #define COMPILER_AR_MAJOR_VERSION 2
    #define COMPILER_AR_MINOR_VERSION 0
    #define COMPILER_AR_PATCH_VERSION 2
    #define COMPILER_SW_MAJOR_VERSION 1
    #define COMPILER_SW_MINOR_VERSION 0
    #define COMPILER_SW_PATCH_VERSION 1
    #define COMPILER_VENDOR_API_INFIX 0

    /*=======[I N C L U D E S]====================================================*/
    /* @req COMPILER052 */
    #include "Compiler_Cfg.h"

    /* @req COMPILER053 @req COMPILER005 @req COMPILER035 @req COMPILER036 */
    /* @req COMPILER042 */
    /*=======[M A C R O S]========================================================*/
    /* @req COMPILER003 @req COMPILER010 @req COMPILER030 @req COMPILER012 */
    #ifndef _DIABDATA_C_MPC5554_
        #define _DIABDATA_C_MPC5554_
    #endif

    /* @req COMPILER046 */
    /* 
 * The memory class AUTOMATIC shall be provided as empty definition,used for 
 * the declaration of local pointers 
 */
    #define AUTOMATIC

    /* @req COMPILER059 */
    /* TYPEDEF used for defining pointer types within type definitions */
    #define TYPEDEF

    /* @req COMPILER049 */
    #define STATIC

    /* @req COMPILER051 */
    #ifndef NULL_PTR
        #define NULL_PTR ((void *)0)
    #endif

    /* @req COMPILER057 */
    #define INLINE inline

    #define INTERRUPT __interrupt

    #define TRAP_FAST __trap_fast

    #define ASM __asm

    #define TRAP __trap

    #define FAR

    #define NEAR

    #define ALIGN __align

    /* @req COMPILER001 @req COMPILER058 */
    /*
 * The compiler abstraction shall define the FUNC macro for the declaration and
 * definition of functions, that ensures correct syntax of function 
 * declarations as required by a specific compiler. - used for API functions
 * rettype     return type of the function
 * memclass    classification of the function itself
 */
    #define FUNC(rettype, memclass) rettype

    /* @req COMPILER006 */
    /*
 * Pointer to variable data
 * ptrtype     type of the referenced data
 * memclass    classification of the pointer's variable itself
 * ptrclass    defines the classification of the pointer's distance
 */
    #define P2VAR(ptrtype, memclass, ptrclass) ptrtype *

    /* @req COMPILER013 */
    /*
 * Pointer to constant data
 * ptrtype     type of the referenced data
 * memclass    classification of the pointer's variable itself
 * ptrclass    defines the classification of the pointer's distance
 */
    #define P2CONST(ptrtype, memclass, ptrclass) const ptrtype *

    /* @req COMPILER031 */
    /*
 * Const pointer to variable data
 * ptrtype     type of the referenced data
 * memclass    classification of the pointer's variable itself
 * ptrclass    defines the classification of the pointer's distance
 */
    #define CONSTP2VAR(ptrtype, memclass, ptrclass) ptrtype *const

    /* @req COMPILER032 */
    /*
 * Const pointer to constant data
 * ptrtype     type of the referenced data
 * memclass    classification of the pointer's variable itself
 * ptrclass    defines the classification of the pointer's distance
 */
    #define CONSTP2CONST(ptrtype, memclass, ptrclass) const ptrtype *const

    /* @req COMPILER039 */
    /*
 * Type definition of pointers to functions
 * rettype     return type of the function
 * ptrclass    defines the classification of the pointer's distance
 * fctname     function name respectively name of the defined type
 */
    #define P2FUNC(rettype, ptrclass, fctname) rettype(*fctname)

    /* @req COMPILER023 */
    /*
 * ROM constant
 * type        type of the constant
 * memclass    classification of the constant
 */
    #define CONST(type, memclass) const type

    /* @req COMPILER026 */
    /*
 * RAM variables
 * type        type of the variable
 * memclass    classification of the variable
 */
    #define VAR(type, memclass) type

#endif /* end of COMPILER_H */

/*=======[E N D   O F   F I L E]==============================================*/
