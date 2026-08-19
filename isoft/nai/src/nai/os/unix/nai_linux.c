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
/// @file       nai_linux.c
/// @brief      
/// @details
/// @date       2022-06-02
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_linux.h"


#if (NAI_AIO_USE_LINUX)


#include "nai/os/nai_thread.h"
#include "nai/os/nai_tlocal.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"
#include <unistd.h>
#include <sys/syscall.h>


int io_setup(u_int nr_reqs, aio_context_t *ctx)
{
    return syscall(SYS_io_setup, nr_reqs, ctx);
};


int io_destroy(aio_context_t ctx)
{
    return syscall(SYS_io_destroy, ctx);
};


int io_getevents(aio_context_t ctx, long min_nr, long nr, 
    struct io_event *events, struct timespec *tmo)
{
    return syscall(SYS_io_getevents, ctx, min_nr, nr, events, tmo);
};


int io_submit(aio_context_t ctx, long n, struct iocb **paiocb)
{
    return syscall(SYS_io_submit, ctx, n, paiocb);
};


int io_cancel(aio_context_t ctx, struct iocb *iocb, struct io_event *result)
{
    return syscall(SYS_io_cancel, ctx, iocb, result);
};



typedef struct nai_aio_wait_s {
    nai_list_entry_t ent;
    nai_cond_t* cond;
} nai_aio_wait_t;

typedef struct nai_aio_global_s {
    nai_int_t inited;
    nai_int_t error;
    nai_once_t once;
    aio_context_t ctx;
    nai_mutex_t lock;
    nai_tid_t leader;
    nai_list_t waits;
} nai_aio_global_t;


static nai_aio_global_t nai_aio_global = {
    0, 0, NAI_ONCE_INIT
};


static void nai_aio_global_fork();
static void nai_aio_global_init()
{
    nai_int_t r;
    nai_int_t max;


    nai_list_init(&nai_aio_global.waits);
    nai_mutex_init(&nai_aio_global.lock);

    r = nai_mutex_open(&nai_aio_global.lock, 0);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_mutex_open() failed");
        goto _fail;
    };

    max = NAI_AIO_LINUX_MAXREQ / 2;
    if (max < 4) {
        max = 4;
    };
    r = io_setup(max, &nai_aio_global.ctx);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "io_setup() failed");
        goto _fail;
    };

    r = 0;

_fail:
    if (r < 0) {
        nai_aio_global.error = nai_errno;
    };

    nai_memory_barrier();
    nai_aio_global.inited = 1;
    return;
};


static void nai_aio_global_term()
{
    if (nai_aio_global.inited) {
        if (nai_aio_global.ctx) {
            io_destroy(nai_aio_global.ctx);
            nai_aio_global.ctx = 0;
        };

        nai_mutex_close(&nai_aio_global.lock);
    };

    return;
};


static void nai_aio_global_fork()
{
    if (nai_aio_global.inited) {
        nai_aio_global_term();
        nai_aio_global_init();
    };

    return;
};


aio_context_t nai_aio_global_get()
{
    aio_context_t r;


    if (nai_aio_global.inited == 0) {
        nai_once(&nai_aio_global.once, nai_aio_global_init);
    };
    if (nai_aio_global.error) {
        nai_errno = nai_aio_global.error;
        r = 0;
        goto _end;
    };

    r = nai_aio_global.ctx;

_end:
    return r;
};


