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
/// @file       nai_agent.c
/// @brief      
/// @details
/// @date       2021-04-18
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_agent.h"
#include "nai/io/nai_dns.h"
#include "nai/io/nai_connection.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_strmatch.h"
#include "nai/runtime/nai_regex.h"
#include "nai/runtime/nai_uri.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_log.h"
#include "nai_agent_io.h"



extern nai_agent_ops_t nai_agent_socks4;
extern nai_agent_ops_t nai_agent_socks4a;
extern nai_agent_ops_t nai_agent_socks5;
extern nai_agent_ops_t nai_agent_http;
static nai_agent_ops_t nai_agent_direct;
static nai_agent_ops_t* nai_agent_ops[] = {
    &nai_agent_direct, 
    &nai_agent_socks4, 
    &nai_agent_socks4a, 
    &nai_agent_socks5, 
    &nai_agent_http, 
    0, 
};


static nai_agent_node_t nai_agent_default;



#define NAI_AGENT_RULE_MATCH        1
#define NAI_AGENT_RULE_WMATCH       2
#define NAI_AGENT_RULE_REGEX        3
#define NAI_AGENT_RULE_CIDR_IN4     4
#if (NAI_HAVE_SOCKADDR_IN6)
#define NAI_AGENT_RULE_CIDR_IN6     5
#endif


typedef struct nai_agent_rule_s {

    nai_int_t node;
    nai_int_t type;
    nai_int_t proto_bits;
    union {
        nai_str_t match;
        nai_cidr_t cidr;
#if (NAI_HAVE_REGEX)
        nai_regex_t regex;
#endif
    };

} nai_agent_rule_t;


typedef struct nai_agent_query_s {

    nai_int_t next;
    nai_int_t proto;

    union {
        struct {
            uint16_t valid_addr:1;
            uint16_t valid_sockaddr:1;
            uint16_t temp_addr:1;   /* the memory of 'addr' is temporary */
            uint16_t temp_host:1;   /* the memory of 'host' is temporary */
            uint16_t found:1;
        };
        uint16_t flags;
    };
    uint16_t port;
    nai_str_t host;
    nai_str_t addr;
    nai_socknbuf_t sockaddr;

} nai_agent_query_t;



static nai_int_t nai_agent_find_node(nai_agent_t* p, const nai_mem_t* name)
{
    nai_int_t r;
    nai_int_t n;
    nai_agent_node_t* elts;

    elts = (nai_agent_node_t*)p->nodes.elts;
    for (n = 0; n < (intptr_t)p->nodes.count; n ++) {
        if (nai_str_eq(name, &elts[n].name)) {
            r = n;
            goto _end;
        };
    };

    r = -1;

_end:
    return r;
};


static nai_int_t nai_agent_find_rule(nai_agent_t* p, nai_agent_query_t* m)
{
    nai_int_t r;
    nai_int_t n;
    nai_agent_rule_t* u;


    u = (nai_agent_rule_t*)p->rules.elts;
    for (n = m->next; n < (intptr_t)p->rules.count; n ++) {
        if (!(u[n].proto_bits & (1 << m->proto))) {
            continue;
        };

        switch (u[n].type) {
        case NAI_AGENT_RULE_MATCH:
            if (nai_str_caseeq(&u[n].match, &m->host)) {
                goto _end;
            };
            break;

        case NAI_AGENT_RULE_WMATCH:
            if (nai_wildmatch(nai_str(&u[n].match), 
                nai_str(&m->host), NAI_WMATCH_CASELESS)) {
                goto _end;
            };

            if (m->valid_addr == 0) {
                goto _addr;
            };

            if (nai_wildmatch(nai_str(&u[n].match), 
                nai_str(&m->addr), NAI_WMATCH_CASELESS)) {
                goto _end;
            };
            break;

#if (NAI_HAVE_REGEX)
        case NAI_AGENT_RULE_REGEX:
            r = nai_regex_exec(&u[n].regex, 
                nai_str(&m->host), nai_str_len(&m->host), 0, 0);
            if (r >= 0) {
                goto _end;
            };

            if (m->valid_addr == 0) {
                goto _addr;
            };

            r = nai_regex_exec(&u[n].regex, 
                nai_str(&m->addr), nai_str_len(&m->addr), 0, 0);
            if (r >= 0) {
                goto _end;
            };
            break;
#endif

        case NAI_AGENT_RULE_CIDR_IN4:
#if (NAI_HAVE_SOCKADDR_IN6)
        case NAI_AGENT_RULE_CIDR_IN6:
#endif
            if (m->valid_sockaddr == 0) {
                goto _addr;
            };

            r = nai_sockaddr_match(
                &m->sockaddr.addr, m->sockaddr.len, &u[n].cidr);
            if (r != 0) {
                goto _end;
            };
            break;

        default:
            assert(0);
            break;
        };
    };


_end:
    m->next = n;
    m->found = 1;
    r = n;

    return r;

_addr:
    m->next = n;
    r = -1;

    return r;
};


