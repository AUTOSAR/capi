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
/// @file       icc_message.h
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _MESSAGE_H_ICC
#define _MESSAGE_H_ICC

#pragma once

#include "icc/core/icc_types.h"
#include "nai/io/nai_buf.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_errno.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // message header

#define ICC_PROTOCOL_VERSION          0x00
#define ICC_DEFAULT_MAJOR             0x00
#define ICC_E_OK                      0x00
#define ICC_E_NOT_OK                  0x01
#define ICC_E_UNKNOWN_SERVICE         0x02
#define ICC_E_UNKNOWN_METHOD          0x03
#define ICC_E_NOT_READY               0x04
#define ICC_E_NOT_REACHABLE           0x05
#define ICC_E_TIMEOUT                 0x06
#define ICC_E_WRONG_PROTOCOL_VERSION  0x07
#define ICC_E_WRONG_INTERFACE_VERSION 0x08
#define ICC_E_MALFORMED_MESSAGE       0x09
#define ICC_E_WRONG_MESSAGE_TYPE      0x0a

#pragma pack(push, 1)

#ifndef _ICC_TYPEDEF_MSGHDR_T
    #define _ICC_TYPEDEF_MSGHDR_T
    typedef struct icc_msghdr_s icc_msghdr_t;
#endif

    /**
 * the structure of the message header
 */
    struct icc_msghdr_s
    {
        /* message id */
        uint16_t serv;   /**< the service id */
        uint16_t method; /**< the method/event id */

        /* message length */
        uint32_t len; /**< the message length */

        /* request id */
        uint16_t client;  /**< the client id */
        uint16_t session; /**< the session id */

        /* message infomation */
        uint8_t protocol;  /**< the protocol version */
        uint8_t interface; /**< the interface version */
        uint8_t type;      /**< the message type */
        uint8_t code;      /**< the result code */

        /* payload */
        /* uint8_t payload[]; */
    };

#pragma pack(pop)

/** A request expecting a response (even void) */
#define ICC_MT_REQUEST 0x00
/** A fire&forget request */
#define ICC_MT_REQUEST_NO_RETURN 0x01
/** A request of a notification/event callback expecting no response */
#define ICC_MT_NOTIFICATION 0x02
/** The response message */
#define ICC_MT_RESPONSE 0x80
/** The response containing an error */
#define ICC_MT_ERROR 0x81

    //////////////////////////////////////////////////////////////////////////////
    // message

#ifndef _ICC_TYPEDEF_MSGPOOL_T
    #define _ICC_TYPEDEF_MSGPOOL_T
    typedef struct icc_msgpool_s icc_msgpool_t;
#endif
#ifndef _ICC_TYPEDEF_MESSAGE_T
    #define _ICC_TYPEDEF_MESSAGE_T
    typedef struct icc_message_s icc_message_t;