nai_int_t nai_aio_global_wait(nai_aio_t* a, uint32_t msec)
{
    nai_int_t r;
    nai_int_t ec;
    uint64_t us;
    uint64_t ticko;
    nai_aio_t* q;
    nai_cond_t* c;
    nai_aio_global_t* g;
    nai_aio_wait_t s;
    nai_aio_wait_t* w;
    struct io_event ev;
    struct timespec ts, *tp;


    (void)msec;

    if (nai_aio_global.inited == 0) {
        nai_once(&nai_aio_global.once, nai_aio_global_init);
    };
    if (nai_aio_global.error) {
        nai_errno = nai_aio_global.error;
        r = -1;
        goto _end;
    };

    g = &nai_aio_global;
    c = 0;

    if (msec == (uint32_t)-1) {
        /* to disable warning */
        ticko = -1;
    } else {
        ticko = nai_tick_to_usec() + msec * 1000;
    };

    nai_mutex_lock(&g->lock);


    /* follow loop */
    for (;;) {

        if (a->code != -1) {
            r = 0;
            break;
        };

        if (a->cond) {
            ec = EBUSY;
            r = -1;
            break;
        };

        /* has leader ? */
        if (g->leader) {
            if (c == 0) {
                c = nai_thread_local_cond();
                if (c == 0) {
                    ec = nai_errno;
                    r = -1;
                    break;
                };
            };

            nai_list_insert_tail(&g->waits, &s.ent);
            s.cond = c;
            a->cond = c;

            if (msec == (uint32_t)-1) {
                r = nai_cond_wait(a->cond, &g->lock);
            } else {
                r = nai_cond_timedwait(a->cond, &g->lock, ticko);
            };

            a->cond = 0;
            nai_list_entry_remove(&s.ent);

            if (r < 0) {
                ec = nai_errno;
                break;
            };

            continue;
        };

        /* to leader */
        g->leader = nai_thread_id();


        /* leader loop */
        for (;;) {
            /* unlock */
            nai_mutex_unlock(&g->lock);


            if (msec == (uint32_t)-1) {
                tp = 0;
            } else {
                us = nai_tick_to_usec();
                if (us >= ticko) {
                    us = 0;
                } else {
                    us = ticko - us;
                };
                ts.tv_sec = (us / (1000 * 1000));
                ts.tv_nsec = (long)(us % (1000 * 1000) * 1000);
                tp = &ts;
            };

            r = io_getevents(g->ctx, 1, 1, &ev, tp);
            if (r < 0) {
                ec = nai_thread_io_errno();
                if (ec == EINTR) {
                    nai_mutex_lock(&g->lock);
                    continue;
                };

                nai_log_alert(NAI_LOG_CORE, ec, "io_getevents failed");
                r = -ec;
                q = 0; /* disable warning */
            } else if (r == 0) {
                r = -ETIMEDOUT;
            } else {
                q = (nai_aio_t*)ev.data;
                if (q == 0) {
                    nai_mutex_lock(&g->lock);
                    continue;
                };

                if (ev.res < 0) {
                    q->cb.aio_nbytes = -1;
                    nai_memory_barrier();
                    q->code = -ev.res;
                } else {
                    q->cb.aio_nbytes = ev.res;
                    nai_memory_barrier();
                    q->code = 0;
                };
            };


            nai_mutex_lock(&g->lock);

            if (r < 0) {
                ec = -r;
                r = -1;
                break;
            } else if (q == a) {
                r = 0;
                break;
            } else {

                if (q->cond) {
                    nai_cond_signal(q->cond);
                };
            };
        };

        if (!nai_list_is_empty(&g->waits)) {
            w = (nai_aio_wait_t*)g->waits.next;
            nai_cond_signal(w->cond);
        };

        g->leader = 0;
        break;
    };

    nai_mutex_unlock(&g->lock);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


#endif


#if (NAI_AIO_USE_URING)


#include "nai/os/nai_thread.h"
#include "nai/os/nai_tlocal.h"
#include "nai/io/nai_event.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"
#include <poll.h>
#include <sys/eventfd.h>


typedef struct nai_uring_wait_s {
    nai_list_entry_t ent;
    nai_cond_t* cond;
} nai_uring_wait_t;

typedef struct nai_uring_global_s {
    nai_int_t inited;
    nai_int_t error;
    nai_once_t once;
    nai_uring_t ring;
    nai_mutex_t lock;
    nai_tid_t leader;
    nai_list_t waits;
} nai_uring_global_t;


static nai_uring_global_t nai_uring_global = {
    0, 0, NAI_ONCE_INIT
};


static void nai_uring_global_fork();
static void nai_uring_global_init()
{
    nai_int_t r;


    nai_list_init(&nai_uring_global.waits);
    nai_mutex_init(&nai_uring_global.lock);
    nai_uring_init(&nai_uring_global.ring);

    r = nai_mutex_open(&nai_uring_global.lock, 0);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_mutex_open() failed");
        goto _fail;
    };

    r = nai_uring_open(&nai_uring_global.ring, NAI_URING_QSIZE_MT, 1);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_uring_open() failed");
        goto _fail;
    };

    r = 0;

