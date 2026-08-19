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
/// @file       nai_dns_init.c
/// @brief      
/// @details
/// @date       2021-08-09
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_dns.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"



#define NAI_DNS_READ_ALL        0


typedef nai_int_t (*nai_dns_parse_f)(
    void* ud, const char* s, const char* send);


static nai_int_t nai_dns_parse_file(
    const char* path, nai_dns_parse_f fn, void* ud)
{
    nai_int_t ec;
    intptr_t r;
    intptr_t len;
    intptr_t total;
    char* buf;
    char* s;
    char* send;
#if (NAI_DNS_READ_ALL)
    nai_fd_t fd;
#else
    FILE* fp;
#endif

#if (NAI_DNS_READ_ALL)
    fd = nai_file_open(path, NAI_O_RDONLY, 0);
    if (fd == NAI_FD_INVALID) {
        ec = nai_errno;
        r = ec != ENOENT ? -1 : 0;
        goto _end;
    };

    total = nai_file_get_size(fd);
    if (total < 0) {
        r = -1;
        goto _end;
    };

    buf = nai_malloc(total);
    if (!buf) {
        r = -1;
        goto _fail;
    };

    r = nai_file_read(fd, buf, total);
    if (r < 0) {
        goto _fail;
    };

    len = r;
    s = buf;
    send = buf;
    for ( ; len > 0; s = send) {
        send = nai_strnchr(s, len, '\n');
        if (send == 0) {
            send = s + len;
        } else {
            send ++;
        };
        len -= (send - s);

        r = fn(ud, s, send);
        if (r < 0) {
            goto _fail;
        };
    };

#else

    fp = fopen(path, "r");
    if (fp == 0) {
        ec = nai_errno;
        r = ec != ENOENT ? -1 : 0;
        goto _end;
    };

    total = 128;
    len = 0;
    buf = nai_malloc(total);
    if (buf == 0) {
        r = -1;
        goto _fail;
    };

    for (;;) {

        if (!fgets(buf + len, (nai_int_t)(total - len), fp)) {
            if (len <= 0) {
                break;
            };
        } else {
            len += nai_strlen(buf + len);
            if (len > 0 && buf[len - 1] != '\n') {
                total *= 2;
                s = nai_malloc(total);
                if (s == 0) {
                    r = -1;
                    goto _end;
                };

                nai_memcpy(s, buf, len+1);
                nai_free(buf);
                buf = s;
                continue;
            };
        };

        s = buf;
        send = s + len;
        len = 0;

        r = fn(ud, s, send);
        if (r < 0) {
            goto _fail;
        };
    };
#endif

    r = 0;

_fail:
    if (r < 0) {
        ec = nai_errno;
    };

    if (buf) {
        nai_free(buf);
    };

#if (NAI_DNS_READ_ALL)
    if (fd != NAI_FD_INVALID) {
        nai_file_close(fd);
    };
#else
    if (fp) {
        fclose(fp);
    };
#endif

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return (nai_int_t)r;
};


static nai_int_t nai_dns_parse_host(void* ud, const char* s, const char* send)
{
    nai_int_t r;
    nai_int_t ec;
    nai_dns_t* d = (nai_dns_t*)ud;
    nai_mem_t addr;
    nai_mem_t host;


    /* skip spaces */
    while (s < send && nai_isspace(*s)) {
        s ++;
    };

    /* get address */
    nai_str_setm(&addr, s, 0);
    while (s < send && (!nai_isspace(*s) && *s != '#')) {
        s ++;
    };
    nai_str_setl(&addr, s - addr.ptr);

    /* is empry addr */
    if (nai_str_len(&addr) <= 0) {
        r = 0;
        goto _end;
    };

    for (;;) {
        /* skip spaces */
        while (s < send && nai_isspace(*s)) {
            s ++;
        };

        /* get host */
        nai_str_setm(&host, s, 0);
        while (s < send && (!nai_isspace(*s) && *s != '#')) {
            s ++;
        };
        nai_str_setl(&host, s - host.ptr);

        /* is empry host */
        if (nai_str_len(&host) <= 0) {
            break;
        };

        nai_log_debug(NAI_LOG_CORE, 
            0, "get host %.*s %.*s", 
            nai_str_len(&addr), nai_str(&addr), 
            nai_str_len(&host), nai_str(&host));

        r = nai_dns_add_static_addr(d, &host, &addr);
        if (r < 0) {
            ec = nai_errno;
            nai_log_debug(NAI_LOG_CORE, 
                ec, "nai_dns_add_static_addr failed");
            if (ec == ENOMEM) {
                goto _end;
            };
        };
    };

    r = 0;

_end:
    return r;
};


