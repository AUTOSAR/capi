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
/// @file       cpi_rbtree.c
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "npc/cpi/cpi_mm.h"
#include "npc/cpi/cpi_rbtree.h"

CPI_CHAIN_FUNC_DECLARE(cpi_rbtree, \
                       cpi_rbtree_node_t, \
                       static inline void,);
CPI_CHAIN_FUNC_DEFINE(cpi_rbtree, \
                      cpi_rbtree_node_t, \
                      static inline void, \
                      prev, \
                      next);

/*static declarations*/
static inline void
left_rotate(cpi_rbtree_t *t, cpi_rbtree_node_t *n);
static inline void
right_rotate(cpi_rbtree_t *t, cpi_rbtree_node_t *n);
static inline void
rbtree_insert_fixup(cpi_rbtree_t *t, cpi_rbtree_node_t *n);
static inline cpi_rbtree_node_t *
rbtree_minimum(cpi_rbtree_t *t, cpi_rbtree_node_t *n);
static inline void
rbtree_transplant(cpi_rbtree_t *t, cpi_rbtree_node_t *u, cpi_rbtree_node_t *v);
static inline void
rbtree_delete_fixup(cpi_rbtree_t *t, cpi_rbtree_node_t *n);

/*rbtree_init*/
cpi_rbtree_t *
cpi_rbtree_init(struct cpi_rbtree_attr *attr)
{
    cpi_rbtree_t *t;
    if (attr->pool == NULL) {
        t = (cpi_rbtree_t *)cpi_malloc(sizeof(cpi_rbtree_t));
    } else {
        t = (cpi_rbtree_t *)attr->pool_alloc(attr->pool, sizeof(cpi_rbtree_t));
    }
    if (t == NULL)
        goto out;

    t->pool = attr->pool;
    t->pool_alloc = attr->pool_alloc;
    t->pool_free = attr->pool_free;
    t->nil.tree = t;
    t->nil.data = NULL;
    t->nil.parent = &(t->nil);
    t->nil.left = &(t->nil);
    t->nil.right = &(t->nil);
    t->nil.color = M_RB_BLACK;
    t->root = &(t->nil);
    t->min = &(t->nil);
    t->head = t->tail = NULL;
    t->free_head = t->free_tail = NULL;
    t->iter = NULL;
    t->cmp = attr->cmp;
    t->data_free = attr->data_free;
    t->nr_node = 0;
    t->del = 0;
    t->cache = attr->cache;

out:
    return t;
}

/*rbtree_destroy*/
void
cpi_rbtree_destroy(cpi_rbtree_t *t)
{
    cpi_rbtree_node_t *fr;

    if (t != NULL) {
        t->cache = 0;

        /*
         * Warning: cpi_lang_sys.c: cpi_import is very dependent on this release order.
         * This release order ensures that the resources of the dynamic extension library
         * are released first, and then the import resources are released.
         * If the import resource is released before the dynamic library resource,
         * the function in the dynamic library cannot be read when the dynamic extension
         * resource is released, and the program terminates abnormally.
         */
        while ((fr = t->tail) != NULL) {
            cpi_rbtree_chain_del(&(t->head), &(t->tail), fr);
            cpi_rbtree_node_free(fr);
        }
        while ((fr = t->free_head) != NULL) {
            cpi_rbtree_chain_del(&(t->free_head), &(t->free_tail), fr);
            cpi_rbtree_node_free(fr);
        }
        if (t->pool != NULL) t->pool_free(t);
        else cpi_free(t);
    }
}

/*rbtree successor*/
cpi_rbtree_node_t *
cpi_rbtree_successor(cpi_rbtree_t *t, cpi_rbtree_node_t *n)
{
    cpi_rbtree_node_t *r;

    if (n != &(t->nil) && n->right != &(t->nil)) {
        r = rbtree_minimum(t, n->right);
    } else {
        r = n->parent;
        while (r!=&(t->nil) && n==r->right) {
            n = r;
            r = r->parent;
        }
    }
    return r;
}

/*rbtree new node*/
cpi_rbtree_node_t *
cpi_rbtree_node_new(cpi_rbtree_t *t, void *data)
{
    cpi_rbtree_node_t *n;

    if ((n = t->free_head) != NULL) {
        cpi_rbtree_chain_del(&(t->free_head), &(t->free_tail), n);
    } else {
        if (t->pool == NULL)
            n = (cpi_rbtree_node_t *)cpi_malloc(sizeof(cpi_rbtree_node_t));
        else
            n = (cpi_rbtree_node_t *)t->pool_alloc(t->pool, sizeof(cpi_rbtree_node_t));
        if (n == NULL)
            goto out;
    }
    n->data = data;
    n->tree = t;
    n->prev = n->next = NULL;
    n->parent = &(t->nil);
    n->left = &(t->nil);
    n->right = &(t->nil);

out:
    return n;
}

