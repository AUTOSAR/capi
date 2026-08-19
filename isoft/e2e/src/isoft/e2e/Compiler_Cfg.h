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
 *  @file       <Compiler_Cfg.h>
 *  @brief      <Briefly describe file(one line)>
 *  
 *  <Compiler: TASKING    MCU:TC17XX>
 *  
 *  @author     <chen xue hua>
 *  @date       <2013-02-27>
 */
/*============================================================================*/
#ifndef COMPILER_CFG_H
    #define COMPILER_CFG_H

    /*=======[R E V I S I O N   H I S T O R Y]====================================*/
    /*  <VERSION>    <DATE>    <AUTHOR>    <REVISION LOG>
 *  V1.0.0       20130227  chenxuehua  Initial version
 *  V1.0.1       20140916  wbn         add EcuM_MemoryAndPointerClasses
 */
    /*============================================================================*/

    /*=======[V E R S I O N  I N F O R M A T I O N]===============================*/
    #define COMPILER_CFG_VENDOR_ID        62
    #define COMPILER_CFG_MODULE_ID        0
    #define COMPILER_CFG_AR_MAJOR_VERSION 2
    #define COMPILER_CFG_AR_MINOR_VERSION 0
    #define COMPILER_CFG_AR_PATCH_VERSION 2
    #define COMPILER_CFG_SW_MAJOR_VERSION 1
    #define COMPILER_CFG_SW_MINOR_VERSION 0
    #define COMPILER_CFG_SW_PATCH_VERSION 0
    #define COMPILER_CFG_VENDOR_API_INFIX 0

    /* @req COMPILER055 @req COMPILER054 */
    /*=======[M A C R O S]========================================================*/

    /*=======[Os_MemoryAndPointerClasses]========================================*/
    /* Configurable memory class for code. */
    #define OS_CODE

    /* Configurable memory class for ISR code. */
    #define OS_CODE_FAST

    /* 
 * Configurable memory class for all global or static variables that are never 
 * initialized. 
 */
    #define OS_VAR_NOINIT

    /* 
 * Configurable memory class for all global or static variables that are 
 * initialized only after power on reset. 
 */
    #define OS_VAR_POWER_ON_INIT

    /*
 * Configurable memory class for all global or static variables that are 
 * initialized after every reset. 
 */
    #define OS_VAR

    /* 
 * Configurable memory class for all global or static variables that will 
 * be accessed frequently. 
 */
    #define OS_VAR_NOINIT_FAST

    /* 
 * Configurable memory class for all global or static variables that have at 
 * be accessed frequently. 
 */
    #define OS_VAR_POWER_ON_INIT_FAST

    /* 
 * Configurable memory class for all global or static variables that have at 
 * be accessed frequently. 
 */
    #define OS_VAR_FAST

    /* Configurable memory class for global or static constants. */
    #define OS_CONST

    /*
 * Configurable memory class for global or static constants that will be 
 * accessed frequently.
 */
    #define OS_CONST_FAST

    /* Configurable memory class for global or static constants in post build. */
    #define OS_CONST_PBCFG

    /*
 * Configurable memory class for pointers to applicaiton data(expected to be 
 * in RAM or ROM)passed via API.
 */
    #define OS_APPL_DATA

    /* 
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
    #define OS_APPL_CONST

    /* 
 * Configurable memory class for pointers to applicaiton functions(e.g. call 
 * back function pointers).
 */
    #define OS_APPL_CODE

    /* @req COMPILER044 @req COMPILER040 */
    /*=======[Can_MemoryAndPointerClasses]========================================*/
    /* Configurable memory class for code. */
    #define CAN_CODE

    /* Configurable memory class for ISR code. */
    #define CAN_CODE_FAST

    /* 
 * Configurable memory class for all global or static variables that are never 
 * initialized. 
 */
    #define CAN_VAR_NOINIT

    /* 
 * Configurable memory class for all global or static variables that are 
 * initialized only after power on reset. 
 */
    #define CAN_VAR_POWER_ON_INIT

    /*
 * Configurable memory class for all global or static variables that are 
 * initialized after every reset. 
 */
    #define CAN_VAR

    /* 
 * Configurable memory class for all global or static variables that will 
 * be accessed frequently. 
 */
    #define CAN_VAR_NOINIT_FAST

    /* 
 * Configurable memory class for all global or static variables that have at 
 * be accessed frequently. 
 */
    #define CAN_VAR_POWER_ON_INIT_FAST

    /* 
 * Configurable memory class for all global or static variables that have at 
 * be accessed frequently. 
 */
    #define CAN_VAR_FAST

    /* Configurable memory class for global or static constants. */
    #define CAN_CONST

    /*
 * Configurable memory class for global or static constants that will be 
 * accessed frequently.
 */
    #define CAN_CONST_FAST

    /* Configurable memory class for global or static constants in post build. */
    #define CAN_CONST_PBCFG

    /*
 * Configurable memory class for pointers to applicaiton data(expected to be 
 * in RAM or ROM)passed via API.
 */
    #define CAN_APPL_DATA

    /* 
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
    #define CAN_APPL_CONST

    /* 
 * Configurable memory class for pointers to applicaiton functions(e.g. call 
 * back function pointers).
 */
    #define CAN_APPL_CODE

    /*=======[CanIf_MemoryAndPointerClasses]========================================*/
    #define CANIF_CODE

    #define CANIF_CODE_FAST

    #define CANIF_VAR_NOINIT

    #define CANIF_VAR_POWER_ON_INIT

    #define CANIF_VAR

    #define CANIF_VAR_NOINIT_FAST

    #define CANIF_VAR_POWER_ON_INIT_FAST

    #define CANIF_VAR_FAST

    #define CANIF_CONST

    #define CANIF_CONST_FAST

    #define CANIF_CONST_PBCFG

    #define CANIF_APPL_DATA

    #define CANIF_APPL_CONST

    #define CANIF_APPL_CODE

    /*=======[LinIf_MemoryAndPointerClasses]========================================*/
    #define LINIF_CODE

    #define LINIF_CODE_FAST

    #define LINIF_VAR_NOINIT

    #define LINIF_VAR_POWER_ON_INIT

    #define LINIF_VAR

    #define LINIF_VAR_NOINIT_FAST

    #define LINIF_VAR_POWER_ON_INIT_FAST

    #define LINIF_VAR_FAST

    #define LINIF_CONST

    #define LINIF_CONST_FAST

    #define LINIF_CONST_PBCFG

    #define LINIF_APPL_DATA

    #define LINIF_APPL_CONST

    #define LINIF_APPL_CODE

    /* @req COMPILER044 @req COMPILER040 */
    /*=======[OSEKCOM_MemoryAndPointerClasses]====================================*/
    /* Configurable memory class for code. */
    #define COM_CODE

    /* Configurable memory class for ISR code. */
    #define COM_CODE_FAST

    /* 
 * Configurable memory class for all global or static variables that are never 
 * initialized. 
 */
    #define COM_VAR_NOINIT

    /* 
 * Configurable memory class for all global or static variables that are 
 * initialized only after power on reset. 
 */
    #define COM_VAR_POWER_ON_INIT

    /*
 * Configurable memory class for all global or static variables that are 
 * initialized after every reset. 
 */
    #define COM_VAR

    /* 
 * Configurable memory class for all global or static variables that will 
 * be accessed frequently. 
 */
    #define COM_VAR_NOINIT_FAST

    /* 
 * Configurable memory class for all global or static variables that have at 
 * be accessed frequently. 
 */
    #define COM_VAR_POWER_ON_INIT_FAST

    /* 
 * Configurable memory class for all global or static variables that have at 
 * be accessed frequently. 
 */
    #define COM_VAR_FAST

    /* Configurable memory class for global or static constants. */
    #define COM_CONST

    /*
 * Configurable memory class for global or static constants that will be 
 * accessed frequently.
 */
    #define COM_CONST_FAST

    /* Configurable memory class for global or static constants in post build. */
    #define COM_CONST_PBCFG

    /*
 * Configurable memory class for pointers to applicaiton data(expected to be 
 * in RAM or ROM)passed via API.
 */
    #define COM_APPL_DATA

    /* 
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
    #define COM_APPL_CONST

    /* 
 * Configurable memory class for pointers to applicaiton functions(e.g. call 
 * back function pointers).
 */
    #define COM_APPL_CODE
    /*=======[CanSM_MemoryAndPointerClasses]========================================*/
    #define CANSM_CODE

    #define CANSM_CODE_FAST

    #define CANSM_VAR_NOINIT

    #define CANSM_VAR_POWER_ON_INIT

    #define CANSM_VAR

    #define CANSM_VAR_NOINIT_FAST

    #define CANSM_VAR_POWER_ON_INIT_FAST

    #define CANSM_VAR_FAST

    #define CANSM_CONST

    #define CANSM_CONST_FAST

    #define CANSM_CONST_PBCFG

    #define CANSM_APPL_DATA

    #define CANSM_APPL_CONST

    #define CANSM_APPL_CODE

