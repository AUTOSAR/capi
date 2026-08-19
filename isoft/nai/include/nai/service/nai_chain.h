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
/// @file       nai_chain.h
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _CHAIN_H_NAI
#define _CHAIN_H_NAI

#pragma once

#include "nai/runtime/nai_list.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_CHAIN_T
    #define _NAI_TYPEDEF_CHAIN_T
    typedef struct nai_chain_s nai_chain_t;
#endif
#ifndef _NAI_TYPEDEF_BUFLIST_T
    #define _NAI_TYPEDEF_BUFLIST_T
    typedef struct nai_buflist_s nai_buflist_t;
#endif
#ifndef _NAI_TYPEDEF_FILTER_F
    #define _NAI_TYPEDEF_FILTER_F
    typedef intptr_t (*nai_filter_f)(nai_chain_t* n, void*, nai_buflist_t* l, size_t limit);
#endif

    /**
 * the sturcture of the io chain
 */
    struct nai_chain_s
    {
        nai_list_entry_t ent; /**< the entry of the chain list */
        nai_filter_f handle;  /**< the handler of the chain */
        const char* name;     /**< the name of the chain */
        nai_int_t priority;   /**< the priority of the chain */
    };

/** 
 * @name    nai_chain_defines   the functions of chain
 * @{
 */

/**
 * initial the chain
 * @param   c       pointer to the chain
 * @param   n       pointer to the string of the chain name
 * @param   h       the handler of the chain
 * @param   p       the priority of the chain
 * @return  void
 */
#define nai_chain_init(c, n, h, p)                                                                                     \
    {                                                                                                                  \
        nai_list_init(&(c)->ent);                                                                                      \
        (c)->name     = (n);                                                                                           \
        (c)->handle   = (h);                                                                                           \
        (c)->priority = (p);                                                                                           \
    }

/**
 * call the handler of this chain
 * @param   c       pointer to the chain
 * @param   p       pointer to the context
 * @param   l       pointer to the buffer list
 * @param   limit   the limit of transmit bytes
 * @retval  >=0     the bytes of trasmited
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nai_chain_this(c, p, l, limit) ((c)->handle((c), (p), (l), (limit)))

/**
 * call the handler of next chain
 * @param   c       pointer to the chain
 * @param   p       pointer to the context
 * @param   l       pointer to the buffer list
 * @param   limit   the limit of transmit bytes
 * @retval  >=0     the bytes of trasmited
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nai_chain_next(c, p, l, limit) ((c)->handle((nai_chain_t*)(c)->ent.next, (p), (l), (limit)))

    /** @} */

    /**
 * link a chain node to the chain
 * @param   c       pointer to the chain
 * @param   node    pointer to the chain node
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_chain_link(nai_chain_t* c, nai_chain_t* node);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
