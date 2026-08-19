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
/// @file       nai_main.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/service/nai_main.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"
#include "nai/os/nai_file.h"
#include "nai_main_conf.h"


static nai_int_t nai_main_conf_end(nai_main_t* m, nai_int_t force)
{
    nai_int_t r;
    nai_pool_t pool;
    nai_conf_t* c;


    c = m->conf;
    if (c == 0) {
        r = 0;
        goto _end;
    };

    if (c->stack != 0) {
        nai_log_error(NAI_LOG_CORE, EPERM, 
            "segment '%s' is not end", c->segs[c->stack]->name);

        if (force == 0) {
            nai_errno = EPERM;
            r = -1;
            goto _end;
        };
    };

    /* free pool */
    nai_pool_init(&pool, 0);
    nai_pool_exchange(&pool, &c->pool);
    nai_pool_close(&pool);

    /* clear */
    m->conf = 0;
    r = 0;

_end:
    return r;
};


extern nai_int_t nai_modules_max;
extern nai_module_t* nai_modules[];


nai_int_t nai_main_module_init(nai_main_t* m)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t ec;
    nai_int_t alloc;


    if (m->inited) {
        r = 0;
        goto _end;
    };

    alloc = nai_modules_max + 1;

    if (m->local == 0) {
        /* alloc local data */
        m->local = (nai_local_t*)nai_palloc(&m->pool, sizeof(void*) * alloc);
        if (m->local == 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, 
                "alloc local failed, size %d", alloc);

            r = -1;
            goto _end;
        };
        /* fill null-pointer */
        nai_pzero(m->local->data, alloc);
    };


    if (m->local->slot[0] == 0) {
        /* alloc local slot */
        m->local->slot[0] = (nai_slot_t*)
            nai_palloc(&m->pool, sizeof(nai_slot_t) * alloc);
        if (m->local->slot[0] == 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, 
                "alloc local slot failed, size %d", alloc);

            r = -1;
            goto _end;
        };

        nai_memset(m->local->slot[0], 0, sizeof(nai_slot_t) * alloc);
    };


    r = nai_module_map_open(&m->modules, m);
    if (r < 0) {
        goto _end;
    };

    /* load static modules */
    for (n = 0; nai_modules[n]; n ++) {
        r = nai_module_map_push(&m->modules, nai_modules[n]);
        if (r < 0) {
            ec = nai_errno;
            nai_log_alert(NAI_LOG_CORE, 
                ec, "add static module failed");
            goto _fail;
        };
    };


    /* init static modules */
    nai_log_debug(NAI_LOG_CORE, 0, 
        "init with %d static modules", (nai_int_t)m->modules.a.count);


    r = nai_module_map_on(&m->modules, NAI_MODULE_INIT);
    if (r < 0) {
        ec = nai_errno;
        nai_log_alert(NAI_LOG_CORE, ec, "init modules failed");
        goto _fail;
    };

    /* success */
    nai_log_debug(NAI_LOG_CORE, 0, "init success");
    m->inited = 1;

_end:
    return r;

_fail:
    nai_module_map_close(&m->modules);
    nai_errno = ec;
    goto _end;
};


nai_int_t nai_main_init(nai_main_t* m)
{
    nai_int_t r;


    /* initialize struct */
    nai_pool_init(&m->pool, 0);
    nai_module_map_init(&m->modules);
    nai_str_setc(&m->root_path, "/");
    nai_str_setc(&m->conf_path, "/");
    m->local = 0;
    m->conf = 0;
    m->inited = 0;
    m->prepare = 0;
    m->start = 0;
    r = 0;

    return r;
};