/*=======[NmIf_MemoryAndPointerClasses]========================================*/

    #define NM_CODE

    #define NM_VAR_NOINIT

    #define NM_VAR_POWER_ON_INIT

    #define NM_VAR

    #define NM_CONST

    #define NM_APPL_DATA

    #define NM_APPL_CONST

    #define NM_APPL_CODE

/*=======[OsekNm_MemoryAndPointerClasses]========================================*/

    #define OSEKNM_CODE

    #define OSEKNM_VAR_NOINIT

    #define OSEKNM_VAR_POWER_ON_INIT

    #define OSEKNM_VAR

    #define OSEKNM_CONST

    #define OSEKNM_APPL_DATA

    #define OSEKNM_APPL_CONST

    #define OSEKNM_APPL_CODE

    /*=======[ComM_MemoryAndPointerClasses]========================================*/
    #define COMM_CODE

    #define COMM_CODE_FAST

    #define COMM_VAR_NOINIT

    #define COMM_VAR_POWER_ON_INIT

    #define COMM_VAR

    #define COMM_VAR_NOINIT_FAST

    #define COMM_VAR_POWER_ON_INIT_FAST

    #define COMM_VAR_FAST

    #define COMM_CONST

    #define COMM_CONST_FAST

    #define COMM_CONST_PBCFG

    #define COMM_APPL_DATA

    #define COMM_APPL_CONST

    #define COMM_APPL_CODE

    /* @req COMPILER044 @req COMPILER040 */
    /*=======[XCP_MemoryAndPointerClasses]====================================*/
    /* Configurable memory class for code. */
    #define XCP_CODE

    /* Configurable memory class for ISR code. */
    #define XCP_CODE_FAST

    /* 
 * Configurable memory class for all global or static variables that are never 
 * initialized. 
 */
    #define XCP_VAR_NOINIT

    /* 
 * Configurable memory class for all global or static variables that are 
 * initialized only after power on reset. 
 */
    #define XCP_VAR_POWER_ON_INIT

    /*
 * Configurable memory class for all global or static variables that are 
 * initialized after every reset. 
 */
    #define XCP_VAR

    /* 
 * Configurable memory class for all global or static variables that will 
 * be accessed frequently. 
 */
    #define XCP_VAR_NOINIT_FAST

    /* 
 * Configurable memory class for all global or static variables that have at 
 * be accessed frequently. 
 */
    #define XCP_VAR_POWER_ON_INIT_FAST

    /* 
 * Configurable memory class for all global or static variables that have at 
 * be accessed frequently. 
 */
    #define XCP_VAR_FAST

    /* Configurable memory class for global or static constants. */
    #define XCP_CONST

    /*
 * Configurable memory class for global or static constants that will be 
 * accessed frequently.
 */
    #define XCP_CONST_FAST

    /* Configurable memory class for global or static constants in post build. */
    #define XCP_CONST_PBCFG

    /*
 * Configurable memory class for pointers to applicaiton data(expected to be 
 * in RAM or ROM)passed via API.
 */
    #define XCP_APPL_DATA

    /* 
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
    #define XCP_APPL_CONST

    /* 
 * Configurable memory class for pointers to applicaiton functions(e.g. call 
 * back function pointers).
 */
    #define XCP_APPL_CODE

    /*=======[EcuM_MemoryAndPointerClasses]========================================*/
    #define ECUM_CODE

    #define ECUM_CODE_FAST

    #define ECUM_VAR_NOINIT

    #define ECUM_VAR_POWER_ON_INIT

    #define ECUM_VAR

    #define ECUM_VAR_NOINIT_FAST

    #define ECUM_VAR_POWER_ON_INIT_FAST

    #define ECUM_VAR_FAST

    #define ECUM_CONST

    #define ECUM_CONST_FAST

    #define ECUM_CONST_PBCFG

    #define ECUM_APPL_DATA

    #define ECUM_APPL_CONST

    #define ECUM_APPL_CODE

    /*=======[Flash_MemoryAndPointerClasses]========================================*/
    #define FLS_CODE

    #define FLS_CODE_FAST

    #define FLS_VAR_NOINIT

    #define FLS_VAR_POWER_ON_INIT

    #define FLS_VAR

    #define FLS_VAR_NOINIT_FAST

    #define FLS_VAR_POWER_ON_INIT_FAST

    #define FLS_VAR_FAST

    #define FLS_CONST

    #define FLS_CONST_FAST

    #define FLS_CONST_PBCFG

    #define FLS_APPL_DATA

    #define FLS_APPL_CONST

    #define FLS_APPL_CODE

    /*=======[NvM_MemoryAndPointerClasses]========================================*/
    /* Configurable memory class for code. */
    #define NVM_CODE

    /* Configurable memory class for ISR code. */
    #define NVM_CODE_FAST

    /*
 * Configurable memory class for all global or static variables that are never
 * initialized.
 */
    #define NVM_VAR_NOINIT

    /*
 * Configurable memory class for all global or static variables that are
 * initialized only after power on reset.
 */
    #define NVM_VAR_POWER_ON_INIT

    /*
 * Configurable memory class for all global or static variables that are
 * initialized after every reset.
 */
    #define NVM_VAR

    /*
 * Configurable memory class for all global or static variables that will
 * be accessed frequently.
 */
    #define NVM_VAR_NOINIT_FAST

    /*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
    #define NVM_VAR_POWER_ON_INIT_FAST

    /*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
    #define NVM_VAR_FAST

    /* Configurable memory class for global or static constants. */
    #define NVM_CONST

    /*
 * Configurable memory class for global or static constants that will be
 * accessed frequently.
 */
    #define NVM_CONST_FAST

    /* Configurable memory class for global or static constants in post build. */
    #define NVM_CONST_PBCFG

    /*
 * Configurable memory class for pointers to applicaiton data(expected to be
 * in RAM or ROM)passed via API.
 */
    #define NVM_APPL_DATA

    /*
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
    #define NVM_APPL_CONST

    /*
 * Configurable memory class for pointers to applicaiton functions(e.g. call
 * back function pointers).
 */
    #define NVM_APPL_CODE

    /*=======[Crc_MemoryAndPointerClasses]========================================*/
    /* Configurable memory class for code. */
    #define CRC_CODE

    /* Configurable memory class for ISR code. */
    #define CRC_CODE_FAST

    /*
 * Configurable memory class for all global or static variables that are never
 * initialized.
 */
    #define CRC_VAR_NOINIT

    /*
 * Configurable memory class for all global or static variables that are
 * initialized only after power on reset.
 */
    #define CRC_VAR_POWER_ON_INIT

    /*
 * Configurable memory class for all global or static variables that are
 * initialized after every reset.
 */
    #define CRC_VAR

    /*
 * Configurable memory class for all global or static variables that will
 * be accessed frequently.
 */
    #define CRC_VAR_NOINIT_FAST

    /*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
    #define CRC_VAR_POWER_ON_INIT_FAST

    /*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
    #define CRC_VAR_FAST

    /* Configurable memory class for global or static constants. */
    #define CRC_CONST

    /*
 * Configurable memory class for global or static constants that will be
 * accessed frequently.
 */
    #define CRC_CONST_FAST

    /* Configurable memory class for global or static constants in post build. */
    #define CRC_CONST_PBCFG

    /*
 * Configurable memory class for pointers to applicaiton data(expected to be
 * in RAM or ROM)passed via API.
 */
    #define CRC_APPL_DATA

    /*
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
    #define CRC_APPL_CONST

    /*
 * Configurable memory class for pointers to applicaiton functions(e.g. call
 * back function pointers).
 */
    #define CRC_APPL_CODE

    /*=======[Dem_MemoryAndPointerClasses]========================================*/
    /* Configurable memory class for code. */
    #define DEM_CODE

    /* Configurable memory class for ISR code. */
    #define DEM_CODE_FAST

    /*
 * Configurable memory class for all global or static variables that are never
 * initialized.
 */
    #define DEM_VAR_NOINIT

    /*
 * Configurable memory class for all global or static variables that are
 * initialized only after power on reset.
 */
    #define DEM_VAR_POWER_ON_INIT

    /*
 * Configurable memory class for all global or static variables that are
 * initialized after every reset.
 */
    #define DEM_VAR

    /*
 * Configurable memory class for all global or static variables that will
 * be accessed frequently.
 */
    #define DEM_VAR_NOINIT_FAST

    /*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
    #define DEM_VAR_POWER_ON_INIT_FAST

    /*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
    #define DEM_VAR_FAST

    /* Configurable memory class for global or static constants. */
    #define DEM_CONST

    /*
 * Configurable memory class for global or static constants that will be
 * accessed frequently.
 */
    #define DEM_CONST_FAST

    /* Configurable memory class for global or static constants in post build. */
    #define DEM_CONST_PBCFG

    /*
 * Configurable memory class for pointers to applicaiton data(expected to be
 * in RAM or ROM)passed via API.
 */
    #define DEM_APPL_DATA

    /*
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
    #define DEM_APPL_CONST

    /*
 * Configurable memory class for pointers to applicaiton functions(e.g. call
 * back function pointers).
 */
    #define DEM_APPL_CODE

#endif /* end of COMPILER_CFG_H */

/*=======[E N D   O F   F I L E]==============================================*/