#if defined(_WIN32)


#include "nai/os/win/nai_wsock.h"
#include <iphlpapi.h>
#include <winerror.h>


typedef ULONG (WINAPI* GetAdaptersAddresses_f)(
    ULONG Family,
    ULONG Flags,
    PVOID Reserved,
    PIP_ADAPTER_ADDRESSES AdapterAddresses,
    PULONG SizePointer
);

typedef DWORD (WINAPI* GetNetworkParams_f)(
    PFIXED_INFO pFixedInfo,
    PULONG      pOutBufLen
);


static nai_int_t nai_dns_add_server_from_adapter(nai_dns_t* d)
{
    nai_int_t r;
    nai_int_t ec;
    nai_mem_t addr;
    nai_sockaddr_t* sa;
    ULONG size;
    ULONG alloc;
    PIP_ADAPTER_ADDRESSES list;
    PIP_ADAPTER_ADDRESSES cur;
    PIP_ADAPTER_DNS_SERVER_ADDRESS dns;
    GetAdaptersAddresses_f GetAdaptersAddresses;
    char name[64];


    if (nai_wapi.inited == 0) {
        nai_wapi_init();
    };
    if (nai_wapi.GetAdaptersAddresses == 0) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    GetAdaptersAddresses = 
        (GetAdaptersAddresses_f)nai_wapi.GetAdaptersAddresses;

    alloc = 1024;
    for (;;) {

        list = (PIP_ADAPTER_ADDRESSES)nai_malloc(alloc);
        if (list == 0) {
            r = -1;
            goto _end;
        };

        size = alloc;
        ec = GetAdaptersAddresses(AF_UNSPEC, 0, 0, list, &size);
        if (ec != ERROR_BUFFER_OVERFLOW) {
            break;
        };

        nai_free(list);
        list = 0;
        alloc = size;
    };

    if (ec != ERROR_SUCCESS) {
        nai_errno = nai_errno_from_oserr(ec);
        r = -1;
        goto _fail;
    };

    cur = list;
    for ( ; cur; cur = cur->Next) {
        if (cur->OperStatus != IfOperStatusUp) {
            continue;
        };

        dns = cur->FirstDnsServerAddress;
        for ( ; dns; dns = dns->Next) {
            sa = dns->Address.lpSockaddr;
            switch (sa->sa_family) {
            case AF_INET:
            case AF_INET6:
                break;
            default:
                continue;
            };

            r = (nai_int_t)nai_sockaddr_ntop((nai_sockaddr_t*)
                sa, dns->Address.iSockaddrLength, name, sizeof(name), 
                NAI_ADDR_PORT);
            if (r < 0) {
                goto _fail;
            };

            nai_str_setm(&addr, name, r);
            nai_log_debug(NAI_LOG_CORE, 
                0, "get dns server %.*s", 
                nai_str_len(&addr), nai_str(&addr));

            r = nai_dns_add_server(d, &addr, 1);
            if (r < 0) {
                ec = nai_errno;
                nai_log_debug(NAI_LOG_CORE, 
                    ec, "nai_dns_add_server failed");
                if (ec == ENOMEM) {
                    goto _fail;
                };
            };
        };
    };

    r = 0;

_fail:
    if (list) {
        if (r < 0) {
            ec = nai_errno;
        };

        nai_free(list);

        if (r < 0) {
            nai_errno = ec;
        };
    };

_end:
    return r;
};