nai_int_t nai_main_set_root(nai_main_t* m, 
    const char* root, const char* conf)
{
    intptr_t r;
    intptr_t len;
    char* buf;
    nai_str_t root_path;
    nai_str_t conf_path;


    buf = 0;
    len = 0;
    for (;;) {
        if (root == 0) {
            r = nai_path_get_cwd(buf, len);
        } else {
            r = nai_path_get_real(buf, len, root);
        };
        if (r < 0) {
            goto _end;
        };
        if (r < len) {
            break;
        };

        len = r + 2;
        buf = (char*)nai_palloc(&m->pool, len);
        if (buf == 0) {
            r = -1;
            goto _end;
        };
    };

    if (r <= 0 || buf[r-1] != nai_path_sep) {
        buf[r] = nai_path_sep;
        r ++;
    };
    nai_str_setm(&root_path, buf, r);

    if (conf == 0) {
        conf_path = root_path;
    } else {
        buf = 0;
        len = 0;
        for (;;) {
            r = nai_path_get_real(buf, len, conf);
            if (r < 0) {
                goto _end;
            };
            if (r < len) {
                break;
            };

            len = r + 2;
            buf = (char*)nai_palloc(&m->pool, len);
            if (buf == 0) {
                r = -1;
                goto _end;
            };
        };

        if (r <= 0 || buf[r-1] != nai_path_sep) {
            buf[r] = nai_path_sep;
            r ++;
        };
        nai_str_setm(&conf_path, buf, r);
    };

    m->root_path = root_path;
    m->conf_path = conf_path;
    r = 0;

_end:
    return (nai_int_t)r;
};


nai_int_t nai_main_add_module(nai_main_t* m, nai_module_t* mod)
{
    nai_int_t r;


    if (m->inited == 0) {
        r = nai_main_module_init(m);
        if (r < 0) {
            goto _end;
        };
    };

    r = nai_module_map_push(&m->modules, mod);

_end:
    return r;
};


nai_int_t nai_main_open(nai_main_t* m)
{
    nai_int_t r;
    nai_conf_t* c;


    c = m->conf;
    if (c != 0 && c->stack != 0) {
        nai_log_error(NAI_LOG_CORE, EPERM, 
            "segment '%s' is not end", c->segs[c->stack]->name);

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    if (m->inited == 0) {
        r = nai_main_module_init(m);
        if (r < 0) {
            goto _end;
        };
    };

    if (m->prepare == 0) {
        /* prepare all modules */
        r = nai_module_map_on(&m->modules, NAI_MODULE_PREPARE);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, "prepare modules failed");
            goto _end;
        };

        /* mark prepared */
        m->prepare = 1;
    };

    if (m->prepare == 1) {
        /* prepare all modules */
        r = nai_module_map_on(&m->modules, NAI_MODULE_PREPARE);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, "prepare modules failed");
            goto _end;
        };

        /* mark prepared second */
        m->prepare = 2;
    };

    /* end configure */
    r = nai_main_conf_end(m, 0);
    if (r < 0) {
        goto _end;
    };

    if (m->start == 0) {
        /* start all modules */
        r = nai_module_map_on(&m->modules, NAI_MODULE_START);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, "start modules failed");
            goto _end;
        };

        /* mark starting */
        m->start = 1;
    };

    /* success */
    nai_log_debug(NAI_LOG_CORE, 0, "main started");
    r = 0;

_end:
    return r;
};


nai_int_t nai_main_reuse(nai_main_t* m)
{
    nai_int_t r;


    if (m->start) {

        /* stop all modules */
        r = nai_module_map_on(&m->modules, NAI_MODULE_STOP);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, "stop modules failed");
            goto _end;
        };

        m->start = 0;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_main_close(nai_main_t* m)
{
    nai_int_t r;


    /* end configure */
    r = nai_main_conf_end(m, 1);
    if (r < 0) {
        goto _end;
    };

    if (m->start) {

        /* stop all modules */
        r = nai_module_map_on(&m->modules, NAI_MODULE_STOP);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, "stop modules failed");
            goto _end;
        };

        m->start = 0;
    };

    /* cleanup all modules */
    r = nai_module_map_on(&m->modules, NAI_MODULE_CLEANUP);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, nai_errno, "unload modules failed");
        goto _end;
    };

    /* free */
    nai_module_map_close(&m->modules);
    nai_pool_close(&m->pool);


    /* success */
    nai_log_debug(NAI_LOG_CORE, 0, "main closed");
    r = 0;

_end:
    return r;
};


