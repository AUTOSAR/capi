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
/// @file       nai_netif.h
/// @brief      network interface
/// @details    this implementation is an encapsulation of network interface and related functions.
/// @date       2023-10-09
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _NETIF_H_NAI
#define _NETIF_H_NAI

#pragma once

#include "nai_socket.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

//////////////////////////////////////////////////////////////////////////////
// network interface

/**
 * @anchor  NAI_IF_TYPE
 * @name    NAI_IF_TYPE         the type of interface entry
 * @{
 */
#define NAI_IF_TYPE_PHYS  0x01 /**< the physical entry */
#define NAI_IF_TYPE_PROTO 0x02 /**< the protocol entry */
/** @} */

/**
 * @anchor  NAI_IF_STAT
 * @name    NAI_IF_STAT         the stat of interface
 * @{
 */
#define NAI_IF_STAT_DOWN 0x00 /**< the down interface */
#define NAI_IF_STAT_UP   0x01 /**< the up interface */
/** @} */

/**
 * @anchor  NAI_IF_OPT
 * @name    NAI_IF_OPT          the option of interface
 * @{
 */
#define NAI_IF_OPT_NO_PHYS  0x01 /**< skip all physical entry */
#define NAI_IF_OPT_NO_PROTO 0x02 /**< skip all protocol entry */
#define NAI_IF_OPT_NO_UP    0x04 /**< skip all up interface */
#define NAI_IF_OPT_NO_DOWN  0x08 /**< skip all down interface */
#define NAI_IF_OPT_NO_NAME  0x10 /**< disable output name */
#define NAI_IF_OPT_NO_INDEX 0x20 /**< disable output index */
#define NAI_IF_OPT_NO_ADDR  0x40 /**< disable output address */
#define NAI_IF_OPT_NO_MTU   0x80 /**< disable output mtu */
/** get the physical interface only */
#define NAI_IF_OPT_PHYS (NAI_IF_OPT_NO_PROTO)
/** get the specified protocol only */
#define NAI_IF_OPT_PROTO(x) (NAI_IF_OPT_NO_PHYS | ((x) << 16))
    /** @} */

#ifndef _NAI_TYPEDEF_IF_ENTRY_T
    #define _NAI_TYPEDEF_IF_ENTRY_T
    typedef struct nai_if_entry_s nai_if_entry_t;
#endif

    struct nai_if_entry_s
    {
        const char* name; /**< pointer to the name string */
        const char* show; /**< pointer to the show name string */
        uint32_t index;   /**< the interface index */
        uint16_t type;    /**< the entry type, see @ref NAI_IF_TYPE */
        uint16_t stat;    /**< the interface stat, see @ref NAI_IF_STAT */
        union
        {
            struct
            {
                uint32_t mtu;    /**< the mtu */
                uint8_t phys[6]; /**< the physical address */
            };
            struct
            {
                nai_sockname_t addr; /**< the address */
                nai_sockname_t mask; /**< the address mask */
            };
        };
    };

/**
 * test whether the entry is valid
 * @param   e       pointer to the if entry
 * @return  if it is valid, return 1, otherwise return 0
 */
#define nai_if_is_valid(e) ((e)->type != 0)

    /**
 * lookup the interface and return a list of interface entries
 * @param   index   the specified interface index, 0 means all
 * @param   opt     the option of interface, see @ref NAI_IF_OPT
 * @param   buf     pointer to list buffer
 * @param   buflen  the length of list buffer
 * @retval  >=0     the length of required is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the return value greater than buflen, 
 *          the content is truncated and the error code is set to ERANGE
 */
    NAI_EXTERN
    intptr_t nai_if_list(uint32_t index, nai_int_t opt, void* buf, size_t buflen);

    /**
 * get the mtu size of the specified interface
 * @param   index   the specified interface index
 * @retval  >=0     the value of mtu size is returned
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_if_get_mtu(uint32_t index);

    /**
 * mapping network interface address to index
 * @param   name    pointer to the local address
 * @param   namelen the length of local address
 * @param   pindex  pointer to the index
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_if_addrtoindex(const nai_sockaddr_t* name, nai_int_t namelen, uint32_t* pindex);

    /**
 * mapping network interface address to index
 * @param   name    pointer to the local address
 * @param   namelen the length of local address
 * @param   buf     pointer to the buffer
 * @param   len     the length of buffer
 * @retval  >=0     the length of output without null-terminated is returned
 * @retval  <0      an error occurred, see #nai_errno
 * @note    this function do not write the content into the buffer 
 *          when 'len' is less than the returned value.
 */
    NAI_EXTERN
    intptr_t nai_if_addrtoname(const nai_sockaddr_t* name, nai_int_t namelen, char* buf, size_t len);

    /**
 * mapping network interface name to index
 * @param   buf     pointer to the name
 * @param   len     the length of name, <0 means null-terminated string
 * @param   pindex  pointer to the index
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_if_nametoindex(const char* buf, size_t len, uint32_t* pindex);

    /**
 * mapping network interface index to name
 * @param   index   network interface index
 * @param   buf     pointer to the buffer
 * @param   len     the length of buffer
 * @param   name_only will fail if the device does not exist
 * @retval  >=0     the length of output without null-terminated is returned
 * @retval  <0      an error occurred, see #nai_errno
 * @note    this function do not write the content into the buffer 
 *          when 'len' is less than the returned value.
 */
    NAI_EXTERN
    intptr_t nai_if_indextoname(uint32_t index, char* buf, size_t len, nai_int_t name_only);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
