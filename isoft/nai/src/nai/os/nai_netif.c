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
/// @file       nai_netif.c
/// @brief      
/// @details
/// @date       2023-10-09
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_netif.h"
#include "nai/os/nai_tlocal.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



//////////////////////////////////////////////////////////////////////////////
// network interface


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


intptr_t nai_if_list(uint32_t index, nai_int_t opt, void* buf, size_t buflen)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t af;
    nai_int_t count;
    nai_int_t output;
    nai_int_t flags;
    intptr_t esize;
    intptr_t nsize;
    intptr_t msize;
    intptr_t total;
    uint32_t ifindex;
    uint16_t family;
    uint16_t prefix;
    char* name;
    char* show;
    uint8_t* mem;
    nai_if_entry_t* out;
    nai_sockaddr_t* in;
    nai_sockaddr_in4_t* in4;
    nai_sockaddr_in6_t* in6;
    nai_tlocal_t* t;
    ULONG size;
    ULONG alloc;
    PIP_ADAPTER_ADDRESSES list;
    PIP_ADAPTER_ADDRESSES cur;
    PIP_ADAPTER_UNICAST_ADDRESS_LH addr;
    IP_ADAPTER_UNICAST_ADDRESS_LH phys;
    GetAdaptersAddresses_f GetAdaptersAddresses;



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


    t = nai_tlocal_get();
    if (t == 0) {
        r = -1;
        goto _end;
    };

    flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | 
        GAA_FLAG_SKIP_DNS_SERVER;

    af = opt >> 16;
    if (opt & NAI_IF_OPT_NO_PROTO) {
        af = AF_UNSPEC;
        flags |= GAA_FLAG_SKIP_UNICAST;
    };

    alloc = 1024;
    for (;;) {

        list = (PIP_ADAPTER_ADDRESSES)nai_tlocal_mem(t, alloc, 1);
        if (list == 0) {
            r = -1;
            goto _end;
        };

        size = alloc;
        ec = GetAdaptersAddresses(af, flags, 0, list, &size);
        if (ec != ERROR_BUFFER_OVERFLOW) {
            break;
        };

        nai_tlocal_mem_unlock(t, list);
        list = 0;
        alloc = size;
    };

    if (ec != ERROR_SUCCESS) {
        nai_errno = nai_errno_from_oserr(ec);
        r = -1;
        goto _fail;
    };


    /* scan interfaces */
    out = 0;
    output = 0;
    for (;;) {
        count = 0;
        msize = sizeof(*out);

        cur = list;
        for ( ; cur; cur = cur->Next) {
            /* test status */
            if (cur->OperStatus == IfOperStatusUp) {
                if (opt & NAI_IF_OPT_NO_UP) {
                    continue;
                };
            } else {
                if (opt & NAI_IF_OPT_NO_DOWN) {
                    continue;
                };
            };

            if (!(opt & NAI_IF_OPT_NO_NAME)) {
                /* get name sizes */
                nsize = nai_strlen(cur->AdapterName) + 1;
                r = nai_wcs_to_utf8(0, 0, cur->FriendlyName, -1);
                if (r < 0) {
                    r = -1;
                    goto _fail;
                };
                nsize += r;

                /* output names */
                if (out) {
                    name = (char*)mem;
                    nai_memcpy(name, cur->AdapterName, nsize - r);
                    show = (char*)mem + nsize - r;
                    nai_wcs_to_utf8(show, r, cur->FriendlyName, -1);
                    mem += nai_alignup(nsize, sizeof(int32_t));
                };
            };

            /* scan addresses */
            phys.Address.lpSockaddr = 0;
            phys.Next = cur->FirstUnicastAddress;
            addr = &phys;
            for ( ; addr; addr = addr->Next) {
                /* test type */
                in = addr->Address.lpSockaddr;
                if (in) {
                    family = in->sa_family;
                    if (opt & NAI_IF_OPT_NO_PROTO) {
                        continue;
                    };
                    if (af != AF_UNSPEC && af != (nai_int_t)family) {
                        continue;
                    };

                    switch (family) {
                    case AF_INET:
                        ifindex = cur->IfIndex;
                        break;
#if (NAI_HAVE_SOCKADDR_IN6)
                    case AF_INET6:
                        ifindex = cur->Ipv6IfIndex;
                        break;
#endif
                    default:
                        /* skip unknown protocol */
                        continue;
                    };

                } else {
                    family = AF_UNSPEC;
                    if (opt & NAI_IF_OPT_NO_PHYS) {
                        continue;
                    };

                    ifindex = cur->IfIndex;
                };

                /* test index */
                if (index && index != ifindex) {
                    continue;
                };

                /* count space */
                esize = sizeof(nai_if_entry_t);
                if (!(opt & NAI_IF_OPT_NO_NAME)) {
                    esize += nai_alignup(nsize, sizeof(int32_t));
                    nsize = 0;
                };
                if (!(opt & NAI_IF_OPT_NO_ADDR)) {
                    switch (family) {
                    case AF_INET:
                        esize += sizeof(nai_sockaddr_in4_t)*2;
                        break;
#if (NAI_HAVE_SOCKADDR_IN6)
                    case AF_INET6:
                        esize += sizeof(nai_sockaddr_in6_t)*2;
                        break;
#endif
                    default:
                        break;
                    };
                };


                /* output */
                if (out) {
                    out->type = (family == AF_UNSPEC) ? 
                        NAI_IF_TYPE_PHYS : NAI_IF_TYPE_PROTO;
                    out->stat = (cur->OperStatus == IfOperStatusUp) ? 
                        NAI_IF_STAT_UP : NAI_IF_STAT_DOWN;

                    if (!(opt & NAI_IF_OPT_NO_INDEX)) {
                        out->index = ifindex;
                    } else {
                        out->index = 0;
                    };

                    /* set names */
                    if (!(opt & NAI_IF_OPT_NO_NAME)) {
                        out->name = name;
                        out->show = show;
                    } else {
                        out->name = 0;
                        out->show = 0;
                    };

                    /* copy addresses */
                    switch (family) {
                    case AF_INET:
                        if (!(opt & NAI_IF_OPT_NO_ADDR)) {
                            in4 = (nai_sockaddr_in4_t*)mem;
                            mem += sizeof(*in4);
                            nai_sockname_set(&out->addr, 
                                (nai_sockaddr_t*)in4, sizeof(*in4));
                            nai_memcpy(in4, in, sizeof(*in4));

                            in4 = (nai_sockaddr_in4_t*)mem;
                            mem += sizeof(*in4);
                            nai_sockname_set(&out->mask, 
                                (nai_sockaddr_t*)in4, sizeof(*in4));

                            prefix = addr->OnLinkPrefixLength;
                            nai_memcpy(in4, in, sizeof(*in4));
                            in4->sin_addr.s_addr = 0;
                            in4->sin_addr.s_addr = (prefix > 0) ?
                                nai_htonl(0xffffffff << (32 - prefix)) : 0;
                        } else {
                            nai_sockname_init(&out->addr);
                            nai_sockname_init(&out->mask);
                        };
                        break;

#if (NAI_HAVE_SOCKADDR_IN6)
                    case AF_INET6:
                        if (!(opt & NAI_IF_OPT_NO_ADDR)) {
                            in6 = (nai_sockaddr_in6_t*)mem;
                            mem += sizeof(*in6);
                            nai_sockname_set(&out->addr, 
                                (nai_sockaddr_t*)in6, sizeof(*in6));
                            nai_memcpy(in6, in, sizeof(*in6));

                            in6 = (nai_sockaddr_in6_t*)mem;
                            mem += sizeof(*in6);
                            nai_sockname_set(&out->mask, 
                                (nai_sockaddr_t*)in6, sizeof(*in6));

                            prefix = addr->OnLinkPrefixLength;
                            nai_memcpy(in6, in, sizeof(*in6));
                            nai_memset(in6->sin6_addr.s6_addr, 0, 
                                sizeof(nai_addr_in6_t));
                            nai_memset(in6->sin6_addr.s6_addr, 
                                0xff, prefix >> 3);
                            if (prefix % 8) {
                                in6->sin6_addr.s6_addr[prefix >> 3] =
                                  0xff << (8 - prefix % 8);
                            };
                        } else {
                            nai_sockname_init(&out->addr);
                            nai_sockname_init(&out->mask);
                        };
                        break;
#endif

                    default:
                        if (!(opt & NAI_IF_OPT_NO_MTU)) {
                            out->mtu = cur->Mtu;
                            if (out->mtu > 0x10000) {
                                out->mtu = 0x10000;
                            };
                        } else {
                            out->mtu = 0;
                        };
                        if (!(opt & NAI_IF_OPT_NO_ADDR) && 
                            cur->PhysicalAddressLength == sizeof(out->phys)) {
                            nai_memcpy(out->phys, 
                                cur->PhysicalAddress, sizeof(out->phys));
                        } else {
                            nai_memset(out->phys, 0, sizeof(out->phys));
                        };
                        break;
                    };

                    out ++;
                    output --;
                    if (output <= 0) {
                        break;
                    };
                } else {
                    if (msize < (intptr_t)buflen && 
                        msize + esize > (intptr_t)buflen) {
                        output = count;
                    };
                };

                count ++;
                msize += esize;
            };

            if (out && output <= 0) {
                break;
            };
        };

        if (out) {
            break;
        };

        if (output == 0 && 
            msize <= (intptr_t)buflen) {
            output = count;
        };

        total = msize;
        out = (nai_if_entry_t*)buf;
        mem = (uint8_t*)(out + (output + 1));
        if (output <= 0) {
            break;
        };
    };

    /* output the end entry */
    if (buflen >= sizeof(*out)) {
        out->index = 0;
        out->name = 0;
        out->show = 0;
        out->type = 0;
        out->stat = 0;
        nai_sockname_init(&out->addr);
        nai_sockname_init(&out->mask);
    };

    r = total;

