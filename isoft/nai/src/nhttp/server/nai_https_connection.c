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
/// @file       nai_https_connection.c
/// @brief      
/// @details
/// @date       2021-01-25
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_system.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/service/nai_main.h"
#include "nai/service/nai_main_event.h"
#include "nai/service/nai_listening.h"
#include "nai_https_core.h"



static nai_sult_t nai_https_connection_free(
    nai_https_t* h, nai_https_connection_t* c)
{
    nai_sult_t rc;
    nai_pool_t pool;


    /* remove from active list */
    assert(h->nlist > 0);
    h->nlist --;
    nai_list_entry_remove(&c->c.ent);


    /* free connection */
    if (h->nlist + h->nfree + 1 <= h->min_connections || 
        h->nfree < h->min_free_connections) {

        /* move pool to stack */
        nai_pool_init(&pool, 0);
        nai_pool_exchange(&pool, c->c.pool);

        /* reset pool */
        rc = nai_pool_reset(&pool);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "nai_pool_reset failed");
            goto _end;
        };

        c = (nai_https_connection_t*)nai_palloc(&pool, sizeof(*c));
        if (c == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "failed to alloc connection for reuse");
            goto _end;
        };

        c->c.pool = (nai_pool_t*)nai_palloc(&pool, sizeof(*c));
        if (c->c.pool == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "failed to alloc connection pool for reuse");
            goto _end;
        };

        nai_pool_init(c->c.pool, 0);
        nai_pool_exchange(c->c.pool, &pool);

        /* add into free list */
        nai_list_insert_head(&h->cfree, &c->c.ent);
        h->nfree ++;

    } else {
        /* too many conncections, close pool */
        rc = nai_pool_close(c->c.pool);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "nai_pool_close failed");
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_https_connection_create(
    nai_listening_t* ps, nai_fd_t f, const nai_sockname_t* name, 
    nai_https_connection_t** pv)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_listening_t* pm;
    nai_main_t* m;
    nai_main_event_t* e;
    nai_https_t* h;
    nai_https_server_t* s;
    nai_https_listening_t* l;
    nai_https_connection_t* c;
    nai_socknbuf_t sockname;


    l = (nai_https_listening_t*)ps->key;
    s = l->default_serv;
    h = nai_https_get_service(s);


    if (h->nfree > 0) {
        /* peek a freed connection */
        c = nai_containof(h->cfree.next, nai_https_connection_t, c.ent);
        nai_list_entry_remove(&c->c.ent);
        h->nfree --;

        r = nai_connection_init(&c->c, c->c.pool);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "initialize connection failed");
            goto _fail;
        };

    } else {
        /* create a new connection */
        c = (nai_https_connection_t*)nai_connection_new(0, sizeof(*c));
        if (c == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "alloc connection failed");
            goto _end;
        };
    };


    /* init local */
    rc = nai_https_local_alloc(h, c->c.pool, &c->local);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc local of connection failed");
        goto _fail;
    };


    /* remap the listening */
    pm = nai_listening_remap(ps, f, &sockname);
    if (pm == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "lookup the listening failed");
        goto _fail;
    } else if (pm != ps) {
        l = (nai_https_listening_t*)pm->key;
        s = l->default_serv;
    };

    /* save peer address */
    r = nai_connection_set_addr(
        &c->c, NAI_CONN_PEERNAME, name->addr, name->len);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "set connection peer address failed");
        goto _fail;
    };

    /* save sock address */
    if (sockname.len > 0) {
        r = nai_connection_set_addr(
            &c->c, NAI_CONN_SOCKNAME, &sockname.addr, sockname.len);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "set connection sock address failed");
            goto _fail;
        };
    };


#if (NAI_HAVE_SSL)
    /* ssl */
    c->ssl = 0;
    c->ssl_servername = 0;
#endif

    /* set tcp stats */
    c->c.tcp_nopush = 1;

    /* set connection type */
    switch (name->addr->sa_family) {
    case AF_INET:
#if defined(AF_INET6)
    case AF_INET6:
#endif
        c->c.tcp = 1;
        break;
    default:
        break;
    };

    /* count */
    c->locked = 0;
    c->requests = 0;

    /* request stream */
    c->proto = 0;

    /* time */
    c->lingering_time = 0;
    c->start_msec = nai_tickcache_to_msec();

    /* save listening */
    c->ls = ps;

    /* save listening configure */
    c->lc = l;

    /* set default configure */
    c->conf = s->conf;


    /* add to event */
    m = h->main;
    e = nai_main_event_get(m);
    rc = nai_main_event_inc_connection(e);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };


    /* init stream */
    nai_stream_set_fd(&c->c.str, f, NAI_FD_TYPE_SOCK);
    nai_stream_set_fdown(&c->c.str, 1);
    nai_stream_set_opt(&c->c.str, NAI_IO_RECVTIMEO, 
        (nai_int_t)s->read_head_timeo);
    nai_stream_inherit(&c->c.str, &ps->l);


    /* add into active list */
    nai_list_insert_tail(&h->clist, &c->c.ent);
    h->nlist ++;


    /* success */
    pv[0] = c;
    rc = 0;

_end:
    return rc;

_fail:
    nai_pool_close(c->c.pool);
    goto _end;
};


nai_sult_t nai_https_connection_close(nai_https_connection_t* c)
{
    nai_sult_t rc;
    nai_main_t* m;
    nai_main_event_t* e;
    nai_https_t* h;
    nai_https_protocol_t* p;


    /* turn off linger if already timedout */
    if (c->c.timedout && !c->c.closed) {
        rc = nai_stream_set_opt(&c->c.str, NAI_IO_LINGER, 0);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_errno, "turn off connection linger failed");
        };
    };

    /* close stream */
    rc = nai_stream_close(&c->c.str);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_errno, "close the stream of connection failed");
    };

    p = c->proto;
    if (p != 0) {
        nai_buflist_close(&p->send_list);
    };

    /* remove from event */
    h = nai_https_get_service(c);
    m = h->main;
    e = nai_main_event_get(m);
    rc = nai_main_event_dec_connection(e);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "decrement connection failed");
    };


    /* free connection pool */
    rc = nai_https_connection_free(h, c);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "close the pool of connection failed");
    };

    return 0;
};



