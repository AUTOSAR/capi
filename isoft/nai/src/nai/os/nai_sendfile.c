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
/// @file       nai_sendfile.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_sendfile.h"
#include "nai/runtime/nai_errno.h"



size_t nai_filearray_to_filechunk(
    nai_filearray_t* a, 
    nai_filechunk_t* c, nai_bufvec_t* e, nai_int_t count, nai_int_t max_ht)
{
    nai_int_t n;
    nai_int_t file = 0;
    nai_int_t head = 0;
    nai_int_t tail = 0;
    size_t hsize = 0;
    size_t tsize = 0;
    nai_bufvec_t* b = e;
    nai_bufvec_t* bend = b + count;
    nai_filevec_t* v;


    c->fd = NAI_FD_INVALID;
    c->off = 0;
    c->size = 0;

    /* scan file array */
    for (n = 0; n < a->count; n ++) {
        v = &a->v[n];

        /* process memory element */
        if (v->fd == NAI_FD_INVALID) {

            // check enough bufvec space
            if (b >= bend) {
                break;
            };
            b->buf = v->ptr;
            b->len = v->size;

            /* add head if the element before file, otherwise add tail */
            if (file) {
                /* break if too many tailers */
                if (tail >= max_ht && max_ht >= 0) {
                    break;
                };

                tail ++;
                tsize += b->len;
            } else {
                head ++;
                hsize += b->len;
            };
            b ++;

        } else {
            /* already found a file, filechunk accept only one file */
            if (file) {
                break;
            };
            /* break if too many headers */
            if (head > max_ht && max_ht >= 0) {
                break;
            };

            c->fd = v->fd;
            c->oflags = v->oflags;
            c->off = v->off;
            c->size = v->size;
            file = 1;
        };
    };
    a->v += n;
    a->count -= n;

    c->hvec = e;
    c->hcnt = head;
    c->hsize = hsize;
    c->tvec = e + head;
    c->tcnt = tail;
    c->tsize = tsize;

    return c->size + c->hsize + c->tsize;
};


#if defined(_WIN32)


#include "nai/os/nai_aio.h"
#include "win/nai_windows.h"
#include "win/nai_wsock.h"


#define nai_aio_set_fd(a, f)        \
    ((a)->fd = (f))                 \


static nai_int_t nai_send_packets(nai_fd_t s, 
    TRANSMIT_PACKETS_ELEMENT* e, nai_int_t ecnt, 
    size_t* sent, uint32_t timeout)
{
    intptr_t r;
    nai_int_t ec;
    nai_aio_t a;


    /* set overlapped event for waiting */
    nai_aio_init(&a);
    nai_aio_set_fd(&a, s);
    nai_aio_set_waitable(&a, 1);

    r = nai_aio_set_event(&a);
    if (r < 0) {
        goto _end;
    };

    if (!nai_wsockops.TransmitPackets(
        (SOCKET)s, e, ecnt, 0, &a.cb, NAI_SENDFILE_FLAGS)) {
        ec = GetLastError();
        if (ec != WSA_IO_PENDING) {
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            goto _end;
        };
    };

    r = nai_aio_result(&a, 0);
    if (r < 0) {
        if (nai_errno != EINPROGRESS) {
            goto _end;
        };

        // wait result
        r = nai_aio_wait(&a, timeout);
        if (r < 0) {
            /* whether timedout or failed, op should be canceled */
            nai_aio_cancel(&a);
        };

        // wait result
        r = nai_aio_result(&a, 1);
        if (r < 0) {
            goto _end;
        };
    };

    sent[0] = r;
    r = 0;

_end:
    return (nai_int_t)r;
};


