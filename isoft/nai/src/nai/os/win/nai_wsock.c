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
/// @file       nai_wsock.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_wsock.h"


#if defined(_WIN32)


#include "nai/os/nai_aio.h"
#include "nai/os/nai_socket.h"
#include "nai/os/nai_system.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"



nai_wsockops_t nai_wsockops;

static GUID ax_guid = WSAID_ACCEPTEX;
static GUID cx_guid = WSAID_CONNECTEX;
static GUID as_guid = WSAID_GETACCEPTEXSOCKADDRS;
static GUID tf_guid = WSAID_TRANSMITFILE;
static GUID tp_guid = WSAID_TRANSMITPACKETS;


nai_int_t nai_wsock_init()
{
    nai_int_t r;
    nai_int_t ec;
    nai_fd_t s = NAI_FD_INVALID;
    DWORD bytes;
    WSADATA wsa = {0};


    r = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (r) {
        nai_errno = ec = nai_errno_from_oserr(r);
        nai_log_emerg(NAI_LOG_CORE, ec, "WSAStartup() failed");
        r = -1;
        goto _end;
    };

    nai_wsockops.inited = 1;

    /* create socket for get functions */
    s = nai_sock_open(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (s == NAI_FD_INVALID) {
        nai_log_alert(NAI_LOG_CORE, nai_errno, "socket() failed");
        r = -1;
        goto _end;
    };

    /* get extension functions */
    if (WSAIoctl((SOCKET)s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
        &ax_guid, sizeof(GUID), &nai_wsockops.AcceptEx, sizeof(void*), 
        &bytes, NULL, NULL) < 0) {
        nai_errno = ec = nai_errno_from_oserr(GetLastError());
        nai_log_warn(NAI_LOG_CORE, 
            ec, "WSAIoctl() get AcceptEx failed");
    };
    if (WSAIoctl((SOCKET)s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
        &cx_guid, sizeof(GUID), &nai_wsockops.ConnectEx, sizeof(void*), 
        &bytes, NULL, NULL) < 0) {
        nai_errno = ec = nai_errno_from_oserr(GetLastError());
        nai_log_warn(NAI_LOG_CORE, 
            ec, "WSAIoctl() get ConnectEx failed");
    };
    if (WSAIoctl((SOCKET)s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
        &as_guid, sizeof(GUID), &nai_wsockops.GetAcceptExSockaddrs, sizeof(void*), 
        &bytes, NULL, NULL) < 0) {
        nai_errno = ec = nai_errno_from_oserr(GetLastError());
        nai_log_warn(NAI_LOG_CORE, 
            ec, "WSAIoctl() get GetAcceptExSockaddrs failed");
    };
    if (WSAIoctl((SOCKET)s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
        &tf_guid, sizeof(GUID), &nai_wsockops.TransmitFile, sizeof(void*), 
        &bytes, NULL, NULL) < 0) {
        nai_errno = ec = nai_errno_from_oserr(GetLastError());
        nai_log_warn(NAI_LOG_CORE, 
            ec, "WSAIoctl() get TransmitFile failed");
    };
    if (WSAIoctl((SOCKET)s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
        &tp_guid, sizeof(GUID), &nai_wsockops.TransmitPackets, sizeof(void*), 
        &bytes, NULL, NULL) < 0) {
        nai_errno = ec = nai_errno_from_oserr(GetLastError());
        nai_log_warn(NAI_LOG_CORE, 
            ec, "WSAIoctl() get TransmitPackets failed");
    };


    if (nai_wsockops.ConnectEx) {
        nai_aiofeat.connect = 1;
    };
    if (nai_wsockops.AcceptEx && nai_wsockops.GetAcceptExSockaddrs) {
        nai_aiofeat.acceptex = 1;
    };
    if (nai_wsockops.TransmitFile) {
        nai_aiofeat.sendfile = 1;
        nai_aiofeat.sendfile_sim = 1;
        nai_aiofeat.sendfilev_sim = 1;
        nai_iofeat.sendfile = 1;
        nai_iofeat.sendfile_sim = 1;
        nai_iofeat.sendfilev_sim = 1;
    };
    if (nai_wsockops.TransmitPackets) {
        nai_aiofeat.sendfilev = 1;
        nai_aiofeat.sendfilev_sim = 1;
        nai_iofeat.sendfilev = 1;
        nai_iofeat.sendfilev_sim = 1;
    };

    r = 0;

_end:
    if (s != NAI_FD_INVALID) {
        ec = nai_errno;
        nai_sock_close(s);
        if (r < 0) {
            nai_errno = ec;
        };
    };
    return r;
};


nai_int_t nai_filearray_to_packets(nai_filearray_t* a, 
    TRANSMIT_PACKETS_ELEMENT* e, nai_int_t ecnt, size_t* psize)
{
    nai_int_t n;
    size_t size = 0;
    nai_filevec_t* v;
    TRANSMIT_PACKETS_ELEMENT* s = e;


    for (n = 0; n < a->count; n ++) {
        v = &a->v[n];

        if (v->size <= 0) {
            continue;
        };
        if (ecnt <= 0) {
            break;
        };
        if (v->fd == NAI_FD_INVALID) {
            e->dwElFlags = TP_ELEMENT_MEMORY;
            e->cLength = (uint32_t)v->size;
            e->pBuffer = v->ptr;
            e ++;
            ecnt --;
        } else {
            e->dwElFlags = TP_ELEMENT_FILE;
            e->cLength = (uint32_t)v->size;
            e->hFile = v->fd;
            e->nFileOffset.QuadPart = v->off;
            e ++;
            ecnt --;
        };
        size += v->size;
    };

    a->v += n;
    a->count -= n;
    psize[0] = size;

    return (nai_int_t)(e-s);
};


nai_int_t nai_filechunk_to_packets(nai_filechunk_t* c, 
    TRANSMIT_PACKETS_ELEMENT* e, nai_int_t ecnt, size_t* psize)
{
    nai_int_t n;
    size_t size = 0;
    nai_bufvec_t* v;
    TRANSMIT_PACKETS_ELEMENT* s = e;


    for (n = 0; n < c->hcnt; n ++) {
        v = &c->hvec[n];

        if (v->len <= 0) {
            continue;
        };
        if (ecnt <= 0) {
            break;
        };

        e->dwElFlags = TP_ELEMENT_MEMORY;
        e->cLength = (uint32_t)v->len;
        e->pBuffer = v->buf;
        e ++;
        ecnt --;
        size += v->len;
    };
    c->hcnt -= n;
    c->hvec += n;

    if (ecnt > 0 && 
        c->size > 0 && 
        c->fd != NAI_FD_INVALID) {

        e->dwElFlags = TP_ELEMENT_FILE;
        e->cLength = (uint32_t)c->size;
        e->hFile = c->fd;
        e->nFileOffset.QuadPart = c->off;
        e ++;
        ecnt --;
        size += c->size;

        c->fd = NAI_FD_INVALID;
        c->size = 0;
    };

    for (n = 0; n < c->tcnt; n ++) {
        v = &c->tvec[n];

        if (v->len <= 0) {
            continue;
        };
        if (ecnt <= 0) {
            break;
        };

        e->dwElFlags = TP_ELEMENT_MEMORY;
        e->cLength = (uint32_t)v->len;
        e->pBuffer = v->buf;
        e ++;
        ecnt --;
        size += v->len;
    };
    c->tcnt -= n;
    c->tvec += n;

    psize[0] = size;

    return (nai_int_t)(e-s);
};


#endif