_fail:
    if (r < 0) {
        nai_uring_global.error = nai_errno;
    };

    nai_memory_barrier();
    nai_uring_global.inited = 1;
    return;
};


static void nai_uring_global_term()
{
    if (nai_uring_global.inited) {
        nai_mutex_close(&nai_uring_global.lock);
        nai_uring_close(&nai_uring_global.ring);
    };

    return;
};


static void nai_uring_global_fork()
{
    if (nai_uring_global.inited) {
        nai_uring_global_term();
        nai_uring_global_init();
    };

    return;
};


nai_uring_t* nai_uring_global_get()
{
    nai_uring_t* r;


    if (nai_uring_global.inited == 0) {
        nai_once(&nai_uring_global.once, nai_uring_global_init);
    };
    if (nai_uring_global.error) {
        nai_errno = nai_uring_global.error;
        r = 0;
        goto _end;
    };

    r = &nai_uring_global.ring;

_end:
    return r;
};


nai_int_t nai_uring_global_wait(nai_aio_t* a, uint32_t msec)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t batch;
    uint64_t us;
    uint64_t ticko;
    nai_aio_t* q;
    nai_cond_t* c;
    nai_uring_t* u;
    nai_uring_global_t* g;
    nai_uring_postpro_t* p;
    nai_uring_wait_t s;
    nai_uring_wait_t* w;
    struct io_uring_cqe* cqe;
    struct __kernel_timespec ts, *tp;


    (void)msec;

    if (nai_uring_global.inited == 0) {
        nai_once(&nai_uring_global.once, nai_uring_global_init);
    };
    if (nai_uring_global.error) {
        nai_errno = nai_uring_global.error;
        r = -1;
        goto _end;
    };

    g = &nai_uring_global;
    u = &g->ring;
    c = 0;

    if (msec != (uint32_t)-1) {
        ticko = nai_tick_to_usec() + msec * 1000;
    };


    batch = nai_uring_is_batch(u);

    nai_mutex_lock(&g->lock);

    /* follow loop */
    for (;;) {

        if (a->code != -1) {
            r = 0;
            break;
        };

        if (a->cond) {
            ec = EBUSY;
            r = -1;
            break;
        };

        /* has leader ? */
        if (g->leader) {
            if (c == 0) {
                c = nai_thread_local_cond();
                if (c == 0) {
                    ec = nai_errno;
                    r = -1;
                    break;
                };
            };

            nai_list_insert_tail(&g->waits, &s.ent);
            s.cond = c;
            a->cond = c;

            if (msec == (uint32_t)-1) {
                r = nai_cond_wait(a->cond, &g->lock);
            } else {
                r = nai_cond_timedwait(a->cond, &g->lock, ticko);
            };

            a->cond = 0;
            nai_list_entry_remove(&s.ent);

            if (r < 0) {
                ec = nai_errno;
                break;
            };

            continue;
        };

        /* to leader */
        g->leader = nai_thread_id();


        /* leader loop */
        for (;;) {
            /* unlock */
            nai_mutex_unlock(&g->lock);


            if (msec == (uint32_t)-1) {
                tp = 0;
            } else {
                us = nai_tick_to_usec();
                if (us >= ticko) {
                    us = 0;
                } else {
                    us = ticko - us;
                };
                ts.tv_sec = (us / (1000 * 1000));
                ts.tv_nsec = (long)(us % (1000 * 1000) * 1000);
                tp = &ts;
            };

            r = io_uring_wait_cqe_timeout(&u->queue, &cqe, tp);
            if (r < 0) {
                ec = -r;
                if (ec == EINTR) {
                    if (!nai_thread_io_canceling()) {
                        nai_mutex_lock(&g->lock);
                        continue;
                    };

                    ec = ECANCELED;
                };
                if (ec != ETIMEDOUT && ec != ETIME) {
                    nai_log_alert(NAI_LOG_CORE, ec, 
                        "io_uring_wait_cqe_timeout failed");
                };
            } else {
                q = (nai_aio_t*)io_uring_cqe_get_data(cqe);
                if (q == 0) {
                    /* a canceled */
                    io_uring_cqe_seen(&u->queue, cqe);
                    nai_mutex_lock(&g->lock);
                    continue;
                };

                /* should update before fill result */
                if ((batch || q->postpro) && q->data) {
                    if (q->postpro) {
                        p = (nai_uring_postpro_t*)q->data;
                        p->handle(p);
                    };

                    nai_uring_lock(u);
                    nai_uring_free(u, q->data, q->datalen);
                    nai_uring_unlock(u);
                    q->data = 0;
                };

                /* fill result */
                if (cqe->res >= 0) {
                    q->sult = cqe->res;
                    nai_memory_barrier();
                    q->code = 0;
                } else {
                    q->code = -cqe->res;
                };

                io_uring_cqe_seen(&u->queue, cqe);
            };


            nai_mutex_lock(&g->lock);

            if (r < 0) {
                ec = -r;
                r = -1;
                break;
            } else if (q == a) {
                r = 0;
                break;
            } else {

                if (q->cond) {
                    nai_cond_signal(q->cond);
                };
            };
        };

        if (!nai_list_is_empty(&g->waits)) {
            w = (nai_uring_wait_t*)g->waits.next;
            nai_cond_signal(w->cond);
        };

        g->leader = 0;
        break;
    };

    nai_mutex_unlock(&g->lock);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


