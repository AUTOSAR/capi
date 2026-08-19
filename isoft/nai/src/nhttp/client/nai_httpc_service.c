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
/// @file       nai_httpc_service.c
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_uri.h"
#include "nai/service/nai_main.h"
#include "nai_httpc_core.h"


static nai_sult_t nai_httpc_local_init(
    nai_httpc_t* h, nai_local_t* l, size_t offset, 
    nai_pool_t* p)
{
    nai_int_t n;
    void* data;
    nai_sult_t rc;
    nai_main_t* m = h->main;
    nai_module_t* e;
    nai_httpc_module_t * c;
    nai_httpc_local_alloc_f alloc;
    nai_httpc_local_ctx_t ctx;
    nai_httpc_conf_t cf = {0};


    l->slot[0] = nai_main_slot(m);

    if (p == 0) {
        p = &h->main->pool;
    };

    ctx.flags = 0;
    ctx.service = h;;
    ctx.conf = cf;
    ctx.pool = p;
    ctx.pool_tmp = 0;
    ctx.done = 1;
    for (n = 0; n < (intptr_t)nai_module_map_count(&m->modules); n ++) {
        e = nai_module_map_at(&m->modules, n);
        if (e->grp != NAI_MODULE_HTTPC) {
            continue;
        };

        c = (nai_httpc_module_t*)e->ctx;
        if (c == 0) {
            nai_local_at(l, *e) = 0;
            continue;
        };

        alloc = *(nai_httpc_local_alloc_f*)((uint8_t*)c + offset);
        if (alloc == 0) {
            nai_local_at(l, *e) = 0;
            continue;
        };

        rc = alloc(&ctx, &data);
        if (rc < 0) {
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "httpc modules '%s' alloc local data failed", e->name);
            goto _end;
        };

        nai_local_at(l, *e) = data;
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_local_merge(
    nai_httpc_t* h, nai_httpc_conf_t* cf, nai_local_t* l, 
    nai_local_t* u, size_t offset, nai_int_t done, 
    nai_pool_t* p, nai_pool_t* pt)
{
    nai_int_t n;
    void* data_l;
    void* data_u;
    nai_sult_t rc;
    nai_main_t* m = h->main;
    nai_module_t* e;
    nai_httpc_module_t * c;
    nai_httpc_local_merge_f merge;
    nai_httpc_local_ctx_t ctx;


    if (p == 0) {
        p = &h->main->pool;
    };
    if (pt == 0) {
        pt = nai_main_get_temp_pool(h->main);
    };

    ctx.flags = 0;
    ctx.service = h;
    ctx.conf = *cf;
    ctx.pool = p;
    ctx.pool_tmp = pt;
    ctx.done = done;
    for (n = 0; n < (intptr_t)nai_module_map_count(&m->modules); n ++) {
        e = nai_module_map_at(&m->modules, n);
        if (e->grp != NAI_MODULE_HTTPC) {
            continue;
        };

        c = (nai_httpc_module_t*)e->ctx;
        if (c == 0) {
            continue;
        };

        merge = *(nai_httpc_local_merge_f*)((uint8_t*)c + offset);
        if (merge == 0) {
            continue;
        };

        if (u == 0) {
            data_u = 0;
        } else {
            data_u = nai_local_at(u, *e);
            if (data_u == 0) {
                rc = NAI_E_INTERNAL;
                nai_log_alert(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), 
                    "get local of httpc modules '%s' failed", e->name);
                goto _end;
            };
        };

        data_l = nai_local_at(l, *e);
        if (data_l == 0) {
            rc = NAI_E_INTERNAL;
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "get local of httpc modules '%s' failed", e->name);
            goto _end;
        };

        rc = merge(&ctx, data_l, data_u);
        if (rc < 0) {
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "httpc modules '%s' combine local data failed", e->name);
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_httpc_local_alloc(
    nai_httpc_t* h, nai_pool_t* p, nai_local_t** pv)
{
    nai_sult_t rc;
    nai_local_t* local;


    local = (nai_local_t*)nai_palloc(p, 
        sizeof(*local) + sizeof(void*)*h->max_modules);
    if (local == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    local->slot[0] = nai_main_slot(h->main);
    nai_pzero(local->data + 1, h->max_modules);

    pv[0] = local;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_httpc_service_create(
    nai_main_t* m, nai_pool_t* p, nai_httpc_t** pv)
{
    nai_int_t n;
    nai_int_t modules;
    nai_sult_t rc;
    nai_httpc_t* h;
    nai_httpc_stage_group_t* g;


    modules = nai_module_map_count_grp(
        &m->modules, NAI_MODULE_HTTPC);

    h = nai_palloc(p, sizeof(*h) + sizeof(void*)*modules);
    if (h == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc nai_httpc_t failed");
        goto _end;
    };

    h->main = m;
    rc = nai_httpc_local_init(h, &h->local, 
        nai_offsetof(nai_httpc_module_t, module_alloc), p);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "init local of nai_httpc_t failed");
        goto _end;
    };

    nai_local_at(&h->local, nai_httpc_core_module) = h;
    nai_pool_init(&h->pool, 1024);
    nai_list_init(&h->named);
    nai_list_init(&h->anony);
    nai_chain_init(&h->proto_in, "proto", 0, 0);
    nai_chain_init(&h->proto_out, "proto", 0, 0);
    nai_chain_init(&h->trans_in, "trans", 0, 0);
    nai_chain_init(&h->trans_out, "trans", 0, 0);
    nai_hashsnap_init(&h->headers_in, p);
    nai_hashsnap_init(&h->headers_out, p);
    nai_script_vartable_init(&h->vars, &m->pool);
    h->ncapture = 0;
    h->max_modules = modules;
    h->prepared = 0;


    for (n = 0; n < (nai_int_t)nai_countof(h->groups); n ++) {
        g = &h->groups[n];
        g->start_index = 0;
        nai_array_init(&g->handles, 
            sizeof(nai_httpc_stage_entry_t), p);
    };

    pv[0] = h;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_httpc_agent_create(
    nai_httpc_t* h, nai_pool_t* p, nai_httpc_agent_t** pv)
{
    nai_sult_t rc;
    nai_httpc_agent_t* a;
    nai_httpc_server_t** ps;
    nai_str_t name;


    a = nai_palloc(p, sizeof(*a) + sizeof(void*) * h->max_modules);
    if (a == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc nai_httpc_agent_t failed");
        goto _end;
    };

    rc = nai_httpc_local_init(h, &a->local, 
        nai_offsetof(nai_httpc_module_t, server_alloc), p);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_errno_to_sult(rc), "init local of nai_httpc_server_t failed");
        goto _end;
    };

    nai_local_at(&a->local, nai_httpc_core_module) = a;
    nai_array_init(&a->servers, sizeof(nai_httpc_server_t*), p);
    nai_array_init(&a->named, sizeof(nai_httpc_server_name_t*), p);
    nai_array_init(&a->match, sizeof(nai_httpc_server_name_t*), p);
    nai_list_init(&a->ent);
    nai_list_init(&a->clist);
    nai_list_init(&a->cfree);
    nai_rbtree_init(&a->cmap);
    a->nlist = 0;
    a->nfree = 0;
    a->min_connections = NAI_OPT_UNSET_VALUE;
    a->min_free_connections = NAI_OPT_UNSET_VALUE;
    a->conf.main = &h->local;
    a->conf.agent = &a->local;


    ps = (nai_httpc_server_t**)nai_array_push(&a->servers);
    if (ps == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "push default server failed");
        goto _end;
    };

    rc = nai_httpc_server_create(h, p, a, ps);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "create default server failed");
        goto _end;
    };

    nai_str_setc(&name, "default");
    a->conf.server = ps[0]->conf.server;
    a->conf.location = ps[0]->conf.location;
    ps[0]->name = name;
    pv[0] = a;
    rc = 0;