_fail:
    if (list) {
        if (r < 0) {
            ec = nai_errno;
        };

        nai_tlocal_mem_unlock(t, list);

        if (r < 0) {
            nai_errno = ec;
        };
    };

_end:
    return r;
};



#elif (NAI_HAVE_IFADDRS_H)


#include <ifaddrs.h>


#if (NAI_HAVE_NET_IF_H)
#include <net/if.h>
#endif

#if (NAI_HAVE_SYS_IOCTL_H)
#include <sys/ioctl.h>
#endif

#if defined(AF_PACKET)
#define NAI_AF_LINK             AF_PACKET
#if (__linux__)
#include <linux/if_packet.h>
#elif (__sylixos__)
#include <netpacket/packet.h>
#endif
#elif defined(AF_LINK)
#define NAI_AF_LINK             AF_LINK
#if (NAI_HAVE_NET_IF_DL_H)
#include <net/if_dl.h>
#endif
#endif

#if !defined(IFF_RUNNING)
#define IFF_RUNNING             IFF_LINK
#endif



#if defined(SIOCGIFMTU)


typedef struct nai_if_req_s {
    nai_fd_t fd;
} nai_if_req_t;


static nai_int_t nai_if_req_mtu(nai_if_req_t* req, struct ifaddrs* ifa)
{
    nai_int_t r;
    struct ifreq ifr;


    if (req->fd < 0) {
        req->fd = nai_sock_open(AF_INET, SOCK_DGRAM, 0);
        if (req->fd < 0) {
            r = -1;
            goto _end;
        };
    };

    nai_strcpy(ifr.ifr_name, ifa->ifa_name);   // eth0
    r = ioctl(req->fd, SIOCGIFMTU, &ifr);
    if (r < 0) {
        goto _end;
    };

    r = ifr.ifr_ifru.ifru_mtu;

_end:
    return r;
};