#endif

    /**
 * the structure of the message pool
 */
    struct icc_msgpool_s
    {
        nai_spin_t* lock;      /**< pointer to the spin lock */
        nai_list_entry_t list; /**< the list of free messages */
        size_t size;           /**< the allocate size of the buffer pool */
        size_t alloc;          /**< the number of allocated messages */
    };

    /**
 * the structure of the message
 */
    struct icc_message_s
    {
        nai_list_entry_t ent; /**< the list entry */
        icc_msgpool_t* pool;  /**< pointer to the message pool */
        icc_message_t* refm;  /**< pointer to the reference message */
        nai_atomic32_t refc;  /**< reference count */
        icc_inst_t inst;      /**< instance id */
        union
        {
            struct
            {
                uint32_t reserved : 8; /**< reserved bits */
            };
            uint32_t flags;
        };
        icc_msghdr_t hdr;      /**< the message header */
        nai_buflist_t payload; /**< the buffer list */
    };

    ICC_EXTERN uint16_t icc_ntohs_ua(const uint16_t* p);
    ICC_EXTERN uint32_t icc_ntohl_ua(const uint32_t* p);
    ICC_EXTERN uint16_t icc_htons_ua(const uint16_t* p);
    ICC_EXTERN uint32_t icc_htonl_ua(const uint32_t* p);

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
    ICC_EXTERN
    int icc_msgpool_init(icc_msgpool_t* p, size_t size, nai_spin_t* lock);

    /**
 * close the message pool
 * @param   p       pointer to the message pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_msgpool_close(icc_msgpool_t* p);

    /**
 * create a new meesage
 * @param   p       pointer to the message pool
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    ICC_EXTERN
    icc_message_t* icc_message_create(icc_msgpool_t* p);

    /**
 * dupilcate the message
 * @param   m       pointer to the message
 * @param   mt      whether cloned message will be used by other threads
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    ICC_EXTERN
    icc_message_t* icc_message_dup(icc_message_t* m, int mt);

    /**
 * increase the reference of the message
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_message_ref(icc_message_t* m);

    /**
 * close the message
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_message_close(icc_message_t* m);

    //////////////////////////////////////////////////////////////////////////////
    // message io

#ifndef _ICC_TYPEDEF_MESSAGE_IO_T
    #define _ICC_TYPEDEF_MESSAGE_IO_T
    typedef struct icc_message_io_s icc_message_io_t;
#endif

    /**
 * the structure of the io to help read/write the message
 */
    struct icc_message_io_s
    {
        uint8_t* ptr;       /**< pointer to the read/write buffer */
        size_t len;         /**< the left length of the buffer */
        nai_buf_t* buf;     /**< pointer to the current buffer */
        icc_message_t* msg; /**< pointer to the message */
    };

/**
 * initial the  message io
 * @param   p       pointer to the message io
 * @return  void
 */
#define icc_message_io_init(p)                                                                                         \
    {                                                                                                                  \
        (p)->ptr = 0;                                                                                                  \
        (p)->len = 0;                                                                                                  \
        (p)->buf = 0;                                                                                                  \
        (p)->msg = 0;                                                                                                  \
    };

#define icc_message_read_init  icc_message_io_init
#define icc_message_write_init icc_message_io_init

    /**
 * start read the message
 * @param   p       pointer to the message io
 * @param   m       pointer to the message
 * @param   offset  the offset of the message to read
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_message_read_start(icc_message_io_t* p, icc_message_t* m, size_t offset);

    /**
 * stop read the message
 * @param   p       pointer to the message io
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_message_read_end(icc_message_io_t* p);

    /**
 * read data from the message
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to read
 * @param   len     the length of the buffer
 * @retval  >=0     the number of bytes read on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    intptr_t icc_message_read(icc_message_io_t* p, void* buf, size_t len);

    /**
 * read at least the specified number of bytes from the message
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to read
 * @param   len     the length of the buffer
 * @retval  >=0     the number of bytes read on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    intptr_t icc_message_read_least(icc_message_io_t* p, void* buf, size_t len);

    /**
 * skip the number of bytes
 * @param   p       pointer to the message io
 * @param   len     the number of bytes to skip
 * @retval  >=0     the number of bytes skipped on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    intptr_t icc_message_read_skip(icc_message_io_t* p, size_t len);

    /**
 * start write to the end of message
 * @param   p       pointer to the message io
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_message_write_start(icc_message_io_t* p, icc_message_t* m);

    /**
 * stop write the message
 * @param   p       pointer to the message io
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_message_write_end(icc_message_io_t* p);

    /**
 * reserve buffer space of the specified size to the tail
 * @param   p       pointer to the message io
 * @param   len     the specified size
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_message_reserve(icc_message_io_t* p, size_t len);

    /**
 * insert a user buffer to current position
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to insert
 * @param   len     the length of the buffer
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_message_insert(icc_message_io_t* p, const void* buf, size_t len);

    /**
 * write data to the message
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to write
 * @param   len     the length of the buffer
 * @retval  >=0     the number of bytes written on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    intptr_t icc_message_write(icc_message_io_t* p, const void* buf, size_t len);

    /**
 * over write data to the message
 * @param   p       pointer to the message io
 * @param   buf     pointer to the buffer to write
 * @param   len     the length of the buffer
 * @retval  >=0     the number of bytes written on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    intptr_t icc_message_overwrite(icc_message_io_t* p, const void* buf, size_t len);

//////////////////////////////////////////////////////////////////////////////
// message bcopy

/**
 * copy a byte
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define icc_bcopy_u1(d, s)                                                                                             \
    {                                                                                                                  \
        (d)[0] = (s)[0];                                                                                               \
    };

/**
 * copy a word
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define icc_bcopy_u2(d, s)                                                                                             \
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
#define icc_bcopy_u4(d, s)                                                                                             \
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
#define icc_bcopy_u8(d, s)                                                                                             \
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
#define icc_bswap_u1(d, s)                                                                                             \
    {                                                                                                                  \
        (d)[0] = (s)[0];                                                                                               \
    };

/**
 * byte swap a word
 * @param   d       pointer to the dest
 * @param   s       pointer to the source
 * @return  void
 */
