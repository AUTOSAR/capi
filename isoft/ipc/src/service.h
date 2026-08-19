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
/// @file       service.h
/// @brief
/// @details
/// @date       2022-12-21
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_SERVICE_H
#define IPC_SERVICE_H

#include <isoft/ipc/ipc.h>
#include <stdlib.h>

#define IPC_SERVICE_NAME_LENGTH 4

static inline bool ipc_service_check_path(const char *path, size_t len)
{
    if (len < 7 || len > IPC_ADDRESS_LENGTH_MAX) {
        return false;
    }

    if (path[len - 5] != '/') {
        return false;
    }

    if (path[len - 4] == '/' || path[len - 3] == '/' || path[len - 2] == '/' || path[len - 1] == '/') {
        return false;
    }

    return true;
}

static inline bool ipc_service_check_name(const char *name)
{
    if (name[4] != '\0') {
        return false;
    }
    if (name[0] == '\0' || name[1] == '\0' || name[2] == '\0' || name[3] == '\0') {
        return false;
    }
    return true;
}

// just 4 bytes
static inline uint32_t ipc_service_extract_id_from_name(const char *name)
{
    uint32_t id;
    memcpy(&id, name, sizeof(uint32_t));
    return id;
}

#endif
