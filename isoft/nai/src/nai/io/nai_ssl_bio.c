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
/// @file       nai_ssl_bio.c
/// @brief      
/// @details
/// @date       2022-10-22
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_ssl_ctx.h"


#if (NAI_HAVE_SSL)


#include "nai/io/nai_event.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"

#if (NAI_HAVE_OPENSSL) || (NAI_HAVE_LIBRESSL)
#include <openssl/bio.h>
#elif (NAI_HAVE_WOLFSSL)
#include <wolfssl/openssl/bio.h>
#ifndef BIO_TYPE_SOURCE_SINK
#define BIO_TYPE_SOURCE_SINK    BIO_TYPE_FILE
#endif
#endif


//////////////////////////////////////////////////////////////////////////////
// bio



#if (OPENSSL_VERSION_NUMBER < 0x10100000L) || \
    (defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER < 0x20700000L)


static inline BIO_METHOD* BIO_meth_new(int type, const char* name)
{
    BIO_METHOD* biom = nai_malloc(sizeof(BIO_METHOD));

    if (biom != NULL) {
        biom->create = NULL;
        biom->destroy = NULL;
        biom->bgets = NULL;
        biom->bputs = NULL;
        biom->bread = NULL;
        biom->bwrite = NULL;
        biom->ctrl = NULL;
        biom->callback_ctrl = NULL;
        biom->type = type;
        biom->name = name;
    }
    return biom;
};


static inline void BIO_meth_free(BIO_METHOD* biom)
{
    if (biom != NULL) {
        nai_free(biom);
    };

    return;
};


#define BIO_meth_set_write(b, f) (b)->bwrite = (f)
#define BIO_meth_set_read(b, f) (b)->bread = (f)
#define BIO_meth_set_puts(b, f) (b)->bputs = (f)
#define BIO_meth_set_ctrl(b, f) (b)->ctrl = (f)
#define BIO_meth_set_create(b, f) (b)->create = (f)
#define BIO_meth_set_destroy(b, f) (b)->destroy = (f)

#define BIO_set_init(b, val) (b)->init = (val)
#define BIO_set_data(b, val) (b)->ptr = (val)
#define BIO_set_shutdown(b, val) (b)->shutdown = (val)
#define BIO_get_init(b) (b)->init
#define BIO_get_data(b) (b)->ptr
#define BIO_get_shutdown(b) (b)->shutdown


#endif



#if (NAI_HAVE_KTLS)


#define BIO_CTRL_SET_KTLS                   72
#define BIO_CTRL_SET_KTLS_TX_SEND_CTRL_MSG  74
#define BIO_CTRL_CLEAR_KTLS_TX_CTRL_MSG     75

#define BIO_FLAGS_KTLS_TX_CTRL_MSG          0x1000
#define BIO_FLAGS_KTLS_RX                   0x2000
#define BIO_FLAGS_KTLS_TX                   0x4000

/* KTLS related controls and flags */
#define BIO_set_ktls_flag(b, is_tx)                     \
    BIO_set_flags(b, (is_tx) ? BIO_FLAGS_KTLS_TX : BIO_FLAGS_KTLS_RX)
#define BIO_should_ktls_flag(b, is_tx)                  \
    BIO_test_flags(b, (is_tx) ? BIO_FLAGS_KTLS_TX : BIO_FLAGS_KTLS_RX)
#define BIO_set_ktls_ctrl_msg_flag(b)                   \
    BIO_set_flags(b, BIO_FLAGS_KTLS_TX_CTRL_MSG)
#define BIO_should_ktls_ctrl_msg_flag(b)                \
    BIO_test_flags(b, BIO_FLAGS_KTLS_TX_CTRL_MSG)
#define BIO_clear_ktls_ctrl_msg_flag(b)                 \
    BIO_clear_flags(b, BIO_FLAGS_KTLS_TX_CTRL_MSG)


extern nai_int_t nai_ktls_enable(nai_fd_t fd);
extern nai_int_t nai_ktls_start(nai_int_t fd, void* info, nai_int_t is_tx);
extern intptr_t nai_ktls_recv(nai_ssl_t* s, 
    void* buf, size_t len, nai_int_t slot);