static nai_int_t nai_uring_handle_mt(
    nai_aio_t* a, nai_int_t err, size_t bytes)
{
    nai_int_t r;
    nai_uring_t* u;


    (void)err;
    (void)bytes;

    u = nai_containof(a, nai_uring_t, wait);
    nai_uring_wait(u->mt, 0);
    nai_uring_handle(u->mt);

    r = nai_uring_poll(u, &u->wait, u->mt->queue.ring_fd, 1);

    return r;
};


static nai_int_t nai_uring_setup_mt(nai_uring_t* u, nai_int_t max_ents)
{
    nai_int_t r;
    nai_aio_t* a;


    /* create a mt uring */
    u->mt = (nai_uring_t*)nai_malloc(sizeof(*u));
    if (u->mt == 0) {
        r = -1;
        goto _end;
    };

    nai_uring_init(u->mt);
    r = nai_uring_open(u->mt, max_ents, 1);
    if (r < 0) {
        goto _end;
    };

    /* start a poll to wait eventfd */
    a = &u->wait;
    nai_aio_init(a);
    nai_aio_set_cb(a, nai_uring_handle_mt);
    r = nai_uring_poll(u, &u->wait, u->mt->queue.ring_fd, 1);

_end:
    return r;
};


nai_int_t nai_uring_init(nai_uring_t* u)
{
    nai_int_t r;


    nai_bufpool_init(&u->pool);
    nai_memset(&u->queue, 0, sizeof(u->queue));
    u->queue.ring_fd = -1;
    u->lock = 0;
    u->mt = 0;
    u->count = 0;
    u->batch = 0;
    u->max_ents = 0;
    u->cqe = 0;

    r = 0;

    return r;
};


