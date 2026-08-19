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
/// @file       nsi_message.h
/// @brief
/// @details
/// @date       2021-05-11
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _MESSAGE_H_NSI
#define _MESSAGE_H_NSI

#pragma once

#include "nsomeip/core/nsi_types.h"
#include "nsomeip/net/nsi_cred.h"
#include "nai/io/nai_buf.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_errno.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // message header

#define NSI_PROTOCOL_VERSION 0x01

#define NSI_DEFAULT_MAJOR 0x00

/**
 * @anchor  NSI_MTYPE
 * @name    NSI_MTYPE           the type of message
 * @{
 */
/** A request expecting a response (even void) */
#define NSI_MT_REQUEST 0x00
/** A fire&forget request */
#define NSI_MT_REQUEST_NO_RETURN 0x01
/** A request of a notification/event callback expecting no response */
#define NSI_MT_NOTIFICATION 0x02
/** The response message */
#define NSI_MT_RESPONSE 0x80
/** The response containing an error */
#define NSI_MT_ERROR 0x81
/** A TP request expecting a response (even void) */
#define NSI_TP_REQUEST 0x20
/** A TP fire&forget request */
#define NSI_TP_REQUEST_NO_RETURN 0x21
/** A TP request of a notification/event callback expecting no response */
#define NSI_TP_NOTIFICATION 0x22
/** The TP response message */
#define NSI_TP_RESPONSE 0x23
/** The TP response containing an error */
#define NSI_TP_ERROR 0x24
/** @} */

/**
 * @anchor  NSI_ERROR
 * @name    NSI_ERROR           the error code
 * @{
 */
/** No error occurred */
#define NSI_E_OK 0x00
/** An unspecified error occurred */
#define NSI_E_NOT_OK 0x01
/** The requested Service ID is unknown. */
#define NSI_E_UNKNOWN_SERVICE 0x02
/** The requested Method ID is unknown. Service ID is known. */
#define NSI_E_UNKNOWN_METHOD 0x03
/** Service ID and Method ID are known. Application not running. */
#define NSI_E_NOT_READY 0x04
/** System running the service is not reachable (internal error code only). */
#define NSI_E_NOT_REACHABLE 0x05
/** A timeout occurred (internal error code only). */
#define NSI_E_TIMEOUT 0x06
/** Version of SOME/IP protocol not supported */
#define NSI_E_WRONG_PROTOCOL_VERSION 0x07
/** Interface version mismatch */
#define NSI_E_WRONG_INTERFACE_VERSION 0x08
/** Deserialization error, so that payload cannot be deserialized. */
#define NSI_E_MALFORMED_MESSAGE 0x09
/** An unexpected message type was received (e.g. */
#define NSI_E_WRONG_MESSAGE_TYPE 0x0a
/** @} */

/**
 * @anchor  NSI_MSGLEN
 * @name    NSI_MSGLEN          the length of message header
 * @{
 */
/* the length of message header */
#define NSI_MSGHDR_SIZE (nai_offsetof(nsi_msghdr_t, client))
/* the length of message extra, PRS_SOMEIPSD_00153 */
#define NSI_MSGEXT_SIZE (sizeof(nsi_msghdr_t) - NSI_MSGHDR_SIZE)
/* the length of signal extra */
#define NSI_SIGEXT_SIZE (0)
    /** @} */

#pragma pack(push, 1)

#ifndef _NSI_TYPEDEF_MSGHDR_T
    #define _NSI_TYPEDEF_MSGHDR_T
    typedef struct nsi_msghdr_s nsi_msghdr_t;
#endif
#ifndef _NSI_TYPEDEF_MSGHDR_TP_T
    #define _NSI_TYPEDEF_MSGHDR_TP_T
    typedef struct nsi_msghdr_tp_s nsi_msghdr_tp_t;