/*rbtree free node*/
void
cpi_rbtree_node_free(cpi_rbtree_node_t *n)
{
    cpi_rbtree_t *t = n->tree;
    if (n->data != NULL && t->data_free != NULL)
        t->data_free(n->data);
    if (t->cache) {
        n->prev = n->next = NULL;
        cpi_rbtree_chain_add(&(t->free_head), &(t->free_tail), n);
    } else {
        if (t->pool != NULL) t->pool_free(n);
        else cpi_free(n);
    }
}

/*Left rotate*/
static inline void
left_rotate(cpi_rbtree_t *t, cpi_rbtree_node_t *n)
{
    if (n->right != &(t->nil)) {
        cpi_rbtree_node_t *tmp = n->right;
        n->right = tmp->left;
        if (tmp->left != &(t->nil)) tmp->left->parent = n;
        tmp->parent = n->parent;
        if (n->parent == &(t->nil)) t->root = tmp;
        else if (n == n->parent->left) n->parent->left = tmp;
        else n->parent->right = tmp;
        tmp->left = n;
        n->parent = tmp;
    }
}

/*Right rotate*/
static inline void
right_rotate(cpi_rbtree_t *t, cpi_rbtree_node_t *n)
{
    if (n->left != &(t->nil)) {
        cpi_rbtree_node_t *tmp = n->left;
        n->left = tmp->right;
        if (tmp->right != &(t->nil)) tmp->right->parent = n;
        tmp->parent = n->parent;
        if (n->parent == &(t->nil)) t->root = tmp;
        else if (n==n->parent->right) n->parent->right = tmp;
        else n->parent->left = tmp;
        tmp->right = n;
        n->parent = tmp;
    }
}

/*Insert*/
void
cpi_rbtree_insert(cpi_rbtree_node_t *n)
{
    cpi_rbtree_t *t = n->tree;
    cpi_rbtree_node_t *y = &(t->nil);
    cpi_rbtree_node_t *x = t->root;
    while (x != &(t->nil)) {
        y = x;
        if (t->cmp(n->data, x->data) < 0) x = x->left;
        else x = x->right;
    }
    n->parent = y;
    if (y == &(t->nil)) t->root = n;
    else if (t->cmp(n->data, y->data) < 0) y->left = n;
    else y->right = n;
    n->left = &(t->nil);
    n->right = &(t->nil);
    n->color = M_RB_RED;
    rbtree_insert_fixup(t, n);
    if (t->min == &(t->nil)) t->min = n;
    else if (t->cmp(n->data, t->min->data) < 0) t->min = n;
    ++(t->nr_node);
    cpi_rbtree_chain_add(&(t->head), &(t->tail), n);
}

/*insert fixup*/
static inline void
rbtree_insert_fixup(cpi_rbtree_t *t, cpi_rbtree_node_t *n)
{
    cpi_rbtree_node_t *tmp;
    while (n->parent->color == M_RB_RED) {
        if (n->parent == n->parent->parent->left) {
            tmp = n->parent->parent->right;
            if (tmp->color == M_RB_RED) {
                n->parent->color = M_RB_BLACK;
                tmp->color = M_RB_BLACK;
                n->parent->parent->color = M_RB_RED;
                n = n->parent->parent;
                continue;
            } else if (n == n->parent->right) {
                n = n->parent;
                left_rotate(t, n);
            }
            n->parent->color = M_RB_BLACK;
            n->parent->parent->color = M_RB_RED;
            right_rotate(t, n->parent->parent);
        } else {
            tmp = n->parent->parent->left;
            if (tmp->color == M_RB_RED) {
                n->parent->color = M_RB_BLACK;
                tmp->color = M_RB_BLACK;
                n->parent->parent->color = M_RB_RED;
                n = n->parent->parent;
                continue;
            } else if (n == n->parent->left) {
                n = n->parent;
                right_rotate(t, n);
            }
            n->parent->color = M_RB_BLACK;
            n->parent->parent->color = M_RB_RED;
            left_rotate(t, n->parent->parent);
        }
    }
    t->root->color = M_RB_BLACK;
}

/*Tree Minimum*/
static inline cpi_rbtree_node_t *
rbtree_minimum(cpi_rbtree_t *t, cpi_rbtree_node_t *n)
{
    while (n->left != &(t->nil)) n = n->left;
    return n;
}

