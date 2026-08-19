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
/// @file       event.c
/// @brief
/// @details
/// @date       2022-11-30
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "event.h"

#include <fcntl.h>
#include <nai/os/nai_file.h>

#include "channel.h"
#include "manager.h"
#include "peer.h"

static ipc_spinlock_t ipc_event_lock = IPC_SPINLOCK_INITIALIZER;

static struct
{
    nai_evnode_t event_node;
    nai_evloop_t* event_loop;
    ipc_event_manager_t event_manager;
    ipc_channel_t* channel;
} rt;

static int ipc_event_on_read(int fd)
{
    int status            = -1;
    ipc_packet_t* message = NULL;
    ipc_peer_t* peer      = NULL;

    ipc_channel_t* channel = rt.channel;
    if (channel == NULL)
        goto L_RETURN;

    if (fd != ipc_channel_get_fd(rt.channel)) {
        goto L_RETURN;
    }

    status = ipc_channel_read_packet(channel, &message, &peer);
    if (status == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
            rt.event_manager.on_error(rt.event_manager.context, NULL);
        goto L_RETURN;
    }

    if (peer == NULL) {
        if (errno != EINVAL)
            rt.event_manager.on_error(rt.event_manager.context, NULL);
        goto L_RETURN;
    }

    // read_packet hold a reference
    // just take the ownership
    message->peer = peer;

    rt.event_manager.on_received(rt.event_manager.context, message);
    status = 0;

L_RETURN:
    if (message != NULL)
        ipc_packet_release(message);
    return status;
}

static int ipc_event_on_write(int fd)
{
    int status = -1;

    ipc_packet_t* message;
    int r;

    ipc_channel_t* channel = rt.channel;

    if (fd != ipc_channel_get_fd(rt.channel)) {
        goto L_RETURN;
    }

    message = rt.event_manager.peek_message(rt.event_manager.context);
    if (message == NULL) {
        nai_evnode_set_event(&rt.event_node, NAI_EV_DEL, NAI_EV_WRITE);
        goto L_RETURN;
    }

    r = ipc_channel_write_packet(channel, message, message->peer);
    if (r == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            nai_evnode_set_event(&rt.event_node, NAI_EV_ADD, NAI_EV_WRITE);
            goto L_RETURN;
        } else {
            rt.event_manager.on_error(rt.event_manager.context, message);
            status = 0;
            goto L_RELEASE_MESSAGE;
        }
    }
    rt.event_manager.on_sent(rt.event_manager.context, message);
    status = 0;

L_RELEASE_MESSAGE:
    if (message != NULL)
        ipc_packet_release(message);
L_RETURN:

    return status;
}

int ipc_event_notify(void) { return nai_evnode_post(&rt.event_node, 0); }

int ipc_event_close(void) { return nai_evnode_post(&rt.event_node, 3); }

int ipc_event_cleanup(void)
{
    ipc_spinlock_lock(&ipc_event_lock);
    int status = nai_evnode_close(&rt.event_node);
    if (status == 0)
        rt.event_loop = NULL;
    ipc_spinlock_unlock(&ipc_event_lock);

    ipc_channel_release(rt.channel);
    rt.channel = NULL;

    return status;
}

static int ipc_event_on_close(void)
{
    int status = -1;

    ipc_evnode_dump(rt.event_loop, &rt.event_node, "ipc_channel_on_close");

    status = ipc_module_cleanup_delayed();

    return status;
}

// libnai event callback function
static int ipc_event_dispatch(nai_evnode_t* node, int event)
{
    if (event & NAI_EV_READ)
        while (ipc_event_on_read(node->fd) == 0)
            ;
    if (event & NAI_EV_WRITE)
        while (ipc_event_on_write(node->fd) == 0)
            ;
    if (event & NAI_EV_NOTIFY) {
        if (NAI_EV_NOTIFY_CODE((uint32_t)event) == 3) {
            ipc_event_on_close();
        } else {
            while (ipc_event_on_write(node->fd) == 0)
                ;
        }
    }
    return 0;
}

int ipc_event_startup(nai_evloop_t* event_loop, ipc_event_manager_t* manager)
{
    int status = -1;
    int fd, flag, events = NAI_EV_READ | NAI_EV_NOTIFY;

    rt.channel = ipc_channel_create();
    if (rt.channel == NULL)
        goto L_RETURN;

    fd = ipc_channel_get_fd(rt.channel);
    ipc_init_dump(0, fd, "ipc_channel_get_fd");
    if (fd <= 0)
        goto L_RETURN;

    flag = fcntl(fd, F_GETFL, 0);
    ipc_init_dump(flag, fd, "fcntl F_GETFL");
    if (flag == -1)
        goto L_RETURN;
    if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) == -1)
        goto L_RETURN;
    ipc_init_dump(0, fd, "fcntl F_SETFL");

    ipc_spinlock_lock(&ipc_event_lock);
    // initialize event_node
    if (nai_evnode_init(&rt.event_node) != 0)
        goto L_UNLOCK;
    if (nai_evnode_set_fd(&rt.event_node, fd, NAI_FD_TYPE_SOCK) != 0)
        goto L_UNLOCK;
    if (nai_evnode_set_event(&rt.event_node, NAI_EV_ADD, events) != 0)
        goto L_UNLOCK;
    if (nai_evnode_set_cb(&rt.event_node, ipc_event_dispatch) != 0)
        goto L_UNLOCK;
    if (nai_evnode_open(&rt.event_node, event_loop) == -1)
        goto L_UNLOCK;

    rt.event_loop    = event_loop;
    rt.event_manager = *manager;
    init_ipc_peer_module();

    status = 0;

L_UNLOCK:
    ipc_spinlock_unlock(&ipc_event_lock);
L_RETURN:
    return status;
}
