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
/// @file       peer.h
/// @brief
/// @details
/// @date       2022-11-03
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef _PEER_H_IPC
#define _PEER_H_IPC

typedef struct ipc_peer_s ipc_peer_t;

void init_ipc_peer_module(void);
void deinit_ipc_peer_module(void);

/*
 * @brief Find a peer create it if not exsits.
 * @param name process name
 * @return 0 on success, -1 on failure with errno set
 */
ipc_peer_t* ipc_peer_find(const char* name);
ipc_peer_t* ipc_peer_ref(ipc_peer_t* peer);
void ipc_peer_release(ipc_peer_t* peer);

#endif
