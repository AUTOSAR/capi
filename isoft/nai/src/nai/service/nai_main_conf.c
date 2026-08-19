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
/// @file       nai_main_conf.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_main_conf.h"
#include "nai/os/nai_file.h"
#include "nai/service/nai_command.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"
#include <stdarg.h>


#define NAI_MAIN_PARSE_DUP      0x1     /* do dup args */
#define NAI_MAIN_PARSE_NO_NAME  0x2     /* used with line mode */
#define NAI_MAIN_PARSE_LINE     0x4     /* in line mode */
#define NAI_MAIN_PARSE_PART     0x8     /* part of the input data */


typedef struct nai_main_block_s {
    const char* path;
    char* bstart;
    char* bend;
    nai_int_t stat;
    nai_int_t segs;
    nai_int_t line;
    nai_int_t start;
    nai_int_t argc;
    nai_int_t argmax;
    nai_str_t name;
    nai_value_t* argv;
} nai_main_block_t;



static nai_int_t nai_main_parse_line(nai_main_t* m, 
    char* buf, size_t len, 
    char** name, nai_value_t* argv, nai_int_t argc);
static nai_int_t nai_main_parse_block(
    nai_main_t* m, nai_main_block_t* b, nai_int_t flags);
static nai_int_t nai_main_process_block(
    nai_main_t* m, nai_main_block_t* b, nai_int_t flags);


static nai_int_t nai_main_conf_init(nai_main_t* m);
static nai_int_t nai_main_conf_grow_segments(nai_main_t* m);
static nai_int_t nai_main_conf_file(
    nai_main_t* m, nai_value_t* argv, nai_int_t argc);
static nai_int_t nai_main_conf_option(
    nai_main_t* m, nai_int_t n, const char* name, 
    nai_value_t* argv, nai_int_t argc, nai_int_t endc);


nai_int_t nai_main_set_optv(nai_main_t* m, const char* name)
{
    nai_int_t r;
    nai_value_t val;


    /* prepare args */
    nai_value_init_void(&val);


    /* call option */
    r = nai_main_conf_option(m, -1, name, &val, 0, 0);
    if (r < 0) {
        goto _end;
    };

_end:
    return r;
};


nai_int_t nai_main_set_opti(nai_main_t* m, const char* name, intptr_t v)
{
    nai_int_t r;
    nai_value_t val;


    /* prepare args */
    nai_value_init_intptr(&val, &v);


    /* call option */
    r = nai_main_conf_option(m, -1, name, &val, 1, 0);
    if (r < 0) {
        goto _end;
    };

_end:
    return r;
};


nai_int_t nai_main_set_optf(nai_main_t* m, const char* name, float v)
{
    nai_int_t r;
    nai_value_t val;


    /* prepare args */
    nai_value_init_float(&val, &v);


    /* call option */
    r = nai_main_conf_option(m, -1, name, &val, 1, 0);
    if (r < 0) {
        goto _end;
    };

_end:
    return r;
};


nai_int_t nai_main_set_optd(nai_main_t* m, const char* name, double v)
{
    nai_int_t r;
    nai_value_t val;


    /* prepare args */
    nai_value_init_double(&val, &v);


    /* call option */
    r = nai_main_conf_option(m, -1, name, &val, 1, 0);
    if (r < 0) {
        goto _end;
    };

_end:
    return r;
};


nai_int_t nai_main_set_opts(nai_main_t* m, const char* name, const char* str)
{
    nai_int_t r;
    nai_value_t val;


    /* prepare args */
    if (str == 0) {
        r = 0;
    } else {
        nai_value_init_string(&val, str, nai_strlen(str));
        r = 1;
    };


    /* call option */
    r = nai_main_conf_option(m, -1, name, &val, r, 0);
    if (r < 0) {
        goto _end;
    };

_end:
    return r;
};


nai_int_t nai_main_set_optm(nai_main_t* m, const char* name, const char* str)
{
    nai_int_t r;
    nai_int_t len;
    nai_int_t alloc;
    char* buf;
    nai_conf_t* c;
    nai_value_t argv[NAI_CONF_MAX_ARG];


    if (m->start) {
        nai_log_error(NAI_LOG_CORE, EPERM, 
            "already started, can't change options");

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };
    if (m->conf == 0) {
        r = nai_main_conf_init(m);
        if (r < 0) {
            goto _end;
        };
    };
    c = m->conf;


    if (str == 0) {
        r = 0;
    } else {

        len = (nai_int_t)nai_strlen(str);
        if (c->bufalloc < len + 1) {
            alloc = c->bufalloc;
            if (alloc <= 0) {
                alloc = 128;
            };
            while (alloc < len + 1) {
                alloc <<= 1;
            };

            buf = (char*)nai_palloc(&c->pool, alloc);
            if (buf == 0) {
                nai_log_alert(NAI_LOG_CORE, nai_errno, 
                    "nai_palloc allocate buffer failed");
                r = -1;
                goto _end;
            };

            c->buf = buf;
            c->bufalloc = alloc;
        };
        buf = c->buf;
        nai_memcpy(buf, str, len);
        buf[len] = 0;


        /* parse */
        r = nai_main_parse_line(m, buf, len, 0, argv, nai_countof(argv));
        if (r < 0) {
            goto _end;
        };
    };


    /* call option */
    r = nai_main_conf_option(m, -1, name, argv, r, 0);
    if (r < 0) {
        goto _end;
    };

_end:
    return r;
};