_end:
    return rc;
};



nai_sult_t nai_httpc_server_create(
    nai_httpc_t* h, nai_pool_t* p, nai_httpc_agent_t* a, 
    nai_httpc_server_t** pv)
{
    nai_sult_t rc;
    nai_httpc_server_t* s;
    nai_httpc_location_t** pl;
    nai_str_t name;


    s = nai_palloc(p, sizeof(*s) + sizeof(void*) * h->max_modules);
    if (s == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc nai_httpc_server_t failed");
        goto _end;
    };

    rc = nai_httpc_local_init(h, &s->local, 
        nai_offsetof(nai_httpc_module_t, server_alloc), p);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_errno_to_sult(rc), "init local of nai_httpc_server_t failed");
        goto _end;
    };

    nai_local_at(&s->local, nai_httpc_core_module) = s;
    nai_str_setn(&s->name);
    nai_array_init(&s->names, sizeof(nai_httpc_server_name_t), p);
    nai_array_init(&s->table.list, sizeof(nai_httpc_location_t*), p);
    nai_array_init(&s->table.patterns, 
        sizeof(nai_httpc_location_entry_t*), p);
    nai_rbtree_init(&s->table.named);
    nai_rbtree_init(&s->table.symbols);
    nai_conn_attr_init(&s->conn);
    s->version = NAI_OPT_UNSET_VALUE;
    s->max_requests = NAI_OPT_UNSET_VALUE;
    s->max_header_buffers.num = NAI_OPT_UNSET_VALUE;
    s->max_header_buffers.size = NAI_OPT_UNSET_VALUE;
    s->min_header_buffer = NAI_OPT_UNSET_VALUE;
    s->max_headers = NAI_OPT_UNSET_VALUE;
    s->send_so_bufsize = NAI_OPT_UNSET_VALUE;
    s->read_so_bufsize = NAI_OPT_UNSET_VALUE;
    s->conf.main = &h->local;
    s->conf.agent = &a->local;
    s->conf.server = &s->local;


    pl = (nai_httpc_location_t**)nai_array_push(&s->table.list);
    if (pl == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "push default location failed");
        goto _end;
    };

    rc = nai_httpc_location_create(h, p, &s->conf, pl);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "create default location failed");
        goto _end;
    };

    nai_str_setc(&name, "root");
    s->conf.location = &pl[0]->local;
    pl[0]->name = name;
    pl[0]->type = NAI_HTTPC_LOC_ROOT;
    pv[0] = s;
    rc = 0;

