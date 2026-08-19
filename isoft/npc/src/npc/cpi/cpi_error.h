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
/// @file       cpi_error.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_ERROR_H
#define __CPI_ERROR_H


#include "nai/runtime/nai_log.h"
#include "npc/core/npc_types.h"
#include "npc/cpi/cpi_srcs.h"
#include <string.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CPI_ESUCC 0
#define CPI_ENMEM 1
#define CPI_ECONF 2
#define CPI_EBIND 3
#define CPI_ECONN 4
#define CPI_ESEXI 5
#define CPI_EGNEX 6
#define CPI_EENEX 7
#define CPI_ECONS 8
#define CPI_EACPT 9
#define CPI_ERECV 10
#define CPI_ECLOS 11
#define CPI_ESEND 12
#define CPI_EBLEN 13
#define CPI_ECOND 14
#define CPI_EMUTE 15
#define CPI_EINVA 16
#define CPI_ESOPR 17
#define CPI_ENENT 18
#define CPI_ENSVC 19
#define CPI_ENENP 20
#define CPI_ENEVT 21
#define CPI_EPERM 22
#define CPI_ENSUB 23
#define CPI_ECOMM 24
#define CPI_EBUSY 25
#define CPI_ECMIF 26
#define CPI_EOPPT 27
#define CPI_ENSCR 28
#define CPI_EPORT 29
#define CPI_EGEXI 30
#define CPI_ENAPP 31
#define CPI_ECMEV 32
#define CPI_EAGAN 33
#define CPI_ECALL 34

#define log(fmt, ...)                                                                                                  \
    ({                                                                                                                 \
        struct timeval __tv;                                                                                           \
        gettimeofday(&__tv, NULL);                                                                                     \
        nai_log_debug(NAI_LOG_CORE, 0, "%d:%s:%d:%lu: " fmt, getpid(), __FUNCTION__, __LINE__,                         \
                      __tv.tv_sec * 1000000 + __tv.tv_usec, ##__VA_ARGS__);                                            \
    })

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define RET(code)                                                                                                      \
    ({                                                                                                                 \
        int _a;                                                                                                        \
        char _msg[64];                                                                                                 \
        if ((code)) {                                                                                                  \
            cpi_string_t _tmp;                                                                                         \
            cpi_string_set(&_tmp, __FILENAME__);                                                                       \
            _a = (cpi_srcs_index(&_tmp) << 24) | (__LINE__ << 8) | ((code)&0xff);                                      \
            _a = -_a;                                                                                                  \
            if (code != CPI_EBLEN && code != CPI_ENENP) {                                                              \
                nai_log_debug(NAI_LOG_CORE, 0, "%s", cpi_error(_a, _msg, sizeof(_msg)));                               \
            }                                                                                                          \
        } else {                                                                                                       \
            _a = 0;                                                                                                    \
        }                                                                                                              \
        _a;                                                                                                            \
    })

#define CODE(r) ((-(r)) & 0xff)

    NPC_EXTERN
    char *cpi_error(int err, void *buf, cpi_size_t len);

#ifdef __cplusplus
}
#endif

#endif
