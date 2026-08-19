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
/// @file       npc_message.h
/// @brief
/// @details
/// @date       2022-08-29
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _MESSAGE_H_NPC
#define _MESSAGE_H_NPC

#pragma once

#include "nai/os/nai_socket.h"
#include "npc/core/npc_types.h"
#include "nai/io/nai_buf.h"
#include "nai/runtime/nai_errno.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define NPC_PROTOCOL_VERSION 0x01

#define NPC_DEFAULT_MAJOR 0x00

#define NPC_CRED_PID 1

/**
 * @anchor  NPC_MTYPE
 * @name    NPC_MTYPE           the type of message
 * @{
 */
/** A request expecting a response (even void) */
#define NPC_MT_REQUEST 0x00
/** A fire&forget request */
#define NPC_MT_REQUEST_NO_RETURN 0x01
/** A request of a notification/event callback expecting no response */
#define NPC_MT_NOTIFICATION 0x02
/** The response message */
#define NPC_MT_RESPONSE 0x40
/** The response containing an error */
#define NPC_MT_ERROR 0x41
    /** @} */

#define NPC_CRED_LOCAL   0
#define NPC_CRED_PID     1
#define NPC_CRED_ADDRESS 2
#define NPC_CRED_CERT    3

/**
 * @anchor  NPC_ERROR
 * @name    NPC_ERROR           the error code
 * @{
 */
/** No error occurred */
#define NPC_E_OK 0x00
/** An unspecified error occurred */
#define NPC_E_NOT_OK 0x01
/** The requested Service ID is unknown. */
#define NPC_E_UNKNOWN_SERVICE 0x02
/** The requested Method ID is unknown. Service ID is known. */
#define NPC_E_UNKNOWN_METHOD 0x03
/** Service ID and Method ID are known. Application not running. */
#define NPC_E_NOT_READY 0x04
/** System running the service is not reachable (internal error code only). */
#define NPC_E_NOT_REACHABLE 0x05
/** A timeout occurred (internal error code only). */
#define NPC_E_TIMEOUT 0x06
/** Version of SOME/IP protocol not supported */
#define NPC_E_WRONG_PROTOCOL_VERSION 0x07
/** Interface version mismatch */
#define NPC_E_WRONG_INTERFACE_VERSION 0x08
/** Deserialization error, so that payload cannot be deserialized. */
#define NPC_E_MALFORMED_MESSAGE 0x09
/** An unexpected message type was received (e.g. */
#define NPC_E_WRONG_MESSAGE_TYPE 0x0a
/** @} */

/**
 * @anchor  NPC_MSGLEN
 * @name    NPC_MSGLEN          the length of message header
 * @{
 */
/* the length of message header */
#define NPC_MSGHDR_SIZE (nai_offsetof(npc_msghdr_t, client))
/* the length of message extra */
#define NPC_MSGEXT_SIZE (sizeof(npc_msghdr_t) - NPC_MSGHDR_SIZE)
    /** @} */

#ifndef _NPC_TYPEDEF_MSGHDR_T
    #define _NPC_TYPEDEF_MSGHDR_T
    typedef struct npc_msghdr_s npc_msghdr_t;
#endif
#ifndef _NPC_TYPEDEF_MSGPOOL_T
    #define _NPC_TYPEDEF_MSGPOOL_T
    typedef struct npc_msgpool_s npc_msgpool_t;
#endif
#ifndef _NPC_TYPEDEF_MESSAGE_T
    #define _NPC_TYPEDEF_MESSAGE_T
    typedef struct npc_message_s npc_message_t;
#endif
#ifndef _NPC_TYPEDEF_PORT_T
    #define _NPC_TYPEDEF_PORT_T
    typedef struct npc_port_s npc_port_t;
#endif
    typedef struct npc_cred_s npc_cred_t;

    struct npc_cred_s
    {
        uint8_t type; /**< the cred type */
        uint8_t len;  /**< the length of data, in bytes */
        union
        {
            int pid; /**< the pid */
            union
            {
                nai_sockaddr_t in; /**< the address */
                nai_sockaddr_in4_t in4;
                nai_sockaddr_in6_t in6;
            } addr;
            char cert[28]; /**< the cert name */
            char data[28]; /**< the data */
        };
    };

#define npc_cred_init(c) ((c)->type = 0, (c)->len = 0)

