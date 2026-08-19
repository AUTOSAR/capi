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
/// @file       icc_endpoint.c
/// @brief
/// @details
/// @date       2026-02-13
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#include "icc/core/icc_log.h"
#include "icc/core/icc_routing.h"
#include "icc_network.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"

//////////////////////////////////////////////////////////////////////////////
// endpoint

extern icc_endpoint_ops_t icc_endpoint_fd_ops;
extern icc_endpoint_ops_t icc_endpoint_msgbox_ops;
static icc_endpoint_ops_t* icc_endpoint_ops[] = {
    &icc_endpoint_fd_ops,
    &icc_endpoint_msgbox_ops,
};

icc_endpoint_t* icc_endpoint_create(
    icc_network_t* p, uint16_t icc_type, const nai_str_t* icc_identifier, icc_serv_t serv, icc_inst_t inst)
{
    intptr_t r;
    int ec;
    int idx;
    icc_endpoint_t* e;

    nai_log_info(ICC_LOG_CORE, 0, "create endpoint %d:%s", icc_type, nai_str(icc_identifier));

    e = (icc_endpoint_t*)icc_network_alloc(p, sizeof(*e));
    if (e == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "allocate endpoint failed");
        goto _end;
    };

    idx = e->icc_type;

    e->flags          = 0;
    e->serv           = serv;
    e->inst           = inst;
    e->net            = p;
    e->cb             = p->cb;
    e->ud             = 0;
    e->ops            = icc_endpoint_ops[idx];
    e->ctx            = 0;
    e->pexit          = 0;
    e->icc_type       = icc_type;
    e->icc_identifier = *icc_identifier;

    /* open endpoint */
    r = e->ops->open(e);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(ICC_LOG_CORE, ec, "failed to create endpoint %d:%s", icc_type, nai_str(&e->icc_identifier));
        icc_network_free(p, e);
        nai_errno = ec;
        e         = 0;
        goto _end;
    };

_end:
    return e;
};

int icc_endpoint_send(icc_endpoint_t* e, icc_message_t* m, uint32_t uid, int flags)
{
    int r;

    assert(m != 0);
    assert(e != 0);
    assert(e->ops != 0);

    r = e->ops->send(e, m, uid, flags);

    return r;
};

int icc_endpoint_flush(icc_endpoint_t* e)
{
    int r;

    assert(e != 0);
    assert(e->ops != 0);

    r = e->ops->flush(e);

    return r;
};

int icc_endpoint_close(icc_endpoint_t* e)
{
    int r;
    int idx;
    icc_network_t* p;

    if (e->closing) {
        r = 0;
        goto _end;
    };

    nai_log_info(ICC_LOG_CORE, 0, "close endpoint %d:%s", e->icc_type, nai_str(&e->icc_identifier));

    /* mark closing */
    e->closing = 1;

    /* close */
    r = e->ops->close(e);
    if (r < 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to close endpoint %d:%s", e->icc_type,
                      nai_str(&e->icc_identifier));
        e->closing = 0;
        goto _end;
    };

    /* remove from map */
    p = e->net;
    if (p != 0) {
        idx = e->icc_type;
        nai_rbtree_erase(&p->eps[idx], &e->ent);
        e->net = 0;
    };

    /* mark closed */
    if (e->pexit) {
        e->pexit[0] = 1;
    };

    /* free */
    icc_network_free(p, e);
    r = 0;

_end:
    return r;
};

nai_buf_t* icc_network_create_buf(icc_network_t* n);
int icc_network_release_buf(icc_network_t* n, nai_buf_t* b);

int icc_endpoint_realloc_buf(icc_endpoint_t* p, nai_buf_t** pbuf)
{
    int r;
    int ec;
    nai_buf_t* b;
    nai_buf_t* c;
    icc_network_t* n;

    n = p->net;
    b = pbuf[0];
    if (b != 0) {
        /* if no extra reference, we direct reset buffer */
        if (b->refcount <= 1) {
            nai_memmove(b->ref.obj, b->start, b->size);
            b->total += (b->start - (uint8_t*)b->ref.obj);
            b->start = (uint8_t*)b->ref.obj;
            r        = 0;
            goto _end;
        };
    };

    c = icc_network_create_buf(n);
    if (c == 0) {
        nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to allocate buffer for endpoint %d:%s", p->icc_type,
                      nai_str(&p->icc_identifier));
        r = -1;
        goto _end;
    };

    if (b != 0) {
        nai_memcpy(c->start, b->start, b->size);
        nai_buf_wcommit(c, b->size);

        r = icc_network_release_buf(n, b);
        if (r < 0) {
            ec = nai_errno;
            icc_network_release_buf(n, c);
            nai_errno = ec;
            goto _end;
        };
    };

    pbuf[0] = c;
    r       = 0;

_end:
    return r;
};

int icc_endpoint_release_buf(icc_endpoint_t* p, nai_buf_t* buf)
{
    int r;

    r = icc_network_release_buf(p->net, buf);

    return r;
};

//////////////////////////////////////////////////////////////////////////////
// endpoint callbacks

int icc_endpoint_handle_sent(icc_endpoint_t* p, nai_list_entry_t* msgs, size_t* qsize, int errcode)
{
    int r;
    int* pexit;
    int* pexitbk;
    int exit = 0;
    icc_message_t* m;

    if (p == 0 || p->cb == 0) {
        r = NAI_DECLINED;
        goto _close;
    };

    pexitbk = p->pexit;
    if (!pexitbk) {
        p->pexit = &exit;
    };
    pexit = p->pexit;

    while (!nai_list_is_empty(msgs)) {
        m = (icc_message_t*)msgs->next;
        qsize[0] -= m->hdr.len;
        nai_list_entry_remove(&m->ent);
        r = p->cb->sent(p, m, errcode);
        icc_message_close(m);

        if (*pexit) {
            r = 0;
            goto _close;
        };
    };

    (void)r;

    p->pexit = pexitbk;
    r        = NAI_DECLINED;

_close:
    while (!nai_list_is_empty(msgs)) {
        m = (icc_message_t*)msgs->next;
        qsize[0] -= m->hdr.len;
        nai_list_entry_remove(&m->ent);
        icc_message_close(m);
    };

    return r;
};

int icc_endpoint_handle_incoming(icc_endpoint_t* p, icc_message_t* m)
{
    int r;
    int* pexit;
    int* pexitbk;
    int exit = 0;

    if (p->cb == 0) {
        goto _close;
    };

    pexitbk = p->pexit;
    if (!pexitbk) {
        p->pexit = &exit;
    };
    pexit = p->pexit;

    r = p->cb->message(p, m);

    if (*pexit) {
        r = 0;
        goto _end;
    };

    p->pexit = pexitbk;
    m        = 0;

_close:
    (void)r;

    r = NAI_DECLINED;
    if (m) {
        icc_message_close(m);
    };

_end:
    return r;
};