static nai_int_t nai_dns_add_server_from_network(nai_dns_t* d)
{
    nai_int_t r;
    nai_int_t ec;
    nai_mem_t addr;
    ULONG size;
    ULONG alloc;
    FIXED_INFO* list;
    IP_ADDR_STRING* dns;
    GetNetworkParams_f GetNetworkParams;


    if (nai_wapi.inited == 0) {
        nai_wapi_init();
    };
    if (nai_wapi.GetNetworkParams == 0) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    GetNetworkParams = 
        (GetNetworkParams_f)nai_wapi.GetNetworkParams;

    alloc = sizeof(*list);
    for (;;) {

        list = (FIXED_INFO*)nai_malloc(alloc);
        if (list == 0) {
            r = -1;
            goto _end;
        };

        size = alloc;
        ec = GetNetworkParams(list, &size);
        if (ec != ERROR_BUFFER_OVERFLOW) {
            break;
        };

        nai_free(list);
        list = 0;
        alloc = size;
    };

    if (ec != ERROR_SUCCESS) {
        nai_errno = nai_errno_from_oserr(ec);
        r = -1;
        goto _fail;
    };

    dns = &list->DnsServerList;
    for ( ; dns; dns = dns->Next) {

        nai_str_sets(&addr, dns->IpAddress.String);
        nai_log_debug(NAI_LOG_CORE, 
            0, "get dns server %.*s", 
            nai_str_len(&addr), nai_str(&addr));

        r = nai_dns_add_server(d, &addr, 1);
        if (r < 0) {
            ec = nai_errno;
            nai_log_debug(NAI_LOG_CORE, 
                ec, "nai_dns_add_server failed");
            if (ec == ENOMEM) {
                goto _fail;
            };
        };
    };

    r = 0;

_fail:
    if (list) {
        if (r < 0) {
            ec = nai_errno;
        };

        nai_free(list);

        if (r < 0) {
            nai_errno = ec;
        };
    };

_end:
    return r;
};


static nai_int_t nai_dns_add_default_server(nai_dns_t* d)
{
    nai_int_t r;

    r = nai_dns_add_server_from_adapter(d);
    if (r >= 0) {
        goto _end;
    } else {
        if (nai_errno != ENOTSUP) {
            goto _end;
        };
    };

    r = nai_dns_add_server_from_network(d);
    if (r >= 0) {
        goto _end;
    } else {
        if (nai_errno != ENOTSUP) {
            goto _end;
        };
    };

_end:
    return r;
};


#elif (NAI_HAVE_RES_GETSERVERS)


#include <resolv.h>


static nai_int_t nai_dns_add_default_server(nai_dns_t* d)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t n, count;
    nai_mem_t addr;
    struct __res_state s = {0};
    union res_sockaddr_union list[MAXNS];
    char name[64];


    r = res_ninit(&s);
    if (r < 0 && !(s.options & RES_INIT)) {
        goto _end;
    };

    r = res_getservers(&s, list, nai_countof(list));
    if (r < 0) {
        goto _fail;
    };

    count = r;
    n = 0;
    for ( ; n < count; n ++) {
        r = (nai_int_t)nai_sockaddr_ntop((nai_sockaddr_t*)
            &list[n].sin, sizeof(list[n].sin), name, sizeof(name), 
            NAI_ADDR_PORT);
        if (r < 0) {
            goto _fail;
        };

        nai_str_setm(&addr, name, r);
        nai_log_debug(NAI_LOG_CORE, 
            0, "get dns server %.*s", 
            nai_str_len(&addr), nai_str(&addr));

        r = nai_dns_add_server(d, &addr, 1);
        if (r < 0) {
            ec = nai_errno;
            nai_log_debug(NAI_LOG_CORE, 
                ec, "nai_dns_add_server failed");
            if (ec == ENOMEM) {
                goto _fail;
            };
        };
    };

    r = 0;

_fail:
    if (r < 0) {
        ec = nai_errno;
    };

    res_ndestroy(&s);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


#elif (NAI_HAVE_RES_INIT) && (NAI_HAVE_RES_STATE)


#include <resolv.h>