static nai_int_t nai_agent_add_default_node(nai_agent_t* p)
{
    nai_int_t r;
    nai_agent_node_t* n;

    n = (nai_agent_node_t*)nai_array_push(&p->nodes);
    if (n == 0) {
        r = -1;
        goto _end;
    };

    n->ctx = p;
    n->ops = &nai_agent_direct;
    nai_str_setc(&n->name, "default");
    r = 0;

_end:
    return r;
};



nai_int_t nai_agent_init(nai_agent_t* p, nai_pool_t* pool)
{
    nai_int_t r;

    p->flags = 0;
    p->loop = 0;
    p->pool = pool;
    nai_array_init(&p->rules, sizeof(nai_agent_rule_t), pool);
    nai_array_init(&p->nodes, sizeof(nai_agent_node_t), pool);
    r = 0;

    return r;
};


nai_int_t nai_agent_add_node_uri(
    nai_agent_t* p, const nai_mem_t* name, const nai_uri_t* attr)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t ec;
    nai_uri_t uri;
    nai_agent_ops_t* ops;
    nai_agent_node_t* node;


    if (p->opened) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };


    /* find node type */
    n = 0;
    for ( ; n < (intptr_t)nai_countof(nai_agent_ops); n ++) {
        ops = nai_agent_ops[n];
        if (nai_strncmp(ops->name, 
            nai_str(&attr->scheme), nai_str_len(&attr->scheme)) == 0) {
            break;
        };
    };
    if (n >= (intptr_t)nai_countof(nai_agent_ops)) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };


    /* add default node before any node added */
    if (p->nodes.count <= 0) {
        r = nai_agent_add_default_node(p);
        if (r < 0) {
            goto _end;
        };
    };


    /* check exists nodes */
    r = nai_agent_find_node(p, name);
    if (r >= 0) {
        nai_errno = EEXIST;
        r = -1;
        goto _end;
    };


    /* fix attr */
    if (nai_str_len(&attr->hostname) <= 0 && 
        nai_str_len(&attr->host) > 0) {
        r = nai_uri_parse(&uri, 
            nai_str(&attr->host), nai_str_len(&attr->host), 
            NAI_URI_STRICT|NAI_URI_HOSTONLY);
        if (r < 0) {
            goto _end;
        };

        uri.scheme = attr->scheme;
        uri.host = attr->host;
        uri.user = attr->user;
        uri.password = attr->password;
        uri.path = attr->path;
        uri.query = attr->query;
        uri.fragment = attr->fragment;
        uri.flags = attr->flags;
        attr = &uri;
    };


    /* add new node */
    node = (nai_agent_node_t*)nai_array_push(&p->nodes);
    if (node == 0) {
        r = -1;
        goto _end;
    };

    node->ctx = 0;
    node->ops = ops;
    r = (nai_int_t)nai_str_dup(
        &node->name, nai_str(name), nai_str_len(name), p->pool);
    if (r < 0) {
        p->nodes.count --;
        r = -1;
        goto _end;
    };

    r = node->ops->open(node, p, attr);
    if (r < 0) {
        ec = nai_errno;
        nai_free(nai_str(&node->name));
        nai_errno = ec;
        p->nodes.count --;
        r = -1;
        goto _end;
    };

    r = (nai_int_t)(p->nodes.count - 1);

_end:
    return r;
};


nai_int_t nai_agent_add_node(
    nai_agent_t* p, const nai_mem_t* name, const nai_mem_t* uri)
{
    nai_int_t r;
    nai_uri_t attr;


    r = nai_uri_parse(&attr, 
        nai_str(uri), nai_str_len(uri), NAI_URI_STRICT);
    if (r < 0) {
        goto _end;
    };

    r = nai_agent_add_node_uri(p, name, &attr);

_end:
    return r;
};


