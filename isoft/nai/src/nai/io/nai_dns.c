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
/// @file       nai_dns.c
/// @brief      
/// @details
/// @date       2021-03-08
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_dns.h"
#include "nai/io/nai_io.h"
#include "nai/os/nai_socket.h"
#include "nai/os/nai_thread.h"
#include "nai/os/nai_tlocal.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"

#if (NAI_HAVE_NETDB_H)
#include <netdb.h>
#endif


#define NAI_DNS_QTYPE_NAME           0
#define NAI_DNS_QTYPE_SERV           1
#define NAI_DNS_QTYPE_ADDR           2
#define NAI_DNS_QTYPE_ADDR6          3
#define NAI_DNS_QTYPE_ROOT           15
#define NAI_DNS_RTYPE_LOCK           15


#define NAI_DNS_TYPE_A              1
#define NAI_DNS_TYPE_CNAME          5
#define NAI_DNS_TYPE_PTR            12
#define NAI_DNS_TYPE_AAAA           28
#define NAI_DNS_TYPE_SRV            33
#define NAI_DNS_TYPE_DNAME          39


#define NAI_DNS_CLASS_IN            1
#define NAI_DNS_CLASS_ANY           255


#define NAI_DNS_FORMERR             1
#define NAI_DNS_SERVFAIL            2
#define NAI_DNS_NXDOMAIN            3
#define NAI_DNS_NOTIMP              4
#define NAI_DNS_REFUSED             5


#define NAI_DNS_RECURSION_MAX       50


#define NAI_DNS_UDP_BUFSIZE         2000
#define NAI_DNS_TCP_BUFSIZE         8000


#pragma pack(push, 1)


typedef struct nai_dns_hdr_s {
    union {
        struct {
            uint8_t ident_hi;
            uint8_t ident_lo;
        };
        uint16_t ident;
    };
    union {
        struct {
            uint8_t rd :1;      /* recursion desired */
            uint8_t tc :1;      /* truncated message */
            uint8_t aa :1;      /* authoritive answer */
            uint8_t opcode :4;  /* purpose of message */
            uint8_t qr :1;      /* query/response flag */
            uint8_t rcode :4;   /* response code */
            uint8_t z :3;       /* its z! reserved */
            uint8_t ra :1;      /* recursion available */
        };
        uint16_t flags;
    };
    uint16_t nqs;
    uint16_t nan;
    uint16_t nns;
    uint16_t nnr;
} nai_dns_hdr_t;


typedef struct nai_dns_qry_s {
    uint16_t type;
    uint16_t class;
} nai_dns_qry_t;


typedef struct nai_dns_ans_s {
    uint16_t type;
    uint16_t class;
    int32_t  ttl;
    uint16_t len;
} nai_dns_ans_t;


#pragma pack(pop)


typedef struct nai_dns_connection_s {

    nai_iobase_t str;

    /* read */
    uint8_t* buf;
    size_t buf_off;
    size_t buf_last;
    size_t buf_total;

    /* send */
    nai_int_t vnext;
    nai_int_t vcount;
    nai_bufvec_t v[8];
    nai_list_entry_t send;
    nai_list_entry_t queue;

} nai_dns_connection_t;


typedef struct nai_dns_server_s {

    nai_socknbuf_in_t name;
    nai_dns_t* dns;
    nai_dns_connection_t udp;
    nai_dns_connection_t tcp;
    char strname[64];

} nai_dns_server_t;


#ifndef _NAI_TYPEDEF_DNS_ENTRY_T
#define _NAI_TYPEDEF_DNS_ENTRY_T
typedef struct nai_dns_entry_s nai_dns_entry_t;
#endif
#ifndef _NAI_TYPEDEF_DNS_LINK_T
#define _NAI_TYPEDEF_DNS_LINK_T
typedef struct nai_dns_link_s nai_dns_link_t;
#endif

struct nai_dns_link_s {
    nai_dns_link_t* next;
    nai_dns_serv_t* serv;
    nai_dns_entry_t* src;
    nai_dns_entry_t* dst;
    nai_int_t recursion;
};


struct nai_dns_entry_s {
    nai_rbnode_t ent;
    nai_list_entry_t qent;

    union {
        struct {
            uint32_t qtype:4;
            uint32_t rtype:4;
            uint32_t permanent:1;
            uint32_t complete:1;
            uint32_t query_in4:1;
            uint32_t query_in6:1;
            uint32_t tcp:1;
            uint32_t locked:1;
            uint32_t cname_skiped:1;
        };
        uint32_t flags;
    };

    nai_int_t refs;
    nai_int_t send;
    nai_int_t ttl;
    uint32_t expire;
    uint16_t ident;
    uint16_t len;
    uint16_t len_n;
    uint8_t* query;

    union {
        nai_mem_t name;
        nai_addr_in4_t addr4;
#if (NAI_HAVE_SOCKADDR_IN6)
        nai_addr_in6_t addr6;
#endif
    };

    union {
        struct {
            nai_mem_t mem;
        } hname;
        struct {
            nai_mem_t mem;
        } cname;
        struct {
            nai_int_t count;
            nai_dns_serv_t* elts;
            nai_dns_link_t* link;
        } serv;
        struct {
            struct {
                nai_int_t count;
                nai_addr_in4_t addr;
                nai_addr_in4_t* elts;
            } in4;
#if (NAI_HAVE_SOCKADDR_IN6)
            struct {
                nai_int_t count;
                nai_addr_in6_t addr;
                nai_addr_in6_t* elts;
            } in6;
#endif
        };
        nai_spin_t lock;
    };

    nai_dns_link_t* link;
    nai_dns_query_t* wait;

};



#if (NAI_HAVE_SOCKADDR_IN6)

#define nai_dns_count_addrs(p)                          \
    ((p)->in4.count + (p)->in6.count)                   \


#define nai_dns_clear_addrs(p) {                        \
    (p)->in4.count = 0;                                 \
    (p)->in4.elts = 0;                                  \
    (p)->in6.count = 0;                                 \
    (p)->in6.elts = 0;                                  \
}                                                       \


#define nai_dns_assign_addrs(d, s) {                    \
    (d)->in4.count = (s)->in4.count;                    \
    (d)->in4.elts = (s)->in4.elts;                      \
    (d)->in6.count = (s)->in6.count;                    \
    (d)->in6.elts = (s)->in6.elts;                      \
}                                                       \

#else

#define nai_dns_count_addrs(p)                          \
    ((p)->in4.count)                                    \


#define nai_dns_clear_addrs(p) {                        \
    (p)->in4.count = 0;                                 \
    (p)->in4.elts = 0;                                  \
}                                                       \


#define nai_dns_assign_addrs(d, s) {                    \
    (d)->in4.count = (s)->in4.count;                    \
    (d)->in4.elts = (s)->in4.elts;                      \
}                                                       \


#endif


static nai_int_t nai_dns_set_timeout(nai_dns_t* d, uint32_t timeo);


static nai_int_t nai_dns_query_send(
    nai_dns_t* d, nai_dns_entry_t* ent, nai_int_t tcp);

static nai_int_t nai_dns_entry_free(
    nai_dns_t* d, nai_dns_entry_t* ent, nai_int_t quit);

static nai_dns_entry_t* nai_dns_entry(
    nai_dns_t* d, nai_int_t type, const nai_mem_t* name);



static nai_int_t nai_dns_rcode_to_errno(nai_int_t rcode)
{
    nai_int_t e;

    switch (rcode) {
    case NAI_DNS_FORMERR:
        e = NAI_EFAILED;
        break;
    case NAI_DNS_SERVFAIL:
        e = NAI_EFAILED;
        break;
    case NAI_DNS_NXDOMAIN:
        e = ENOENT;
        break;
    case NAI_DNS_NOTIMP:
    case NAI_DNS_REFUSED:
        e = ENOTSUP;
        break;
    default:
        e = NAI_EFAILED;
        break;
    };

    return e;
};


static nai_int_t nai_dns_ntoh(uint8_t* p, size_t len)
{
#if (NAI_HAVE_BIG_ENDIAN)
    (void)p;
    (void)len;
#else
    size_t n;
    uint8_t b;

    for (n = 0; n < len/2; n ++) {
        b = p[n];
        p[n] = p[len-n-1];
        p[len-n-1] = b;
    };
#endif
    return 0;
};


static void* nai_dns_alloc(nai_dns_t* d, size_t size)
{
    (void)d;

    return nai_malloc(size);
};


static void nai_dns_free(nai_dns_t* d, void* p)
{
    (void)d;

    nai_free(p);
};


//////////////////////////////////////////////////////////////////////////////
// dns string


typedef struct nai_dns_str_s {
    nai_mem_t mem;
    size_t buflen;
    char* buf;
} nai_dns_str_t;


static nai_int_t nai_dns_str_next(uint8_t* start, uint8_t* b, uint8_t* last)
{
    nai_int_t r;
    nai_int_t n;
    uint8_t ch;
    uint8_t* p = b;


    (void)start;

    for ( ; p < last; ) {
        ch = p[0];
        if (ch == 0) {
            p ++;
            break;
        };
        if ((ch & 0xc0) == 0xc0) {
            if (p + 1 >= last) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };
            p += 2;
            break;
            /* n = ((ch & 0x3f) << 8) + p[1]; */
            /* p = start + n; */
        } else {
            n = ch + 1;
            p += n;
        };
    };

    if (p > last) {
        r = -1;
        goto _end;
    };

    r = (nai_int_t)(p - b);

_end:
    return r;
};


static nai_int_t nai_dns_str_init(nai_dns_str_t* str, void* buf, size_t len)
{
    nai_str_setm(&str->mem, (char*)buf, 0);
    str->buf = (char*)buf;
    str->buflen = len;
    return 0;
};


static nai_int_t nai_dns_str_copy(nai_dns_t* d, 
    nai_dns_str_t* str, uint8_t* start, uint8_t* b, uint8_t* last)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t len;
    uint8_t ch;
    uint8_t* s;
    uint8_t* p = b;


    if (str) {
        nai_str_setm(&str->mem, str->buf, 0);
    };

    len = 0;
    for ( ; p < last; ) {
        ch = p[0];
        if (ch == 0) {
            p ++;
            if (len == 0) {
                len ++;
            };
            break;
        };
        if ((ch & 0xc0) == 0xc0) {
            if (p + 1 >= last) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };
            n = ((ch & 0x3f) << 8) + p[1];
            p = start + n;
        } else {
            n = ch + 1;
            p += n;
            len += n;
        };
    };
    if (p > last) {
        r = -1;
        goto _end;
    };

    if (str == 0) {
        r = len;
        goto _end;
    };

    if (len <= (intptr_t)str->buflen) {
        s = (uint8_t*)str->buf;
    } else {
        s = (uint8_t*)nai_dns_alloc(d, len);
        if (s == 0) {
            r = -1;
            goto _end;
        };
    };

    nai_str_setm(&str->mem, s, len-1);

    p = b;
    for (;;) {

        ch = p[0];
        if (ch == 0) {
            p ++;
            break;
        };
        if ((ch & 0xc0) == 0xc0) {
            n = ((ch & 0x3f) << 8) + p[1];
            p = start + n;
        } else {

            if (s != (uint8_t*)nai_str(&str->mem)) {
                s[0] = '.';
                s ++;
            };

            n = ch;
            nai_memcpy(s, p+1, n);
            s += n;
            p += n + 1;
        };
    };

    s[0] = 0;
    r = len;

_end:
    return r;
};


static nai_int_t nai_dns_str_free(nai_dns_t* d, nai_dns_str_t* str)
{
    char* s;

    s = nai_str(&str->mem);
    if (s && s != str->buf) {
        nai_dns_free(d, s);
    };

    return 0;
};



//////////////////////////////////////////////////////////////////////////////
// dns cache lookup