#define npc_cred_copy(d, s)                                                                                            \
    {                                                                                                                  \
        (d)->type = (s)->type;                                                                                         \
        (d)->len  = (s)->len;                                                                                          \
        if ((s)->len > 0) {                                                                                            \
            nai_memcpy((d)->data, (s)->data, (s)->len);                                                                \
        };                                                                                                             \
    }

    /**
 * the structure of the message header
 */
    struct npc_msghdr_s
    {
        /* message id */
        uint16_t serv;   /**< the service id */
        uint16_t method; /**< the method/event id */

        /* message length */
        uint32_t len; /**< the message length, 
                                     exclude member before 'client', 
                                     see #NPC_MSGHDR_SIZE. */

        /* request id */
        uint16_t client;  /**< the client id */
        uint16_t session; /**< the session id */

        /* message infomation */
        uint8_t protocol;  /**< the protocol version */
        uint8_t interface; /**< the interface version */
        uint8_t type : 7;  /**< the message type, see @ref NPC_MTYPE */
        uint8_t unserialized : 1;
        uint8_t code; /**< the result code, see @ref NPC_ERROR */

        /* payload */
        /* uint8_t payload[]; */
    };

    /**
 * the structure of the message
 */
    struct npc_message_s
    {
        nai_list_entry_t ent; /**< the list entry */
        npc_port_t* port;     /**< ipc port */
        npc_message_t* refm;  /**< reference message */
        nai_atomic32_t refc;  /**< reference count */
        npc_inst_t inst;      /**< instance id */
        uint16_t cookie;
        union
        {
            struct
            {
                uint16_t reliable : 1;  /**< is using a reliable channel */
                uint16_t sent : 1;      /**< is a sent message */
                uint16_t reserved : 5;  /**< reserved bits */
                uint16_t unique_id : 8; /**< unique event id */
                uint16_t qindex;        /**< the index of message queue in port */
            };
            uint32_t flags;
        };

        npc_msghdr_t hdr;      /**< the message header */
        nai_buflist_t payload; /**< the buffer list */
        npc_cred_t cred;
    };

    /**
 * dupilcate the message
 * @param   p       pointer to the message
 * @param   mt      whether cloned message will be used by other threads
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    NPC_EXTERN
    npc_message_t* npc_message_dup(npc_message_t* m, int mt);

    /**
 * increase the reference of the message
 * @param   p       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_message_ref(npc_message_t* m);

    /**
 * close the message
 * @param   p       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_message_close(npc_message_t* m);

    //////////////////////////////////////////////////////////////////////////////
    // message io

#ifndef _NPC_TYPEDEF_MESSAGE_IO_T
    #define _NPC_TYPEDEF_MESSAGE_IO_T
    typedef struct npc_message_io_s npc_message_io_t;
#endif

    /**
 * the structure of the io to help read/write the message
 */
    struct npc_message_io_s
    {
        uint8_t* ptr;       /**< pointer to the read/write buffer */
        size_t len;         /**< the left length of the buffer */
        nai_buf_t* buf;     /**< pointer to the current buffer */
        npc_message_t* msg; /**< pointer to the message */
    };

/**
 * initial the  message io
 * @param   p       pointer to the message io
 * @return  void
 */
#define npc_message_io_init(p)                                                                                         \
    {                                                                                                                  \
        (p)->ptr = 0;                                                                                                  \
        (p)->len = 0;                                                                                                  \
        (p)->buf = 0;                                                                                                  \
        (p)->msg = 0;                                                                                                  \
    };