_end:
    return rc;
};


 nai_sult_t nai_httpc_location_create(
    nai_httpc_t* h, nai_pool_t* p, nai_httpc_conf_t* c, 
    nai_httpc_location_t** pv)
{
    nai_sult_t rc;
    nai_httpc_location_t* l;


    /* alloc location */
    l = nai_palloc(p, sizeof(*l) + sizeof(void*) * h->max_modules);
    if (l == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc nai_httpc_location_t failed");
        goto _end;
    };

    /* init location's local */
    rc = nai_httpc_local_init(h, &l->local, 
        nai_offsetof(nai_httpc_module_t, location_alloc), p);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "init local of nai_httpc_location_t failed");
        goto _end;
    };

    /* init location */
    nai_local_at(&l->local, nai_httpc_core_module) = l;
    nai_str_setn(&l->name);
    nai_script_expn_init(&l->user_agent);
    nai_script_expn_init(&l->accept);
    nai_script_expn_init(&l->accept_language);
    nai_script_expn_init(&l->accept_encoding);
    l->type = NAI_HTTPC_LOC_NONAME;
    l->limit_except = NAI_OPT_UNSET_VALUE;
    l->limit_conf = 0;
    l->max_body_size = NAI_OPT_UNSET_VALUE;
    l->directio = NAI_OPT_UNSET_VALUE;
    l->directio_align = NAI_OPT_UNSET_VALUE;
    l->read_chunk_size = NAI_OPT_UNSET_VALUE;
    l->send_chunk_size = NAI_OPT_UNSET_VALUE;
    l->send_lowat = NAI_OPT_UNSET_VALUE;
    l->send_timeo = NAI_OPT_UNSET_VALUE;
    l->connect_timeo = NAI_OPT_UNSET_VALUE;
    l->read_head_timeo = NAI_OPT_UNSET_VALUE;
    l->read_body_timeo = NAI_OPT_UNSET_VALUE;
    l->keepalive_timeo = NAI_OPT_UNSET_VALUE;
    l->lingering_timeo = NAI_OPT_UNSET_VALUE;
    l->lingering_timeomax = NAI_OPT_UNSET_VALUE;
    l->retry_max = NAI_OPT_UNSET_VALUE;
    l->keepalive_max = NAI_OPT_UNSET_VALUE;
    l->lingering_close = NAI_OPT_UNSET_VALUE;
    l->sendfile = NAI_OPT_UNSET_VALUE;
    l->aio = NAI_OPT_UNSET_VALUE;
    l->tcp_nopush = NAI_OPT_UNSET_VALUE;
    l->tcp_nodelay = NAI_OPT_UNSET_VALUE;
    l->etag = NAI_OPT_UNSET_VALUE;
    l->chunked = NAI_OPT_UNSET_VALUE;
    l->satisfy = NAI_OPT_UNSET_VALUE;
    l->internal = 0;
    l->parent = 0;
    l->sibling = 0;
    l->conf.main = c->main;
    l->conf.server = c->server;
    l->conf.location = &l->local;

    /* return */
    pv[0] = l;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_location_tree_merge(nai_rbtree_t* t)
{
    nai_sult_t rc;
    nai_rbnode_str_t* e;
    nai_rbnode_str_t* end;
    nai_httpc_location_t* l;
    nai_httpc_location_node_t* n;


    e = nai_rbtree_str_begin(t);
    end = nai_rbtree_str_end(t);
    while (e != end) {
        n = (nai_httpc_location_node_t*)e;
        if (n->exact) {
            l = n->exact;
            rc = nai_httpc_location_merge(l, l->parent, 0, 0);
            if (rc < 0) {
                nai_log_alert(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), 
                    "combine exact location of '%s' failed", 
                    nai_str(&l->name));
                goto _end;
            };
        };
        if (n->prefix) {
            l = n->prefix;
            rc = nai_httpc_location_merge(l, l->parent, 0, 0);
            if (rc < 0) {
                nai_log_alert(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), 
                    "combine prefix location of '%s' failed", 
                    nai_str(&l->name));
                goto _end;
            };
        };

        rc = nai_httpc_location_tree_merge(&n->sub);
        if (rc < 0) {
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_location_tree_build(nai_rbtree_t* t)
{
    nai_rbnode_str_t* e;
    nai_rbnode_str_t* end;
    nai_httpc_location_node_t* n;
    nai_httpc_location_node_t* c;


    e = nai_rbtree_str_begin(t);
    end = nai_rbtree_str_end(t);
    if (e == end) {
        goto _end;
    };

    /* combile sub tree */
    n = (nai_httpc_location_node_t*)e;
    e = nai_rbtree_str_next(e);
    while (e != end) {
        c = (nai_httpc_location_node_t*)e;
        e = nai_rbtree_str_next(e);
        if (nai_str_len(&n->ent.key) >= nai_str_len(&c->ent.key) || 
            nai_strncmp(nai_str(&n->ent.key), nai_str(&c->ent.key), 
            nai_str_len(&n->ent.key)) != 0) {
            nai_httpc_location_tree_build(&n->sub);
            n = c;
        } else {
            nai_rbtree_erase(t, &c->ent.ent);
            c->ent.key.ptr += nai_str_len(&n->ent.key);
            c->ent.key.len -= nai_str_len(&n->ent.key);
            nai_rbtree_str_insert(&n->sub, &c->ent, 0);
        };
    };

_end:
    return 0;
};


static nai_sult_t nai_httpc_locations_build(
    nai_httpc_t* h, nai_httpc_locations_t* t)
{
    nai_int_t i;
    nai_sult_t rc;
    nai_main_t* m = h->main;
    nai_pool_t* pool = &m->pool;
    nai_pool_t* pool_tmp = nai_main_get_temp_pool(m);
#if (NAI_HAVE_REGEX)
    nai_regex_errinfo_t err;
#endif
    nai_httpc_location_t* l;
    nai_httpc_location_t** pl;
    nai_httpc_location_node_t* n;
    nai_httpc_location_entry_t* e;


    /* scan all locations */
    pl = (nai_httpc_location_t**)t->list.elts;
    for (i = 1; i < (intptr_t)t->list.count; i ++) {
        l = pl[i];
        switch (l->type) {
        case NAI_HTTPC_LOC_EXACT:
        case NAI_HTTPC_LOC_NORMAL:
        case NAI_HTTPC_LOC_PRIORITY:
            n = (nai_httpc_location_node_t*)
                nai_rbtree_str_find(&t->named, &l->name, 0);
            if (n == 0) {
                n = (nai_httpc_location_node_t*)nai_palloc(pool, sizeof(*n));
                if (n == 0) {
                    rc = nai_sult_from_errno();
                    nai_log_alert(NAI_LOG_HTTPC, 
                        nai_sult_to_errno(rc), "alloc location entry failed");
                    goto _end;
                };

                n->ent.key = l->name;
                n->exact = 0;
                n->prefix = 0;
                nai_rbtree_init(&n->sub);
                nai_rbtree_str_insert(&t->named, &n->ent, 0);
            };
            if (l->type == NAI_HTTPC_LOC_EXACT) {
                n->exact = l;
            } else {
                n->prefix = l;
            };
            continue;

        default:
            break;
        };

        rc = nai_httpc_location_merge(l, l->parent, pool, pool_tmp);
        if (rc < 0) {
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "combine location '%s' with parent '%s' failed", 
                nai_str(&l->name), nai_str(&l->parent->name));
            goto _end;
        };

        switch (l->type) {
#if (NAI_HAVE_REGEX)
        case NAI_HTTPC_LOC_PATTERN:
        case NAI_HTTPC_LOC_PATTERN_CASE:
            e = (nai_httpc_location_entry_t*)nai_palloc(pool, sizeof(*e));
            if (e == 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), "alloc location entry failed");
                goto _end;
            };

            e->location = l;
            rc = nai_httpc_regex_compile(h, &e->pattern, &l->name, 
                l->type == NAI_HTTPC_LOC_PATTERN ? NAI_REGEX_CASELESS:0 | 
                NAI_REGEX_JIT, &err);
            if (rc < 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), 
                    "compile location pattern %s: %s%s%s", 
                    nai_str(&l->name), err.str, 
                    err.offset >= 0 ? ", at " : "", 
                    err.offset >= 0 ? nai_str(&l->name) + err.offset : "");
                goto _end;
            };
            break;