static nai_dns_entry_t* nai_dns_lookup_name(
    nai_rbtree_t* t, const nai_mem_t* name)
{
    nai_int_t c;
    nai_rbnode_t** n = (nai_rbnode_t**)&nai_rbtree_root(t);
    nai_rbnode_t* parent;
    nai_dns_entry_t* e;


    while (*n) {
        parent = *n;
        e = (nai_dns_entry_t*)parent;
        c = nai_str_casecmp(&e->name, name);
        if (c == 0) {
            goto _end;
        };
        if (c > 0) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    e = 0;

_end:
    return e;
};


static nai_dns_entry_t* nai_dns_lookup_addr(
    nai_rbtree_t* t, nai_int_t type, const void* addr)
{
    nai_int_t c;
    nai_rbnode_t** n = (nai_rbnode_t**)&nai_rbtree_root(t);
    nai_rbnode_t* parent;
    nai_dns_entry_t* e;


    if (type == AF_INET) {
        while (*n) {
            parent = *n;
            e = (nai_dns_entry_t*)parent;
            c = nai_memcmp(&e->addr4, addr, sizeof(nai_addr_in4_t));
            if (c == 0) {
                goto _end;
            };
            if (c > 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };

#if (NAI_HAVE_SOCKADDR_IN6)
    } else {
        while (*n) {
            parent = *n;
            e = (nai_dns_entry_t*)parent;
            c = nai_memcmp(&e->addr6, addr, sizeof(nai_addr_in6_t));
            if (c == 0) {
                goto _end;
            };
            if (c > 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
#endif
    };

    e = 0;

_end:
    return e;
};


static nai_int_t nai_dns_insert_name(
    nai_rbtree_t* t, nai_dns_entry_t* q)
{
    nai_int_t c;
    nai_rbnode_t** n = (nai_rbnode_t**)&nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nai_dns_entry_t* e = 0;


    while (*n) {
        parent = *n;
        e = (nai_dns_entry_t*)parent;
        c = nai_str_casecmp(&e->name, &q->name);
        if (c > 0) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    nai_rbtree_link(t, &q->ent, parent, n);
    nai_rbtree_color(t, &q->ent);
    return 0;
};


static nai_int_t nai_dns_insert_addr(
    nai_rbtree_t* t, nai_dns_entry_t* q)
{
    nai_int_t c;
    nai_rbnode_t** n = (nai_rbnode_t**)&nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nai_dns_entry_t* e = 0;


    if (q->qtype == NAI_DNS_QTYPE_ADDR) {
        while (*n) {
            parent = *n;
            e = (nai_dns_entry_t*)parent;
            c = nai_memcmp(&e->addr4, &q->addr4, sizeof(nai_addr_in4_t));
            if (c > 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };

#if (NAI_HAVE_SOCKADDR_IN6)
    } else {
        while (*n) {
            parent = *n;
            e = (nai_dns_entry_t*)parent;
            c = nai_memcmp(&e->addr6, &q->addr6, sizeof(nai_addr_in6_t));
            if (c > 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
#endif
    };

    nai_rbtree_link(t, &q->ent, parent, n);
    nai_rbtree_color(t, &q->ent);
    return 0;
};



//////////////////////////////////////////////////////////////////////////////
// dns notify


#define NAI_DNS_NOTIFY_RESET        1
#define NAI_DNS_NOTIFY_COMPLETE     2


static nai_int_t nai_dns_notify(
    nai_dns_t* d, nai_dns_entry_t* q, nai_int_t flags);


static nai_int_t nai_dns_call(
    nai_dns_t* d, nai_dns_entry_t* q, nai_dns_result_t* rs)
{
    nai_int_t r;
    nai_int_t exit;
    nai_int_t* pexit;
    nai_dns_query_t* c;


    if (q->wait == 0) {
        r = 0;
        goto _end;
    };

    pexit = d->pexit;
    if (pexit == 0) {
        pexit = d->pexit = &exit;
        exit = 0;
    };

    for ( ; q->wait; ) {
        c = q->wait;
        q->wait = c->next;
        c->ent = 0;
        if (c->cb) {
            c->cb(c, rs);

            /* is dns closed */
            if (pexit[0]) {
                r = 1;
                goto _end;
            };
        };
    };

    if (d->pexit == &exit) {
        d->pexit = 0;
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_dns_notify_failed(
    nai_dns_t* d, nai_dns_entry_t* q, nai_int_t errcode)
{
    nai_int_t r;
    nai_dns_link_t* l;
    nai_dns_serv_t* s;
    nai_dns_result_t rs;


    for ( ; q->link; ) {
        l = q->link;
        q->link = l->next;
        l->next = 0;
        l->dst = 0;
        s = l->serv;
        s->error = errcode;
        nai_dns_clear_addrs(s);

        r = nai_dns_notify(d, l->src, 0);
        if (r) {
            /* dns is closed */
            goto _end;
        };
    };

    rs.error = errcode;
    rs.type = 0;

    r = nai_dns_call(d, q, &rs);

_end:
    return r;
};


static nai_int_t nai_dns_result(nai_dns_t* d, 
    nai_dns_entry_t* q, nai_dns_result_t* rs)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t wait;
    nai_dns_entry_t* f;
    nai_dns_link_t* l;
    nai_dns_serv_t* s;


    rs->error = 0;
    rs->type = q->rtype;
    rs->ttl = q->ttl;

    switch (q->rtype) {
    case NAI_DNS_RTYPE_CNAME:
        /* recursive query */
        f = nai_dns_entry(d, NAI_DNS_QTYPE_NAME, &q->cname.mem);
        if (f == 0) {
            rs->error = nai_errno;
            r = -1;
            goto _end;
        };

        if (f->complete) {
            r = nai_dns_result(d, f, rs);
        } else {
            nai_errno = NAI_EAGAIN;
            r = -1;
        };
        goto _end;

    case NAI_DNS_RTYPE_HNAME:
        nai_str_setm(&rs->name, 
            nai_str(&q->cname.mem), nai_str_len(&q->cname.mem));
        break;

    case NAI_DNS_RTYPE_ADDR:
        nai_dns_assign_addrs(rs, q);
        break;

    case NAI_DNS_RTYPE_SERV:
        rs->serv.count = q->serv.count;
        rs->serv.elts = q->serv.elts;
        wait = 0;
        for (n = 0; n < q->serv.count; n ++) {
            s = &q->serv.elts[n];

            l = &q->serv.link[n];
            if (l->dst == 0) {
                /* recursive query */
                f = nai_dns_entry(d, 
                    NAI_DNS_QTYPE_NAME, (nai_mem_t*)&s->name);
                if (f == 0) {
                    s->error = nai_errno;
                    continue;
                };

                s->error = 0;
                l->recursion = 0;
                l->dst = f;
                l->next = f->link;
                f->link = l;
                if (f->complete) {
                    if (f->rtype == NAI_DNS_RTYPE_ADDR) {
                        s->ttl = f->ttl;
                        nai_dns_assign_addrs(s, f);
                    };
                };
            };
            if (l->dst->complete == 0) {
                wait = 1;
            };
        };
        if (wait) {
            nai_errno = NAI_EAGAIN;
            r = -1;
            goto _end;
        };
        break;

    default:
        assert(0);
        break;
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_dns_notify(
    nai_dns_t* d, nai_dns_entry_t* q, nai_int_t flags)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t wait;
    nai_dns_entry_t* f;
    nai_dns_query_t* e;
    nai_dns_query_t* c;
    nai_dns_link_t* l;
    nai_dns_link_t* m;
    nai_dns_serv_t* s;
    nai_dns_result_t rs;


    rs.error = 0;
    rs.type = q->rtype;
    rs.ttl = q->ttl;

    switch (q->rtype) {
    case NAI_DNS_RTYPE_CNAME:
        nai_str_setm(&rs.name, 
            nai_str(&q->cname.mem), nai_str_len(&q->cname.mem));

        /* recursive query */
        f = nai_dns_entry(d, NAI_DNS_QTYPE_NAME, &q->cname.mem);
        if (f == 0) {
            rs.error = nai_errno;
            r = nai_dns_notify_failed(d, q, rs.error);
            nai_errno = rs.error;
            goto _end;
        };

        /* */
        rs.error = nai_dns_rcode_to_errno(NAI_DNS_NXDOMAIN);

        /* move all request to f */
        e = q->wait;
        q->wait = 0;
        for ( ; e; ) {
            c = e;
            e = e->next;
            c->recursion ++;
            if (c->recursion < NAI_DNS_RECURSION_MAX) {
                c->ent = f;
                c->next = f->wait;
                f->wait = c;
            } else {
                c->next = q->wait;
                q->wait = c;
            };
        };

        /* move all link to f */
        m = q->link;
        q->link = 0;
        for ( ; m; ) {
            l = m;
            m = m->next;
            if (l->recursion < NAI_DNS_RECURSION_MAX) {
                l->recursion ++;
                l->dst = f;
                l->next = f->link;
                f->link = l;
                continue;
            };

            /* this one is an old entry which reach max recursion, 
             * we only notify it at first time when sult is zero.
             */
            l->next = q->link;
            q->link = l;
            s = l->serv;
            if (flags & NAI_DNS_NOTIFY_COMPLETE && s->error == 0) {
                s->error = rs.error;
                r = nai_dns_notify(d, l->src, 0);
                if (r) {
                    /* dns is closed */
                    goto _end;
                };
            };
        };

        if (f->complete) {
            r = nai_dns_notify(d, f, 0);
            if (r) {
                /* dns is closed */
                goto _end;
            };
        };

        break;

    case NAI_DNS_RTYPE_HNAME:
        nai_str_setm(&rs.name, 
            nai_str(&q->cname.mem), nai_str_len(&q->cname.mem));
        break;

    case NAI_DNS_RTYPE_ADDR:
        nai_dns_assign_addrs(&rs, q);

        if (flags & NAI_DNS_NOTIFY_COMPLETE) {
            l = q->link;
            for ( ; l; ) {
                s = l->serv;
                f = l->src;
                l = l->next;
                s->error = 0;
                s->ttl = rs.ttl;
                nai_dns_assign_addrs(s, &rs);

                r = nai_dns_notify(d, f, 0);
                if (r) {
                    /* dns is closed */
                    goto _end;
                };
            };
        };
        break;

    case NAI_DNS_RTYPE_SERV:
        rs.serv.count = q->serv.count;
        rs.serv.elts = q->serv.elts;
        wait = 0;
        for (n = 0; n < q->serv.count; n ++) {
            s = &q->serv.elts[n];
            if (s->error) {
                if (!(flags & NAI_DNS_NOTIFY_RESET)) {
                    continue;
                };
            };

            l = &q->serv.link[n];
            if (l->dst == 0) {
                /* recursive query */
                f = nai_dns_entry(d, 
                    NAI_DNS_QTYPE_NAME, (nai_mem_t*)&s->name);
                if (f == 0) {
                    s->error = nai_errno;
                    continue;
                };

                s->error = 0;
                l->recursion = 0;
                l->dst = f;
                l->next = f->link;
                f->link = l;
                if (f->complete) {
                    if (f->rtype == NAI_DNS_RTYPE_ADDR) {
                        s->ttl = f->ttl;
                        nai_dns_assign_addrs(s, f);
                    } else {
                        r = nai_dns_notify(d, f, 0);
                        if (r) {
                            /* dns is closed */
                            goto _end;
                        };

                        /* force set wait, 
                         * maybe the status has been modified 
                         * in the previous call.
                         */
                        wait = 1;
                        continue;
                    };
                };
            };
            if (l->dst->complete == 0) {
                wait = 1;
            };
        };
        if (wait) {
            r = 0;
            goto _end;
        };
        break;

    default:
        break;
    };

    r = nai_dns_call(d, q, &rs);

_end:
    return r;
};


static nai_int_t nai_dns_query_failed(
    nai_dns_t* d, nai_dns_entry_t* q, nai_int_t errcode)
{
    nai_int_t r;


    nai_list_entry_remove(&q->qent);
    switch (q->qtype) {
    case NAI_DNS_QTYPE_NAME:
    case NAI_DNS_QTYPE_SERV:
    case NAI_DNS_QTYPE_ADDR:
    case NAI_DNS_QTYPE_ADDR6:
        nai_rbtree_erase(&d->type[q->qtype], &q->ent);
        break;
    default:
        break;
    };

    r = nai_dns_notify_failed(d, q, errcode);
    if (r) {
        /* dns is closed */
        goto _end;
    };

    r = 0;

_end:
    q->refs --;
    if (q->refs <= 0) {
        nai_dns_entry_free(d, q, 0);
    };
    return r;
};


static nai_int_t nai_dns_query_complete(nai_dns_t* d, nai_dns_entry_t* q)
{
    nai_int_t r;


    q->complete = 1;
    if (!q->locked && q->query) {
        nai_dns_free(d, q->query);
        q->query = 0;
    };
    if (!q->locked) {
        if (nai_list_is_empty(&d->qexpire)) {
            nai_dns_set_timeout(d, d->expire);
        };
        nai_list_entry_remove(&q->qent);
        nai_list_insert_tail(&d->qexpire, &q->qent);
        q->expire = nai_tickcache_to_msec32() + d->expire;
    };

    r = nai_dns_notify(d, q, NAI_DNS_NOTIFY_COMPLETE);

    return r;
};


static nai_int_t nai_dns_query_timedout(nai_dns_t* d, nai_dns_entry_t* e)
{
    nai_int_t r;


    if (e->rtype == NAI_DNS_RTYPE_ADDR) {
        if (e->query_in4) {
            e->query_in4 = 0;
            e->in4.count = 0;
            e->in4.elts = 0;
        };
#if (NAI_HAVE_SOCKADDR_IN6)
        if (e->query_in6) {
            e->query_in6 = 0;
            e->in6.count = 0;
            e->in6.elts = 0;
        };
#endif
        if (nai_dns_count_addrs(e) > 0) {
            r = nai_dns_query_complete(d, e);
            goto _end;
        };
    };

    nai_dns_query_failed(d, e, ETIMEDOUT);
    nai_errno = ETIMEDOUT;
    r = -1;

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// dns protocol handle


static nai_int_t nai_dns_handle_name(
    nai_dns_t* d, nai_dns_hdr_t* hdr, 
    nai_dns_qry_t* qry, uint8_t* start, uint8_t* b, size_t len)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t n, c;
    nai_int_t nan;
    nai_int_t naddrs;
    uint8_t* addrs;
    uint8_t* cname;
    nai_mem_t name;
    nai_dns_entry_t* q;
    nai_dns_ans_t* pans;
    nai_dns_ans_t ans;
    nai_dns_str_t str;
    nai_dns_str_t copy;
    char buf[256];


    nai_dns_str_init(&str, buf, sizeof(buf));


    r = nai_dns_str_copy(d, &str, start, start + sizeof(*hdr), b + len);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, nai_errno, 
            "copy compression string from dns response failed");
        goto _end;
    };

    name = str.mem;
    q = nai_dns_lookup_name(&d->type[NAI_DNS_QTYPE_NAME], &name);
    if (q == 0) {
        nai_log_info(NAI_LOG_CORE, ENOENT, 
            "unexcept dns response about %.*s", 
            nai_str_len(&name), nai_str(&name));
        r = -1;
        goto _end;
    };

    if (q->ident != hdr->ident && 
        q->ident + 1 != hdr->ident) {
        nai_log_info(NAI_LOG_CORE, EINVAL, 
            "incorrect seqnum %d in dns response about %.*s", 
            q->ident, nai_str_len(&name), nai_str(&name));
        r = -1;
        goto _end;
    };


    if (hdr->tc) {
        nai_list_entry_remove(&q->qent);

        /* resend it */
        r = nai_dns_query_send(d, q, 1);
        goto _end;
    };


    /* check result code */
    if (hdr->rcode == 0) {
        switch (qry->type) {
        case NAI_DNS_TYPE_A:
            if (!q->query_in4) {
                r = -1;
                goto _end;
            };
            if (!hdr->nan) {
                q->query_in4 = 0;
                q->in4.count = 0;
                q->in4.elts = 0;
                goto _check;
            };
            break;

#if (NAI_HAVE_SOCKADDR_IN6)
        case NAI_DNS_TYPE_AAAA:
            if (!q->query_in6) {
                r = -1;
                goto _end;
            };
            if (!hdr->nan) {
                q->query_in6 = 0;
                q->in6.count = 0;
                q->in6.elts = 0;
                goto _check;
            };
            break;
#endif
        default:
            break;
        };

    } else {

        switch (qry->type) {
        case NAI_DNS_TYPE_A:
            q->query_in4 = 0;
            break;

#if (NAI_HAVE_SOCKADDR_IN6)
        case NAI_DNS_TYPE_AAAA:
            q->query_in6 = 0;
            break;
#endif
        default:
            break;
        };


        /* failed */
        if (!q->query_in4 && !q->query_in6) {
            ec = nai_dns_rcode_to_errno(hdr->rcode);
            if (ec == ENOENT) {
                goto _check;
            };
            goto _fail;
        };

        r = 0;
        goto _end;
    };


    nai_log_debug(NAI_LOG_CORE, 0, 
        "check answer of '%.*s', qtype %d", 
        nai_str_len(&name), nai_str(&name), qry->type);


    /* check answers */
    naddrs = 0;
    cname = 0;
    c = 0;
    n = 0;
    nan = hdr->nan;
    for ( ; n < nan; n ++) {
        r = nai_dns_str_next((uint8_t*)hdr, b + c, b + len);
        if (r < 0) {
            nai_log_info(NAI_LOG_CORE, nai_errno, 
                "bad string in dns response about %.*s", 
                nai_str_len(&name), nai_str(&name));
            ec = nai_errno;
            goto _fail;
        };

        c += r;
        if (c + sizeof(nai_dns_ans_t) > len) {
            nai_log_info(NAI_LOG_CORE, EINVAL, 
                "incorrent length of A/AAAA record "
                "in dns response about %.*s", 
                nai_str_len(&name), nai_str(&name));
            ec = EINVAL;
            goto _fail;
        };

        pans = (nai_dns_ans_t*)(b + c);
        ans.type = nai_ntohs_ua(&pans->type);
        ans.class = nai_ntohs_ua(&pans->class);
        ans.ttl = nai_ntohl_ua((uint32_t*)&pans->ttl);
        ans.len = nai_ntohs_ua(&pans->len);

        if (ans.class != NAI_DNS_CLASS_IN) {
            nai_log_info(NAI_LOG_CORE, EINVAL, 
                "unexpect class %d in dns response about %.*s", 
                ans.class, nai_str_len(&name), nai_str(&name));
            ec = EINVAL;
            goto _fail;
        };

        if (q->ttl > ans.ttl) {
            q->ttl = ans.ttl;
            if (q->ttl < 0) {
                q->ttl = 0;
            };
        };

        c += sizeof(ans);

        nai_log_debug(NAI_LOG_CORE, 0 ,"have a record type %d", ans.type);

        switch (ans.type) {
        case NAI_DNS_TYPE_A:
            if (ans.type != qry->type) {
                nai_log_info(NAI_LOG_CORE, EINVAL, 
                    "record type A is mismatch query type AAAA "
                    "in dns response about %.*s", 
                    nai_str_len(&name), nai_str(&name));
                ec = EINVAL;
                goto _fail;
            };
            if (ans.len != sizeof(nai_addr_in4_t)) {
                nai_log_info(NAI_LOG_CORE, EINVAL, 
                    "incorrent length of A record "
                    "in dns response about %.*s", 
                    nai_str_len(&name), nai_str(&name));
                ec = EINVAL;
                goto _fail;
            };

            naddrs ++;
            break;
#if (NAI_HAVE_SOCKADDR_IN6)
        case NAI_DNS_TYPE_AAAA:
            if (ans.type != qry->type) {
                nai_log_info(NAI_LOG_CORE, EINVAL, 
                    "record type AAAA is mismatch query type A "
                    "in dns response about %.*s", 
                    nai_str_len(&name), nai_str(&name));
                ec = EINVAL;
                goto _fail;
            };
            if (ans.len != sizeof(nai_addr_in6_t)) {
                nai_log_info(NAI_LOG_CORE, EINVAL, 
                    "incorrent length of AAAA record "
                    "in dns response about %.*s", 
                    nai_str_len(&name), nai_str(&name));
                ec = EINVAL;
                goto _fail;
            };

            naddrs ++;
            break;
#endif
        case NAI_DNS_TYPE_CNAME:
            cname = b + c;
            break;
        case NAI_DNS_TYPE_DNAME:
            break;
        default:
            nai_log_info(NAI_LOG_CORE, EINVAL, 
                "unexpect record type %d "
                "in dns response about %.*s", 
                ans.type, nai_str_len(&name), nai_str(&name));
            ec = EINVAL;
            goto _fail;
        };

        c += ans.len;
        if (c > (intptr_t)len) {
            nai_log_info(NAI_LOG_CORE, EINVAL, 
                "incorrent length of A/AAAA record "
                "in dns response about %.*s", 
                nai_str_len(&name), nai_str(&name));
            ec = EINVAL;
            goto _fail;
        };
    };


    nai_log_debug(NAI_LOG_CORE, 0, 
        "read answer, qtype %d, address %d", qry->type, naddrs);


    /* fill address */
    if (naddrs) {

        if (q->rtype == 0) {
            q->rtype = NAI_DNS_RTYPE_ADDR;
            q->in4.count = 0;
#if (NAI_HAVE_SOCKADDR_IN6)
            q->in6.count = 0;
#endif
        };

        switch (qry->type) {
        case NAI_DNS_TYPE_A:
            if (naddrs == 1) {
                addrs = (uint8_t*)&q->in4.addr;
                q->in4.elts = (nai_addr_in4_t*)addrs;
            } else {
                addrs = nai_dns_alloc(d, naddrs*sizeof(nai_addr_in4_t));
                if (addrs == 0) {
                    nai_log_alert(NAI_LOG_CORE, 
                        nai_errno, "alloc inet addresses failed");
                    ec = nai_errno;
                    goto _fail;
                };
                q->in4.elts = (nai_addr_in4_t*)addrs;
            };
            q->in4.count = naddrs;
            q->query_in4 = 0;
            break;
#if (NAI_HAVE_SOCKADDR_IN6)
        case NAI_DNS_TYPE_AAAA:
            if (naddrs == 1) {
                addrs = (uint8_t*)&q->in6.addr;
                q->in6.elts = (nai_addr_in6_t*)addrs;
            } else {
                addrs = nai_dns_alloc(d, naddrs*sizeof(nai_addr_in6_t));
                if (addrs == 0) {
                    nai_log_alert(NAI_LOG_CORE, 
                        nai_errno, "alloc inet6 addresses failed");
                    ec = nai_errno;
                    goto _fail;
                };

                q->in6.elts = (nai_addr_in6_t*)addrs;
            };
            q->in6.count = naddrs;
            q->query_in6 = 0;
            break;
#endif
        default:
            assert(0);
            goto _end;
        };


        c = 0;
        n = 0;
        nan = hdr->nan;
        for ( ; n < nan; n ++) {
            r = nai_dns_str_next((uint8_t*)hdr, b + c, b + len);
            if (r < 0) {
                nai_log_info(NAI_LOG_CORE, nai_errno, 
                    "bad string in dns response about %.*s", 
                    nai_str_len(&name), nai_str(&name));
                ec = nai_errno;
                goto _fail;
            };

            c += r;
            pans = (nai_dns_ans_t*)(b + c);
            ans.type = nai_ntohs_ua(&pans->type);
            ans.len = nai_ntohs_ua(&pans->len);

            c += sizeof(ans);

            switch (ans.type) {
            case NAI_DNS_TYPE_A:
#if (NAI_HAVE_SOCKADDR_IN6)
            case NAI_DNS_TYPE_AAAA:
#endif
                nai_memcpy(addrs, b + c, ans.len);
                addrs += ans.len;
                break;
            default:
                break;
            };

            c += ans.len;
        };

        goto _check;
    };

    switch (qry->type) {
    case NAI_DNS_TYPE_A:
        q->query_in4 = 0;
        q->in4.count = 0;
        q->in4.elts = 0;
        break;

#if (NAI_HAVE_SOCKADDR_IN6)
    case NAI_DNS_TYPE_AAAA:
        q->query_in6 = 0;
        q->in6.count = 0;
        q->in6.elts = 0;
        break;
#endif
    default:
        break;
    };

    /* fill cname */
    if (cname) {
        if (q->rtype == NAI_DNS_RTYPE_ADDR || 
#if (NAI_HAVE_SOCKADDR_IN6)
            q->query_in6 || q->in6.count || 
#endif
            q->query_in4 || q->in4.count) {
            q->cname_skiped = 1;
            goto _check;
        };

        nai_dns_str_init(&copy, 0, 0);

        r = nai_dns_str_copy(d, &copy, start, cname, b + len);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, 
                "bad cname in dns response about %.*s", 
                nai_str_len(&name), nai_str(&name));
            ec = nai_errno;
            goto _fail;
        };

        q->rtype = NAI_DNS_RTYPE_CNAME;
        q->cname.mem = copy.mem;

        r = nai_dns_query_complete(d, q);
        goto _end;
    };

    nai_log_info(NAI_LOG_CORE, 0, "no address in dns response");


_check:
    if (q->query_in4 == 0 && q->query_in6 == 0) {
        if (nai_dns_count_addrs(q) <= 0) {
            ec = ENOENT;
            goto _fail;
        };

        q->rtype = NAI_DNS_RTYPE_ADDR;
        r = nai_dns_query_complete(d, q);
    };

_end:
    nai_dns_str_free(d, &str);
    return r;

_fail:
    r = nai_dns_query_failed(d, q, ec);
    goto _end;
};


static nai_int_t nai_dns_handle_serv(
    nai_dns_t* d, nai_dns_hdr_t* hdr, 
    nai_dns_qry_t* qry, uint8_t* start, uint8_t* b, size_t len)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t n, c;
    nai_int_t nan;
    nai_int_t nservs;
    nai_int_t datalen;
    nai_int_t datause;
    uint8_t* data;
    uint8_t* cname;
    nai_mem_t name;
    nai_dns_entry_t* q;
    nai_dns_ans_t* pans;
    nai_dns_ans_t ans;
    nai_dns_link_t* link;
    nai_dns_serv_t* serv;
    nai_dns_str_t str;
    nai_dns_str_t copy;
    char buf[256];


    nai_dns_str_init(&str, buf, sizeof(buf));


    r = nai_dns_str_copy(d, &str, start, start + sizeof(*hdr), b + len);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, nai_errno, 
            "copy compression string from  dns response failed");
        goto _end;
    };

    name = str.mem;
    q = nai_dns_lookup_name(&d->type[NAI_DNS_QTYPE_SERV], &name);
    if (q == 0) {
        nai_log_info(NAI_LOG_CORE, ENOENT, 
            "unexcept dns response about %.*s", 
            nai_str_len(&name), nai_str(&name));
        r = -1;
        goto _end;
    };

    if (q->ident != hdr->ident && 
        q->ident + 1 != hdr->ident) {
        nai_log_info(NAI_LOG_CORE, EINVAL, 
            "incorrect seqnum %d in dns response about %.*s", 
            q->ident, nai_str_len(&name), nai_str(&name));
        r = -1;
        goto _end;
    };

    if (hdr->tc) {
        nai_list_entry_remove(&q->qent);

        /* resend it */
        r = nai_dns_query_send(d, q, 1);
        goto _end;
    };


    /* check result code */
    if (hdr->rcode) {
        ec = nai_dns_rcode_to_errno(hdr->rcode);
        goto _fail;
    };


    nai_log_debug(NAI_LOG_CORE, 0, 
        "check answer, qtype %d", qry->type);


    /* check answers */
    nservs = 0;
    datalen = 0;
    datause = 0;
    cname = 0;
    c = 0;
    n = 0;
    nan = hdr->nan;
    for ( ; n < nan; n ++) {
        r = nai_dns_str_next((uint8_t*)hdr, b + c, b + len);
        if (r < 0) {
            nai_log_info(NAI_LOG_CORE, nai_errno, 
                "bad string in dns response about %.*s", 
                nai_str_len(&name), nai_str(&name));
            ec = nai_errno;
            goto _fail;
        };

        c += r;
        if (c + sizeof(nai_dns_ans_t) > len) {
            nai_log_info(NAI_LOG_CORE, EINVAL, 
                "incorrent length of SRV record "
                "in dns response about %.*s", 
                nai_str_len(&name), nai_str(&name));
            ec = EINVAL;
            goto _fail;
        };

        pans = (nai_dns_ans_t*)(b + c);
        ans.type = nai_ntohs_ua(&pans->type);
        ans.class = nai_ntohs_ua(&pans->class);
        ans.ttl = nai_ntohl_ua((uint32_t*)&pans->ttl);
        ans.len = nai_ntohs_ua(&pans->len);

        if (ans.class != NAI_DNS_CLASS_IN) {
            nai_log_info(NAI_LOG_CORE, EINVAL, 
                "unexpect class %d in dns response about %.*s", 
                ans.class, nai_str_len(&name), nai_str(&name));
            ec = EINVAL;
            goto _fail;
        };

        if (q->ttl > ans.ttl) {
            q->ttl = ans.ttl;
            if (q->ttl < 0) {
                q->ttl = 0;
            };
        };

        c += sizeof(ans);

        switch (ans.type) {
        case NAI_DNS_TYPE_SRV:
            if (ans.len <= 6 || 
                ans.len + c > (intptr_t)len) {
                nai_log_info(NAI_LOG_CORE, EINVAL, 
                    "incorrent length of SRV record "
                    "in dns response about %.*s", 
                    nai_str_len(&name), nai_str(&name));
                ec = EINVAL;
                goto _fail;
            };

            r = nai_dns_str_copy(d, 0, start, b + c + 6, b + len);
            if (r < 0) {
                nai_log_info(NAI_LOG_CORE, EINVAL, 
                    "bad servname in dns response about %.*s", 
                    nai_str_len(&name), nai_str(&name));
                ec = EINVAL;
                goto _fail;
            };
            if (r <= 1) {
                continue;
            };

            datalen += r;
            nservs ++;
            break;
        case NAI_DNS_TYPE_CNAME:
            cname = b + c;
            break;
        case NAI_DNS_TYPE_DNAME:
            break;
        default:
            nai_log_info(NAI_LOG_CORE, EINVAL, 
                "unexpect record type %d "
                "in dns response about %.*s", 
                ans.type, nai_str_len(&name), nai_str(&name));
            ec = EINVAL;
            goto _fail;
        };

        c += ans.len;
        if (c > (intptr_t)len) {
            nai_log_info(NAI_LOG_CORE, EINVAL, 
                "incorrent length of SRV record "
                "in dns response about %.*s", 
                nai_str_len(&name), nai_str(&name));
            ec = EINVAL;
            goto _fail;
        };
    };

    nai_log_debug(NAI_LOG_CORE, 0, 
        "read answer, qtype %d, service %d", qry->type, nservs);

    /* fill services */
    if (nservs) {

        serv = nai_dns_alloc(d, nservs * 
            (sizeof(*serv) + sizeof(*link)) + datalen);
        if (serv == 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "alloc dns services failed");
            ec = nai_errno;
            goto _fail;
        };

        link = (nai_dns_link_t*)(serv + nservs);
        data = (uint8_t*)(link + nservs);

        q->rtype = NAI_DNS_RTYPE_SERV;
        q->serv.count = nservs;
        q->serv.elts = serv;
        q->serv.link = link;

        n = 0;
        c = 0;
        for ( ; n < nan; n ++) {
            r = nai_dns_str_next((uint8_t*)hdr, b + c, b + len);
            if (r < 0) {
                nai_log_info(NAI_LOG_CORE, nai_errno, 
                    "bad string in dns response about %.*s", 
                    nai_str_len(&name), nai_str(&name));
                ec = nai_errno;
                goto _fail;
            };

            c += r;
            pans = (nai_dns_ans_t*)(b + c);
            ans.type = nai_ntohs_ua(&pans->type);
            ans.len = nai_ntohs_ua(&pans->len);

            c += sizeof(ans);
            if (ans.type != NAI_DNS_TYPE_SRV) {
                c += ans.len;
                continue;
            };

            nai_dns_str_init(&copy, data + datause, datalen - datause);

            r = nai_dns_str_copy(d, &copy, start, b + c + 6, b + len);
            if (r < 0) {
                nai_log_info(NAI_LOG_CORE, nai_errno, 
                    "bad servname in dns response about %.*s", 
                    nai_str_len(&name), nai_str(&name));
                ec = nai_errno;
                goto _fail;
            };
            if (r <= 1) {
                continue;
            };

            link->next = 0;
            link->serv = serv;
            link->src = q;
            link->dst = 0;
            link->recursion = 0;
            link ++;

            nai_str_setm(&serv->name, data + datause, r - 1);
            serv->priority = nai_ntohs_ua((uint16_t*)(b + c));
            serv->weight = nai_ntohs_ua((uint16_t*)(b + c + 2));
            serv->port = nai_ntohs_ua((uint16_t*)(b + c + 4));
            serv->ttl = q->ttl;
            serv->error = 0;
            serv ++;
            datause += r;

            c += ans.len;
        };

        r = nai_dns_query_complete(d, q);
        goto _end;
    };


    /* fill cname */
    if (cname) {
        nai_dns_str_init(&copy, 0, 0);

        r = nai_dns_str_copy(d, &copy, start, cname, b + len);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, 
                "bad cname in dns response about %.*s", 
                nai_str_len(&name), nai_str(&name));
            ec = nai_errno;
            goto _fail;
        };

        q->rtype = NAI_DNS_RTYPE_CNAME;
        q->cname.mem = copy.mem;

        r = nai_dns_query_complete(d, q);
        goto _end;
    };


    nai_log_info(NAI_LOG_CORE, 0, "no service in dns response");

    r = nai_dns_query_failed(d, q, ENOENT);

_end:
    nai_dns_str_free(d, &str);
    return r;

_fail:
    r = nai_dns_query_failed(d, q, ec);
    goto _end;
};


