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
/// @file       cpi_event.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_EVENT_H
#define __CPI_EVENT_H

#include "npc/cpi/cpi_types.h"
#include "npc/cpi/cpi_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CPI_ET_UNKNOWN 0
#define CPI_ET_EVENT   1
#define CPI_ET_FIELD   2

#define CPI_RT_UNKNOWN    0
#define CPI_RT_RELIABLE   1
#define CPI_RT_UNRELIABLE 2
#define CPI_RT_BOTH       3

    typedef struct cpi_event_group_s cpi_event_group_t;
    typedef struct cpi_event_s cpi_event_t;
    typedef struct cpi_subscription_s cpi_subscription_t;
    typedef struct cpi_event_info_s cpi_event_info_t;

    typedef int (*cpi_subscription_iterate_handler)(cpi_message_t *, int, void *);

    typedef enum cpi_event_type_e
    {
        cpi_event_ev,
        cpi_event_field
    } cpi_event_type_t;

    struct cpi_event_group_s
    {
        cpi_rbtree_t *events;
        cpi_u16_t id;
    };

    struct cpi_event_s
    {
        cpi_rbtree_t *groups;
        cpi_rbtree_t *subscribers;
        cpi_u16_t id;
        cpi_event_type_t type;
    };

    struct cpi_subscription_s
    {
        cpi_u16_t gid;
        cpi_u16_t eid;
        cpi_u32_t appid;
    };

    struct cpi_event_info_s
    {
        cpi_s32_t type;
        cpi_s32_t reliability;
        cpi_s32_t ngroup;
        cpi_u16_t *groups;
    };

    cpi_event_group_t *cpi_event_group_new(cpi_u16_t id);
    void cpi_event_group_free(cpi_event_group_t *eg);
    int cpi_event_group_add_event(cpi_rbtree_t *grps, cpi_rbtree_t *evs, cpi_s32_t gid, cpi_s32_t eid);
    cpi_event_t *cpi_event_group_event_search(cpi_event_group_t *group, cpi_u16_t event_id);
    int cpi_event_group_cmp(const cpi_event_group_t *eg1, const cpi_event_group_t *eg2);
    int cpi_event_group_add(cpi_rbtree_t *t, cpi_event_group_t *grp);
    void cpi_event_group_remove_event(cpi_rbtree_t *grps, cpi_event_t *ev);

    cpi_event_t *cpi_event_new(cpi_u16_t id, cpi_event_type_t type);
    void cpi_event_free(cpi_event_t *e);
    int cpi_event_cmp(const cpi_event_t *e1, const cpi_event_t *e2);
    cpi_event_t *cpi_event_search(cpi_rbtree_t *t, cpi_u16_t eid);
    int cpi_event_add(cpi_rbtree_t *t, cpi_event_t *e);
    void cpi_event_remove(cpi_rbtree_t *t, cpi_u16_t eid);

#define cpi_subscription_setid(svc, a, g, e) ((svc)->gid = (g), (svc)->eid = (e), (svc)->appid = (a))
#define cpi_subscription_get_appid(sub)      ((sub)->appid)
#define cpi_subscription_get_gid(sub)        ((sub)->gid)
#define cpi_subscription_get_eid(sub)        ((sub)->eid)
    cpi_subscription_t *cpi_subscription_new(cpi_u32_t appid, cpi_u16_t gid, cpi_u16_t eid);
    void cpi_subscription_free(cpi_subscription_t *s);
    int cpi_subscription_cmp(const cpi_subscription_t *s1, const cpi_subscription_t *s2);
    void cpi_subscription_remove_event(cpi_rbtree_t *subscribers, cpi_rbtree_t *subscriptions, cpi_event_t *ev);
    cpi_subscription_t *cpi_subscription_search(cpi_rbtree_t *t, int appid, cpi_u16_t gid, cpi_u16_t eid);
    int cpi_subscription_add(cpi_rbtree_t *subscriptions, cpi_rbtree_t *subscribers, cpi_subscription_t *sub);
    void cpi_subscription_remove(cpi_rbtree_t *subscriptions, cpi_rbtree_t *subscribers, cpi_subscription_t *sub);
    int cpi_subscription_collect_peers(cpi_rbtree_t *subs,
                                       cpi_subscription_iterate_handler handler,
                                       cpi_message_t *m,
                                       void *udata);

#ifdef __cplusplus
}
#endif

#endif