static nai_int_t nai_if_req_release(nai_if_req_t* req)
{
    nai_int_t r;

    if (req->fd >= 0) {
        nai_sock_close(req->fd);
    };

    r = 0;

    return r;
};


#else

#error "the platform is not supported get mtu!"

#endif



static nai_int_t nai_if_copy_phys(
    nai_if_req_t* req, nai_if_entry_t* e, struct ifaddrs* ifa, nai_int_t opt)
{
    nai_int_t r;
#if defined(AF_PACKET)
    struct sockaddr_ll* ll;
#elif defined(AF_LINK) && (NAI_HAVE_NET_IF_DL_H)
    struct sockaddr_dl* dl;
#endif


    if (!(opt & NAI_IF_OPT_NO_MTU)) {
        r = nai_if_req_mtu(req, ifa);
        if (r < 0) {
            goto _end;
        };
        e->mtu = r;
    } else {
        e->mtu = 0;
    };

    if (!(opt & NAI_IF_OPT_NO_ADDR) && ifa->ifa_addr) {
#if defined(AF_PACKET)
        ll = (struct sockaddr_ll*)ifa->ifa_addr;
        nai_memcpy(e->phys, ll->sll_addr, sizeof(e->phys));
#elif defined(AF_LINK) && (NAI_HAVE_NET_IF_DL_H)
        dl = (struct sockaddr_dl*)ifa->ifa_addr;
        nai_memcpy(e->phys, LLADDR(dl), sizeof(e->phys));
#endif
    } else {
        nai_memset(e->phys, 0, sizeof(e->phys));
    };

    r = 0;

_end:
    return r;
};


