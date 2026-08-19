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
//
// Melon source attribution
//
// Portions of this file are derived from Melon source code.
//
// Reference upstream source:
// https://github.com/Water-Melon/Melon/blob/b3513605e578fbfd92aa2ef2f2f01803671136b6/include/mln_utils.h
//
// Applicable copyright notice for the Melon-derived portions:
// Copyright (C) Niklaus F.Schen.
//
// The Melon-derived portions are licensed under the BSD 3-Clause License.
// A copy of the license is provided in:
// LICENSES/LICENSE-Melon.txt
//
// The Melon-derived portions have been modified for integration into the
// CAPI codebase. In particular, selected definitions were extracted,
// identifiers were renamed from MLN/mln to CPI/cpi, and project-specific
// types and includes were added.
//
// Nothing in the AUTOSAR notice above is intended to restrict the rights
// granted under the BSD 3-Clause License for the Melon-derived portions.
//
// --------------------------------------------------------------------------


/// ================================================================
///
/// File description:
/// ----------------
/// @file       cpi_types.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_TYPES_H
#define __CPI_TYPES_H

#include "npc/core/npc_message.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Chain
 */
#define CPI_CHAIN_FUNC_DECLARE(prefix, type, ret_attr, func_attr)                                                      \
    ;                                                                                                                  \
    ret_attr prefix##_chain_add(type **head, type **tail, type *node) func_attr;                                       \
    ret_attr prefix##_chain_del(type **head, type **tail, type *node) func_attr;

#define CPI_CHAIN_FUNC_DEFINE(prefix, type, ret_attr, prev_ptr, next_ptr)                                              \
    ;                                                                                                                  \
    ret_attr prefix##_chain_add(type **head, type **tail, type *node)                                                  \
    {                                                                                                                  \
        if (head == NULL || tail == NULL || node == NULL)                                                              \
            return;                                                                                                    \
        node->prev_ptr = node->next_ptr = NULL;                                                                        \
        if (*head == NULL) {                                                                                           \
            *head = *tail = node;                                                                                      \
            return;                                                                                                    \
        }                                                                                                              \
        (*tail)->next_ptr = node;                                                                                      \
        node->prev_ptr    = (*tail);                                                                                   \
        *tail             = node;                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    ret_attr prefix##_chain_del(type **head, type **tail, type *node)                                                  \
    {                                                                                                                  \
        if (head == NULL || tail == NULL || node == NULL)                                                              \
            return;                                                                                                    \
        if (*head == node) {                                                                                           \
            if (*tail == node) {                                                                                       \
                *head = *tail = NULL;                                                                                  \
            } else {                                                                                                   \
                *head             = node->next_ptr;                                                                    \
                (*head)->prev_ptr = NULL;                                                                              \
            }                                                                                                          \
        } else {                                                                                                       \
            if (*tail == node) {                                                                                       \
                *tail             = node->prev_ptr;                                                                    \
                (*tail)->next_ptr = NULL;                                                                              \
            } else {                                                                                                   \
                node->prev_ptr->next_ptr = node->next_ptr;                                                             \
                node->next_ptr->prev_ptr = node->prev_ptr;                                                             \
            }                                                                                                          \
        }                                                                                                              \
        node->prev_ptr = node->next_ptr = NULL;                                                                        \
    }

    typedef unsigned char cpi_u8_t;
    typedef char cpi_s8_t;
    typedef unsigned short cpi_u16_t;
    typedef short cpi_s16_t;
    typedef unsigned int cpi_u32_t;
    typedef int cpi_s32_t;
#if defined(i386) || defined(__arm__) || defined(WIN32)
    typedef unsigned long long cpi_u64_t;
    typedef long long cpi_s64_t;
#else
typedef unsigned long cpi_u64_t;
typedef long cpi_s64_t;
#endif
    typedef char *cpi_s8ptr_t;
    typedef unsigned char *cpi_u8ptr_t;
    typedef short *cpi_s16ptr_t;
    typedef unsigned short *cpi_u16ptr_t;
    typedef int *cpi_s32ptr_t;
    typedef unsigned int *cpi_u32ptr_t;
#if defined(i386) || defined(__arm__) || defined(WIN32)
    typedef long long *cpi_s64ptr_t;
    typedef unsigned long long *cpi_u64ptr_t;
#else
typedef long *cpi_s64ptr_t;
typedef unsigned long *cpi_u64ptr_t;
#endif
    typedef size_t cpi_size_t;
    typedef off_t cpi_off_t;
#if defined(WIN32)
    #if defined(i386) || defined(__arm__)
    typedef long cpi_sptr_t;
    typedef unsigned long cpi_uptr_t;
    typedef long cpi_sauto_t;
    typedef unsigned long cpi_uauto_t;
    #else
    typedef long long cpi_sptr_t;
    typedef unsigned long long cpi_uptr_t;
    typedef long long cpi_sauto_t;
    typedef unsigned long long cpi_uauto_t;
    #endif
#else
typedef long cpi_sptr_t;
typedef unsigned long cpi_uptr_t;
typedef long cpi_sauto_t;
typedef unsigned long cpi_uauto_t;
#endif
    typedef cpi_u16_t cpi_method_t;
    typedef npc_message_t cpi_message_t;
    typedef npc_cred_t cpi_cred_t;

#ifdef __cplusplus
}
#endif

#endif