nai_int_t nai_agent_add_rule(nai_agent_t* p, 
    const nai_mem_t* name, const nai_mem_t* pattern, nai_int_t protos)
{
    intptr_t r;
    nai_int_t node;
    nai_int_t len;
    const char* str;
    nai_cidr_t cidr;
    nai_agent_rule_t* u;
    nai_agent_node_t* n;

#if (NAI_HAVE_REGEX)
    nai_int_t ec;
    nai_str_t copy;
#endif


    if (p->opened) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    str = nai_str(pattern);
    len = nai_str_len(pattern);
    if (len <= 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    /* add default node before any rule added */
    if (p->nodes.count <= 0) {
        r = nai_agent_add_default_node(p);
        if (r < 0) {
            goto _end;
        };
    };

    /* find node */
    if (nai_str_len(name) <= 0) {
        node = 0;
    } else {
        node = nai_agent_find_node(p, name);
        if (node < 0) {
            nai_errno = ENOENT;
            r = -1;
            goto _end;
        };
    };
    n = (nai_agent_node_t*)p->nodes.elts;
    n += node;


    /* check supported protocol */
    if (protos == 0) {
        protos = n->ops->proto_bits;
    } else if ((n->ops->proto_bits & protos) != protos) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };


    /* add new rule */
    u = (nai_agent_rule_t*)nai_array_push(&p->rules);
    if (u == 0) {
        r = -1;
        goto _end;
    };

    if (str[0] == '~') {

#if (NAI_HAVE_REGEX)
        r = nai_str_dup(&copy, str+1, len-1, 0);
        if (r < 0) {
            goto _fail;
        };

        nai_regex_init(&u->regex);
        r = nai_regex_compile(
            &u->regex, nai_str(&copy), NAI_REGEX_CASELESS, 0, p->pool);
        if (r < 0) {
            ec = nai_errno;
            nai_free(nai_str(&copy));
            nai_errno = ec;
            goto _fail;
        };

        nai_free(nai_str(&copy));
        u->type = NAI_AGENT_RULE_REGEX;
        p->require_addr = 1;

#else
        nai_errno = ENOTSUP;
        r = -1;
        goto _fail;
#endif

    } else {

        r = nai_inet_ptoc(str, len, &cidr);
        if (r >= 0) {

            switch (cidr.af) {
            case AF_INET:
                u->type = NAI_AGENT_RULE_CIDR_IN4;
                u->cidr.af = cidr.af;
                u->cidr.in4 = cidr.in4;
                break;
#if (NAI_HAVE_SOCKADDR_IN6)
            case AF_INET6:
                u->type = NAI_AGENT_RULE_CIDR_IN6;
                u->cidr.af = cidr.af;
                u->cidr.in6 = cidr.in6;
                break;
#endif
            default:
                assert(0);
                break;
            };

            p->require_sockaddr = 1;

        } else {

            if (nai_strchr(str, '/')) {
                goto _fail;
            };

            r = nai_str_dup(&u->match, str, len, p->pool);
            if (r < 0) {
                goto _fail;
            };

            if (nai_strchr(str, '*') || nai_strchr(str, '?')) {
                u->type = NAI_AGENT_RULE_WMATCH;
                p->require_addr = 1;
            } else {
                u->type = NAI_AGENT_RULE_MATCH;
            };
        };
    };

    u->node = node;
    u->proto_bits = protos;
    r = 0;

_end:
    return (nai_int_t)r;

_fail:
    p->rules.count --;
    goto _end;
};


nai_int_t nai_agent_open(nai_agent_t* p, nai_evloop_t* loop)
{
    nai_int_t r;


    if (p->nodes.count <= 0) {
        r = nai_agent_add_default_node(p);
        if (r < 0) {
            goto _end;
        };
    };

    p->opened = 1;
    p->loop = loop;
    r = 0;

_end:
    return r;
};


nai_int_t nai_agent_reuse(nai_agent_t* p)
{
    nai_int_t r;


    if (p->opened == 0) {
        r = 0;
        goto _end;
    };

    p->opened = 0;
    p->loop = 0;
    r = 0;

_end:
    return r;
};