static nai_int_t nai_dns_handle_addr(
    nai_dns_t* d, nai_dns_hdr_t* hdr, 
    nai_dns_qry_t* qry, uint8_t* start, uint8_t* b, size_t len)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t n, c;
    nai_int_t nan;
    nai_int_t inlen;
    char* in;
    nai_mem_t name;
    nai_addr_in4_t addr4;
#if (NAI_HAVE_SOCKADDR_IN6)
    nai_addr_in6_t addr6;
#endif
    nai_dns_entry_t* q;
    nai_dns_ans_t* pans;
    nai_dns_ans_t ans;
    nai_dns_str_t str;
    nai_dns_str_t copy;
    char buf[256];


    nai_dns_str_init(&str, buf, sizeof(buf));


    r = nai_dns_str_copy(d, &str, start, start + sizeof(*hdr), b + len);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, nai_errno, 
            "copy compression string from dns response failed");
        goto _end;
    };

    name = str.mem;
    in = nai_str(&name);
    inlen = nai_str_len(&name);

    if (nai_strncmp(in + inlen - 13, ".in-addr.arpa", 13) == 0) {
        r = nai_inet_pton(AF_INET, in, inlen-13, &addr4);
        if (r >= 0) {
            nai_dns_ntoh((uint8_t*)&addr4, sizeof(addr4));
            q = nai_dns_lookup_addr(
                &d->type[NAI_DNS_QTYPE_ADDR], AF_INET, &addr4);
        };
#if (NAI_HAVE_SOCKADDR_IN6)
    } else if (nai_strncmp(in + inlen - 9, ".ip6.arpa", 9) == 0) {
        r = nai_inet_pton(AF_INET6, in, inlen-9, &addr6);
        if (r >= 0) {
            nai_dns_ntoh((uint8_t*)&addr6, sizeof(addr6));
            q = nai_dns_lookup_addr(
                &d->type[NAI_DNS_QTYPE_ADDR6], AF_INET6, &addr6);
        };
#endif
    } else {
        r = -1;
    };
    if (r < 0) {
        nai_log_info(NAI_LOG_CORE, EINVAL, 
            "invalid in-addr.arpa or ip6.arpa name in dns response");
        goto _end;
    };

    if (q == 0) {
        nai_log_info(NAI_LOG_CORE, ENOENT, 
            "unexcept dns response about %.*s", 
            nai_str_len(&name), nai_str(&name));
        r = -1;
        goto _end;
    };

    if (q->ident != hdr->ident && 
        q->ident + 1 != hdr->ident) {
        nai_log_info(NAI_LOG_CORE, EINVAL, 
            "incorrect seqnum %d in dns response about %.*s", 
            q->ident, nai_str_len(&name), nai_str(&name));
        r = -1;
        goto _end;
    };

    if (hdr->tc) {
        nai_list_entry_remove(&q->qent);

        /* resend it */
        r = nai_dns_query_send(d, q, 1);
        goto _end;
    };


    /* check result code */
    if (hdr->rcode) {
        ec = nai_dns_rcode_to_errno(hdr->rcode);
        goto _fail;
    } else if (hdr->nan == 0) {
        ec = nai_dns_rcode_to_errno(NAI_DNS_NXDOMAIN);
        goto _fail;
    };


    nai_log_debug(NAI_LOG_CORE, 0, 
        "read answer, qtype %d, answers %d", qry->type, hdr->nan);

    c = 0;
    n = 0;
    nan = hdr->nan;
    for ( ; n < nan; n ++) {
        r = nai_dns_str_next((uint8_t*)hdr, b + c, b + len);
        if (r < 0) {
            nai_log_info(NAI_LOG_CORE, nai_errno, 
                "bad string in dns response about %.*s", 
                nai_str_len(&name), nai_str(&name));
            ec = nai_errno;
            goto _fail;
        };

        c += r;
        if (c + sizeof(nai_dns_ans_t) > len) {
            nai_log_info(NAI_LOG_CORE, EINVAL, 
                "incorrent length of PTR record "
                "in dns response about %.*s", 
                nai_str_len(&name), nai_str(&name));
            ec = EINVAL;
            goto _fail;
        };

        pans = (nai_dns_ans_t*)(b + c);
        ans.type = nai_ntohs_ua(&pans->type);
        ans.class = nai_ntohs_ua(&pans->class);
        ans.ttl = nai_ntohl_ua((uint32_t*)&pans->ttl);
        ans.len = nai_ntohs_ua(&pans->len);

        if (ans.class != NAI_DNS_CLASS_IN) {
            nai_log_info(NAI_LOG_CORE, EINVAL, 
                "unexpect class %d in dns response about %.*s", 
                ans.class, nai_str_len(&name), nai_str(&name));
            ec = EINVAL;
            goto _fail;
        };

        if (q->ttl > ans.ttl) {
            q->ttl = ans.ttl;
            if (q->ttl < 0) {
                q->ttl = 0;
            };
        };

        c += sizeof(ans);

        switch (ans.type) {
        case NAI_DNS_TYPE_PTR:
            nai_dns_str_init(&copy, 0, 0);

            r = nai_dns_str_copy(d, &copy, b, b + c, b + len);
            if (r < 0) {
                nai_log_info(NAI_LOG_CORE, nai_errno, 
                    "bad hostname in dns response about %.*s", 
                    nai_str_len(&name), nai_str(&name));
                ec = nai_errno;
                goto _fail;
            };

            q->rtype = NAI_DNS_RTYPE_HNAME;
            q->hname.mem = copy.mem;
            r = nai_dns_query_complete(d, q);
            goto _end;

        case NAI_DNS_TYPE_CNAME:
            break;

        default:
            nai_log_info(NAI_LOG_CORE, EINVAL, 
                "unexpect record type %d "
                "in dns response about %.*s", 
                ans.type, nai_str_len(&name), nai_str(&name));
            ec = EINVAL;
            goto _fail;
        };

        c += ans.len;
        if (c > (intptr_t)len) {
            nai_log_info(NAI_LOG_CORE, EINVAL, 
                "incorrent length of SRV record "
                "in dns response about %.*s", 
                nai_str_len(&name), nai_str(&name));
            ec = EINVAL;
            goto _fail;
        };
    };

    nai_log_info(NAI_LOG_CORE, 0, "no ptr in dns response");
    r = -1;

