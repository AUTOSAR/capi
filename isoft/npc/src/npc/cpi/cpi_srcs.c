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
/// @file       cpi_srcs.c
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "npc/cpi/cpi_srcs.h"

static cpi_string_t cpi_srcs_files[] = {
    cpi_string("cpi_app.c"),
    cpi_string("cpi_buf.c"),
    cpi_string("cpi_cust.c"),
    cpi_string("cpi_endpoint.c"),
    cpi_string("cpi_event.c"),
    cpi_string("cpi_frame.c"),
    cpi_string("cpi_mm.c"),
    cpi_string("cpi_service.c"),
    cpi_string("cpi_string.c"),
    cpi_string("cpi_app_ep_cb.c"),
    cpi_string("cpi_conf.c"),
    cpi_string("cpi_dis.c"),
    cpi_string("cpi_error.c"),
    cpi_string("cpi_exec.c"),
    cpi_string("cpi_lock.c"),
    cpi_string("cpi_rbtree.c"),
    cpi_string("cpi_srcs.c"),
    cpi_string("cpi_svc.c"),
};

int cpi_srcs_index(cpi_string_t *filename)
{
    unsigned long i, idx = 0;
    for (i = 0; i < sizeof(cpi_srcs_files)/sizeof(cpi_string_t); ++i) {
        if (!cpi_string_strcmp(filename, &(cpi_srcs_files[i]))) {
            idx = i;
            break;
        }
    }
    return idx;
}

char *cpi_srcs_file(int index)
{
    return (char *)(cpi_srcs_files[index].data);
}

