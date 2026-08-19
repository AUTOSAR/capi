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
/// @file       nai_ssl_ktls.c
/// @brief      
/// @details
/// @date       2022-10-22
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_ssl_ctx.h"


#if (NAI_HAVE_KTLS)


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"


typedef union nai_ktls_cmsg_u {
    struct cmsghdr hdr;
    uint8_t buf[CMSG_SPACE(sizeof(uint8_t))];
} nai_ktls_cmsg_t;



#if (__linux__) && defined(SOL_TLS)



#include <linux/tls.h>
#include <linux/version.h>


#define NAI_KTLS_AES_GCM_128

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0)
#define NAI_KTLS_AES_GCM_256
#define NAI_KTLS_TLS13
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 2, 0)
#define NAI_KTLS_AES_CCM_128
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 11, 0)
#if !defined(OPENSSL_NO_CHACHA)
#define NAI_KTLS_CHACHA20_POLY1305
#endif
#endif


#ifndef TLS_RX
#define TLS_RX                  2
#endif


typedef struct nai_ktls_crypto_info_s {

    union {
#ifdef NAI_KTLS_AES_GCM_128
        struct tls12_crypto_info_aes_gcm_128 gcm128;
#endif
#ifdef NAI_KTLS_AES_GCM_256
        struct tls12_crypto_info_aes_gcm_256 gcm256;
#endif
#ifdef NAI_KTLS_AES_CCM_128
        struct tls12_crypto_info_aes_ccm_128 ccm128;
#endif
#ifdef NAI_KTLS_CHACHA20_POLY1305
        struct tls12_crypto_info_chacha20_poly1305 chacha20poly1305;
#endif
    };

    size_t tls_crypto_info_len;

} nai_ktls_crypto_info_t;



nai_int_t nai_ktls_enable(nai_fd_t fd)
{
    nai_int_t r;


    r = nai_sock_set_opt(
        fd, SOL_TCP, TCP_ULP, "tls", sizeof("tls"));

    return r;
};


nai_int_t nai_ktls_start(nai_fd_t fd, void* info, nai_int_t is_tx)
{
    nai_int_t r;
    nai_ktls_crypto_info_t* cinfo;


    cinfo = (nai_ktls_crypto_info_t*)info;
    r = nai_sock_set_opt(fd, 
        SOL_TLS, is_tx ? TLS_TX : TLS_RX,
        (char*)cinfo, cinfo->tls_crypto_info_len);

    return r;
};


intptr_t nai_ktls_recv(nai_ssl_t* s, void* buf, size_t len, nai_int_t slot)
{
    intptr_t r;
    nai_int_t prelen = SSL3_RT_HEADER_LENGTH;
    nai_int_t cmsglen;
    uint8_t* p;
    nai_bufvec_t v;
    nai_ktls_cmsg_t* cmsg;


    if (len <= (size_t)prelen + EVP_GCM_TLS_TAG_LEN) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    cmsg = (nai_ktls_cmsg_t*)SSL_get_ex_data(s->ssl, slot);
    if (cmsg == 0) {
        cmsg = (nai_ktls_cmsg_t*)nai_malloc(sizeof(*cmsg));
        if (cmsg == 0) {
            r = -1;
            goto _end;
        };
    };

    p = (uint8_t*)buf;
    v.buf = (uint8_t*)p + prelen;
    v.len = len - prelen - EVP_GCM_TLS_TAG_LEN;
    cmsglen = sizeof(cmsg->buf);

    r = s->ops->recvm(s->ud, &v, 1, 0, 0, 0, &cmsg, &cmsglen);
    if (r > 0) {
        if (cmsglen > 0 && 
            cmsg->hdr.cmsg_type == TLS_GET_RECORD_TYPE) {
            p[0] = *((uint8_t*)CMSG_DATA(&cmsg->hdr));
            p[1] = TLS1_2_VERSION_MAJOR;
            p[2] = TLS1_2_VERSION_MINOR;
            /* returned length is limited to msg_iov.iov_len above */
            p[3] = (r >> 8) & 0xff;
            p[4] = (r) & 0xff;
            r += prelen;
        };
    };

_end:
    return r;
};


intptr_t nai_ktls_send(nai_ssl_t* s, 
    const void* buf, size_t len, uint8_t rtype)
{
    intptr_t r;
    nai_bufvec_t v;
    nai_ktls_cmsg_t cmsg;


    cmsg.hdr.cmsg_level = SOL_TLS;
    cmsg.hdr.cmsg_type = TLS_SET_RECORD_TYPE;
    cmsg.hdr.cmsg_len = sizeof(cmsg.buf);
    *((uint8_t*)CMSG_DATA(&cmsg.hdr)) = rtype;

    v.buf = (uint8_t*)buf;
    v.len = len;
    r = s->ops->sendm(s->ud, &v, 1, 0, 0, 0, &cmsg, sizeof(cmsg.buf));

    return r;
};