#endif

    /**
 * the structure of the message header
 * PRS_SOMEIPSD_00251
 */
    struct nsi_msghdr_s
    {
        /* message id */
        uint16_t serv;   /**< the service id */
        uint16_t method; /**< the method/event id */

        /* message length */
        uint32_t len; /**< the message length, 
                                     exclude member before 'client', 
                                     see #NSI_MSGHDR_SIZE.
                                     PRS_SOMEIPSD_00153 */

        /* request id */
        uint16_t client;  /**< the client id */
        uint16_t session; /**< the session id */

        /* message infomation */
        uint8_t protocol;  /**< the protocol version */
        uint8_t interface; /**< the interface version */
        uint8_t type;      /**< the message type, see @ref NSI_MTYPE */
        uint8_t code;      /**< the result code, see @ref NSI_ERROR */

        /* payload */
        /* uint8_t payload[]; */
    };

    /**
 * the structrue of the message tp header
 */
    struct nsi_msghdr_tp_s
    {
        union
        {
            struct
            {
                uint32_t more : 1; /**< has more messages */
                uint32_t res : 3;  /**< reserved */
                uint32_t off : 28; /**< the offset of payload */
            };
            uint32_t info;
        };

        /* payload */
        /* uint8_t payload[]; */
    };

#pragma pack(pop)

#define nsi_msgtype_is_tp(t) ((t)&0x20)

    //////////////////////////////////////////////////////////////////////////////
    // message

#ifndef _NSI_TYPEDEF_MSGPOOL_T
    #define _NSI_TYPEDEF_MSGPOOL_T
    typedef struct nsi_msgpool_s nsi_msgpool_t;
#endif
#ifndef _NSI_TYPEDEF_MESSAGE_T
    #define _NSI_TYPEDEF_MESSAGE_T
    typedef struct nsi_message_s nsi_message_t;
#endif
#ifndef _NSI_TYPEDEF_MSGPOOL_OPS_T
    #define _NSI_TYPEDEF_MSGPOLL_OPS_T
    typedef struct nsi_msgpool_ops_s nsi_msgpool_ops_t;
#endif


    /**
 * the structure of the message pool operations
 */
    struct nsi_msgpool_ops_s
    {
        nsi_message_t* (*message_create)(nsi_msgpool_t* p);
        nsi_message_t* (*message_dup)(nsi_message_t* m, int mt);
        int (*message_close)(nsi_message_t* m);
        int (*close)(nsi_msgpool_t* p);
    };

    /**
 * the structure of the message pool
 */
    struct nsi_msgpool_s
    {
        nsi_msgpool_ops_t* ops;
        void* ud;
        nai_spin_t* lock;      /**< pointer to the spin lock */
        nai_list_entry_t list; /**< the list of free messages */
        size_t size;           /**< the allocate size of the buffer pool */
        size_t alloc;          /**< the number of allocated messages */
        int usage;             /**< for send or for read */
    };

    /**
 * the structure of the message
 */
    struct nsi_message_s
    {
        nai_list_entry_t ent; /**< the list entry */
        nsi_msgpool_t* pool;  /**< pointer to the message pool */
        void* port;           /**< for npc - port info */
        nsi_message_t* refm;  /**< pointer to the reference message */
        nai_atomic32_t refc;  /**< reference count */
        nsi_connid_t conn;    /**< source connection id */
        nsi_inst_t inst;      /**< instance id */
        uint16_t cookie;      /**< cookie, used for posted message */
        uint16_t qindex;      /**< for npc - the index of message queue in port */
        union
        {
            struct
            {
                uint32_t reliable : 1;  /**< is using a reliable channel */
                uint32_t sent : 1;      /**< is a sent message */
                uint32_t last : 1;      /**< used for process tp-message */
                uint32_t reserved : 5;  /**< reserved bits */
                uint32_t unique_id : 8; /**< unique event id */
            };
            uint32_t flags;
        };
        nsi_msghdr_t hdr;      /**< the message header */
        nai_buflist_t payload; /**< the buffer list */
        nsi_cred_t cred;       /**< the message cred */
    };

    /**
 * initial the message pool
 * @param   p       pointer to the message pool
 * @param   size    the allocate size of the buffer pool in the message
 * @param   lock    pointer to the spin lock, optional
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NSI_EXTERN
    int nsi_msgpool_init(nsi_msgpool_t* p, size_t size, nai_spin_t* lock, int usage);

    /**
 * close the message pool
 * @param   p       pointer to the message pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_msgpool_close(nsi_msgpool_t* p);

    /**
 * create a new meesage
 * @param   p       pointer to the message pool
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    NSI_EXTERN
    nsi_message_t* nsi_message_create(nsi_msgpool_t* p);

    /**
 * dupilcate the message
 * @param   m       pointer to the message
 * @param   mt      whether cloned message will be used by other threads
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    NSI_EXTERN
    nsi_message_t* nsi_message_dup(nsi_message_t* m, int mt);

    /**
 * increase the reference of the message
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_message_ref(nsi_message_t* m);

    /**
 * close the message
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_message_close(nsi_message_t* m);

    //////////////////////////////////////////////////////////////////////////////
    // message io

#ifndef _NSI_TYPEDEF_MESSAGE_IO_T
    #define _NSI_TYPEDEF_MESSAGE_IO_T
    typedef struct nsi_message_io_s nsi_message_io_t;
#endif

    /**
 * the structure of the io to help read/write the message
 */
    struct nsi_message_io_s
    {
        uint8_t* ptr;       /**< pointer to the read/write buffer */
        size_t len;         /**< the left length of the buffer */
        nai_buf_t* buf;     /**< pointer to the current buffer */
        nsi_message_t* msg; /**< pointer to the message */
    };