intptr_t nai_main_get_full_path(nai_main_t* m, 
    nai_str_t* s, const char* path, size_t len, nai_pool_t* pool)
{
    intptr_t r;
    intptr_t alloc;
    char* p;
    char* buf;
    nai_str_t* root;


    if ((intptr_t)len < 0) {
        len = nai_strlen(path);
    };

    if (nai_path_is_absolute(path, len)) {
        nai_str_setm(s, path, len);
        r = len;
        goto _end;
    };

    root = &m->root_path;
    alloc = nai_str_len(root);
    alloc += len + 1;
    buf = (char*)(pool ? nai_palloc(pool, alloc) : nai_malloc(alloc));
    if (buf == 0) {
        r = -1;
        goto _end;
    };

    p = buf;
    nai_memcpy(p, nai_str(root), nai_str_len(root));
    p += nai_str_len(root);

    nai_memcpy(p, path, len);
    p += len;
    p[0] = 0;

    r = p - buf;
    nai_str_setm(s, buf, r);


_end:
    return r;
};


intptr_t nai_main_copy_full_path(nai_main_t* m, 
    char* buf, size_t buflen, const char* path, size_t len)
{
    intptr_t r;
    char* p;
    nai_str_t* root;


    if ((intptr_t)len < 0) {
        len = nai_strlen(path);
    };

    if (nai_path_is_absolute(path, len)) {
        r = len;
        if (r > (intptr_t)buflen) {
            nai_errno = ERANGE;
            goto _end;
        };

        p = buf;

    } else {
        root = &m->root_path;
        r = nai_str_len(root);
        r += len;
        if (r > (intptr_t)buflen) {
            nai_errno = ERANGE;
            goto _end;
        };

        p = buf;
        nai_memcpy(p, nai_str(root), nai_str_len(root));
        p += nai_str_len(root);
    };

    nai_memcpy(p, path, len);
    p += len;

    if (r < (intptr_t)buflen) {
        p[0] = 0;
    };

_end:
    return r;
};


nai_pool_t* nai_main_get_temp_pool(nai_main_t* m)
{
    nai_pool_t* r;
    nai_conf_t* c;


    c = m->conf;
    if (c == 0) {
        nai_errno = ENOENT;
        r = 0;
        goto _end;
    };

    r = &c->pool;

_end:
    return r;
};


nai_local_t* nai_main_local_alloc(
    nai_main_t* m, nai_int_t modules, nai_pool_t* pool)
{
    nai_int_t alloc;
    nai_local_t* r;


    /* alloc local data */
    if (modules == 0) {
        alloc = nai_modules_max + 1;
    } else {
        alloc = modules + 1;
    };
    r = (nai_local_t*)nai_palloc(pool, sizeof(void*) * alloc);
    if (r == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "alloc local failed, size %d", alloc);

        goto _end;
    };

    /* fill null-pointer */
    r->slot[0] = nai_main_slot(m);
    nai_pzero(r->data + 1, alloc - 1);


_end:
    return r;
};


#include "nai/io/nai_ssl.h"


extern nai_module_t nai_main_event_module;
extern nai_module_t nai_main_task_module;
extern nai_module_t nai_main_dns_module;
extern nai_module_t nai_main_agent_module;

#if (NAI_WITH_HTTP)
extern nai_module_t nai_https_module;
extern nai_module_t nai_https_core_module;
extern nai_module_t nai_https_user_module;
extern nai_module_t nai_httpc_module;
extern nai_module_t nai_httpc_core_module;
extern nai_module_t nai_httpc_user_module;

#if (NAI_HAVE_SSL)
extern nai_module_t nai_https_ssl_module;
extern nai_module_t nai_httpc_ssl_module;
#endif
#endif

nai_module_t* nai_modules[] = {
    &nai_main_event_module, 
    &nai_main_task_module, 
    &nai_main_dns_module, 
    &nai_main_agent_module, 
#if (NAI_WITH_HTTP)
    &nai_https_module, 
    &nai_https_core_module, 
    &nai_https_user_module,
#if (NAI_HAVE_SSL)
    &nai_https_ssl_module, 
#endif
    &nai_httpc_module, 
    &nai_httpc_core_module, 
#if (NAI_HAVE_SSL)
    &nai_httpc_ssl_module, 
#endif
    &nai_httpc_user_module,
#endif
    0
};

nai_int_t nai_modules_max = nai_countof(nai_modules) + 128;


