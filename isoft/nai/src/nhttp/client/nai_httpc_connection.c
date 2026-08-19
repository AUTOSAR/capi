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
/// @file       nai_httpc_connection.c
/// @brief      
/// @details
/// @date       2021-08-19
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
#include "nai_httpc_core.h"



static nai_sult_t nai_httpc_connection_free(
    nai_httpc_agent_t* a, nai_httpc_connection_t* c)
{
    nai_int_t reset = 0;
    nai_sult_t rc;
    nai_pool_t pool;


    /* remove from active list */
    assert(a->nlist > 0);
    a->nlist --;
    nai_list_entry_remove(&c->c.ent);


    /* reset connection */
    if (a->nlist + a->nfree + 1 <= a->min_connections || 
        a->nfree < a->min_free_connections) {

        reset = 1;

        /* move pool to stack */
        nai_pool_init(&pool, 0);
        nai_pool_exchange(&pool, c->c.pool);

        /* reset pool */
        rc = nai_pool_reset(&pool);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "nai_pool_reset failed");
            goto _fail;
        };

        c = (nai_httpc_connection_t*)nai_palloc(&pool, sizeof(*c));
        if (c == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "failed to alloc connection for reuse");
            goto _fail;
        };

        c->c.pool = (nai_pool_t*)nai_palloc(&pool, sizeof(*c));
        if (c->c.pool == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "failed to alloc connection pool for reuse");
            goto _fail;
        };

        nai_pool_init(c->c.pool, 0);
        nai_pool_exchange(c->c.pool, &pool);
    };

    /* free connection */
    if (reset && (
        a->nlist + a->nfree + 1 <= a->min_connections || 
        a->nfree < a->min_free_connections)) {

        /* add into free list */
        nai_list_insert_head(&a->cfree, &c->c.ent);
        a->nfree ++;

    } else {

        /* too many conncections, close pool */
        rc = nai_pool_close(c->c.pool);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "nai_pool_close failed");
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;

_fail:
    nai_pool_close(&pool);
    goto _end;
};


nai_sult_t nai_httpc_connection_create(
    nai_httpc_agent_t* a, nai_httpc_server_t* s, 
    nai_httpc_connection_t** pv)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_main_t* m;
    nai_main_event_t* e;
    nai_httpc_t* h;
    nai_httpc_connection_t* c;


    h = nai_httpc_get_service(a);
    if (a->nfree > 0) {
        /* peek a freed connection */
        c = nai_containof(a->cfree.next, nai_httpc_connection_t, c.ent);
        nai_list_entry_remove(&c->c.ent);
        a->nfree --;

        /* initialize connection */
        r = nai_connection_init(&c->c, c->c.pool);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "initialize connection failed");
            goto _fail;
        };

    } else {

        /* create a new connection */
        c = (nai_httpc_connection_t*)nai_connection_new(0, sizeof(*c));
        if (c == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "alloc connection failed");
            goto _end;
        };
    };


    /* init local */
    rc = nai_httpc_local_alloc(h, c->c.pool, &c->local);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc local of connection failed");
        goto _fail;
    };


#if (NAI_HAVE_SSL)
    /* ssl */
    c->ssl = 0;
#endif

    /* set tcp stats */
    c->c.tcp_nopush = 1;

    /* count */
    c->locked = 0;
    c->requests = 0;

    /* request stream */
    c->proto = 0;

    /* time */
    c->lingering_time = 0;

    /* save server */
    c->agent = a;

    /* set default configure */
    c->conf = s->conf;

    /* set server node */
    c->server = 0;
    nai_list_init(&c->ents);

    /* add to event */
    m = h->main;
    e = nai_main_event_get(m);
    rc = nai_main_event_inc_connection(e);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    /* add into active list */
    nai_list_insert_tail(&a->clist, &c->c.ent);
    a->nlist ++;


    /* success */
    pv[0] = c;
    rc = 0;

_end:
    return rc;

_fail:
    nai_pool_close(c->c.pool);
    goto _end;
};


nai_sult_t nai_httpc_connection_close(nai_httpc_connection_t* c)
{
    nai_sult_t rc;
    nai_main_t* m;
    nai_main_event_t* e;
    nai_httpc_t* h;


    if (c->server) {
        nai_list_entry_remove(&c->ents);
        c->server = 0;
    };

    /* turn off linger if already timedout */
    if (c->c.timedout && !c->c.closed) {
        rc = nai_stream_set_opt(&c->c.str, NAI_IO_LINGER, 0);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_errno, "turn off connection linger failed");
        };
    };

    /* close stream */
    rc = nai_stream_close(&c->c.str);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_errno, "close the stream of connection failed");
    };

    /* remove from event */
    h = nai_httpc_get_service(c);
    m = h->main;
    e = nai_main_event_get(m);
    rc = nai_main_event_dec_connection(e);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "decrement connection failed");
    };


    /* free connection pool */
    rc = nai_httpc_connection_free(c->agent, c);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "close the pool of connection failed");
    };

    return 0;
};