nai_int_t nai_agent_close(nai_agent_t* p)
{
    nai_int_t r;
    intptr_t i;
    char* str;
    nai_agent_rule_t* u;
    nai_agent_node_t* n;


    if (p->pool != 0) {
        goto _skip;
    };

    u = (nai_agent_rule_t*)p->rules.elts;
    for ( ; p->rules.count > 0; p->rules.count --) {
        i = p->rules.count - 1;
        switch (u[i].type) {
        case NAI_AGENT_RULE_REGEX:
#if (NAI_HAVE_REGEX)
            r = nai_regex_close(&u[i].regex);
            if (r < 0) {
                goto _end;
            };
#endif
            break;
        case NAI_AGENT_RULE_MATCH:
        case NAI_AGENT_RULE_WMATCH:
            str = nai_str(&u[i].match);
            if (str) {
                nai_free(str);
                nai_str_setn(&u[i].match);
            };
            break;
        default:
            break;
        };
    };

    n = (nai_agent_node_t*)p->nodes.elts;
    for ( ; p->nodes.count > 0; p->nodes.count --) {
        i = p->nodes.count - 1;
        if (n[i].ops) {
            r = n[i].ops->close(&n[i]);
            if (r < 0) {
                goto _end;
            };

            n[i].ops = 0;
        };
        if (i > 0) {
            str = nai_str(&n[i].name);
            if (str) {
                nai_free(str);
                nai_str_setn(&n[i].name);
            };
        };
    };

_skip:
    nai_array_close(&p->rules);
    nai_array_close(&p->nodes);
    p->opened = 0;
    r = 0;

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// connect


static nai_int_t nai_agent_dns_query(
    nai_conn_attr_t* a, nai_iobase_t* c, nai_agent_query_t* m);


static nai_int_t nai_agent_do_connect(
    nai_conn_attr_t* a, nai_iobase_t* c, nai_agent_query_t* m)
{
    nai_int_t r;
    nai_int_t ec;
    nai_agent_t* p;
    nai_agent_rule_t* u;
    nai_agent_node_t* n;
    nai_agent_attr_t attr;
    nai_agent_dest_t dest;


    /* get agent node */
    p = a->agent;
    if (p != 0) {
        r = m->next;
        n = (nai_agent_node_t*)p->nodes.elts;
        if (r < 0 || r >= (intptr_t)p->rules.count) {
            n = &n[0];
        } else {
            u = (nai_agent_rule_t*)p->rules.elts;
            n = n + u[r].node;
        };
    } else {
        n = &nai_agent_default;
    };


    /* make dest */
    dest.proto = m->proto;
    dest.port = m->port;
    if (a->sel || n->ops->require & NAI_AGENT_REQUIRE_SOCKADDR || (
        m->valid_sockaddr && 
        nai_str(&m->host) == nai_str(&m->addr))) {

        if (!m->valid_sockaddr) {
            ec = EINVAL;
            nai_log_error(NAI_LOG_CORE, ec, 
                "cannot start connect bypass node %.*s without address", 
                nai_str_len(&n->name), nai_str(&n->name));

            nai_errno = ec;
            r = -1;
            goto _end;
        };

        switch (m->sockaddr.addr.sa_family) {
        case AF_INET:
            nai_str_setm(&dest.addr, 
                &m->sockaddr.addr_in4.sin_addr, 
                sizeof(m->sockaddr.addr_in4.sin_addr));
            dest.type = NAI_AGENT_ADDR_IN;
            break;
#if (NAI_HAVE_SOCKADDR_IN6)
        case AF_INET6:
            nai_str_setm(&dest.addr, 
                &m->sockaddr.addr_in6.sin6_addr, 
                sizeof(m->sockaddr.addr_in6.sin6_addr));
            dest.type = NAI_AGENT_ADDR_IN6;
            break;
#endif
        default:
            ec = EAFNOSUPPORT;
            nai_log_error(NAI_LOG_CORE, ec, 
                "cannot start connect bypass node %.*s with "
                "unknown address family(%d)", 
                nai_str_len(&n->name), nai_str(&n->name), 
                m->sockaddr.addr.sa_family);

            nai_errno = ec;
            r = -1;
            goto _end;
        };

        dest.temp = 1;

    } else {

        dest.addr = m->host;
        dest.type = NAI_AGENT_ADDR_DOMAIN;
        dest.temp = (uint8_t)m->temp_host;
    };


    /* set addresses */
    if (c->st.connection) {
        if (m->valid_addr) {
            r = nai_connection_set_name(
                (nai_connection_t*)c, 
                NAI_CONN_PEERNAME, &m->addr, m->port);
            if (r < 0) {
                nai_log_error(NAI_LOG_CORE, 
                    nai_errno, "set connection peer name failed");
                goto _end;
            };
        };
        if (m->valid_sockaddr) {
            r = nai_connection_set_addr(
                (nai_connection_t*)c, 
                NAI_CONN_PEERNAME, &m->sockaddr.addr, m->sockaddr.len);
            if (r < 0) {
                nai_log_error(NAI_LOG_CORE, 
                    nai_errno, "set connection peer address failed");
                goto _end;
            };
        };
    };

    attr.node = n;
    attr.dns = a->dns;
    r = n->ops->connect(&attr, c, &dest);

_end:
    return r;
};


static nai_int_t nai_agent_select_addr(
    nai_conn_attr_t* a, nai_iobase_t* c, 
    nai_socknbuf_t* list, nai_socknbuf_t* nbuf)
{
    nai_int_t r;
    nai_int_t index;
    nai_int_t count;
    nai_addr_in4_t* in4;
#if (NAI_HAVE_SOCKADDR_IN6)
    nai_addr_in6_t* in6;
#endif
    nai_socknbuf_t* name;
    nai_dns_result_t rs;
    uint8_t buf[sizeof(nai_socknbuf_t) * 2];


    /* get all inet4 address */
    in4 = (nai_addr_in4_t*)buf;
    count = 0;
    name = list;
    while (nai_socknbuf_is_valid(name)) {

        switch (name->addr.sa_family) {
        case AF_INET:
            nai_memcpy(&in4[count], 
                &name->addr_in4.sin_addr, sizeof(*in4));
            count ++;
            break;
        default:
            break;
        };

        name = nai_socknbuf_next(name);
    };

    rs.in4.count = count;
    rs.in4.elts = in4;


#if (NAI_HAVE_SOCKADDR_IN6)
    /* get all inet6 address */
    in6 = (nai_addr_in6_t*)(in4 + count);
    count = 0;
    name = list;
    while (nai_socknbuf_is_valid(name)) {

        switch (name->addr.sa_family) {
        case AF_INET6:
            nai_memcpy(&in6[count], 
                &name->addr_in6.sin6_addr, sizeof(*in6));
            count ++;
            break;
        default:
            break;
        };

        name = nai_socknbuf_next(name);
    };

    rs.in6.count = count;
    rs.in6.elts = in6;
#endif

    rs.type = NAI_DNS_RTYPE_ADDR;
    rs.error = 0;


    index = a->sel(c, &rs);
    nbuf->len = sizeof(nbuf->storage);
    r = nai_dns_result_sockaddr(
        &rs, index, -1, &nbuf->addr, &nbuf->len);

    return r;
};


static nai_int_t nai_agent_query_host(
    nai_conn_attr_t* a, nai_iobase_t* c, 
    const nai_str_t* hostname, nai_socknbuf_t* nbuf)
{
    intptr_t r;
    nai_int_t index;
    nai_dns_query_t q;
    nai_dns_result_t rs;
    uint8_t buf[1024];


    if (a->dns) {
        nai_dns_query_init(&q);
        nai_dns_query_set_name(&q, (nai_mem_t*)hostname);
        r = nai_dns_query(&q, a->dns, &rs, buf, sizeof(buf));
        if (r >= 0) {
            if (a->sel) {
                index = a->sel(c, &rs);
            } else {
                index = -1;
            };
            nbuf->len = sizeof(nbuf->storage);
            r = nai_dns_result_sockaddr(
                &rs, index, -1, &nbuf->addr, &nbuf->len);
            goto _end;
        };
        if (nai_errno != EDEADLK) {
            goto _end;
        };
    };


    r = nai_sockaddr_list(
        nai_str(hostname), 0, buf, sizeof(buf));
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "query host '%.*s' through the system failed", 
            nai_str_len(hostname), nai_str(hostname));

        goto _end;
    };

    if (a->sel) {
        r = nai_agent_select_addr(a, c, (nai_socknbuf_t*)buf, nbuf);
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "get dns result of host '%.*s' failed", 
                nai_str_len(hostname), nai_str(hostname));

            goto _end;
        };
    };

    r = 0;