extern intptr_t nai_ktls_send(nai_ssl_t* s, 
    const void* buf, size_t len, uint8_t rtype);


#endif



static int nai_bio_new(BIO *b)
{
    BIO_set_init(b, 0);
    BIO_set_data(b, 0);
    return 1;
};


static int nai_bio_free(BIO *b)
{
    nai_int_t r;


    if (!b) {
        r = 0;
    } else {
        if (BIO_get_shutdown(b)) {
            /* nothing */
        };
        r = 1;
    };

    return r;
};


static long nai_bio_ctrl(BIO *b, int cmd, long num, void *ptr)
{
    long r = 1;
#if (NAI_HAVE_KTLS)
    nai_fd_t fd;
#endif
    nai_ssl_t* ssl;
    nai_ssl_ops_t* ops;
#if defined(BIO_CTRL_DGRAM_SET_NEXT_TIMEOUT)
    uint64_t time;
    struct timeval* tv;
#else
    (void)ptr;
#endif


    ssl = (nai_ssl_t*)BIO_get_data(b);

    switch (cmd) {
    case BIO_CTRL_GET_CLOSE:
        r = BIO_get_shutdown(b);
        break;

    case BIO_CTRL_SET_CLOSE:
        BIO_set_shutdown(b, (int)num);
        break;

    case BIO_CTRL_PENDING:
        ops = ssl->ops;
        if (ops->poll) {
            r = ops->poll(ssl->ud, NAI_EV_READ);
        } else {
            r = 1;
        };
        break;

    case BIO_CTRL_WPENDING:
        ops = ssl->ops;
        if (ops->poll) {
            r = ops->poll(ssl->ud, NAI_EV_WRITE);
        } else {
            r = 1;
        };
        break;

    case BIO_CTRL_DUP:
    case BIO_CTRL_FLUSH:
        r = 1;
        break;

#if defined(BIO_CTRL_DGRAM_GET_MTU)
    case BIO_CTRL_DGRAM_GET_MTU:
#endif
    case BIO_CTRL_DGRAM_QUERY_MTU:
        ops = ssl->ops;
        if (ops->get_mtu == 0) {
            r = 1500;
            break;
        };

        r = ops->get_mtu(ssl->ud);
        break;

#if defined(BIO_CTRL_DGRAM_SET_NEXT_TIMEOUT)
    case BIO_CTRL_DGRAM_SET_NEXT_TIMEOUT:
        if (!ssl->connect || ssl->handshaked) {
            r = 0;
            break;
        };

        ops = ssl->ops;
        if (ops->set_timer == 0) {
            r = 0;
            break;
        };

        tv = ptr;
        time = (tv->tv_sec * 1000000 + tv->tv_usec);
        if (time > 0) {
            if (ssl->timeoset) {
                break;
            };
            time -= nai_timecache_to_usec();
            time /= 1000;
            ssl->timeoset = 1;
            ssl->ops->set_timer(ssl->ud, (uint32_t)time);
        } else {
            ssl->timeoset = 0;
            ssl->ops->set_timer(ssl->ud, (uint32_t)-1);
        };

        r = 0;
        break;
#endif


#if (NAI_HAVE_KTLS)
    case BIO_CTRL_SET_KTLS:
        ops = ssl->ops;
        if (ops->get_socket == 0 || 
            ops->recvm == 0 || 
            ops->sendm == 0) {
            r = 0;
            break;
        };

        fd = ops->get_socket(ssl->ud);
        if (fd == NAI_FD_INVALID) {
            break;
        };

        r = nai_ktls_enable(fd);
        if (r < 0) {
            break;
        };
        r = nai_ktls_start(fd, ptr, num);
        if (r < 0) {
            break;
        };

        BIO_set_ktls_flag(b, num);
        r = 1;
        break;
    case BIO_CTRL_GET_KTLS_SEND:
        r = BIO_should_ktls_flag(b, 1) != 0;
        break;
    case BIO_CTRL_GET_KTLS_RECV:
        r = BIO_should_ktls_flag(b, 0) != 0;
        break;
    case BIO_CTRL_SET_KTLS_TX_SEND_CTRL_MSG:
        BIO_set_ktls_ctrl_msg_flag(b);
        ssl->record_type = (uint8_t)num;
        r = 0;
        break;
    case BIO_CTRL_CLEAR_KTLS_TX_CTRL_MSG:
        BIO_clear_ktls_ctrl_msg_flag(b);
        break;
#endif

    default:
        r = 0;
        break;
    }

    return r;
};


