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
/// @file       nai_rbtree.h
/// @brief      the red-black teee
/// @details
/// @date       2021-02-07
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation is a red-black tree, 
 *          the user is responsible for memory management.
 * @details the code example is as follows:
 *
 * @par     initial a red-black tree
 * @code
 *          nai_rbtree_t t;
 *          nai_rbtree_init(&t);
 * @endcode
 *
 * @par     declare an user node
 * @code
 *          typedef struct my_node_s {
 *              nai_rbnode_t ent;
 *              nai_str_t key;
 *              nai_str_t value;
 *          } my_node_t;
 * @endcode
 *
 * @par     find a node in the red-black tree
 * @code
 *          nai_int_t r;
 *          nai_str_t key;
 *          nai_rbtree_t* t;
 *          nai_rbnode_t** n = nai_rbtree_root(t);
 *          nai_rbnode_t* parent;
 *          my_node_t* e;
 *
 *          while (*n) {
 *              parent = *n;
 *              e = (my_node_t*)parent;
 *              r = nai_str_cmp(&e->key, key);
 *              if (r == 0) {
 *                  break;
 *              } else if (r > 0) {
 *                  n = &parent->rb_left;
 *              } else {
 *                  n = &parent->rb_right;
 *              };
 *          };
 *
 *          if (*n) {
 *              // found
 *          };
 * @endcode
 *
 * @par     insert a node into the red-black tree
 * @code
 *          nai_str_t key;
 *          nai_rbtree_t* t;
 *          nai_rbnode_t** n;
 *          nai_rbnode_t* parent;
 *          my_node_t* e;
 *
 *          // do find as the code above
 *          ...
 *
 *          // create a new node
 *          e = (my_node_t*)nai_malloc(sizeof(*e));
 *          if (e == 0) {
 *              goto _fail;
 *          };
 *
 *          e->key = key;
 *
 *          // insert the new node to the found location 
 *          nai_rbtree_link(t, &e->ent, parent, n);
 *          nai_rbtree_color(t, &e->ent);
 *
 * @endcode
 *
 * @par     erase a node from the red-black tree
 * @code
 *          nai_rbtree_t* t;
 *          nai_rbnode_t* e;
 *
 *          nai_rbtree_erase(r, e);
 * @endcode
 *
 * @par     enumerate nodes in the red-black tree
 * @code
 *          my_node_t* n;
 *          nai_rbtree_t* t;
 *          nai_rbnode_t* e = nai_rbtree_begin(t);
 *          nai_rbnode_t* end = nai_rbtree_end(t);
 *
 *          for ( ; e != end; ) {
 *              n = nai_containof(e, my_node_t, ent);
 *              e = nai_rbtree_next(e);
 *              // process data here
 *          };
 * @endcode
 */

#ifndef _RBTREE_H_NAI
#define _RBTREE_H_NAI

#pragma once

#include "nai_string.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // rbtree

#ifndef _NAI_TYPEDEF_RBNODE_T
    #define _NAI_TYPEDEF_RBNODE_T
    typedef struct nai_rbnode_s nai_rbnode_t;
#endif
#ifndef _NAI_TYPEDEF_RBTREE_T
    #define _NAI_TYPEDEF_RBTREE_T
    typedef struct nai_rbtree_s nai_rbtree_t;
#endif

    /**
 * the structure of the node of rbtree
 */
    struct nai_rbnode_s
    {
        size_t rb_parent_color; /**< the color and pointer to the parent */
        nai_rbnode_t* rb_right; /**< pointer to the right child */
        nai_rbnode_t* rb_left;  /**< pointer to the left child */
    };

    /**
 * structure of the rbtree
 */
    struct nai_rbtree_s
    {
        union
        {
            nai_rbnode_t* rb_root; /**< pointer to the root node */
            nai_rbnode_t rb_tree;  /**< the tree */
        };
    };

/**
 * initial the rbtree
 * @param   t       pointer to the rbtree
 * @return  void
 */
#define nai_rbtree_init(t)                                                                                             \
    {                                                                                                                  \
        (t)->rb_tree.rb_parent_color = 0;                                                                              \
        (t)->rb_tree.rb_left         = &(t)->rb_tree;                                                                  \
        (t)->rb_tree.rb_right        = &(t)->rb_tree;                                                                  \
    }

/**
 * get the root node of the rbtree
 * @param   t       pointer to the rbtree
 * @return  the address of the root node
 */
#define nai_rbtree_root(t) ((t)->rb_root)

/**
 * get the begin node of the rbtree
 * @param   t       pointer to the rbtree
 * @return  the address of the begin node
 */
#define nai_rbtree_begin(t) (&*(t)->rb_tree.rb_left)

/**
 * get the end node of the rbtree
 * @param   t       pointer to the rbtree
 * @return  the address of the end node
 */
#define nai_rbtree_end(t) (&(t)->rb_tree)

/**
 * get the first node of the rbtree
 * @param   t       pointer to the rbtree
 * @return  the address of the first node
 * @note    this function is same as nai_rbtree_begin
 */
#define nai_rbtree_first(t) (&*(t)->rb_tree.rb_left)

/**
 * get the last node of the rbtree
 * @param   t       pointer to the rbtree
 * @return  the address of the last node
 * @note    the last node is before the end node
 */