_end:
    return (nai_int_t)r;
};


static nai_int_t nai_agent_connect_host(
    nai_conn_attr_t* a, nai_iobase_t* c, 
    nai_int_t proto, const nai_str_t* hostname, nai_int_t port)
{
    intptr_t r;
    nai_int_t query;
    nai_dns_t* dns;
    nai_agent_t* p;
    nai_agent_query_t m;
    nai_agent_rule_t* u;
    nai_agent_node_t* n;
    nai_agent_ops_t* ops;
    nai_conn_attr_t attr;
    char addr[128];


    if (a == 0) {
        nai_conn_attr_init(&attr);
        a = &attr;
    };

    p = a->agent;
    dns = a->dns;
    if (dns && p && p->loop && 
        p->loop != nai_evnode_get_loop(&dns->ev)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    m.flags = 0;
    m.proto = proto;
    m.next = 0;
    m.port = (uint16_t)port;
    m.host = *hostname;
    m.temp_host = 1;

    m.sockaddr.len = sizeof(m.sockaddr.storage);
    r = nai_sockaddr_pton(nai_str(hostname), 
        nai_str_len(hostname), &m.sockaddr.addr, &m.sockaddr.len);
    if (r >= 0) {
        m.valid_addr = 1;
        m.valid_sockaddr = 1;
        m.addr = m.host;
        m.temp_addr = 1;
    };

    if (p != 0) {
        r = nai_agent_find_rule(p, &m);
    } else {
        r = 0;
    };
    if (r < 0) {
        query = 1;
    } else {
        if (p != 0) {
            n = (nai_agent_node_t*)p->nodes.elts;
            if (r < 0 || r >= (intptr_t)p->rules.count) {
                n = &n[0];
            } else {
                u = (nai_agent_rule_t*)p->rules.elts;
                n = n + u[r].node;
            };
        } else {
            n = &nai_agent_default;
        };
        query = 0;
        ops = n->ops;
        if (a->sel || 
            (ops->require & NAI_AGENT_REQUIRE_ADDR && !m.valid_addr) || 
            (ops->require & NAI_AGENT_REQUIRE_SOCKADDR && !m.valid_sockaddr)) {
            query = 1;
        };
    };

    c->agent = 0;
    if (query) {
        /* find proc require address or sockaddr */

        /* asynchronous dns query and connect */
        if (dns && p && p->loop && !nai_iobase_is_blocking(c)) {
            r = nai_agent_dns_query(a, c, &m);
            goto _end;
        };

        /* synchronous dns query */
        r = nai_agent_query_host(a, c, hostname, &m.sockaddr);
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "query host '%.*s' failed", 
                nai_str_len(hostname), nai_str(hostname));

            goto _end;
        };

        r = nai_sockaddr_ntop(
            &m.sockaddr.addr, m.sockaddr.len, addr, sizeof(addr), 0);
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "convert address of host '%.*s' to text failed", 
                nai_str_len(hostname), nai_str(hostname));

            goto _end;
        };

        m.temp_addr = 1;
        m.valid_addr = 1;
        m.valid_sockaddr = 1;
        nai_str_setm(&m.addr, addr, r);

        if (p != 0) {
            r = nai_agent_find_rule(p, &m);
            if (r < 0) {
                nai_log_error(NAI_LOG_CORE, ENOENT, 
                    "cannot connect host '%.*s' without a matching rule", 
                    nai_str_len(hostname), nai_str(hostname));
                nai_errno = ENOENT;
                goto _end;
            };
        };
    };

    r = nai_agent_do_connect(a, c, &m);