_end:
    nai_dns_str_free(d, &str);
    return r;

_fail:
    r = nai_dns_query_failed(d, q, ec);
    goto _end;
};


static nai_int_t nai_dns_handle(nai_dns_t* d, 
    nai_dns_server_t* s, uint8_t* b, size_t len)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t ec;
    nai_dns_hdr_t* phdr;
    nai_dns_qry_t* pqry;
    nai_dns_hdr_t hdr;
    nai_dns_qry_t qry;


    if (len < sizeof(hdr) + sizeof(qry) + 1) {
        nai_log_info(NAI_LOG_CORE, 0, 
            "too short dns response, length %d", (nai_int_t)len);
        r = -1;
        goto _end;
    };


    phdr = (nai_dns_hdr_t*)b;
    hdr.ident = nai_ntohs_ua(&phdr->ident);
    hdr.nqs = nai_ntohs_ua(&phdr->nqs);
    hdr.nan = nai_ntohs_ua(&phdr->nan);
    hdr.nnr = nai_ntohs_ua(&phdr->nnr);
    hdr.nns = nai_ntohs_ua(&phdr->nns);
    hdr.flags = phdr->flags;

    if (hdr.z || hdr.opcode || !hdr.ra) {
        nai_log_info(NAI_LOG_CORE, 0, 
            "invalid dns response with flags %x, from %s", 
            nai_ntohs(hdr.flags), s->strname);
        r = -1;
        goto _end;
    };


    if (hdr.rcode) {
        ec = nai_dns_rcode_to_errno(hdr.rcode);
        nai_log_info(NAI_LOG_CORE, ec, 
            "dns response with an error %d, from %s", 
            hdr.rcode, s->strname);

        if (hdr.rcode == NAI_DNS_FORMERR) {
            r = -1;
            goto _end;
        };
        if (hdr.rcode > NAI_DNS_REFUSED) {
            r = -1;
            goto _end;
        };
    };


    n = sizeof(hdr);
    n = nai_dns_str_next(b, b + n, b + len);
    if (n < 0) {
        nai_log_info(NAI_LOG_CORE, nai_errno, 
            "bad compression string in dns response, from %s", 
            s->strname);
        r = -1;
        goto _end;
    };

    n += sizeof(hdr);
    if (n + sizeof(qry) > len) {
        nai_log_info(NAI_LOG_CORE, nai_errno, 
            "invalid length of dns response, too short, from %s", 
            s->strname);
        r = -1;
        goto _end;
    };


    pqry = (nai_dns_qry_t*)(b + n);
    qry.type = nai_ntohs_ua(&pqry->type);
    qry.class = nai_ntohs_ua(&pqry->class);
    if (qry.class != NAI_DNS_CLASS_IN) {
        nai_log_info(NAI_LOG_CORE, nai_errno, 
            "incorrent class of dns response, class %d, from %s", 
            qry.class, s->strname);
        r = -1;
        goto _end;
    };

    nai_log_debug(NAI_LOG_CORE, 0, 
        "get result, query type %d, rcode %d, nan %d", 
        qry.type, hdr.rcode, hdr.nan);

    n += sizeof(qry);
    switch (qry.type) {
    case NAI_DNS_TYPE_A:
#if (NAI_HAVE_SOCKADDR_IN6)
    case NAI_DNS_TYPE_AAAA:
#endif
        nai_dns_handle_name(d, &hdr, &qry, b, b + n, len - n);
        break;

    case NAI_DNS_TYPE_SRV:
        nai_dns_handle_serv(d, &hdr, &qry, b, b + n, len - n);
        break;

    case NAI_DNS_TYPE_PTR:
        nai_dns_handle_addr(d, &hdr, &qry, b, b + n, len - n);
        break;
    default:
        r = -1;
        goto _end;
    };


    r = 0;

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// dns entry manage


static nai_dns_entry_t* nai_dns_entry_from_name(
    nai_dns_t* d, const nai_mem_t* name)
{
    nai_int_t r;
    nai_int_t n = 1;
    nai_int_t len;
    nai_int_t count;
    uint8_t ch;
    uint8_t* p;
    uint8_t* s;
    nai_dns_entry_t* e;
    nai_dns_hdr_t* hdr;
    nai_dns_qry_t qry;


    len = nai_str_len(name);
    e = (nai_dns_entry_t*)nai_dns_alloc(d, sizeof(*e) + len);
    if (e == 0) {
        r = -1;
        goto _end;
    };

    d->nexts += 2;
    e->flags = 0;
    e->qtype = NAI_DNS_QTYPE_NAME;
    e->query_in4 = 1;
    e->ident = d->nexts;
    e->link = 0;
    e->wait = 0;
    e->refs = 1;
    e->send = 0;
    e->ttl = -1;
    nai_str_setm(&e->name, (char*)(e+1), len);
    nai_memcpy(nai_str(&e->name), nai_str(name), len);


#if (NAI_HAVE_SOCKADDR_IN6)
    n = d->no_ipv6 ? 1 : 2;
#endif
    e->len = (uint16_t)(sizeof(*hdr) + sizeof(qry) + len + 2);
    e->query = (uint8_t*)nai_dns_alloc(d, e->len * n);
    if (e->query == 0) {
        r = -1;
        goto _end;
    };


    hdr = (nai_dns_hdr_t*)e->query;
    hdr->ident = nai_htons(e->ident);
    hdr->flags = nai_htons(0x0100);
    hdr->nqs = nai_htons(1);
    hdr->nan = 0;
    hdr->nnr = 0;
    hdr->nns = 0;


    count = 0;
    p = (uint8_t*)(hdr + 1) + 1;
    s = (uint8_t*)nai_str(name);
    n = len - 1;
    for ( ; n >= 0; n --) {
        ch = s[n];
        if (ch != '.') {
            p[n] = ch;
            count ++;
            continue;
        };
        if (count > 63) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        p[n] = (uint8_t)count;
        count = 0;
    };

    p = (uint8_t*)(hdr + 1);
    p[0] = (uint8_t)count;
    p[len + 1] = 0;


    p += len + 2;
    qry.type = nai_htons(NAI_DNS_TYPE_A);
    qry.class = nai_htons(NAI_DNS_CLASS_IN);
    nai_memcpy(p, &qry, sizeof(qry));


#if (NAI_HAVE_SOCKADDR_IN6)

    if (d->no_ipv6 == 0) {
        e->query_in6 = 1;

        nai_memcpy(e->query + e->len, e->query, e->len);
        hdr = (nai_dns_hdr_t*)(e->query + e->len);
        hdr->ident_lo |= 1;

        p = (uint8_t*)(hdr + 1);
        p += len + 2;
        qry.type = nai_htons(NAI_DNS_TYPE_AAAA);
        nai_memcpy(p, &qry, sizeof(qry));
    };

#endif

    e->len_n = nai_htons(e->len);
    nai_list_init(&e->qent);
    nai_dns_insert_name(&d->type[NAI_DNS_QTYPE_NAME], e);
    r = 0;

_end:
    if (r < 0) {
        if (e) {
            if (e->query) {
                nai_dns_free(d, e->query);
            };
            nai_dns_free(d, e);
            e = 0;
        };
    };
    return e;
};


