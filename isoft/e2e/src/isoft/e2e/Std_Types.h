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
**  FILENAME  : Std_Types.h                                                   **
**                                                                            **
**                                                                            **
**  PLATFORM  : Infineon AURIX                                                **
**                                                                            **
**  AUTHOR    :                                                               **
**                                                                            **
**  VENDOR    :                                                               **
**                                                                            **
**  DESCRIPTION: Provision of Standard Types                                  **
**                                                                            **
**                                                                            **
**   SPECIFICATION(S) :   AUTOSAR classic Platform 4.2.2                      **
**                                                                            **
**                                                                            **
*******************************************************************************/
#ifndef STD_TYPES_H
#define STD_TYPES_H

#include "Compiler.h"
#include "Platform_Types.h"

typedef uint8 Std_ReturnType;
typedef uint8 Std_MessageTypeType;
typedef uint8 Std_MessageResultType;

#ifndef STATUSTYPEDEFINED
    #define STATUSTYPEDEFINED
    #define E_OK 0x00u
typedef unsigned char StatusType; /* OSEK compliance */
#endif
#define E_NOT_OK 0x01u

typedef struct
{
    uint16 vendorID;
    uint16 moduleID;
    uint8 sw_major_version;
    uint8 sw_minor_version;
    uint8 sw_patch_version;
    /*added by JiYoufeng*/
    uint8 instanceID;
} Std_VersionInfoType;

#define STD_HIGH 0x01u /* Physical state 5V or 3.3V */

#define STD_LOW 0x00u /* Physical state 0V */

#define STD_ACTIVE 0x01u /* Logical state active */

#define STD_IDLE 0x00u /* Logical state idle */

#define STD_ON 0x01u

#define STD_OFF 0x00u

/*added by JiYoufeng*/
#define STD_TYPES_AR_RELEASE_MAJOR_VERSION 4
/*added by JiYoufeng*/
#define STD_TYPES_AR_RELEASE_MINOR_VERSION 2

#define E_SAFETY_HARD_RUNTIMEERROR 0xFF
#define E_SAFETY_SOFT_RUNTIMEERROR 0x77

#define STD_TYPES_AR_MAJOR_VERSION 4
#define STD_TYPES_AR_MINOR_VERSION 2

/*Message Type*/
#define STD_MESSAGETYPE_REQUEST  0
#define STD_MESSAGETYPE_RESPONSE 1

/*Message Result*/
#define STD_MESSAGERESULT_OK    0
#define STD_MESSAGERESULT_ERROR 1

#endif /* STD_TYPES_H */