static nai_int_t nai_dns_add_default_server(nai_dns_t* d)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t n, count;
    nai_mem_t addr;
    res_state s;
    char name[64];


    do {
        r = res_init();
    } while (r < 0 && nai_errno == EINTR);

    if (r < 0) {
        goto _end;
    };

    s = &_res;

    count = s->nscount;
    n = 0;
    for ( ; n < count; n ++) {
        if (nai_sockaddr_ntop(
            (nai_sockaddr_t*)&s->nsaddr_list[n], 
            sizeof(s->nsaddr_list[n]), name, sizeof(name), 
            NAI_ADDR_PORT) == 0) {
            r = -1;
            goto _end;
        };

        nai_str_sets(&addr, name);
        nai_log_debug(NAI_LOG_CORE, 
            0, "get dns server %.*s", 
            nai_str_len(&addr), nai_str(&addr));

        r = nai_dns_add_server(d, &addr, 1);
        if (r < 0) {
            ec = nai_errno;
            nai_log_debug(NAI_LOG_CORE, 
                ec, "nai_dns_add_server failed");
            if (ec == ENOMEM) {
                goto _end;
            };
        };
    };

    r = 0;

_end:
    return r;
};


#else


static nai_int_t nai_dns_parse_server(
    void* ud, const char* s, const char* send)
{
    nai_int_t r;
    nai_int_t ec;
    nai_dns_t* d = (nai_dns_t*)ud;
    nai_mem_t name;
    nai_mem_t addr;
    nai_str_t command = nai_strconst("nameserver");


    /* skip spaces */
    while (s < send && nai_isspace(*s)) {
        s ++;
    };

    /* get address */
    nai_str_setm(&name, s, 0);
    while (s < send && (!nai_isspace(*s) && *s != '#')) {
        s ++;
    };
    nai_str_setl(&name, s - name.ptr);

    /* is empry command */
    if (nai_str_len(&name) <= 0) {
        r = 0;
        goto _end;
    };
    if (nai_str_cmp(&name, &command) != 0) {
        r = 0;
        goto _end;
    };

    for (;;) {
        /* skip spaces */
        while (s < send && nai_isspace(*s)) {
            s ++;
        };

        /* get host */
        nai_str_setm(&addr, s, 0);
        while (s < send && (!nai_isspace(*s) && *s != '#')) {
            s ++;
        };
        nai_str_setl(&addr, s - addr.ptr);

        /* is empry host */
        if (nai_str_len(&addr) <= 0) {
            break;
        };

        nai_log_debug(NAI_LOG_CORE, 
            0, "get server %.*s", 
            nai_str_len(&addr), nai_str(&addr));

        r = nai_dns_add_server(d, &addr, 1);
        if (r < 0) {
            ec = nai_errno;
            nai_log_debug(NAI_LOG_CORE, 
                ec, "nai_dns_add_server failed");
            if (ec == ENOMEM) {
                goto _end;
            };
        };
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_dns_add_default_server(nai_dns_t* d)
{
    nai_int_t r;
    const char* path = "/etc/resolv.conf";

    r = nai_dns_parse_file(path, nai_dns_parse_server, d);

    return r;
};


#endif


static nai_int_t nai_dns_add_default_host(nai_dns_t* d)
{
    nai_int_t r;


#if defined(_WIN32)
    char path[MAX_PATH];

    r = ExpandEnvironmentStringsA(
        "%SystemRoot%\\System32\\drivers\\etc", path, nai_countof(path));
    if (r == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    strcat(path, "\\hosts");

#else
    const char* path = "/etc/hosts";
#endif

    r = nai_dns_parse_file(path, nai_dns_parse_host, d);

#if defined(_WIN32)
_end:
#endif
    return r;
};


nai_int_t nai_dns_add_default(nai_dns_t* d, nai_int_t flags)
{
    nai_int_t r;


    if (!(flags & NAI_DNS_NO_SERVER)) {
        r = nai_dns_add_default_server(d);
        if (r < 0) {
            goto _end;
        };
    };

    if (!(flags & NAI_DNS_NO_HOST)) {
        r = nai_dns_add_default_host(d);
        if (r < 0) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