static nai_dns_entry_t* nai_dns_entry_from_serv(
    nai_dns_t* d, const nai_mem_t* name)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t len;
    nai_int_t count;
    uint8_t ch;
    uint8_t* p;
    uint8_t* s;
    nai_dns_entry_t* e;
    nai_dns_hdr_t* hdr;
    nai_dns_qry_t qry;


    len = nai_str_len(name);
    e = (nai_dns_entry_t*)nai_dns_alloc(d, sizeof(*e) + len);
    if (e == 0) {
        r = -1;
        goto _end;
    };

    d->nexts += 2;
    e->flags = 0;
    e->qtype = NAI_DNS_QTYPE_SERV;
    e->ident = d->nexts;
    e->link = 0;
    e->wait = 0;
    e->refs = 1;
    e->send = 0;
    e->ttl = -1;
    nai_str_setm(&e->name, (char*)(e+1), len);
    nai_memcpy(nai_str(&e->name), nai_str(name), len);


    e->len = (uint16_t)(sizeof(*hdr) + sizeof(qry) + len + 2);
    e->query = (uint8_t*)nai_dns_alloc(d, e->len);
    if (e->query == 0) {
        r = -1;
        goto _end;
    };


    hdr = (nai_dns_hdr_t*)e->query;
    hdr->ident = nai_htons(e->ident);
    hdr->flags = 0;
    hdr->rd = 1;
    hdr->nqs = nai_htons(1);
    hdr->nan = 0;
    hdr->nnr = 0;
    hdr->nns = 0;


    count = 0;
    p = (uint8_t*)(hdr + 1) + 1;
    s = (uint8_t*)nai_str(name);
    n = len - 1;
    for ( ; n >= 0; n --) {
        ch = s[n];
        if (ch != '.') {
            p[n] = ch;
            count ++;
            continue;
        };
        if (count > 63) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        p[n] = (uint8_t)count;
        count = 0;
    };

    p = (uint8_t*)(hdr + 1);
    p[0] = (uint8_t)count;
    p[len+1] = 0;


    p += len + 2;
    qry.type = nai_htons(NAI_DNS_TYPE_SRV);
    qry.class = nai_htons(NAI_DNS_CLASS_IN);
    nai_memcpy(p, &qry, sizeof(qry));


    e->len_n = nai_htons(e->len);
    nai_list_init(&e->qent);
    nai_dns_insert_name(&d->type[NAI_DNS_QTYPE_SERV], e);
    r = 0;

_end:
    if (r < 0) {
        if (e) {
            if (e->query) {
                nai_dns_free(d, e->query);
            };
            nai_dns_free(d, e);
            e = 0;
        };
    };
    return e;
};


static nai_dns_entry_t* nai_dns_entry_from_addr(
    nai_dns_t* d, const nai_sockaddr_t* name)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t len;
    uint8_t* p;
    uint8_t* s;
    nai_dns_entry_t* e;
    nai_dns_hdr_t* hdr;
    nai_dns_qry_t qry;
    nai_sockaddr_in4_t* in4;
#if (NAI_HAVE_SOCKADDR_IN6)
    nai_sockaddr_in6_t* in6;
#endif


    e = (nai_dns_entry_t*)nai_dns_alloc(d, sizeof(*e));
    if (e == 0) {
        r = -1;
        goto _end;
    };

    d->nexts += 2;
    e->flags = 0;
    e->qtype = NAI_DNS_QTYPE_ADDR;
    e->ident = d->nexts;
    e->link = 0;
    e->wait = 0;
    e->refs = 1;
    e->send = 0;
    e->ttl = -1;
    e->len = sizeof(*hdr) + sizeof(qry);
    e->query = 0;
    switch (name->sa_family) {
    case AF_INET:
        in4 = (nai_sockaddr_in4_t*)name;
        e->addr4 = in4->sin_addr;
        e->len += sizeof("255.255.255.255.in-addr.arpa.");
        break;
#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        in6 = (nai_sockaddr_in6_t*)name;
        e->addr6 = in6->sin6_addr;
        e->len += 64 + sizeof(".ip6.arpa.");
        e->qtype = NAI_DNS_QTYPE_ADDR6;
        break;
#endif
    default:
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    e->query = (uint8_t*)nai_dns_alloc(d, e->len);
    if (e->query == 0) {
        r = -1;
        goto _end;
    };


    hdr = (nai_dns_hdr_t*)e->query;
    hdr->ident = nai_htons(e->ident);
    hdr->flags = 0;
    hdr->rd = 1;
    hdr->nqs = nai_htons(1);
    hdr->nan = 0;
    hdr->nnr = 0;
    hdr->nns = 0;

    p = (uint8_t*)(hdr + 1);
    switch (name->sa_family) {
    case AF_INET:
        s = (uint8_t*)&e->addr4;
        for (n = 3; n >= 0; n --) {
            len = nai_sprintf((char*)p + 1, "%d", s[n]);
            p[0] = len;
            p += len + 1;
        };

        nai_memcpy(p, "\7in-addr\4arpa\0", 14);
        p += 14;
        n = 0;
        break;

#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        s = (uint8_t*)&e->addr6;
        for (n = 15; n >= 0; n --) {
            p += nai_sprintf((char*)p, 
                "\1%x\1%x", s[n] & 0xf, (s[n] >> 4) & 0xf);
        };

        nai_memcpy(p, "\3ip6\4arpa\0", 10);
        p += 10;
        n = 1;
        break;
#endif

    default:
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    qry.type = nai_htons(NAI_DNS_TYPE_PTR);
    qry.class = nai_htons(NAI_DNS_CLASS_IN);
    nai_memcpy(p, &qry, sizeof(qry));
    p += 4;

    e->len = (uint16_t)(p - e->query);
    e->len_n = nai_htons(e->len);
    nai_list_init(&e->qent);
    nai_dns_insert_addr(&d->type[NAI_DNS_QTYPE_ADDR + n], e);
    r = 0;

_end:
    if (r < 0) {
        if (e) {
            if (e->query) {
                nai_dns_free(d, e->query);
            };
            nai_dns_free(d, e);
            e = 0;
        };
    };
    return e;
};


static nai_dns_entry_t* nai_dns_entry(
    nai_dns_t* d, nai_int_t type, const nai_mem_t* name)
{
    nai_int_t r;
    nai_dns_entry_t* q;
    nai_sockaddr_in4_t* in4;
#if (NAI_HAVE_SOCKADDR_IN6)
    nai_sockaddr_in6_t* in6;
#endif
    nai_mem_t nmem;
    nai_socknbuf_in_t nbuf;


    switch (type) {
    case NAI_DNS_QTYPE_NAME:
        q = nai_dns_lookup_name(&d->type[type], name);
        break;
    case NAI_DNS_QTYPE_SERV:
        q = nai_dns_lookup_name(&d->type[type], name);
        break;
    case NAI_DNS_QTYPE_ADDR:
        nbuf.len = sizeof(nbuf.storage);
        r = nai_sockaddr_pton(
            nai_str(name), nai_str_len(name), &nbuf.addr, &nbuf.len);
        if (r < 0) {
            q = 0;
            goto _end;
        };

        nai_str_setm(&nmem, (char*)&nbuf.addr, nbuf.len);
        name = &nmem;
        type = NAI_DNS_QTYPE_SOCKADDR;
        /* fallthrough */

    case NAI_DNS_QTYPE_SOCKADDR:
        in4 = (nai_sockaddr_in4_t*)nai_str(name);
        switch (in4->sin_family) {
        case AF_INET:
            q = nai_dns_lookup_addr(
                &d->type[NAI_DNS_QTYPE_ADDR], AF_INET, &in4->sin_addr);
            break;
#if (NAI_HAVE_SOCKADDR_IN6)
        case AF_INET6:
            in6 = (nai_sockaddr_in6_t*)in4;
            q = nai_dns_lookup_addr(
                &d->type[NAI_DNS_QTYPE_ADDR6], AF_INET6, &in6->sin6_addr);
            break;
#endif

        default:
            nai_errno = ENOTSUP;
            q = 0;
            goto _end;
        };
        break;

    default:
        nai_errno = EINVAL;
        q = 0;
        goto _end;
    };

    if (q != 0) {
        goto _end;
    };

    switch (type) {
    case NAI_DNS_QTYPE_NAME:
        q = nai_dns_entry_from_name(d, name);
        break;
    case NAI_DNS_QTYPE_SERV:
        q = nai_dns_entry_from_serv(d, name);
        break;
    case NAI_DNS_QTYPE_SOCKADDR:
        q = nai_dns_entry_from_addr(d, (nai_sockaddr_t*)nai_str(name));
        break;
    default:
        break;
    };
    if (q == 0) {
        goto _end;
    };


    r = nai_dns_query_send(d, q, 0);
    if (r < 0) {
        q = 0;
    };

_end:
    return q;
};


static nai_dns_entry_t* nai_dns_entry_root(nai_dns_t* d)
{
    nai_int_t r;
    nai_int_t ec;
    nai_dns_entry_t* e;


    e = nai_dns_alloc(d, sizeof(*e));
    if (e == 0) {
        goto _end;
    };

    e->flags = 0;
    e->qtype = NAI_DNS_QTYPE_ROOT;
    e->rtype = NAI_DNS_RTYPE_LOCK;
    e->ident = 0;
    e->link = 0;
    e->wait = 0;
    e->refs = 1;
    e->send = 0;
    e->ttl = 0;
    e->len = 0;
    e->len_n =  0;
    e->query = 0;
    e->expire = 0;

    r = nai_spin_open(&e->lock, 0);
    if (r < 0) {
        ec = nai_errno;
        nai_dns_free(d, e);
        nai_errno = ec;
        e = 0;
    };

_end:
    return e;
};


static nai_int_t nai_dns_entry_free(
    nai_dns_t* d, nai_dns_entry_t* ent, nai_int_t quit)
{
    nai_int_t r;
    nai_int_t n;
    nai_dns_entry_t* ref;
    nai_dns_link_t* next;
    nai_dns_link_t** prev;

    /* unlink */
    next = ent->link;
    while (next) {
        ref = next->src;
        next->recursion = 0;
        next->dst = 0;
        next->serv->error = 0;
        nai_dns_clear_addrs(next->serv);
        next = next->next;

        if (!quit && ref->wait) {
            r = nai_dns_notify(d, ref, 0);
            if (r) {
                quit = 1;
            };
        };
    };
    ent->link = 0;


    switch (ent->rtype) {
    case NAI_DNS_RTYPE_CNAME:
        if (nai_str(&ent->cname.mem)) {
            nai_dns_free(d, nai_str(&ent->cname.mem));
        };
        break;
    case NAI_DNS_RTYPE_HNAME:
        if (nai_str(&ent->hname.mem)) {
            nai_dns_free(d, nai_str(&ent->hname.mem));
        };
        break;
    case NAI_DNS_RTYPE_ADDR:
        if (ent->in4.count > 1) {
            nai_dns_free(d, ent->in4.elts);
        };
#if (NAI_HAVE_SOCKADDR_IN6)
        if (ent->in6.count > 1) {
            nai_dns_free(d, ent->in6.elts);
        };
#endif
        break;
    case NAI_DNS_RTYPE_SERV:
        for (n = 0; n < ent->serv.count; n ++) {
            ref = ent->serv.link[n].dst;
            if (ref == 0) {
                continue;
            };

            prev = &ref->link;
            for (;;) {
                next = prev[0];
                if (next == &ent->serv.link[n]) {
                    prev[0] = next->next;
                    break;
                };
                if (next == 0) {
                    assert(0);
                    break;
                };

                prev = &next->next;
            };
        };

        if (ent->serv.elts) {
            nai_dns_free(d, ent->serv.elts);
        };
        break;

    case NAI_DNS_RTYPE_LOCK:
        nai_spin_close(&ent->lock);
        break;

    default:
        break;
    };

    if (ent->query) {
        nai_dns_free(d, ent->query);
    };

    nai_dns_free(d, ent);
    r = 0;

    return r;
};


static nai_int_t nai_dns_entry_list_free(
    nai_dns_t* d, nai_list_entry_t* list, nai_int_t reuse)
{
    nai_int_t r;
    nai_list_entry_t* e;
    nai_dns_entry_t* q;


    if (!reuse) {
        e = list->next;
        for ( ; e != list; ) {
            q = nai_containof(e, nai_dns_entry_t, qent);
            e = e->next;
            nai_dns_entry_free(d, q, 1);
        };

        nai_list_init(list);

    } else {

        e = list->next;
        for ( ; e != list; ) {
            q = nai_containof(e, nai_dns_entry_t, qent);
            e = e->next;

            if (q->permanent) {
                continue;
            };

            switch (q->qtype) {
            case NAI_DNS_QTYPE_NAME:
            case NAI_DNS_QTYPE_SERV:
            case NAI_DNS_QTYPE_ADDR:
            case NAI_DNS_QTYPE_ADDR6:
                nai_rbtree_erase(&d->type[q->qtype], &q->ent);
                break;
            default:
                break;
            };

            nai_list_entry_remove(&q->qent);
            nai_dns_entry_free(d, q, 1);
        };
    };

    r = 0;

    return r;
};


static nai_int_t nai_dns_close_impl(nai_dns_t* d, nai_int_t reuse)
{
    nai_int_t r;
    nai_int_t n;
    nai_dns_server_t* s;


    r = nai_evnode_close(&d->ev);
    if (r < 0) {
        goto _end;
    };

    if (d->pexit) {
        d->pexit[0] = 1;
        d->pexit = 0;
    };

    if (d->root) {
        nai_dns_entry_free(d, d->root, 1);
        d->root = 0;
    };

    nai_dns_entry_list_free(d, &d->qwait, reuse);
    nai_dns_entry_list_free(d, &d->qexpire, reuse);

    s = (nai_dns_server_t*)d->servers.elts;
    for (n = 0; n < (intptr_t)d->servers.count; n ++) {
        r = nai_iobase_close(&s[n].tcp.str);
        if (r < 0) {
            goto _end;
        };

        r = nai_iobase_close(&s[n].udp.str);
        if (r < 0) {
            goto _end;
        };

        if (s[n].tcp.buf) {
            nai_dns_free(d, s[n].tcp.buf);
            s[n].tcp.buf = 0;
        };
        if (s[n].udp.buf) {
            nai_dns_free(d, s[n].udp.buf);
            s[n].udp.buf = 0;
        };

        nai_dns_entry_list_free(d, &s[n].tcp.send, reuse);
        nai_dns_entry_list_free(d, &s[n].tcp.queue, reuse);
        nai_dns_entry_list_free(d, &s[n].udp.send, reuse);
        nai_dns_entry_list_free(d, &s[n].udp.queue, reuse);
    };

    if (!reuse) {
        nai_array_close(&d->servers);

        for (n = 0; n < (nai_int_t)nai_countof(d->type); n ++) {
            nai_rbtree_init(&d->type[n]);
        };

        d->nexts = 0;
        d->nextc = 0;
    };

    r = 0;

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// dns connection


static nai_int_t nai_dns_set_timeout(nai_dns_t* d, uint32_t timeo)
{
    nai_int_t r;

    r = nai_evnode_set_timeout(&d->ev, NAI_TIMEOP_MIN, timeo);
    nai_log_debug(NAI_LOG_CORE, 0, 
        "dns 0x%p set timeout %d %s", d, timeo, r == 1 ? "skiped" : "");

    return r;
};


static nai_int_t nai_dns_disconnected(
    nai_dns_t* d, nai_dns_connection_t* c, nai_int_t errcode)
{
    nai_int_t r;
    nai_int_t sig;
    uint32_t now;
    nai_list_entry_t* e;
    nai_list_entry_t* b;
    nai_dns_entry_t* q;


    nai_log_debug(NAI_LOG_CORE, 0, 
        "dns  disconnected, errcode %d", errcode);

    nai_iobase_close(&c->str);

    c->vnext = 0;
    c->vcount = 0;
    c->buf_off = 0;
    c->buf_last = 0;


    sig = 0;
    now = nai_tickcache_to_msec32();


    b = d->qwait.next;
    e = c->send.next;
    for ( ; e != &c->send; ) {
        q = nai_containof(e, nai_dns_entry_t, qent);
        e = e->next;

        nai_list_entry_remove(&q->qent);
        q->locked = 0;
        q->refs --;
        if (q->refs <= 0) {
            nai_dns_entry_free(d, q, 0);
            continue;
        } else if (!q->complete) {
            sig |= 1;
            q->expire = now;
            nai_list_insert_before(b, &q->qent);
        } else {;
            if (q->query) {
                nai_dns_free(d, q->query);
                q->query = 0;
                q->len = q->len_n = 0;
            };
            sig |= nai_list_is_empty(&d->qexpire) ? 2 : 0;
            q->expire = now + d->expire;
            nai_list_insert_tail(&d->qexpire, &q->qent);
        };
    };


    e = c->queue.next;
    for ( ; e != &c->queue; ) {
        q = nai_containof(e, nai_dns_entry_t, qent);
        e = e->next;

        sig |= 1;
        q->expire = now;
        nai_list_entry_remove(&q->qent);
        nai_list_insert_before(b, &q->qent);
    };

    if (sig & 1) {
        nai_evnode_post(&d->ev, 0);
    } else if (sig & 2) {
        nai_dns_set_timeout(d, d->expire);
    };

    r = 0;

    return r;
};


static nai_int_t nai_dns_send_tcp(nai_dns_t* d, nai_dns_server_t* s)
{
    intptr_t r;
    nai_int_t n;
    nai_int_t ec;
    nai_int_t sig = 0;
    uint32_t now;
    uint32_t timeo;
    nai_list_entry_t* e;
    nai_dns_entry_t* q;
    nai_dns_connection_t* c;


    c = &s->tcp;
    if (nai_stream_is_opened(&c->str) == 0) {
        r = nai_stream_connect(&c->str, 
            nai_evnode_get_loop(&d->ev), &s->name.addr, s->name.len);
        if (r < 0) {
            ec = nai_errno;
            goto _disconn;
        };

    };

    if (c->buf == 0) {
        c->buf_total = NAI_DNS_TCP_BUFSIZE;
        c->buf = nai_dns_alloc(d, c->buf_total);
        if (c->buf == 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, 
                "alloc the read buffer of dns tcp connection failed");
            /* */
            ec = nai_errno;
            goto _disconn;
        };
    };

    if (!(nai_stream_get_event(&c->str) & NAI_EV_WRITE)) {
        r = 0;
        goto _end;
    };

    now = nai_tickcache_to_msec32();

    for (;;) {
        while (c->vcount) {

            r = nai_stream_writev(&c->str, c->v + c->vnext, c->vcount);
            if (r < 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || 
                    ec == NAI_EINPROGRESS) {
                    r = 0;
                    goto _end;
                };

                ec = 0;
                goto _disconn;
            };

            for ( ; c->vcount; ) {
                n = c->vnext;
                if (c->v[n].len > (size_t)r) {
                    c->v[n].len -= r;
                    break;
                } else {
                    r -= c->v[n].len;
                    c->vnext ++;
                    c->vcount --;
                };
            };
        };

        e = c->send.next;
        for ( ; e != &c->send; ) {
            q = nai_containof(e, nai_dns_entry_t, qent);
            e = e->next;

            nai_list_entry_remove(&q->qent);
            q->locked = 0;
            q->refs --;
            if (q->refs <= 0) {
                nai_dns_entry_free(d, q, 0);
                continue;
            } else if (!q->complete) {
                sig |= nai_list_is_empty(&d->qwait) ? 1 : 0;
                q->expire = now + d->resend;
                nai_list_insert_tail(&d->qwait, &q->qent);
            } else {
                if (q->query) {
                    nai_dns_free(d, q->query);
                    q->query = 0;
                    q->len = q->len_n = 0;
                };
                sig |= nai_list_is_empty(&d->qexpire) ? 2 : 0;
                q->expire = now + d->expire;
                nai_list_insert_tail(&d->qexpire, &q->qent);
            };
        };


        c->vnext = 0;
        e = c->queue.next;
        for ( ; e != &c->queue; ) {
            q = nai_containof(e, nai_dns_entry_t, qent);
            e = e->next;

            if (q->qtype == NAI_DNS_QTYPE_NAME) {
                if (q->query_in4 && q->query_in6 && 
                    c->vcount > (nai_int_t)nai_countof(c->v) - 4) {
                    break;
                };
            };
            if (q->qtype != NAI_DNS_QTYPE_NAME || q->query_in4) {
                c->v[c->vcount].buf = (uint8_t*)&q->len_n;
                c->v[c->vcount].len = sizeof(q->len_n);
                c->v[c->vcount+1].buf = q->query;
                c->v[c->vcount+1].len = q->len;
                c->vcount += 2;
            };
            if (q->qtype == NAI_DNS_QTYPE_NAME && q->query_in6) {
                c->v[c->vcount].buf = (uint8_t*)&q->len_n;
                c->v[c->vcount].len = sizeof(q->len_n);
                c->v[c->vcount+1].buf = q->query + q->len;
                c->v[c->vcount+1].len = q->len;
                c->vcount += 2;
            };

            q->locked = 1;
            q->refs ++;
            nai_list_entry_remove(&q->qent);
            nai_list_insert_tail(&c->send, &q->qent);

            if (c->vcount >= (nai_int_t)nai_countof(c->v) - 1) {
                break;
            };
        };

        if (c->vcount <= 0) {
            break;
        };
    };

    r = 0;

_end:
    if (sig) {
        switch (sig) {
        case 1:
            timeo = d->resend;
            break;
        case 2:
            timeo = d->expire;
            break;
        default:
            timeo = d->resend;
            if (timeo > d->expire) {
                timeo = d->expire;
            };
            break;
        };
        nai_dns_set_timeout(d, timeo);
    };
    return (nai_int_t)r;

_disconn:
    r = nai_dns_disconnected(d, c, ec);
    goto _end;
};


