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
/// @file       nai_rbtree.c
/// @brief      
/// @details
/// @date       2020-12-13
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_rbtree.h"
#include "nai/runtime/nai_string.h"


#if 1


#define RB_RED      0
#define RB_BLACK    1


#define nai_rbnode_set_parent(n, p)             \
    (n)->rb_parent_color = (                    \
    (n)->rb_parent_color & 3) | (size_t)(p)     \

#define nai_rbnode_set_color(n, c)              \
    (n)->rb_parent_color = (                    \
    (n)->rb_parent_color & ~3) | (size_t)(c)    \

#define nai_rbnode_set_red(n)                   \
    nai_rbnode_set_color(n, RB_RED)             \

#define nai_rbnode_set_black(n)                 \
    nai_rbnode_set_color(n, RB_BLACK)           \


#define nai_rbnode_parent(n)    ((nai_rbnode_t*)((n)->rb_parent_color & ~3))
#define nai_rbnode_color(n)     ((n)->rb_parent_color & 3)
#define nai_rbnode_is_red(n)    (nai_rbnode_color(n) == RB_RED)
#define nai_rbnode_is_black(n)  (nai_rbnode_color(n) == RB_BLACK)



static nai_rbnode_t* nai_rbtree_rotate_left(nai_rbtree_t* t, nai_rbnode_t* n)
{
    nai_rbnode_t* r;
    nai_rbnode_t* p;


    r = n->rb_right;
    if (r->rb_left) {
        nai_rbnode_set_parent(r->rb_left, n);
    };
    n->rb_right = r->rb_left;
    r->rb_left = n;


    p = nai_rbnode_parent(n);
    nai_rbnode_set_parent(r, p);


    if (nai_rbtree_root(t) == n) {
        nai_rbtree_root(t) = r;
    } else if (n == p->rb_left) {
        p->rb_left = r;
    } else {
        p->rb_right = r;
    };

    nai_rbnode_set_parent(n, r);

    return n;
};


static nai_rbnode_t* nai_rbtree_rotate_right(nai_rbtree_t* t, nai_rbnode_t* n)
{
    nai_rbnode_t* l;
    nai_rbnode_t* p;


    l = n->rb_left;
    if (l->rb_right) {
        nai_rbnode_set_parent(l->rb_right, n);
    };
    n->rb_left = l->rb_right;
    l->rb_right = n;


    p = nai_rbnode_parent(n);
    nai_rbnode_set_parent(l, p);


    if (nai_rbtree_root(t) == n) {
        nai_rbtree_root(t) = l;
    } else if (n == p->rb_right) {
        p->rb_right = l;
    } else {
        p->rb_left = l;
    };

    nai_rbnode_set_parent(n, l);

    return n;
};


static nai_rbnode_t* nai_rbtree_minimum(nai_rbnode_t* n)
{
    nai_rbnode_t* m = n;
    while (m->rb_left) {
        m = m->rb_left;
    };
    return m;
};


static nai_rbnode_t* nai_rbtree_maximum(nai_rbnode_t* n)
{
    nai_rbnode_t* m = n;
    while (m->rb_right) {
        m = m->rb_right;
    };
    return m;
};


nai_rbnode_t* nai_rbtree_prev(const nai_rbnode_t* node)
{
    nai_rbnode_t* parent;


    if (nai_rbnode_is_red(node) && 
        nai_rbnode_parent(nai_rbnode_parent(node)) == node) {
        node = node->rb_right;
        goto _end;
    };
    /* If we have a left-hand child, go down and then right as far
       as we can. */
    if (node->rb_left) {
        node = nai_rbtree_maximum(node->rb_left);
        goto _end;
    };

    /* No left-hand children. Go up till we find an ancestor which
       is a right-hand child of its parent */

    while ((parent = nai_rbnode_parent(node)) && node == parent->rb_left) {
        node = parent;
    };
    node = parent;

_end:
    return (nai_rbnode_t*)node;
};


nai_rbnode_t* nai_rbtree_next(const nai_rbnode_t* node)
{
    nai_rbnode_t* parent;

    /* If we have a right-hand child, go down and then left as far
       as we can. */
    if (node->rb_right) {
        node = nai_rbtree_minimum(node->rb_right);
        goto _end;
    };

    /* No right-hand children.  Everything down and left is
       smaller than us, so any 'next' node must be in the general
       direction of our parent. Go up the tree; any time the
       ancestor is a right-hand child of its parent, keep going
       up. First time it's a left-hand child of its parent, said
       parent is our 'next' node. */

    while ((parent = nai_rbnode_parent(node)) && node == parent->rb_right) {
        node = parent;
    };
    if (node->rb_right != parent) {
        node = parent;
    };

_end:
    return (nai_rbnode_t*)node;
};


