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
/// @file       icc_conf.c
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "icc/core/icc_conf.h"

#include "icc/core/icc_const.h"
#include "icc/core/icc_log.h"
#include "icc/core/icc_routing.h"
#include "icc/core/icc_routing_impl.h"
#include "icc/net/icc_message.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_value.h"

typedef struct icc_conf_service_s icc_conf_service_t;
typedef struct icc_conf_app_s icc_conf_app_t;

struct icc_conf_service_s
{
    nai_rbnode_t ent;

    nai_list_entry_t node;

    /* service id */
    union
    {
        /**
         * keep serv is high word, 
         * so that the same serv is continuously in the tree.
         */
        struct
        {
#if defined(NAI_HAVE_BIG_ENDIAN)
            icc_serv_t serv;
            icc_inst_t inst;
#else
            icc_inst_t inst;
            icc_serv_t serv;
#endif
        };
        icc_servinst_t servinst;
    };

    uint16_t icc_type;
    nai_str_t icc_identifier;
};

struct icc_conf_app_s
{
    nai_rbnode_t ent;
    nai_str_t name;
    icc_cid_t cid;
};

typedef struct icc_conf_ctx_s icc_conf_ctx_t;
typedef struct icc_conf_arg_s icc_conf_arg_t;
typedef struct icc_conf_stack_s icc_conf_stack_t;
typedef struct icc_conf_optinfo_s icc_conf_optinfo_t;
typedef struct icc_conf_command_s icc_conf_command_t;
typedef int (*icc_conf_command_f)(icc_conf_command_t* ctx, icc_conf_arg_t* arg);

struct icc_conf_optinfo_s
{
    nai_str_t name;
    int value;
};

struct icc_conf_command_s
{
    const char* name;
    icc_conf_command_f handle;
    int flags;
    size_t offset;
    void* ctx;
};

struct icc_conf_arg_s
{
    icc_conf_ctx_t* ctx;
    nai_value_t v;
    char* data;
};

struct icc_conf_stack_s
{
    icc_conf_stack_t* prev;
    icc_conf_command_t* cmds;
    char* data;
};

struct icc_conf_ctx_s
{
    nai_pool_t pool;
    icc_conf_t* conf;
    icc_conf_stack_t* top;
};

#define ICC_CONF_BEGIN    0x01
#define ICC_CONF_END      0x02
#define ICC_CONF_ADDRONLY 0x0100

static int icc_conf_setopt(icc_conf_t* p, const char* name, nai_value_t* v);

static icc_conf_command_t* icc_conf_find_command(icc_conf_command_t* tbl, const char* name, size_t namelen)
{
    int n;
    icc_conf_command_t* cmd;

    for (n = 0;; n++) {
        cmd = tbl + n;
        if (cmd->name == 0) {
            break;
        };
        if (nai_strncmp(cmd->name, name, namelen + 1) == 0) {
            goto _end;
        };
    };

    cmd = 0;

_end:
    return cmd;
};

