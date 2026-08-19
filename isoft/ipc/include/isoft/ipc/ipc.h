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
/// @file       ipc.h
/// @brief
/// @details
/// @date       2021-11-18
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_MODULE_PUBLIC_HEADER
#define IPC_MODULE_PUBLIC_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <nai/io/nai_event.h>

#define IPC_ADDRESS_LENGTH_MAX (108 - 1 + 5)

    /**
   IPC module initialization function, needs to be called as the first IPC function.
 */
    extern int ipc_init_nai_unix_socket(nai_evloop_t* event_loop);

    /**
   IPC module release function.
 */
    extern int ipc_deinit_nai_unix_socket(void);

#include <isoft/ipc/client.h>
#include <isoft/ipc/packet.h>
#include <isoft/ipc/server.h>

#ifdef __cplusplus
}
#endif

#endif
