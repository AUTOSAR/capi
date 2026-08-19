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
/// @file       nai_iocp.c
/// @brief      
/// @details
/// @date       2020-12-07
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/nai_config.h"


#if defined(_WIN32)


#include "nai_evloop.h"
#include "nai_evbase.h"
#include "nai/runtime/nai_rbtree.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/os/nai_aio.h"
#include "nai/os/win/nai_windows.h"
#include "nai/os/win/nai_wsock.h"
#include <ioapiset.h>
#include <handleapi.h>


typedef struct nai_wpoll_s {

    /* wpoll */
    HANDLE port;
    nai_rbtree_t smap;
    nai_list_t poll;
    nai_list_t update;
    nai_fixedpool_t pool;
    nai_int_t next;
    nai_int_t notsup;

} nai_wpoll_t;


static nai_int_t nai_wpoll_init(nai_wpoll_t* p);
static nai_int_t nai_wpoll_open(nai_wpoll_t* p, HANDLE port);
static nai_int_t nai_wpoll_close(nai_wpoll_t* p);
static nai_int_t nai_wpoll_set(
    nai_wpoll_t* p, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_wpoll_del(nai_wpoll_t* p, nai_evnode_t* h);
static nai_int_t nai_wpoll_update(
    nai_wpoll_t* p, nai_evloop_ent_t** list[2]);
static nai_int_t nai_wpoll_handle(
    nai_wpoll_t* p, nai_evloop_ent_t** list[2], OVERLAPPED* op);



//////////////////////////////////////////////////////////////////////////////
// iocp


#define NAI_IOCP_TYPE_AIO       0
#define NAI_IOCP_TYPE_POLL      1


#define NAI_IOCP_KEY_FD         0
#define NAI_IOCP_KEY_SIG        1
#define NAI_IOCP_KEY_POLL       2


typedef struct nai_iocp_s {

    HANDLE port;

    nai_wpoll_t poll;
    nai_evloop_ent_t* list[2];
    nai_int_t type;
    nai_int_t signaled;

    nai_int_t nops;
    OVERLAPPED_ENTRY ea[64];

} nai_iocp_t;


static void* nai_iocp_open(nai_int_t flags);
static void* nai_iocp_poll_open(nai_int_t flags);
static nai_int_t nai_iocp_add(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_iocp_del(void* e, nai_evnode_t* h);
static nai_int_t nai_iocp_set(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_iocp_wait(void* e, uint32_t msec);
static nai_int_t nai_iocp_fetch(void* e, nai_evloop_ent_t* list[2]);
static nai_int_t nai_iocp_submit(void* e);
static nai_int_t nai_iocp_signal(void* e);
static nai_int_t nai_iocp_close(void* e);
static nai_int_t nai_iocp_rearm(void* e);
static nai_fd_t nai_iocp_no_fd(void* e);
static nai_aio_port_t* nai_iocp_no_port(void* e);


#define NAI_IMPL_FEAT_POLL      NAI_EV_FEAT_LEVEL       \
    | NAI_EV_FEAT_SOCK                                  \


#define NAI_IMPL_FEAT_ASYNC     NAI_EV_FEAT_ASYNC       \
    | NAI_EV_FEAT_FILE | NAI_EV_FEAT_SOCK               \
    | NAI_EV_FEAT_PIPE | NAI_EV_FEAT_DEVC               \


nai_evbase_ops_t nai_iocp = {
    "iocp", 
    NAI_IMPL_FEAT_ASYNC, 
    NAI_IMPL_FEAT_POLL, 
    nai_iocp_open, 
    nai_iocp_add, 
    nai_iocp_del, 
    nai_iocp_set, 
    nai_iocp_wait, 
    nai_iocp_fetch, 
    nai_iocp_submit, 
    nai_iocp_signal, 
    nai_iocp_close, 
    nai_iocp_rearm, 
    nai_iocp_no_fd, 
    nai_iocp_no_port
};

nai_evbase_ops_t nai_wpoll = {
    "wpoll", 
    NAI_IMPL_FEAT_POLL, 
    NAI_IMPL_FEAT_ASYNC, 
    nai_iocp_poll_open, 
    nai_iocp_add, 
    nai_iocp_del, 
    nai_iocp_set, 
    nai_iocp_wait, 
    nai_iocp_fetch, 
    nai_iocp_submit, 
    nai_iocp_signal, 
    nai_iocp_close, 
    nai_iocp_rearm, 
    nai_iocp_no_fd, 
    nai_iocp_no_port
};



static nai_int_t nai_iocp_init(nai_iocp_t* s, nai_int_t type)
{
    nai_int_t r;
    nai_int_t ec;


    /* init */
    nai_wpoll_init(&s->poll);
    s->signaled = 0;
    s->type = type;
    s->nops = 0;
    s->list[0] = 0;
    s->list[1] = 0;

    /* create completion port */
    s->port = CreateIoCompletionPort(NAI_FD_INVALID, 0, 0, 1);
    if (s->port == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    /* open wpoll */
    r = nai_wpoll_open(&s->poll, s->port);
    if (r < 0) {
        ec = nai_errno;
        if (type == NAI_IOCP_TYPE_POLL) {
            goto _end;
        };
        if (ec != ENOTSUP) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_iocp_term(nai_iocp_t* s)
{
    nai_int_t r;


    if (s->port != 0) {
        CloseHandle(s->port);
    };

    nai_wpoll_close(&s->poll);
    r = 0;

    return r;
};


static void* nai_iocp_open_impl(nai_int_t flags, nai_int_t type)
{
    nai_int_t r;
    nai_int_t ec;
    nai_iocp_t* s;


    (void)flags;

    s = nai_malloc(sizeof(*s));
    if (s == 0) {
        goto _end;
    };

    r = nai_iocp_init(s, type);
    if (r < 0) {
        ec = nai_errno;
        nai_iocp_close(s);
        nai_errno = ec;
        s = 0;
    };

_end:
    return s;
};


static nai_fd_t nai_iocp_open(nai_int_t flags)
{
    return nai_iocp_open_impl(flags, NAI_IOCP_TYPE_AIO);
};


static nai_fd_t nai_iocp_poll_open(nai_int_t flags)
{
    return nai_iocp_open_impl(flags, NAI_IOCP_TYPE_POLL);
};


static nai_int_t nai_iocp_close(void* e)
{
    nai_int_t r;
    nai_iocp_t* s = (nai_iocp_t*)e;


    nai_iocp_term(s);
    nai_free(s);
    r = 0;

    return r;
};


static nai_int_t nai_iocp_rearm(void* e)
{
    nai_int_t r;
    nai_iocp_t* s = (nai_iocp_t*)e;
    nai_iocp_t t[2];


    /* backup */
    nai_memcpy(&t[0], s, sizeof(*s));

    /* init */
    r = nai_iocp_init(s, s->type);
    if (r < 0) {
        /* restore */
        nai_memcpy(s, &t[0], sizeof(*s));
    } else {
        /* cleanup */
        nai_memcpy(&t[1], s, sizeof(*s));
        nai_memcpy(s, &t[0], sizeof(*s));
        nai_iocp_term(s);
        nai_memcpy(s, &t[1], sizeof(*s));
    };

    return r;
};


static nai_fd_t nai_iocp_no_fd(void* e)
{
    (void)e;
    return NAI_FD_INVALID;
};


static nai_aio_port_t* nai_iocp_no_port(void* e)
{
    (void)e;
    return 0;
};


static nai_int_t nai_iocp_signal(void* e)
{
    nai_int_t r;
    nai_int_t ec;
    nai_iocp_t* s = (nai_iocp_t*)e;


    if (s->signaled) {
        r = 0;
    } else {
        s->signaled = 1;
        if (PostQueuedCompletionStatus(s->port, 0, NAI_IOCP_KEY_SIG, 0)) {
            r = 0;
        } else {
            ec = nai_errno_from_oserr(GetLastError());
            nai_log_alert(NAI_LOG_CORE, 
                ec, "PostQueuedCompletionStatus failed");
            nai_errno = ec;
            s->signaled = 0;
            r = -1;
        };
    };

    return r;
};


static nai_int_t nai_iocp_add(void* e, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_iocp_t* s = (nai_iocp_t*)e;
    nai_evloop_ent_t* ent;


    ent = h->ent;
    if (ent == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (ent->key != -1) {
        nai_errno = EEXIST;
        r = -1;
        goto _end;
    };

    h->st.seted &= ~(NAI_EV_IOE|NAI_EV_ASYNC);
    r = nai_iocp_set(e, h, events);

_end:
    return r;
};


static nai_int_t nai_iocp_set(void* e, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_iocp_t* s = (nai_iocp_t*)e;
    nai_evloop_ent_t* ent;


    if ((events | h->st.seted) & NAI_EV_IOE) {
        r = nai_wpoll_set(&s->poll, h, events);
        if (r < 0) {
            goto _end;
        };
    };

    if (events & NAI_EV_ASYNC && !(h->st.seted & NAI_EV_ASYNC)) {
        if (CreateIoCompletionPort(h->fd, s->port, NAI_IOCP_KEY_FD, 0) == 0) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };

        h->st.seted |= NAI_EV_ASYNC;
    };

    ent = h->ent;
    if (ent->key == -1) {
        ent->key = 0;
    };
    r = 0;

_end:
    return r;
};


static nai_int_t nai_iocp_del(void* e, nai_evnode_t* h)
{
    nai_int_t r;
    nai_iocp_t* s = (nai_iocp_t*)e;
    nai_evloop_ent_t* ent;


    r = nai_wpoll_del(&s->poll, h);
    if (r >= 0) {
        h->st.seted &= ~(NAI_EV_IOE|NAI_EV_ASYNC);
        ent = h->ent;
        ent->key = -1;
    };

    return r;
};


static nai_int_t nai_iocp_wait(void* e, uint32_t msec)
{
    nai_int_t r;
    intptr_t key;
    nai_iocp_t* s = (nai_iocp_t*)e;
    nai_evloop_ent_t** lptr[2];

    DWORD ec;
    ULONG count;
    DWORD bytes;
    OVERLAPPED* op;


    /* init list pointers */
    lptr[0] = &s->list[0];
    lptr[1] = &s->list[1];


    /* update poll opeartions */
    r = nai_wpoll_update(&s->poll, lptr);
    if (r > 0) {
        r = 0;
        goto _end;
    };

    if (nai_wapi.GetQueuedCompletionStatusEx) {

        r = nai_wapi.GetQueuedCompletionStatusEx(
            s->port, s->ea, nai_countof(s->ea), &count, msec, 0);

        if (r == 0) {
            ec = GetLastError();
            if (ec == WAIT_TIMEOUT) {
                goto _end;
            };
            if (count <= 0) {
                nai_errno = nai_errno_from_oserr(ec);
                nai_log_alert(NAI_LOG_CORE, 
                    nai_errno, "GetQueuedCompletionStatusEx failed");
                r = -1;
                goto _end;
            };
        };

        s->nops = count;

    } else {

        op = 0;
        r = GetQueuedCompletionStatus(
            s->port, &bytes, &key, (OVERLAPPED**)&op, msec);

        if (r == 0) {
            ec = GetLastError();
            if (ec == WAIT_TIMEOUT) {
                goto _end;
            };
            if (op == 0) {
                nai_errno = nai_errno_from_oserr(ec);
                nai_log_alert(NAI_LOG_CORE, 
                    nai_errno, "GetQueuedCompletionStatus failed");
                r = -1;
                goto _end;
            };

            bytes = 0;
        } else {
            switch (key) {
            case NAI_IOCP_KEY_SIG:
                s->signaled = 0;
                r = 0;
                goto _end;
            default:
                break;
            };

            if (op == 0) {
                nai_errno = NAI_EFAILED;
                nai_log_error(NAI_LOG_CORE, nai_errno, 
                    "GetQueuedCompletionStatus return a null overlap");
                r = -1;
                goto _end;
            };
        };

        s->ea[0].lpCompletionKey = key;
        s->ea[0].lpOverlapped = op;
        s->ea[0].dwNumberOfBytesTransferred = bytes;
        s->nops = 1;
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_iocp_fetch(void* e, nai_evloop_ent_t* list[2])
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t n;
    nai_int_t count;
    intptr_t key;
    nai_aio_t* op;
    nai_iocp_t* s = (nai_iocp_t*)e;
    nai_evloop_ent_t** lptr[2];

    DWORD bytes;
    OVERLAPPED_ENTRY* ea;


    if (s->nops <= 0) {
        /* fetch event lists */
        list[0] = s->list[0];
        list[1] = s->list[1];
        s->list[0] = 0;
        s->list[1] = 0;
        r = 0;
        goto _end;
    };

    assert(s->list[0] == 0);
    assert(s->list[1] == 0);


    /* init list pointers */
    lptr[0] = &list[0];
    lptr[1] = &list[1];


    /* handle async opeartions */
    ea = s->ea;
    count = s->nops;
    s->nops = 0;
    for (n = 0; n < count; n ++) {
        key = ea[n].lpCompletionKey;
        switch (key) {
        case NAI_IOCP_KEY_SIG:
            s->signaled = 0;
            continue;
        default:
            break;
        };

        op = (nai_aio_t*)ea[n].lpOverlapped;
        if (op == 0) {
            nai_errno = NAI_EFAILED;
            nai_log_error(NAI_LOG_CORE, nai_errno, 
                "GetQueuedCompletionStatusEx return a null overlap");
            continue;
        };

        switch (key) {
        case NAI_IOCP_KEY_POLL:
            nai_wpoll_handle(&s->poll, lptr, &op->cb);
            continue;

        default:
            if (!op->handle) {
                continue;
            };

            bytes = ea[n].dwNumberOfBytesTransferred;
            if (!bytes) {
                bytes = (DWORD)nai_aio_result(op, 0);
                if (bytes == -1) {
                    bytes = 0;
                    ec = nai_errno;
                    break;
                };
            } else if (op->postpro) {
                r = nai_aio_postpro(op);
                if (r < 0) {
                    bytes = 0;
                    ec = nai_errno;
                    break;
                };
            } else {
                /* nothing */
                ;
            };

            ec = 0;
            break;
        };

        op->handle(op, ec, bytes);
    };


    lptr[0][0] = 0;
    lptr[1][0] = 0;
    r = 0;

_end:
    return r;
};


static nai_int_t nai_iocp_submit(void* e)
{
    (void)e;

    return 0;
};



//////////////////////////////////////////////////////////////////////////////
// wpoll


#ifndef FILE_OPEN
#define FILE_OPEN 0x00000001UL
#endif

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS              ((NTSTATUS) 0x00000000L)
#endif

#ifndef STATUS_PENDING
#define STATUS_PENDING              ((NTSTATUS) 0x00000103L)
#endif

#ifndef STATUS_CANCELLED
#define STATUS_CANCELLED            ((NTSTATUS) 0xC0000120L)
#endif

#ifndef STATUS_NOT_FOUND
#define STATUS_NOT_FOUND            ((NTSTATUS) 0xC0000225L)
#endif


#define IOCTL_AFD_POLL              0x00012024


#define AFD_POLL_RECEIVE            0x0001
#define AFD_POLL_RECEIVE_EXPEDITED  0x0002
#define AFD_POLL_SEND               0x0004
#define AFD_POLL_DISCONNECT         0x0008
#define AFD_POLL_ABORT              0x0010
#define AFD_POLL_LOCAL_CLOSE        0x0020
#define AFD_POLL_ACCEPT             0x0080
#define AFD_POLL_CONNECT_FAIL       0x0100


typedef struct _AFD_POLL_HANDLE_INFO {
    HANDLE Handle;
    ULONG Events;
    NTSTATUS Status;
} AFD_POLL_HANDLE_INFO, *PAFD_POLL_HANDLE_INFO;

typedef struct _AFD_POLL_INFO {
    LARGE_INTEGER Timeout;
    ULONG NumberOfHandles;
    ULONG Exclusive;
    AFD_POLL_HANDLE_INFO Handles[1];
} AFD_POLL_INFO, *PAFD_POLL_INFO;


static UNICODE_STRING afd_device_name =
    RTL_CONSTANT_STRING(L"\\Device\\Afd\\Wpoll");

static OBJECT_ATTRIBUTES afd_device_attributes =
    RTL_CONSTANT_OBJECT_ATTRIBUTES(&afd_device_name, 0);


static HANDLE nai_afd_open(HANDLE port)
{
    DWORD ec;
    HANDLE afd;
    NTSTATUS status;
    IO_STATUS_BLOCK iosb;

    /* By opening \Device\Afd without specifying any extended attributes, we'll
     * get a handle that lets us talk to the AFD driver, but that doesn't have an
     * associated endpoint (so it's not a socket). */
    status = nai_wapi.NtCreateFile(&afd,
        SYNCHRONIZE, &afd_device_attributes, &iosb,
        0, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN,
        0, 0, 0);

    if (status != STATUS_SUCCESS) {
        ec = nai_wapi.RtlNtStatusToDosError(status);
        nai_errno = nai_errno_from_oserr(ec);
        afd = 0;
        goto _end;
    };

    if (!CreateIoCompletionPort(afd, port, NAI_IOCP_KEY_POLL, 0)) {
        goto _fail;
    };

    if (nai_wapi.SetFileCompletionNotificationModes) {
        if (!nai_wapi.SetFileCompletionNotificationModes(
            afd, FILE_SKIP_SET_EVENT_ON_HANDLE)) {
            goto _fail;
        };
    };

_end:
    return afd;

_fail:
    ec = GetLastError();
    CloseHandle(afd);
    nai_errno = nai_errno_from_oserr(ec);
    afd = 0;
    goto _end;
};


static nai_int_t nai_afd_poll(
    HANDLE afd, AFD_POLL_INFO* info, IO_STATUS_BLOCK* sb)
{
    nai_int_t r;
    DWORD ec;
    NTSTATUS status;

    /* Blocking operation is not supported. */
    assert(sb != 0);

    sb->Status = STATUS_PENDING;
    status = nai_wapi.NtDeviceIoControlFile(
        afd, 0, 0, sb, sb, IOCTL_AFD_POLL, 
        info, sizeof(*info),
        info, sizeof(*info));

    switch (status) {
    case STATUS_SUCCESS:
        r = 0;
        break;
    case STATUS_PENDING:
        r = 0;
        break;
    default:
        ec = nai_wapi.RtlNtStatusToDosError(status);
        nai_errno = nai_errno_from_oserr(ec);
        r = -1;
        break;
    };

    return r;
}

static nai_int_t nai_afd_cancel(HANDLE afd, IO_STATUS_BLOCK* sb)
{
    nai_int_t r;
    DWORD ec;
    NTSTATUS status;
    IO_STATUS_BLOCK cb;


    assert(sb != 0);

    /* If the poll operation has already completed or has been 
     * cancelled earlier, there's nothing left for us to do.
     */
    if (sb->Status != STATUS_PENDING) {
        r = 0;
        goto _end;
    };

    status = nai_wapi.NtCancelIoFileEx(afd, sb, &cb);

    /* NtCancelIoFileEx() may return STATUS_NOT_FOUND if the operation 
     * completed just before calling NtCancelIoFileEx(). 
     * This is not an error.
     */
    if (status == STATUS_SUCCESS || status == STATUS_NOT_FOUND) {
        r = 0;
        goto  _end;
    };

    ec = nai_wapi.RtlNtStatusToDosError(status);
    nai_errno = nai_errno_from_oserr(ec);
    r = -1;

_end:
    return r;
};


static SOCKET nai_get_base_socket(SOCKET socket)
{
    SOCKET r;
    DWORD ec;
    DWORD bytes;


    if (WSAIoctl(socket, SIO_BASE_HANDLE, 
        0, 0, &r, sizeof(r), &bytes, 0, 0) != SOCKET_ERROR) {
        goto _end;
    };

    ec = GetLastError();
    if (ec == WSAENOTSOCK) {
        nai_errno = EBADF;
        r = 0;
        goto _end;
    };

    /* Even though Microsoft documentation clearly states that LSPs should
     * never intercept the `SIO_BASE_HANDLE` ioctl [1], Komodia based LSPs do
     * so anyway, breaking it, with the apparent intention of preventing LSP
     * bypass [2]. Fortunately they don't handle `SIO_BSP_HANDLE_POLL`, which
     * will at least let us obtain the socket associated with the next winsock
     * protocol chain entry. If this succeeds, loop around and call
     * `SIO_BASE_HANDLE` again with the returned BSP socket, to make sure that
     * we unwrap all layers and retrieve the actual base socket.
     *  [1] https://docs.microsoft.com/en-us/windows/win32/winsock/winsock-ioctls
     *  [2] https://www.komodia.com/newwiki/index.php?title=Komodia%27s_Redirector_bug_fixes#Version_2.2.2.6
     */
    if (WSAIoctl(socket, SIO_BSP_HANDLE_POLL, 
        0, 0, &r, sizeof(r), &bytes, 0, 0) != SOCKET_ERROR) {
        goto _end;
    };

    ec = GetLastError();
    if (ec == WSAENOTSOCK) {
        nai_errno = EBADF;
    } else {
        nai_errno = nai_errno_from_oserr(ec);
    };

    r = 0;

_end:
    return r;
};



#define NAI_WPOLL_MAX_PRE_COUNT     64

#define NAI_WPOLL_IDLE              0
#define NAI_WPOLL_PENDING           1
#define NAI_WPOLL_CANCELLED         2



typedef SOCKET nai_fdos_t;

typedef struct nai_wpoll_afd_s {
    nai_list_entry_t ent;
    HANDLE poll;
    nai_int_t count;
} nai_wpoll_afd_t;

typedef struct nai_wpoll_sock_s {
    IO_STATUS_BLOCK iosb;
    AFD_POLL_INFO info;
    nai_wpoll_afd_t* afd;
    nai_evloop_ent_t* ev;
    nai_rbnode_t node;
    nai_list_entry_t ent;
    nai_fdos_t base;
    nai_int_t key;
    union {
        struct {
            uint32_t status:8;
            uint32_t events_new:8;
            uint32_t events_set:8;
            uint32_t deleted:1;
            uint32_t closed:1;
        };
        uint32_t flags;
    };
} nai_wpoll_sock_t;



static nai_wpoll_afd_t* nai_wpoll_ref_afd(nai_wpoll_t* p)
{
    nai_wpoll_afd_t* r;
    nai_list_entry_t* e;


    e = p->poll.next;
    if (e != &p->poll) {
        r = nai_containof(e, nai_wpoll_afd_t, ent);
        if (r->count < NAI_WPOLL_MAX_PRE_COUNT) {
            r->count ++;
            /* move to tail */
            if (r->count >= NAI_WPOLL_MAX_PRE_COUNT) {
                nai_list_entry_remove(&r->ent);
                nai_list_insert_tail(&p->poll, &r->ent);
            };
            goto _end;
        };
    };

    r = (nai_wpoll_afd_t*)nai_fixedpool_alloc(&p->pool);
    if (r == 0) {
        goto _end;
    };

    r->poll = nai_afd_open(p->port);
    if (r->poll == 0) {
        nai_fixedpool_free(&p->pool, r);
        goto _end;
    };

    nai_list_insert_head(&p->poll, &r->ent);
    r->count = 1;

_end:
    return r;
};


static nai_int_t nai_wpoll_release_afd(nai_wpoll_t* p, nai_wpoll_afd_t* a)
{
    nai_int_t r;


    a->count --;
    if (a->count == NAI_WPOLL_MAX_PRE_COUNT - 1) {
        nai_list_entry_remove(&a->ent);
        nai_list_insert_head(&p->poll, &a->ent);
    };

    r = 0;

    return r;
};


static nai_rbnode_t** nai_wpoll_find_socket(
    nai_wpoll_t* p, nai_int_t key, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->smap);
    nai_rbnode_t* parent = nai_rbtree_end(&p->smap);
    nai_wpoll_sock_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nai_wpoll_sock_t, node);
        if (e->key == key) {
            break;
        } else if (e->key >= key) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    if (pparent) {
        pparent[0] = parent;
    };

    return n;
};


static nai_wpoll_sock_t* nai_wpoll_get_socket(
    nai_wpoll_t* p, nai_evnode_t* h, nai_int_t na)
{
    nai_int_t ec;
    nai_int_t key;
    nai_fdos_t base;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_wpoll_sock_t* r;
    nai_evloop_ent_t* ent;


    ent = h->ent;
    if (ent->key <= 0) {  /* create */
        if (na) {
            nai_errno = ENOENT;
            r = 0;
            goto _end;
        };

        base = nai_get_base_socket((nai_fdos_t)h->fd);
        if (base == 0) {
            r = 0;
            goto _end;
        };

        for (;;) {
            p->next ++;
            if (p->next <= 0) {
                p->next = 1;
            };

            key = p->next;
            n = nai_wpoll_find_socket(p, key, &parent);
            if (n[0] != 0) {
                continue;
            };

            r = nai_fixedpool_alloc(&p->pool);
            if (r == 0) {
                goto _end;
            };

            nai_memset(r, 0, sizeof(*r));
            nai_list_init(&r->ent);
            r->base = base;
            r->key = key;
            r->ev = ent;
            r->flags = 0;
            r->status = NAI_WPOLL_IDLE;
            r->afd = nai_wpoll_ref_afd(p);
            if (r->afd) {
                ent->key = key;
                nai_rbtree_link(&p->smap, &r->node, parent, n);
                nai_rbtree_color(&p->smap, &r->node);
            } else {
                ec = nai_errno;
                nai_fixedpool_free(&p->pool, r);
                r = 0;
            };

            break;
        };
    } else {
        key = ent->key;
        n = nai_wpoll_find_socket(p, key, 0);

        assert(n[0] != 0);

        r = nai_containof(n[0], nai_wpoll_sock_t, node);
    };

_end:
    return r;
};


static nai_int_t nai_wpoll_delete_socket(nai_wpoll_t* p, nai_wpoll_sock_t* s)
{
    nai_int_t r;


    nai_wpoll_release_afd(p, s->afd);
    nai_rbtree_erase(&p->smap, &s->node);
    nai_list_entry_remove(&s->ent);
    nai_fixedpool_free(&p->pool, s);
    r = 0;

    return r;
};


static nai_int_t nai_wpoll_remove_socket(nai_wpoll_t* p, nai_evnode_t* h)
{
    nai_int_t r;
    nai_int_t key;
    nai_rbnode_t** n;
    nai_wpoll_sock_t* s;
    nai_evloop_ent_t* ent;


    ent = h->ent;
    if (ent->key <= 0) {
        r = 0;
        goto _end;
    };

    key = ent->key;
    n = nai_wpoll_find_socket(p, key, 0);

    assert(n[0] != 0);

    s = nai_containof(n[0], nai_wpoll_sock_t, node);
    switch(s->status) {
    case NAI_WPOLL_PENDING:
        r = nai_afd_cancel(s->afd->poll, &s->iosb);
        if (r < 0) {
            /* ignore */
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "nai_afd_cancel failed");
        };
        s->status = NAI_WPOLL_CANCELLED;
        break;
    };

    h->st.seted &= ~NAI_EV_IOE;
    ent->key = 0;
    if (s->status != NAI_WPOLL_IDLE) {
        s->deleted = 1;
        r = 0;
        goto _end;
    };

    r = nai_wpoll_delete_socket(p, s);

_end:
    return r;
};


static DWORD nai_wpoll_to_afd_events(nai_int_t events)
{
    DWORD afd_events;

    afd_events = AFD_POLL_LOCAL_CLOSE;
    afd_events |= AFD_POLL_DISCONNECT;
    afd_events |= AFD_POLL_ABORT;
    afd_events |= AFD_POLL_CONNECT_FAIL;

    if (events & NAI_EV_READ)
        afd_events |= AFD_POLL_RECEIVE | AFD_POLL_ACCEPT;
    if (events & NAI_EV_WRITE)
        afd_events |= AFD_POLL_SEND;
    if (events & NAI_EV_EXCEPT)
        afd_events |= AFD_POLL_RECEIVE_EXPEDITED;

    return afd_events;
};


static nai_int_t nai_wpoll_from_afd_events(DWORD afd_events)
{
    nai_int_t events;

    if (afd_events & (AFD_POLL_CONNECT_FAIL)) {
        events = nai_ev_error_from(ECONNREFUSED);
        goto _end;
    };

    events = 0;
    if (afd_events & (AFD_POLL_RECEIVE | AFD_POLL_ACCEPT))
        events |= NAI_EV_READ;
    if (afd_events & (AFD_POLL_DISCONNECT | AFD_POLL_ABORT))
        events |= NAI_EV_READ;
    if (afd_events & AFD_POLL_SEND)
        events |= NAI_EV_WRITE;
    if (afd_events & AFD_POLL_RECEIVE_EXPEDITED)
        events |= NAI_EV_EXCEPT;

_end:
    return events;
};



static nai_int_t nai_wpoll_init(nai_wpoll_t* p)
{
    nai_int_t r;


    nai_rbtree_init(&p->smap);
    nai_list_init(&p->poll);
    nai_list_init(&p->update);
    nai_fixedpool_init(&p->pool, sizeof(nai_wpoll_sock_t), 0, 0);
    p->next = 0;
    p->notsup = 0;
    r = 0;

    return r;
};


static nai_int_t nai_wpoll_open(nai_wpoll_t* p, HANDLE port)
{
    nai_int_t r;
    nai_wpoll_afd_t* a;


    p->port = port;
    a = nai_wpoll_ref_afd(p);
    if (a == 0) {
        if (nai_errno == ENOENT) {
            nai_errno = ENOTSUP;
            p->notsup = 1;
        };
        r = -1;
        goto _end;
    };

    nai_wpoll_release_afd(p, a);
    r = 0;

_end:
    return r;
};


static nai_int_t nai_wpoll_close(nai_wpoll_t* p)
{
    nai_int_t r;
    nai_wpoll_afd_t* a;
    nai_list_entry_t* e;


    e = p->poll.next;
    for ( ; e != &p->poll; ) {
        a = nai_containof(e, nai_wpoll_afd_t, ent);
        e = e->next;
        nai_sock_close(a->poll);
    };

    nai_fixedpool_close(&p->pool);
    nai_rbtree_init(&p->smap);
    nai_list_init(&p->update);
    nai_list_init(&p->poll);
    p->next = 0;
    r = 0;

    return r;
};


static nai_int_t nai_wpoll_set(
    nai_wpoll_t* p, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t na;
    uint32_t set;
    nai_wpoll_sock_t* s;


    events &= NAI_EV_IOE;
    if (p->notsup || nai_evnode_get_type(h) != NAI_FD_TYPE_SOCK) {
        if (events) {
            nai_errno = ENOTSUP;
            r = -1;
        } else {
            r = 0;
        };
        goto _end;
    };

    na = !events;
    s = nai_wpoll_get_socket(p, h, na);
    if (s == 0) {
        r = -1;
        if (na) {
            ec = nai_errno;
            if (ec == ENOENT) {
                r = 0;
            };
        };
        goto _end;
    };

    set = events;
    switch (s->status) {
    case NAI_WPOLL_PENDING:
        if (s->events_set != set) {
            r = nai_afd_cancel(s->afd->poll, &s->iosb);
            if (r < 0) {
                nai_log_alert(NAI_LOG_CORE, 
                    nai_errno, "nai_afd_cancel failed");
                goto _end;
            };
            s->status = NAI_WPOLL_CANCELLED;
            s->events_new = set;
        };
        break;

    case NAI_WPOLL_CANCELLED:
        s->events_new = set;
        break;

    case NAI_WPOLL_IDLE:
        s->events_new = set;
        if (s->events_new) {
            nai_list_entry_remove(&s->ent);
            nai_list_insert_tail(&p->update, &s->ent);
        };
        break;

    default:
        assert(0);
        break;
    };

    h->st.seted = set | (h->st.seted & ~NAI_EV_IOE);
    r = 0;

_end:
    return r;
};


static nai_int_t nai_wpoll_del(nai_wpoll_t* p, nai_evnode_t* h)
{
    nai_int_t r;


    r = nai_wpoll_remove_socket(p, h);

    return r;
};


static nai_int_t nai_wpoll_update(
    nai_wpoll_t* p, nai_evloop_ent_t** lptr[2])
{
    nai_int_t r;
    nai_int_t count;
    nai_wpoll_sock_t* s;
    nai_list_entry_t* e;
    nai_evloop_ent_t* ent;


    count = 0;
    e = p->update.next;
    for ( ; e != &p->update; ) {
        s = nai_containof(e, nai_wpoll_sock_t, ent);
        e = e->next;

        nai_list_entry_remove(&s->ent);
        nai_list_init(&s->ent);
        if (!s->events_new) {
            continue;
        };

        s->info.Exclusive = FALSE;
        s->info.NumberOfHandles = 1;
        s->info.Timeout.QuadPart = INT64_MAX;
        s->info.Handles[0].Handle = (HANDLE)s->base;
        s->info.Handles[0].Status = 0;
        s->info.Handles[0].Events = nai_wpoll_to_afd_events(s->events_new);
        r = nai_afd_poll(s->afd->poll, &s->info, &s->iosb);
        if (r >= 0) {
            s->events_set = s->events_new;
            s->status = NAI_WPOLL_PENDING;
        } else {
            count ++;
            ent = s->ev;
            ent->events = nai_ev_error_from(nai_errno);
            lptr[ent->priority][0] = ent;
            lptr[ent->priority] = &ent->next;
        };
    };

    lptr[0][0] = 0;
    lptr[1][0] = 0;

    return count;
};


static nai_int_t nai_wpoll_handle(
    nai_wpoll_t* p, nai_evloop_ent_t** lptr[2], OVERLAPPED* op)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t events;
    DWORD err;
    nai_wpoll_sock_t* s;
    nai_evloop_ent_t* ent;


    if (op == 0) {
        r = 0;
        goto _end;
    };

    s = nai_containof(op, nai_wpoll_sock_t, iosb);
    if (s->deleted) {
        nai_wpoll_delete_socket(p, s);
        r = 0;
        goto _end;
    };

    if (s->iosb.Status == STATUS_CANCELLED) {
        events = 0;
    } else if (!NT_SUCCESS(s->iosb.Status)) {
        err = nai_wapi.RtlNtStatusToDosError(s->iosb.Status);
        ec = nai_errno_from_oserr(err);
        events = nai_ev_error_from(ec);
    } else if (s->info.NumberOfHandles < 1) {
        events = 0;
    } else if (s->info.Handles[0].Events & AFD_POLL_LOCAL_CLOSE) {
        events = nai_ev_error_from(ENOTCONN);
    } else {
        events = nai_wpoll_from_afd_events(s->info.Handles[0].Events);
    };

    s->status = NAI_WPOLL_IDLE;
    s->events_set = 0;
    if (s->events_new) {
        nai_list_entry_remove(&s->ent);
        nai_list_insert_tail(&p->update, &s->ent);
    };

    if (events) {
        ent = s->ev;
        ent->events = events;
        lptr[ent->priority][0] = ent;
        lptr[ent->priority] = &ent->next;
    };

    r = 0;

_end:
    return r;
};



#endif