/**
 * initial the  message io
 * @param   p       pointer to the message io
 * @return  void
 */
#define nsi_message_io_init(p)                                                                                         \
    {                                                                                                                  \
        (p)->ptr = 0;                                                                                                  \
        (p)->len = 0;                                                                                                  \
        (p)->buf = 0;                                                                                                  \
        (p)->msg = 0;                                                                                                  \
    };

#define nsi_message_read_init  nsi_message_io_init
#define nsi_message_write_init nsi_message_io_init

    /**
 * start read the message
 * @param   p       pointer to the message io
 * @param   m       pointer to the message
 * @param   offset  the offset of the message to read
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_message_read_start(nsi_message_io_t* p, nsi_message_t* m, size_t offset);

    /**
 * stop read the message
 * @param   p       pointer to the message io
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_message_read_end(nsi_message_io_t* p);

    /**
 * read data from the message
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to read
 * @param   len     the length of the buffer
 * @retval  >=0     the number of bytes read on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    intptr_t nsi_message_read(nsi_message_io_t* p, void* buf, size_t len);

    /**
 * read at least the specified number of bytes from the message
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to read
 * @param   len     the length of the buffer
 * @retval  >=0     the number of bytes read on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    intptr_t nsi_message_read_least(nsi_message_io_t* p, void* buf, size_t len);

    /**
 * skip the number of bytes
 * @param   p       pointer to the message io
 * @param   len     the number of bytes to skip
 * @retval  >=0     the number of bytes skipped on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    intptr_t nsi_message_read_skip(nsi_message_io_t* p, size_t len);

    /**
 * start write to the end of message
 * @param   p       pointer to the message io
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_message_write_start(nsi_message_io_t* p, nsi_message_t* m);

    /**
 * stop write the message
 * @param   p       pointer to the message io
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_message_write_end(nsi_message_io_t* p);

    /**
 * reserve buffer space of the specified size to the tail
 * @param   p       pointer to the message io
 * @param   len     the specified size
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_message_reserve(nsi_message_io_t* p, size_t len);

    /**
 * insert a user buffer to current position
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to insert
 * @param   len     the length of the buffer
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_message_insert(nsi_message_io_t* p, const void* buf, size_t len);

    /**
 * write data to the message
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to write
 * @param   len     the length of the buffer
 * @retval  >=0     the number of bytes written on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    intptr_t nsi_message_write(nsi_message_io_t* p, const void* buf, size_t len);

    /**
 * over write data to the message
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to write
 * @param   len     the length of the buffer
 * @retval  >=0     the number of bytes written on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    intptr_t nsi_message_overwrite(nsi_message_io_t* p, const void* buf, size_t len);

//////////////////////////////////////////////////////////////////////////////
// message bcopy

/**
 * copy a byte
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define nsi_bcopy_u1(d, s)                                                                                             \
    {                                                                                                                  \
        (d)[0] = (s)[0];                                                                                               \
    };

/**
 * copy a word
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define nsi_bcopy_u2(d, s)                                                                                             \
    {                                                                                                                  \
        (d)[0] = (s)[0];                                                                                               \
        (d)[1] = (s)[1];                                                                                               \
    };

/**
 * copy a dword
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define nsi_bcopy_u4(d, s)                                                                                             \
    {                                                                                                                  \
        (d)[0] = (s)[0];                                                                                               \
        (d)[1] = (s)[1];                                                                                               \
        (d)[2] = (s)[2];                                                                                               \
        (d)[3] = (s)[3];                                                                                               \
    };

/**
 * copy a qword
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define nsi_bcopy_u8(d, s)                                                                                             \
    {                                                                                                                  \
        (d)[0] = (s)[0];                                                                                               \
        (d)[1] = (s)[1];                                                                                               \
        (d)[2] = (s)[2];                                                                                               \
        (d)[3] = (s)[3];                                                                                               \
        (d)[4] = (s)[4];                                                                                               \
        (d)[5] = (s)[5];                                                                                               \
        (d)[6] = (s)[6];                                                                                               \
        (d)[7] = (s)[7];                                                                                               \
    };

/**
 * byte swap a byte
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define nsi_bswap_u1(d, s)                                                                                             \
    {                                                                                                                  \
        (d)[0] = (s)[0];                                                                                               \
    };

/**
 * byte swap a word
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define nsi_bswap_u2(d, s)                                                                                             \
    {                                                                                                                  \
        (d)[0] = (s)[1];                                                                                               \
        (d)[1] = (s)[0];                                                                                               \
    };

/**
 * byte swap a dword
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define nsi_bswap_u4(d, s)                                                                                             \
    {                                                                                                                  \
        (d)[0] = (s)[3];                                                                                               \
        (d)[1] = (s)[2];                                                                                               \
        (d)[2] = (s)[1];                                                                                               \
        (d)[3] = (s)[0];                                                                                               \
    };

/**
 * byte swap a qword
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define nsi_bswap_u8(d, s)                                                                                             \
    {                                                                                                                  \
        (d)[0] = (s)[7];                                                                                               \
        (d)[1] = (s)[6];                                                                                               \
        (d)[2] = (s)[5];                                                                                               \
        (d)[3] = (s)[4];                                                                                               \
        (d)[4] = (s)[3];                                                                                               \
        (d)[5] = (s)[2];                                                                                               \
        (d)[6] = (s)[1];                                                                                               \
        (d)[7] = (s)[0];                                                                                               \
    };

#if defined(NAI_HAVE_BIG_ENDIAN)
    #define nsi_message_get_le nsi_message_get_bswap
    #define nsi_message_put_le nsi_message_put_bswap
    #define nsi_message_get_be nsi_message_get_bcopy
    #define nsi_message_put_be nsi_message_put_bcopy
    #define nsi_message_put_u2 nsi_message_put_u2_be
    #define nsi_message_put_u4 nsi_message_put_u4_be
    #define nsi_message_put_u8 nsi_message_put_u8_be
    #define nsi_message_put_f4 nsi_message_put_f4_be
    #define nsi_message_put_f8 nsi_message_put_f8_be
    #define nsi_message_get_u2 nsi_message_get_u2_be
    #define nsi_message_get_u4 nsi_message_get_u4_be
    #define nsi_message_get_u8 nsi_message_get_u8_be
    #define nsi_message_get_f4 nsi_message_get_f4_be
    #define nsi_message_get_f8 nsi_message_get_f8_be
#else
    #define nsi_message_get_le nsi_message_get_bcopy
    #define nsi_message_put_le nsi_message_put_bcopy
    #define nsi_message_get_be nsi_message_get_bswap
    #define nsi_message_put_be nsi_message_put_bswap
    #define nsi_message_put_u2 nsi_message_put_u2_le
    #define nsi_message_put_u4 nsi_message_put_u4_le
    #define nsi_message_put_u8 nsi_message_put_u8_le
    #define nsi_message_put_f4 nsi_message_put_f4_le
    #define nsi_message_put_f8 nsi_message_put_f8_le
    #define nsi_message_get_u2 nsi_message_get_u2_le
    #define nsi_message_get_u4 nsi_message_get_u4_le
    #define nsi_message_get_u8 nsi_message_get_u8_le
    #define nsi_message_get_f4 nsi_message_get_f4_le
    #define nsi_message_get_f8 nsi_message_get_f8_le
#endif

#define nsi_message_get_u1 nsi_message_get_u1_le
#define nsi_message_put_u1 nsi_message_put_u1_le

    //////////////////////////////////////////////////////////////////////////////
    // message fast get

#define nsi_message_get_bcopy(p, d, s)                                                                                 \
    if ((p)->len >= (s)) {                                                                                             \
        nsi_bcopy_u##s((uint8_t*)d, (p)->ptr);                                                                         \
        (p)->ptr += (s);                                                                                               \
        (p)->len -= (s);                                                                                               \
        r = (s);                                                                                                       \
    } else {                                                                                                           \
        r = (int)nsi_message_read((p), (d), (s));                                                                      \
        if (r >= 0) {                                                                                                  \
            if (r < (s)) {                                                                                             \
                nai_errno = EOVERFLOW;                                                                                 \
                r         = -1;                                                                                        \
            };                                                                                                         \
        };                                                                                                             \
    };

#define nsi_message_get_bswap(p, d, s)                                                                                 \
    if ((p)->len >= (s)) {                                                                                             \
        nsi_bswap_u##s((uint8_t*)d, (p)->ptr);                                                                         \
        (p)->ptr += (s);                                                                                               \
        (p)->len -= (s);                                                                                               \
        r = (s);                                                                                                       \
    } else {                                                                                                           \
        uint8_t m[s];                                                                                                  \
        r = (int)nsi_message_read((p), (m), (s));                                                                      \
        if (r >= 0) {                                                                                                  \
            if (r < (s)) {                                                                                             \
                nai_errno = EOVERFLOW;                                                                                 \
                r         = -1;                                                                                        \
            } else {                                                                                                   \
                nsi_bswap_u##s((uint8_t*)(d), m);                                                                      \
            };                                                                                                         \
        };                                                                                                             \
    };

#if 0 && defined(__GNUC__)

    #define nsi_message_get_u1_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            nsi_message_get_bcopy(p, d, 1);                                                                            \
            r;                                                                                                         \
        })

    #define nsi_message_get_u2_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            nsi_message_get_be(p, d, 2);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_get_u4_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            nsi_message_get_be(p, d, 4);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_get_u8_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            nsi_message_get_be(p, d, 8);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_get_u1_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            nsi_message_get_bcopy(p, d, 1);                                                                            \
            r;                                                                                                         \
        })

    #define nsi_message_get_u2_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            nsi_message_get_le(p, d, 2);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_get_u4_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            nsi_message_get_le(p, d, 4);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_get_u8_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            nsi_message_get_le(p, d, 8);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_get_f4_be nsi_message_get_u4_be
    #define nsi_message_get_f8_be nsi_message_get_u8_be
    #define nsi_message_get_f4_le nsi_message_get_u4_le
    #define nsi_message_get_f8_le nsi_message_get_u8_le

#else

static NSI_INLINE int nsi_message_get_u1_be(nsi_message_io_t* p, uint8_t* d)
{
    int r;

    nsi_message_get_bcopy(p, d, 1);

    return r;
};

static NSI_INLINE int nsi_message_get_u2_be(nsi_message_io_t* p, uint16_t* d)
{
    int r;

    nsi_message_get_be(p, d, 2);

    return r;
};

static NSI_INLINE int nsi_message_get_u4_be(nsi_message_io_t* p, uint32_t* d)
{
    int r;

    nsi_message_get_be(p, d, 4);

    return r;
};

static NSI_INLINE int nsi_message_get_u8_be(nsi_message_io_t* p, uint64_t* d)
{
    int r;

    nsi_message_get_be(p, d, 8);

    return r;
};

static NSI_INLINE int nsi_message_get_f4_be(nsi_message_io_t* p, float* d)
{
    int r;

    nsi_message_get_be(p, d, 4);

    return r;
};

static NSI_INLINE int nsi_message_get_f8_be(nsi_message_io_t* p, double* d)
{
    int r;

    nsi_message_get_be(p, d, 8);

    return r;
};

static NSI_INLINE int nsi_message_get_u1_le(nsi_message_io_t* p, uint8_t* d)
{
    int r;

    nsi_message_get_bcopy(p, d, 1);

    return r;
};

static NSI_INLINE int nsi_message_get_u2_le(nsi_message_io_t* p, uint16_t* d)
{
    int r;

    nsi_message_get_le(p, d, 2);

    return r;
};

static NSI_INLINE int nsi_message_get_u4_le(nsi_message_io_t* p, uint32_t* d)
{
    int r;

    nsi_message_get_le(p, d, 4);

    return r;
};

static NSI_INLINE int nsi_message_get_u8_le(nsi_message_io_t* p, uint64_t* d)
{
    int r;

    nsi_message_get_le(p, d, 8);

    return r;
};

static NSI_INLINE int nsi_message_get_f4_le(nsi_message_io_t* p, float* d)
{
    int r;

    nsi_message_get_le(p, d, 4);

    return r;
};

static NSI_INLINE int nsi_message_get_f8_le(nsi_message_io_t* p, double* d)
{
    int r;

    nsi_message_get_le(p, d, 8);

    return r;
};

#endif

    //////////////////////////////////////////////////////////////////////////////
    // message fast put

#define nsi_message_put_bcopy(p, d, s)                                                                                 \
    if ((p)->len >= (s)) {                                                                                             \
        nsi_bcopy_u##s((p)->ptr, (uint8_t*)&(d));                                                                      \
        (p)->ptr += (s);                                                                                               \
        (p)->len -= (s);                                                                                               \
        r = (s);                                                                                                       \
    } else {                                                                                                           \
        r = (int)nsi_message_write((p), &(d), (s));                                                                    \
    };

#define nsi_message_put_bswap(p, d, s)                                                                                 \
    if ((p)->len >= (s)) {                                                                                             \
        nsi_bswap_u##s((p)->ptr, (uint8_t*)&(d));                                                                      \
        (p)->ptr += (s);                                                                                               \
        (p)->len -= (s);                                                                                               \
        r = (s);                                                                                                       \
    } else {                                                                                                           \
        uint8_t m[s];                                                                                                  \
        nsi_bswap_u##s(m, (uint8_t*)&(d));                                                                             \
        r = (int)nsi_message_write((p), (m), (s));                                                                     \
    };

#if 0 && defined(__GNUC__)

    #define nsi_message_put_u1_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint8_t v = d;                                                                                             \
            nsi_message_put_bcopy(p, v, 1);                                                                            \
            r;                                                                                                         \
        })

    #define nsi_message_put_u2_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint16_t v = d;                                                                                            \
            nsi_message_put_be(p, v, 2);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_put_u4_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint32_t v = d;                                                                                            \
            nsi_message_put_be(p, v, 4);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_put_u8_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint64_t v = d;                                                                                            \
            nsi_message_put_be(p, v, 8);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_put_f4_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            float v = d;                                                                                               \
            nsi_message_put_be(p, v, 4);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_put_f8_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            double v = d;                                                                                              \
            nsi_message_put_be(p, v, 8);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_put_u1_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint8_t v = d;                                                                                             \
            nsi_message_put_bcopy(p, v, 1);                                                                            \
            r;                                                                                                         \
        })

    #define nsi_message_put_u2_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint16_t v = d;                                                                                            \
            nsi_message_put_le(p, v, 2);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_put_u4_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint32_t v = d;                                                                                            \
            nsi_message_put_le(p, v, 4);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_put_u8_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint64_t v = d;                                                                                            \
            nsi_message_put_le(p, v, 8);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_put_f4_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            float v = d;                                                                                               \
            nsi_message_put_le(p, v, 4);                                                                               \
            r;                                                                                                         \
        })

    #define nsi_message_put_f8_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            double v = d;                                                                                              \
            nsi_message_put_le(p, v, 8);                                                                               \
            r;                                                                                                         \
        })

#else

static NSI_INLINE int nsi_message_put_u1_be(nsi_message_io_t* p, uint8_t d)
{
    int r;

    nsi_message_put_be(p, d, 1);

    return r;
};

static NSI_INLINE int nsi_message_put_u2_be(nsi_message_io_t* p, uint16_t d)
{
    int r;

    nsi_message_put_be(p, d, 2);

    return r;
};

static NSI_INLINE int nsi_message_put_u4_be(nsi_message_io_t* p, uint32_t d)
{
    int r;

    nsi_message_put_be(p, d, 4);

    return r;
};

static NSI_INLINE int nsi_message_put_u8_be(nsi_message_io_t* p, uint64_t d)
{
    int r;

    nsi_message_put_be(p, d, 8);

    return r;
};

static NSI_INLINE int nsi_message_put_f4_be(nsi_message_io_t* p, float d)
{
    int r;

    nsi_message_put_be(p, d, 4);

    return r;
};

static NSI_INLINE int nsi_message_put_f8_be(nsi_message_io_t* p, double d)
{
    int r;

    nsi_message_put_be(p, d, 8);

    return r;
};

static NSI_INLINE int nsi_message_put_u1_le(nsi_message_io_t* p, uint8_t d)
{
    int r;

    nsi_message_put_bcopy(p, d, 1);

    return r;
};

static NSI_INLINE int nsi_message_put_u2_le(nsi_message_io_t* p, uint16_t d)
{
    int r;

    nsi_message_put_le(p, d, 2);

    return r;
};

static NSI_INLINE int nsi_message_put_u4_le(nsi_message_io_t* p, uint32_t d)
{
    int r;

    nsi_message_put_le(p, d, 4);

    return r;
};

static NSI_INLINE int nsi_message_put_u8_le(nsi_message_io_t* p, uint64_t d)
{
    int r;

    nsi_message_put_le(p, d, 8);

    return r;
};

static NSI_INLINE int nsi_message_put_f4_le(nsi_message_io_t* p, float d)
{
    int r;

    nsi_message_put_le(p, d, 4);

    return r;
};

static NSI_INLINE int nsi_message_put_f8_le(nsi_message_io_t* p, double d)
{
    int r;

    nsi_message_put_le(p, d, 8);

    return r;
};

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