#define nai_rbtree_last(t) (&*(t)->rb_tree.rb_right)

/**
 * close the rbtree
 * @param   t       pointer to the rbtree
 * @return  void
 */
#define nai_rbtree_close(t) nai_rbtree_init(t)

    /**
 * get the node before the specified node
 * @param   e       pointer to the specified node
 * @return  the address of the previous node
 */
    NAI_EXTERN
    nai_rbnode_t* nai_rbtree_prev(const nai_rbnode_t* e);

    /**
 * get the node after the specified node
 * @param   e       pointer to the specified node
 * @return  the address of the next node
 */
    NAI_EXTERN
    nai_rbnode_t* nai_rbtree_next(const nai_rbnode_t* e);

    /**
 * link a node to a specified location in the tree
 * @param   t       pointer to the rbtree
 * @param   e       pointer to the node to link
 * @param   p       pointer to the parent node
 * @param   link    pointer to the link pointer
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_rbtree_link(nai_rbtree_t* t, nai_rbnode_t* e, nai_rbnode_t* p, nai_rbnode_t** link);

    /**
 * set the color of the linked node
 * @param   t       pointer to the rbtree
 * @param   e       pointer to the linked node
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function should be used follow nai_rbtree_link
 */
    NAI_EXTERN
    nai_int_t nai_rbtree_color(nai_rbtree_t* t, nai_rbnode_t* e);

    /**
 * erase a node from the rbtree
 * @param   t       pointer to the rbtree
 * @param   e       pointer to the node to erase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_rbtree_erase(nai_rbtree_t* t, nai_rbnode_t* e);

    //////////////////////////////////////////////////////////////////////////////
    // string of rbtree

#ifndef _NAI_TYPEDEF_RBNODE_STR_T
    #define _NAI_TYPEDEF_RBNODE_STR_T
    typedef struct nai_rbnode_str_s nai_rbnode_str_t;
#endif

    /**
 * the structure of the node with string key
 */
    struct nai_rbnode_str_s
    {
        nai_rbnode_t ent; /**< the node */
        nai_str_t key;    /**< the string key */
    };

/**
 * get the root node of the rbtree
 * @param   t       pointer to the rbtree
 * @return  the address of the root node
 */
#define nai_rbtree_str_root(t) ((nai_rbnode_str_t*)nai_rbtree_root(t))

/**
 * get the begin node of the rbtree
 * @param   t       pointer to the rbtree
 * @return  the address of the begin node
 */
#define nai_rbtree_str_begin(t) ((nai_rbnode_str_t*)nai_rbtree_begin(t))

/**
 * get the end node of the rbtree
 * @param   t       pointer to the rbtree
 * @return  the address of the end node
 */
#define nai_rbtree_str_end(t) ((nai_rbnode_str_t*)nai_rbtree_end(t))

/**
 * get the first node of the rbtree
 * @param   t       pointer to the rbtree
 * @return  the address of the first node
 * @note    this function is same as nai_rbtree_begin
 */
#define nai_rbtree_str_first(t) ((nai_rbnode_str_t*)nai_rbtree_first(t))

/**
 * get the last node of the rbtree
 * @param   t       pointer to the rbtree
 * @return  the address of the last node
 * @note    the last node is before the end node
 */
#define nai_rbtree_str_last(t) ((nai_rbnode_str_t*)nai_rbtree_last(t))

/**
 * get the node after the specified node
 * @param   e       pointer to the specified node
 * @return  the address of the next node
 */
#define nai_rbtree_str_next(e) ((nai_rbnode_str_t*)nai_rbtree_next(&(e)->ent))

/**
 * get the node before the specified node
 * @param   e       pointer to the specified node
 * @return  the address of the previous node
 */
#define nai_rbtree_str_prev(e) ((nai_rbnode_str_t*)nai_rbtree_prev(&(e)->ent))

    /**
 * find the node with the specified key
 * @param   t       pointer to the rbtree
 * @param   str     pointer to the string key
 * @param   cases   character case sensitivity
 * @return  the address of the found node, 
 *          if the key is not exists, null is returned.
 */
    NAI_EXTERN
    nai_rbnode_str_t* nai_rbtree_str_find(const nai_rbtree_t* t, const nai_str_t* str, nai_int_t cases);

    /**
 * get the lower bound of the specified key
 * @param   t       pointer to the rbtree
 * @param   str     pointer to the string key
 * @param   cases   character case sensitivity
 * @return  the address of the lower bound node
 */
    NAI_EXTERN
    nai_rbnode_str_t* nai_rbtree_str_lbound(const nai_rbtree_t* t, const nai_str_t* str, nai_int_t cases);

    /**
 * get the upper bound of the specified key
 * @param   t       pointer to the rbtree
 * @param   str     pointer to the string key
 * @param   cases   character case sensitivity
 * @return  the address of the upper bound node
 */
    NAI_EXTERN
    nai_rbnode_str_t* nai_rbtree_str_ubound(const nai_rbtree_t* t, const nai_str_t* str, nai_int_t cases);

    /**
 * insert a node into the rbtree
 * @param   t       pointer to the rbtree
 * @param   i       pointer to the node with string key to insert
 * @param   cases   character case sensitivity
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_rbtree_str_insert(nai_rbtree_t* t, nai_rbnode_str_t* i, nai_int_t cases);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
