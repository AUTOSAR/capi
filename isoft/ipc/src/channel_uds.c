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
/// @file       channel_uds.c
/// @brief
/// @details
/// @date       2021-11-18
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include <isoft/ipc/ipc_base_C.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "channel.h"
#include "peer_uds.h"

#define IPC_CHANNEL_MAX_IOVEC_SIZE            64
#define IPC_CHANNEL_MAX_PACKET_BUF_SIZE       1U << 12
#define IPC_CHANNEL_MAX_PACKET_EXTRA_BUF_SIZE 1U << 12
typedef struct
{
    uint32_t service;
    uint8_t model;
    uintptr_t session_id;
} ipc_message_header_t;

static int receive_packet(int fd, ipc_packet_t *packet, struct sockaddr_un *client_addr)
{
    struct msghdr msgh;
    struct iovec iov[3];
    struct ucred *ucredp;
    size_t addr_size;

    union
    {
        struct cmsghdr cmh;
        char control[CMSG_SPACE(sizeof(struct ucred))];
    } control_un;

    struct cmsghdr *cmhp;

    addr_size                 = sizeof(struct sockaddr_un);
    control_un.cmh.cmsg_len   = CMSG_LEN(sizeof(struct ucred));
    control_un.cmh.cmsg_level = SOL_SOCKET;
    control_un.cmh.cmsg_type  = SCM_CREDENTIALS;

    msgh.msg_control    = control_un.control;
    msgh.msg_controllen = sizeof(control_un.control);

    msgh.msg_iov    = iov;
    msgh.msg_iovlen = 3;

    // iov[0] for packet header
    ipc_message_header_t header;
    iov[0].iov_base = (void *)&header;
    iov[0].iov_len  = sizeof(header);
    memset(&header, 0x0, sizeof(header));

    // iov[1] for data buf
    ipc_buffer_t *buf;
    buf             = ipc_packet_append_buffer(packet, IPC_CHANNEL_MAX_PACKET_BUF_SIZE);
    iov[1].iov_base = (void *)ipc_buffer_get_ptr(buf);
    iov[1].iov_len  = ipc_buffer_get_size(buf);

    // iov[2] for extra data buf
    size_t extra_buf_size = IPC_CHANNEL_MAX_PACKET_EXTRA_BUF_SIZE;
    uint8_t extra_buf[extra_buf_size];
    iov[2].iov_base = (void *)extra_buf;
    iov[2].iov_len  = extra_buf_size;

    // client addr
    memset(client_addr, 0x0, addr_size);
    msgh.msg_name    = (void *)client_addr;
    msgh.msg_namelen = (socklen_t)addr_size;

    ssize_t r = recvmsg(fd, &msgh, 0);
    if (r == -1) {
        return -1;
    }

    // client pid
    cmhp        = CMSG_FIRSTHDR(&msgh);
    ucredp      = (struct ucred *)CMSG_DATA(cmhp);
    packet->pid = ucredp->pid;

    packet->service    = header.service;
    packet->model      = header.model;
    packet->session_id = header.session_id;
    ipc_packet_reduce_received_kind(packet);

    // assert(r == iov[0].iov_len + buf_len + extra_buf_len)
    size_t buf_len       = (size_t)r - iov[0].iov_len;
    size_t extra_buf_len = 0;
    if (buf_len > 0) {
        if (buf_len > IPC_CHANNEL_MAX_PACKET_BUF_SIZE) {
            extra_buf_len = buf_len - (IPC_CHANNEL_MAX_PACKET_BUF_SIZE);
            buf_len       = IPC_CHANNEL_MAX_PACKET_BUF_SIZE;
        }
    } else {
        buf_len = 0;
    }
    ipc_buffer_set_len(buf, buf_len);

    if (extra_buf_len) {
        buf = ipc_packet_append_buffer(packet, extra_buf_len);
        memcpy((void *)ipc_buffer_get_ptr(buf), extra_buf, extra_buf_len);
        ipc_buffer_set_len(buf, extra_buf_len);
    }

    return 0;
}