static int nai_bio_read(BIO* b, char* buf, int size)
{
    nai_int_t r;
    nai_int_t ec;
    nai_ssl_t* ssl;
    nai_ssl_ops_t* ops;


    ssl = (nai_ssl_t*)BIO_get_data(b);
    ops = ssl->ops;

#if (NAI_HAVE_KTLS)
    if (BIO_get_ktls_recv(b)) {
        r = (nai_int_t)nai_ktls_recv(ssl, buf, size, nai_ssl.extend);
    } else {
#endif
        r = (nai_int_t)ops->read(ssl->ud, buf, size);
#if (NAI_HAVE_KTLS)
    };
#endif

    if (r < 0) {
        ec = nai_errno;
        if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
#if defined(SSL_MODE_RELEASE_BUFFERS)
            /**
             * when SSL_MODE_RELEASE_BUFFERS is set, 
             * the read buffer from openssl will be freed later, 
             * the async operation write on freed memory, 
             * and program will be crashed
             */
            if (ec == NAI_EINPROGRESS) {
                ssl->read_pending = 1;
                SSL_clear_mode(ssl->ssl, SSL_MODE_RELEASE_BUFFERS);
            };
#endif
            BIO_set_retry_read(b);
        } else {
            BIO_clear_retry_flags(b);
        };
    } else {
#if defined(SSL_MODE_RELEASE_BUFFERS)
        if (ssl->read_pending) {
            ssl->read_pending = 0;
            SSL_set_mode(ssl->ssl, SSL_MODE_RELEASE_BUFFERS);
        };
#endif
    };

    return r;
};


static int nai_bio_write(BIO* b, const char* buf, int size)
{
    nai_int_t r;
    nai_int_t ec;
    nai_ssl_t* ssl;
    nai_ssl_ops_t* ops;


    ssl = (nai_ssl_t*)BIO_get_data(b);
    ops = ssl->ops;

#if (NAI_HAVE_KTLS)
    if (BIO_should_ktls_ctrl_msg_flag(b)) {
        r = (nai_int_t)nai_ktls_send(ssl, buf, size, ssl->record_type);
        if (r >= 0) {
            BIO_clear_ktls_ctrl_msg_flag(b);
        };
    } else {
#endif
        r = (nai_int_t)ops->write(ssl->ud, buf, size);
#if (NAI_HAVE_KTLS)
    };
#endif

    if (r < 0) {
        ec = nai_errno;
        if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
            BIO_set_retry_write(b);
        } else {
            BIO_clear_retry_flags(b);
        };
    };

    return r;
};


static int nai_bio_puts(BIO *b, const char* s)
{
    return nai_bio_write(b, s, nai_strlen(s));
};



static int nai_bio_dgram_read(BIO* b, char* buf, int size)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t namelen;
    nai_ssl_t* ssl;
    nai_ssl_ops_t* ops;
    nai_ssl_dgram_t* d;
    nai_bufvec_t v;


    ssl = (nai_ssl_t*)BIO_get_data(b);

    d = (nai_ssl_dgram_t*)SSL_get_ex_data(ssl->ssl, nai_ssl.extend);
    assert(d != 0);

    /**
     * the read buffer from DTLS_listen always be freed later, 
     * the async operation write on freed memory, 
     * and program will be crashed
     */
    namelen = sizeof(d->peer.storage);
    if (d->async) {
        v.buf = (uint8_t*)d->rbuf;
        v.len = d->size;
    } else {
        v.buf = (uint8_t*)buf;
        v.len = size;
    };

    ops = ssl->ops;
    r = (nai_int_t)ops->recvm(
        ssl->ud, &v, 1, 0, &d->peer.addr, &namelen, 0, 0);
    if (r < 0) {
        ec = nai_errno;
        if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
            BIO_set_retry_read(b);
        } else {
            BIO_clear_retry_flags(b);
        };
    } else {
        d->peer.len = namelen;

        if (r > 0 && d->async) {
            if (r > size) {
                r = size;
            };
            nai_memcpy(buf, d->rbuf, r);
        };
    };

    return r;
};


