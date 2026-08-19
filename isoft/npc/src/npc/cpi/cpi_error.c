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
/// @file       cpi_error.c
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "npc/cpi/cpi_error.h"
#include <stdio.h>

static char *err_msg[] = {
    "Success",
    "No memory",
    "Invalid configuration",
    "Bind error",
    "Connect error",
    "Service exists",
    "Group not exists",
    "Event not exists",
    "Connection status error",
    "Accept error",
    "Receive error",
    "Connection close",
    "Send error",
    "Buffer length error",
    "Pthread condition error",
    "Pthread mutex error",
    "Invalid value",
    "Service operation error",
    "No entry",
    "No such service",
    "No such endpoint",
    "No such event",
    "No permission",
    "No such subscription",
    "Comm manager error",
    "Resource busy",
    "Comif error",
    "Port operation error",
    "No such service consumer",
    "Port not initialized",
    "Group exists",
    "No such application",
    "Comif_event error",
    "Resource temporarily unavailable",
    "Callback error",
};

char *cpi_error(int err, void *buf, cpi_size_t len)
{
    int i = 0, line = 0;
    char *b = (char *)buf;

    if (err) {
        err = -err;
        i = (err >> 24) & 0xff;
        line = (err >> 8) & 0xffff;
        err &= 0xff;

        i = snprintf(b, len - 1, "%s:%d:%s", cpi_srcs_file(i), line, err_msg[err]);
    } else {
        i = snprintf(b, len - 1, "Success");
    }
    b[i] = 0;

    return b;
}