static nai_int_t nai_send_filechunk(nai_fd_t s, 
    nai_fd_t f, nai_off64_t off, 
    size_t size, TRANSMIT_FILE_BUFFERS* b, 
    size_t* sent, uint32_t timeout)
{
    intptr_t r;
    nai_int_t ec;
    nai_aio_t a;


    /* set overlapped event for waiting */
    nai_aio_init(&a);
    nai_aio_set_fd(&a, s);
    nai_aio_set_offset(&a, off);
    nai_aio_set_waitable(&a, 1);

    r = nai_aio_set_event(&a);
    if (r < 0) {
        goto _end;
    };

    if (!nai_wsockops.TransmitFile(
        (SOCKET)s, f, (DWORD)size, 0, &a.cb, b, NAI_SENDFILE_FLAGS)) {
        ec = GetLastError();
        if (ec != WSA_IO_PENDING) {
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            goto _end;
        };
    };

    r = nai_aio_result(&a, 0);
    if (r < 0) {
        if (nai_errno != EINPROGRESS) {
            goto _end;
        };

        // wait result
        r = nai_aio_wait(&a, timeout);
        if (r < 0) {
            /* whether timedout or failed, op should be canceled */
            nai_aio_cancel(&a);
        };

        // wait result
        r = nai_aio_result(&a, 1);
        if (r < 0) {
            goto _end;
        };
    };

    sent[0] = r;
    r = 0;

_end:
    return (nai_int_t)r;
};


intptr_t nai_sendfile(nai_fd_t s, 
    const nai_filechunk_t* c, uint32_t timeout)
{
    intptr_t r;
    intptr_t total = 0;
    nai_int_t t;
    nai_int_t ecnt;
    size_t sent;
    size_t esize;
    size_t bsize;
    nai_filechunk_t d;
    TRANSMIT_FILE_BUFFERS b;
    TRANSMIT_PACKETS_ELEMENT e[NAI_BUFV_MAX];


    if (nai_wsockops.TransmitPackets) {

        d = *c;
        for (;;) {
            ecnt = nai_filechunk_to_packets(&d, e, nai_countof(e), &esize);
            if (ecnt <= 0) {
                r = 0;
                break;
            };

            r = nai_send_packets(s, e, ecnt, &sent, timeout);
            if (r < 0) {
                break;
            };

            total += sent;
            if (sent < esize) {
                break;
            };
        };

        if (r < 0) {
            if (!total || nai_errno != NAI_EAGAIN) {
                goto _end;
            };
        } else {
            nai_errno = 0;
        };

    } else if (nai_wsockops.TransmitFile) {

        for (;;) {
            if (c->hcnt > 1 || c->fd == NAI_FD_INVALID) {
                r = nai_sock_sendv(s, c->hvec, c->hcnt, 0);
                if (r < 0) {
                    break;
                };
                total += r;
                if (r < (intptr_t)c->hsize) {
                    break;
                };
            };

            t = 0;
            if (c->fd != NAI_FD_INVALID) {
                bsize = c->size;
                if (c->hcnt == 1) {
                    b.Head = c->hvec[0].buf;
                    b.HeadLength = (uint32_t)c->hvec[0].len;
                    bsize += b.HeadLength;
                };
                if (c->tcnt > 0) {
                    b.Tail = c->tvec[0].buf;
                    b.TailLength = (uint32_t)c->tvec[0].len;
                    bsize += b.TailLength;
                    t = 1;
                };

                r = nai_send_filechunk(s, 
                    c->fd, c->off, c->size, &b, &sent, timeout);
                if (r < 0) {
                    break;
                };

                total += sent;
                if (sent < esize) {
                    break;
                };
            };

            if (c->tcnt > t) {
                r = nai_sock_sendv(s, c->tvec+t, c->tcnt-t, 0);
                if (r < 0) {
                    break;
                };
                total += r;
            };

            r = 0;
            break;
        };

        if (r < 0) {
            if (!total || nai_errno != NAI_EAGAIN) {
                goto _end;
            };
        } else {
            nai_errno = 0;
        };

    } else {

        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };


    r = total;

_end:
    return r;
};


