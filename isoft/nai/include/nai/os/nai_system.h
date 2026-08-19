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
/// @file       nai_system.h
/// @brief      system information and basic interface
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation provides basic system information, such as 
 *          the number of cpu cores, page size, disk cluster size, and system 
 *          time.
 *
 * @details the code example is as follows:
 *
 * @par     initialize the library
 * @code
 *          int main()
 *          {
 *              nai_int_t r;
 *
 *              // should be called first to initialize some sub-modules
 *              r = nai_init();
 *              if (r < 0) {
 *                  goto _fail;
 *              };
 *          };
 * @endcode
 */

#ifndef _SYSTEM_H_NAI
#define _SYSTEM_H_NAI

#pragma once

#include "nai/nai_config.h"
#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NAI_TIME_UNIT
 * @name    NAI_TIME_UNIT       the flags of time units
 * @{
 */
#define NAI_TIME_MSEC 0x0 /**< in milli-seconds unit */
#define NAI_TIME_USEC 0x1 /**< in micro-seconds unit */
/** @} */

/**
 * @anchor  nai_time_defines
 * @name    nai_time_defines
 * @{
 */

/**
 * convert structure timeval to milli-second time
 * @param   t       pointer to the timeval
 * @return  the value of time, in milli-seconds
 */
#define nai_timeval_to_msec(t) ( (uint64_t)((t)->tv_sec) * (1000) + (t)->tv_usec / (1000))

/**
 * convert structure timeval to micro-second time
 * @param   t       pointer to the timeval
 * @return  the value of time, in micro-seconds
 */
#define nai_timeval_to_usec(t) ( (uint64_t)((t)->tv_sec) * (1000 * 1000) + (t)->tv_usec)

/**
 * convert structure timespec to milli-second time
 * @param   t       pointer to the timespec
 * @return  the value of time, in milli-seconds
 */
#define nai_timespec_to_msec(t) ( (uint64_t)((t)->tv_sec) * (1000) + (t)->tv_nsec / (1000 * 1000))

/**
 * convert structure timespec to micro-second time
 * @param   t       pointer to the timespec
 * @return  the value of time, in micro-seconds
 */
#define nai_timespec_to_usec(t) ( (uint64_t)((t)->tv_sec) * (1000 * 1000) + (t)->tv_nsec / 1000)

/* macro of nai_tick */
#define nai_tick_to_msec() nai_tick(NAI_TIME_MSEC)
#define nai_tick_to_usec() nai_tick(NAI_TIME_USEC)

/* macro of nai_tick32 */
#define nai_tick_to_msec32() nai_tick32(NAI_TIME_MSEC)
#define nai_tick_to_usec32() nai_tick32(NAI_TIME_USEC)

    /** @} */

    /** the size of memory page */
    NAI_EXTERN
    extern uint32_t nai_pagesize;

    /** the bits of memory page */
    NAI_EXTERN
    extern uint32_t nai_pagesize_shift;

    /** the size of cpu cacheline */
    NAI_EXTERN
    extern uint32_t nai_cacheline_size;

    /** the default size of sector which is used for direct io.
 *  default value: 512
 *  @note   we should modify it to adapt to the current disk.
 *          setting the value too large will cause waste of 
 *          memory and disk throughput.
 *          on windows, the value is physical sector size.
 *          on linux, the value is logical sector size.
 */
    NAI_EXTERN
    extern uint32_t nai_sector_size;

    /**
 * initial the system, ie. winsock on win32
 * @return  if success zero is returned, othewise -1 is returned
 * @note    this function function should be called before all the code 
 *          in the application
 */
    NAI_EXTERN
    nai_int_t nai_init();

    /**
 * get current time
 * @return  the value of current time, in mirco-seconds
 */
    NAI_EXTERN
    uint64_t nai_time();

    /**
 * get current tick
 * @param   units   the flags of time units, see @ref NAI_TIME_UNIT
 * @return  the value of current tick, in milli|mirco-seconds
 */
    NAI_EXTERN
    uint64_t nai_tick(nai_int_t units);

    /**
 * get current tick
 * @param   units   the flags of time units, see @ref NAI_TIME_UNIT
 * @return  the value of current tick, in milli|mirco-seconds
 */
    NAI_EXTERN
    uint32_t nai_tick32(nai_int_t units);

    /**
 * sleep the specified time in mirco-seconds
 * @param   usec    execution suspends the specified time interval, 
 *                  in micro-seconds
 * @return  if success zero is returned, othewise -1 is returned
 */
    NAI_EXTERN
    nai_int_t nai_sleep(uint64_t usec);

    /**
 * yield the processor
 * @return  if success zero is returned, othewise -1 is returned
 */
    NAI_EXTERN
    nai_int_t nai_yield();

    /**
 * get cpu cores
 * @param   logical if set 0, only returns the real cores
 * @return  the number of cpu cores
 */
    NAI_EXTERN
    nai_int_t nai_nprocs(nai_int_t logical);

#if defined(_WIN32)

    NAI_EXTERN
    nai_int_t nai_errno_from_oserr(unsigned long oserrno);

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
