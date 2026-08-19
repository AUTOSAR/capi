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
/// @file       cpi_exec.c
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "npc/cpi/cpi_app.h"
#include "npc/cpi/cpi_exec.h"
#include "npc/cpi/cpi_error.h"
#include "npc/cpi/cpi_mm.h"
#include "npc/cpi/cpi_endpoint.h"
#include "npc/cpi/cpi_error.h"

CPI_CHAIN_FUNC_DECLARE(cpi_exec, cpi_exec_t, static inline void, );
CPI_CHAIN_FUNC_DEFINE(cpi_exec, cpi_exec_t, static inline void, prev, next);

/*
 * cpi_exec_t
 */
int cpi_exec(void *app, cpi_exec_cb_t cb, void *data, cpi_u32_t nowait)
{
    cpi_app_t *a = (cpi_app_t *)app;
    int r = RET(CPI_ESUCC);
    cpi_exec_t *e;

    if ((e = cpi_malloc(sizeof(cpi_exec_t))) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }

    if (cpi_app_evloop_get(a) == NULL || nai_evloop_in_dispatch(cpi_app_evloop_get(a))) {
        e->app = a;
        e->error = CPI_ESUCC;
        e->cb = cb;
        e->data = data;
        e->nowait = nowait;
        e->transfer = 0;
        if (pthread_mutex_init(&e->lock, NULL) != 0) {
            r = RET(CPI_EMUTE);
            goto out;
        }
        if (pthread_cond_init(&e->cond, NULL) != 0) {
            r = RET(CPI_EMUTE);
            goto out;
        }
        r = cb(app, &e->data);
        pthread_cond_destroy(&e->cond);
        pthread_mutex_destroy(&e->lock);
        goto out;
    }

    e->app = a;
    e->error = CPI_ESUCC;
    e->cb = cb;
    e->data = data;
    e->transfer = 0;
    e->nowait = nowait;
    if (pthread_mutex_init(&e->lock, NULL) != 0) {
        r = RET(CPI_EMUTE);
        goto out;
    }
    if (pthread_cond_init(&e->cond, NULL) != 0) {
        r = RET(CPI_EMUTE);
        goto out;
    }
    e->next = e->prev = NULL;

    cpi_app_spin_lock(a);
    cpi_exec_chain_add(cpi_app_get_execq(a), e);
    cpi_app_spin_unlock(a);

    if (!nowait) {
        pthread_mutex_lock(&e->lock);
        nai_evnode_post(cpi_app_get_evnode(a), 0);

        log("---LOG--- before wait %p\n", e);
        pthread_cond_wait(&e->cond, &e->lock);
        log("---LOG--- after wait %p\n", e);

        pthread_mutex_unlock(&e->lock);

        pthread_cond_destroy(&e->cond);
        pthread_mutex_destroy(&e->lock);
        r = e->error;
        cpi_free(e);
    } else {
        nai_evnode_post(cpi_app_get_evnode(a), 0);
    }

out:
    return r;
}

int cpi_exec_cb(nai_evnode_t *node, int events)
{
    cpi_app_t *app = nai_containof(node, cpi_app_t, node);
    cpi_exec_t *e;

    (void)events;

lp:
    cpi_spin_lock(&app->lock);
    e = app->head;
    if (e == NULL) {
        goto out;
    }

    cpi_exec_chain_del(cpi_app_get_execq(app), e);
    cpi_spin_unlock(&app->lock);

    if (e->cb != NULL)
        e->error = e->cb(app, &e->data);

    if (!e->transfer) {
        if (!e->nowait) {
            cpi_exec_feedback(e);
        } else {
            pthread_cond_destroy(&e->cond);
            pthread_mutex_destroy(&e->lock);
            cpi_free(e);
        }
    }
    goto lp;

out:
    cpi_spin_unlock(&app->lock);
    return RET(CPI_ESUCC);
}

void cpi_exec_feedback(cpi_exec_t *e)
{
    pthread_mutex_lock(&e->lock);
    pthread_cond_signal(&e->cond);
    pthread_mutex_unlock(&e->lock);
}

void cpi_exec_transfer(cpi_exec_t *e, void *ep)
{
    cpi_endpoint_t *endpoint = (cpi_endpoint_t *)ep;
    if (!e->transfer && ep != NULL) {
        assert(e->nowait == 0);
        cpi_exec_chain_add(cpi_endpoint_e_queue(endpoint), e);
        e->transfer = 1;
    }
}

void cpi_exec_no_transfer(cpi_exec_t *e, void *ep)
{
    cpi_endpoint_t *endpoint = (cpi_endpoint_t *)ep;
    if (e->transfer && ep != NULL) {
        cpi_exec_chain_del(cpi_endpoint_e_queue(endpoint), e);
        e->transfer = 0;
    }
}