intptr_t nai_if_list(uint32_t index, nai_int_t opt, void* buf, size_t buflen)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t af;
    nai_int_t count;
    nai_int_t output;
    intptr_t esize;
    intptr_t nsize;
    intptr_t msize;
    intptr_t total;
    uint32_t ifindex;
    uint16_t family;
    uint32_t flags;
    struct ifaddrs *ifa;
    struct ifaddrs *list;
    nai_if_entry_t* out;
    nai_sockaddr_in4_t* in4;
#if (NAI_HAVE_SOCKADDR_IN6)
    nai_sockaddr_in6_t* in6;
#endif
    nai_if_req_t req = { -1 };
    uint8_t* mem;


    r = getifaddrs(&list);
    if (r < 0) {
        goto _end;
    };

    af = opt >> 16;

    /* to disable warning */
    mem = 0;
    total = 0;
    nsize = 0;

    /* scan interface */
    out = 0;
    output = 0;
    for (;;) {
        count = 0;
        msize = sizeof(*out);

        ifa = list;
        for ( ; ifa; ifa = ifa->ifa_next) {
            /* test status */
            flags = ifa->ifa_flags;
            if ((flags & IFF_UP) && (flags & IFF_RUNNING)) {
                if (opt & NAI_IF_OPT_NO_UP) {
                    continue;
                };
            } else {
                if (opt & NAI_IF_OPT_NO_DOWN) {
                    continue;
                };
            };

            /* test type */
            if (ifa->ifa_addr) {
                family = ifa->ifa_addr->sa_family;
                if (family == NAI_AF_LINK) {
                    if (opt & NAI_IF_OPT_NO_PHYS) {
                        continue;
                    };
                } else {
                    if (opt & NAI_IF_OPT_NO_PROTO) {
                        continue;
                    };
                    if (af != AF_UNSPEC && af != (nai_int_t)family) {
                        continue;
                    };

                    switch (family) {
                    case AF_INET:
#if (NAI_HAVE_SOCKADDR_IN6)
                    case AF_INET6:
                        break;
#endif
                    default:
                        /* skip unknown protocol */
                        continue;
                    };
                };

            } else {
                family = NAI_AF_LINK;
                if (opt & NAI_IF_OPT_NO_PHYS) {
                    continue;
                };
            };

            /* test index */
            if (index || (out && !(opt & NAI_IF_OPT_NO_INDEX))) {
                r = nai_if_nametoindex(ifa->ifa_name, -1, &ifindex);
                if (r < 0) {
                    goto _fail;
                };
                if (index && index != ifindex) {
                    continue;
                };
            };

            /* count space */
            esize = sizeof(nai_if_entry_t);
            if (!(opt & NAI_IF_OPT_NO_NAME)) {
                nsize = nai_strlen(ifa->ifa_name) + 1;
                esize += nai_alignup(nsize, sizeof(int32_t));
            };
            if (!(opt & NAI_IF_OPT_NO_ADDR)) {
                switch (family) {
                case AF_INET:
                    esize += sizeof(nai_sockaddr_in4_t)*2;
                    break;
#if (NAI_HAVE_SOCKADDR_IN6)
                case AF_INET6:
                    esize += sizeof(nai_sockaddr_in6_t)*2;
                    break;
#endif
                default:
                    break;
                };
            };

            /* output */
            if (out) {
                out->type = (family == NAI_AF_LINK) ? 
                    NAI_IF_TYPE_PHYS : NAI_IF_TYPE_PROTO;
                out->stat = ((flags & IFF_UP) && (flags & IFF_RUNNING)) ? 
                    NAI_IF_STAT_UP : NAI_IF_STAT_DOWN;

                /* copy index */
                if (!(opt & NAI_IF_OPT_NO_INDEX)) {
                    out->index = ifindex;
                } else {
                    out->index = 0;
                };

                /* copy name */
                if (!(opt & NAI_IF_OPT_NO_NAME)) {
                    out->name = (char*)mem;
                    out->show = (char*)mem;
                    nai_memcpy(mem, ifa->ifa_name, nsize);
                    mem += nai_alignup(nsize, sizeof(int32_t));
                } else {
                    out->name = 0;
                    out->show = 0;
                };

                /* copy addresses */
                switch (family) {
                case AF_INET:
                    if (!(opt & NAI_IF_OPT_NO_ADDR)) {
                        in4 = (nai_sockaddr_in4_t*)mem;
                        mem += sizeof(*in4);
                        nai_sockname_set(&out->addr, 
                            (nai_sockaddr_t*)in4, sizeof(*in4));
                        nai_memcpy(in4, ifa->ifa_addr, sizeof(*in4));

                        in4 = (nai_sockaddr_in4_t*)mem;
                        mem += sizeof(*in4);
                        nai_sockname_set(&out->mask, 
                            (nai_sockaddr_t*)in4, sizeof(*in4));
                        if (ifa->ifa_netmask) {
                            nai_memcpy(in4, ifa->ifa_netmask, sizeof(*in4));
                        } else {
                            nai_memcpy(in4, ifa->ifa_addr, sizeof(*in4));
                            in4->sin_addr.s_addr = 0;
                        };
                    } else {
                        nai_sockname_init(&out->addr);
                        nai_sockname_init(&out->mask);
                    };
                    break;

#if (NAI_HAVE_SOCKADDR_IN6)
                case AF_INET6:
                    if (!(opt & NAI_IF_OPT_NO_ADDR)) {
                        in6 = (nai_sockaddr_in6_t*)mem;
                        mem += sizeof(*in6);
                        nai_sockname_set(&out->addr, 
                            (nai_sockaddr_t*)in6, sizeof(*in6));
                        nai_memcpy(in6, ifa->ifa_addr, sizeof(*in6));

                        in6 = (nai_sockaddr_in6_t*)mem;
                        mem += sizeof(*in6);
                        nai_sockname_set(&out->mask, 
                            (nai_sockaddr_t*)in6, sizeof(*in6));
                        if (ifa->ifa_netmask) {
                            nai_memcpy(in6, ifa->ifa_netmask, sizeof(*in6));
                        } else {
                            nai_memcpy(in6, ifa->ifa_addr, sizeof(*in6));
                            nai_memset(&in6->sin6_addr, 
                                0, sizeof(in6->sin6_addr));
                        };
                    } else {
                        nai_sockname_init(&out->addr);
                        nai_sockname_init(&out->mask);
                    };
                    break;
#endif

                default:
                    r = nai_if_copy_phys(&req, out, ifa, opt);
                    if (r < 0) {
                        goto _fail;
                    };
                    break;
                };

                out ++;
                output --;
                if (output <= 0) {
                    break;
                };
            } else {
                if (msize < (intptr_t)buflen && 
                    msize + esize > (intptr_t)buflen) {
                    output = count;
                };
            };

            count ++;
            msize += esize;
        };

        if (out) {
            break;
        };

        if (output == 0 && 
            msize <= (intptr_t)buflen) {
            output = count;
        };

        total = msize;
        out = (nai_if_entry_t*)buf;
        mem = (uint8_t*)(out + (output + 1));
        if (output <= 0) {
            break;
        };
    };

    /* output the end entry */
    if (buflen >= sizeof(*out)) {
        out->index = 0;
        out->name = 0;
        out->show = 0;
        out->type = 0;
        out->stat = 0;
        nai_sockname_init(&out->addr);
        nai_sockname_init(&out->mask);
    };

    nai_if_req_release(&req);
    freeifaddrs(list);
    r = total;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_if_req_release(&req);
    freeifaddrs(list);
    nai_errno = ec;
    r = -1;
    goto _end;
};