#endif
        case NAI_HTTPC_LOC_SYMBOL:
            e = (nai_httpc_location_entry_t*)nai_palloc(pool, sizeof(*e));
            if (e == 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), "alloc location entry failed");
                goto _end;
            };

            e->location = l;
            e->ent.key = l->name;
            nai_rbtree_str_insert(&t->symbols, &e->ent, 0);
            break;

        default:
            break;
        };
    };

    rc = nai_httpc_location_tree_build(&t->named);
    if (rc < 0) {
        goto _end;
    };

    rc = nai_httpc_location_tree_merge(&t->named);
    if (rc < 0) {
        goto _end;
    };

_end:
    return rc;
};


nai_sult_t nai_httpc_agent_merge(
    nai_httpc_agent_t* a, nai_httpc_agent_t* up, 
    nai_pool_t* p, nai_pool_t* pt)
{
    nai_int_t n;
    nai_sult_t rc;
    nai_httpc_t* h;
    nai_httpc_server_t* s;
    nai_httpc_server_t** sa;


    h = nai_httpc_get_service(a);
    s = nai_httpc_get_server(a);
    if (up == 0) {
        rc = nai_httpc_local_merge(h, 
            &a->conf, a->conf.agent, 0, 
            nai_offsetof(nai_httpc_module_t, agent_merge), 1, p, pt);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "init agent '%s' failed", nai_str(&a->name));
            goto _end;
        };

        rc = nai_httpc_server_merge(s, 0, p, pt);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "init agent '%s' server failed", nai_str(&a->name));
            goto _end;
        };

    } else {
        rc = nai_httpc_local_merge(h, 
            &a->conf, a->conf.server, up->conf.server, 
            nai_offsetof(nai_httpc_module_t, server_merge), 1, p, pt);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "combine agent '%s' failed", nai_str(&a->name));
            goto _end;
        };

        rc = nai_httpc_server_merge(s, nai_httpc_get_server(up), p, pt);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "combine agent '%s' server failed", nai_str(&a->name));
            goto _end;
        };
    };

    sa = (nai_httpc_server_t**)a->servers.elts;
    for (n = 1; n < (intptr_t)a->servers.count; n ++) {
        rc = nai_httpc_server_merge(sa[n], s, p, pt);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "combine server '%s' failed", nai_str(&s->name));
            goto _end;
        };
    };