static int nai_bio_dgram_write(BIO* b, const char* buf, int size)
{
    nai_int_t r;
    nai_int_t ec;
    nai_ssl_t* ssl;
    nai_ssl_ops_t* ops;
    nai_ssl_dgram_t* d;
    nai_bufvec_t v;


    ssl = (nai_ssl_t*)BIO_get_data(b);

    d = (nai_ssl_dgram_t*)SSL_get_ex_data(ssl->ssl, nai_ssl.extend);
    assert(d != 0);

    if (d->wpending) {
        BIO_set_retry_write(b);
        r = -1;
        goto _end;
    };

    d->wbuf = (uint8_t*)buf;
    d->wlen = size;

    v.buf = (uint8_t*)d->wbuf;
    v.len = d->wlen;

    ops = ssl->ops;
    r = (nai_int_t)ops->sendm(
        ssl->ud, &v, 1, 0, &d->peer.addr, d->peer.len, 0, 0);
    if (r < 0) {
        ec = nai_errno;
        if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
            d->wpending = 1;
            r = d->wlen;
        } else {
            BIO_clear_retry_flags(b);
        };
    };

_end:
    return r;
};


static int nai_bio_dgram_puts(BIO *b, const char* s)
{
    return nai_bio_dgram_write(b, s, nai_strlen(s));
};


nai_int_t nai_ssl_bio_init()
{
    nai_int_t r;


    nai_ssl.meth = BIO_meth_new(BIO_TYPE_SOURCE_SINK, "nai_bio");
    if (nai_ssl.meth == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "BIO_meth_new() failed");
        r = -1;
        goto _end;
    } else {
        BIO_meth_set_read(nai_ssl.meth, nai_bio_read);
        BIO_meth_set_puts(nai_ssl.meth, nai_bio_puts);
        BIO_meth_set_write(nai_ssl.meth, nai_bio_write);
        BIO_meth_set_ctrl(nai_ssl.meth, nai_bio_ctrl);
        BIO_meth_set_create(nai_ssl.meth, nai_bio_new);
        BIO_meth_set_destroy(nai_ssl.meth, nai_bio_free);
    };

    nai_ssl.meth_dgram = BIO_meth_new(BIO_TYPE_SOURCE_SINK, "nai_bio_dgram");
    if (nai_ssl.meth_dgram == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "BIO_meth_new() failed");
        r = -1;
        goto _end;
    } else {
        BIO_meth_set_read(nai_ssl.meth_dgram, nai_bio_dgram_read);
        BIO_meth_set_puts(nai_ssl.meth_dgram, nai_bio_dgram_puts);
        BIO_meth_set_write(nai_ssl.meth_dgram, nai_bio_dgram_write);
        BIO_meth_set_ctrl(nai_ssl.meth_dgram, nai_bio_ctrl);
        BIO_meth_set_create(nai_ssl.meth_dgram, nai_bio_new);
        BIO_meth_set_destroy(nai_ssl.meth_dgram, nai_bio_free);
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_bio_term()
{
    nai_int_t r;


    if (nai_ssl.meth != 0) {
        BIO_meth_free(nai_ssl.meth);
        nai_ssl.meth = 0;
    };
    if (nai_ssl.meth_dgram != 0) {
        BIO_meth_free(nai_ssl.meth_dgram);
        nai_ssl.meth_dgram = 0;
    };

    r = 0;

    return r;
};


nai_int_t nai_bio_in_ktls_send(BIO* b)
{
    nai_int_t r;


#if (NAI_HAVE_KTLS)
    r = BIO_should_ktls_flag(b, 1) != 0;
#else
    (void)b;

    r = 0;
#endif

    return r;
};


#endif

