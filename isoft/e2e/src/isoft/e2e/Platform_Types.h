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
**                                                                           **
**  $FILENAME   : Platform_Types.h $                                         **
**                                                                           **
**                                                                           **
**                                                                           **
**  AUTHOR      :                                                            **
**                                                                           **
**  VENDOR      :                                                            **
**                                                                           **
**  DESCRIPTION : File holding the definition of all the Platform types      **
**                as defined by AUTOSAR                                      **
**                                                                           **
**  SPECIFICATION(S) :   AUTOSAR classic Platform 4.2.2                      **
**                                                                           **
******************************************************************************/
#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

/*
  CPU register type width
*/
#define CPU_TYPE_8  8
#define CPU_TYPE_16 16
#define CPU_TYPE_32 32

/*
  Bit order definition
*/
#define MSB_FIRST 0 /* Big endian bit ordering     */
#define LSB_FIRST 1 /* Little endian bit ordering  */

/*
  Byte order definition
*/
#define HIGH_BYTE_FIRST 0 /* Big endian byte ordering    */
#define LOW_BYTE_FIRST  1 /* Little endian byte ordering */

/*
  Platform type and endianess definitions for TC275
*/
#define CPU_TYPE CPU_TYPE_32

#define CPU_BIT_ORDER LSB_FIRST

#define CPU_BYTE_ORDER LOW_BYTE_FIRST

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/
#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

/* for use with TRUE/FALSE        */
typedef unsigned char boolean;

typedef unsigned char uint8;   /*           0 .. 255             */
typedef unsigned short uint16; /*           0 .. 65535           */
typedef unsigned int uint32;   /*           0 .. 4294967295      */
typedef unsigned long long uint64;
typedef signed char sint8;   /*        -128 .. +127            */
typedef signed short sint16; /*      -32768 .. +32767          */
typedef signed int sint32;   /* -2147483648 .. +2147483647     */
typedef signed long long sint64;

typedef unsigned long uint8_least;  /* At least 8 bit                 */
typedef unsigned long uint16_least; /* At least 16 bit                */
typedef unsigned long uint32_least; /* At least 32 bit                */

typedef signed long sint8_least;  /* At least 7 bit + 1 bit sign    */
typedef signed long sint16_least; /* At least 15 bit + 1 bit sign   */
typedef signed long sint32_least; /* At least 31 bit + 1 bit sign   */

typedef float float32;
typedef double float64;

#endif /* PLATFORM_TYPES_H */