static nai_int_t nai_dns_send_udp(nai_dns_t* d, nai_dns_server_t* s)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t sig = 0;
    uint32_t now;
    uint32_t timeo;
    nai_list_entry_t* e;
    nai_dns_entry_t* q;
    nai_dns_connection_t* c;


    c = &s->udp;
    if (nai_dgram_is_opened(&c->str) == 0) {
        r = nai_dgram_connect(&c->str, 
            nai_evnode_get_loop(&d->ev), &s->name.addr, s->name.len);
        if (r < 0) {
            ec = nai_errno;
            if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                goto _end;
            };
            goto _disconn;
        };

    };

    if (c->buf == 0) {
        c->buf_total = NAI_DNS_UDP_BUFSIZE;
        c->buf = nai_dns_alloc(d, c->buf_total);
        if (c->buf == 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, 
                "alloc the read buffer of dns udp connection failed");
            /* */
            ec = nai_errno;
            goto _disconn;
        };
    };

    if (!(nai_dgram_get_event(&c->str) & NAI_EV_WRITE)) {
        r = 0;
        goto _end;
    };

    now = nai_tickcache_to_msec32();

    for (;;) {
        while (c->vcount) {

            r = nai_dgram_send(&c->str, 
                c->v[c->vnext].buf, c->v[c->vnext].len, 0);
            if (r < 0) {
                if (nai_errno == NAI_EAGAIN || 
                    nai_errno == NAI_EINPROGRESS) {
                    r = 0;
                    goto _end;
                };

                ec = 0;
                goto _disconn;
            };

            c->vnext ++;
            c->vcount --;
        };

        e = c->send.next;
        for ( ; e != &c->send; ) {
            q = nai_containof(e, nai_dns_entry_t, qent);
            e = e->next;

            nai_list_entry_remove(&q->qent);
            q->locked = 0;
            q->refs --;
            if (q->refs <= 0) {
                nai_dns_entry_free(d, q, 0);
                continue;
            } else if (!q->complete) {
                sig |= nai_list_is_empty(&d->qwait) ? 1 : 0;
                q->expire = now + d->resend;
                nai_list_insert_tail(&d->qwait, &q->qent);
            } else {
                if (q->query) {
                    nai_dns_free(d, q->query);
                    q->query = 0;
                    q->len = q->len_n = 0;
                };
                sig |= nai_list_is_empty(&d->qexpire) ? 2 : 0;
                q->expire = now + d->expire;
                nai_list_insert_tail(&d->qexpire, &q->qent);
            };
        };


        c->vnext = 0;
        e = c->queue.next;
        for ( ; e != &c->queue; ) {
            q = nai_containof(e, nai_dns_entry_t, qent);
            e = e->next;

            if (q->qtype == NAI_DNS_QTYPE_NAME) {
                if (q->query_in4 && q->query_in6 && 
                    c->vcount > (nai_int_t)nai_countof(c->v) - 2) {
                    break;
                };
            };
            if (q->qtype != NAI_DNS_QTYPE_NAME || q->query_in4) {
                c->v[c->vcount].buf = q->query;
                c->v[c->vcount].len = q->len;
                c->vcount ++;
            };
            if (q->qtype == NAI_DNS_QTYPE_NAME && q->query_in6) {
                c->v[c->vcount].buf = q->query + q->len;
                c->v[c->vcount].len = q->len;
                c->vcount ++;
            };

            q->locked = 1;
            q->refs ++;
            nai_list_entry_remove(&q->qent);
            nai_list_insert_tail(&c->send, &q->qent);

            if (c->vcount >= (nai_int_t)nai_countof(c->v) - 1) {
                break;
            };
        };

        if (c->vcount <= 0) {
            break;
        };
    };

    r = 0;

_end:
    if (sig) {
        switch (sig) {
        case 1:
            timeo = d->resend;
            break;
        case 2:
            timeo = d->expire;
            break;
        default:
            timeo = d->resend;
            if (timeo > d->expire) {
                timeo = d->expire;
            };
            break;
        };
        nai_dns_set_timeout(d, timeo);
    };
    return (nai_int_t)r;

_disconn:
    r = nai_dns_disconnected(d, c, ec);
    goto _end;
};


static nai_int_t nai_dns_query_send(
    nai_dns_t* d, nai_dns_entry_t* e, nai_int_t tcp)
{
    nai_int_t r;
    nai_int_t next;
    nai_dns_server_t* s;


    if (e->send >= (intptr_t)d->resends) {
        nai_list_init(&e->qent);
        r = nai_dns_query_timedout(d, e);
        goto _end;
    };

    next = d->nextc ++;
    if (d->nextc >= d->servers.count) {
        d->nextc = 0;
    };
    if (next >= (intptr_t)d->servers.count) {
        nai_list_init(&e->qent);
        nai_dns_query_failed(d, e, ENOTCONN);
        nai_errno = ENOTCONN;
        r = -1;
        goto _end;
    };

    s = (nai_dns_server_t*)d->servers.elts + next;
    e->send ++;
    if (tcp || d->tcp) {
        e->tcp = 1;
        nai_list_insert_tail(&s->tcp.queue, &e->qent);
        r = nai_dns_send_tcp(d, s);
    } else {
        e->tcp = 0;
        nai_list_insert_tail(&s->udp.queue, &e->qent);
        r = nai_dns_send_udp(d, s);
    };

_end:
    return r;
};


static nai_int_t nai_dns_handle_udp(nai_iobase_t* u, nai_int_t events)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t exit;
    nai_dns_t* d;
    nai_dns_server_t* s;
    nai_dns_connection_t* c;


    s = nai_containof(u, nai_dns_server_t, udp);
    if (events & NAI_EV_WRITE) {
        r = nai_dns_send_udp(s->dns, s);
        if (r < 0) {
            goto _end;
        };
    };
    if (events & NAI_EV_READ) {
        c = &s->udp;

        exit = 0;
        d = s->dns;
        d->pexit = &exit;
        for (;;) {
            r = nai_dgram_recv(&c->str, c->buf, c->buf_total, 0);
            if (r < 0) {
                d->pexit = 0;

                ec = nai_errno;
                if (ec != NAI_EAGAIN && ec != NAI_EINPROGRESS) {
                    goto _disconn;
                };
                break;
            };

            r = nai_dns_handle(s->dns, s, c->buf, r);
            if (exit) {
                goto _end;
            };

        };

        d->pexit = 0;
    };

    r = 0;

_end:
    return (nai_int_t)r;

_disconn:
    nai_dns_disconnected(s->dns, c, 0);
    r = -1;
    goto _end;
};


static nai_int_t nai_dns_handle_tcp(nai_iobase_t* u, nai_int_t events)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t len;
    nai_int_t exit;
    uint8_t* buf;
    uint8_t* bend;
    nai_dns_t* d;
    nai_dns_server_t* s;
    nai_dns_connection_t* c;


    s = nai_containof(u, nai_dns_server_t, tcp);
    c = &s->tcp;

    if (events & NAI_EV_TIMEOUT) {
        goto _disconn;
    };
    if (events & NAI_EV_WRITE) {
        r = nai_dns_send_tcp(s->dns, s);
        if (r < 0) {
            goto _end;
        };
    };
    if (events & NAI_EV_READ) {
        exit = 0;
        d = s->dns;
        d->pexit = &exit;

        for (;;) {

            r = nai_stream_read(&c->str, 
                c->buf + c->buf_last, c->buf_total - c->buf_last);
            if (r < 0) {
                d->pexit = 0;
                ec = nai_errno;
                if (ec != NAI_EAGAIN && ec != NAI_EINPROGRESS) {
                    goto _disconn;
                };
                goto _end;
            };
            if (r == 0) {
                d->pexit = 0;
                goto _disconn;
            };

            c->buf_last += r;
            buf = c->buf + c->buf_off;
            bend = c->buf + c->buf_last;

            for ( ; buf + 2 < bend; ) {
                len = nai_ntohs_ua((uint16_t*)buf);
                if (buf + len + 2 > bend) {
                    break;
                };

                r = nai_dns_handle(s->dns, s, buf + 2, len);
                if (exit) {
                    goto _end;
                };

                buf += len + 2;
            };

            c->buf_off = buf - c->buf;
            if (c->buf_last <= c->buf_off) {
                c->buf_last = 0;
                c->buf_off = 0;
            } else if (c->buf_last >= c->buf_total) {
                nai_memcpy(c->buf, 
                    c->buf + c->buf_off, c->buf_last - c->buf_off);
                c->buf_last -= c->buf_off;
                c->buf_off = 0;
            };
        };
    };

    r = 0;

_end:
    return (nai_int_t)r;

_disconn:
    nai_dns_disconnected(s->dns, c, 0);
    r = -1;
    goto _end;
};


