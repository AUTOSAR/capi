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
/// @date       2025-03-07
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _MESSAGE_H_NPC
#define _MESSAGE_H_NPC


#pragma once

#include "nsomeip/net/nsi_message.h"
#include "nai/os/nai_socket.h"
#include "nai/io/nai_buf.h"
#include "nai/runtime/nai_errno.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef _NPC_TYPEDEF_MESSAGE_T
#define _NPC_TYPEDEF_MESSAGE_T
typedef struct nsi_message_s npc_message_t;
#endif

#ifndef _NPC_TYPEDEF_MSGHDR_T
#define _NPC_TYPEDEF_MSGHDR_T
typedef struct nsi_msghdr_s  npc_msghdr_t;
#endif


/**
 * dupilcate the message
 * @param   p       pointer to the message
 * @param   mt      whether cloned message will be used by other threads
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
NSI_EXTERN
npc_message_t* nsi_npc_message_dup(npc_message_t* m, int mt);


/**
 * close the message
 * @param   p       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
NSI_EXTERN
int nsi_npc_message_close(npc_message_t* m);



//////////////////////////////////////////////////////////////////////////////
// message io

#ifndef _NPC_TYPEDEF_MESSAGE_IO_T
#define _NPC_TYPEDEF_MESSAGE_IO_T
typedef struct nsi_message_io_s npc_message_io_t;
#endif

#define npc_message_io_init         nsi_message_io_init
#define npc_message_read_init       npc_message_io_init
#define npc_message_write_init      npc_message_io_init
#define npc_message_read_start      nsi_message_read_start
#define npc_message_read_end        nsi_message_read_end
#define npc_message_read            nsi_message_read
#define npc_message_read_least      nsi_message_read_least
#define npc_message_read_skip       nsi_message_read_skip
#define npc_message_write_start     nsi_message_write_start
#define npc_message_write_end       nsi_message_write_end
#define npc_message_reserve         nsi_message_reserve
#define npc_message_insert          nsi_message_insert
#define npc_message_write           nsi_message_write
#define npc_message_overwrite       nsi_message_overwrite

#define npc_cred_init               nsi_cred_init

#define npc_message_get_from_buf(b, type_size)       ({\
    unsigned char *_p = (unsigned char *)(b) + (type_size);\
    npc_message_t *_m = *((npc_message_t **)(_p + 2));\
    if (_p[0] != 0 || _p[1] != 0) {\
        abort();\
    }\
    _m;\
})

//#define npc_message_unserialized_get(m)         ((m)->hdr.unserialized)

//////////////////////////////////////////////////////////////////////////////
// message bcopy

#define npc_bcopy_u1    nsi_bcopy_u1
#define npc_bcopy_u2    nsi_bcopy_u2
#define npc_bcopy_u4    nsi_bcopy_u4
#define npc_bcopy_u8    nsi_bcopy_u8
#define npc_bswap_u1    nsi_bswap_u1
#define npc_bswap_u2    nsi_bswap_u2
#define npc_bswap_u4    nsi_bswap_u4
#define npc_bswap_u8    nsi_bswap_u8

#if defined(NAI_HAVE_BIG_ENDIAN)
#define npc_message_get_le nsi_message_get_bswap
#define npc_message_put_le nsi_message_put_bswap
#define npc_message_get_be nsi_message_get_bcopy
#define npc_message_put_be nsi_message_put_bcopy
#define npc_message_put_u2 nsi_message_put_u2_be
#define npc_message_put_u4 nsi_message_put_u4_be
#define npc_message_put_u8 nsi_message_put_u8_be
#define npc_message_put_f4 nsi_message_put_f4_be
#define npc_message_put_f8 nsi_message_put_f8_be
#define npc_message_get_u2 nsi_message_get_u2_be
#define npc_message_get_u4 nsi_message_get_u4_be
#define npc_message_get_u8 nsi_message_get_u8_be
#define npc_message_get_f4 nsi_message_get_f4_be
#define npc_message_get_f8 nsi_message_get_f8_be
#else
#define npc_message_get_le nsi_message_get_bcopy
#define npc_message_put_le nsi_message_put_bcopy
#define npc_message_get_be nsi_message_get_bswap
#define npc_message_put_be nsi_message_put_bswap
#define npc_message_put_u2 nsi_message_put_u2_le
#define npc_message_put_u4 nsi_message_put_u4_le
#define npc_message_put_u8 nsi_message_put_u8_le
#define npc_message_put_f4 nsi_message_put_f4_le
#define npc_message_put_f8 nsi_message_put_f8_le
#define npc_message_get_u2 nsi_message_get_u2_le
#define npc_message_get_u4 nsi_message_get_u4_le
#define npc_message_get_u8 nsi_message_get_u8_le
#define npc_message_get_f4 nsi_message_get_f4_le
#define npc_message_get_f8 nsi_message_get_f8_le
#endif

#define npc_message_get_u1 nsi_message_get_u1_le
#define npc_message_put_u1 nsi_message_put_u1_le

#ifdef __cplusplus
};
#endif  /* __cplusplus */

#endif