static int send_packet(int fd, ipc_packet_t *packet, struct sockaddr *server_addr)
{
    struct msghdr msgh;
    struct iovec iov[IPC_CHANNEL_MAX_IOVEC_SIZE];
    socklen_t server_addr_size;

    // iov[0] for packet header
    ipc_message_header_t header = {
        .service    = packet->service,
        .model      = packet->model,
        .session_id = packet->session_id,
    };

    server_addr_size = sizeof(struct sockaddr_un);

    iov[0].iov_base = (void *)&header;
    iov[0].iov_len  = sizeof(header);

    // iov[1..n] for data buf
    int i = 1;
    ipc_buffer_t *buf;
    buf = ipc_packet_get_buffer(packet);
    while (i < IPC_CHANNEL_MAX_IOVEC_SIZE) {
        if (buf == NULL) {
            break;
        }

        iov[i].iov_base = (void *)ipc_buffer_get_ptr(buf);
        iov[i].iov_len  = ipc_buffer_get_len(buf);

        buf = ipc_buffer_get_next(packet, buf);
        i++;
    }

    msgh.msg_iov    = iov;
    msgh.msg_iovlen = (size_t)i;

    msgh.msg_control    = NULL;
    msgh.msg_controllen = 0;

    msgh.msg_name    = server_addr;
    msgh.msg_namelen = server_addr_size;

    return (int)sendmsg(fd, &msgh, 0);
}

struct ipc_channel_s
{
    int fd;
};

static ipc_channel_t default_uds_channel = {.fd = -1};

static void assert_default_channel(ipc_channel_t *channel) { assert(&default_uds_channel == channel); }

int ipc_channel_get_fd(ipc_channel_t *channel)
{
    if (channel == NULL) {
        goto L_RETURN;
    }

    assert_default_channel(channel);

    return channel->fd;

L_RETURN:
    return -1;
}

int ipc_channel_write_packet(ipc_channel_t *channel, ipc_packet_t *packet, ipc_peer_t *peer)
{
    int status = -1;
    struct sockaddr_un *addr;

    if (peer == NULL) {
        goto L_RETURN;
    }

    if (channel == NULL) {
        goto L_RETURN;
    }

    assert_default_channel(channel);

    addr = ipc_peer_uds_get_address(peer);

    while (send_packet(channel->fd, packet, (struct sockaddr *)addr) == -1) {
        if (errno == EINTR)
            continue;
        goto L_RETURN;
    }
    status = 0;

L_RETURN:
    return status;
}

int ipc_channel_read_packet(ipc_channel_t *channel, ipc_packet_t **packet, ipc_peer_t **peer)
{
    int status = -1;
    struct sockaddr_un addr;

    if (peer == NULL) {
        goto L_RETURN;
    }
    if (channel == NULL) {
        goto L_RETURN;
    }

    assert_default_channel(channel);

    // create a buffer
    *packet = ipc_packet_create(0);
    if (*packet == NULL) {
        goto L_RETURN;
    }

    while (receive_packet(channel->fd, *packet, &addr) == -1) {
        if (errno == EINTR)
            continue;
        goto L_RETURN;
    }

    status = 0;

    //hold a reference
    *peer = (ipc_peer_t *)ipc_peer_uds_find_by_address(&addr);

L_RETURN:
    if (status != 0) {
        ipc_packet_release(*packet);
    }

    return status;
}

ipc_channel_t *ipc_channel_create(void)
{
    int fd;
    ipc_channel_t *channel = NULL;

    if (default_uds_channel.fd == -1) {
        fd = ipc_base_socket_get();
        if (fd <= 0) {
            goto L_RETURN;
        }
        default_uds_channel.fd = fd;
    }

    channel = &(default_uds_channel);

L_RETURN:
    return channel;
}

void ipc_channel_release(ipc_channel_t *channel)
{
    if (channel == NULL) {
        goto L_RETURN;
    }

    assert_default_channel(channel);

L_RETURN:
    return;
}
