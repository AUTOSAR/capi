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
 *  @file       < Det.h >
 *  @brief      <   >
 *
 *  <Compiler:      MCU:  >
 *
 *  @author     <   >
 *  @date       <  >
 */
/*============================================================================*/

#ifndef DET_H
#define DET_H

/****************************** references *********************************/
#include "Std_Types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define DET_H_AR_MAJOR_VERSION 2
#define DET_H_AR_MINOR_VERSION 2
    /****************************** declarations *********************************/
    extern void Det_ReportError(uint16 Module_Id, uint8 Instance_Id, uint8 ApiId, uint8 ErrorId);
    extern void Det_Clear(void);

    extern uint16 Det_ModuleId;
    extern uint8 Det_InstanceId;
    extern uint8 Det_ApiId;
    extern uint8 Det_ErrorId;
    /****************************** definitions *********************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DET_H */