#define icc_bswap_u2(d, s)                                                                                             \
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
#define icc_bswap_u4(d, s)                                                                                             \
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
#define icc_bswap_u8(d, s)                                                                                             \
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
    #define icc_message_get_le icc_message_get_bswap
    #define icc_message_put_le icc_message_put_bswap
    #define icc_message_get_be icc_message_get_bcopy
    #define icc_message_put_be icc_message_put_bcopy
    #define icc_message_put_u2 icc_message_put_u2_be
    #define icc_message_put_u4 icc_message_put_u4_be
    #define icc_message_put_u8 icc_message_put_u8_be
    #define icc_message_put_f4 icc_message_put_f4_be
    #define icc_message_put_f8 icc_message_put_f8_be
    #define icc_message_get_u2 icc_message_get_u2_be
    #define icc_message_get_u4 icc_message_get_u4_be
    #define icc_message_get_u8 icc_message_get_u8_be
    #define icc_message_get_f4 icc_message_get_f4_be
    #define icc_message_get_f8 icc_message_get_f8_be
#else
    #define icc_message_get_le icc_message_get_bcopy
    #define icc_message_put_le icc_message_put_bcopy
    #define icc_message_get_be icc_message_get_bswap
    #define icc_message_put_be icc_message_put_bswap
    #define icc_message_put_u2 icc_message_put_u2_le
    #define icc_message_put_u4 icc_message_put_u4_le
    #define icc_message_put_u8 icc_message_put_u8_le
    #define icc_message_put_f4 icc_message_put_f4_le
    #define icc_message_put_f8 icc_message_put_f8_le
    #define icc_message_get_u2 icc_message_get_u2_le
    #define icc_message_get_u4 icc_message_get_u4_le
    #define icc_message_get_u8 icc_message_get_u8_le
    #define icc_message_get_f4 icc_message_get_f4_le
    #define icc_message_get_f8 icc_message_get_f8_le
#endif

#define icc_message_get_u1 icc_message_get_u1_le
#define icc_message_put_u1 icc_message_put_u1_le

    //////////////////////////////////////////////////////////////////////////////
    // message fast get

#define icc_message_get_bcopy(p, d, s)                                                                                 \
    if ((p)->len >= (s)) {                                                                                             \
        icc_bcopy_u##s((uint8_t*)d, (p)->ptr);                                                                         \
        (p)->ptr += (s);                                                                                               \
        (p)->len -= (s);                                                                                               \
        r = (s);                                                                                                       \
    } else {                                                                                                           \
        r = (int)icc_message_read((p), (d), (s));                                                                      \
        if (r >= 0) {                                                                                                  \
            if (r < (s)) {                                                                                             \
                nai_errno = EOVERFLOW;                                                                                 \
                r         = -1;                                                                                        \
            };                                                                                                         \
        };                                                                                                             \
    };

#define icc_message_get_bswap(p, d, s)                                                                                 \
    if ((p)->len >= (s)) {                                                                                             \
        icc_bswap_u##s((uint8_t*)d, (p)->ptr);                                                                         \
        (p)->ptr += (s);                                                                                               \
        (p)->len -= (s);                                                                                               \
        r = (s);                                                                                                       \
    } else {                                                                                                           \
        uint8_t m[s];                                                                                                  \
        r = (int)icc_message_read((p), (m), (s));                                                                      \
        if (r >= 0) {                                                                                                  \
            if (r < (s)) {                                                                                             \
                nai_errno = EOVERFLOW;                                                                                 \
                r         = -1;                                                                                        \
            } else {                                                                                                   \
                icc_bswap_u##s((uint8_t*)(d), m);                                                                      \
            };                                                                                                         \
        };                                                                                                             \
    };

