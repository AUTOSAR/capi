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
/// @file       cpi_frame.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_FRAME_H
#define __CPI_FRAME_H


#include "nai/io/nai_event.h"
#include "npc/shm/npc_port.h"
#include "npc/cpi/cpi_exec.h"
#include "npc/cpi/cpi_endpoint.h"
#include "npc/cpi/cpi_const.h"
#include <sys/socket.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct cpi_frame_s cpi_frame_t;
    typedef struct cpi_frame_info_s cpi_frame_info_t;
    typedef enum cpi_frame_op_e cpi_frame_op_t;

    typedef int (*cpi_frame_op_cb_t)(cpi_endpoint_t *ep, cpi_frame_t *f);

    enum cpi_frame_op_e
    {
        cpi_frame_op_peer_id = 0,
        cpi_frame_op_offer_service,
        cpi_frame_op_offer_service_ack,
        cpi_frame_op_offer_service_nack,
        cpi_frame_op_stop_service,
        cpi_frame_op_stop_service_ack,
        cpi_frame_op_stop_service_nack,
        cpi_frame_op_find_service,
        cpi_frame_op_find_service_ack,
        cpi_frame_op_find_request_port,
        cpi_frame_op_find_request_port_ack,
        cpi_frame_op_stop_find_service,
        cpi_frame_op_stop_find_service_ack,
        cpi_frame_op_stop_find_service_notice,
        cpi_frame_op_subscribe,
        cpi_frame_op_subscribe_ack,
        cpi_frame_op_unsubscribe,
        cpi_frame_op_send,
        cpi_frame_op_max
    };

    struct cpi_frame_info_s
    {
        cpi_u16_t inst;
        cpi_u16_t svc;
        cpi_u32_t major;
        cpi_u32_t minor;
        cpi_u16_t gid;
        cpi_u16_t eid;
        cpi_s32_t appid;
        npc_mref_t read;
        npc_mref_t write;
        union
        {
            int code;
            cpi_u32_t event;
        };
        cpi_endpoint_addr_t addr;
        cpi_exec_t *e;
        cpi_s8_t shm_path[CPI_SHM_PATH_LEN];
        cpi_u32_t seg_size;
        cpi_u32_t seg_count;
    };

    struct cpi_frame_s
    {
        cpi_frame_info_t src;
        cpi_frame_info_t dest;
        cpi_frame_op_t op;
    };

#define cpi_frame_init(f) memset(f, 0, sizeof(*f))

    int cpi_frame_callback(cpi_endpoint_t *ep, cpi_frame_t *f);

#ifdef __cplusplus
}
#endif

#endif