nai_int_t nai_main_set_line(nai_main_t* m, const char* str, size_t size)
{
    nai_int_t r;
    nai_int_t len;
    nai_int_t alloc;
    char* buf;
    char* name;
    nai_conf_t* c;
    nai_value_t argv[NAI_CONF_MAX_ARG];


    if (m->start) {
        nai_log_error(NAI_LOG_CORE, EPERM, 
            "already started, can't change options");

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };
    if (m->conf == 0) {
        r = nai_main_conf_init(m);
        if (r < 0) {
            goto _end;
        };
    };
    c = m->conf;


    if (str == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if ((intptr_t)size < 0) {
        len = (nai_int_t)nai_strlen(str);
    } else {
        len = (nai_int_t)size;
    };
    if (c->bufalloc < len + 1) {
        alloc = c->bufalloc;
        if (alloc <= 0) {
            alloc = 128;
        };
        while (alloc < len + 1) {
            alloc <<= 1;
        };

        buf = (char*)nai_palloc(&c->pool, alloc);
        if (buf == 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, 
                "nai_palloc allocate buffer failed");
            r = -1;
            goto _end;
        };

        c->buf = buf;
        c->bufalloc = alloc;
    };
    buf = c->buf;
    nai_memcpy(buf, str, len);
    buf[len] = 0;


    /* parse */
    r = nai_main_parse_line(m, buf, len, &name, argv, nai_countof(argv));
    if (r < 0) {
        goto _end;
    };

    /* check name */
    if (name == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    /* call option */
    r = nai_main_conf_option(m, -1, name, argv, r, 0);
    if (r < 0) {
        goto _end;
    };

_end:
    return r;
};


nai_int_t nai_main_set_fmtv(nai_main_t* m, const char* fmt, va_list va)
{
    nai_int_t r;
    nai_int_t len;
    nai_int_t alloc;
    char* buf;
    char* name;
    va_list vb;
    nai_conf_t* c;
    nai_value_t argv[NAI_CONF_MAX_ARG];


    if (m->start) {
        nai_log_error(NAI_LOG_CORE, EPERM, 
            "already started, can't change options");

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };
    if (m->conf == 0) {
        r = nai_main_conf_init(m);
        if (r < 0) {
            goto _end;
        };
    };
    c = m->conf;


    if (fmt == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    va_copy(vb, va);

    buf = c->buf;
    len = nai_vsnprintf(buf, c->bufalloc, fmt, vb);

    va_end(vb);

    if (c->bufalloc < len + 1) {
        alloc = c->bufalloc;
        if (alloc <= 0) {
            alloc = 128;
        };
        while (alloc < len + 1) {
            alloc <<= 1;
        };

        buf = (char*)nai_palloc(&c->pool, alloc);
        if (buf == 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "nai_palloc allocate buffer failed");
            r = -1;
            goto _end;
        };

        c->buf = buf;
        c->bufalloc = alloc;
        len = nai_vsnprintf(buf, c->bufalloc, fmt, va);
    };


    /* parse */
    r = nai_main_parse_line(m, buf, len, &name, argv, nai_countof(argv));
    if (r < 0) {
        goto _end;
    };

    /* check name */
    if (name == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    /* call option */
    r = nai_main_conf_option(m, -1, name, argv, r, 0);
    if (r < 0) {
        goto _end;
    };

_end:
    return r;
};


nai_int_t nai_main_set_fmts(nai_main_t* m, const char* fmt, ...)
{
    nai_int_t r;
    va_list va;


    va_start(va, fmt);
    r = nai_main_set_fmtv(m, fmt, va);
    va_end(va);

    return r;
};


nai_int_t nai_main_set_block(
    nai_main_t* m, const char* str, size_t size)
{
    nai_int_t r;
    nai_int_t flags;
    intptr_t len;
    nai_main_block_t b;
    nai_value_t argv[NAI_CONF_MAX_ARG];


    if (m->start) {
        nai_log_error(NAI_LOG_CORE, EPERM, 
            "already started, can't change options");

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };
    if (m->conf == 0) {
        r = nai_main_conf_init(m);
        if (r < 0) {
            goto _end;
        };
    };

    if ((intptr_t)size < 0) {
        len = (nai_int_t)nai_strlen(str);
    } else {
        len = size;
    };

    b.path = "memblock";
    b.bstart = (char*)str;
    b.bend = (char*)str + len;
    b.stat = 0;
    b.line = 1;
    b.segs = 0;
    b.argmax = nai_countof(argv);
    b.argv = argv;

    flags = 0;
    flags |= NAI_MAIN_PARSE_DUP;

    r = nai_main_process_block(m, &b, flags);

_end:
    return r;
};


nai_int_t nai_main_push(nai_main_t* m, nai_int_t n)
{
    nai_int_t r;
    nai_conf_t* c;
    nai_conf_seg_t* s;


    if (m->start) {
        nai_log_error(NAI_LOG_CORE, EPERM, 
            "already started, can't change options");

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    c = m->conf;
    if (c == 0 || n <= 0 || n >= c->count) {
        nai_log_error(NAI_LOG_CORE, 
            EINVAL, "seg index %d outbound", n);

        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    s = c->segs[n];
    if (s->stack >= 0) {
        nai_log_crit(NAI_LOG_CORE, 
            EINVAL, "segment '%s' nested", s->name);

        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    s->push = 1;
    s->stack = c->stack;
    c->stack = n;
    r = 0;

_end:
    return r;
};


nai_int_t nai_main_lock_segment(nai_main_t* m)
{
    nai_int_t r;
    nai_conf_t* c;
    nai_conf_seg_t* s;


    if (m->start) {
        nai_log_error(NAI_LOG_CORE, 
            EPERM, "already started, can't change options");

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    c = m->conf;
    if (c == 0 || c->stack < 0) {
        nai_log_error(NAI_LOG_CORE, 
            EPERM, "no segments on the stack to lock");

        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    s = c->segs[c->stack];
    if (s->lock) {
        nai_log_error(NAI_LOG_CORE, 
            EPERM, "the current segment is already locked");

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    s->lock = 1;
    r = 0;

_end:
    return r;
};


nai_int_t nai_main_unlock_segment(nai_main_t* m)
{
    nai_int_t r;
    nai_conf_t* c;
    nai_conf_seg_t* s;


    if (m->start) {
        nai_log_error(NAI_LOG_CORE, 
            EPERM, "already started, can't change options");

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    c = m->conf;
    if (c == 0 || c->stack < 0) {
        nai_log_error(NAI_LOG_CORE, 
            EPERM, "no segments on the stack to unlock");

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    s = c->segs[c->stack];
    if (s->lock == 0) {
        nai_log_error(NAI_LOG_CORE, 
            EPERM, "the current segment is not locked");

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    s->lock = 0;
    r = 0;

_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// conf


static nai_int_t nai_main_conf_init(nai_main_t* m)
{
    nai_int_t r;
    nai_int_t ec;
    nai_pool_t pool;
    nai_conf_t* c;
    nai_conf_seg_t* s;


    if (m->inited == 0) {
        r = nai_main_module_init(m);
        if (r < 0) {
            goto _end;
        };
    };


    nai_pool_init(&pool, 0);

    /* alloc conf */
    c = (nai_conf_t*)nai_palloc(&pool, sizeof(*c));
    if (c == 0) {
        nai_log_alert(NAI_LOG_CORE, nai_errno, "alloc conf failed");
        r = -1;
        goto _fail;
    };

    /* init conf */
    nai_str_setn(&c->path);
    c->list = 0;
    c->stack = 0;
    c->buf = 0;
    c->bufalloc = 0;


    /* alloc segement table */
    c->count = 0;
    c->alloc = 64;
    c->segs = (nai_conf_seg_t**)nai_palloc(&pool, sizeof(*c->segs)*c->alloc);
    if (c->segs == 0) {
        nai_log_alert(NAI_LOG_CORE, nai_errno, "alloc segs table failed");
        r = -1;
        goto _fail;
    };


    /* alloc first segment for main */
    s = (nai_conf_seg_t*)nai_palloc(&pool, sizeof(*s));
    s->stack = -1;
    s->pool = &m->pool;
    s->pool_tmp = &c->pool;
    s->clocal = &m->local;
    s->grp = NAI_MODULE_MAIN;
    s->grp_mask = NAI_CLOC_MAIN;
    s->push = 0;
    s->lock = 0;
    c->segs[0] = s;
    c->count = 1;


    /* success */
    nai_log_debug(NAI_LOG_CORE, 0, "alloc conf success");
    nai_pool_init(&c->pool, 0);
    nai_pool_exchange(&c->pool, &pool);
    m->conf = c;
    r = 0;


_end:
    return r;

_fail:
    if (r < 0) {
        ec = nai_errno;
        nai_pool_close(&pool);
        nai_errno = ec;
    };
    goto _end;
};


static nai_int_t nai_main_conf_grow_segments(nai_main_t* m)
{
    nai_int_t r;
    nai_int_t alloc;
    nai_conf_t* c = m->conf;
    nai_conf_seg_t** segs;


    alloc = c->alloc*2;
    segs = (nai_conf_seg_t**)nai_palloc(&c->pool, sizeof(*segs)*alloc);
    if (segs == 0) {
        nai_log_alert(NAI_LOG_CORE, nai_errno, "grow segs table failed");
        r = -1;
        goto _end;
    };

    nai_memcpy(segs, c->segs, sizeof(*segs)*c->count);
    c->segs = segs;
    c->alloc = alloc;
    r = 0;

_end:
    return r;
};


static nai_int_t nai_main_conf_path(nai_main_t* m, 
    nai_str_t* out, const nai_value_t* val, const char* name)
{
    intptr_t r;
    intptr_t len;
    intptr_t pathlen;
    char* buf;
    char* path;
    nai_sult_t rc;
    nai_str_t root;
    nai_str_t concat;
    nai_str_t mem;
    nai_main_conf_t* c;


    c = m->conf;
    if (val->type != NAI_VALUE_STRING && 
        val->type != NAI_VALUE_MEMORY) {
        rc = nai_value_dups(val, &mem, &c->pool);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "command '%s' dupilcate string failed", name);

            r = -1;
            goto _end;
        };
    } else {
        rc = nai_value_getm(val, (nai_mem_t*)&mem);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "command '%s' get string failed", name);

            r = -1;
            goto _end;
        };
    };

    path = nai_str(&mem);
    pathlen = nai_str_len(&mem);
    if (nai_path_is_absolute(path, pathlen)) {

        buf = 0;
        len = 0;
        while (1) {
            r = nai_path_get_real(buf, len, path);
            if (r < 0) {
                nai_log_error(NAI_LOG_CORE, 
                    nai_errno, "fails to get real path '%s'", path);

                goto _end;
            };
            if (r < len) {
                break;
            };

            len = r + 1;
            buf = nai_palloc(&c->pool, len);
            if (buf == 0) {
                nai_log_alert(NAI_LOG_CORE, nai_errno, 
                    "allocate a path of %d bytes failed", (nai_int_t)len);

                r = -1;
                goto _end;
            };
        };

    } else {

        root = c->path;
        if (nai_str_len(&root) > 0) {
            nai_str_setc(&concat, "/../");
        } else {
            root = m->conf_path;
            nai_str_setc(&concat, "/");
        };

        len = pathlen + nai_str_len(&root) + nai_str_len(&concat) + 1;
        buf = nai_palloc(&c->pool, len);
        if (buf == 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, 
                "allocate a path of %d bytes failed", (nai_int_t)len);

            r = -1;
            goto _end;
        };

        len = 0;
        nai_memcpy(buf, nai_str(&root), nai_str_len(&root));
        len += nai_str_len(&root);
        nai_memcpy(buf + len, nai_str(&concat), nai_str_len(&concat));
        len += nai_str_len(&concat);
        nai_memcpy(buf + len, path, pathlen);
        len += pathlen;
        buf[len] = 0;
        len ++;

        r = nai_path_canonicalize(buf, len, buf, len, '/');
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "fails to canonicalize path '%s'", path);

            goto _end;
        };
    };

    path = buf;
    pathlen = r;
    nai_str_setm(out, path, pathlen);

_end:
    return (nai_int_t)r;
};


static nai_int_t nai_main_conf_file(
    nai_main_t* m, nai_value_t* args, nai_int_t argc)
{
    nai_int_t ec;
    nai_int_t flags;
    intptr_t r;
    intptr_t len;
    char* buf;
    nai_fd_t fd;
    nai_str_t path;
    nai_str_t last;
    nai_conf_file_t f;
    nai_conf_file_t* e;
    nai_main_conf_t* c;
    nai_main_block_t b;
    nai_value_t argv[NAI_CONF_MAX_ARG];


    if (argc != 1) {
        nai_log_error(NAI_LOG_CORE, EINVAL, 
            "incorrect number of 'include' arguments %d ", argc);

        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = nai_main_conf_path(m, &path, args, "include");
    if (r < 0) {
        goto _end;
    };

    c = m->conf;
    e = c->list;
    for ( ; e; e = e->next) {
        if (nai_str_eq(&e->path, &path)) {
            nai_log_error(NAI_LOG_CORE, EMFILE, 
                "ecursive include '%s'", nai_str(&path));

            nai_errno = EMFILE;
            r = -1;
            goto _end;
        };
    };

    last = c->path;

    fd = nai_file_open(nai_str(&path), NAI_O_RDONLY, 0);
    if (fd == NAI_FD_INVALID) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "can't open configure file %s", nai_str(&path));

        r = -1;
        goto _end;
    };

    buf = nai_malloc(NAI_CONF_FILE_READ);
    if (buf == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "allocate read buffer failed");

        r = -1;
        goto _fail;
    };


    f.path = path;
    f.next = c->list;
    c->list = &f;
    c->path = path;
    b.path = nai_str(&path);
    b.stat = 0;
    b.segs = 0;
    b.line = 1;
    b.argmax = nai_countof(argv);
    b.argv = argv;

    flags = 0;
    flags |= NAI_MAIN_PARSE_DUP;
    flags |= NAI_MAIN_PARSE_PART;

    len = 0;
    for (;;) {
        r = nai_file_read(fd, buf + len, NAI_CONF_FILE_READ - len);
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "read configure file %s failed", nai_str(&path));

            goto _fail;
        };
        if (r == 0) {
            flags &= ~NAI_MAIN_PARSE_PART;
        };

        len += r;
        b.bstart = (char*)buf;
        b.bend = (char*)buf + len;

        r = nai_main_process_block(m, &b, flags);
        if (r < 0) {
            goto _fail;
        };

        /* finished */
        if (!(flags & NAI_MAIN_PARSE_PART)) {
            break;
        };

        len = b.bend - b.bstart;
        if (len >= NAI_CONF_FILE_READ) {
            nai_log_error(NAI_LOG_CORE, EINVAL, 
                "too long argument '%.*s' in %s", 10, b.bstart, b.path);

            nai_errno = EINVAL;
            r = -1;
            goto _fail;
        };

        if (len > 0) {
            nai_memcpy(buf, b.bstart, len);
        };
    };

    r = 0;

_fail:
    c->path = last;
    if (c->list == &f) {
        c->list = f.next;
    };

    if (r < 0) {
        ec = nai_errno;
    };

    if (buf != 0) {
        nai_free(buf);
    };
    if (fd != NAI_FD_INVALID) {
        nai_file_close(fd);
    };

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return (nai_int_t)r;
};


static nai_int_t nai_main_conf_option(
    nai_main_t* m, nai_int_t n, const char* name, 
    nai_value_t* argv, nai_int_t argc, nai_int_t endc)
{
    nai_int_t r;
    nai_int_t index;
    nai_int_t stack;
    nai_int_t popstack = 0;
    nai_conf_t* c;
    nai_conf_seg_t* s;
    nai_module_t* mod;
    nai_command_t* opt;
    nai_command_args_t args;


    if (m->start) {
        nai_log_error(NAI_LOG_CORE, EPERM, 
            "already started, can't change options");

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };
    if (m->conf == 0) {
        r = nai_main_conf_init(m);
        if (r < 0) {
            goto _end;
        };
    };


    c = m->conf;
    stack = 0;
    if (n == -1) {
        n = c->stack;
        stack = 1;
    };


    /* check index args */
    if (n < 0 || n >= c->count) {
        nai_log_error(NAI_LOG_CORE, EINVAL, "seg index %d outbound", n);
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    s = c->segs[n];


    /* check special options */
    if (nai_strcasecmp(name, "include") == 0) {
        r = nai_main_conf_file(m, argv, argc);
        goto _end;
    };
    if (nai_strcasecmp(name, "end") == 0) {
        if (!stack) {
            nai_log_error(NAI_LOG_CORE, EPERM, 
                "command 'end' shouldn't be used in non-stack mode");

            nai_errno = EPERM;
            r = -1;
            goto _end;
        };
        if (s->stack == -1) {
            nai_log_error(NAI_LOG_CORE, EPERM, "already reach stack bottom");
            nai_errno = EPERM;
            r = -1;
            goto _end;
        };
        if (s->lock) {
            nai_log_error(NAI_LOG_CORE, EPERM, "the segment is locked");
            nai_errno = EPERM;
            r = -1;
            goto _end;
        };
        if (s->push) {
            c->stack = s->stack;
            s->stack = -1;
            s->push = 0;
            r = 0;
            goto _end;
        };

        popstack = 1;
        name = s->name;
        s = c->segs[s->stack];
    };


    /* lookup command */
    opt = nai_module_map_command(&m->modules, s->grp, s->grp_mask, name, &mod);
    if (opt == 0) {
        nai_log_error(NAI_LOG_CORE, EINVAL, "unkown command '%s'", name);
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (!(opt->flags & s->grp_mask)) {
        nai_log_error(NAI_LOG_CORE, EPERM, 
            "command '%s' shouldn't used here", opt->name);

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    /* check */
    if (endc == '{') {
        if (!(opt->flags & NAI_COPT_SEGEMENT)) {
            nai_log_error(NAI_LOG_CORE, 
                EINVAL, "command '%s' is not a segment", name);
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };

    /* check args */
    if (popstack) {
        if (argc > 0) {
            nai_log_error(NAI_LOG_CORE, EINVAL, 
                "'%s' dose not require too many arguments %d", 
                "end", argc);

            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    } else {
        if (argc >= NAI_CONF_MAX_ARG) {
            nai_log_error(NAI_LOG_CORE, EINVAL, 
                "'%s' dose not require too many arguments %d", 
                opt->name, argc);

            nai_errno = EINVAL;
            r = -1;
            goto _end;
        } else if (!(opt->flags | (1<<argc))) {
            nai_log_error(NAI_LOG_CORE, EINVAL, 
                "incorrect number of '%s' arguments %d ", opt->name, argc);

            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };


    index = opt->flags & NAI_COPT_LOCALMASK;
    index >>= 24;

    /* prepare args */
    args.argv = argv;
    args.argc = argc;
    args.main = m;
    args.pool = s->pool;
    args.pool_tmp = s->pool_tmp;
    args.clocal = s->clocal;
    args.grp = s->grp;
    args.grp_mask = s->grp_mask;
    args.endseg = popstack;
    args.data = (uint8_t*)nai_local_at(s->clocal[index], *mod);


    /* call */
    r = nai_command(opt, &args);
    if (r < 0) {
        nai_errno = nai_sult_to_errno(r);
        nai_log_error(NAI_LOG_CORE, nai_errno, "call %s failed", opt->name);
        goto _end;
    };
    r = 0;


    /* check segement */
    if (opt->flags & NAI_COPT_SEGEMENT) {
        if (popstack) {
            s = c->segs[n];
            c->stack = s->stack;
            s->stack = -1;
            r = 0;
            goto _end;
        };

        if (args.endseg) {
            /* segment is end */
            r = 0;
            goto _end;
        };

        if (endc == ';') {
            nai_log_error(NAI_LOG_CORE, 
                EINVAL, "command '%s' is a segment", name);
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

#if 0
        for (n = 0; n < c->count; n ++) {
            if (args.clocal == c->segs[n]->clocal) {
                break;
            };
        };
        if (n >= c->count) {
#endif
            if (c->count >= c->alloc) {
                r = nai_main_conf_grow_segments(m);
                if (r < 0) {
                    goto _end;
                };
            };
            s = (nai_conf_seg_t*)nai_palloc(&c->pool, sizeof(*s));
            if (s == 0) {
                nai_log_alert(NAI_LOG_CORE, nai_errno, "alloc seg failed");
                r = -1;
                goto _end;
            };

            n = c->count;
            s->stack = -1;
            s->name = opt->name;
            s->clocal = args.clocal;
            s->pool = args.pool;
            s->pool_tmp = args.pool_tmp;
            s->grp = args.grp;
            s->grp_mask = args.grp_mask;
            s->push = 0;
            s->lock = 0;
            c->segs[n] = s;
            c->count ++;
#if 0
        };
#endif

        /* stack mode, push */
        if (stack) {
            s = c->segs[n];
            if (s->stack >= 0) {
                nai_log_crit(NAI_LOG_CORE, 
                    nai_errno, "segment '%s' nested", opt->name);
                r = -1;
                goto _end;
            };

            s->stack = c->stack;
            c->stack = n;
            r = n;
        };
    };

_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// arguments parse


static intptr_t nai_main_escape(char* dst, const char* src, size_t size)
{
    intptr_t r;
    nai_int_t quote = 0;
    nai_int_t escape = 0;
    char ch;
    char* out = dst;
    const char* buf = src;
    const char* bend = src + size;


    for ( ; buf < bend; ) {
        ch = *buf ++;

        if (escape) {
            escape = 0;

            switch (ch) {
            case 't':
                ch = '\t';
                break;
            case 'n':
                ch = '\n';
                break;
            case 'r':
                ch = '\r';
                break;
            case 'b':
                ch = '\b';
                break;
            case 'f':
                ch = '\f';
                break;
            default:
                break;
            };
            *out ++ = ch;

        } else {

            switch (ch) {
            case '\\':
                escape = 1;
                break;

            case '\'':
            case '\"':
                if (!quote) {
                    quote = ch;
                    break;
                };
                if (quote == ch) {
                    quote = 0;
                    break;
                };

                /* fallthrough */

            default:
                *out ++ = ch;
                break;
            };
        };
    };

    *out = 0;
    r = out - dst;
    return r;
};


static nai_int_t nai_main_parse_line(
    nai_main_t* m, 
    char* buf, size_t len, 
    char** name, nai_value_t* argv, nai_int_t count)
{
    nai_int_t r;
    nai_int_t flags;
    nai_main_block_t b;


    b.path = "memline";
    b.bstart = buf;
    b.bend = buf + len;
    b.stat = 0;
    b.line = 1;
    b.argmax = count;
    b.argv = argv;

    flags = 0;
    flags |= NAI_MAIN_PARSE_LINE;
    flags |= name ? 0 : NAI_MAIN_PARSE_NO_NAME;

    r = nai_main_parse_block(m, &b, flags);
    if (r < 0) {
        goto _end;
    };

    if (name) {
        name[0] = nai_str(&b.name);
    };

    r = b.argc;

_end:
    return r;
};


static nai_int_t nai_main_parse_block(
    nai_main_t* m, nai_main_block_t* b, nai_int_t flags)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t quote = 0;
    nai_int_t escape = 0;
    nai_int_t translate = 0;
    nai_int_t comment = 0;
    nai_int_t dollar = 0;
    nai_int_t variable = 0;
    nai_int_t need_space = 0;
    intptr_t size;
    char ch;
    char endc = 0;
    char fmts[128];
    char* token;
    char* start = 0;
    char* buf;
    char* bend;
    const char* emsg;
    nai_conf_t* c;


    if (b->stat == 0) {
        b->stat = 1;
        b->start = 0;
        b->argc = 0;
        nai_str_setn(&b->name);
    };

    c = m->conf;
    buf = b->bstart;
    bend = b->bend;
    for ( ; buf < bend; buf ++) {
        ch = buf[0];

        if (!start) {
            switch (ch) {
            case '\n':
                b->line ++;
                /* fallthrough */

            case '\r':
                comment = 0;
                continue;
            default:
                break;
            };

            if (comment) {
                continue;
            };

            switch (ch) {
            case ' ':
            case '\t':
                break;

            case '#':
                comment = 1;
                break;

            case ';':
            case '{':
                if (flags & NAI_MAIN_PARSE_LINE) {
                    emsg = "unexcepted '%c' in line mode";
                    ec = EINVAL;
                    goto _echar;
                };

                if (ch == ';' && nai_str(&b->name) == 0) {
                    continue;
                };

                buf ++;
                endc = ch;
                goto _done;

            case '}':
                if (flags & NAI_MAIN_PARSE_LINE) {
                    emsg = "unexcepted '%c' in line mode";
                    ec = EINVAL;
                    goto _echar;
                };

                if (nai_str(&b->name) || b->argc) {
                    goto _done;
                };

                buf ++;
                endc = ch;
                goto _done;

            case '\\':
                translate = 1;
                escape = 1;
                start = buf;
                break;

            case '$':
                dollar = 1;
                start = buf;
                break;

            case '\'':
            case '\"':
                translate = 1;
                quote = ch;

                /* fallthrough */

            default:
                start = buf;
                break;
            };

        } else {
            if (escape) {
                escape = 0;
                continue;
            };

            if (dollar) {
                dollar = 0;
                switch (ch) {
                case '{':
                    variable = 1;
                    continue;
                case '\'':
                case '\"':
                case '#':
                case ';':
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    emsg = "'$' without a name";
                    ec = EINVAL;
                    goto _error;
                default:
                    break;
                };
            };

            if (quote) {
                switch (ch) {
                case '\\':
                    translate = 1;
                    escape = 1;
                    break;

                case '\'':
                case '\"':
                    if (quote != ch) {
                        break;
                    };

                    quote = 0;
                    need_space = 1;
                    break;
                default:
                    break;
                };
            } else {
                switch (ch) {
                case '\\':
                    translate = 1;
                    escape = 1;
                    break;

                case '\'':
                case '\"':
                    if (variable) {
                        emsg = "invalid variable";
                        ec = EINVAL;
                        goto _error;
                    };
                    if (need_space) {
                        emsg = "unexcepted '%c'";
                        ec = EINVAL;
                        goto _echar;
                    };

                    translate = 1;
                    quote = ch;
                    break;

                case '}':
                    if (variable) {
                        variable = 0;
                        need_space = 1;
                        continue;
                    };

                    emsg = "excepted ';' to end command";
                    ec = EINVAL;
                    goto _error;

                case '{':
                case ';':
                    if (flags & NAI_MAIN_PARSE_LINE) {
                        emsg = "unexcepted '%c' in line mode";
                        ec = EINVAL;
                        goto _echar;
                    };

                    /* fallthrough */

                case '#':
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    if (variable) {
                        emsg = "excepted '}'";
                        ec = EINVAL;
                        goto _error;
                    };

                    need_space = 0;

                    /* dupilcate token */
                    size = buf - start;
                    if (flags & NAI_MAIN_PARSE_DUP) {
                        token = nai_palloc(&c->pool, size + 1);
                        if (token == 0) {
                            emsg = "dupilcate token failed";
                            ec = nai_errno;
                            goto _error;
                        };
                        if (translate == 0) {
                            nai_memcpy(token, start, size);
                            token[size] = 0;
                        };
                    } else {
                        buf[0] = 0;
                        token = start;
                    };

                    /* translate token */
                    if (translate) {
                        translate = 0;
                        size = nai_main_escape(token, start, size);
                    };

                    /* set name and value */
                    if (!(flags & NAI_MAIN_PARSE_NO_NAME) && 
                        nai_str(&b->name) == 0) {
                        nai_str_setm(&b->name, token, size);
                        b->start = b->line;
                    } else {
                        if (b->argc >= b->argmax) {
                            goto _margs;
                        };

                        nai_value_init_string(
                            &b->argv[b->argc], token, size);
                        b->argc ++;
                    };

                    start = 0;
                    if (ch == '#') {
                        comment = 1;
                    } else if (ch == '\n') {
                        b->line ++;
                    } else if (ch == ';' || ch == '{') {
                        if (flags & NAI_MAIN_PARSE_LINE) {
                            emsg = "unexcepted '%c' in line mode";
                            ec = EINVAL;
                            goto _echar;
                        };

                        buf ++;
                        endc = ch;
                        goto _done;
                    };
                    break;

                default:
                    if (!need_space) {
                        break;
                    };
                    if (ch != ')') {
                        emsg = "unexcepted '%c'";
                        ec = EINVAL;
                        goto _echar;
                    };
                    break;
                };
            };
        };
    };

    if (start) {
        if (flags & NAI_MAIN_PARSE_PART) {
            b->bstart = start; /* need more */
            r = 0;
            goto _end;
        };

        if (variable) {
            emsg = "excepted '}'";
            ec = EINVAL;
            goto _error;
        };

        if (quote || escape) {
            emsg = "no end of argument";
            ec = EINVAL;
            goto _error;
        };

        /* dupilcate token */
        size = buf - start;
        if (flags & NAI_MAIN_PARSE_DUP) {
            token = nai_palloc(&c->pool, size + 1);
            if (token == 0) {
                emsg = "dupilcate token failed";
                ec = nai_errno;
                goto _error;
            };
            if (translate == 0) {
                nai_memcpy(token, start, size);
                token[size] = 0;
            };
        } else {
            buf[0] = 0;
            token = start;
        };

        /* translate token */
        if (translate) {
            /* translate = 0; */
            size = nai_main_escape(token, start, size);
        };

        /* set name and value */
        if (!(flags & NAI_MAIN_PARSE_NO_NAME) && 
            nai_str(&b->name) == 0) {
            nai_str_setm(&b->name, token, size);
            b->start = b->line;
        } else {
            if (b->argc >= b->argmax) {
_margs:
                emsg = "too many arguments";
                ec = EINVAL;
                goto _error;
            };

            nai_value_init_string(
                &b->argv[b->argc], token, size);
            b->argc ++;
        };

        start = 0;
    };

_done:
    if (!(flags & NAI_MAIN_PARSE_NO_NAME) && 
        nai_str(&b->name) == 0) {
        if (endc != '}' && (
            endc != 0 || (flags & NAI_MAIN_PARSE_LINE))) {
            if (endc == 0) {
                emsg = "empty line without command name";
                ec = EINVAL;
                goto _error;
            } else {
                emsg = "no command name ends with '%c'";
                ch = endc;
                ec = EINVAL;
                goto _echar;
            };
        };
    };

    b->bstart = buf;
    b->stat = endc == 0;
    r = endc;

_end:
    return r;

_error:
    nai_log_error(NAI_LOG_CORE, ec, "at %s:%d, %s", b->path, b->line, emsg);
    nai_errno = ec;
    r = -1;
    goto _end;

_echar:
    nai_memcpy(fmts, "at %s:%d, ", 10);
    nai_memcpy(fmts + 10, emsg, nai_strlen(emsg) + 1);
    nai_log_error(NAI_LOG_CORE, ec, fmts, b->path, b->line, ch);
    nai_errno = ec;
    r = -1;
    goto _end;
};


static nai_int_t nai_main_process_block(
    nai_main_t* m, nai_main_block_t* b, nai_int_t flags)
{
    nai_int_t r;
    nai_int_t endc;


    for (;;) {
        r = nai_main_parse_block(m, b, flags);
        if (r < 0) {
            goto _end;
        };
        if (r == 0) {
            break;
        };

        endc = r;
        switch (endc) {
        case '{':
            b->segs ++;
            break;

        case '}':
            if (b->segs <= 0) {
                nai_log_error(NAI_LOG_CORE, 
                    EINVAL, "unpaired '}' at %s:%d", b->path, b->line);

                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            b->segs --;
            nai_str_setc(&b->name, "end");
            break;

        default:
            break;
        };

        r = nai_main_conf_option(
            m, -1, nai_str(&b->name), b->argv, b->argc, endc);
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "fails to execute '%s' in %s:%d", 
                nai_str(&b->name), b->path, b->start);

            goto _end;
        };
    };

    if (!(flags & NAI_MAIN_PARSE_PART)) {
        if (b->segs > 0) {
            nai_log_error(NAI_LOG_CORE, 
                EINVAL, "missing end-of-paragraph '}' in %s", b->path);

            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


