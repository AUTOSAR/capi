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
/// @file       cpi_conf.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_CONF_H
#define __CPI_CONF_H

#include "npc/cpi/cpi_string.h"
#include "npc/cpi/cpi_types.h"
#include "npc/cpi/cpi_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
 * json
 * {
 *     netname: "ipc",
 *     appname: 'app',
 *     appid: 1,
 *     shm_size: 1024,
 *     host: '/tmp',
 *     svc_net_type: 'unixsock',
 *     svc_access_handle: 'svc1.sock',
 *     dis_net_type: 'unixsock',
 *     dis_access_handle: 'dis.sock',
 *     mode: 'discovery', //or 'application'
 *     service: [{
 *         service: 1,
 *         instance: 1,
 *         major: 1,
 *         minor: 1,
 *         shm: '1_1.shm',
 *         seg_size: 10485760,
 *         seg_count: 1,
 *         msg_queue_len: 16,
 *     }]
 * }
 *
 * appid
 * shm_size
 * host
 * mode
 * svc_net_type
 * svc_access_handle
 * dis_net_type
 * dis_access_handle
 * nservice
 * service.0.service
 * service.0.instance
 * service.0.major
 * service.0.minor
 * service.0.ngroup
 * service.0.shm_path
 * service.0.shm_seg_size
 * service.0.shm_seg_count
 * service.0.msg_queue_len
 */

    typedef cpi_rbtree_t cpi_conf_t;
    typedef struct cpi_conf_item_s cpi_conf_item_t;

    typedef enum cpi_conf_item_type_e
    {
        cpi_conf_type_string,
        cpi_conf_type_num,
        cpi_conf_type_true,
        cpi_conf_type_false,
        cpi_conf_type_null
    } cpi_conf_item_type_t;

    struct cpi_conf_item_s
    {
        cpi_conf_item_type_t type;
        cpi_string_t *key;
        union
        {
            cpi_string_t *s;
            cpi_s64_t i;
        } val;
    };

#define cpi_conf_get_type(ci)       ((ci)->type)
#define cpi_conf_get_string_val(ci) ((ci)->val.s)
#define cpi_conf_get_int_val(ci)    ((ci)->val.i)

    NPC_EXTERN
    int cpi_conf_update(cpi_conf_t *conf, cpi_conf_item_type_t type, cpi_string_t *key, void *val);
    NPC_EXTERN
    cpi_conf_item_t *cpi_conf_search(cpi_conf_t *conf, cpi_string_t *key);
    NPC_EXTERN
    cpi_conf_t *cpi_conf_init(void);
    NPC_EXTERN
    void cpi_conf_close(cpi_conf_t *cf);

#ifdef __cplusplus
}
#endif

#endif