nai_int_t nai_rbtree_link(
    nai_rbtree_t* t, nai_rbnode_t* n, nai_rbnode_t* p, nai_rbnode_t** link)
{
    nai_rbnode_t* tree = nai_rbtree_end(t);


    nai_rbnode_set_parent(n, p);
    nai_rbnode_set_red(n);
    n->rb_left = 0;
    n->rb_right = 0;

    if (p == tree) {
        nai_rbnode_set_parent(tree, n);
        tree->rb_left = n;
        tree->rb_right = n;
    } else if (link == &p->rb_left) {
        p->rb_left = n;
        if (tree->rb_left == p) {
            tree->rb_left = n;
        };
    } else {
        p->rb_right = n;
        if (tree->rb_right == p) {
            tree->rb_right = n;
        };
    };

    return 0;
};


nai_int_t nai_rbtree_color(nai_rbtree_t* t, nai_rbnode_t* n)
{
    nai_rbnode_t* tree = nai_rbtree_end(t);
    nai_rbnode_t* parent;
    nai_rbnode_t* gparent;
    nai_rbnode_t* uncle;
    /* nai_rbnode_t* tmp; */


    while ((parent = nai_rbnode_parent(n)) != tree && 
        nai_rbnode_is_red(parent)) {

        gparent = nai_rbnode_parent(parent);

        if (parent == gparent->rb_left) {
            uncle = gparent->rb_right;
            if (uncle && nai_rbnode_is_red(uncle)) {
                nai_rbnode_set_black(uncle);
                nai_rbnode_set_black(parent);
                nai_rbnode_set_red(gparent);
                n = gparent;
                continue;
            }

            if (parent->rb_right == n) {
                nai_rbtree_rotate_left(t, parent);
                /* tmp = parent; */
                /* parent = n; */
                /* n = tmp; */
                n = parent;
                parent = nai_rbnode_parent(n);
            }

            nai_rbnode_set_black(parent);
            nai_rbnode_set_red(gparent);
            nai_rbtree_rotate_right(t, gparent);

        } else {

            uncle = gparent->rb_left;
            if (uncle && nai_rbnode_is_red(uncle)) {
                nai_rbnode_set_black(uncle);
                nai_rbnode_set_black(parent);
                nai_rbnode_set_red(gparent);
                n = gparent;
                continue;
            }

            if (parent->rb_left == n) {
                nai_rbtree_rotate_right(t, parent);
                /* tmp = parent; */
                /* parent = n; */
                /* n = tmp; */
                n = parent;
                parent = nai_rbnode_parent(n);
            }

            nai_rbnode_set_black(parent);
            nai_rbnode_set_red(gparent);
            nai_rbtree_rotate_left(t, gparent);
        }
    }

    nai_rbnode_set_black(nai_rbtree_root(t));

    return 0;
};


static void nai_rbtree_erase_color(
    nai_rbtree_t* t, nai_rbnode_t* node, nai_rbnode_t* parent)
{
    nai_rbnode_t* other;


    while ((!node || nai_rbnode_is_black(node)) && 
        node != nai_rbtree_root(t)) {

        if (parent->rb_left == node) {
            other = parent->rb_right;
            if (nai_rbnode_is_red(other)) {
                nai_rbnode_set_black(other);
                nai_rbnode_set_red(parent);
                nai_rbtree_rotate_left(t, parent);
                other = parent->rb_right;
            };

            if ((!other->rb_left || nai_rbnode_is_black(other->rb_left)) &&
                (!other->rb_right || nai_rbnode_is_black(other->rb_right))) {
                nai_rbnode_set_red(other);
                node = parent;
                parent = nai_rbnode_parent(node);
            } else {
                if (!other->rb_right || nai_rbnode_is_black(other->rb_right)) {
                    nai_rbnode_set_black(other->rb_left);
                    nai_rbnode_set_red(other);
                    nai_rbtree_rotate_right(t, other);
                    other = parent->rb_right;
                }
                nai_rbnode_set_color(other, nai_rbnode_color(parent));
                nai_rbnode_set_black(parent);
                nai_rbnode_set_black(other->rb_right);
                nai_rbtree_rotate_left(t, parent);
                /* node = nai_rbtree_root(t); */
                break;
            }

        } else {
            other = parent->rb_left;
            if (nai_rbnode_is_red(other)) {
                nai_rbnode_set_black(other);
                nai_rbnode_set_red(parent);
                nai_rbtree_rotate_right(t, parent);
                other = parent->rb_left;
            }

            if ((!other->rb_left || nai_rbnode_is_black(other->rb_left)) &&
                (!other->rb_right || nai_rbnode_is_black(other->rb_right))) {
                nai_rbnode_set_red(other);
                node = parent;
                parent = nai_rbnode_parent(node);
            } else {
                if (!other->rb_left || nai_rbnode_is_black(other->rb_left)) {
                    nai_rbnode_set_black(other->rb_right);
                    nai_rbnode_set_red(other);
                    nai_rbtree_rotate_left(t, other);
                    other = parent->rb_left;
                }
                nai_rbnode_set_color(other, nai_rbnode_color(parent));
                nai_rbnode_set_black(parent);
                nai_rbnode_set_black(other->rb_left);
                nai_rbtree_rotate_right(t, parent);
                /* node = nai_rbtree_root(t); */
                break;
            }
        }
    }
    if (node) {
        nai_rbnode_set_black(node);
    };
}