_end:
    return (nai_int_t)r;
};


static nai_int_t nai_agent_connect_addr(
    nai_conn_attr_t* a, nai_iobase_t* c, 
    nai_int_t proto, const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;
    nai_dns_t* dns;
    nai_agent_t* p;
    nai_agent_query_t m;
    nai_conn_attr_t attr;
    const nai_sockaddr_info_t* i;
    char addr[128];


    if (namelen <= 0 || 
        namelen > (nai_int_t)sizeof(m.sockaddr.storage)) {
        nai_log_error(NAI_LOG_CORE, 
            EINVAL, "cannot start the connection with an invalid address");
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    i = nai_sockaddr_info(name->sa_family);
    if (i == 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "unknown address family %d", 
            name->sa_family);
        r = -1;
        goto _end;
    };

    r = nai_sockaddr_ntop(name, namelen, addr, sizeof(addr), 0);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "convert address to text failed");
        goto _end;
    };


    if (a == 0) {
        nai_conn_attr_init(&attr);
        a = &attr;
    };


    p = a->agent;
    dns = a->dns;
    if (dns && p && p->loop && 
        p->loop != nai_evnode_get_loop(&dns->ev)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    nai_str_setm(&m.host, addr, r);
    nai_str_setm(&m.addr, addr, r);
    nai_memcpy(&m.sockaddr.addr, name, namelen);
    m.sockaddr.len = namelen;
    m.proto = proto;
    m.next = 0;
    m.port = nai_sockaddr_get_port(i, name);
    m.flags = 0;
    m.valid_addr = 1;
    m.valid_sockaddr = 1;
    m.temp_addr = 1;
    m.temp_host = 1;

    if (p != 0) {
        r = nai_agent_find_rule(p, &m);
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, ENOENT, 
                "cannot connect host '%.*s' without a matching rule", 
                nai_str_len(&m.host), nai_str(&m.host));
            goto _end;
        };
    };

    c->agent = 0;
    r = nai_agent_do_connect(a, c, &m);

_end:
    return (nai_int_t)r;
};


nai_int_t nai_stream_connect_addr(nai_iobase_t* s, 
    nai_conn_attr_t* c, const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;


    r = nai_agent_connect_addr(
        c, s, NAI_IO_TYPE_STREAM, name, namelen);

    return r;
};


nai_int_t nai_stream_connect_host(nai_iobase_t* s, 
    nai_conn_attr_t* c, const nai_str_t* host, nai_int_t port)
{
    nai_int_t r;


    r = nai_agent_connect_host(
        c, s, NAI_IO_TYPE_STREAM, host, port);

    return r;
};


nai_int_t nai_dgram_connect_addr(nai_iobase_t* s, 
    nai_conn_attr_t* c, const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;


    r = nai_agent_connect_addr(
        c, s, NAI_IO_TYPE_DGRAM, name, namelen);

    return r;
};


