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
/// @file       nai_ssl_ocsp.h
/// @brief      
/// @details
/// @date       2023-03-31
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _SSL_OCSP_H_NAI
#define _SSL_OCSP_H_NAI

#pragma once

#include "nai/io/nai_ssl.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if (NAI_HAVE_SSL)

    #include "nai/io/nai_agent.h"
    #include "nai/io/nai_dns.h"
    #include "nai/io/nai_io.h"
    #include "nai/io/nai_sample_http.h"
    #include "nai/io/nai_ssl_ctx.h"
    #include "nai/runtime/nai_pool.h"
    #include "nai/runtime/nai_uri.h"

    #ifndef _NAI_TYPEDEF_SSL_CTX_T
        #define _NAI_TYPEDEF_SSL_CTX_T
    typedef struct nai_ssl_ctx_s nai_ssl_ctx_t;
    #endif
    #ifndef _NAI_TYPEDEF_SSL_OCSP_T
        #define _NAI_TYPEDEF_SSL_OCSP_T
    typedef struct nai_ssl_ocsp_s nai_ssl_ocsp_t;
    #endif
    #ifndef _NAI_TYPEDEF_SSL_OCSP_CB_F
        #define _NAI_TYPEDEF_SSL_OCSP_CB_F
    typedef nai_int_t (*nai_ssl_ocsp_cb_f)(nai_ssl_ocsp_t* p, void* ud);
    #endif

    /**
 * the structure of the ssl ocsp request
 */
    struct nai_ssl_ocsp_s
    {
        nai_stream_t str;     /**< the request connection */
        nai_pool_t pool;      /**< the memory pool */
        nai_conn_attr_t conn; /**< the connect attributes */
        nai_ssl_t* ssl;       /**< pointer to the ssl to verify */
        nai_ssl_ctx_t* ctx;   /**< pointer to the ssl context */
        nai_ssl_ocsp_cb_f cb; /**< pointer to the callback of oscp */
        void* ud;             /**< pointer to the user data */

        /* cert chain */
        STACK_OF(X509) * certs; /**< pointer to the chain of X509 */
        X509_STORE* store;      /**< pointer to the store of X509 */

        /* cert to verify */
        X509* cert;   /**< pointer to the current X509 */
        X509* issuer; /**< pointer to the issuer of current X509 */

        /* status */
        nai_int_t mode;     /**< the mode of oscp verification */
        nai_int_t next;     /**< the index of next X509 */
        nai_int_t stat;     /**< the step of ocsp */
        nai_int_t mkey;     /**< is the key of result available */
        nai_int_t blocking; /**< is work in blocking mode */
        nai_int_t status;   /**< the verification status of X509 */
        time_t expire;      /**< the expire time of verification result */
        nai_mem_t key;      /**< the key of verification result */

        /* request */
        nai_int_t port;           /**< the port of the ocsp server */
        nai_str_t host;           /**< the host of the ocsp server */
        nai_uri_t uri;            /**< the uri of the ocsp server */
        nai_sample_request_t req; /**< the request of sample http */

        /* request buffer */
        struct
        {
            nai_int_t alloc; /**< the allocated size */
            nai_int_t size;  /**< the content size */
            char* buf;       /**< pointer to the request buffer */
        } out;

        /* response buffer */
        struct
        {
            nai_int_t alloc; /**< the allocated size */
            nai_int_t size;  /**< the content size */
            char* buf;       /**< pointer to the response buffer */
        } in;

        /* dest addresses */
        struct
        {
            nai_dns_result_t rs; /**< the dns result */
            nai_int_t index;     /**< the index of addresses */
            nai_int_t count;     /**< the count of addresses */
            nai_int_t alloc;     /**< the allocated size */
            char* buf;           /**< pointer to the addresses buffer */
        } dst;
    };

    /**
 * initial the ocsp request
 * @param   p       pointer to the ocsp request
 * @return  void
 */
    #define nai_ssl_ocsp_init(p)                                                                                       \
        {                                                                                                              \
            nai_uri_init(&(p)->uri);                                                                                   \
            nai_str_setn(&(p)->host);                                                                                  \
            nai_str_setn(&(p)->key);                                                                                   \
            nai_pool_init(&(p)->pool, 4096 * 4);                                                                       \
            nai_conn_attr_init(&(p)->conn);                                                                            \
            nai_stream_init(&(p)->str);                                                                                \
            (p)->ctx       = 0;                                                                                        \
            (p)->ssl       = 0;                                                                                        \
            (p)->cb        = 0;                                                                                        \
            (p)->ud        = 0;                                                                                        \
            (p)->stat      = 0;                                                                                        \
            (p)->status    = 0;                                                                                        \
            (p)->expire    = 0;                                                                                        \
            (p)->certs     = 0;                                                                                        \
            (p)->store     = 0;                                                                                        \
            (p)->cert      = 0;                                                                                        \
            (p)->issuer    = 0;                                                                                        \
            (p)->next      = 0;                                                                                        \
            (p)->mode      = 0;                                                                                        \
            (p)->mkey      = 0;                                                                                        \
            (p)->blocking  = 0;                                                                                        \
            (p)->in.buf    = 0;                                                                                        \
            (p)->in.size   = 0;                                                                                        \
            (p)->in.alloc  = 0;                                                                                        \
            (p)->out.buf   = 0;                                                                                        \
            (p)->out.size  = 0;                                                                                        \
            (p)->out.alloc = 0;                                                                                        \
            (p)->dst.buf   = 0;                                                                                        \
            (p)->dst.alloc = 0;                                                                                        \
            (p)->dst.index = 0;                                                                                        \
            (p)->dst.count = 0;                                                                                        \
        }

    /**
 * set the callback of the ocsp request
 * @param   p       pointer to the ocsp request
 * @param   c       pointer to the callback of the ocsp request
 * @param   u       pointer to the user data
 * @return  void
 */
    #define nai_ssl_ocsp_set_cb(p, c, u)                                                                               \
        {                                                                                                              \
            (p)->cb = (c);                                                                                             \
            (p)->ud = (u);                                                                                             \
        }

    /**
 * set the blocking mode of the ocsp request
 * @param   p       pointer to the ocsp request
 * @param   on      indicates whether enable blocking mode
 * @return  void
 */
    #define nai_ssl_ocsp_set_blocking(p, on)                                                                           \
        {                                                                                                              \
            (p)->blocking = !!(on);                                                                                    \
        }

    /**
 * set the uri of the ocsp server
 * @param   p       pointer to the ocsp request
 * @param   u       pointer to the uri of the ocsp server
 * @return  void
 */
    #define nai_ssl_ocsp_set_uri(p, u)                                                                                 \
        {                                                                                                              \
            (p)->uri = *(u);                                                                                           \
        }

    /**
 * set the connect attributes
 * @param   p       pointer to the ocsp request
 * @param   c       pointer to the connect attributes
 * @return  void
 */
    #define nai_ssl_ocsp_set_conn(p, c)                                                                                \
        {                                                                                                              \
            (p)->conn = *(c);                                                                                          \
        }

    /**
 * create a new ocsp request
 * @return  the address of the new ocsp request
 */
    nai_ssl_ocsp_t* nai_ssl_ocsp_new();

    /**
 * open the ocsp request
 * @param   p       pointer to the ocsp request
 * @param   ctx     pointer to the ssl context
 * @param   ssl     pointer to the ssl to verify
 * @param   certs   pointer to the chain of X509
 * @param   mode    the mode of oscp verification, see @ref NAI_SSL_OCSP
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_ssl_ocsp_open(
        nai_ssl_ocsp_t* p, nai_ssl_ctx_t* ctx, nai_ssl_t* ssl, STACK_OF(X509) * certs, nai_int_t mode);

    /**
 * close the ocsp request
 * @param   p       pointer to the ocsp request
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_ssl_ocsp_close(nai_ssl_ocsp_t* p);

    /**
 * do ocsp verify by the ocsp request
 * @param   p       pointer to the ocsp request
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_ssl_ocsp_verify(nai_ssl_ocsp_t* p);

    /**
 * do ocsp verify by cache
 * @param   ctx     pointer to the ssl context
 * @param   ssl     pointer to the ssl to verify
 * @param   certs   pointer to the chain of X509
 * @param   mode    the mode of oscp verification, see @ref NAI_SSL_OCSP
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_ssl_ocsp_verify_cache(nai_ssl_ctx_t* ctx, nai_ssl_t* ssl, STACK_OF(X509) * certs, nai_int_t mode);

    /**
 * prepare the ocsp request of the ssl
 * @param   s       pointer to the ssl
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_ssl_prepare_ocsp(nai_ssl_t* s);

    /**
 * do ocsp verify for the ssl
 * @param   s       pointer to the ssl
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_ssl_verify_ocsp(nai_ssl_t* s);

    /**
 * cleanup the ocsp request of the ssl
 * @param   s       pointer to the ssl
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_ssl_cleanup_ocsp(nai_ssl_t* s);

    /**
 * cleanup the ocsp data of the ssl context
 * @param   c       pointer to the ssl context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_ssl_ctx_cleanup_ocsp(nai_ssl_ctx_t* c);

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