#elif defined(__freebsd__)



#include <sys/ktls.h>


typedef union nai_ktls_cmsgrd_u {
    struct cmsghdr hdr;
    uint8_t buf[CMSG_SPACE(sizeof(struct tls_get_record))];
} nai_ktls_cmsgrd_t;


nai_int_t nai_ktls_enable(nai_fd_t fd)
{
    nai_int_t r;


    r = 1;

    return r;
};


nai_int_t nai_ktls_start(nai_fd_t fd, void* info, nai_int_t is_tx)
{
    nai_int_t r;
    struct tls_enable* cinfo;


    cinfo = (struct tls_enable*)info;
    if (is_tx) {
        r = nai_sock_set_opt(fd, 
            IPPROTO_TCP, TCP_TXTLS_ENABLE, (char*)cinfo, sizeof(*cinfo));
    } else {
#if defined(TCP_RXTLS_ENABLE)
        r = nai_sock_set_opt(fd, 
            IPPROTO_TCP, TCP_RXTLS_ENABLE, (char*)cinfo, sizeof(*cinfo));
#else
        nai_errno = ENOTSUP;
        r = -1;
#endif
    };

    return r;
};


intptr_t nai_ktls_recv(nai_ssl_t* s, void* buf, size_t len, nai_int_t slot)
{
    intptr_t r;


#if defined(TCP_RXTLS_ENABLE)

    nai_int_t prelen = SSL3_RT_HEADER_LENGTH;
    nai_int_t cmsglen;
    uint8_t* p;
    nai_bufvec_t v;
    nai_ktls_cmsgrd_t* cmsg;
    struct tls_get_record* tgr;


    if (len <= (size_t)prelen) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    cmsg = (nai_ktls_cmsgrd_t*)SSL_get_ex_data(s->ssl, slot);
    if (cmsg == 0) {
        cmsg = (nai_ktls_cmsgrd_t*)nai_malloc(sizeof(*cmsg));
        if (cmsg == 0) {
            r = -1;
            goto _end;
        };
    };

    p = (uint8_t*)buf;
    v.buf = (uint8_t*)p + prelen;
    v.len = len - prelen - EVP_GCM_TLS_TAG_LEN;
    cmsglen = sizeof(cmsg->buf);

    r = s->ops->recvm(s->ud, &v, 1, 0, 0, 0, &cmsg, &cmsglen);
    if (r > 0) {
        if (cmsglen == 0) {
            nai_errno = EBADMSG;
            r = -1;
            goto _end;
        };

        if (cmsg->hdr.cmsg_level != IPPROTO_TCP || 
            cmsg->hdr.cmsg_type != TLS_GET_RECORD || 
            cmsg->hdr.cmsg_len != sizeof(cmsg->buf)) {
            nai_errno = EBADMSG;
            r = -1;
            goto _end;
        };

        tgr = (struct tls_get_record*)CMSG_DATA(&cmsg->hdr);
        p[0] = tgr->tls_type;
        p[1] = tgr->tls_vmajor;
        p[2] = tgr->tls_vminor;
        *(uint16_t*)(p + 3) = htons((int16_t)r);
        r += prelen;
    };

_end:

#else

    (void)s;
    (void)buf;
    (void)len;
    (void)slot;

    nai_errno = ENOTSUP;
    r = -1;

#endif

    return r;
};


intptr_t nai_ktls_send(nai_ssl_t* s, 
    const void* buf, size_t len, uint8_t rtype)
{
    intptr_t r;
    nai_bufvec_t v;
    nai_ktls_cmsg_t cmsg;


    cmsg.hdr.cmsg_level = IPPROTO_TCP;
    cmsg.hdr.cmsg_type = TLS_SET_RECORD_TYPE;
    cmsg.hdr.cmsg_len = sizeof(cmsg.buf);
    *((uint8_t*)CMSG_DATA(&cmsg.hdr)) = rtype;

    v.buf = (uint8_t*)buf;
    v.len = len;
    r = s->ops->sendm(s->ud, &v, 1, 0, 0, 0, &cmsg, sizeof(cmsg.buf));

    return r;
};



#else


nai_int_t nai_ktls_enable(nai_fd_t fd)
{
    nai_int_t r;


    (void)fd;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


nai_int_t nai_ktls_start(nai_fd_t fd, void* info, nai_int_t is_tx)
{
    nai_int_t r;


    (void)fd;
    (void)info;
    (void)is_tx;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


intptr_t nai_ktls_recv(nai_ssl_t* s, void* buf, size_t len, nai_int_t slot)
{
    intptr_t r;


    (void)s;
    (void)buf;
    (void)len;
    (void)slot;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


intptr_t nai_ktls_send(nai_ssl_t* s, 
    const void* buf, size_t len, uint8_t rtype)
{
    intptr_t r;


    (void)s;
    (void)buf;
    (void)len;
    (void)rtype;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};



#endif


#endif