nai_int_t nai_uring_open(nai_uring_t* u, nai_int_t max_ents, nai_int_t mt)
{
    nai_int_t r;
    nai_int_t ec;
    struct io_uring_params p;


    if (u->queue.ring_fd >= 0) {
        r = 0;
        goto _end;
    };

    if (u == &nai_uring_global.ring) {
        r = io_uring_queue_init(max_ents, &u->queue, 0);
    } else {
        if (nai_uring_global.inited == 0) {
            nai_once(&nai_uring_global.once, nai_uring_global_init);
        };
        if (nai_uring_global.error) {
            nai_errno = nai_uring_global.error;
            r = -1;
            goto _end;
        };

        nai_memset(&p, 0, sizeof(p));
        p.flags = IORING_SETUP_ATTACH_WQ;
        p.wq_fd = nai_uring_global.ring.queue.ring_fd;
        r = io_uring_queue_init_params(max_ents, &u->queue, &p);
    };

    if (r < 0) {
        nai_errno = -r;
        r = -1;
        goto _end;
    };

    r = nai_bufpool_open(&u->pool, 2048, 0);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };

    if (!mt) {
        r = nai_uring_setup_mt(u, NAI_URING_QSIZE_MT);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };

        u->batch = max_ents / 2;
    };

    u->max_ents = max_ents;

_end:
    return r;

_fail:
    nai_uring_close(u);
    nai_errno = ec;
    goto _end;
};


nai_int_t nai_uring_close(nai_uring_t* u)
{
    nai_int_t r;


    if (u->mt) {
        nai_uring_close(u->mt);
        nai_free(u->mt);
        u->mt = 0;
    };
    if (u->queue.ring_fd >= 0) {
        io_uring_queue_exit(&u->queue);
        nai_memset(&u->queue, 0, sizeof(u->queue));
        u->queue.ring_fd = -1;
    };


    nai_bufpool_close(&u->pool);
    u->lock = 0;
    u->count = 0;
    u->batch = 0;
    u->max_ents = 0;
    u->cqe = 0;
    r = 0;

    return r;
};


nai_int_t nai_uring_poll(
    nai_uring_t* u, nai_aio_t* a, nai_fd_t fd, nai_int_t once)
{
    nai_int_t r;
    nai_int_t ec;
    struct io_uring_sqe* sqe;


    /* lock */
    nai_uring_lock(u);

    /* get sqe */
    sqe = io_uring_get_sqe(&u->queue);
    if (sqe == 0) {
        nai_uring_unlock(u);

        ec = EBUSY;
        nai_log_alert(NAI_LOG_CORE, 
            ec, "io_uring_get_sqe failed");

        nai_errno = ec;
        r = -1;
        goto _end;
    };

    /* set poll args */
    if (once) {
        io_uring_prep_poll_add(sqe, fd, POLLIN);
    } else {
        io_uring_prep_poll_multishot(sqe, fd, POLLIN);
    };
    io_uring_sqe_set_data(sqe, a);


    /* check batch */
    if (u->batch > 0) {
        u->count ++;
        if (u->count < u->batch) {
            nai_uring_unlock(u);
            r = 0;
            goto _end;
        };
    };

    /* submit */
    r = io_uring_submit(&u->queue);
    u->count = 0;

    /* unlock */
    nai_uring_unlock(u);

    /* check result */
    if (r < 0) {
        ec = -r;
        nai_log_alert(NAI_LOG_CORE, 
            ec, "io_uring_submit failed");

        nai_errno = ec;
        r = -1;
    };

_end:
    return r;
};


nai_int_t nai_uring_submit(nai_uring_t* u)
{
    nai_int_t r = 0;
    nai_int_t ec;


    if (u->batch > 0) {
        /* lock */
        nai_uring_lock(u);

        if (u->count > 0) {
            r = io_uring_submit(&u->queue);
            if (r >= 0) {
                u->count = 0;
            } else {
                ec = -r;
                nai_log_alert(NAI_LOG_CORE, 
                    ec, "io_uring_submit failed");
            };
        };

        /* unlock */
        nai_uring_unlock(u);
    };

    if (r < 0) {
        nai_errno = ec;
        r = -1;
    };

    return r;
};


