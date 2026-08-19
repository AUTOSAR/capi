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

/// ================================================================
///
/// File description:
/// ----------------
/// @file       cpi_const.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_CONST_H
#define __CPI_CONST_H

#ifdef __cplusplus
extern "C"
{
#endif

#define CPI_EVENT_ANY    0x00
#define CPI_METHOD_ANY   0x00
#define CPI_SERVICE_ANY  0xffff
#define CPI_INSTANCE_ANY 0xffff
#define CPI_MAJOR_ANY    0xff
#define CPI_MINOR_ANY    0xffffffff

#define CPI_SHM_PATH_LEN 64

#define CPI_AVAIL_OFFLINED 0
#define CPI_AVAIL_ONLINED  1
#define CPI_AVAIL_CHANGED  2

#ifdef __cplusplus
}
#endif

#endif