#else


#error "the platform is not supported get interfaces!"


#endif


nai_int_t nai_if_get_mtu(uint32_t index)
{
    intptr_t r;
    nai_int_t opt;
    nai_if_entry_t e[2];


    if (index == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    opt = NAI_IF_OPT_NO_PROTO;
    opt |= NAI_IF_OPT_NO_NAME;
    opt |= NAI_IF_OPT_NO_ADDR;
    opt |= NAI_IF_OPT_NO_INDEX;
    r = nai_if_list(index, opt, e, sizeof(e));
    if (r < 0) {
        goto _end;
    };

    assert((size_t)r <= sizeof(e));
    if (nai_if_is_valid(e)) {
        r = e[0].mtu;
    } else {
        nai_errno = ENODEV;
        r = -1;
    };

_end:
    return (nai_int_t)r;
};


static nai_if_entry_t* nai_if_find_addr(
    const nai_sockaddr_t* name, nai_int_t namelen, nai_int_t opt, 
    nai_tlocal_t* t, void** pbuf)
{
    intptr_t r;
    nai_int_t ec;
    intptr_t len;
    char* buf;
    nai_if_entry_t* ent;
    nai_sockaddr_t* addr;


    opt |= NAI_IF_OPT_PROTO(name->sa_family);
    buf = 0;
    len = 1024;
    for (;;) {
        buf = nai_tlocal_mem(t, len, 1);
        if (buf == 0) {
            ent = 0;
            goto _end;
        };

        r = nai_if_list(0, opt, buf, len);
        if (r < 0) {
            ec = nai_errno;
            nai_tlocal_mem_unlock(t, buf);
            nai_errno = ec;
            ent = 0;
            goto _end;
        };
        if (r <= len) {
            break;
        };

        nai_tlocal_mem_unlock(t, buf);
        len = r;
    };

    ent = (nai_if_entry_t*)buf;
    for ( ; nai_if_is_valid(ent); ent ++) {
        addr = ent->addr.addr;
        if (name->sa_family != addr->sa_family) {
            continue;
        };
        if (namelen != ent->addr.len) {
            continue;
        };

        switch (name->sa_family) {
        case AF_INET:
            r = nai_memcmp(
                &((nai_sockaddr_in4_t*)name)->sin_addr, 
                &((nai_sockaddr_in4_t*)addr)->sin_addr, 
                sizeof(nai_addr_in4_t));

            break;
#if (NAI_HAVE_SOCKADDR_IN6)
        case AF_INET6:
            r = nai_memcmp(
                &((nai_sockaddr_in6_t*)name)->sin6_addr, 
                &((nai_sockaddr_in6_t*)addr)->sin6_addr, 
                sizeof(nai_addr_in6_t));
            break;
#endif
        default:
            r = nai_memcmp(&name->sa_data, &addr->sa_data, 
                namelen-nai_offsetof(nai_sockaddr_t, sa_data));
            break;
        };
        if (r == 0) {
            break;
        };
    };

    pbuf[0] = buf;

_end:
    return ent;
};


nai_int_t nai_if_addrtoindex(
    const nai_sockaddr_t* name, nai_int_t namelen, uint32_t* pindex)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t opt;
    void* mem;
    nai_tlocal_t* t;
    nai_if_entry_t* ent;


    t = nai_tlocal_get();
    if (t == 0) {
        r = -1;
        goto _end;
    };

    opt = NAI_IF_OPT_NO_NAME;
    opt |= NAI_IF_OPT_NO_MTU;
    ent = nai_if_find_addr(name, namelen, opt, t, &mem);
    if (ent == 0) {
        r = -1;
        goto _end;
    };

    if (nai_if_is_valid(ent)) {
        pindex[0] = ent->index;
        ec = 0;
        r = 0;
    } else {
        ec = ENODEV;
    };

    nai_tlocal_mem_unlock(t, mem);

    if (ec) {
        nai_errno = ec;
        r = -1;
    };

_end:
    return (nai_int_t)r;
};