nai_int_t nai_dgram_connect_host(nai_iobase_t* s, 
    nai_conn_attr_t* c, const nai_str_t* host, nai_int_t port)
{
    nai_int_t r;


    r = nai_agent_connect_host(
        c, s, NAI_IO_TYPE_DGRAM, host, port);

    return r;
};



//////////////////////////////////////////////////////////////////////////////
// asynchronous dns query


typedef struct nai_agent_dns_query_s {
    nai_dns_query_t query;
    nai_iobase_t* conn;
    nai_agent_t* agent;
    nai_conn_sel_f sel;
    nai_int_t next;
    nai_int_t proto;
    uint16_t port;
    uint16_t found:1;
    uint16_t async:1;
} nai_agent_dns_query_t;


static nai_int_t nai_agent_dns_result(
    nai_dns_query_t* d, const nai_dns_result_t* rs)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t index;
    nai_int_t async;
    nai_mem_t* host;
    nai_iobase_t* c;
    nai_conn_attr_t a;
    nai_agent_t* p;
    nai_agent_query_t m;
    nai_agent_dns_query_t* q;
    nai_agent_context_t* cc;
    const nai_sockaddr_info_t* si;
    char name[128];


    /* get connect arguments */
    q = nai_containof(d, nai_agent_dns_query_t, query);
    p = q->agent;
    c = q->conn;
    async = q->async;

    /* construct addr */
    host = &q->query.name;
    m.flags = 0;
    m.found = q->found;
    m.proto = q->proto;
    m.next = q->next;
    m.port = q->port;
    m.host = *(nai_str_t*)host;


    /* check dns result */
    if (rs->error) {
        ec = rs->error;
        nai_log_error(NAI_LOG_CORE, 
            ec, "dns query host '%.*s' failed", 
            nai_str_len(host), nai_str(host));
        goto _fail;
    };

    /* get address from dns result */
    if (q->sel) {
        index = q->sel(c, rs);
    } else {
        index = -1;
    };
    m.sockaddr.len = sizeof(m.sockaddr.storage);
    r = nai_dns_result_sockaddr(
        rs, index, -1, &m.sockaddr.addr, &m.sockaddr.len);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NAI_LOG_CORE, 
            ec, "get dns result of host '%.*s' failed", 
            nai_str_len(host), nai_str(host));
        goto _fail;
    };

    /* set address port */
    si = nai_sockaddr_info(m.sockaddr.addr.sa_family);
    if (si != 0) {
        nai_sockaddr_set_port(si, &m.sockaddr.addr, m.port);
    };

    /* convert to text form for find rule */
    r = nai_sockaddr_ntop(
        &m.sockaddr.addr, m.sockaddr.len, name, sizeof(name), 0);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NAI_LOG_CORE, 
            ec, "get text address of host '%.*s' failed", 
            nai_str_len(host), nai_str(host));
        goto _fail;
    };

    m.valid_addr = 1;
    m.valid_sockaddr = 1;
    m.temp_addr = 1;
    nai_str_setm(&m.addr, name, r);


    /* make attr */
    a.agent = p;
    a.dns = d->dns;
    a.sel = q->sel;

    /* free memory of  query context  */
    cc = (nai_agent_context_t*)c->agent;
    cc->ctx = 0;
    nai_pool_free_last(cc->pool, q, sizeof(*q));
    q = 0;


    /* find rule again */
    if (m.found == 0 && p) {
        r = nai_agent_find_rule(p, &m);
        if (r < 0) {
            ec = ENOENT;
            nai_log_warn(NAI_LOG_CORE, 
                ec, "host '%.*s' not match rules", 
                nai_str_len(host), nai_str(host));

            goto _fail;
        };
    };

    /* do connect */
    r = nai_agent_do_connect(&a, c, &m);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NAI_LOG_CORE, 
            ec, "agent start connect host %.*s failed", 
            nai_str_len(host), nai_str(host));

        /* reconstruct pseudo connection for user close it in callback */
        goto _fail;
    };


_end:
    return (nai_int_t)r;

_fail:
    if (q != 0) {
        cc = (nai_agent_context_t*)c->agent;
        nai_pool_free_last(cc->pool, q, sizeof(*q));
    };
    if (async) {
        r = nai_agent_failed(c, ec);
    } else {
        nai_errno = ec;
        r = -1;
    };
    goto _end;
};


