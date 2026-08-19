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
/// @file       cpi_rbtree.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_RBTREE_H
#define __CPI_RBTREE_H
#include "npc/cpi/cpi_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct cpi_rbtree_node_s cpi_rbtree_node_t;
    typedef struct cpi_rbtree_s cpi_rbtree_t;
    /*
 * >0 -- the first argument greater than the second.
 * ==0 -- equal.
 * <0 -- less.
 */
    typedef int (*rbtree_cmp)(const void *, const void *);
    typedef void (*rbtree_free_data)(void *);
    typedef int (*rbtree_act)(cpi_rbtree_node_t *node, void *rn_data, void *udata);
    typedef void *(*rbtree_pool_alloc_handler)(void *, cpi_size_t);
    typedef void (*rbtree_pool_free_handler)(void *);

    enum rbtree_color
    {
        M_RB_RED,
        M_RB_BLACK
    };

    struct cpi_rbtree_attr
    {
        void *pool;
        rbtree_pool_alloc_handler pool_alloc;
        rbtree_pool_free_handler pool_free;
        rbtree_cmp cmp;
        rbtree_free_data data_free;
        cpi_u32_t cache : 1;
    };

    struct cpi_rbtree_node_s
    {
        void *data;
        cpi_rbtree_t *tree;
        struct cpi_rbtree_node_s *prev;
        struct cpi_rbtree_node_s *next;
        struct cpi_rbtree_node_s *parent;
        struct cpi_rbtree_node_s *left;
        struct cpi_rbtree_node_s *right;
        enum rbtree_color color;
    };

    struct cpi_rbtree_s
    {
        void *pool;
        rbtree_pool_alloc_handler pool_alloc;
        rbtree_pool_free_handler pool_free;
        cpi_rbtree_node_t nil;
        cpi_rbtree_node_t *root;
        cpi_rbtree_node_t *min;
        cpi_rbtree_node_t *head;
        cpi_rbtree_node_t *tail;
        cpi_rbtree_node_t *free_head;
        cpi_rbtree_node_t *free_tail;
        cpi_rbtree_node_t *iter;
        rbtree_cmp cmp;
        rbtree_free_data data_free;
        cpi_uauto_t nr_node;
        cpi_u32_t del : 1;
        cpi_u32_t cache : 1;
    };

#define cpi_rbtree_get_nr_node(t) ((t)->nr_node)
#define cpi_rbtree_null(ptr)      ((ptr) == &((ptr)->tree->nil))

    extern cpi_rbtree_t *cpi_rbtree_init(struct cpi_rbtree_attr *attr);
    extern void cpi_rbtree_destroy(cpi_rbtree_t *t);
    extern void cpi_rbtree_insert(cpi_rbtree_node_t *n);
    extern void cpi_rbtree_delete(cpi_rbtree_node_t *n);
    extern cpi_rbtree_node_t *cpi_rbtree_successor(cpi_rbtree_t *t, cpi_rbtree_node_t *n);
    extern cpi_rbtree_node_t *cpi_rbtree_search(cpi_rbtree_t *t, const void *key);
    extern cpi_rbtree_node_t *cpi_rbtree_min(cpi_rbtree_t *t);

    extern cpi_rbtree_node_t *cpi_rbtree_node_new(cpi_rbtree_t *t, void *data);
    extern void cpi_rbtree_node_free(cpi_rbtree_node_t *n);
    extern int cpi_rbtree_scan_all(cpi_rbtree_t *t, rbtree_act act, void *udata);

#ifdef __cplusplus
}
#endif

#endif