#define npc_message_read_init  npc_message_io_init
#define npc_message_write_init npc_message_io_init

    /**
 * start read the message
 * @param   p       pointer to the message io
 * @param   m       pointer to the message
 * @param   offset  the offset of the message to read
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_message_read_start(npc_message_io_t* p, npc_message_t* m, size_t offset);

    /**
 * stop read the message
 * @param   p       pointer to the message io
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_message_read_end(npc_message_io_t* p);

    /**
 * read data from the message
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to read
 * @param   len     the length of the buffer
 * @retval  >=0     the number of bytes read on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    intptr_t npc_message_read(npc_message_io_t* p, void* buf, size_t len);

    /**
 * read at least the specified number of bytes from the message
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to read
 * @param   len     the length of the buffer
 * @retval  >=0     the number of bytes read on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    intptr_t npc_message_read_least(npc_message_io_t* p, void* buf, size_t len);

    /**
 * skip the number of bytes
 * @param   p       pointer to the message io
 * @param   len     the number of bytes to skip
 * @retval  >=0     the number of bytes skipped on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    intptr_t npc_message_read_skip(npc_message_io_t* p, size_t len);

    /**
 * start write to the end of message
 * @param   p       pointer to the message io
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_message_write_start(npc_message_io_t* p, npc_message_t* m);

    /**
 * stop write the message
 * @param   p       pointer to the message io
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_message_write_end(npc_message_io_t* p);

    /**
 * reserve buffer space of the specified size to the tail
 * @param   p       pointer to the message io
 * @param   len     the specified size
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_message_reserve(npc_message_io_t* p, size_t len);

    /**
 * insert a user buffer to current position
 * @param   p       pointer to the message io
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_message_insert(npc_message_io_t* p, const void* buf, size_t len);

    /**
 * write data to the message
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to write
 * @param   len     the length of the buffer
 * @retval  >=0     the number of bytes written on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    intptr_t npc_message_write(npc_message_io_t* p, const void* buf, size_t len);

    /**
 * over write data to the message
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to write
 * @param   len     the length of the buffer
 * @retval  >=0     the number of bytes written on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    intptr_t npc_message_overwrite(npc_message_io_t* p, const void* buf, size_t len);

    NPC_EXTERN
    void* npc_message_buf_alloc(npc_message_t* m, size_t size);

#define npc_message_get_from_buf(b, type_size)                                                                         \
    ({                                                                                                                 \
        unsigned char* _p = (unsigned char*)(b) + (type_size);                                                         \
        npc_message_t* _m = *((npc_message_t**)(_p + 2));                                                              \
        if (_p[0] != 0 || _p[1] != 0) {                                                                                \
            abort();                                                                                                   \
        }                                                                                                              \
        _m;                                                                                                            \
    })

#define npc_message_unserialized_get(m) ((m)->hdr.unserialized)

//////////////////////////////////////////////////////////////////////////////
// message bcopy

/**
 * copy a byte
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define npc_bcopy_u1(d, s)                                                                                             \
    {                                                                                                                  \
        (d)[0] = (s)[0];                                                                                               \
    };

/**
 * copy a word
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define npc_bcopy_u2(d, s)                                                                                             \
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
#define npc_bcopy_u4(d, s)                                                                                             \
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
#define npc_bcopy_u8(d, s)                                                                                             \
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
#define npc_bswap_u1(d, s)                                                                                             \
    {                                                                                                                  \
        (d)[0] = (s)[0];                                                                                               \
    };

/**
 * byte swap a word
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define npc_bswap_u2(d, s)                                                                                             \
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
#define npc_bswap_u4(d, s)                                                                                             \
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
#define npc_bswap_u8(d, s)                                                                                             \
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
    #define npc_message_get_le npc_message_get_bswap
    #define npc_message_put_le npc_message_put_bswap
    #define npc_message_get_be npc_message_get_bcopy
    #define npc_message_put_be npc_message_put_bcopy
    #define npc_message_put_u2 npc_message_put_u2_be
    #define npc_message_put_u4 npc_message_put_u4_be
    #define npc_message_put_u8 npc_message_put_u8_be
    #define npc_message_put_f4 npc_message_put_f4_be
    #define npc_message_put_f8 npc_message_put_f8_be
    #define npc_message_get_u2 npc_message_get_u2_be
    #define npc_message_get_u4 npc_message_get_u4_be
    #define npc_message_get_u8 npc_message_get_u8_be
    #define npc_message_get_f4 npc_message_get_f4_be
    #define npc_message_get_f8 npc_message_get_f8_be
#else
    #define npc_message_get_le npc_message_get_bcopy
    #define npc_message_put_le npc_message_put_bcopy
    #define npc_message_get_be npc_message_get_bswap
    #define npc_message_put_be npc_message_put_bswap
    #define npc_message_put_u2 npc_message_put_u2_le
    #define npc_message_put_u4 npc_message_put_u4_le
    #define npc_message_put_u8 npc_message_put_u8_le
    #define npc_message_put_f4 npc_message_put_f4_le
    #define npc_message_put_f8 npc_message_put_f8_le
    #define npc_message_get_u2 npc_message_get_u2_le
    #define npc_message_get_u4 npc_message_get_u4_le
    #define npc_message_get_u8 npc_message_get_u8_le
    #define npc_message_get_f4 npc_message_get_f4_le
    #define npc_message_get_f8 npc_message_get_f8_le
#endif

#define npc_message_get_u1 npc_message_get_u1_le
#define npc_message_put_u1 npc_message_put_u1_le

    //////////////////////////////////////////////////////////////////////////////
    // message fast get

#define npc_message_get_bcopy(p, d, s)                                                                                 \
    if ((p)->len >= (s)) {                                                                                             \
        npc_bcopy_u##s((uint8_t*)d, (p)->ptr);                                                                         \
        (p)->ptr += (s);                                                                                               \
        (p)->len -= (s);                                                                                               \
        r = (s);                                                                                                       \
    } else {                                                                                                           \
        r = (int)npc_message_read((p), (d), (s));                                                                      \
        if (r >= 0) {                                                                                                  \
            if (r < (s)) {                                                                                             \
                nai_errno = EOVERFLOW;                                                                                 \
                r         = -1;                                                                                        \
            };                                                                                                         \
        };                                                                                                             \
    };

#define npc_message_get_bswap(p, d, s)                                                                                 \
    if ((p)->len >= (s)) {                                                                                             \
        npc_bswap_u##s((uint8_t*)d, (p)->ptr);                                                                         \
        (p)->ptr += (s);                                                                                               \
        (p)->len -= (s);                                                                                               \
        r = (s);                                                                                                       \
    } else {                                                                                                           \
        uint8_t m[s];                                                                                                  \
        r = (int)npc_message_read((p), (m), (s));                                                                      \
        if (r >= 0) {                                                                                                  \
            if (r < (s)) {                                                                                             \
                nai_errno = EOVERFLOW;                                                                                 \
                r         = -1;                                                                                        \
            } else {                                                                                                   \
                npc_bswap_u##s((uint8_t*)(d), m);                                                                      \
            };                                                                                                         \
        };                                                                                                             \
    };

#if 0 && defined(__GNUC__)

    #define npc_message_get_u1_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            npc_message_get_bcopy(p, d, 1);                                                                            \
            r;                                                                                                         \
        })

    #define npc_message_get_u2_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            npc_message_get_be(p, d, 2);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_get_u4_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            npc_message_get_be(p, d, 4);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_get_u8_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            npc_message_get_be(p, d, 8);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_get_u1_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            npc_message_get_bcopy(p, d, 1);                                                                            \
            r;                                                                                                         \
        })

    #define npc_message_get_u2_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            npc_message_get_le(p, d, 2);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_get_u4_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            npc_message_get_le(p, d, 4);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_get_u8_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            npc_message_get_le(p, d, 8);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_get_f4_be npc_message_get_u4_be
    #define npc_message_get_f8_be npc_message_get_u8_be
    #define npc_message_get_f4_le npc_message_get_u4_le
    #define npc_message_get_f8_le npc_message_get_u8_le

#else

static NPC_INLINE int npc_message_get_u1_be(npc_message_io_t* p, uint8_t* d)
{
    int r;

    npc_message_get_bcopy(p, d, 1);

    return r;
};

static NPC_INLINE int npc_message_get_u2_be(npc_message_io_t* p, uint16_t* d)
{
    int r;

    npc_message_get_be(p, d, 2);

    return r;
};

static NPC_INLINE int npc_message_get_u4_be(npc_message_io_t* p, uint32_t* d)
{
    int r;

    npc_message_get_be(p, d, 4);

    return r;
};

static NPC_INLINE int npc_message_get_u8_be(npc_message_io_t* p, uint64_t* d)
{
    int r;

    npc_message_get_be(p, d, 8);

    return r;
};

static NPC_INLINE int npc_message_get_f4_be(npc_message_io_t* p, float* d)
{
    int r;

    npc_message_get_be(p, d, 4);

    return r;
};

static NPC_INLINE int npc_message_get_f8_be(npc_message_io_t* p, double* d)
{
    int r;

    npc_message_get_be(p, d, 8);

    return r;
};

static NPC_INLINE int npc_message_get_u1_le(npc_message_io_t* p, uint8_t* d)
{
    int r;

    npc_message_get_bcopy(p, d, 1);

    return r;
};

static NPC_INLINE int npc_message_get_u2_le(npc_message_io_t* p, uint16_t* d)
{
    int r;

    npc_message_get_le(p, d, 2);

    return r;
};

static NPC_INLINE int npc_message_get_u4_le(npc_message_io_t* p, uint32_t* d)
{
    int r;

    npc_message_get_le(p, d, 4);

    return r;
};

static NPC_INLINE int npc_message_get_u8_le(npc_message_io_t* p, uint64_t* d)
{
    int r;

    npc_message_get_le(p, d, 8);

    return r;
};

static NPC_INLINE int npc_message_get_f4_le(npc_message_io_t* p, float* d)
{
    int r;

    npc_message_get_le(p, d, 4);

    return r;
};

static NPC_INLINE int npc_message_get_f8_le(npc_message_io_t* p, double* d)
{
    int r;

    npc_message_get_le(p, d, 8);

    return r;
};

#endif

    //////////////////////////////////////////////////////////////////////////////
    // message fast put

#define npc_message_put_bcopy(p, d, s)                                                                                 \
    if ((p)->len >= (s)) {                                                                                             \
        npc_bcopy_u##s((p)->ptr, (uint8_t*)&(d));                                                                      \
        (p)->ptr += (s);                                                                                               \
        (p)->len -= (s);                                                                                               \
        r = (s);                                                                                                       \
    } else {                                                                                                           \
        r = (int)npc_message_write((p), &(d), (s));                                                                    \
    };

#define npc_message_put_bswap(p, d, s)                                                                                 \
    if ((p)->len >= (s)) {                                                                                             \
        npc_bswap_u##s((p)->ptr, (uint8_t*)&(d));                                                                      \
        (p)->ptr += (s);                                                                                               \
        (p)->len -= (s);                                                                                               \
        r = (s);                                                                                                       \
    } else {                                                                                                           \
        uint8_t m[s];                                                                                                  \
        npc_bswap_u##s(m, (uint8_t*)&(d));                                                                             \
        r = (int)npc_message_write((p), (m), (s));                                                                     \
    };

#if 0 && defined(__GNUC__)

    #define npc_message_put_u1_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint8_t v = d;                                                                                             \
            npc_message_put_bcopy(p, v, 1);                                                                            \
            r;                                                                                                         \
        })

    #define npc_message_put_u2_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint16_t v = d;                                                                                            \
            npc_message_put_be(p, v, 2);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_put_u4_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint32_t v = d;                                                                                            \
            npc_message_put_be(p, v, 4);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_put_u8_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint64_t v = d;                                                                                            \
            npc_message_put_be(p, v, 8);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_put_f4_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            float v = d;                                                                                               \
            npc_message_put_be(p, v, 4);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_put_f8_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            double v = d;                                                                                              \
            npc_message_put_be(p, v, 8);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_put_u1_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint8_t v = d;                                                                                             \
            npc_message_put_bcopy(p, v, 1);                                                                            \
            r;                                                                                                         \
        })

    #define npc_message_put_u2_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint16_t v = d;                                                                                            \
            npc_message_put_le(p, v, 2);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_put_u4_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint32_t v = d;                                                                                            \
            npc_message_put_le(p, v, 4);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_put_u8_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint64_t v = d;                                                                                            \
            npc_message_put_le(p, v, 8);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_put_f4_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            float v = d;                                                                                               \
            npc_message_put_le(p, v, 4);                                                                               \
            r;                                                                                                         \
        })

    #define npc_message_put_f8_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            double v = d;                                                                                              \
            npc_message_put_le(p, v, 8);                                                                               \
            r;                                                                                                         \
        })

#else

static NPC_INLINE int npc_message_put_u1_be(npc_message_io_t* p, uint8_t d)
{
    int r;

    npc_message_put_be(p, d, 1);

    return r;
};

static NPC_INLINE int npc_message_put_u2_be(npc_message_io_t* p, uint16_t d)
{
    int r;

    npc_message_put_be(p, d, 2);

    return r;
};

static NPC_INLINE int npc_message_put_u4_be(npc_message_io_t* p, uint32_t d)
{
    int r;

    npc_message_put_be(p, d, 4);

    return r;
};

static NPC_INLINE int npc_message_put_u8_be(npc_message_io_t* p, uint64_t d)
{
    int r;

    npc_message_put_be(p, d, 8);

    return r;
};

static NPC_INLINE int npc_message_put_f4_be(npc_message_io_t* p, float d)
{
    int r;

    npc_message_put_be(p, d, 4);

    return r;
};

static NPC_INLINE int npc_message_put_f8_be(npc_message_io_t* p, double d)
{
    int r;

    npc_message_put_be(p, d, 8);

    return r;
};

static NPC_INLINE int npc_message_put_u1_le(npc_message_io_t* p, uint8_t d)
{
    int r;

    npc_message_put_bcopy(p, d, 1);

    return r;
};

static NPC_INLINE int npc_message_put_u2_le(npc_message_io_t* p, uint16_t d)
{
    int r;

    npc_message_put_le(p, d, 2);

    return r;
};

static NPC_INLINE int npc_message_put_u4_le(npc_message_io_t* p, uint32_t d)
{
    int r;

    npc_message_put_le(p, d, 4);

    return r;
};

static NPC_INLINE int npc_message_put_u8_le(npc_message_io_t* p, uint64_t d)
{
    int r;

    npc_message_put_le(p, d, 8);

    return r;
};

static NPC_INLINE int npc_message_put_f4_le(npc_message_io_t* p, float d)
{
    int r;

    npc_message_put_le(p, d, 4);

    return r;
};

static NPC_INLINE int npc_message_put_f8_le(npc_message_io_t* p, double d)
{
    int r;

    npc_message_put_le(p, d, 8);

    return r;
};

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