#if 0 && defined(__GNUC__)

    #define icc_message_get_u1_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            icc_message_get_bcopy(p, d, 1);                                                                            \
            r;                                                                                                         \
        })

    #define icc_message_get_u2_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            icc_message_get_be(p, d, 2);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_get_u4_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            icc_message_get_be(p, d, 4);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_get_u8_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            icc_message_get_be(p, d, 8);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_get_u1_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            icc_message_get_bcopy(p, d, 1);                                                                            \
            r;                                                                                                         \
        })

    #define icc_message_get_u2_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            icc_message_get_le(p, d, 2);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_get_u4_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            icc_message_get_le(p, d, 4);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_get_u8_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            icc_message_get_le(p, d, 8);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_get_f4_be icc_message_get_u4_be
    #define icc_message_get_f8_be icc_message_get_u8_be
    #define icc_message_get_f4_le icc_message_get_u4_le
    #define icc_message_get_f8_le icc_message_get_u8_le

#else

static ICC_INLINE int icc_message_get_u1_be(icc_message_io_t* p, uint8_t* d)
{
    int r;

    icc_message_get_bcopy(p, d, 1);

    return r;
};

static ICC_INLINE int icc_message_get_u2_be(icc_message_io_t* p, uint16_t* d)
{
    int r;

    icc_message_get_be(p, d, 2);

    return r;
};

static ICC_INLINE int icc_message_get_u4_be(icc_message_io_t* p, uint32_t* d)
{
    int r;

    icc_message_get_be(p, d, 4);

    return r;
};

static ICC_INLINE int icc_message_get_u8_be(icc_message_io_t* p, uint64_t* d)
{
    int r;

    icc_message_get_be(p, d, 8);

    return r;
};

static ICC_INLINE int icc_message_get_f4_be(icc_message_io_t* p, float* d)
{
    int r;

    icc_message_get_be(p, d, 4);

    return r;
};

static ICC_INLINE int icc_message_get_f8_be(icc_message_io_t* p, double* d)
{
    int r;

    icc_message_get_be(p, d, 8);

    return r;
};

static ICC_INLINE int icc_message_get_u1_le(icc_message_io_t* p, uint8_t* d)
{
    int r;

    icc_message_get_bcopy(p, d, 1);

    return r;
};

static ICC_INLINE int icc_message_get_u2_le(icc_message_io_t* p, uint16_t* d)
{
    int r;

    icc_message_get_le(p, d, 2);

    return r;
};

static ICC_INLINE int icc_message_get_u4_le(icc_message_io_t* p, uint32_t* d)
{
    int r;

    icc_message_get_le(p, d, 4);

    return r;
};

static ICC_INLINE int icc_message_get_u8_le(icc_message_io_t* p, uint64_t* d)
{
    int r;

    icc_message_get_le(p, d, 8);

    return r;
};

static ICC_INLINE int icc_message_get_f4_le(icc_message_io_t* p, float* d)
{
    int r;

    icc_message_get_le(p, d, 4);

    return r;
};

static ICC_INLINE int icc_message_get_f8_le(icc_message_io_t* p, double* d)
{
    int r;

    icc_message_get_le(p, d, 8);

    return r;
};

#endif

    //////////////////////////////////////////////////////////////////////////////
    // message fast put

#define icc_message_put_bcopy(p, d, s)                                                                                 \
    if ((p)->len >= (s)) {                                                                                             \
        icc_bcopy_u##s((p)->ptr, (uint8_t*)&(d));                                                                      \
        (p)->ptr += (s);                                                                                               \
        (p)->len -= (s);                                                                                               \
        r = (s);                                                                                                       \
    } else {                                                                                                           \
        r = (int)icc_message_write((p), &(d), (s));                                                                    \
    };