intptr_t nai_if_addrtoname(
    const nai_sockaddr_t* name, nai_int_t namelen, char* buf, size_t len)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t opt;
    void* mem;
    nai_tlocal_t* t;
    nai_if_entry_t* ent;


    t = nai_tlocal_get();
    if (t == 0) {
        r = -1;
        goto _end;
    };

    opt = NAI_IF_OPT_NO_INDEX;
    opt |= NAI_IF_OPT_NO_MTU;
    ent = nai_if_find_addr(name, namelen, opt, t, &mem);
    if (ent == 0) {
        r = -1;
        goto _end;
    };

    if (nai_if_is_valid(ent)) {
        r = nai_strlen(ent->name);

        if (r <= (intptr_t)len) {
            nai_memcpy(buf, ent->name, r + (r < (intptr_t)len));
            ec = 0;
        } else {
            ec = ERANGE;
        };

    } else {
        ec = ENODEV;
    };

    nai_tlocal_mem_unlock(t, mem);

    if (ec != 0) {
        nai_errno = ec;
        r = -1;
    };

_end:
    return r;
};



#if !defined(_WIN32)


#if (NAI_HAVE_NET_IF_H)
#include <net/if.h>
#endif

#else


#if !defined(IF_NAMESIZE)
#define IF_NAMESIZE 32
#endif

