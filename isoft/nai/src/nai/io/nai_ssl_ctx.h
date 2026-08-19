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
/// @file       nai_ssl_ctx.h
/// @brief      
/// @details
/// @date       2022-10-22
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _SSL_CTX_H_NAI
#define _SSL_CTX_H_NAI

#pragma once

#include "nai/io/nai_ssl.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if (NAI_HAVE_SSL)

    /* for use deprecated function in openssl 3.0.0 */
    #define OPENSSL_API_COMPAT 0x10101000

    #include "nai/os/nai_socket.h"
    #include "nai/os/nai_thread.h"

    #if (NAI_HAVE_OPENSSL) || (NAI_HAVE_LIBRESSL)
        #include <openssl/ssl.h>
    #elif (NAI_HAVE_WOLFSSL)
        #define HAVE_OCSP    1
        #define WOLFSSL_QT   1
        #define WOLFSSL_DTLS 1
        #define OPENSSL_ALL  1
        #include <wolfssl/openssl/ssl.h>
        #undef SSL_MODE_RELEASE_BUFFERS
    #endif

    #if defined(SSL_OP_ENABLE_KTLS) && !defined(OPENSSL_NO_KTLS)
        #define NAI_HAVE_KTLS 1
    #endif

    //////////////////////////////////////////////////////////////////////////////
    // ssl global

    typedef struct nai_ssl_lib_s nai_ssl_lib_t;

    /**
 * the structure of the ssl global data
 */
    struct nai_ssl_lib_s
    {
        nai_int_t inited;       /**< is initialized */
        nai_int_t error;        /**< the error code of initialization */
        nai_once_t once;        /**< the once */
        BIO_METHOD* meth;       /**< pointer to the method of stream bio */
        BIO_METHOD* meth_dgram; /**< pointer to the method of dgram bio */
    #if OPENSSL_VERSION_NUMBER >= 0x10101000L && !(defined(LIBRESSL_VERSION_NUMBER) || defined(LIBWOLFSSL_VERSION_HEX))
        BIO_ADDR* temp; /**< pointer to the temporary address */
    #else
        nai_sockaddr_t* temp; /**< pointer to the temporary address */
    #endif
        union
        {
            struct
            {
                nai_int_t connection;       /**< the slot of ssl */
                nai_int_t extend;           /**< the slot of ssl */
                nai_int_t ocsp;             /**< the slot of ssl */
                nai_int_t ocsp_conf;        /**< the slot of ctx */
                nai_int_t certificate;      /**< the slot of ctx */
                nai_int_t certificate_next; /**< the slot of cert */
                nai_int_t certificate_name; /**< the slot of cert */
                nai_int_t stapling;         /**< the slot of cert */
            };
            nai_int_t index[8]; /**< the slot array */
        };
        uint8_t secret[16]; /**< the secret data */
    };

    extern nai_ssl_lib_t nai_ssl;

    //////////////////////////////////////////////////////////////////////////////
    // ssl error

    /**
 * set last ssl error to nai_errno
 * @param   e       the default value of current error
 * @return  void
 */
    void nai_ssl_set_errno(nai_int_t e);

    /**
 * clear ssl error
 * @return  void
 */
    void nai_ssl_clear_errno();

    //////////////////////////////////////////////////////////////////////////////
    // ssl cert

    typedef struct nai_ssl_cert_s nai_ssl_cert_t;
    typedef struct nai_ssl_pkey_s nai_ssl_pkey_t;
    typedef struct nai_ssl_cert_crl_s nai_ssl_cert_crl_t;

    /**
 * the structure of the ssl certificate
 */
    struct nai_ssl_cert_s
    {
        X509* cert;             /**< pointer to the X509 */
        STACK_OF(X509) * chain; /**< pointer to the chain of X509 */
    };

    /**
 * the structure of the ssl private key
 */
    struct nai_ssl_pkey_s
    {
        EVP_PKEY* pkey; /**< pointer to the private key */
    };

    /**
 * the structure of the ssl certificate revocation list
 */
    struct nai_ssl_cert_crl_s
    {
        STACK_OF(X509_INFO) * chain; /**< pointer to the chain of X509 info */
    };

    /**
 * load certificate
 * @param   ctx     pointer to the ssl context
 * @param   p       pointer to the struct of cert
 * @param   cert    pointer to the string view of the certificte
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_ssl_load_certificate(nai_ssl_ctx_t* ctx, nai_ssl_cert_t* p, const nai_str_t* cert);

    /**
 * load certificate private key
 * @param   ctx     pointer to the ssl context
 * @param   p       pointer to the struct of key
 * @param   key     pointer to the string view of the key
 * @param   passwords pointer to the array of the string view
 * @param   count   the count of the passwords array
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_ssl_load_certificate_key(
        nai_ssl_ctx_t* ctx, nai_ssl_pkey_t* p, const nai_str_t* key, const nai_str_t* passwords, nai_int_t count);

    /**
 * load certificate and revoked certificate
 * @param   ctx     pointer to the ssl context
 * @param   p       pointer to the struct of cert and crl
 * @param   cert    pointer to the string view of the certificte
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_ssl_load_cert_crl(nai_ssl_ctx_t* ctx, nai_ssl_cert_crl_t* p, const nai_str_t* cert);

    //////////////////////////////////////////////////////////////////////////////
    // dtls

    typedef struct nai_ssl_dgram_s nai_ssl_dgram_t;

    /**
 * the structure of the context of dgram ssl
 */
    struct nai_ssl_dgram_s
    {
        nai_int_t size;        /**< the size of write buffer */
        nai_int_t wlen;        /**< the length of written */
        uint32_t async : 1;    /**< is async read opeartion */
        uint32_t wpending : 1; /**< is async write pending */
        uint8_t* rbuf;         /**< pointer to read buffer */
        uint8_t* wbuf;         /**< pointer to write buffer */
        nai_socknbuf_t peer;   /**< the buffer of peer address */
    };

    //////////////////////////////////////////////////////////////////////////////
    // ssl loadfile

    /**
 * reduce hit count and trigger memory cleanup when count returns to zero
 * @param   s       pointer to the ssl
 * @return  void
 */
    #define nai_ssl_load_unhits(s)                                                                                     \
        {                                                                                                              \
            assert((s)->loadstat == NAI_ASYNC_LOAD_DONE);                                                              \
            nai_async_load_unhits((nai_async_load_t*)(s)->load);                                                       \
        }

    /**
 * test whether the async loader is available
 * @param   s       pointer to the ssl
 * @return  if the async loader is available, return 1, otherwise return 0
 */
    nai_int_t nai_ssl_load_available(nai_ssl_t* s);

    /**
 * discard the async transmit opeartion
 * @param   s       pointer to the ssl
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_ssl_load_discard(nai_ssl_t* s);

    /**
 * load files into the buffer list and flush to the ssl
 * @param   s       pointer to the ssl
 * @param   list    pointer to the buffer list
 * @param   bytes   the maximum number of bytes to write
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    intptr_t nai_ssl_load_and_flush(nai_ssl_t* s, nai_buflist_t* list, size_t bytes);

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
