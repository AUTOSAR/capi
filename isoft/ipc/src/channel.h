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
/// @file       channel.h
/// @brief
/// @details
/// @date       2021-11-18
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef _CHANNEL_H_IPC
#define _CHANNEL_H_IPC

#include "packet.h"
#include "peer.h"

typedef struct ipc_channel_s ipc_channel_t;

ipc_channel_t *ipc_channel_create(void);
void ipc_channel_release(ipc_channel_t *channel);

int ipc_channel_get_fd(ipc_channel_t *channel);
int ipc_channel_read_packet(ipc_channel_t *channel, ipc_packet_t **packet, ipc_peer_t **peer);
int ipc_channel_write_packet(ipc_channel_t *channel, ipc_packet_t *packet, ipc_peer_t *peer);

#endif
