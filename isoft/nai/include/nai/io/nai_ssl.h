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
/// @file       nai_ssl.h
/// @brief      package implementation of openssl
/// @details
/// @date       2021-03-01
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation separates the data source from the ssl. 
 *          the ssl itself is regarded as a layer of data filter, 
 *          and different data sources are realized through user-defined.
 * @details the ocsp verification and stapling not implemented
 *
 * @details the code example is as follows:
 *
 * @par     initial and open the ssl context
 * @code
 *          nai_int_t r;
 *          nai_int_t methods;
 *          nai_ssl_ctx_t c;
 *
 *          nai_ssl_ctx_init(&c);
 *          r = nai_ssl_ctx_open(&c, methods);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     in server side, load certificate
 * @code
 *          nai_int_t r;
 *          nai_ssl_ctx_t* c;
 *          nai_str_t cert;
 *          nai_str_t key;
 *
 *          nai_str_setc(&cert, "conf/domain.cert");
 *          nai_str_setc(&key, "conf/domain.key");
 *
 *          r = nai_ssl_ctx_certificate(c, &cert, &key, 0, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     implement the callback of the iobase source
 * @code
 *          static nai_int_t my_ssl_lookup_certs(void* ud)
 *          {
 *              ...
 *          };
 *
 *          static nai_int_t my_ssl_lookup_server(void* ud, const char* name)
 *          {
 *              ...
 *          };
 *
 *          static nai_ssl_cb_t my_ssl_cb = {
 *              my_ssl_lookup_certs, 
 *              my_ssl_lookup_server, 
 *              0, // cookie verify
 *              0, // cookie generate
 *              nai_ssl_iobase_poll, 
 *              nai_ssl_iobase_want, 
 *              nai_ssl_iobase_emit, 
 *              nai_ssl_iobase_get_loop, 
 *              nai_ssl_iobase_get_opt, 
 *              nai_ssl_iobase_get_peer, 
 *              0, // realloc
 *              (nai_ssl_read_f)nai_iobase_read, 
 *              (nai_ssl_write_f)nai_iobase_write, 
 *              (nai_ssl_recvm_f)nai_iobase_recvm, 
 *              (nai_ssl_sendm_f)nai_iobase_sendm, 
 *              (nai_ssl_sendfile_f)nai_iobase_sendfile, 
 *              nai_ssl_iobase_get_socket, 
 *          };
 * @endcode
 *
 * @par     initial and open the ssl
 * @code
 *          nai_int_t r;
 *          nai_int_t connect;
 *          nai_stream_t* s;
 *          nai_ssl_ctx_t* c;
 *          nai_ssl_t ssl;
 *
 *
 *          nai_ssl_init(&ssl);
 *          nai_ssl_set_cb(&ssl, &my_ssl_cb, s);
 *
 *          r = nai_ssl_open(&ssl, c, connect);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     handshake before read or write
 * @code
 *          nai_int_t r;
 *          nai_int_t ec;
 *          nai_ssl_t* ssl;
 *
 *          r = nai_ssl_handshake(ssl);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec != NAI_EAGAIN) {
 *                  goto _fail;
 *              };
 *
 *              // blocked, need to retry on next event
 *              goto _wait;
 *          };
 *
 *          // success
 *          ...
 * @endcode
 *
 * @par     shut down the ssl
 * @code
 *          nai_int_t r;
 *          nai_int_t ec;
 *          nai_int_t how;
 *          nai_ssl_t* ssl;
 *
 *          r = nai_ssl_shutdown(ssl, how);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec != NAI_EAGAIN) {
 *                  goto _fail;
 *              };
 *
 *              // blocked, need to retry on next event
 *              goto _wait;
 *          };
 *
 *          // success
 *          ...
 * @endcode
 */

#ifndef _SSL_H_NAI
#define _SSL_H_NAI

#pragma once

#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if (NAI_HAVE_OPENSSL) || (NAI_HAVE_LIBRESSL)
    #define NAI_HAVE_SSL 1
#elif (NAI_HAVE_WOLFSSL)
    #define NAI_HAVE_SSL 1
    #error "the license of wolfssl is GPL, please remove this line to continue."
#endif