static nai_rbnode_t** icc_conf_find_app(icc_conf_t* p, const nai_str_t* name, nai_rbnode_t** pparent)
{
    int c;
    nai_rbnode_t** n     = &nai_rbtree_root(&p->apps);
    nai_rbnode_t* parent = nai_rbtree_end(&p->apps);
    icc_conf_app_t* e;

    while (*n) {
        parent = *n;
        e      = (icc_conf_app_t*)parent;
        c      = nai_str_cmp(&e->name, name);
        if (c == 0) {
            break;
        } else if (c >= 0) {
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

static nai_rbnode_t** icc_conf_find_service(icc_conf_t* p, icc_serv_t serv, icc_inst_t inst, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n     = &nai_rbtree_root(&p->servs);
    nai_rbnode_t* parent = nai_rbtree_end(&p->servs);
    icc_conf_service_t* e;
    icc_servkey_t v;

    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e      = (icc_conf_service_t*)parent;
        if (e->servinst == v.servinst) {
            break;
        } else if (e->servinst >= v.servinst) {
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

int icc_conf_init(icc_conf_t* p)
{
    int r;

    p->ctx = 0;
    nai_pool_init(&p->pool, 1000);
    nai_rbtree_init(&p->apps);
    nai_rbtree_init(&p->servs);

    r = 0;

    return r;
};

int icc_conf_setopti(icc_conf_t* p, const char* name, intptr_t v)
{
    int r;
    nai_value_t val;

    nai_value_init_intptr(&val, &v);
    r = icc_conf_setopt(p, name, &val);

    return r;
};

int icc_conf_setoptf(icc_conf_t* p, const char* name, float v)
{
    int r;
    nai_value_t val;

    nai_value_init_float(&val, &v);
    r = icc_conf_setopt(p, name, &val);

    return r;
};

int icc_conf_setoptd(icc_conf_t* p, const char* name, double v)
{
    int r;
    nai_value_t val;

    nai_value_init_double(&val, &v);
    r = icc_conf_setopt(p, name, &val);

    return r;
};

int icc_conf_setopts(icc_conf_t* p, const char* name, const char* v)
{
    int r;
    nai_value_t val;

    nai_value_init_string(&val, v, nai_strlen(v));
    r = icc_conf_setopt(p, name, &val);

    return r;
};

int icc_conf_finish(icc_conf_t* p)
{
    int r;
    icc_conf_ctx_t* ctx;

    if (p->ctx) {
        ctx = (icc_conf_ctx_t*)p->ctx;
        if (ctx->top->data != (char*)p) {
            nai_log_error(ICC_LOG_CORE, EINVAL, "configuration end not reached");
            nai_errno = EINVAL;
            r         = -1;
            goto _end;
        };

        nai_pool_close(&ctx->pool);
        p->ctx = 0;
    };

    r = 0;

_end:
    return r;
};

int icc_conf_close(icc_conf_t* p)
{
    int r;
    icc_conf_ctx_t* ctx;

    if (p->ctx) {
        ctx = (icc_conf_ctx_t*)p->ctx;
        nai_pool_close(&ctx->pool);
        p->ctx = 0;
    };

    nai_pool_close(&p->pool);
    icc_conf_init(p);
    r = 0;

    return r;
};

int icc_conf_get_client_id(icc_conf_t* p, const nai_str_t* name, icc_cid_t* cid)
{
    int r;
    nai_rbnode_t** n;
    icc_conf_app_t* a;

    n = icc_conf_find_app(p, name, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r         = -1;
        goto _end;
    };

    a = (icc_conf_app_t*)n[0];
    if (cid) {
        cid[0] = a->cid;
    };

    r = 0;

_end:
    return r;
};

int icc_conf_get_service(icc_conf_t* p, icc_serv_t serv, icc_inst_t inst, icc_service_info_t* info)
{
    int r;
    nai_rbnode_t** n;
    icc_conf_service_t* s;

    n = icc_conf_find_service(p, serv, inst, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r         = -1;
        goto _end;
    };

    s = nai_containof(n[0], icc_conf_service_t, ent);

    if (info) {
        info->serv           = serv;
        info->inst           = inst;
        info->icc_type       = s->icc_type;
        info->icc_identifier = s->icc_identifier;
    };
    r = 0;

_end:
    return r;
};

//////////////////////////////////////////////////////////////////////////////
// conf option commands

/**
 * 1. service info
 * 2. service local range
 * 3. event: type, reliable
 * 4. eventgroup: multi-cast
 * 5. client port
 * 6. max message size, reliable/unreliable/local
 * 7. service discovery
 */

static nai_sult_t icc_conf_set_int16(icc_conf_command_t* c, icc_conf_arg_t* arg)
{
    int v;
    uint16_t* d;
    nai_sult_t rc;

    rc = nai_value_geti(&arg->v, &v);
    if (rc < 0) {
        nai_log_error(ICC_LOG_CORE, nai_sult_to_errno(rc), "invalid argument of command '%s'", c->name);
        goto _end;
    };
    if (v < -32768 || v > 65535) {
        rc = nai_errno_to_sult(ERANGE);
        nai_log_error(ICC_LOG_CORE, nai_sult_to_errno(rc), "command '%s' argument is out of range", c->name);
        goto _end;
    };

    d    = (uint16_t*)(arg->data + c->offset);
    d[0] = (uint16_t)v;
    rc   = 0;

_end:
    return rc;
};

static nai_sult_t icc_conf_set_string(icc_conf_command_t* c, icc_conf_arg_t* arg)
{
    nai_sult_t rc;
    nai_str_t* d;
    icc_conf_t* conf;

    d = (nai_str_t*)(arg->data + c->offset);
    if (nai_str(d)) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_error(ICC_LOG_CORE, nai_sult_to_errno(rc), "duplicated of command '%s'", c->name);
        goto _end;
    };

    conf = arg->ctx->conf;
    rc   = nai_value_dups(&arg->v, d, &conf->pool);

_end:
    return rc;
};

static nai_sult_t icc_conf_command_begin(icc_conf_command_t* c, icc_conf_arg_t* arg);
static nai_sult_t icc_conf_command_end(icc_conf_command_t* c, icc_conf_arg_t* arg);
static nai_sult_t icc_conf_app(icc_conf_command_t* c, icc_conf_arg_t* arg);
static nai_sult_t icc_conf_app_end(icc_conf_command_t* c, icc_conf_arg_t* arg);
static nai_sult_t icc_conf_service(icc_conf_command_t* c, icc_conf_arg_t* arg);
static nai_sult_t icc_conf_service_end(icc_conf_command_t* c, icc_conf_arg_t* arg);

static icc_conf_command_t icc_conf_begin = {"begin", icc_conf_command_begin, 0, 0};
static icc_conf_command_t icc_conf_end   = {"end", icc_conf_command_end, 0, 0};

static icc_conf_command_t icc_conf_main[] = {
    {"application", icc_conf_app, ICC_CONF_BEGIN, 0},
    {"service", icc_conf_service, ICC_CONF_BEGIN, 0},
    {0},
};

static int icc_conf_setopt(icc_conf_t* p, const char* name, nai_value_t* v)
{
    int r;
    int ec;
    icc_conf_arg_t arg;
    icc_conf_ctx_t* ctx;
    icc_conf_stack_t* top;
    icc_conf_command_t* cmd;
    nai_pool_t pool;

    ctx = (icc_conf_ctx_t*)p->ctx;
    if (ctx == 0) {
        nai_pool_init(&pool, 0);

        ctx = (icc_conf_ctx_t*)nai_palloc(&pool, sizeof(*ctx));
        if (ctx == 0) {
            nai_log_alert(ICC_LOG_CORE, nai_errno, "create memory pool failed");
            r = -1;
            goto _end;
        };

        top = (icc_conf_stack_t*)nai_palloc(&pool, sizeof(*top));
        if (top == 0) {
            ec = nai_errno;
            nai_log_alert(ICC_LOG_CORE, ec, "allocate memory failed");
            nai_pool_close(&pool);
            nai_errno = ec;
            r         = -1;
            goto _end;
        };

        top->prev = 0;
        top->cmds = icc_conf_main;
        top->data = (char*)p;
        ctx->conf = p;
        ctx->top  = top;
        nai_pool_init(&ctx->pool, 0);
        nai_pool_exchange(&ctx->pool, &pool);
        p->ctx = ctx;
    };

    top      = ctx->top;
    arg.ctx  = ctx;
    arg.v    = *v;
    arg.data = top->data;

    if (nai_strcmp(name, "begin") == 0) {
        cmd = &icc_conf_begin;
    } else if (nai_strcmp(name, "end") == 0) {
        cmd = &icc_conf_end;
    } else {
        cmd = icc_conf_find_command(top->cmds, name, nai_strlen(name));
        if (cmd == 0) {
            nai_log_error(ICC_LOG_CORE, ENOENT, "command '%s' is not exist", name);
            nai_errno = ENOENT;
            r         = -1;
            goto _end;
        };
        if (cmd->flags & (ICC_CONF_BEGIN | ICC_CONF_END)) {
            nai_log_error(ICC_LOG_CORE, ENOENT, "invalid command '%s'", name);
            nai_errno = ENOENT;
            r         = -1;
            goto _end;
        };
    };

    r = cmd->handle(cmd, &arg);
    if (r < 0) {
        nai_errno = nai_sult_to_errno(r);
        r         = -1;
    };

_end:
    return r;
};

static nai_sult_t icc_conf_command_begin(icc_conf_command_t* c, icc_conf_arg_t* arg)
{
    nai_sult_t rc;
    nai_mem_t name;
    icc_conf_ctx_t* ctx;
    icc_conf_stack_t* top;
    icc_conf_command_t* cmd;

    rc = nai_value_getm(&arg->v, &name);
    if (rc < 0) {
        nai_log_error(ICC_LOG_CORE, nai_sult_to_errno(rc), "invalid argument of command '%s'", c->name);
        goto _end;
    };

    ctx = arg->ctx;
    top = ctx->top;
    cmd = icc_conf_find_command(top->cmds, nai_str(&name), nai_str_len(&name));
    if (cmd == 0) {
        rc = nai_errno_to_sult(ENOENT);
        nai_log_error(ICC_LOG_CORE, nai_sult_to_errno(rc), "no valid begin command of segement.");
        goto _end;
    };
    if (!(cmd->flags & ICC_CONF_BEGIN)) {
        rc = nai_errno_to_sult(ENOENT);
        nai_log_error(ICC_LOG_CORE, nai_sult_to_errno(rc), "no valid begin command of segement. without begin flags");
        goto _end;
    };

    top = (icc_conf_stack_t*)nai_palloc(&ctx->pool, sizeof(*top));
    if (top == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(ICC_LOG_CORE, nai_sult_to_errno(rc), "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    top->cmds = 0;
    top->data = 0;
    top->prev = ctx->top;
    ctx->top  = top;

    rc = cmd->handle(cmd, arg);
    if (rc < 0) {
        ctx->top = top;
        goto _end;
    };

_end:
    return rc;
};

static nai_sult_t icc_conf_command_end(icc_conf_command_t* c, icc_conf_arg_t* arg)
{
    nai_sult_t rc;
    icc_conf_ctx_t* ctx;
    icc_conf_stack_t* top;
    icc_conf_command_t* cmd;

    (void)c;

    ctx = arg->ctx;
    top = ctx->top;
    if (top->prev == 0) {
        rc = nai_errno_to_sult(ENOENT);
        nai_log_error(ICC_LOG_CORE, nai_sult_to_errno(rc), "no valid segement.");
        goto _end;
    };

    cmd = icc_conf_find_command(top->cmds, "", 0);
    if (cmd == 0) {
        rc = nai_errno_to_sult(ENOENT);
        nai_log_error(ICC_LOG_CORE, nai_sult_to_errno(rc), "no valid end command of segement.");
        goto _end;
    };
    if (!(cmd->flags & ICC_CONF_END)) {
        rc = nai_errno_to_sult(ENOENT);
        nai_log_error(ICC_LOG_CORE, nai_sult_to_errno(rc), "no valid end command of segement. without end flags");
        goto _end;
    };

    rc = cmd->handle(cmd, arg);
    if (rc < 0) {
        goto _end;
    };

    ctx->top = top->prev;
    rc       = 0;

_end:
    return rc;
};

static icc_conf_command_t icc_conf_app_commands[] = {
    {"name", icc_conf_set_string, 0, nai_offsetof(icc_conf_app_t, name)},
    {"client", icc_conf_set_int16, 0, nai_offsetof(icc_conf_app_t, cid)},
    {"", icc_conf_app_end, ICC_CONF_END, 0},
    {0},
};

static nai_sult_t icc_conf_app(icc_conf_command_t* c, icc_conf_arg_t* arg)
{
    nai_sult_t rc;
    icc_conf_t* conf;
    icc_conf_ctx_t* ctx;
    icc_conf_stack_t* top;
    icc_conf_app_t* a;

    ctx  = arg->ctx;
    conf = ctx->conf;
    a    = (icc_conf_app_t*)nai_palloc(&conf->pool, sizeof(*a));
    if (a == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(ICC_LOG_CORE, nai_sult_to_errno(rc), "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    nai_str_setn(&a->name);
    a->cid = 0;

    top       = ctx->top;
    top->data = (char*)a;
    top->cmds = icc_conf_app_commands;
    rc        = 0;

_end:
    return rc;
};

static nai_sult_t icc_conf_app_end(icc_conf_command_t* c, icc_conf_arg_t* arg)
{
    nai_sult_t rc;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    icc_conf_t* conf;
    icc_conf_ctx_t* ctx;
    icc_conf_app_t* a;

    (void)c;

    a = (icc_conf_app_t*)arg->data;
    if (nai_str_len(&a->name) <= 0) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(ICC_LOG_CORE, nai_sult_to_errno(rc), "command cannot add application without name");
        goto _end;
    }
    if (a->cid == (icc_cid_t)-1) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(ICC_LOG_CORE, nai_sult_to_errno(rc), "command cannot add application without client id");
        goto _end;
    };

    ctx  = arg->ctx;
    conf = ctx->conf;
    n    = icc_conf_find_app(conf, &a->name, &parent);
    if (n[0] != 0) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_alert(ICC_LOG_CORE, nai_sult_to_errno(rc), "application(%s) already exists", nai_str(&a->name));
        goto _end;
    };

    nai_rbtree_link(&conf->apps, &a->ent, parent, n);
    nai_rbtree_color(&conf->apps, &a->ent);
    rc = 0;

_end:
    return rc;
};

static icc_conf_command_t icc_conf_service_commands[] = {
    {"service", icc_conf_set_int16, 0, nai_offsetof(icc_conf_service_t, serv)},
    {"instance", icc_conf_set_int16, 0, nai_offsetof(icc_conf_service_t, inst)},
    {"icc_type", icc_conf_set_int16, 0, nai_offsetof(icc_conf_service_t, icc_type)},
    {"icc_identifier", icc_conf_set_string, 0, nai_offsetof(icc_conf_service_t, icc_identifier)},
    {"", icc_conf_service_end, ICC_CONF_END, 0},
    {0},
};

static nai_sult_t icc_conf_service(icc_conf_command_t* c, icc_conf_arg_t* arg)
{
    nai_sult_t rc;
    icc_conf_t* conf;
    icc_conf_ctx_t* ctx;
    icc_conf_stack_t* top;
    icc_conf_service_t* s;

    /* PRS_SOMEIPSD_00470 */

    ctx  = arg->ctx;
    conf = ctx->conf;
    s    = (icc_conf_service_t*)nai_palloc(&conf->pool, sizeof(*s));
    if (s == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(ICC_LOG_CORE, nai_sult_to_errno(rc), "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    s->serv     = -1;
    s->inst     = -1;
    s->icc_type = -1;
    nai_str_setn(&s->icc_identifier);

    top       = ctx->top;
    top->data = (char*)s;
    top->cmds = icc_conf_service_commands;
    rc        = 0;

_end:
    return rc;
};

static nai_sult_t icc_conf_service_end(icc_conf_command_t* c, icc_conf_arg_t* arg)
{
    nai_sult_t rc;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    icc_conf_t* conf;
    icc_conf_ctx_t* ctx;
    icc_conf_service_t* s;

    (void)c;

    s = (icc_conf_service_t*)arg->data;
    if (s->serv == (icc_serv_t)-1) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(ICC_LOG_CORE, nai_sult_to_errno(rc), "command cannot add service without service id");
        goto _end;
    };
    if (s->inst == (icc_inst_t)-1) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(ICC_LOG_CORE, nai_sult_to_errno(rc), "command cannot add service without instance id");
        goto _end;
    };

    ctx  = arg->ctx;
    conf = ctx->conf;
    n    = icc_conf_find_service(conf, s->serv, s->inst, &parent);
    if (n[0] != 0) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_alert(ICC_LOG_CORE, nai_sult_to_errno(rc), "service (%d, %d) already exists", s->serv, s->inst);
        goto _end;
    };

    nai_rbtree_link(&conf->servs, &s->ent, parent, n);
    nai_rbtree_color(&conf->servs, &s->ent);
    rc = 0;

_end:
    return rc;
};