static nai_int_t nai_agent_dns_query(
    nai_conn_attr_t* a, nai_iobase_t* c, nai_agent_query_t* m)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t mt;
    nai_str_t name;
    nai_str_t* host;
    nai_dns_t* dns;
    nai_dns_result_t rs;
    nai_evloop_t* loop;
    nai_agent_t* p;
    nai_agent_dns_query_t* q;
    nai_agent_context_t* cc;


    p = a->agent;
    dns = a->dns;

    if (p != 0) {
        loop = p->loop;
    } else {
        loop = 0;
    };

    /* construct pseudo connection 
     * to make the connection look like it is connecting
     */
    r = nai_agent_pseudo_open(
        c, loop, NAI_AGENT_WARP_DNS, m->proto);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "open pseudo connection failed");
        goto _end;
    };

    mt = (nai_int_t)r;
    cc = (nai_agent_context_t*)c->agent;
    cc->ctx  = 0;


    /* if host is temporary, we need dup on async mode */
    name = m->host;
    if (m->temp_host) {
        r = nai_str_dup(&name, nai_str(&name), nai_str_len(&name), cc->pool);
        if (r < 0) {
            ec = nai_errno;
            nai_log_alert(NAI_LOG_CORE, 
                ec, "duplicate host '%.*s' failed", 
                nai_str_len(&name), nai_str(&name));
            goto _fail;
        };
    };

    /* allocate memory for dns query */
    q = (nai_agent_dns_query_t*)nai_palloc(cc->pool, sizeof(*q));
    if (q == 0) {
        ec = nai_errno;
        nai_log_alert(NAI_LOG_CORE, 
            ec, "allocate dns query context failed");
        goto _fail;
    };

    host = &name;
    q->agent = p;
    q->sel = a->sel;
    q->conn = c;
    q->next = m->next;
    q->port = m->port;
    q->proto = m->proto;
    q->found = m->found;
    q->async = 1;
    nai_dns_query_init(&q->query);
    nai_dns_query_set_cb(&q->query, nai_agent_dns_result);
    nai_dns_query_set_name(&q->query, (nai_mem_t*)host);
    cc->ctx = q;


    /* start dns query */
    r = nai_dns_query_submit(&q->query, dns, &rs);
    if (r < 0) {
        ec = nai_errno;
        if (ec != NAI_EINPROGRESS) {
            nai_pool_free_last(cc->pool, q, sizeof(*q));
            nai_log_error(NAI_LOG_CORE, 
                ec, "start dns query host '%.*s' failed", 
                nai_str_len(&name), nai_str(&name));
            goto _fail;
        };

        r = mt;
        goto _end;
    };

    /* complete */
    q->async = 0;
    r = nai_agent_dns_result(&q->query, &rs);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };

_end:
    return (nai_int_t)r;

_fail:
    if (mt) {
        /* we can't close the event node in other thread, 
         * save error code and post event to the event loop thread.
         */
        nai_agent_failed(c, ec);
        r = 1;
    } else {
        /* cleanup */
        nai_iobase_close(c);
        nai_errno = ec;
        r = -1;
    };

    goto _end;
};



//////////////////////////////////////////////////////////////////////////////
// default: direct connect


static nai_int_t nai_agent_direct_open(
    nai_agent_node_t* n, nai_agent_t* p, const nai_uri_t* attr)
{
    (void)attr;

    n->ctx = p;
    return 0;
};


static nai_int_t nai_agent_direct_close(nai_agent_node_t* n)
{
    n->ctx = 0;
    return 0;
};


static nai_int_t nai_agent_direct_connect(
    nai_agent_attr_t* a, nai_iobase_t* c, 
    nai_agent_dest_t* dest)
{
    nai_int_t r;
    nai_socknbuf_in_t nbuf;
    nai_evloop_t* loop;
    nai_agent_node_t* n = a->node;
    nai_agent_t* p = (nai_agent_t*)n->ctx;


    nai_agent_context_free(c);

    r = nai_agent_make_sockaddr(&nbuf, 
        dest->type, dest->addr.ptr, dest->port);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "nai_agent_make_sockaddr failed");
        goto _end;
    };

    if (p != 0) {
        loop = p->loop;
    } else {
        loop = 0;
    };

    switch (dest->proto) {
    case NAI_IO_TYPE_STREAM:
        r = nai_stream_connect(
            c, loop, &nbuf.addr, nbuf.len);
        break;
    case NAI_IO_TYPE_DGRAM:
        r = nai_dgram_connect(
            c, loop, &nbuf.addr, nbuf.len);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "failed to start direct connection");
    };

_end:
    return r;
};


static nai_agent_ops_t nai_agent_direct = {
    "direct", 
    (1 << NAI_IO_TYPE_STREAM) | (1 << NAI_IO_TYPE_DGRAM), 
    NAI_AGENT_REQUIRE_SOCKADDR, 
    nai_agent_direct_open, 
    nai_agent_direct_close, 
    nai_agent_direct_connect, 
};


static nai_agent_node_t nai_agent_default = {
    nai_strconst("default"), 
    &nai_agent_direct, 
    0, 
};