_end:
    return rc;
};


nai_sult_t nai_httpc_server_merge(
    nai_httpc_server_t* s, nai_httpc_server_t* up, 
    nai_pool_t* p, nai_pool_t* pt)
{
    nai_sult_t rc;
    nai_httpc_t* h;


    h = nai_httpc_get_service(s);

    if (up == 0) {
        rc = nai_httpc_local_merge(h, 
            &s->conf, s->conf.server, 0, 
            nai_offsetof(nai_httpc_module_t, server_merge), 1, p, pt);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "init server '%s' failed", nai_str(&s->name));
            goto _end;
        };

        rc = nai_httpc_local_merge(h, 
            &s->conf, s->conf.location, 0, 
            nai_offsetof(nai_httpc_module_t, location_merge), 1, p, pt);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "init server '%s' location failed", nai_str(&s->name));
            goto _end;
        };

    } else {
        rc = nai_httpc_local_merge(h, 
            &s->conf, s->conf.server, up->conf.server, 
            nai_offsetof(nai_httpc_module_t, server_merge), 1, p, pt);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "combine server '%s' failed", nai_str(&s->name));
            goto _end;
        };

        rc = nai_httpc_local_merge(h, 
            &s->conf, s->conf.location, up->conf.location, 
            nai_offsetof(nai_httpc_module_t, location_merge), 1, p, pt);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "combine server '%s' location failed", nai_str(&s->name));
            goto _end;
        };
    };

    rc = nai_httpc_locations_build(h, &s->table);
    if (rc < 0) {
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "build server '%s' locations failed", nai_str(&s->name));
        goto _end;
    };

