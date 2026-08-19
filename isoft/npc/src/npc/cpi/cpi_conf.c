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
/// @file       cpi_conf.c
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "npc/cpi/cpi_mm.h"
#include "npc/cpi/cpi_error.h"
#include "npc/cpi/cpi_conf.h"

static int cpi_conf_cmp(const cpi_conf_item_t *ci1, const cpi_conf_item_t *ci2);
static void cpi_conf_item_free(cpi_conf_item_t *ci);

cpi_conf_t *cpi_conf_init(void)
{
    struct cpi_rbtree_attr rbattr;

    rbattr.pool = NULL;
    rbattr.pool_alloc = NULL;
    rbattr.pool_free = NULL;
    rbattr.cmp = (rbtree_cmp)cpi_conf_cmp;
    rbattr.data_free = (rbtree_free_data)cpi_conf_item_free;
    rbattr.cache = 0;
    return cpi_rbtree_init(&rbattr);
}

void cpi_conf_close(cpi_conf_t *cf)
{
    cpi_rbtree_destroy(cf);
}

static cpi_conf_item_t *cpi_conf_item_new(cpi_conf_item_type_t type, cpi_string_t *key, void *val)
{
    cpi_conf_item_t *ci = (cpi_conf_item_t *)cpi_malloc(sizeof(cpi_conf_item_t));
    if (ci == NULL)
        goto out;

    ci->type = type;
    ci->key = key;
    switch (type) {
    case cpi_conf_type_string:
        ci->val.s = (cpi_string_t *)val;
        break;
    case cpi_conf_type_num:
    case cpi_conf_type_true:
    case cpi_conf_type_false:
        ci->val.i = *(cpi_s64_t *)val;
        break;
    default:
        cpi_free(ci);
        ci = NULL;
        goto out;
    }

out:
    return ci;
}

static void cpi_conf_item_free(cpi_conf_item_t *ci)
{
    if (ci != NULL) {
        if (ci->type == cpi_conf_type_string && ci->val.s != NULL) {
            cpi_string_free(ci->val.s);
        }
        if (ci->key != NULL)
            cpi_string_free(ci->key);
        cpi_free(ci);
    }
}

int cpi_conf_update(cpi_conf_t *conf, cpi_conf_item_type_t type, cpi_string_t *key, void *val)
{
    cpi_string_t *k = NULL, *v = NULL;
    cpi_rbtree_node_t *rn;
    cpi_conf_item_t *ci;
    int r = RET(CPI_ESUCC);

    if ((k = cpi_string_dup(key)) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    if (type == cpi_conf_type_string) {
        if ((v = cpi_string_dup((cpi_string_t *)val)) == NULL) {
            cpi_string_free(k);
            r = RET(CPI_ENMEM);
            goto out;
        }
        ci = cpi_conf_item_new(type, k, v);
    } else {
        ci = cpi_conf_item_new(type, k, val);
    }
    if (ci == NULL) {
        if (v != NULL)
            cpi_string_free(v);
        cpi_string_free(k);
        r = RET(CPI_ENMEM);
        goto out;
    }

    rn = cpi_rbtree_search(conf, ci);
    if (!cpi_rbtree_null(rn)) {
        cpi_rbtree_delete(rn);
        cpi_rbtree_node_free(rn);
    }

    if ((rn = cpi_rbtree_node_new((cpi_rbtree_t *)conf, ci)) == NULL) {
        cpi_conf_item_free(ci);
        r = RET(CPI_ENMEM);
        goto out;
    }
    cpi_rbtree_insert(rn);

out:
    return r;
}

cpi_conf_item_t *cpi_conf_search(cpi_conf_t *conf, cpi_string_t *key)
{
    cpi_conf_item_t ci, *r;
    cpi_rbtree_node_t *rn;

    ci.key = key;
    rn = cpi_rbtree_search(conf, &ci);
    if (cpi_rbtree_null(rn)) {
        r = NULL;
    } else {
        r = (cpi_conf_item_t *)(rn->data);
    }
    return r;
}

static int cpi_conf_cmp(const cpi_conf_item_t *ci1, const cpi_conf_item_t *ci2)
{
    return cpi_string_strcmp(ci1->key, ci2->key);
}

