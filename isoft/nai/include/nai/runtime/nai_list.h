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
/// @file       nai_list.h
/// @brief      the doubly linked list
/// @details
/// @date       2021-02-07
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation is an intrusive doubly linked list, 
 *          the user is responsible for memory management.
 *
 * @details the code example is as follows:
 *
 * @par     declare an user list entry
 * @code
 *          typedef struct my_node_s {
 *              nai_list_entry_t ent;
 *              nai_str_t value;
 *          } my_node_t;
 * @endcode
 *
 * @par     initial a linked list
 * @code
 *          nai_list_entry_t list;
 *          nai_list_init(&list);
 * @endcode
 *
 * @par     insert or remove an element
 * @code
 *          my_node_t node;
 *          nai_list_entry_t list;
 *          nai_list_init(&list);
 *
 *          // insert
 *          nai_str_setc(&node.value, "value");
 *          nai_list_insert_tail(&list, &node.ent);
 *
 *          // remove
 *          nai_list_remove_entry(&node.ent);
 * @endcode
 *
 * @par     enumerate nodes in the linked list
 * @code
 *          my_node_t* n;
 *          nai_list_entry_t* e;
 *          nai_list_entry_t* list;
 *
 *          e = list->next;
 *          for ( ; e != list; e = e->next) {
 *              n = nai_containof(e, my_node_t, ent);
 *              // process data here
 *          };
 * @endcode
 */

#ifndef _LIST_H_NAI
#define _LIST_H_NAI

#pragma once

#include "nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // list

#ifndef _NAI_DEFINED_LIST_T
    #define _NAI_DEFINED_LIST_T

    #ifndef _NAI_TYPEDEF_LIST_T
        #define _NAI_TYPEDEF_LIST_T
    typedef struct nai_list_entry_s nai_list_t;
    #endif
    #ifndef _NAI_TYPEDEF_LIST_ENTRY_T
        #define _NAI_TYPEDEF_LIST_ENTRY_T
    typedef struct nai_list_entry_s nai_list_entry_t;
    #endif

    /**
 * the structure of the linked list entry
 */
    struct nai_list_entry_s
    {
        nai_list_entry_t* next; /**< pointer to the next element */
        nai_list_entry_t* prev; /**< pointer to the prev element */
    };

#endif

/**
 * initial the linked list
 * @param   l       pointer to the linked list
 * @return  void
 */
#define nai_list_init(l)                                                                                               \
    {                                                                                                                  \
        (l)->next = (l);                                                                                               \
        (l)->prev = (l);                                                                                               \
    }

/**
 * close the linked list
 * @param   l       pointer to the linked list
 * @return  void
 */
#define nai_list_close(l) nai_list_init(l)

/**
 * test whether the linked list is empty
 * @param   l       pointer to the linked list
 * @return  bool result of the test
 */
#define nai_list_is_empty(l) ((l)->next == (l))

/**
 * insert a list element into the head of the linked list
 * @param   l       pointer to the linked list
 * @param   e       pointer to the list element to insert
 * @return  void
 */
#define nai_list_insert_head(l, e) nai_list_insert_after(l, e)

/**
 * insert a list element into the tail of the linked list
 * @param   l       pointer to the linked list
 * @param   e       pointer to the list element to insert
 * @return  void
 */
#define nai_list_insert_tail(l, e) nai_list_insert_before(l, e)

/**
 * insert a list element after the specified list element
 * @param   l       pointer to the specified list element
 * @param   e       pointer to the list element to insert
 * @return  void
 */
#define nai_list_insert_after(l, e)                                                                                    \
    {                                                                                                                  \
        (e)->next       = (l)->next;                                                                                   \
        (l)->next       = (e);                                                                                         \
        (e)->prev       = (l);                                                                                         \
        (e)->next->prev = (e);                                                                                         \
    }

/**
 * insert a list element before the specified list element
 * @param   l       pointer to the specified list element
 * @param   e       pointer to the list element to insert
 * @return  void
 */
#define nai_list_insert_before(l, e)                                                                                   \
    {                                                                                                                  \
        (e)->prev       = (l)->prev;                                                                                   \
        (l)->prev       = (e);                                                                                         \
        (e)->next       = (l);                                                                                         \
        (e)->prev->next = (e);                                                                                         \
    }

/**
 * insert a linked list into the head of the linked list
 * @param   l       pointer to the linked list
 * @param   a       pointer to the linked list to inseret
 * @return  void
 */
#define nai_list_add_head(l, a)                                                                                        \
    if (!nai_list_is_empty(a)) {                                                                                       \
        (a)->prev->next = (l)->next;                                                                                   \
        (a)->next->prev = (l)->next->prev;                                                                             \
        (l)->next->prev = (a)->prev;                                                                                   \
        (l)->next       = (a)->next;                                                                                   \
        nai_list_init(a);                                                                                              \
    }

/**
 * insert a linked list into the tail of the linked list
 * @param   l       pointer to the linked list
 * @param   a       pointer to the linked list to inseret
 * @return  void
 */
#define nai_list_add_tail(l, a)                                                                                        \
    if (!nai_list_is_empty(a)) {                                                                                       \
        (a)->prev->next = (l)->next->prev;                                                                             \
        (a)->next->prev = (l)->prev;                                                                                   \
        (l)->prev->next = (a)->next;                                                                                   \
        (l)->prev       = (a)->prev;                                                                                   \
        nai_list_init(a);                                                                                              \
    }

/**
 * remove a list element from the linked list
 * @param   e       pointer to the list element to remove
 * @return  void
 */
#define nai_list_entry_remove(e)                                                                                       \
    {                                                                                                                  \
        (e)->next->prev = (e)->prev;                                                                                   \
        (e)->prev->next = (e)->next;                                                                                   \
    }

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