nai_int_t nai_rbtree_erase(nai_rbtree_t* t, nai_rbnode_t* n)
{
    nai_int_t color;
    nai_rbnode_t* tree = nai_rbtree_end(t);
    nai_rbnode_t* left;
    nai_rbnode_t* node = n;
    nai_rbnode_t* child;
    nai_rbnode_t* parent;


    if (!node->rb_left) {
        child = node->rb_right;
    } else if (!node->rb_right) {
        child = node->rb_left;
    } else {
        node = node->rb_right;
        while ((left = node->rb_left) != 0) {
            node = left;
        };
        child = node->rb_right;

        /* case 'node != n' */
        nai_rbnode_set_parent(n->rb_left, node);
        node->rb_left = n->rb_left;
        if (node == n->rb_right) {
            parent = node;
        } else {
            parent = nai_rbnode_parent(node);
            parent->rb_left = child;
            if (child) {
                nai_rbnode_set_parent(child, parent);
            };
            node->rb_right = n->rb_right;
            nai_rbnode_set_parent(n->rb_right, node);
        };

        if (nai_rbtree_root(t) == n) {
            nai_rbtree_root(t) = node;
        } else if (nai_rbnode_parent(n)->rb_left == n) {
            nai_rbnode_parent(n)->rb_left = node;
        } else {
            nai_rbnode_parent(n)->rb_right = node;
        };

        color = nai_rbnode_color(node);
        node->rb_parent_color = n->rb_parent_color;
        nai_rbnode_set_color(n, color);
        goto _color;
    }

    /* case 'node == n' */
    parent = nai_rbnode_parent(node);

    if (child) {
        nai_rbnode_set_parent(child, parent);
    };
    if (nai_rbtree_root(t) == n) {
        nai_rbtree_root(t) = child;
    } else if (parent->rb_left == n) {
        parent->rb_left = child;
    } else {
        parent->rb_right = child;
    };

    if (tree->rb_left == n) {
        if (n->rb_right == 0) {
            tree->rb_left = nai_rbnode_parent(n);
        } else {
            tree->rb_left = nai_rbtree_minimum(child);
        };
    }
    if (tree->rb_right == n) {
        if (n->rb_left == 0) {
            tree->rb_right = nai_rbnode_parent(n);
        } else {
            tree->rb_right = nai_rbtree_maximum(child);
        };
    }

    color = nai_rbnode_color(node);

_color:
    if (color == RB_BLACK) {
        nai_rbtree_erase_color(t, child, parent);
    };
    return 0;
};


#else


#include "thirdparty/rbtree/rbtree.h"


nai_rbnode_t* nai_rbtree_first(const nai_rbtree_t* t)
{
    nai_rbnode_t* n;

    n = t->rb_node;
    if (n) {
        while (n->rb_left)
            n = n->rb_left;
    };
    return n;
};


nai_rbnode_t* nai_rbtree_last(const nai_rbtree_t* t)
{
    nai_rbnode_t* n;

    n = t->rb_node;
    if (n) {
        while (n->rb_right)
            n = n->rb_right;
    };
    return n;
};


#define nai_rbtree_parent(r)    \
    ((nai_rbnode_t*)((r)->rb_parent_color & ~3))


nai_rbnode_t* nai_rbtree_prev(const nai_rbnode_t* node)
{
    nai_rbnode_t* parent;

    if (nai_rbtree_parent(node) == node)
        return NULL;

    /* If we have a left-hand child, go down and then right as far
       as we can. */
    if (node->rb_left) {
        node = node->rb_left; 
        while (node->rb_right)
            node=node->rb_right;
        return (nai_rbnode_t*)node;
    }

    /* No left-hand children. Go up till we find an ancestor which
       is a right-hand child of its parent */
    while ((parent = nai_rbtree_parent(node)) && node == parent->rb_left)
        node = parent;

    return parent;
};