#define NAI_HAVE_IF_NAMETOINDEX 1
#define NAI_HAVE_IF_INDEXTONAME 1


static uint32_t if_nametoindex_u(const char* name)
{
    uint32_t r;


    if (nai_wapi.inited == 0) {
        nai_wapi_init();
    };

    if (nai_wapi.if_nametoindex == 0) {
        nai_errno = ENOTSUP;
        r = 0;
        goto _end;
    };

    r = nai_wapi.if_nametoindex(name);
    if (r == 0) {
        nai_errno = ENODEV;
    };

_end:
    return r;
};


static char* if_indextoname_u(uint32_t n, char* buf)
{
    char* r;


    if (nai_wapi.inited == 0) {
        nai_wapi_init();
    };

    if (nai_wapi.if_indextoname == 0) {
        nai_errno = ENOTSUP;
        r = 0;
        goto _end;
    };

    r = nai_wapi.if_indextoname(n, buf);
    if (r == 0) {
        nai_errno = ENXIO;
    };

_end:
    return r;
};


#define if_nametoindex if_nametoindex_u
#define if_indextoname if_indextoname_u


#endif


extern intptr_t nai_wtodec(char* dst, intptr_t len, uint16_t i);


nai_int_t nai_if_nametoindex(
    const char* buf, size_t len, uint32_t* pindex)
{
    nai_int_t r;

#if (NAI_HAVE_IF_NAMETOINDEX)
    uint32_t n;
    char name[IF_NAMESIZE];

    if ((intptr_t)len < 0) {
        n = if_nametoindex(buf);
    } else if (len+1 > sizeof(name)) {
        /* too large */
        n = 0;
    } else {

        nai_memcpy(name, buf, len);
        name[len] = 0;

        n = if_nametoindex(name);
    };
    if (n != 0) {
        /* success */
        pindex[0] = n;
        r = 0;
        goto _end;
    };
#endif

    r = nai_atoui(pindex, buf, len, 0);

#if (NAI_HAVE_IF_NAMETOINDEX)
_end:
#endif
    return r;
};


intptr_t nai_if_indextoname(
    uint32_t index, char* buf, size_t len, nai_int_t name_only)
{
    intptr_t r;

#if (NAI_HAVE_IF_INDEXTONAME)
    char* p;
    char* d;
    char name[IF_NAMESIZE];

    p = buf;
    if ((intptr_t)len < IF_NAMESIZE) {
        p = name;
    };

    d = if_indextoname(index, p);
    if (d == p) {
        r = nai_strlen(p);

        if (r <= (intptr_t)len) {
            nai_memcpy(buf, p, r + (r < (intptr_t)len));
        } else {
            nai_errno = ERANGE;
        };

        goto _end;
    };
    if (name_only) {
        r = -1;
        goto _end;
    };
#else
    if (name_only) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };
#endif

    if (index < 0x10000) {
        r = nai_wtodec(buf, 
            (intptr_t)len > 0 ? len : 0, (uint16_t)index);
    } else {
        r = nai_snprintf(buf, 
            (intptr_t)len > 0 ? len : 0, "%u", index);
    };

_end:
    return r;
};