/*transplant*/
static inline void
rbtree_transplant(cpi_rbtree_t *t, cpi_rbtree_node_t *u, cpi_rbtree_node_t *v)
{
    if (u->parent == &(t->nil)) t->root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    v->parent = u->parent;
}

/*rbtree_delete*/
void
cpi_rbtree_delete(cpi_rbtree_node_t *n)
{
    cpi_rbtree_t *t = n->tree;
    if (n == t->min)
        t->min = cpi_rbtree_successor(t, n);
    enum rbtree_color y_original_color;
    cpi_rbtree_node_t *x, *y;
    y = n;
    y_original_color = y->color;
    if (n->left == &(t->nil)) {
        x = n->right;
        rbtree_transplant(t, n, n->right);
    } else if (n->right == &(t->nil)) {
        x = n->left;
        rbtree_transplant(t, n, n->left);
    } else {
        y = rbtree_minimum(t, n->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == n) x->parent = y;
        else {
            rbtree_transplant(t, y, y->right);
            y->right = n->right;
            y->right->parent = y;
        }
        rbtree_transplant(t, n, y);
        y->left = n->left;
        y->left->parent = y;
        y->color = n->color;
    }
    if (y_original_color == M_RB_BLACK) rbtree_delete_fixup(t, x);
    n->parent = n->left = n->right = &(t->nil);
    --(t->nr_node);
    if (t->iter != NULL && t->iter == n) {
        t->iter = n->next;
        t->del = 1;
    }
    cpi_rbtree_chain_del(&(t->head), &(t->tail), n);
}

/*rbtree_delete_fixup*/
static inline void
rbtree_delete_fixup(cpi_rbtree_t *t, cpi_rbtree_node_t *n)
{
    cpi_rbtree_node_t *tmp;
    while ((n != t->root) && (n->color == M_RB_BLACK)) {
        if (n == n->parent->left) {
            tmp = n->parent->right;
            if (tmp->color == M_RB_RED) {
                tmp->color = M_RB_BLACK;
                n->parent->color = M_RB_RED;
                left_rotate(t, n->parent);
                tmp = n->parent->right;
            }
            if ((tmp->left->color == M_RB_BLACK) && (tmp->right->color == M_RB_BLACK)) {
                tmp->color = M_RB_RED;
                n = n->parent;
                continue;
            } else if (tmp->right->color == M_RB_BLACK) {
                tmp->left->color = M_RB_BLACK;
                tmp->color = M_RB_RED;
                right_rotate(t, tmp);
                tmp = n->parent->right;
            }
            tmp->color = n->parent->color;
            n->parent->color = M_RB_BLACK;
            tmp->right->color = M_RB_BLACK;
            left_rotate(t, n->parent);
            n = t->root;
        } else {
            tmp = n->parent->left;
            if (tmp->color == M_RB_RED) {
                tmp->color = M_RB_BLACK;
                n->parent->color = M_RB_RED;
                right_rotate(t, n->parent);
                tmp = n->parent->left;
            }
            if ((tmp->right->color == M_RB_BLACK) && (tmp->left->color == M_RB_BLACK)) {
                tmp->color = M_RB_RED;
                n = n->parent;
                continue;
            } else if (tmp->left->color == M_RB_BLACK) {
                tmp->right->color = M_RB_BLACK;
                tmp->color = M_RB_RED;
                left_rotate(t, tmp);
                tmp = n->parent->left;
            }
            tmp->color = n->parent->color;
            n->parent->color = M_RB_BLACK;
            tmp->left->color = M_RB_BLACK;
            right_rotate(t, n->parent);
            n = t->root;
        }
    }
    n->color = M_RB_BLACK;
}

/*search*/
cpi_rbtree_node_t *
cpi_rbtree_search(cpi_rbtree_t *t, const void *key)
{
    int ret;
    cpi_rbtree_node_t *root = t->root;
    while ((root != &(t->nil)) && ((ret = t->cmp(key, root->data)) != 0)) {
        if (ret < 0) root = root->left;
        else root = root->right;
    }
    return root;
}

/*min*/
cpi_rbtree_node_t *
cpi_rbtree_min(cpi_rbtree_t *t)
{
    return t->min;
}

/*scan_all*/
int cpi_rbtree_scan_all(cpi_rbtree_t *t, rbtree_act act, void *udata)
{
    int r = 0;
    for (t->iter = t->head; t->iter != NULL; ) {
        if ((r = act(t->iter, t->iter->data, udata)) < 0) {
            break;
        }
        if (t->del) {
            t->del = 0;
            continue;
        } else {
            t->iter = t->iter->next;
        }
    }
    return r;
}