nai_rbnode_t* nai_rbtree_next(const nai_rbnode_t* node)
{
    nai_rbnode_t* parent;

    if (!node || nai_rbtree_parent(node) == node)
        return 0;

    /* If we have a right-hand child, go down and then left as far
       as we can. */
    if (node->rb_right) {
        node = node->rb_right; 
        while (node->rb_left)
            node=node->rb_left;
        return (nai_rbnode_t*)node;
    }

    /* No right-hand children.  Everything down and left is
       smaller than us, so any 'next' node must be in the general
       direction of our parent. Go up the tree; any time the
       ancestor is a right-hand child of its parent, keep going
       up. First time it's a left-hand child of its parent, said
       parent is our 'next' node. */
    while ((parent = nai_rbtree_parent(node)) && node == parent->rb_right)
        node = parent;

    return parent;
};


nai_int_t nai_rbtree_link(nai_rbtree_t* t, 
    nai_rbnode_t* e, nai_rbnode_t* p, nai_rbnode_t** link)
{
    rb_link_node((rb_node_t*)e, (rb_node_t*)p, (rb_node_t**)link);
    return 0;
};


nai_int_t nai_rbtree_color(nai_rbtree_t* t, nai_rbnode_t* e)
{
    rb_insert_color((rb_node_t*)e, (rb_root_t*)t);
    return 0;
};


nai_int_t nai_rbtree_erase(nai_rbtree_t* t, nai_rbnode_t* e)
{
    rb_erase((rb_node_t*)e, (rb_root_t*)t);
    return 0;
};


#endif



//////////////////////////////////////////////////////////////////////////////
// string of rbtree


nai_rbnode_str_t* nai_rbtree_str_find(
    const nai_rbtree_t* t, const nai_str_t* str, nai_int_t cases)
{
    nai_rbnode_str_t* l;


    l = nai_rbtree_str_lbound(t, str, cases);
    if (l != (nai_rbnode_str_t*)nai_rbtree_end(t)) {
        if (!nai_str_eq(&l->key, str)) {
            l = (nai_rbnode_str_t*)nai_rbtree_end(t);
        };
    };

    return l;
};


nai_rbnode_str_t* nai_rbtree_str_lbound(
    const nai_rbtree_t* t, const nai_str_t* str, nai_int_t cases)
{
    nai_rbnode_t** n = (nai_rbnode_t**)&nai_rbtree_root(t);
    nai_rbnode_t* parent;
    nai_rbnode_str_t* e;
    nai_rbnode_str_t* l = (nai_rbnode_str_t*)nai_rbtree_end(t);


    if (cases == 0) {
        while (*n) {
            parent = *n;
            e = (nai_rbnode_str_t*)parent;
            if (nai_str_cmp(&e->key, str) >= 0) {
                l = e;
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
    } else {
        while (*n) {
            parent = *n;
            e = (nai_rbnode_str_t*)parent;
            if (nai_str_casecmp(&e->key, str) >= 0) {
                l = e;
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
    };

    return l;
};


nai_rbnode_str_t* nai_rbtree_str_ubound(
    const nai_rbtree_t* t, const nai_str_t* str, nai_int_t cases)
{
    nai_rbnode_t** n = (nai_rbnode_t**)&nai_rbtree_root(t);
    nai_rbnode_t* parent;
    nai_rbnode_str_t* e;
    nai_rbnode_str_t* l = (nai_rbnode_str_t*)nai_rbtree_end(t);


    if (cases == 0) {
        while (*n) {
            parent = *n;
            e = (nai_rbnode_str_t*)parent;
            if (nai_str_cmp(str, &e->key) < 0) {
                l = e;
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
    } else {
        while (*n) {
            parent = *n;
            e = (nai_rbnode_str_t*)parent;
            if (nai_str_casecmp(str, &e->key) < 0) {
                l = e;
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
    };

    return l;
};


nai_int_t nai_rbtree_str_insert(
    nai_rbtree_t* t, nai_rbnode_str_t* i, nai_int_t cases)
{
    nai_rbnode_t** n = (nai_rbnode_t**)&nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nai_rbnode_str_t* e = 0;


    if (cases == 0) {
        while (*n) {
            parent = *n;
            e = (nai_rbnode_str_t*)parent;
            if (nai_str_cmp(&i->key, &e->key) < 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
    } else {
        while (*n) {
            parent = *n;
            e = (nai_rbnode_str_t*)parent;
            if (nai_str_casecmp(&i->key, &e->key) < 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
    };

    nai_rbtree_link(t, &i->ent, parent, n);
    nai_rbtree_color(t, &i->ent);

    return 0;
};