#define icc_message_put_bswap(p, d, s)                                                                                 \
    if ((p)->len >= (s)) {                                                                                             \
        icc_bswap_u##s((p)->ptr, (uint8_t*)&(d));                                                                      \
        (p)->ptr += (s);                                                                                               \
        (p)->len -= (s);                                                                                               \
        r = (s);                                                                                                       \
    } else {                                                                                                           \
        uint8_t m[s];                                                                                                  \
        icc_bswap_u##s(m, (uint8_t*)&(d));                                                                             \
        r = (int)icc_message_write((p), (m), (s));                                                                     \
    };

#if 0 && defined(__GNUC__)

    #define icc_message_put_u1_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint8_t v = d;                                                                                             \
            icc_message_put_bcopy(p, v, 1);                                                                            \
            r;                                                                                                         \
        })

    #define icc_message_put_u2_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint16_t v = d;                                                                                            \
            icc_message_put_be(p, v, 2);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_put_u4_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint32_t v = d;                                                                                            \
            icc_message_put_be(p, v, 4);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_put_u8_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint64_t v = d;                                                                                            \
            icc_message_put_be(p, v, 8);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_put_f4_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            float v = d;                                                                                               \
            icc_message_put_be(p, v, 4);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_put_f8_be(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            double v = d;                                                                                              \
            icc_message_put_be(p, v, 8);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_put_u1_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint8_t v = d;                                                                                             \
            icc_message_put_bcopy(p, v, 1);                                                                            \
            r;                                                                                                         \
        })

    #define icc_message_put_u2_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint16_t v = d;                                                                                            \
            icc_message_put_le(p, v, 2);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_put_u4_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint32_t v = d;                                                                                            \
            icc_message_put_le(p, v, 4);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_put_u8_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            uint64_t v = d;                                                                                            \
            icc_message_put_le(p, v, 8);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_put_f4_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            float v = d;                                                                                               \
            icc_message_put_le(p, v, 4);                                                                               \
            r;                                                                                                         \
        })

    #define icc_message_put_f8_le(p, d)                                                                                \
        ({                                                                                                             \
            int r;                                                                                                     \
            double v = d;                                                                                              \
            icc_message_put_le(p, v, 8);                                                                               \
            r;                                                                                                         \
        })

#else

static ICC_INLINE int icc_message_put_u1_be(icc_message_io_t* p, uint8_t d)
{
    int r;

    icc_message_put_be(p, d, 1);

    return r;
};

static ICC_INLINE int icc_message_put_u2_be(icc_message_io_t* p, uint16_t d)
{
    int r;

    icc_message_put_be(p, d, 2);

    return r;
};

static ICC_INLINE int icc_message_put_u4_be(icc_message_io_t* p, uint32_t d)
{
    int r;

    icc_message_put_be(p, d, 4);

    return r;
};

static ICC_INLINE int icc_message_put_u8_be(icc_message_io_t* p, uint64_t d)
{
    int r;

    icc_message_put_be(p, d, 8);

    return r;
};

static ICC_INLINE int icc_message_put_f4_be(icc_message_io_t* p, float d)
{
    int r;

    icc_message_put_be(p, d, 4);

    return r;
};

static ICC_INLINE int icc_message_put_f8_be(icc_message_io_t* p, double d)
{
    int r;

    icc_message_put_be(p, d, 8);

    return r;
};

static ICC_INLINE int icc_message_put_u1_le(icc_message_io_t* p, uint8_t d)
{
    int r;

    icc_message_put_bcopy(p, d, 1);

    return r;
};

static ICC_INLINE int icc_message_put_u2_le(icc_message_io_t* p, uint16_t d)
{
    int r;

    icc_message_put_le(p, d, 2);

    return r;
};

static ICC_INLINE int icc_message_put_u4_le(icc_message_io_t* p, uint32_t d)
{
    int r;

    icc_message_put_le(p, d, 4);

    return r;
};

static ICC_INLINE int icc_message_put_u8_le(icc_message_io_t* p, uint64_t d)
{
    int r;

    icc_message_put_le(p, d, 8);

    return r;
};

static ICC_INLINE int icc_message_put_f4_le(icc_message_io_t* p, float d)
{
    int r;

    icc_message_put_le(p, d, 4);

    return r;
};

static ICC_INLINE int icc_message_put_f8_le(icc_message_io_t* p, double d)
{
    int r;

    icc_message_put_le(p, d, 8);

    return r;
};

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