static nai_int_t nai_dns_event(nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_int_t closed;
    nai_int_t ttl;
    uint32_t now;
    uint32_t next;
    nai_list_entry_t* e;
    nai_dns_t* d = (nai_dns_t*)h;
    nai_dns_entry_t* q;
    nai_dns_query_t* n;
    nai_dns_query_t* c;
    nai_dns_result_t rs;


    closed = 0;
    d->pexit = &closed;

    if (events & NAI_EV_NOTIFY || 
        events & NAI_EV_TIMEOUT) {

        now = nai_tickcache_to_msec32();
        next = now + 60 * 1000;

        e = d->qwait.next;
        for ( ; e != &d->qwait; ) {
            q = nai_containof(e, nai_dns_entry_t, qent);
            e = e->next;
            if ((int32_t)(q->expire - now) <= 0) {
                nai_list_entry_remove(&q->qent);
                nai_dns_query_send(d, q, q->tcp);
                if (closed) {
                    break;
                };
                e = d->qwait.next;
                continue;
            };

            if ((int32_t)(q->expire - next) <= 0) {
                next = q->expire;
                break;
            };
        };

        ttl = (d->expire + 999) / 1000;
        e = d->qexpire.next;
        for ( ; e != &d->qexpire; ) {
            q = nai_containof(e, nai_dns_entry_t, qent);
            e = e->next;
            if ((int32_t)(q->expire - now) <= 0) {
                if (q->ttl > ttl) {
                    if (q->permanent == 0) {
                        q->ttl -= ttl;
                    };
                    q->expire = now + d->expire;
                    nai_list_entry_remove(&q->qent);
                    nai_list_insert_tail(&d->qexpire, &q->qent);
                } else {
                    switch (q->qtype) {
                    case NAI_DNS_QTYPE_NAME:
                    case NAI_DNS_QTYPE_SERV:
                    case NAI_DNS_QTYPE_ADDR:
                    case NAI_DNS_QTYPE_ADDR6:
                        nai_rbtree_erase(&d->type[q->qtype], &q->ent);
                        break;

                    default:
                        break;
                    };
                    nai_list_entry_remove(&q->qent);
                    nai_dns_entry_free(d, q, 0);
                };
                continue;
            };

            if ((int32_t)(q->expire - next) <= 0) {
                next = q->expire;
                break;
            };
        };

        nai_log_debug(NAI_LOG_CORE, 0, 
            "dns %p next timeout %d", d, next - now);
        nai_evnode_set_timeout(&d->ev, 0, next - now);


        if (d->root->wait) {

            nai_spin_lock(&d->root->lock);

            n = d->root->wait;
            while (n) {
                n->ent = 0;
                n = n->next;
            };
            n = d->root->wait;
            d->root->link = 0;

            nai_spin_unlock(&d->root->lock);


            rs.type = 0;
            while (n) {
                c = n;
                n = n->next;
                if (closed) {
                    q = 0;
                    rs.error = ECANCELED;
                } else {
                    q = nai_dns_entry(d, c->type, &c->name);
                    rs.error = nai_errno;
                };
                if (q == 0) {
                    /* closed or failed */
                    if (c->cb) {
                        c->cb(c, &rs);
                    };
                    continue;
                };

                c->ent = q;
                c->next = q->wait;
                q->wait = c;
                if (q->complete) {
                    r = nai_dns_notify(d, q, NAI_DNS_NOTIFY_RESET);
                    if (r) {
                        /* closed */
                        closed = 1;
                    };
                };
            };
        };
    };

    d->pexit = 0;

    r = 0;
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// dns apis


nai_int_t nai_dns_init(nai_dns_t* d)
{
    nai_int_t r;
    nai_int_t n;


    nai_evnode_init(&d->ev);
    nai_array_init(&d->servers, sizeof(nai_dns_server_t), 0);
    nai_list_init(&d->qwait);
    nai_list_init(&d->qexpire);
    for (n = 0; n < (nai_int_t)nai_countof(d->type); n ++) {
        nai_rbtree_init(&d->type[n]);
    };

    d->flags = 0;
    d->root = 0;
    d->pexit = 0;
    d->nexts = 0;
    d->nextc = 0;
    d->expire = 600*1000;
    d->resend = 3*1000;
    d->resends = 4;
    r = 0;

    return r;
};


nai_int_t nai_dns_add_server(
    nai_dns_t* d, const nai_mem_t* name, nai_int_t count)
{
    nai_int_t r;
    nai_int_t n, m, l;
    nai_int_t servers;
    const nai_sockaddr_info_t* si;
    nai_dns_server_t* s;
    nai_dns_server_t* a;


    if (nai_evnode_is_opened(&d->ev)) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    servers = (nai_int_t)d->servers.count;
    s = (nai_dns_server_t*)nai_array_push_n(&d->servers, count);
    if (s == 0) {
        r = -1;
        goto _end;
    };

    n = 0;
    l = 0;
    a = (nai_dns_server_t*)d->servers.elts;
    for ( ; l < count; l ++) {
        s[n].name.len = sizeof(s[n].name.storage);
        r = nai_sockaddr_pton(nai_str(&name[l]), 
            nai_str_len(&name[l]), &s[n].name.addr, &s[n].name.len);
        if (r < 0) {
            d->servers.count -= count;
            goto _end;
        };

        si = nai_sockaddr_info(s[n].name.addr.sa_family);
        if (si && nai_sockaddr_get_port(si, &s[n].name.addr) == 0) {
            nai_sockaddr_set_port(si, &s[n].name.addr, 53);
        };

        /* compare exist */
        for (m = 0; m < servers; m ++) {
            r = nai_sockaddr_compare(
                &s[n].name.addr, s[n].name.len, 
                &a[m].name.addr, a[m].name.len);
            if (r == 0) {
                break;
            };
        };
        if (m < servers) {
            continue;
        };

        s[n].dns = d;
        s[n].tcp.buf = 0;
        s[n].tcp.buf_off = 0;
        s[n].tcp.buf_total = 0;
        s[n].tcp.vnext = 0;
        s[n].tcp.vcount = 0;
        s[n].udp.buf = 0;
        s[n].udp.buf_off = 0;
        s[n].udp.buf_total = 0;
        s[n].udp.vnext = 0;
        s[n].udp.vcount = 0;
        nai_list_init(&s[n].tcp.send);
        nai_list_init(&s[n].tcp.queue);
        nai_list_init(&s[n].udp.send);
        nai_list_init(&s[n].udp.queue);
        nai_iobase_init(&s[n].tcp.str);
        nai_iobase_init(&s[n].udp.str);
        nai_iobase_set_cb(&s[n].tcp.str, nai_dns_handle_tcp);
        nai_iobase_set_cb(&s[n].udp.str, nai_dns_handle_udp);

        nai_sockaddr_ntop(&s[n].name.addr, 
            s[n].name.len, s[n].strname, sizeof(s[n].strname), 0);

        n ++;
    };

    d->servers.count -= count - n;
    r = n;

_end:
    return r;
};


nai_int_t nai_dns_add_static_addr(
    nai_dns_t* d, const nai_mem_t* name, const nai_mem_t* addr)
{
    nai_int_t r;
    nai_socknbuf_in_t nbuf;


    nbuf.len = sizeof(nbuf.storage);
    r = nai_sockaddr_pton(
        nai_str(addr), nai_str_len(addr), &nbuf.addr, &nbuf.len);
    if (r < 0) {
        goto _end;
    };

    r = nai_dns_add_static_sockaddr(d, name, &nbuf.addr, nbuf.len);

_end:
    return r;
};


nai_int_t nai_dns_add_static_sockaddr(
    nai_dns_t* d, const nai_mem_t* name, const nai_sockaddr_t* addr, 
    nai_int_t addrlen)
{
    nai_int_t r;
    nai_int_t qtype;
    nai_int_t count;
    nai_mem_t mem;
    nai_evloop_t* loop;
    nai_dns_entry_t* e;
    nai_dns_entry_t* h;
    nai_addr_in4_t* in4;
    nai_sockaddr_in4_t* sa4;
#if (NAI_HAVE_SOCKADDR_IN6)
    nai_addr_in6_t* in6;
    nai_sockaddr_in6_t* sa6;
#endif


    switch (addr->sa_family) {
    case AF_INET:
        if (addrlen < (nai_int_t)sizeof(nai_sockaddr_in4_t)) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
        break;
#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        if (addrlen < (nai_int_t)sizeof(nai_sockaddr_in6_t)) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
        break;
#endif
    default:
        nai_errno = EAFNOSUPPORT;
        r = -1;
        goto _end;
    };

    /* if already opened, we can't add record in other thread */
    if (nai_evnode_is_opened(&d->ev)) {
        loop = nai_evnode_get_loop(&d->ev);
        if (nai_evloop_in_dispatch(loop)) {
            nai_errno = EPERM;
            r = -1;
            goto _end;
        };
    };

    /* add record for host to address */
    e = nai_dns_lookup_name(&d->type[NAI_DNS_QTYPE_NAME], (nai_mem_t*)name);
    if (e == 0) {
        e = nai_dns_entry_from_name(d, (nai_mem_t*)name);
        if (e == 0) {
            r = -1;
            goto _end;
        };
    };

    if (e->permanent == 0) {
        switch (e->rtype) {
        case NAI_DNS_RTYPE_ADDR:
            if (e->in4.count > 1) {
                nai_dns_free(d, e->in4.elts);
            };
#if (NAI_HAVE_SOCKADDR_IN6)
            if (e->in6.count > 1) {
                nai_dns_free(d, e->in6.elts);
            };
#endif
            break;
        case NAI_DNS_RTYPE_CNAME:
            if (nai_str_len(&e->cname.mem)) {
                nai_dns_free(d, nai_str(&e->cname.mem));
            };
            break;
        default:
            break;
        };

        if (e->query) {
            nai_dns_free(d, e->query);
            e->query = 0;
        };

        e->rtype = NAI_DNS_RTYPE_ADDR;
        e->ttl = INT_MAX;
        e->permanent = 1;
        e->query_in4 = 0;
        e->query_in6 = 0;
        switch (addr->sa_family) {
        case AF_INET:
            sa4 = (nai_sockaddr_in4_t*)addr;
            e->in4.addr = sa4->sin_addr;
            e->in4.elts = &e->in4.addr;
            e->in4.count = 1;
#if (NAI_HAVE_SOCKADDR_IN6)
            e->in6.elts = 0;
            e->in6.count = 0;
#endif
            break;
#if (NAI_HAVE_SOCKADDR_IN6)
        case AF_INET6:
            sa6 = (nai_sockaddr_in6_t*)addr;
            e->in6.addr = sa6->sin6_addr;
            e->in6.elts = &e->in6.addr;
            e->in6.count = 1;
            e->in4.elts = 0;
            e->in4.count = 0;
            break;
#endif
        default:
            break;
        };

        e->expire = nai_tickcache_to_msec32() + d->expire;
        nai_list_entry_remove(&e->qent);
        nai_list_insert_tail(&d->qexpire, &e->qent);

    } else {
        switch (e->rtype) {
        case NAI_DNS_RTYPE_CNAME:
            if (nai_str_len(&e->cname.mem)) {
                nai_dns_free(d, nai_str(&e->cname.mem));
            };

            e->rtype = NAI_DNS_RTYPE_ADDR;
            nai_dns_clear_addrs(e);
            break;
        default:
            break;
        };

        switch (addr->sa_family) {
        case AF_INET:
            sa4 = (nai_sockaddr_in4_t*)addr;
            count = e->in4.count;
            if (count == 0) {
                e->in4.addr = sa4->sin_addr;
                e->in4.elts = &e->in4.addr;
            } else {
                in4 = (nai_addr_in4_t*)
                    nai_dns_alloc(d, sizeof(*in4)*(count+1));
                if (in4 == 0) {
                    r = -1;
                    goto _end;
                };

                nai_memcpy(in4, e->in4.elts, sizeof(*in4)*count);
                in4[count] = sa4->sin_addr;

                if (count > 1) {
                    nai_dns_free(d, e->in4.elts);
                };
                e->in4.elts = in4;
            };

            e->in4.count ++;
            break;
#if (NAI_HAVE_SOCKADDR_IN6)
        case AF_INET6:
            sa6 = (nai_sockaddr_in6_t*)addr;
            count = e->in6.count;
            if (count == 0) {
                e->in6.addr = sa6->sin6_addr;
                e->in6.elts = &e->in6.addr;
            } else {
                in6 = (nai_addr_in6_t*)
                    nai_dns_alloc(d, sizeof(*in6)*(count+1));
                if (in6 == 0) {
                    r = -1;
                    goto _end;
                };

                nai_memcpy(in6, e->in6.elts, sizeof(*in6)*count);
                in6[count] = sa6->sin6_addr;

                if (count > 1) {
                    nai_dns_free(d, e->in6.elts);
                };
                e->in6.elts = in6;
            };

            e->in6.count ++;
            break;
#endif

        default:
            break;
        };
    };


    /* add record for address to host */
    switch (addr->sa_family) {
    case AF_INET:
        sa4 = (nai_sockaddr_in4_t*)addr;
        qtype = NAI_DNS_QTYPE_ADDR;
        h = nai_dns_lookup_addr(&d->type[qtype], AF_INET, &sa4->sin_addr);
        break;
#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        sa6 = (nai_sockaddr_in6_t*)addr;
        qtype = NAI_DNS_QTYPE_ADDR6;
        h = nai_dns_lookup_addr(&d->type[qtype], AF_INET6, &sa6->sin6_addr);
        break;
#endif
    default:
        assert(0);
        nai_errno = EAFNOSUPPORT;
        r = -1;
        goto _end;
    };
    if (h == 0) {
        h = nai_dns_entry_from_addr(d, addr);
        if (h == 0) {
            r = -1;
            goto _end;
        };
    };

    mem.len = nai_str_len(name);
    mem.ptr = nai_dns_alloc(d, mem.len+1);
    if (mem.ptr == 0) {
        r = -1;
        goto _end;
    };

    nai_memcpy(mem.ptr, nai_str(name), mem.len);
    mem.ptr[mem.len] = 0;

    switch (h->rtype) {
    case NAI_DNS_RTYPE_CNAME:
    case NAI_DNS_RTYPE_HNAME:
        if (nai_str_len(&h->hname.mem) > 0) {
            nai_dns_free(d, nai_str(&h->hname.mem));
        };
        break;
    default:
        break;
    };
    if (h->query) {
        nai_dns_free(d, h->query);
        h->query = 0;
    };

    if (h->permanent == 0) {
        h->permanent = 1;
        h->ttl = INT_MAX;
        h->expire = nai_tickcache_to_msec32() + d->expire;
        nai_list_entry_remove(&h->qent);
        nai_list_insert_tail(&d->qexpire, &h->qent);
    };
    h->rtype = NAI_DNS_RTYPE_HNAME;
    h->complete = 1;
    h->hname.mem = mem;


    /* notify */
    if (e->complete) {
        r = nai_dns_notify(d, e, NAI_DNS_NOTIFY_RESET);
    } else {
        r = nai_dns_query_complete(d, e);
    };
    if (r) {
        /* user closed */
        goto _end;
    };
    if (h->complete) {
        r = nai_dns_notify(d, h, NAI_DNS_NOTIFY_RESET);
    } else {
        r = nai_dns_query_complete(d, h);
    };
    if (r) {
        /* user closed */
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_dns_open(nai_dns_t* d, nai_evloop_t* loop)
{
    nai_int_t r;


    if (nai_evnode_is_opened(&d->ev)) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    d->root = nai_dns_entry_root(d);
    if (d->root == 0) {
        r = -1;
        goto _end;
    };

    nai_evnode_set_cb(&d->ev, nai_dns_event);

    r = nai_evnode_open(&d->ev, loop);
    if (r < 0) {
        goto _end;
    };

_end:
    return r;
};


nai_int_t nai_dns_reuse(nai_dns_t* d)
{
    return nai_dns_close_impl(d, 1);
};


nai_int_t nai_dns_close(nai_dns_t* d)
{
    return nai_dns_close_impl(d, 0);
};



//////////////////////////////////////////////////////////////////////////////
// dns query apis


typedef struct nai_dns_query_wait_s {
    nai_dns_query_t q;
    nai_cond_t* wait;
    nai_dns_result_t* sult;
    char* buf;
    size_t size;
} nai_dns_query_wait_t;


static nai_int_t nai_dns_query_copy(
    nai_dns_query_t* q, const nai_dns_result_t* rs)
{
    nai_int_t r;
    nai_mutex_t* m;
    nai_dns_query_wait_t* w;


    w = (nai_dns_query_wait_t*)q;
    w->size = nai_dns_result_copy(rs, w->sult, w->buf, w->size);


    /* notify */
    m = nai_thread_mutex();
    assert(m != 0);
    nai_mutex_lock(m);

    w->sult = 0;
    nai_cond_signal(w->wait);

    nai_mutex_unlock(m);

    r = 0;

    return r;
};


static nai_int_t nai_dns_query_impl(
    nai_dns_query_t* q, nai_dns_t* d, 
    nai_dns_result_t* rs, nai_int_t blocking)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t sig;
    nai_int_t inloop;
    nai_evloop_t* loop;
    nai_dns_entry_t* e;


    if (q->ent) {
        nai_errno = EINPROGRESS;
        r = -1;
        goto _end;
    };

    loop = nai_evnode_get_loop(&d->ev);
    if (loop == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    if (blocking || rs) {
        inloop = nai_evloop_in_dispatch(loop);
    } else {
        /* disable warning */
        inloop = 0;
    };

    if (blocking) {
        if (inloop) {
            nai_errno = EDEADLK;
            r = -1;
            goto _end;
        };
    };

    if (rs == 0 || !inloop) {

        q->dns = d;
        e = d->root;

        nai_spin_lock(&e->lock);

        sig = e->wait == 0;
        q->ent = e;
        q->next = e->wait;
        e->wait = q;

        nai_spin_unlock(&e->lock);

        if (sig) {
            r = nai_evnode_post(&d->ev, 0);
            if (r < 0) {
                /* ignore error */
            };
        };

        nai_errno = NAI_EINPROGRESS;
        r = -1;
        goto _end;
    };

    e = nai_dns_entry(d, q->type, &q->name);
    if (e == 0) {
        r = -1;
        goto _end;
    };

    q->dns = d;
    if (e->complete) {
        r = nai_dns_result(d, e, rs);
        if (r < 0) {
            ec = nai_errno;
            if (ec == NAI_EAGAIN) {
                ec = NAI_EINPROGRESS;
            };
        };
    } else {
        ec = NAI_EINPROGRESS;
        r = -1;
    };

    if (r < 0 && ec == NAI_EINPROGRESS) {
        q->ent = e;
        q->next = e->wait;
        e->wait = q;

        /* set errno is 'inprogress' */
        nai_errno = ec;
    };

_end:
    return r;
};


nai_int_t nai_dns_query_submit(
    nai_dns_query_t* q, nai_dns_t* d, nai_dns_result_t* rs)
{
    nai_int_t r;


    r = nai_dns_query_impl(q, d, rs, 0);

    return r;
};


nai_int_t nai_dns_query_cancel(nai_dns_query_t* q)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t locked;
    nai_evloop_t* loop;
    nai_dns_t* d;
    nai_dns_entry_t* e;
    nai_dns_query_t** prev;


    if (q->ent == 0) {
        r = 0;
        goto _end;
    };

    d = q->dns;
    if (d == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    loop = nai_evnode_get_loop(&d->ev);
    if (loop == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    e = q->ent;
    if (e == 0) {
        nai_errno = ECANCELED;
        r = -1;
        goto _end;
    };

    locked = 0;
    if (!nai_evloop_in_dispatch(loop)) {

        if (e != d->root) {
            r = nai_evloop_exec(loop, (nai_task_f)nai_dns_query_cancel, q);
            goto _end;
        };

        nai_spin_lock(&e->lock);

        if (e != q->ent) {
            nai_spin_unlock(&e->lock);

            r = nai_evloop_exec(loop, (nai_task_f)nai_dns_query_cancel, q);
            goto _end;
        };

        locked = 1;
    };

    prev = &e->wait;
    for (;;) {
        if (prev[0] == q) {
            prev[0] = q->next;
            q->ent = 0;
            q->next = 0;
            break;
        };
        if (prev[0] == 0) {
            ec = ENOENT;
            if (locked) {
                goto _fail;
            };

            nai_errno = ec;
            r = -1;
            goto _end;
        };
    };

    if (locked) {
        nai_spin_unlock(&e->lock);
    };

    r = 0;

_end:
    return r;

_fail:
    if (locked) {
        nai_spin_unlock(&e->lock);
    };
    nai_errno = ec;
    r = -1;
    goto _end;
};


intptr_t nai_dns_query(nai_dns_query_t* q, 
    nai_dns_t* d, nai_dns_result_t* rs, void* buf, size_t size)
{
    intptr_t r;
    nai_int_t ec;
    nai_mutex_t* m;
    nai_dns_query_wait_t w;


    if (rs == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    m = nai_thread_mutex();
    if (m == 0) {
        r = -1;
        goto _end;
    };

    w.q = *q;
    w.buf = buf;
    w.size = size;
    w.sult = rs;
    w.wait = nai_thread_local_cond();
    if (w.wait == 0) {
        r = -1;
        goto _end;
    };

    nai_dns_query_set_cb(&w.q, nai_dns_query_copy);
    r = nai_dns_query_impl(&w.q, d, 0, 1);
    if (r < 0) {
        ec = nai_errno;
        if (ec != NAI_EINPROGRESS) {
            goto _end;
        };
    };


    /* wait */
    nai_mutex_lock(m);

    while (w.sult) {
        nai_cond_wait(w.wait, m);
    };

    nai_mutex_unlock(m);


    if (rs->error) {
        nai_errno = rs->error;
        r = -1;
    } else {
        r = w.size;
    };

_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// dns result apis


typedef struct nai_dns_addrs_s {
    nai_dns_serv_t* serv;
    struct {
        nai_int_t count;
        nai_addr_in4_t* elts;
    } in4;
#if (NAI_HAVE_SOCKADDR_IN6)
    struct {
        nai_int_t count;
        nai_addr_in6_t* elts;
    } in6;
#endif
} nai_dns_addrs_t;


static nai_int_t nai_dns_result_get_addrs(
    const nai_dns_result_t* rs, nai_dns_addrs_t* a, nai_int_t serv)
{
    nai_int_t r;
    nai_int_t n;
    uint16_t w = -1;
    uint16_t p = -1;
    nai_dns_serv_t* s;


    if (rs->error) {
        nai_errno = rs->error;
        r = -1;
        goto _end;
    };

    switch (rs->type) {
    case NAI_DNS_RTYPE_ADDR:
        if (a) {
            a->serv = 0;
            nai_dns_assign_addrs(a, rs);
        };
        r = nai_dns_count_addrs(rs);
        break;

    case NAI_DNS_RTYPE_SERV:
        s = rs->serv.elts;

        if (rs->serv.count <= 1) {
            if (serv < 0) {
                serv = 0;
            };
        } else if (serv < 0) {
            for (n = 0; n < rs->serv.count; n ++) {
                if (s[n].error) {
                    continue;
                };
                if (nai_dns_count_addrs(&s[n]) <= 0) {
                    continue;
                };
                if (p > s[n].priority || (
                    p == s[n].priority && w > s[n].weight)) {
                    p = s[n].priority;
                    w = s[n].weight;
                    serv = n;
                };
            };
        };
        if (serv < 0 || serv >= rs->serv.count) {
            goto _noent;
        };
        if (s[serv].error) {
            goto _noent;
        };

        s = s + serv;
        if (a) {
            a->serv = s;
            nai_dns_assign_addrs(a, s);
        };
        r = nai_dns_count_addrs(s);
        break;

    default:
        goto _noent;
    };


_end:
    return r;

_noent:
    if (a) {
        a->serv = 0;
        nai_dns_clear_addrs(a);
    };
    r = 0;
    goto _end;
};


nai_int_t nai_dns_result_count(const nai_dns_result_t* rs, nai_int_t serv)
{
    nai_int_t n;

    n = nai_dns_result_get_addrs(rs, 0, serv);

    return n;
};


intptr_t nai_dns_result_addr(
    const nai_dns_result_t* rs, nai_int_t addr, nai_int_t serv, 
    void* buf, size_t buflen, nai_int_t with_opt)
{
    intptr_t r;
    nai_socknbuf_in_t nbuf;


    nbuf.len = sizeof(nbuf.storage);
    r = nai_dns_result_sockaddr(rs, addr, serv, &nbuf.addr, &nbuf.len);
    if (r < 0) {
        goto _end;
    };

    r = nai_sockaddr_ntop(&nbuf.addr, nbuf.len, buf, buflen, with_opt);

_end:
    return r;
};


nai_int_t nai_dns_result_sockaddr(
    const nai_dns_result_t* rs, nai_int_t addr, nai_int_t serv, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t r;
    nai_dns_addrs_t a;


    r = nai_dns_result_get_addrs(rs, &a, serv);
    if (r < 0) {
        goto _end;
    };

    if (r <= 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    if (addr < 0) {
        addr = 0;
    };

    if (addr < a.in4.count) {
        r = nai_sockaddr_mk_inet(AF_INET, 
            &a.in4.elts[addr], 
            a.serv ? a.serv->port : 0, name, namelen);

#if (NAI_HAVE_SOCKADDR_IN6)
    } else if (addr < r) {
        r = nai_sockaddr_mk_inet(AF_INET6, 
            &a.in6.elts[addr-a.in4.count], 
            a.serv ? a.serv->port : 0, name, namelen);
#endif

    } else {
        nai_errno = ENOENT;
        r = -1;
    };


_end:
    return r;
};


intptr_t nai_dns_result_copy(
    const nai_dns_result_t* rs, 
    nai_dns_result_t* cp, void* buf, size_t size)
{
    intptr_t r;
    nai_int_t n;
    size_t cpy;
    size_t out;
    uint8_t* dst;
    nai_dns_serv_t* s;
    nai_dns_serv_t* d;


    cp->type = rs->type;
    cp->ttl = rs->ttl;
    cp->error = rs->error;

    out = 0;
    dst = (uint8_t*)buf;
    switch (rs->type) {
    case NAI_DNS_RTYPE_CNAME:
    case NAI_DNS_RTYPE_HNAME:
        cpy = nai_str_len(&rs->name) + 1;
        if (size >= out + cpy) {
            nai_memcpy(dst + out, nai_str(&rs->name), cpy);
            nai_str_setm(&cp->name, dst + out, cpy - 1);
        } else {
            nai_str_setn(&cp->name);
        };
        out += cpy;
        break;

    case NAI_DNS_RTYPE_ADDR:
        cpy = sizeof(*rs->in4.elts) * rs->in4.count;
        if (size >= out + cpy) {
            nai_memcpy(dst + out, rs->in4.elts, cpy);
            cp->in4.count = rs->in4.count;
            cp->in4.elts = (nai_addr_in4_t*)(dst + out);
        } else {
            cp->in4.count = 0;
            cp->in4.elts = 0;
        };
        out += cpy;

#if (NAI_HAVE_SOCKADDR_IN6)
        cpy = sizeof(*rs->in6.elts) * rs->in6.count;
        if (size >= out + cpy) {
            nai_memcpy(dst + out, rs->in6.elts, cpy);
            cp->in6.count = rs->in6.count;
            cp->in6.elts = (nai_addr_in6_t*)(dst + out);
        } else {
            cp->in6.count = 0;
            cp->in6.elts = 0;
        };
        out += cpy;
#endif
        break;

    case NAI_DNS_RTYPE_SERV:
        cpy = sizeof(*rs->serv.elts) * rs->serv.count;
        if (size >= out + cpy) {
            nai_memcpy(dst + out, rs->serv.elts, cpy);
            cp->serv.count = rs->in4.count;
            cp->serv.elts = (nai_dns_serv_t*)(dst + out);
        } else {
            cp->serv.count = 0;
            cp->serv.elts = 0;
        };
        out += cpy;

        /* copy addresses */
        for (n = 0; n < rs->serv.count; n ++) {
            s = rs->serv.elts + n;
            d = cp->serv.elts + n;
            if (s->error) {
                continue;
            };

            cpy = sizeof(*s->in4.elts) * s->in4.count;
            if (size >= out + cpy) {
                nai_memcpy(dst + out, s->in4.elts, cpy);
                d->in4.count = s->in4.count;
                d->in4.elts = (nai_addr_in4_t*)(dst + out);
            } else {
                if (cp->serv.count > n) {
                    d->in4.count = 0;
                    d->in4.elts = 0;
                };
            };
            out += cpy;

#if (NAI_HAVE_SOCKADDR_IN6)
            cpy = sizeof(*s->in6.elts) * s->in6.count;
            if (size >= out + cpy) {
                nai_memcpy(dst + out, s->in6.elts, cpy);
                d->in6.count = s->in6.count;
                d->in6.elts = (nai_addr_in6_t*)(dst + out);
            } else {
                if (cp->serv.count > n) {
                    d->in6.count = 0;
                    d->in6.elts = 0;
                };
            };
            out += cpy;
#endif
        };

        /* copy names */
        for (n = 0; n < rs->serv.count; n ++) {
            s = rs->serv.elts + n;
            d = cp->serv.elts + n;
            if (s->error) {
                continue;
            };

            cpy = nai_str_len(&s->name) + 1;
            if (size >= out + cpy) {
                nai_memcpy(dst + out, nai_str(&s->name), cpy);
                nai_str_setm(&d->name, dst + out, cpy - 1);
            } else {
                if (cp->serv.count > n) {
                    nai_str_setn(&d->name);
                };
            };
            out += cpy;
        };
        break;

    default:
        assert(0);
        break;
    };

    r = out;

    return r;
};