_end:
    return rc;
};


nai_sult_t nai_httpc_location_merge(
    nai_httpc_location_t* s, nai_httpc_location_t* up, 
    nai_pool_t* p, nai_pool_t* pt)
{
    nai_sult_t rc;
    nai_httpc_t* h;
    nai_httpc_location_t* sibling;


    h = nai_httpc_get_service(s);

    rc = nai_httpc_local_merge(h, 
        &s->conf, &s->local, up ? &up->local : 0, 
        nai_offsetof(nai_httpc_module_t, location_merge), 1, p, pt);
    if (rc < 0) {
        if (up == 0) {
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "init location '%s' failed", 
                nai_str(&s->name));
        } else {
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "combine location '%s' and '%s' failed", 
                nai_str(&s->name), nai_str(&up->name));
        };
        goto _end;
    };

    if (s->type != NAI_HTTPC_LOC_NONAME) {
        sibling = s->sibling;
        while (sibling) {
            rc = nai_httpc_local_merge(h, 
                &sibling->conf, &sibling->local, &s->local, 
                nai_offsetof(nai_httpc_module_t, location_merge), 1, p, pt);
            if (rc < 0) {
                nai_log_alert(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), 
                    "combine child '%s' of location '%s' failed", 
                    nai_str(&sibling->name), nai_str(&s->name));
                goto _end;
            };
        };
    };

    rc = 0;

_end:
    return rc;
};


