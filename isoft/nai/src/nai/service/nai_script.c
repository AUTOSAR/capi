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
/// @file       nai_script.c
/// @brief      
/// @details
/// @date       2021-09-14
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/service/nai_script.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"



//////////////////////////////////////////////////////////////////////////////
// script


typedef struct nai_script_vardecl_s {

    nai_rbnode_t ent;
    union {
        struct {
            nai_mem_t name;
            nai_script_get_f get;
            nai_script_set_f set;
            uintptr_t ud;
            nai_int_t flags;
            nai_int_t index;
        };
        nai_script_variable_t var;
    };

} nai_script_vardecl_t;



static nai_rbnode_t** nai_script_find_variable(
    nai_script_vartable_t* s, const nai_mem_t* name, nai_rbnode_t** pparent)
{
    nai_int_t r;
    nai_rbnode_t** n = &nai_rbtree_root(&s->map);
    nai_rbnode_t* parent = nai_rbtree_end(&s->map);
    nai_script_vardecl_t* v;


    while (*n) {
        parent = *n;
        v = nai_containof(parent, nai_script_vardecl_t, ent);

        r = nai_str_cmp(&v->name, name);
        if (r == 0) {
            break;
        } else if (r >= 0) {
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


static nai_sult_t nai_script_add_prefix_variable(
    nai_script_vartable_t* s, const nai_mem_t* name, 
    nai_script_get_f get, nai_script_set_f set, uintptr_t ud, nai_int_t flags)
{
    nai_int_t n;
    nai_int_t found = 0;
    nai_sult_t rc;
    nai_script_vardecl_t* v;
    nai_script_vardecl_t** p;


    p = (nai_script_vardecl_t**)s->prefix.elts;
    for (n = 0; n < (intptr_t)s->prefix.count; n ++) {
        if (nai_str_eq(&p[n]->name, name)) {
            found = 1;
            break;
        };
    };
    if (found) {
        rc = nai_errno_to_sult(EEXIST);
        goto _end;
    };

    p = (nai_script_vardecl_t**)nai_array_push(&s->prefix);
    if (p == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    v = (nai_script_vardecl_t*)nai_palloc(s->pool, sizeof(*v));
    if (v == 0) {
        s->list.count --;
        rc = nai_sult_from_errno();
        goto _end;
    };

    v->name = *name;
    v->get = get;
    v->set = set;
    v->ud = ud;
    v->flags = flags;
    v->index = (nai_int_t)(s->list.count - 1);
    p[0] = v;

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_script_add_variable_impl(
    nai_script_vartable_t* s, const nai_mem_t* name, 
    nai_script_get_f get, nai_script_set_f set, uintptr_t ud, nai_int_t flags, 
    nai_script_vardecl_t** pv)
{
    intptr_t r;
    nai_sult_t rc;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_script_vardecl_t* v;


    if (get == 0) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    if (flags & NAI_SCRIPT_VAR_PREFIX) {
        rc = nai_script_add_prefix_variable(s, name, get, set, ud, flags);
        if (pv && rc >= 0) {
            pv[0] = 0;
        };
        goto _end;
    };

    n = nai_script_find_variable(s, name, &parent);
    if (n[0] != 0) {
        rc = nai_errno_to_sult(EEXIST);
        goto _end;
    };

    v = (nai_script_vardecl_t*)nai_palloc(s->pool, sizeof(*v));
    if (v == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    r = nai_str_dup((nai_str_t*)&v->name, 
        nai_str(name), nai_str_len(name), s->pool);
    if (r < 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    v->get = get;
    v->set = set;
    v->ud = ud;
    v->flags = flags;
    v->index = -1;

    nai_rbtree_link(&s->map, &v->ent, parent, n);
    nai_rbtree_color(&s->map, &v->ent);

    if (pv) {
        pv[0] = v;
    };
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_script_vartable_init(
    nai_script_vartable_t* s, nai_pool_t* pool)
{
    nai_sult_t rc;


    if (pool == 0) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };


    s->pool = pool;
    nai_array_init(&s->list, sizeof(nai_script_vardecl_t*), s->pool);
    nai_array_init(&s->prefix, sizeof(nai_script_vardecl_t*), s->pool);
    nai_rbtree_init(&s->map);
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_script_vartable_close(nai_script_vartable_t* s)
{
    nai_sult_t rc;


    nai_array_close(&s->list);
    nai_array_close(&s->prefix);
    nai_array_init(&s->list, sizeof(nai_script_vardecl_t*), s->pool);
    nai_array_init(&s->prefix, sizeof(nai_script_vardecl_t*), s->pool);
    nai_rbtree_init(&s->map);
    rc = 0;

    return rc;
};


nai_sult_t nai_script_get_variable(
    nai_script_vartable_t* s, const nai_mem_t* name, 
    nai_script_variable_t** pv)
{
    nai_int_t m;
    nai_int_t found;
    nai_sult_t rc;
    nai_rbnode_t** n;
    nai_script_vardecl_t* v;
    nai_script_vardecl_t** p;


    n = nai_script_find_variable(s, name, 0);
    if (n[0] != 0) {
        v = nai_containof(n[0], nai_script_vardecl_t, ent);
        goto _found;
    };

    found = 0;
    p = (nai_script_vardecl_t**)s->prefix.elts;
    for (m = 0; m < (intptr_t)s->prefix.count; m ++) {
        v = p[m];
        if (v->name.len > name->len) {
            continue;
        };
        if (nai_strncmp(nai_str(&v->name), 
            nai_str(name), nai_str_len(&v->name)) == 0) {
            found = 1;
            break;
        };
    };
    if (!found) {
        rc = nai_errno_to_sult(ENOENT);
        goto _end;
    };

    /* do add a new value */
    rc = nai_script_add_variable_impl(s, 
        name, v->get, v->set, v->ud, v->flags & ~NAI_SCRIPT_VAR_PREFIX, &v);
    if (rc < 0) {
        goto _end;
    };

_found:
    if (v->index == -1) {
        p = (nai_script_vardecl_t**)nai_array_push(&s->list);
        if (p == 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };

        v->index = (nai_int_t)(s->list.count - 1);
        p[0] = v;
    };

    if (pv) {
        pv[0] = &v->var;
    };

    rc = v->index;

_end:
    return rc;
};


nai_sult_t nai_script_add_variable(
    nai_script_vartable_t* s, const nai_mem_t* name, 
    nai_script_get_f get, nai_script_set_f set, uintptr_t ud, nai_int_t flags)
{
    return nai_script_add_variable_impl(s, name, get, set, ud, flags, 0);
};


nai_sult_t nai_script_add_variables(
    nai_script_vartable_t* s, const nai_script_variable_t* v, nai_int_t count)
{
    nai_int_t n;
    nai_sult_t rc;


    for (n = 0; n < count; n ++) {
        rc = nai_script_add_variable(s, 
            &v[n].name, v[n].get, v[n].set, v[n].ud, v[n].flags);
        if (rc < 0) {
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};



//////////////////////////////////////////////////////////////////////////////
// script execute


nai_sult_t nai_script_init(
    nai_script_t* s, nai_script_vartable_t* st)
{
    nai_sult_t rc;


    s->sult = 0;
    s->st = st;
    s->values = 0;
    s->sp = 0;
    s->ip = 0;
    s->ctx = 0;
    s->len = 0;
    rc = 0;

    return rc;
};


nai_sult_t nai_script_get_value(
    nai_script_t* s, nai_int_t index, nai_script_value_t* v)
{
    nai_sult_t rc;
    nai_script_vartable_t* st;
    nai_script_value_t* c;
    nai_script_vardecl_t* n;


    st = s->st;
    if (index < 0 || index >= (intptr_t)st->list.count) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    if (s->values) {
        c = &s->values[index];
        if (c->no_cacheable) {
            c->valid = 0;
            c->not_found = 0;
        } else if (c->valid || c->not_found) {
            v[0] = *c;
            rc = 0;
            goto _end;
        } else {
            /* nothing */
            ;
        };

    } else {
        c = v;
    };

    n = *((nai_script_vardecl_t**)st->list.elts + index);
    rc = n->get(s->ctx, n->ud, c);
    if (rc >= 0) {
        if (c->not_found == 0) {
            c->valid = 1;
        };
        if (n->flags & NAI_SCRIPT_VAR_NOCACHEABLE) {
            c->no_cacheable = 1;
        };
        if (v != c) {
            v[0] = *c;
        };
    };

_end:
    return rc;
};


nai_sult_t nai_script_set_value(
    nai_script_t* s, nai_int_t index, const nai_script_value_t* v)
{
    nai_sult_t rc;
    nai_script_vartable_t* st;
    nai_script_vardecl_t* n;


    st = s->st;
    if (index < 0 || index >= (intptr_t)st->list.count) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    n = *((nai_script_vardecl_t**)st->list.elts + index);
    if (n->set == 0) {
        rc = nai_errno_to_sult(ENOTSUP);
        goto _end;
    };

    rc = n->set(s->ctx, n->ud, v);

_end:
    return rc;
};


nai_sult_t nai_script_exec(nai_script_t* s, const void* code)
{
    nai_sult_t rc;
    nai_script_code_f f;
    nai_script_code_t* c;


    s->sult = 0;
    s->ip = (uint8_t*)code;
    while (1) {
        c = (nai_script_code_t*)s->ip;
        f = c->fn;
        if (f == 0) {
            break;
        };

        f(s);
    };

    rc = s->sult;

    return rc;
};


nai_sult_t nai_script_expn_value(
    nai_script_t* s, const nai_script_expn_t* expn, 
    nai_str_t* v, const nai_reserved_t* rev, nai_pool_t* pool)
{
    uint8_t* buf;
    size_t len;
    size_t pre;
    size_t reserved;
    nai_sultp_t rc;


    if (rev == 0) {
        pre = 0;
        reserved = 0;
    } else {
        pre = rev->pre;
        reserved = pre + rev->post;
    };
    if (expn->lengths == 0) {
        if (!reserved) {
            *v = expn->value;
            rc = 0;
            goto _end;
        };

        rc = nai_str_dup_r(v, 
            nai_str(&expn->value), 
            nai_str_len(&expn->value), rev, pool);
        if (rc < 0) {
            rc = nai_sult_from_errno();
        };
        goto _end;
    };

    s->len = 0;
    rc = nai_script_exec(s, expn->lengths);
    if (rc < 0) {
        goto _end;
    };

    len = s->len + reserved;
    buf = (uint8_t*)(pool ? nai_palloc(pool, len+1) : nai_malloc(len+1));
    if (buf == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    len = s->len;
    s->pos = buf + pre;
    rc = nai_script_exec(s, expn->values);
    if (rc < 0) {
        if (pool == 0) {
            nai_free(buf);
        };
        goto _end;
    };

    len += pre;
    buf[len] = 0;
    nai_str_setm(v, buf, len);

_end:
    return (nai_sult_t)rc;
};



//////////////////////////////////////////////////////////////////////////////
// script complex code generate


const nai_script_code_t nai_script_code_return = { 0 };


typedef struct nai_script_code_copy_s {
    nai_script_code_f fn;
    uint32_t len;
    uint8_t data[1];
} nai_script_code_copy_t;


typedef struct nai_script_code_var_s {
    nai_script_code_f fn;
    nai_int_t index;
} nai_script_code_var_t;


typedef struct nai_script_compile_value_s {
    nai_script_vartable_t* sc;
    nai_array_t* values;
    nai_array_t* lengths;
} nai_script_compile_value_t;


static void nai_script_code_copy(nai_script_t* s)
{
    nai_script_code_copy_t* c;


    c = (nai_script_code_copy_t*)s->ip;
    nai_memcpy(s->pos, c->data, c->len);
    s->pos += c->len;
    s->ip += (nai_offsetof(
        nai_script_code_copy_t, data) + c->len) & ~(sizeof(intptr_t)-1);
};


static void nai_script_code_copy_len(nai_script_t* s)
{
    nai_script_code_copy_t* c;


    c = (nai_script_code_copy_t*)s->ip;
    s->len += c->len;
    s->ip += nai_offsetof(nai_script_code_copy_t, data);
};


static nai_sult_t nai_script_add_code_copy(
    nai_script_compile_value_t* b, const nai_mem_t* value)
{
    size_t size;
    nai_sult_t rc;
    nai_script_code_copy_t* c;


    size = nai_offsetof(nai_script_code_copy_t, data);
    c = (nai_script_code_copy_t*)nai_array_push_n(b->lengths, size);
    if (c == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    c->fn = nai_script_code_copy_len;
    c->len = nai_str_len(value);


    size = (size + nai_str_len(value)) & ~(sizeof(intptr_t)-1);
    c = (nai_script_code_copy_t*)nai_array_push_n(b->values, size);
    if (c == 0) {
        b->lengths->count -= nai_offsetof(nai_script_code_copy_t, data);
        rc = nai_sult_from_errno();
        goto _end;
    };

    c->fn = nai_script_code_copy;
    c->len = nai_str_len(value);
    nai_memcpy(c->data, nai_str(value), c->len);

    rc = 0;

_end:
    return rc;
};


static void nai_script_code_variable(nai_script_t* s)
{
    nai_sult_t rc;
    nai_script_value_t v;
    nai_script_code_var_t* c;


    c = (nai_script_code_var_t*)s->ip;

    rc = nai_script_get_value(s, c->index, &v);
    if (rc < 0) {
        nai_script_return(s, rc);
        goto _end;
    };

    nai_memcpy(s->pos, v.ptr, v.len);
    s->pos += v.len;
    s->ip += sizeof(*c);

_end:
    return;
};


static void nai_script_code_variable_len(nai_script_t* s)
{
    nai_sult_t rc;
    nai_script_value_t v;
    nai_script_code_var_t* c;


    c = (nai_script_code_var_t*)s->ip;

    rc = nai_script_get_value(s, c->index, &v);
    if (rc < 0) {
        nai_script_return(s, rc);
        goto _end;
    };

    s->len += v.len;
    s->ip += sizeof(*c);

_end:
    return;
};


static nai_sult_t nai_script_add_code_variable(
    nai_script_compile_value_t* b, const nai_mem_t* value)
{
    nai_sult_t rc;
    nai_script_code_var_t* c;


    rc = nai_script_get_variable(b->sc, value, 0);
    if (rc < 0) {
        goto _end;
    };


    c = (nai_script_code_var_t*)nai_array_push_n(b->lengths, sizeof(*c));
    if (c == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    c->fn = nai_script_code_variable_len;
    c->index = (nai_int_t)rc;


    c = (nai_script_code_var_t*)nai_array_push_n(b->values, sizeof(*c));
    if (c == 0) {
        rc = nai_sult_from_errno();
        b->lengths->count -= sizeof(*c);
        goto _end;
    };

    c->fn = nai_script_code_variable;
    c->index = (nai_int_t)rc;


    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_script_add_code_return(nai_script_compile_value_t* b)
{
    nai_sult_t rc;
    nai_script_code_t* c;


    c = (nai_script_code_t*)nai_array_push_n(b->lengths, sizeof(*c));
    if (c == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    c->fn = 0;


    c = (nai_script_code_t*)nai_array_push_n(b->values, sizeof(*c));
    if (c == 0) {
        rc = nai_sult_from_errno();
        b->lengths->count -= sizeof(*c);
        goto _end;
    };

    c->fn = 0;


    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_script_expn_compile(
    nai_script_expn_t* e, nai_script_vartable_t* s, const nai_str_t* str)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t bracket;
    char ch;
    char* p;
    char* pend;
    char* start;
    size_t alloc;
    nai_sult_t rc;
    nai_mem_t name;
    nai_array_t values;
    nai_array_t lengths;
    nai_script_compile_value_t cv;


    p = nai_str(str);
    pend = p + nai_str_len(str);

    /* count the number of variables */
    n = 0;
    for ( ; p < pend; p ++) {
        ch = p[0];
        switch (ch) {
        case '$':
            n ++;
            break;
        default:
            break;
        };
    };

    if (n == 0) {
        e->value = *str;
        e->values = 0;
        e->lengths = 0;
        rc = 0;
        goto _end;
    };


    nai_array_init(&lengths, sizeof(char), s->pool);
    nai_array_init(&values, sizeof(char), s->pool);


    /* allocate buffer of length codes */
    alloc = sizeof(nai_script_code_var_t) * n + 
        sizeof(nai_script_code_copy_t) * (n + 1) + 
        sizeof(nai_script_code_t);

    r = nai_array_reserve(&lengths, alloc);
    if (r < 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };


    /* allocate buffer of value codes */
    alloc += (nai_str_len(str) + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

    r = nai_array_reserve(&values, alloc);
    if (r < 0) {
        rc = nai_sult_from_errno();
        goto _fail;
    };


    cv.sc = s;
    cv.values = &values;
    cv.lengths = &lengths;


    /* start */
    bracket = 0;
    p = nai_str(str);
    for ( ; p < pend; ) {
        ch = *p ++;


        if (ch != '$') {

            start = p - 1;
            for ( ; p < pend; p ++) {
                ch = p[0];
                if (ch == '$') {
                    break;
                };
            };

            nai_str_setm(&name, start, p - start);
            rc = nai_script_add_code_copy(&cv, &name);
            if (rc < 0) {
                goto _fail;
            };
            continue;
        };


        if (p >= pend) {
            rc = nai_errno_to_sult(EINVAL);
            goto _fail;
        };

        ch = p[0];
        if (ch >= '0' && ch <= '9') {
            nai_str_setm(&name, p, 1);
            p ++;

        } else {

            if (ch == '{') {
                bracket = 1;
                p ++;
            };

            nai_str_setm(&name, p, 0);
            for ( ; p < pend; p ++) {

                if (ch == '}' && bracket) {
                    bracket = 0;
                    p ++;
                    break;
                };

                if (ch != '_' && !nai_isalnum(ch)) {
                    break;
                };

                name.len ++;
            };

            if (bracket) {
                rc = nai_errno_to_sult(EINVAL);
                goto _fail;
            };

            if (nai_str_len(&name) <= 0) {
                rc = nai_errno_to_sult(EINVAL);
                goto _fail;
            };
        };

        rc = nai_script_add_code_variable(&cv, &name);
        if (rc < 0) {
            goto _fail;
        };
    };


    rc = nai_script_add_code_return(&cv);
    if (rc < 0) {
        goto _fail;
    };


    e->value = *str;
    e->values = (nai_script_code_t*)values.elts;
    e->lengths = (nai_script_code_t*)lengths.elts;
    rc = 0;


_end:
    return rc;

_fail:
    nai_array_close(&values);
    nai_array_close(&lengths);
    goto _end;
};



//////////////////////////////////////////////////////////////////////////////
// script code generate


struct nai_script_stmt_s {
    nai_script_stmt_t* next;
    nai_script_stmt_t* list;
    nai_script_code_t* code;
    uint32_t type;
};


static void nai_script_code_jump(nai_script_t* s)
{
    nai_script_code_jump_t* c;


    c = (nai_script_code_jump_t*)s->ip;
    nai_script_jump(s, c);

    return;
};


static void nai_script_code_link(
    nai_script_codegen_t* c, nai_script_stmt_t* top, uint8_t* end)
{
    uint8_t* next;
    nai_script_code_jump_t* jump;
    nai_script_code_loop_t* loop;
    nai_script_stmt_t* last;
    nai_script_stmt_t* free;


    next = 0;
    if (top->type == NAI_SCRIPT_CODE_LOOP) {
        loop = (nai_script_code_loop_t*)top->code;
        if (loop->next) {
            next = (end - sizeof(nai_script_code_jump_t));
        } else {
            next = (uint8_t*)top->code;
        };
    };

    last = top;
    while (last) {
        switch (last->type) {
        case NAI_SCRIPT_CODE_LOOP:
            loop = (nai_script_code_loop_t*)last->code;
            loop->off = end - (uint8_t*)&loop->off;
            break;
        case NAI_SCRIPT_CODE_CONTINUE:
            if (top->type != NAI_SCRIPT_CODE_LOOP) {
                assert(0);
            };

            jump = (nai_script_code_jump_t*)last->code;
            jump->off = next - (uint8_t*)&jump->off;
            break;
        case NAI_SCRIPT_CODE_BREAK:
            if (top->type != NAI_SCRIPT_CODE_LOOP) {
                assert(0);
            };
            jump = (nai_script_code_jump_t*)last->code;
            jump->off = end - (uint8_t*)&jump->off;
            break;

        case NAI_SCRIPT_CODE_IF:
        case NAI_SCRIPT_CODE_ELIF:
        case NAI_SCRIPT_CODE_ELSE:
            jump = (nai_script_code_jump_t*)last->code;
            jump->off = end - (uint8_t*)&jump->off;
            break;

        default:
            assert(0);
            break;
        };

        free = last;
        last = last->next;
        free->next = c->free;
        c->free = free;
    };

    return;
};


static nai_script_stmt_t* nai_script_stmt_alloc(nai_script_codegen_t* c)
{
    nai_script_stmt_t* stmt;


    stmt = c->free;
    if (stmt == 0) {
        stmt = (nai_script_stmt_t*)(c->pool_tmp ? 
            nai_palloc(c->pool_tmp, sizeof(*stmt)) : 
            nai_malloc(sizeof(*stmt)));
    };

    return stmt;
};


static void nai_script_stmt_cleanup(nai_script_stmt_t* list, nai_int_t sub)
{
    nai_script_stmt_t* stmt;
    nai_script_stmt_t* next;


    stmt = list;
    while (stmt) {
        if (sub && stmt->list) {
            nai_script_stmt_cleanup(stmt->list, 0);
        };

        next = stmt->next;
        nai_free(stmt);
        stmt = next;
    };
};


nai_sult_t nai_script_codegen_init(nai_script_codegen_t* c)
{
    nai_sult_t rc;


    c->flags = 0;
    c->code = 0;
    c->top = 0;
    c->free = 0;
    c->pool_tmp = 0;
    rc = 0;

    return rc;
};


nai_sult_t nai_script_codegen_open(
    nai_script_codegen_t* c, nai_array_t* out, nai_pool_t* tmp)
{
    nai_sult_t rc;


    c->code = out;
    c->pool_tmp = tmp;
    rc = 0;

    return rc;
};


nai_sult_t nai_script_codegen_close(nai_script_codegen_t* c)
{
    nai_sult_t rc;


    if (c->pool_tmp) {
        nai_script_stmt_cleanup(c->top, 1);
        nai_script_stmt_cleanup(c->free, 0);
    };


    nai_script_codegen_init(c);
    rc = 0;

    return rc;
};


nai_sult_t nai_script_add_code(
    nai_script_codegen_t* c, nai_script_code_f fn, uint32_t type, 
    size_t size, nai_script_code_t** pv)
{
    size_t alloc;
    nai_sult_t rc;
    nai_script_code_t* code;
    nai_script_code_loop_t* loop;
    nai_script_stmt_t* seg;
    nai_script_stmt_t* stmt;
    nai_script_stmt_t* last;


    /* check type and get min size */
    switch (type) {
    case NAI_SCRIPT_CODE_NORMAL:
        alloc = sizeof(nai_script_code_t);
        break;
    case NAI_SCRIPT_CODE_IF:
    case NAI_SCRIPT_CODE_ELIF:
    case NAI_SCRIPT_CODE_ELSE:
    case NAI_SCRIPT_CODE_BREAK:
    case NAI_SCRIPT_CODE_CONTINUE:
        alloc = sizeof(nai_script_code_jump_t);
        break;
    case NAI_SCRIPT_CODE_LOOP:
        alloc = sizeof(nai_script_code_loop_t);
        break;
    default:
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    /* get allocate size */
    if (size != 0) {
        if (size < alloc) {
            rc = nai_errno_to_sult(EINVAL);
            goto _end;
        };
        if (size & (sizeof(void*)-1)) {
            rc = nai_errno_to_sult(EINVAL);
            goto _end;
        };

        alloc = size;
    };


    if (c->end) {
        rc = nai_errno_to_sult(EPERM);
        goto _end;
    };


    /* process special code */
    seg = 0;
    switch (type) {
    case NAI_SCRIPT_CODE_ELIF:
    case NAI_SCRIPT_CODE_ELSE:
        /* the code 'else if' and 'else' should follow 'if' */
        if (!c->last_if) {
            rc = nai_errno_to_sult(EINVAL);
            goto _end;
        };
        break;

    case NAI_SCRIPT_CODE_BREAK:
    case NAI_SCRIPT_CODE_CONTINUE:
        /* the code 'break' and 'continue' is belong a loop segment */
        seg = c->top;
        for ( ; seg; seg = seg->next) {
            if (seg->type == NAI_SCRIPT_CODE_LOOP) {
                break;
            };
        };
        if (seg == 0) {
            rc = nai_errno_to_sult(EINVAL);
            goto _end;
        };
        break;

    default:
        break;
    };


    stmt = 0;
    if (type != NAI_SCRIPT_CODE_NORMAL) {
        stmt = nai_script_stmt_alloc(c);
        if (stmt == 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };
    };


    code = (nai_script_code_t*)nai_array_push_n(c->code, alloc);
    if (code == 0) {
        if (stmt) {
            stmt->next = c->free;
            c->free = stmt;
        };
        rc = nai_sult_from_errno();
        goto _end;
    };

    if (type != NAI_SCRIPT_CODE_LOOP) {
        code->fn = fn;
    } else {
        loop = (nai_script_code_loop_t*)code;
        loop->fn = fn;
        loop->next = 0;
    };


    last = 0;
    if (c->last_if) {
        last = c->top;
        c->top = last->next;
        c->last_if = 0;
    };

    if (type != NAI_SCRIPT_CODE_NORMAL) {
        stmt->list = 0;
        stmt->code = code;
        stmt->type = type;

        switch (type) {
        case NAI_SCRIPT_CODE_ELIF:
        case NAI_SCRIPT_CODE_ELSE:
            if (last) {
                last->next = last->list;
                last->list = 0;
                stmt->list = last;
                last = 0;
            };

            /* fallthrough */

        case NAI_SCRIPT_CODE_IF:
        case NAI_SCRIPT_CODE_LOOP:
            stmt->next = c->top;
            c->top = stmt;
            break;

        case NAI_SCRIPT_CODE_BREAK:
        case NAI_SCRIPT_CODE_CONTINUE:
            assert(seg);
            stmt->next = seg->list;
            seg->list = stmt;
            break;

        default:
            break;
        };
    };

    if (last) {
        last->next = last->list;
        last->list = 0;
        nai_script_code_link(c, last, (uint8_t*)code);
    };

    if (pv) {
        pv[0] = code;
    };

    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_script_add_loop(
    nai_script_codegen_t* c, nai_script_code_f fn, nai_script_code_f next, 
    size_t size, nai_script_code_t** pv)
{
    nai_sult_t rc;
    nai_script_code_loop_t* code;


    rc = nai_script_add_code(c, fn, 
        NAI_SCRIPT_CODE_LOOP, size, (nai_script_code_t**)&code);
    if (rc < 0) {
        goto _end;
    };

    code->next = next;

    if (pv) {
        pv[0] = (nai_script_code_t*)code;
    };

    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_script_end_stmt(nai_script_codegen_t* c)
{
    uint8_t* end;
    nai_sult_t rc;
    nai_script_stmt_t* top;
    nai_script_stmt_t* last;
    nai_script_code_loop_t* loop;
    nai_script_code_jump_t* code;


    top = c->top;
    if (top == 0) {
        rc = nai_errno_to_sult(EPERM);
        goto _end;
    };

    /* save old postion */
    end = (uint8_t*)c->code->elts + c->code->count;

    /* add end code of loop */
    if (top->type == NAI_SCRIPT_CODE_LOOP) {
        loop = (nai_script_code_loop_t*)top->code;
        code = (nai_script_code_jump_t*)
            nai_array_push_n(c->code, sizeof(*code));
        if (code == 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };

        code->off = (uint8_t*)loop - (uint8_t*)&code->off;
        code->fn = loop->next;
        if (code->fn == 0) {
            code->fn = nai_script_code_jump;
        };
    };

    /* handle last 'if' */
    if (c->last_if) {
        last = top;
        top = last->next;
        c->top = top;
        c->last_if = 0;

        last->next = last->list;
        last->list = 0;
        nai_script_code_link(c, last, end);
    };

    /* check code 'if' */
    switch (top->type) {
    case NAI_SCRIPT_CODE_IF:
    case NAI_SCRIPT_CODE_ELIF:
        c->last_if = 1;
        rc = 0;
        goto _end;

    case NAI_SCRIPT_CODE_ELSE:
    case NAI_SCRIPT_CODE_LOOP:
        end = (uint8_t*)c->code->elts + c->code->count;
        break;

    default:
        rc = NAI_E_INTERNAL;
        goto _end;
    };


    /* handle link */
    last = top;
    c->top = last->next;
    last->next = last->list;
    last->list = 0;
    nai_script_code_link(c, last, end);
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_script_end_code(nai_script_codegen_t* c)
{
    nai_sult_t rc;
    nai_script_stmt_t* top;


    top = c->top;
    if (top != 0) {
        rc = nai_errno_to_sult(EPERM);
        goto _end;
    };

    rc = nai_script_add_code(c, 0, 0, 0, 0);
    if (rc < 0) {
        goto _end;
    };

    c->end = 1;
    rc = 0;

_end:
    return rc;
};