intptr_t nai_sendfilev(nai_fd_t s, 
    const nai_filevec_t* v, nai_int_t count, uint32_t timeout)
{
    intptr_t r;
    intptr_t total = 0;
    nai_int_t ecnt;
    size_t sent;
    size_t esize;
    size_t bsize;
    nai_filearray_t a;
    nai_filechunk_t c;
    nai_bufvec_t b[NAI_BUFV_MAX];
    TRANSMIT_PACKETS_ELEMENT e[NAI_BUFV_MAX];


    if (nai_wsockops.TransmitPackets) {

        a.v = (nai_filevec_t*)v;
        a.count = count;
        for (;;) {
            ecnt = nai_filearray_to_packets(&a, e, nai_countof(e), &esize);
            if (ecnt <= 0) {
                r = 0;
                break;
            };

            r = nai_send_packets(s, e, ecnt, &sent, timeout);
            if (r < 0) {
                break;
            };

            total += sent;
            if (sent < esize) {
                break;
            };
        };

        if (r < 0) {
            if (!total || nai_errno != NAI_EAGAIN) {
                goto _end;
            };
        } else {
            nai_errno = 0;
        };

    } else if (nai_wsockops.TransmitFile) {

        a.v = (nai_filevec_t*)v;
        a.count = count;
        for (;;) {
            bsize = nai_filearray_to_filechunk(&a, &c, b, nai_countof(b), -1);
            if (bsize <= 0) {
                r = 0;
                break;
            };

            r = nai_sendfile(s, &c, timeout);
            if (r < 0) {
                break;
            };
            sent = r;

            total += sent;
            if (sent < esize) {
                break;
            };
        };

        if (r < 0) {
            if (!total || nai_errno != NAI_EAGAIN) {
                goto _end;
            };
        } else {
            nai_errno = 0;
        };

    } else {

        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    r = total;

_end:
    return r;
};

#else


#include "nai/os/nai_tlocal.h"


#if (NAI_HAVE_SYS_UIO_H)
#include <sys/uio.h>
#endif
#if (NAI_HAVE_SYS_SENDFILE_H)
#include <sys/sendfile.h>
#endif
#if (NAI_HAVE_SYS_SOCKET_H)
#include <sys/socket.h>
#endif
#if (NAI_HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif


#if defined(__freebsd__)
#include <osreldate.h>
#endif



intptr_t nai_sendfile(nai_fd_t s, 
    const nai_filechunk_t* c, uint32_t timeout)
{
    intptr_t r;
    nai_int_t ec;


    (void)timeout;

#if (NAI_HAVE_SENDFILE_FREEBSD)

    intptr_t sent;
    struct sf_hdtr ht;


    if (c->fd == NAI_FD_INVALID) {
        sent = 0;

        if (c->hcnt) {
            r = nai_file_writev(s, c->hvec, c->hcnt);
            if (r < 0) {
                goto _fail;
            };

            sent += r;
            if ((size_t)r < c->hsize) {
                goto _end;
            };
        };
        if (c->tcnt) {
            r = nai_file_writev(s, c->tvec, c->tcnt);
            if (r < 0) {
                goto _fail;
            };

            sent += r;
            if ((size_t)r < c->tsize) {
                goto _end;
            };
        };

    } else {

        ht.headers = c->hcnt ? (struct iovec*)c->hvec : 0;
        ht.hdr_cnt = c->hcnt;
        ht.trailers = c->tcnt ? (struct iovec*)c->tvec : 0;
        ht.trl_cnt = c->tcnt;


        for (;;) {

#if (__darwin__)
            sent = c->size + c->hsize + c->tsize;
            r = sendfile(c->fd, s, c->off, &sent, &ht, 0);
#else /* __freebsd__ */
            sent = 0;

#if (__FreeBSD__ == 4 && __FreeBSD_version >= 460102) \
    || __FreeBSD_version == 460002 || __FreeBSD_version >= 500039
            r = sendfile(c->fd, s, c->off, c->size, &ht, &sent, 0);
#else
            r = sendfile(c->fd, s, c->off, 
                c->size + c->hsize + c->tsize, &ht, &sent, 0);
#endif

#endif
            if (r < 0) {
                ec = nai_errno;
                switch (ec) {
                case EINTR:
                    /* EINTR  sendfile will save sent bytes in 'sent' */
                    if (sent <= 0) {
                        if (!nai_thread_io_canceling()) {
                            continue;
                        };

                        nai_errno = ECANCELED;
                        goto _fail;
                    };
                    break;

                case NAI_EAGAIN:
                    /* EAGAIN sendfile will save sent bytes in 'sent' */
                    /* fail to default case */

                default:
                    goto _fail;
                };
            };

            break;
        };
    };

_end:
    nai_errno = 0;
    r = sent;

    if (0) {
_fail:
        if (sent > 0) {
            ec = nai_errno;
            if (ec == NAI_EAGAIN || ec == ECANCELED) {
                r = sent;
            };
        };
    };

#elif (NAI_HAVE_SENDFILE_LINUX)

    off_t off;
    intptr_t sent = 0;

    if (c->hcnt) {
        r = nai_file_writev(s, c->hvec, c->hcnt);
        if (r < 0) {
            goto _fail;
        };

        sent += r;
        if ((size_t)r < c->hsize) {
            goto _end;
        };
    };

    if (c->fd != NAI_FD_INVALID) {
        off = (off_t)c->off;
        do {
            r = sendfile(s, c->fd, &off, c->size);
        } while (r < 0 && nai_thread_io_continue(&nai_errno));

        if (r < 0) {
            goto _fail;
        };

        sent += r;
        if ((size_t)r < c->size) {
            goto _end;
        };
    };

    if (c->tcnt) {
        r = nai_file_writev(s, c->tvec, c->tcnt);
        if (r < 0) {
            goto _fail;
        };

        sent += r;
        if ((size_t)r < c->tsize) {
            goto _end;
        };
    };

_end:
    nai_errno = 0;
    r = sent;

    if (0) {
_fail:
        if (sent > 0) {
            ec = nai_errno;
            if (ec == NAI_EAGAIN || ec == ECANCELED) {
                r = sent;
            };
        };
    };

#elif (NAI_HAVE_SENDFILEV)

    nai_int_t n = 0;
    nai_int_t h = 0, t = 0;
    size_t total = 0;
    size_t sent;
    size_t vsize;
    size_t vcnt;
    sendfilevec_t* v;
    sendfilevec_t sfv[NAI_BUFV_MAX];


    for (;;) {
        v = sfv;
        vcnt = 0;
        vsize = 0;

        for ( ; h < c->hcnt && vcnt < nai_countof(sfv); ) {
            v->sfv_fd = SFV_FD_SELF;
            v->sfv_flag = 0;
            v->sfv_off = (uintptr_t)c->hvec[h].buf;
            v->sfv_len = c->hvec[h].len;
            vsize += v->sfv_len;
            vcnt ++;
            v ++;
            h ++;
        };

        if (n < 1 && vcnt < nai_countof(sfv)) {
            v->sfv_fd = c->fd;
            v->sfv_flag = 0;
            v->sfv_off = c->off;
            v->sfv_len = c->size;
            vsize += v->sfv_len;
            vcnt ++;
            v ++;
            n ++;
        };

        for ( ; t < c->tcnt && vcnt < nai_countof(sfv); ) {
            v->sfv_fd = SFV_FD_SELF;
            v->sfv_flag = 0;
            v->sfv_off = (uintptr_t)c->tvec[t].buf;
            v->sfv_len = c->tvec[t].len;
            vsize += v->sfv_len;
            vcnt ++;
            v ++;
            t ++;
        };

        if (vsize <= 0) {
            nia_errno = 0;
            break;
        };

        for (;;) {
            sent = 0;
            r = sendfilev(s, sfv, vcnt, &sent);
            if (r < 0) {
                ec = nai_errno;
                switch (ec) {
                case EINTR:
                    /* EINTR  sendfilev will save sent bytes in 'sent' */
                    if (sent <= 0) {
                        if (!nai_thread_io_canceling()) {
                            continue;
                        };
                        if (total == 0) {
                            nai_errno = ECANCELED;
                            goto _fail;
                        };
                    };
                    break;
                case NAI_EAGAIN:
                    /* EAGAIN sendfilev will save sent bytes in 'sent' */
                    break;

                default:
                    goto _fail;
                };
            };

            break;
        };

        total += sent;
        if (r < 0) {
            break;
        } else if (sent < vsize) {
            nia_errno = 0;
            break;
        } else {
            /* nothing */
            ;
        };
    };

    r = total;

_fail:


#else
    (void)s;
    (void)c;
    (void)timeout;
    (void)ec;

    /* not supported */
    nai_errno = ENOTSUP;
    r = -1;

#endif

    return r;
};


intptr_t nai_sendfilev(nai_fd_t s, 
    const nai_filevec_t* p, nai_int_t count, uint32_t timeout)
{
    intptr_t r;

#if (NAI_HAVE_SENDFILEV)

    nai_int_t n = 0;
    intptr_t total = 0;
    size_t sent;
    size_t vsize;
    size_t vcnt;
    sendfilevec_t* v;
    sendfilevec_t sfv[NAI_BUFV_MAX];


    (void)timeout;

    for (;;) {
        v = sfv;
        vcnt = 0;
        vsize = 0;

        for ( ; n < count && vcnt < nai_countof(sfv); ) {
            if (p[n].fd == NAI_FD_INVALID) {
                v->sfv_fd = SFV_FD_SELF;
                v->sfv_flag = 0;
                v->sfv_off = (uintptr_t)p[n].ptr;
            } else {
                v->sfv_fd = p[n].fd;
                v->sfv_flag = 0;
                v->sfv_off = p[n].off;
            };
            v->sfv_len = p[n].size;
            vsize += v->sfv_len;
            vcnt ++;
            v ++;
            h ++;
        };

        if (vsize <= 0) {
            nia_errno = 0;
            break;
        };

        for (;;) {
            sent = 0;
            r = sendfilev(s, sfv, vcnt, &sent);
            if (r < 0) {
                switch (nai_errno) {
                case EINTR:
                    /* EINTR  sendfilev will save sent bytes in 'sent' */
                    if (sent <= 0) {
                        continue;
                    };
                    break;
                case NAI_EAGAIN:
                    /* EAGAIN sendfilev will save sent bytes in 'sent' */
                    break;
                default:
                    goto _fail;
                };
            };

            break;
        };

        total += sent;
        if (r < 0) {
            break;
        } else if (sent < vsize) {
            nia_errno = 0;
            break;
        };
    };

    r = total;

_fail:

#elif (NAI_HAVE_SENDFILE_FREEBSD) || (NAI_HAVE_SENDFILE_LINUX)

    intptr_t total = 0;
    size_t sent;
    size_t bsize;
    nai_filearray_t a;
    nai_filechunk_t c;
    nai_bufvec_t b[64];


    a.v = (nai_filevec_t*)p;
    a.count = count;
    for (;;) {
        bsize = nai_filearray_to_filechunk(&a, &c, b, nai_countof(b), -1);
        if (bsize <= 0) {
            break;
        };

        r = nai_sendfile(s, &c, timeout);
        if (r < 0) {
            goto _fail;
        };
        sent = r;

        total += sent;
        if (sent < bsize) {
            break;
        };
    };

    nai_errno = 0;
    r = total;

    if (0) {
_fail:
        if (total > 0 && nai_errno == NAI_EAGAIN) {
            r = total;
        };
    };

#else
    (void)s;
    (void)p;
    (void)count;
    (void)timeout;

    /* not supported */
    nai_errno = ENOTSUP;
    r = -1;

#endif

    return r;
};

#endif