#if (NAI_HAVE_SSL)

    /**
 * @anchor  NAI_SSL_METHOD
 * @name    NAI_SSL_METHOD      the methods of the ssl
 * @{
 */
    #define NAI_SSL_SSLv2    0x002
    #define NAI_SSL_SSLv3    0x004
    #define NAI_SSL_TLSv1    0x008
    #define NAI_SSL_TLSv1_1  0x010
    #define NAI_SSL_TLSv1_2  0x020
    #define NAI_SSL_TLSv1_3  0x040
    #define NAI_SSL_TLS      0x07e
    #define NAI_SSL_DTLSv1   0x080
    #define NAI_SSL_DTLSv1_2 0x100
    #define NAI_SSL_DTLS     0x180
    /** @} */

    /**
 * @anchor  NAI_SSL_SESSION
 * @name    NAI_SSL_SEESION     the options of the session cache
 * @{
 */
    #define NAI_SSL_SESSION_OFF    0x1 /**< to disable the cache */
    #define NAI_SSL_SESSION_NONE   0x2 /**< the server mode, no auto clear */
    #define NAI_SSL_SESSION_SERVER 0x3 /**< the server mode of the cache */
    #define NAI_SSL_SESSION_CLIENT 0x4 /**< the client mode of the cache */
    /** @} */

    /**
 * @anchor  NAI_SSL_VERIFY
 * @name    NAI_SSL_VERIFY      the options of the certificate verification
 * @{
 */
    #define NAI_SSL_VERIFY_OFF      0 /**< disable the certificate verification */
    #define NAI_SSL_VERIFY_ON       1 /**< perform the certificate verification */
    #define NAI_SSL_VERIFY_OPTIONAL 2 /**< allow without peer certificate */
    #define NAI_SSL_VERIFY_NO_CA    3 /**< allow without ca and peer certificate */
    /** @} */
    
    /**
 * @anchor  NAI_SSL_VERIFY_MODE
 * @name    NAI_SSL_VERIFY_MODE      the modes of verification
 * @{
 */
    #define NAI_SSL_VERIFY_MODE_NONE      		0
    #define NAI_SSL_VERIFY_MODE_PEER      		1
    #define NAI_SSL_VERIFY_MODE_FAIL_IF_NO_PEER_CERT	2
    #define NAI_SSL_VERIFY_MODE_CLIENT_ONCE    		4
    /** @} */

    /**
 * @anchor  NAI_SSL_OCSP
 * @name    NAI_SSL_OCSP        the options of the ocsp verification
 * @{
 */
    #define NAI_SSL_OCSP_OFF 0 /**< disable the ocsp verification */
    #define NAI_SSL_OCSP_ON  1 /**< perform the ocsp verification */
    #define NAI_SSL_OCSP_LEAF                                                                                          \
        2 /**< perform the ocsp verification without 
                                       recursion cerfificate chain */
    /** @} */

    /**
 * @anchor  NAI_SSL_ERROR
 * @name    NAI_SSL_ERROR       the range of the ssl error code
 */
    #define NAI_SSL_ESTART 20000
    #define NAI_SSL_ELAST  1300000
    /** @} */

    /**
 * test the error code belongs to the ssl
 * @param   s       the error code
 * @return  if it belongs to the ssl, return 1, otherwise return 0
 */
    #define nai_errno_is_ssl(s) ((s) >= NAI_SSL_ESTART && (s) < NAI_SSL_ELAST)

    /**
 * convert error code to ssl lib id
 * @param   s       the error code
 * @return  the value of ssl lib id
 */
    #define nai_errno_to_ssl_lib(s) (((s)-NAI_SSL_ESTART) / 10000 + 2)

    /**
 * convert error code to ssl error code
 * @param   s       the error code
 * @return  the value of ssl error code
 */
    #define nai_errno_to_ssl_code(s) (                      \
    ((s) - NAI_SSL_ESTART % 10000                       \
)

    /**
 * convert original ssl error code to the error code
 * @param   s       the ssl error code
 * @return  the value of the error code
 */
    #define nai_errno_from_ssl(s) (NAI_SSL_ESTART + (s))

    /**
 * test the result code belongs to the ssl
 * @param   s       the result code
 * @return  if it belongs to the ssl, return 1, otherwise return 0
 */
    #define nai_sult_is_ssl(s) (nai_errno_is_ssl(-(s)))

    /**
 * convert the result code to ssl lib id
 * @param   s       the result code
 * @return  the value of ssl lib id
 */
    #define nai_sult_to_ssl_lib(s) (nai_errno_to_ssl(-(s)))

    /**
 * convert the result code to ssl error code
 * @param   s       the result code
 * @return  the value of ssl error code
 */
    #define nai_sult_to_ssl_code(s) (nai_errno_to_ssl(-(s)))

    /**
 * convert original ssl error code to the result code
 * @param   s       the ssl error code
 * @return  the value of the result code
 */
    #define nai_sult_from_ssl(s) (-nai_errno_from_ssl(s))

        //////////////////////////////////////////////////////////////////////////////
        // ssl store

    #ifndef _NAI_TYPEDEF_SSL_STORE_T
        #define _NAI_TYPEDEF_SSL_STORE_T
    typedef struct nai_ssl_store_s nai_ssl_store_t;
    #endif
    #ifndef _NAI_TYPEDEF_SSL_STORE_OPS_T
        #define _NAI_TYPEDEF_SSL_STORE_OPS_T
    typedef struct nai_ssl_store_ops_s nai_ssl_store_ops_t;
    #endif

    /**
 * the structure of the ssl store opeartions
 */
    struct nai_ssl_store_ops_s
    {
        nai_int_t (*set)(void* ud, const nai_mem_t* k, const nai_mem_t* v, time_t expire);
        nai_int_t (*get)(void* ud, const nai_mem_t* k, nai_mem_t* v);
        nai_int_t (*unset)(void* ud, const nai_mem_t* k);
        nai_int_t (*close)(void* ud);
    };

    /**
 * the structure of the ssl store
 */
    struct nai_ssl_store_s
    {
        nai_ssl_store_ops_t* ops; /**< pointer to the ssl store opeartions */
        void* ud;                 /**< pointer to the user data */
    };

    #define nai_ssl_store_is_opened(s) ((s)->ops != 0)

    /**
 * initial the ssl store
 * @param   s       pointer to the ssl store
 * @return  void
 */
    #define nai_ssl_store_init(s)                                                                                      \
        {                                                                                                              \
            (s)->ops = 0;                                                                                              \
            (s)->ud  = 0;                                                                                              \
        }

    /**
 * open the ssl store with the specified memory
 * @param   s       pointer to the ssl store
 * @param   mem     pointer to the specified memory
 * @param   size    the size of the specified memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_store_from(nai_ssl_store_t* s, void* mem, size_t size);

    /**
 * open the ssl store
 * @param   s       pointer to the ssl store
 * @param   size    the size of the store memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_store_open(nai_ssl_store_t* s, size_t size);

    /**
 * add a new data into the ssl store
 * @param   s       pointer to the ssl store
 * @param   k       pointer to the memory view of key
 * @param   v       pointer to the memory view of value
 * @param   expire  the value of the expire time
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_store_set(nai_ssl_store_t* s, const nai_mem_t* k, const nai_mem_t* v, time_t expire);

    /**
 * get an exists data from the ssl store
 * @param   s       pointer to the ssl store
 * @param   k       pointer to the memory view of key
 * @param   v       pointer to the memory view of value to get data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_store_get(nai_ssl_store_t* s, const nai_mem_t* k, nai_mem_t* v);

    /**
 * remove an exists data from the ssl store
 * @param   s       pointer to the ssl store
 * @param   k       pointer to the memory view of key
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_store_unset(nai_ssl_store_t* s, const nai_mem_t* k);

    /**
 * close the ssl store
 * @param   s       pointer to the ssl store
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_store_close(nai_ssl_store_t* s);

        //////////////////////////////////////////////////////////////////////////////
        // ssl context

    #ifndef _NAI_TYPEDEF_SSL_CTX_OPS_T
        #define _NAI_TYPEDEF_SSL_CTX_OPS_T
    typedef struct nai_ssl_ctx_ops_s nai_ssl_ctx_ops_t;
    #endif

    /**
 * the structure of the ssl context operations
 */
    struct nai_ssl_ctx_ops_s
    {
        /**
     * get full path
     * @param   ud      pointer to the user data
     * @param   buf     pointer to the buffer
     * @param   buflen  the length of the buffer
     * @param   path    pointer to the string of the path
     * @param   len     the length of the path
     * @retval  >=0     the length of output without null-terminated on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        intptr_t (*get_full_path)(void* ud, char* buf, size_t buflen, const char* path, size_t len);
    };

    #if (NAI_HAVE_OPENSSL) || (NAI_HAVE_LIBRESSL)
    typedef struct ssl_st SSL;
    typedef struct ssl_ctx_st SSL_CTX;
    #elif (NAI_HAVE_WOLFSSL)
    /* the latest version '5.6' is missing some interfaces */
    typedef struct WOLFSSL SSL;
    typedef struct WOLFSSL_CTX SSL_CTX;
    #endif

    #ifndef _NAI_TYPEDEF_SSL_CTX_T
        #define _NAI_TYPEDEF_SSL_CTX_T
    typedef struct nai_ssl_ctx_s nai_ssl_ctx_t;
    #endif
    #ifndef _NAI_TYPEDEF_CONN_ATTR_T
        #define _NAI_TYPEDEF_CONN_ATTR_T
    typedef struct nai_conn_attr_s nai_conn_attr_t;
    #endif

    /**
 * the structure of the ssl context
 */
    struct nai_ssl_ctx_s
    {
        SSL_CTX* ctx;           /**< pointer to the openssl context */
        nai_ssl_ctx_ops_t* ops; /**< pointer to the ssl context operations */
        void* ud;               /**< pointer to the user data */
        union
        {
            struct
            {
                uint32_t tls : 1; /**< tls or dtls */
            };
            uint32_t flags;
        };
    };

    /**
 * initial the ssl context
 * @param   s       pointer to the ssl context
 * @return  void
 */
    #define nai_ssl_ctx_init(s)                                                                                        \
        {                                                                                                              \
            (s)->ctx   = 0;                                                                                            \
            (s)->ops   = 0;                                                                                            \
            (s)->ud    = 0;                                                                                            \
            (s)->flags = 0;                                                                                            \
        }

    /**
 * set the ssl context operations
 * @param   s       pointer to the ssl context
 * @param   p       pointer to the ssl context operations
 * @param   u       pointer to the user data
 * @return  void
 */
    #define nai_ssl_ctx_set_ops(s, p, u)                                                                               \
        {                                                                                                              \
            (s)->ops = (p);                                                                                            \
            (s)->ud  = (u);                                                                                            \
        }

    /**
 * open the ssl context
 * @param   ssl     pointer to the ssl context
 * @param   methods the methods, multiple options, see @ref NAI_SSL_METHOD
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_open(nai_ssl_ctx_t* ssl, nai_int_t methods);

    /**
 * close the ssl context
 * @param   ssl     pointer to the ssl context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_close(nai_ssl_ctx_t* ssl);

    /**
 * load certificate
 * @param   ssl     pointer to the ssl context
 * @param   cert    pointer to the string view of the certificte
 * @param   key     pointer to the string view of the key
 * @param   passwords pointer to the array of the string view
 * @param   count   the count of the passwords array
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_certificate(
        nai_ssl_ctx_t* ssl, const nai_str_t* cert, const nai_str_t* key, const nai_str_t* passwords, nai_int_t count);

    /**
 * set the depth of the certificate verification
 * @param   ssl     pointer to the ssl context
 * @param   depth   the value of the depth
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_verify_depth(nai_ssl_ctx_t* ssl, nai_int_t depth);

    /**
 * set the mode of the certificate verification
 * @param   ssl     pointer to the ssl context
 * @param   mode    the mode of the verification
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_set_verify(nai_ssl_ctx_t* ssl, nai_int_t mode);
    
    /**
 * to use the server's cipher suite order 
 * instead of the client's during cipher suite negotiation
 * @param   ssl     pointer to the ssl context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_prefer_server_ciphers(nai_ssl_ctx_t* ssl);

    /**
 * load the client certificate
 * @param   ssl     pointer to the ssl context
 * @param   cert    pointer to the string view of the certificate
 * @retval  >=0     the number of added certificate is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_client_certificate(nai_ssl_ctx_t* ssl, const nai_str_t* cert);

    /**
 * set the path of the trusted certificate
 * @param   ssl     pointer to the ssl context
 * @param   cert    pointer to the string view of the certificate
 * @retval  >=0     the number of added certificate is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_trusted_certificate(nai_ssl_ctx_t* ssl, const nai_str_t* cert);

    /**
 * load the revoked certificates (CRL) in the PEM format
 * @param   ssl     pointer to the ssl context
 * @param   cert    pointer to the string view of the certificate
 * @retval  >=0     the number of revoked certificate is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_crl(nai_ssl_ctx_t* ssl, const nai_str_t* cert);

    /**
 * load DH parameters for DHE ciphers
 * @param   ssl     pointer to the ssl context
 * @param   data    pointer to the string view of DH parameters
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_dhparam(nai_ssl_ctx_t* ssl, const nai_str_t* data);

    /**
 * set the ciphers list
 * @param   ssl     pointer to the ssl context
 * @param   ciphers pointer to the string of the ciphers list
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_ciphers(nai_ssl_ctx_t* ssl, const char* ciphers);

    /**
 * set the ECDHE ciphers
 * @param   ssl     pointer to the ssl context
 * @param   name    pointer to the name of ECDHE ciphers
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_ecdh_curve(nai_ssl_ctx_t* ssl, const char* name);

    /**
 * to enable ktls (kernel tls)
 * @param   ssl     pointer to the ssl context
 * @param   on      indicates whether ktls is enabled
 * @retval  1       the ktls is enabled
 * @retval  0       the ktls is disabled, always return 0 if not supported
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_enable_ktls(nai_ssl_ctx_t* ssl, nai_int_t on);

    /**
 * set the attribute of backend connect
 * @param   ssl     pointer to the ssl context
 * @param   attr    pointer to the attribute of connect
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_conn_attr(nai_ssl_ctx_t* ssl, const nai_conn_attr_t* attr);

    /**
 * to enable ocsp verify
 * @param   ssl     pointer to the ssl context
 * @param   mode    the options of ocsp, see @ref NAI_SSL_OCSP
 * @param   ocsp    pointer to the default ocsp, optional
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_ocsp(nai_ssl_ctx_t* ssl, nai_int_t mode, const nai_str_t* ocsp);

    /**
 * set the store of ssl ocsp
 * @param   ssl     pointer to the ssl context
 * @param   store   pointer to the ssl store
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_ocsp_store(nai_ssl_ctx_t* ssl, nai_ssl_store_t* store);

    /**
 * set the option of the session cache
 * @param   ssl     pointer to the ssl context
 * @param   type    the type of the cache, see @ref NAI_SSL_SESSION
 * @param   cache_size the toal size of the cache in bytes
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the cache size is used when type is NAI_SSL_SESSION_SERVER
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_session_cache(nai_ssl_ctx_t* ssl, nai_int_t type, size_t cache_size);

    /**
 * set the timeout of the session
 * @param   ssl     pointer to the ssl context
 * @param   sec     the value of the timeout in seconds
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_session_timeout(nai_ssl_ctx_t* ssl, nai_int_t sec);

    /**
 * set the prefix of the unique session id
 * @param   ssl     pointer to the ssl context
 * @param   prefix  pointer to the fixed prefix
 * @param   extra   pointer to an array of the string view to gen unique parts
 * @param   count   the count of the array
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_ctx_session_unique_id(nai_ssl_ctx_t* ssl,
                                            const char* prefix,
                                            const nai_str_t* extra,
                                            nai_int_t count);

    /**
 * read a password file which format is password + newline
 * @param   a       pointer to an array of the string view
 * @param   path    pointer to the path of the passwords
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_read_password(nai_array_t* a, const char* path);

    /**
 * the callback of cleanup the memory of the passwords
 * @param   p       pointer to the passwords array
 * @return  void
 */
    NAI_EXTERN
    void nai_ssl_cleanup_password(void* p);

    /**
 * dupilcate a new passwords array
 * @param   p       pointer to the memory pool
 * @param   a       pointer to the passwords array
 * @return  the address of the new password array, 
 *          null is returned on fails, see #nai_errno.
 */
    NAI_EXTERN
    nai_array_t* nai_ssl_dup_password(nai_pool_t* p, const nai_array_t* a);

    /**
 * load a passwords file
 * @param   p       pointer to the memory pool
 * @param   path    pointer to the path of the passwords
 * @return  the address of the new password array, 
 *          null is returned on fails, see #nai_errno.
 */
    NAI_EXTERN
    nai_array_t* nai_ssl_load_password(nai_pool_t* p, const char* path);

    #ifndef _NAI_TYPEDEF_COMMAND_T
        #define _NAI_TYPEDEF_COMMAND_T
    typedef struct nai_command_s nai_command_t;
    #endif
    #ifndef _NAI_TYPEDEF_COMMAND_ARGS_T
        #define _NAI_TYPEDEF_COMMAND_ARGS_T
    typedef struct nai_command_args_s nai_command_args_t;
    #endif

    /**
 * the command of load passwrod
 */
    NAI_EXTERN
    nai_sult_t nai_ssl_command_password(nai_command_t* c, nai_command_args_t* args);

        //////////////////////////////////////////////////////////////////////////////
        // ssl

        /**
 * @anchor  NAI_SSL_OPT
 * @name    NAI_SSL_OPT         the options of the ssl operation
 * @{
 */

    #define NAI_SSL_OPT_ASYNCREAD 1 /**< is read on async mode */
    #define NAI_SSL_OPT_SENDFILE  2 /**< is supported sendfile */
    #define NAI_SSL_OPT_BLOCKING  3 /**< is work on blocking */

        /** @} */

    #ifndef _NAI_TYPEDEF_BUFVEC_T
        #define _NAI_TYPEDEF_BUFVEC_T
    typedef struct nai_bufvec_s nai_bufvec_t;
    #endif
    #ifndef _NAI_TYPEDEF_BUFLIST_T
        #define _NAI_TYPEDEF_BUFLIST_T
    typedef struct nai_buflist_s nai_buflist_t;
    #endif
    #ifndef _NAI_TYPEDEF_SOCKADDR_T
        #define _NAI_TYPEDEF_SOCKADDR_T
    typedef struct sockaddr nai_sockaddr_t;
    #endif
    #ifndef _NAI_TYPEDEF_EVLOOP_T
        #define _NAI_TYPEDEF_EVLOOP_T
    typedef struct nai_evloop_s nai_evloop_t;
    #endif
    #ifndef _NAI_TYPEDEF_SSL_READ_F
        #define _NAI_TYPEDEF_SSL_READ_F
    typedef intptr_t (*nai_ssl_read_f)(void* ud, void* buf, size_t size);
    #endif
    #ifndef _NAI_TYPEDEF_SSL_WRITE_F
        #define _NAI_TYPEDEF_SSL_WRITE_F
    typedef intptr_t (*nai_ssl_write_f)(void* ud, const void* buf, size_t size);
    #endif
    #ifndef _NAI_TYPEDEF_SSL_RECVM_F
        #define _NAI_TYPEDEF_SSL_RECVM_F
    typedef intptr_t (*nai_ssl_recvm_f)(void* ud,
                                        nai_bufvec_t* v,
                                        nai_int_t count,
                                        nai_uint_t flags,
                                        nai_sockaddr_t* name,
                                        nai_int_t* namelen,
                                        void* ctrl,
                                        nai_int_t* ctrllen);
    #endif
    #ifndef _NAI_TYPEDEF_SSL_SENDM_F
        #define _NAI_TYPEDEF_SSL_SENDM_F
    typedef intptr_t (*nai_ssl_sendm_f)(void* ud,
                                        const nai_bufvec_t* v,
                                        nai_int_t count,
                                        nai_uint_t flags,
                                        const nai_sockaddr_t* name,
                                        nai_int_t namelen,
                                        const void* ctrl,
                                        nai_int_t ctrllen);
    #endif
    #ifndef _NAI_TYPEDEF_SSL_SENDFILE_F
        #define _NAI_TYPEDEF_SSL_SENDFILE_F
    typedef intptr_t (*nai_ssl_sendfile_f)(void* ud, nai_fd_t fd, size_t size, nai_off64_t offset);
    #endif

    #ifndef _NAI_TYPEDEF_SSL_OPS_T
        #define _NAI_TYPEDEF_SSL_OPS_T
    typedef struct nai_ssl_ops_s nai_ssl_ops_t;
    #endif

    /**
 * the structure of the ssl operations
 */
    struct nai_ssl_ops_s
    {
        /**
     * the callback to load certificate
     * @param   ud      pointer to the user data
     * @retval  >=0     on success
     * @retval  <0      on failed
     */
        nai_int_t (*lookup_certs)(void* ud);

        /**
     * the callback to check the host name
     * @param   ud      pointer to the user data
     * @param   name    pointer to the host name
     * @retval  >=0     on success
     * @retval  <0      on failed
     */
        nai_int_t (*lookup_server)(void* ud, const char* name);

        /**
     * custom verify a cookie
     * @param   ud      pointer to the user data
     * @param   cookie  pointer to the cookie data
     * @param   len     the length of the cookie data
     * @retval  1       on success
     * @retval  0       on failed
     */
        nai_int_t (*cookie_verify)(void* ud, const char* cookie, size_t len);

        /**
     * custom generate a cookie
     * @param   ud      pointer to the user data
     * @param   cookie  pointer to the cookie buffer
     * @param   len     pointer to the length of cookie
     * @retval  1       on success
     * @retval  0       on failed
     */
        nai_int_t (*cookie_generate)(void* ud, char* cookie, size_t* len);

        /**
     * poll for specified events
     * @param   ud      pointer to the user data
     * @param   events  the value of the events
     * @retval  1       the one of the events is ready
     * @retval  0       the events is not ready
     */
        nai_int_t (*poll)(void* ud, nai_int_t event);

        /**
     * set waiting event
     * @param   ud      pointer to the user data
     * @param   event   the value of the event
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     * @note    if event is NAI_EV_READ, means ssl writing blocked on read
     * @note    if event is NAI_EV_WRITE, means ssl reading blocked on write
     * @note    the event does not match the call that returns a blocking, 
     *          when the event arrives, you need to simulate the event that 
     *          matches the blocking call.
     */
        nai_int_t (*want)(void* ud, nai_int_t event);

        /**
     * emit an event when async load complete
     * @param   ud      pointer to the user data
     * @param   event   the value of the event
     * @return  any
     */
        nai_int_t (*emit)(void* ud, nai_int_t event);

        /**
     * get the event loop
     * @param   ud      pointer to the user data
     * @return  the address of the event loop
     */
        nai_evloop_t* (*get_loop)(void* ud);

        /**
     * get the option of the operations
     * @param   ud      pointer to the user data
     * @param   which   the request option, see @ref NAI_SSL_OPT
     * @retval  1       the option is on
     * @retval  0       the option is off
     */
        nai_int_t (*get_opt)(void* ud, nai_int_t which);

        /**
     * get peer name for generate cookie
     * @param   ud      pointer to the user data
     * @param   name    pointer to the address buffer
     * @param   namelen pointer to the length of the address
     * @retval  >=0     the length of the address is returned on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        nai_int_t (*get_peer)(void* ud, nai_sockaddr_t* name, nai_int_t* namelen);

        /**
     * alloc and free memory
     * @param   ud      pointer to the user data
     * @param   ptr     pointer to the memory to free
     * @param   size    the allocate size, do nothing when size is 0
     * @retval  !=0     the address of new memory is returned on success
     * @retval  0       an error occurred when size != 0, see #nai_errno
     * @note    this function should not copy the content in the free memory
     */
        void* (*realloc)(void* ud, void* ptr, size_t size);

        /* used for tls/dtls */
        /**
     * read data from the source
     * @param   ud      pointer to the user data
     * @param   buf     pointer to the buffer to read
     * @param   size    the size of the buffer
     * @retval  >=0     the number of bytes read is returned on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        intptr_t (*read)(void* ud, void* buf, size_t size);

        /**
     * write data to the source
     * @param   ud      pointer to the user data
     * @param   buf     pointer to the buffer to write
     * @param   size    the size of the buffer
     * @retval  >=0     the number of bytes wrote is returned on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        intptr_t (*write)(void* ud, const void* buf, size_t size);

        /* used for dtls/ktls */
        /**
     * receive data from the source
     * @param   ud      pointer to the user data
     * @param   v       pointer to an array of the buffer vector
     * @param   count   the count of the buffer vector
     * @param   flags   the flags of send, see man of recv
     * @param   name    pointer to the address buffer to receive address
     * @param   namelen pointer to the length of the address buffer
     * @param   ctrl    pointer to the control buffer
     * @param   ctrllen pointer to the length of the control buffer
     * @retval  >=0     the number of bytes received is returned on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        intptr_t (*recvm)(void* ud,
                          nai_bufvec_t* v,
                          nai_int_t count,
                          nai_uint_t flags,
                          nai_sockaddr_t* name,
                          nai_int_t* namelen,
                          void* ctrl,
                          nai_int_t* ctrllen);

        /**
     * send data to the source
     * @param   ud      pointer to the user data
     * @param   v       pointer to an array of the buffer vector
     * @param   count   the count of the buffer array
     * @param   flags   the flags of send, see man of send
     * @param   name    pointer to the destination address
     * @param   namelen the length of the address
     * @param   ctrl    pointer to the control buffer
     * @param   ctrllen the length of control buffer
     * @retval  >=0     the number of bytes sent is returned on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        intptr_t (*sendm)(void* ud,
                          const nai_bufvec_t* v,
                          nai_int_t count,
                          nai_uint_t flags,
                          const nai_sockaddr_t* name,
                          nai_int_t namelen,
                          const void* ctrl,
                          nai_int_t ctrllen);

        /* used for ktls only */
        /**
     * send a file to the source
     * @param   s       pointer to the iobase
     * @param   fd      the file descriptor to write
     * @param   size    the number of bytes written
     * @param   offset  the start offset of the file
     * @retval  >=0     the number of bytes written
     * @retval  -1      an error occurred, see #nai_errno
     */
        intptr_t (*sendfile)(void* ud, nai_fd_t fd, size_t size, nai_off64_t offset);

        /**
     * get file descriptor of socket
     * @param   ud      pointer to the user data
     * @return  the file descriptor of socket
     */
        nai_fd_t (*get_socket)(void* ud);

        /* used for dtls only */
        /**
     * get mtu size, default value is 1500
     * @param   ud      pointer to the user data
     * @return  the mtu size
     */
        nai_int_t (*get_mtu)(void* ud);

        /**
     * set dtls handshake timeout
     * @param   ud      pointer to the iobase
     * @param   msec    the value of timeout, in milli-seconds
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        nai_int_t (*set_timer)(void* ud, uint32_t msec);
    };

    #ifndef _NAI_TYPEDEF_SSL_T
        #define _NAI_TYPEDEF_SSL_T
    typedef struct nai_ssl_s nai_ssl_t;
    #endif

    /**
 * the structure of the ssl
 */
    struct nai_ssl_s
    {
        SSL* ssl;            /**< pointer to the openssl */
        nai_ssl_ctx_t* ctx;  /**< pointer to the ssl context */
        nai_ssl_ops_t* ops;  /**< pointer to the ssl operations */
        void* ud;            /**< pointer to the user data */
        void* load;          /**< pointer to the async load */
        void* wbuf;          /**< pointer to the write buffer */
        uint16_t wsize;      /**< the size of the write buffer */
        uint8_t early_byte;  /**< the first byte of early data */
        uint8_t record_type; /**< the ktls record type */

        union
        {
            struct
            {
                uint32_t early_byte_get : 1; /**< the first byte is valid */
                uint32_t early_hand : 1;     /**< in early handshake */
                uint32_t early_read : 1;     /**< in ealry reading */
                uint32_t early_write : 1;    /**< in early writing */
                uint32_t tls : 1;            /**< is tls or dtls */
                uint32_t connect : 1;        /**< in connect mode */
                uint32_t handshaked : 1;     /**< is handshaked */
                uint32_t rejected : 1;       /**< is rejected */
                uint32_t renegotiation : 1;  /**< is renegotiation */
                uint32_t listened : 1;       /**< is dtls listened */
                uint32_t timeoset : 1;       /**< is dtls timeout seted */
                uint32_t ktls : 2;           /**< is ktls is enabled */
                uint32_t ocsp : 3;           /**< the state of ocsp */
                uint32_t loadstat : 2;       /**< the state of loadfile */
                uint32_t loadfile : 1;       /**< is enable loadfile */
                uint32_t buffer : 1;         /**< is enable internal write buffer */
                uint32_t buffered : 1;       /**< with pending write buffer */
                uint32_t error : 1;          /**< has an error occurred */
                uint32_t read_pending : 1;   /**< is read pending */
            };
            uint32_t flags;
        };
    };

    /**
 * initial the ssl
 * @param   s       pointer to the ssl
 * @return  void
 */
    #define nai_ssl_init(s)                                                                                            \
        {                                                                                                              \
            (s)->ssl   = 0;                                                                                            \
            (s)->ctx   = 0;                                                                                            \
            (s)->ops   = 0;                                                                                            \
            (s)->ud    = 0;                                                                                            \
            (s)->load  = 0;                                                                                            \
            (s)->wbuf  = 0;                                                                                            \
            (s)->wsize = 0;                                                                                            \
            (s)->flags = 0;                                                                                            \
            (s)->ktls  = 3;                                                                                            \
        }

    /**
 * set the ssl operations
 * @param   s       pointer to the ssl
 * @param   p       pointer to the ssl operations
 * @param   u       pointer to the user data
 * @return  void
 */
    #define nai_ssl_set_ops(s, p, u)                                                                                   \
        {                                                                                                              \
            (s)->ops = (p);                                                                                            \
            (s)->ud  = (u);                                                                                            \
        }

    /**
 * set the mark of the handshake rejected
 * @param   s       pointer to the ssl
 * @return  void
 * @note    this function is used in handshaked
 */
    #define nai_ssl_reject(s)                                                                                          \
        {                                                                                                              \
            (s)->rejected = 1;                                                                                         \
        }

    /**
 * test the ssl supported the callback of certifictes
 * @return  if it supported, return 1, otherwise return 0
 */
    NAI_EXTERN
    nai_int_t nai_ssl_support_certs_cb();

    /**
 * test the ssl supported ktls
 * @return  if it supported, return 1, otherwise return 0
 */
    NAI_EXTERN
    nai_int_t nai_ssl_support_ktls();

    /**
 * open the ssl
 * @param   s       pointer to the ssl
 * @param   ssl     pointer to the ssl context
 * @param   connect do connect or accept
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_open(nai_ssl_t* s, const nai_ssl_ctx_t* ssl, nai_int_t connect);

    /**
 * load certificate
 * @param   s       pointer to the ssl
 * @param   cert    pointer to the string view of the certificte
 * @param   key     pointer to the string view of the key
 * @param   passwords pointer to the array of the string view
 * @param   count   the count of the passwords array
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_certificate(
        nai_ssl_t* s, const nai_str_t* cert, const nai_str_t* key, const nai_str_t* passwords, nai_int_t count);

    /**
 * change a new ssl context
 * @param   s       pointer to the ssl
 * @param   ctx     pointer to the new ssl context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_set_ctx(nai_ssl_t* s, const nai_ssl_ctx_t* ctx);

    /**
 * set the host name to send to peer on handshake
 * @param   s       pointer to the ssl
 * @param   name    pointer to the string view of the host name
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_set_host(nai_ssl_t* s, const nai_str_t* name);

    /**
 * do handshake
 * @param   s       pointer to the ssl
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is #NAI_EAGAIN, 
 *          means need to try again on next event.
 */
    NAI_EXTERN
    nai_int_t nai_ssl_handshake(nai_ssl_t* s);

    /**
 * verify the certificate of peer
 * @param   s       pointer to the ssl
 * @param   opt     the verification option, see @ref NAI_SSL_VERIFY
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_verify(nai_ssl_t* s, nai_int_t opt);

    /**
 * shut down part of a full-duplex connection
 * @param   s       pointer to the ssl
 * @param   how     which to shut down, see @ref NAI_SOCK_SHUTDOWN
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is #NAI_EAGAIN, 
 *          means need to try again on next event.
 */
    NAI_EXTERN
    nai_int_t nai_ssl_shutdown(nai_ssl_t* s, nai_int_t how);

    /**
 * close the ssl
 * @param   s       pointer to the ssl
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_close(nai_ssl_t* s);

    /**
 * to enable write buffer
 * @param   s       pointer to the ssl
 * @param   size    the size of write buffer, set 0 to disable
 * @retval  1       the write buffer is enabled
 * @retval  0       the write buffer is disabled
 * @retval  -1      an error occurred, see #nai_errno
 * @note    openssl does not support writev. when writing multiple small and 
 *          discontinuous buffers, openssl will generate a lot of extra load 
 *          on the network. enable it to solve this problem.
 */
    NAI_EXTERN
    nai_int_t nai_ssl_enable_wbuf(nai_ssl_t* s, size_t size);

    /**
 * to enable ktls (kernel tls)
 * @param   s       pointer to the ssl
 * @param   on      indicates whether ktls is enabled
 * @retval  1       the ktls is enabled
 * @retval  0       the ktls is disabled, always return 0 if not supported
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function should be called before handshake
 * @note    ktls can use hardware acceleration when the system supports it.
 * @note    ktls can reduce the number of kernel switches when sending files 
 *          or using hardware acceleration.
 */
    NAI_EXTERN
    nai_int_t nai_ssl_enable_ktls(nai_ssl_t* s, nai_int_t on);

    /**
 * to enable async file load
 * @param   s       pointer to the ssl
 * @param   on      indicates whether ktls is enabled
 * @retval  1       the async load is enabled
 * @retval  0       the async load is disabled
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_enable_load(nai_ssl_t* s, nai_int_t on);

    /**
 * read data from the ssl
 * @param   s       pointer to the ssl
 * @param   buf     pointer to the buffer to receive data
 * @param   size    the length of buffer
 * @retval  >=0     the number of bytes read is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN.
 */
    NAI_EXTERN
    intptr_t nai_ssl_read(nai_ssl_t* s, void* buf, size_t size);

    /**
 * read data into multiple buffers
 * @param   s       pointer to the ssl
 * @param   v       pointer to an array of the buffer vector
 * @param   count   the count of buffer vector
 * @retval  >=0     the number of bytes read is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to #NAI_EAGAIN.
 */
    NAI_EXTERN
    intptr_t nai_ssl_readv(nai_ssl_t* s, nai_bufvec_t* v, nai_int_t count);

    /**
 * write data to the iobase
 * @param   s       pointer to the ssl
 * @param   buf     pointer to the buffer to write
 * @param   size    the length of buffer
 * @retval  >=0     the number of bytes written is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN.
 */
    NAI_EXTERN
    intptr_t nai_ssl_write(nai_ssl_t* s, const void* buf, size_t size);

    /**
 * write multiple buffers to the iobase
 * @param   s       pointer to the ssl
 * @param   v       pointer to an array of the buffer vector
 * @param   count   the count of buffer vector
 * @retval  >=0     the number of bytes written is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN.
 */
    NAI_EXTERN
    intptr_t nai_ssl_writev(nai_ssl_t* s, const nai_bufvec_t* v, nai_int_t count);

    /**
 * read data into the buffer list
 * @param   s       pointer to the ssl
 * @param   in      pointer to the buffer list to receive data
 * @param   limit   maximum the number of bytes read
 * @retval  >=0     the number of bytes read is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN.
 */
    NAI_EXTERN
    intptr_t nai_ssl_readq(nai_ssl_t* s, nai_buflist_t* in, size_t limit);

    /**
 * write the buffer list to the ssl
 * @param   s       pointer to the ssl
 * @param   out     pointer to the buffer list to write
 * @param   limit   maximum the number of bytes written
 * @retval  >=0     the number of bytes written is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN or NAI_EINPROGRESS.
 * @note    if it fails and errno is set to NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing the buffer will cause incorrect data to be sent.
 */
    NAI_EXTERN
    intptr_t nai_ssl_writeq(nai_ssl_t* s, nai_buflist_t* out, size_t limit);

    /**
 * send a file to the ssl
 * @param   s       pointer to the ssl
 * @param   fd      the file descriptor
 * @param   size    the number of bytes written
 * @param   offset  the start offset of the file
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN or NAI_EINPROGRESS.
 * @note    if it fails and errno is set to NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the file descriptor until the next call returns successfully. 
 *          close the file descriptor will cause send failed.
 * @note    this function will failed if it is not support sendfile and ktls.
 * @note    this function will not failed if loadfile is is enabled.
 */
    NAI_EXTERN
    intptr_t nai_ssl_sendfile(nai_ssl_t* s, nai_fd_t fd, size_t size, nai_off64_t offset);

    //////////////////////////////////////////////////////////////////////////////
    // dtls server

    /**
 * listen on the dtls server
 * @param   s       pointer to the ssl
 * @param   name    pointer to the peer address to retrieve
 * @param   namelen pointer to the length of the address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dtls_listen(nai_ssl_t* s, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * accept the incoming dtls connection
 * @param   s       pointer to the ssl
 * @param   l       pointer to the listen ssl
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    what this function mainly does is to move the member of listen ssl 
 *          to the dest ssl and create a new ssl to continue listening
 */
    NAI_EXTERN
    nai_int_t nai_dtls_bypass(nai_ssl_t* s, nai_ssl_t* l);

    /**
 * drop the incoming dtls connection
 * @param   s       pointer to the ssl
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dtls_drop(nai_ssl_t* s);

    /**
 * handle dtls handshake timeout and 
 * retransmits the previous flight of handshake messages
 * @param   s       pointer to the ssl
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dtls_handshake_timeout(nai_ssl_t* s);

    /**
 * get max data size
 * @param   s       pointer to the ssl
 * @retval  >=0     the mtu size is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dtls_get_mtu(nai_ssl_t* s);

    //////////////////////////////////////////////////////////////////////////////
    // default ssl operations implementation
    // which assume custom user data points to an iobase

    /**
 * default implementation 'poll' of nai_ssl_ops_t
 * @param   ud      pointer to the iobase
 * @param   event   the value of the event
 * @retval  1       the one of the events is ready
 * @retval  0       the events is not ready
 */
    NAI_EXTERN
    nai_int_t nai_ssl_iobase_poll(void* ud, nai_int_t event);

    /**
 * default implementation 'want' of nai_ssl_ops_t
 * @param   ud      pointer to the iobase
 * @param   event   the value of the event
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_iobase_want(void* ud, nai_int_t event);

    /**
 * default implementation 'emit' of nai_ssl_ops_t
 * @param   ud      pointer to the iobase
 * @param   event   the value of the event
 * @return  any
 */
    NAI_EXTERN
    nai_int_t nai_ssl_iobase_emit(void* ud, nai_int_t event);

    /**
 * default implementation 'get_loop' of nai_ssl_ops_t
 * @param   ud      pointer to the iobase
 * @return  the address of the event loop
 */
    NAI_EXTERN
    nai_evloop_t* nai_ssl_iobase_get_loop(void* ud);

    /**
 * default implementation 'get_opt' of nai_ssl_ops_t
 * @param   ud      pointer to the iobase
 * @param   which   the request option, see @ref NAI_SSL_OPT
 * @retval  1       the option is on
 * @retval  0       the option is off
 */
    NAI_EXTERN
    nai_int_t nai_ssl_iobase_get_opt(void* ud, nai_int_t which);

    /**
 * default implementation 'get_peer' of nai_ssl_ops_t
 * @param   ud      pointer to the iobase
 * @param   name    pointer to the address buffer
 * @param   namelen pointer to the length of the address
 * @retval  >=0     the length of the address is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_ssl_iobase_get_peer(void* ud, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * default implementation 'get_socket' of nai_ssl_ops_t
 * @param   ud      pointer to the user data
 * @return  the file descriptor of socket
 */
    NAI_EXTERN
    nai_int_t nai_ssl_iobase_get_socket(void* ud);

    /**
 * default implementation 'get_mtu' of nai_ssl_ops_t
 * @param   ud      pointer to the iobase
 * @return  the mtu size
 */
    NAI_EXTERN
    nai_int_t nai_ssl_iobase_get_mtu(void* ud);

    /**
 * default implementation 'set_timer' of nai_ssl_ops_t
 * @param   ud      pointer to the iobase
 * @param   msec    the value of timeout, in milli-seconds
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function uses nai_iobase_set_timeout directly
 */
    NAI_EXTERN
    nai_int_t nai_ssl_iobase_set_timer(void* ud, uint32_t msec);

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