nai_int_t nai_uring_wait(nai_uring_t* u, uint32_t msec)
{
    nai_int_t r;
    nai_int_t ec;
    struct __kernel_timespec ts, *tp;
    struct io_uring_cqe* cqe;
    struct io_uring_sqe* sqe;


    if (msec == (uint32_t)-1) {
        tp = 0;
    } else {
        ts.tv_sec = (long)(msec / 1000);
        ts.tv_nsec = (long)(msec % 1000 * 1000 * 1000);
        tp = &ts;
    };

    if (u->count > 0) {
        if (tp) {
            sqe = io_uring_get_sqe(&u->queue);
            io_uring_prep_timeout(sqe, tp, 1, 0);
            io_uring_sqe_set_data(sqe, 0);
        };

        r = io_uring_submit_and_wait(&u->queue, 1);
        if (r < 0) {
            ec = -r;
            if (ec == ETIMEDOUT || ec == ETIME || ec == EINTR) {
                r = 0;
            } else {
                ec = -r;
                nai_log_alert(NAI_LOG_CORE, 
                    ec, "io_uring_submit_and_wait failed");

                nai_errno = ec;
                r = -1;
            };
            goto _end;
        };

        u->count = 0;

        /* set timeout 0, already waited */
        ts.tv_sec = 0;
        ts.tv_nsec = 0;
        tp = &ts;
    };

    r = io_uring_wait_cqe_timeout(&u->queue, &cqe, tp);
    if (r < 0) {
        ec = -r;
        if (ec == ETIMEDOUT || ec == ETIME || ec == EINTR) {
            r = 0;
        } else {
            nai_log_alert(NAI_LOG_CORE, 
                ec, "io_uring_wait_cqe_timeout failed");

            nai_errno = ec;
            r = -1;
        };
        goto _end;
    };

    u->cqe = cqe;
    r = 0;

_end:
    return r;
};


nai_int_t nai_uring_handle(nai_uring_t* u)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t batch;
    nai_aio_t* q;
    nai_uring_postpro_t* p;
    struct io_uring_cqe* cqe;


    cqe = u->cqe;
    if (cqe == 0) {
        r = 0;
        goto _end;
    };

    u->cqe = 0;
    batch = nai_uring_is_batch(u);
    for (;;) {
        q = (nai_aio_t*)io_uring_cqe_get_data(cqe);
        if (q == 0) {
            /* a cancel or internal timeout */
            io_uring_cqe_seen(&u->queue, cqe);
        } else {

            /* should update before fill result */
            if ((batch || q->postpro) && q->data) {
                if (q->postpro) {
                    p = (nai_uring_postpro_t*)q->data;
                    p->handle(p);
                };

                nai_uring_lock(u);
                nai_uring_free(u, q->data, q->datalen);
                nai_uring_unlock(u);
                q->data = 0;
            };

            /* fill result */
            if (cqe->res >= 0) {
                q->sult = cqe->res;
                nai_memory_barrier();
                q->code = 0;
            } else {
                q->code = -cqe->res;
            };

            io_uring_cqe_seen(&u->queue, cqe);


            if (q->handle) {
                q->handle(q, q->code, q->sult);
            };
        };

        /* peek next */
        r = io_uring_peek_cqe(&u->queue, &cqe);
        if (r < 0) {
            ec = -r;
            if (ec != EAGAIN) {
                nai_log_crit(NAI_LOG_CORE, -r, "io_uring_peek_cqe failed");
                nai_errno = ec;
                r = -1;
                goto _end;
            };
            break;
        };
    };

    r = 0;

_end:
    return r;
};


#endif


void nai_aio_fork(nai_int_t at)
{
    switch (at) {
    case 2: /* at child */
#if (NAI_AIO_USE_LINUX)
        nai_aio_global_fork();
#endif
#if (NAI_AIO_USE_URING)
        nai_uring_global_fork();
#endif
        break;

    default:
        break;
    };

    return;
};


void nai_aio_term()
{
#if (NAI_AIO_USE_LINUX)
    nai_aio_global_term();
#endif
#if (NAI_AIO_USE_URING)
    nai_uring_global_term();
#endif

    return;
};


