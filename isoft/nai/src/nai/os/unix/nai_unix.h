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
/// @file       nai_unix.h
/// @brief      
/// @details
/// @date       2021-06-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _UNIX_H_NAI
#define _UNIX_H_NAI

#pragma once

#include "nai/runtime/nai_types.h"

#if (NAI_HAVE_SIGNAL_H)
    #include <signal.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
 * the structure of the sched parameters
 */
    typedef struct nai_sched_priority_s
    {
        nai_int_t sched;    /**< the sched class */
        nai_int_t priority; /**< the sched priority */
    } nai_sched_priority_t;

    /**
 * convert the sched and priority to the value of the current system
 * @param   p       pointer to the sched parameters
 * @param   sched   the value of the sched class
 * @param   priority the value of the sched priority
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_sched_query_priority(nai_sched_priority_t* p, nai_int_t sched, nai_int_t priority);

    /**
 * examine and change blocked signals
 * @param   op      the value of blocked signal opeartion
 * @param   newset  pointer to the new signal masks
 * @param   oldset  pointer to the old signal masks
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_sigprocmask(nai_int_t op, sigset_t* newset, sigset_t* oldset);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
