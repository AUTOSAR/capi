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
/// @file       nai_proc.h
/// @brief      process management interface
/// @details
/// @date       2020-12-07
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 *
 * @details this implementation supports process creation, signal processing, 
 *          processor binding, priority setting, and resource limits.
 *
 * @details on all platforms, process creation(#nai_proc_spawn) is supported, 
 *          and fork is only available on unix-like systems.
 *
 * @details this implementation ensures that the basic signals are supported: 
 *          SIGTERM, SIGKILL, SIGINT, SIGCONT, SIGSTOP, and others depend on 
 *          the implementation of the specific platform.
 *
 * @details different platforms have different priority definitions, and 
 *          the definitions provided here are similar to posix. for platforms 
 *          that are not compatible with the POSix standard, support through 
 *          simulation.
 *
 * @details the main platforms all provide processor binding support, 
 *          but the mechanism is different. For example, on win32, processor 
 *          binding is for threads, and the processor binding of a process is 
 *          expressed as a collection of all thread bindings. in actual 
 *          development, the setting interface of a process can only provide 
 *          bindings for a group of processors.
 *
 * @details implementation support resource limits: execution time, memory 
 *          working set, child process, file descriptor. this function is not 
 *          supported on win32.
 *
 * @details the code example is as follows:
 *
 * @par     initial the attriubte of the new process
 * @code
 *          nai_int_t r;
 *          nai_proc_attr_t a;
 *
 *          nai_proc_attr_init(&a);
 *
 *          r = nai_proc_attr_open(&a);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     set the basic attributes
 * @code
 *          nai_int_t r;
 *          nai_proc_attr_t* a;
 *
 *          nai_proc_attr_set_dir(a, "./");
 *          nai_proc_attr_set_daemon(a, 0);
 *          nai_proc_attr_set_suspend(a, 1); // suspended on create
 * @endcode
 *
 * @par     set idle priority
 * @code
 *          nai_int_t r;
 *          nai_proc_attr_t* a;
 *
 *          nai_proc_attr_set_sched(a, NAI_SCHED_NORMAL, NAI_PRIORITY_LOWEST);
 * @endcode
 *
 * @par     set the processor set
 * @code
 *          nai_int_t r;
 *          nai_cpuset_t set;
 *          nai_proc_attr_t* a;
 *
 *          nai_cpuset_zero(&set);
 *          nai_cpuset_set(0, &set); // to enable cpu 0
 *          nai_cpuset_set(1, &set); // to enable cpu 1
 *
 *          nai_proc_attr_set_affinity(a, NAI_CPUSET_SIZE, &set);
 * @endcode
 *
 * @par     set the limit on the maximum number of file descriptors
 * @code
 *          nai_int_t r;
 *          nai_int_t limit;
 *          nai_rlimit_t l;
 *          nai_proc_attr_t* a;
 *
 *          r = nai_get_rlimit(NAI_RLIMIT_NOFILE, &l);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec != ENOTSUP) {
 *                  goto _fail;
 *              };
 *              // not suppoted on this platforms
 *              goto _ignore;
 *          };
 *
 *          l.rlim_cur = 500;
 *          r = nai_proc_attr_set_rlimit(a, NAI_RLIMIT_NOFILE, &l);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     start up the new process
 * @code
 *          nai_int_t r;
 *          const char* path;
 *          const char** args;
 *          const char** envs;
 *          nai_proc_t c;
 *          nai_proc_attr_t* a;
 *
 *          r = nai_proc_spawn(&c, path, a, args, envs);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          printf("new child process id: %d\n", r);
 * @endcode
 *
 * @par     suspend the child process
 * @code
 *          nai_int_t r;
 *          nai_proc_t* c;
 *
 *          r = nai_proc_kill(c, SIGSTOP);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     resume the child process
 * @code
 *          nai_int_t r;
 *          nai_proc_t* c;
 *
 *          r = nai_proc_kill(c, SIGCONT);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     wait the child process
 * @code
 *          nai_int_t r;
 *          nai_int_t code;
 *          nai_proc_t* c;
 *
 *          r = nai_proc_wait(c, &code, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          printf("the exit code of child process: %d\n", code);
 * @endcode
 *
 * @par     kill the child process
 * @code
 *          nai_int_t r;
 *          nai_int_t code;
 *          nai_proc_t* c;
 *
 *          r = nai_proc_kill(c, SIGKILL);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     set the signal handler
 * @code
 *          void my_signal_handler(nai_int_t signum)
 *          {
 *              ...
 *          };
 *
 *          nai_signal(SIGBREAK, my_signal_handler);
 * @endcode
 */

#ifndef _PROC_H_NAI
#define _PROC_H_NAI

#pragma once

#include "nai_system.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

//////////////////////////////////////////////////////////////////////////////
// cpu set

/**
 * @anchor  NAI_CPUSET
 * @name    NAI_CPUSET          the defines of cpu set
 * @{
 */

/** the cpu count of the default cpuset */
#define NAI_CPUSET_COUNT (1024)
/** the size of the default cpuset, in bytes */
#define NAI_CPUSET_SIZE (NAI_CPUSET_COUNT / 8)
/** the bits of the nai_cpumask_t */
#define NAI_CPUMASK_BITS (8 * sizeof(nai_cpumask_t))
#define NAI_CPUELT(cpu)  ((cpu) / NAI_CPUMASK_BITS)
#define NAI_CPUMASK(cpu) ((nai_cpumask_t)1 << ((cpu) % NAI_CPUMASK_BITS)) /** @} */

#ifndef _NAI_TYPEDEF_CPUMASK_T
    #define _NAI_TYPEDEF_CPUMASK_T
    typedef uintptr_t nai_cpumask_t;
#endif
#ifndef _NAI_TYPEDEF_CPUSET_T
    #define _NAI_TYPEDEF_CPUSET_T
    typedef struct nai_cpuset_s nai_cpuset_t;
#endif

    struct nai_cpuset_s
    {
        nai_cpumask_t bits[NAI_CPUSET_COUNT / NAI_CPUMASK_BITS];
    };

/**
 * @name    nai_cpuset_defines
 * @{
 */

/**
 * get the number of bytes required for the specified number of cpu
 * @param   count   the number of cpu
 * @return  the number of bytes required
 */
#define nai_cpuset_size(count) (((count) + NAI_CPUMASK_BITS - 1) / NAI_CPUMASK_BITS) * sizeof(nai_cpu_mask_t)

/**
 * allocate a new cpuset with the specified number of cpu
 * @param   count   the number of cpu
 * @return  the address of the new cpuset on success, 
 *          NULL is returned on fails, see #nai_errno.
 */
#define nai_cpuset_alloc(count) (nai_cpuset_t*)nai_malloc(nai_cpuset_size(count))

/**
 * free the cpuset
 * @param   set     pointer to the cpuset
 * @return  void
 */
#define nai_cpuset_free(set) nai_free(set)

/**
 * the template of the cpuset operation
 * @param   size    the size of the cpuset
 * @param   sd      pointer to the result cpuset
 * @param   s1      pointer to the source cpuset 1
 * @param   s2      pointer to the source cpuset 2
 * @param   op      the opteartion
 * @return  void
 */
#define nai_cpuset_op_s(size, sd, s1, s2, op)                                                                          \
    {                                                                                                                  \
        intptr_t max              = ((size) / sizeof(nai_cpumask_t));                                                  \
        nai_cpumask_t* dst        = (sd)->bits;                                                                        \
        cosnt nai_cpumask_t* src1 = (s1)->bits;                                                                        \
        cosnt nai_cpumask_t* src2 = (s2)->bits;                                                                        \
        for (intptr_t n = 0; n < max; n++) {                                                                           \
            dst[n] = src1 op src2;                                                                                     \
        };                                                                                                             \
    }

/**
 * clear the cpuset to zero
 * @param   size    the size of the cpuset
 * @param   set     pointer to the cpuset to clear
 * @return  void
 */
#define nai_cpuset_zero_s(size, set)                                                                                   \
    {                                                                                                                  \
        intptr_t max        = ((size) / sizeof(nai_cpumask_t));                                                        \
        nai_cpumask_t* bits = (set)->bits;                                                                             \
        for (intptr_t n = 0; n < max; n++) {                                                                           \
            bits[n] = 0;                                                                                               \
        };                                                                                                             \
    }

/**
 * perform the OR operation of two cpusets with the specified size
 * @param   size    the size of the cpuset
 * @param   sd      pointer to the result cpuset
 * @param   s1      pointer to the source cpuset 1
 * @param   s2      pointer to the source cpuset 2
 * @return  void
 */
#define nai_cpuset_or_s(size, sd, s1, s2) nai_cpuset_op_s(size, sd, s1, s2, |)

/**
 * perform the XOR operation of two cpusets with the specified size
 * @param   size    the size of the cpuset
 * @param   sd      pointer to the result cpuset
 * @param   s1      pointer to the source cpuset 1
 * @param   s2      pointer to the source cpuset 2
 * @return  void
 */
#define nai_cpuset_xor_s(size, sd, s1, s2) nai_cpuset_op_s(size, sd, s1, s2, ^)

/**
 * perform the AND operation of two cpusets with the specified size
 * @param   size    the size of the cpuset
 * @param   sd      pointer to the result cpuset
 * @param   s1      pointer to the source cpuset 1
 * @param   s2      pointer to the source cpuset 2
 * @return  void
 */
#define nai_cpuset_and_s(size, sd, s1, s2) nai_cpuset_op_s(size, sd, s1, s2, &)

/**
 * enable the specified cpu in the cpuset
 * @param   cpu     the cpu id
 * @param   size    the size of the cpuset
 * @param   set     pointer to the cpuset
 * @return  void
 */
#define nai_cpuset_set_s(cpu, size, set) ((cpu) / 8 < (size) ? (set)->bits[NAI_CPUELT(cpu)] |= NAI_CPUMASK(cpu) : 0)

/**
 * disable the specified cpu in the cpuset
 * @param   cpu     the cpu id
 * @param   size    the size of the cpuset
 * @param   set     pointer to the cpuset
 * @return  void
 */
#define nai_cpuset_clr_s(cpu, size, set) ((cpu) / 8 < (size) ? (set)->bits[NAI_CPUELT(cpu)] &= ~NAI_CPUMASK(cpu) : 0)

/**
 * test to see if the specified cpu in the cpuset
 * @param   cpu     the cpu id
 * @param   size    the size of the cpuset
 * @param   set     pointer to the cpuset
 * @return  the value of bool
 */
#define nai_cpuset_isset_s(cpu, size, set) ((cpu) / 8 < (size) ? (set)->bits[NAI_CPUELT(cpu)] & NAI_CPUMASK(cpu) : 0)

/**
 * clear the cpuset to zero
 * @param   set     pointer to the cpuset to clear
 * @return  void
 */
#define nai_cpuset_zero(set) nai_cpuset_zero_s(sizeof(*set), set)

/**
 * compare two cpusets
 * @param   s1      pointer to the source cpuset 1
 * @param   s2      pointer to the source cpuset 2
 * @return  the bool result of compare
 */
#define nai_cpuset_equal(s1, s2) nai_cpuset_equal_s(sizeof(*s1), s1, s2)

/**
 * perform the OR operation of two cpusets
 * @param   sd      pointer to the result cpuset
 * @param   s1      pointer to the source cpuset 1
 * @param   s2      pointer to the source cpuset 2
 * @return  void
 */
#define nai_cpuset_or(sd, s1, s2) nai_cpuset_or_s(sizeof(*sd), sd, s1, s2)

/**
 * perform the XOR operation of two cpusets
 * @param   sd      pointer to the result cpuset
 * @param   s1      pointer to the source cpuset 1
 * @param   s2      pointer to the source cpuset 2
 * @return  void
 */
#define nai_cpuset_xor(sd, s1, s2) nai_cpuset_xor_s(sizeof(*sd), sd, s1, s2)

/**
 * perform the AND operation of two cpusets
 * @param   sd      pointer to the result cpuset
 * @param   s1      pointer to the source cpuset 1
 * @param   s2      pointer to the source cpuset 2
 * @return  void
 */
#define nai_cpuset_and(sd, s1, s2) nai_cpuset_and_s(sizeof(*sd), sd, s1, s2)

/**
 * enable the specified cpu in the cpuset
 * @param   cpu     the cpu id
 * @param   set     pointer to the cpuset
 * @return  void
 */
#define nai_cpuset_set(cpu, set) nai_cpuset_set_s(cpu, sizeof(*set), set)

/**
 * disable the specified cpu in the cpuset
 * @param   cpu     the cpu id
 * @param   set     pointer to the cpuset
 * @return  void
 */
#define nai_cpuset_clr(cpu, set) nai_cpuset_clr_s(cpu, sizeof(*set), set)

/**
 * test to see if the specified cpu in the cpuset
 * @param   cpu     the cpu id
 * @param   set     pointer to the cpuset
 * @return  the value of bool
 */
#define nai_cpuset_isset(cpu, set) nai_cpuset_isset_s(cpu, sizeof(*set), set)

    /**
 * @}
 */

    /**
 * compare two cpusets
 * @param   s       the size of the cpuset
 * @param   s1      pointer to the source cpuset 1
 * @param   s2      pointer to the source cpuset 2
 * @return  the bool result of compare
 */
    NAI_EXTERN
    nai_int_t nai_cpuset_equal_s(size_t s, const nai_cpuset_t* s1, const nai_cpuset_t* s2);

    //////////////////////////////////////////////////////////////////////////////
    // procattr

#if (NAI_HAVE_GETPWNAM)
    #define NAI_HAVE_UID 1
#endif

#if (NAI_HAVE_SETRLIMIT)
    #define NAI_HAVE_RLIMIT 1
#endif

/**
 * @anchor  NAI_SCHED
 * @name    NAI_SCHED           the schedule class
 * @{
 */
#define NAI_SCHED_UNSET  0 /**< */
#define NAI_SCHED_NORMAL 1 /**< default */
#define NAI_SCHED_FIFO   2 /**< first in-first out scheduling */
#define NAI_SCHED_RR     3 /**< round-robin scheduling */
/** @} */

/** 
 * @anchor  NAI_PRIORITY
 * @name    NAI_PRIORITY        the schedule priority
 * @{
 */
#define NAI_PRIORITY_LOWEST  0   /**< lowest priority */
#define NAI_PRIORITY_LOWER   25  /**< lower priority */
#define NAI_PRIORITY_NORMAL  50  /**< default */
#define NAI_PRIORITY_HIGHER  75  /**< higher priority */
#define NAI_PRIORITY_HIGHEST 100 /**< highest priority */
/** @} */

/** 
 * @anchor  NAI_RLIMIT
 * @name    NAI_RLIMIT          the options of rlimit
 * @{
 */
#define NAI_RLIMIT_CPU    0 /**< max cpu time */
#define NAI_RLIMIT_MEM    1 /**< max memory workset */
#define NAI_RLIMIT_NPROC  2 /**< max child process */
#define NAI_RLIMIT_NOFILE 3 /**< max file descriptor */
    /** @} */

#ifndef _NAI_TYPEDEF_RLIM_T
    #define _NAI_TYPEDEF_RLIM_T
    typedef size_t nai_rlim_t;
#endif
#ifndef _NAI_TYPEDEF_RLIMIT_T
    #define _NAI_TYPEDEF_RLIMIT_T
    typedef struct nai_rlimit_s nai_rlimit_t;
#endif

    /** 
 * the structure of resource limit
 */
    struct nai_rlimit_s
    {
        nai_rlim_t rlim_cur; /**< soft limit */
        nai_rlim_t rlim_max; /**< hard limit */
    };

#ifndef _NAI_TYPEDEF_LOG_T
    #define _NAI_TYPEDEF_LOG_T
    typedef struct nai_log_s nai_log_t;
#endif
#ifndef _NAI_TYPEDEF_PROC_ATTR_T
    #define _NAI_TYPEDEF_PROC_ATTR_T
    typedef struct nai_proc_attr_s nai_proc_attr_t;
#endif

    /**
 * the structure of process attributes
 */
    struct nai_proc_attr_s
    {
        const char* cwd; /**< the current work directory */

        nai_log_t* log; /**< pointer to the user's logger */

        nai_fd_t stdio[3]; /**< the specified stdio */

#if (NAI_HAVE_UID)
        uint32_t uid; /**< the user id of child process */
        uint32_t gid; /**< the group id of child process */
#elif defined(_WIN32)
    HANDLE token; /**< the token of user */
#endif

#if (NAI_HAVE_RLIMIT)
        nai_rlimit_t rlimit[4]; /**< the resource limits */
#endif

        nai_int_t sched;    /**< the scheduling class */
        nai_int_t priority; /**< the scheduling priority */

        struct
        {
            size_t size;             /**< the size of the cpuset */
            const nai_cpuset_t* set; /**< the cpuset */
        } cpu;

        union
        {
            uint32_t bits;
            struct
            {
                uint32_t daemon : 1;  /**< to enable daemon */
                uint32_t suspend : 1; /**< to enable suspend on create */
                uint32_t stdio : 3;   /**< the stdio is valid */
                uint32_t sched : 1;   /**< the schuduling is valid */
#if (NAI_HAVE_UID)
                uint32_t uid : 1; /**< the user id is valid */
                uint32_t gid : 1; /**< the group id is valid */
#endif
#if (NAI_HAVE_RLIMIT)
                uint32_t rlimit : 4; /**< the resource limits is valid */
#endif
            };
        } valid;
    };

    /**
 * initial the process attributes
 * @param   a       pointer to the process attributes
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_init(nai_proc_attr_t* a);

    /**
 * create the process attributes
 * @param   a       pointer to the thread attrubutes
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_open(nai_proc_attr_t* a);

    /**
 * set current work directory
 * @param   a       pointer to the process attributes
 * @param   cwd     pointer to the work directory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the function does not copy the string of the directory, 
 *          the caller needs to keep the memory of the string
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_set_cwd(nai_proc_attr_t* a, const char* cwd);

    /**
 * to enable the daemon
 * @param   a       pointer to the process attributes
 * @param   daemon  bool value to enable the daemon
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the daemon is enabled, there is no need to call nai_proc_wait.
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_set_daemon(nai_proc_attr_t* a, nai_int_t daemon);

    /**
 * to enable suspend on spawn
 * @param   a       pointer to the process attributes
 * @param   suspend bool value to enable suspend on spawn
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the suspend is enabled, 
 *          use nai_proc_kill(SIGCONT) to resumse process.
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_set_suspend(nai_proc_attr_t* a, nai_int_t suspend);

    /**
 * set scheduling class and priority
 * @param   a       pointer to the process attributes
 * @param   sched   the scheduling class, see @ref NAI_SCHED
 * @param   priority the scheduling priority, see @ref NAI_PRIORITY
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    when sched class is not normal and negative priority is set, 
 *          the actual priority is (sched_min_priority + -priority)
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_set_sched(nai_proc_attr_t* a, nai_int_t sched, nai_int_t priority);

    /**
 * set the cpuset of affinity
 * @param   a       pointer to the process attributes
 * @param   size    the size of the cpuset
 * @param   set     pointer to the cpuset
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the function does not copy the cpuset, 
 *          the caller needs to keep the memory of the cpuset
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_set_affinity(nai_proc_attr_t* a, size_t size, const nai_cpuset_t* set);

    /**
 * set a file descriptor to the specified stdio
 * @param   a       pointer to the process attributes
 * @param   no      the number of the stdio, ie. 0, 1, 2
 * @param   fd      the file descriptor
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_set_stdio(nai_proc_attr_t* a, nai_int_t no, nai_fd_t fd);

    /**
 * to enable stdio that inherit from the parent process.
 * @param   a       pointer to the process attributes
 * @param   no      the number of the stdio, ie. 0, 1, 2
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_set_stdio_inherit(nai_proc_attr_t* a, nai_int_t no);

    /**
 * set the resource limit
 * @param   a       pointer to the process attributes
 * @param   res     the resource option, see @ref NAI_RLIMIT
 * @param   limit   pointer to the value of limit
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function depends on the platform implementation. 
 *          if it fails and sets nai_errno to ENOTSUP, 
 *          it means that the platform does not support it.
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_set_limit(nai_proc_attr_t* a, nai_int_t res, const nai_rlimit_t* limit);

    /**
 * set the user of child process
 * @param   a       pointer to the process attributes
 * @param   user    pointer to the name of the specified user
 * @param   passwd  pointer to the password, it's required on win32
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_set_user(nai_proc_attr_t* a, const char* user, const char* passwd);

    /**
 * set the group of child process
 * @param   a       pointer to the process attributes
 * @param   group   pointer to the name of the specified group
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_set_group(nai_proc_attr_t* a, const char* group);

    /**
 * set the user specified logger
 * @param   a       pointer to the process attributes
 * @param   log     pointer to the user specified logger
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_set_log(nai_proc_attr_t* a, nai_log_t* log);

    /**
 * close the process attributes
 * @param   a       pointer to the process attributes
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_proc_attr_close(nai_proc_attr_t* a);

    //////////////////////////////////////////////////////////////////////////////
    // proc

    /**
 * @anchor  NAI_PROC_CODE
 * @name    NAI_PROC_CODE       the defines of process code
 * @{
 */

#if defined(_WIN32)

    /**
 * test whether the process is exit normally
 * @param   c       the exit status of process
 * @return  if it is exit normally, return 1, otherwise return 0
 */
    #define nai_ifexited(c) (!nai_ifsignaled(c))

    /**
 * test whether the process is killed by signal
 * @param   c       the exit status of process
 * @return  if it is killed by signal, return 1, otherwise return 0
 */
    #define nai_ifsignaled(c) (((c)&0xc0000000) == 0xc0000000 && ((c)&0x3fff0000) == 0)

    /**
 * get the exit code from the exit status
 * @param   c       the exit status of process
 * @return  the value of the exit code
 */
    #define nai_exitcode(c) (c)

    /**
 * get the terminate code from the exit status
 * @param   c       the exit status of process
 * @return  the value of the terminate code
 */
    #define nai_termcode(c) (c)

#else

    #define __NAI_SIGNALED    0x0100
    #define __NAI_STOPPED     0x0200

    /**
 * test whether the process is exit normally
 * @param   c       the exit status of process
 * @return  if it is exit normally, return 1, otherwise return 0
 */
    #define nai_ifexited(c)   (((c)&0xff00) == 0)

    /**
 * test whether the process is killed by signal
 * @param   c       the exit status of process
 * @return  if it is killed by signal, return 1, otherwise return 0
 */
    #define nai_ifsignaled(c) (((c)&0xff00) == __NAI_SIGNALED)

    /**
 * get the exit code from the exit status
 * @param   c       the exit status of process
 * @return  the value of the exit code
 */
    #define nai_exitcode(c)   ((c)&0xff)

    /**
 * get the terminate code from the exit status
 * @param   c       the exit status of process
 * @return  the value of the terminate code
 */
    #define nai_termcode(c)   ((c)&0xff)

#endif

/** deprecated, will be removed */
#define NAI_IFEXITED nai_ifexited
/** deprecated, will be removed */
#define NAI_IFSIGNALED nai_ifsignaled
/** deprecated, will be removed */
#define NAI_EXITCODE nai_exitcode
/** deprecated, will be removed */
#define NAI_TERMCODE nai_termcode

    /** @} */

#ifndef _NAI_TYPEDEF_PID_T
    #define _NAI_TYPEDEF_PID_T
    typedef int32_t nai_pid_t;
#endif
#ifndef _NAI_TYPEDEF_PROC_T
    #define _NAI_TYPEDEF_PROC_T
    typedef struct nai_proc_s nai_proc_t;
#endif
#ifndef _NAI_TYPEDEF_FORK_CB_F
    #define _NAI_TYPEDEF_FORK_CB_F
    typedef void (*nai_fork_cb_f)();
#endif

    /**
 * the structure of the process
 */
    struct nai_proc_s
    {
        nai_pid_t pid; /**< the id of process */
        union
        {
            uint32_t stat;
            union
            {
                uint32_t detach : 1;  /**< detached from parent process */
                uint32_t suspend : 1; /**< become suspended */
            };
        };
    };

/**
 * @name    nai_proc_defines
 * @{
 */

/**
 * initial the prococess
 * @param   p       a pointer of nai_proc_t to be initialized
 * @return  void
 */
#define nai_proc_init(p)                                                                                               \
    {                                                                                                                  \
        (p)->pid  = 0;                                                                                                 \
        (p)->stat = 0;                                                                                                 \
    }

    /** @} */

    /**
 * get the current process
 * @return  the current process
 */
    NAI_EXTERN
    nai_proc_t nai_proc_self();

    /**
 * get the pid of the current process
 * @return  the pid of current process
 */
    NAI_EXTERN
    nai_pid_t nai_get_pid();

    /**
 * get the pid of the process
 * @param   p       pointer to the process
 * @return  the pid of the process
 */
    NAI_EXTERN
    nai_pid_t nai_proc_get_pid(nai_proc_t* p);

    /**
 * spawn a new child process
 * @param   p       pointer to the process
 * @param   path    pointer to the path of executable
 * @param   attr    pointer to the process attributes, can be null.
 * @param   args    pointer to the array of arguments and terminated with 0.
 * @param   env     pointer to the array of environments and terminated with 0.
 * @retval  >=0     the pid of child is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_pid_t nai_proc_spawn(
        nai_proc_t* p, const char* path, const nai_proc_attr_t* attr, const char* args[], const char* env[]);

    /**
 * fork a new process
 * @param   p       pointer to the process
 * @param   suspend bool value to enable suspend on fork
 * @retval  >0      the pid of child is returned in parent process.
 * @retval  =0      on sucess in child process.
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function depends on the platform implementation. 
 *          if it fails and sets nai_errno to ENOTSUP, 
 *          it means that the platform does not support it.
 */
    NAI_EXTERN
    nai_pid_t nai_proc_fork(nai_proc_t* p, nai_int_t suspend);

    /**
 * wait a child process exit
 * @param   p       pointer to the process
 * @param   code    pointer to a nai_int_t to returned exit code
 * @param   nowait  return immediately without waiting for the process to exit
 * @retval  >=0     the pid of child is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the child process is still alive, 
 *          return failure and set nai_errno to EINPROGRESS
 * @note    if the process gives null, it will wait for all child processes.
 */
    NAI_EXTERN
    nai_pid_t nai_proc_wait(nai_proc_t* p, nai_int_t* code, nai_int_t nowait);

    /**
 * send a signal to the process
 * @param   p       pointer to the process
 * @param   signum  the number of signal, see SIG*
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function ensures that basic signals are supported:
 *          SIGTERM, SIGKILL, SIGINT, SIGCONT, SIGSTOP. 
 *          other signal dependent platform support.
 */
    NAI_EXTERN
    nai_int_t nai_proc_kill(nai_proc_t* p, nai_int_t signum);

    /**
 * set the scheduling class and priotiry
 * @param   p       pointer to the process
 * @param   sched   the scheduling class, see @ref NAI_SCHED
 * @param   priority the scheduling priority, see @ref NAI_PRIORITY
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    when sched class is not normal and negative priority is set, 
 *          the actual priority is (sched_min_priority + -priority)
 * @note    on linux, this function only effect the main thread of the 
 *          process.
 */
    NAI_EXTERN
    nai_int_t nai_proc_set_sched(nai_proc_t* p, nai_int_t sched, nai_int_t priority);

    /**
 * get the cpuset of affinity
 * @param   p       pointer to the process
 * @param   size    the size of the cpuset
 * @param   set     pointer to the cpuset to receive
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    on linux, this function only effect the main thread of the 
 *          process.
 */
    NAI_EXTERN
    nai_int_t nai_proc_get_affinity(nai_proc_t* p, size_t size, nai_cpuset_t* set);

    /**
 * set the cpuset of affinity
 * @param   p       pointer to the process
 * @param   size    the size of the cpuset
 * @param   set     pointer to the cpuset
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    on linux, this function only effect the main thread of the 
 *          process.
 */
    NAI_EXTERN
    nai_int_t nai_proc_set_affinity(nai_proc_t* p, size_t size, const nai_cpuset_t* set);

    /**
 * execute a file to replace the current executable image
 * @param   path    pointer to the path of executable
 * @param   attr    pointer to the process attributes, can be null.
 * @param   args    pointer to the array of arguments and terminated with 0.
 * @param   env     pointer to the array of environments and terminated with 0.
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function depends on the platform implementation. 
 *          if it fails and sets nai_errno to ENOTSUP, 
 *          it means that the platform does not support it.
 */
    NAI_EXTERN
    nai_int_t nai_exec(const char* path, const nai_proc_attr_t* attr, const char* args[], const char* env[]);

    /**
 * to exit the current process
 * @param   code    the exit code
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_exit(nai_int_t code);

    /**
 * detach from the parent process
 * @param   daemon  bool value to enable the daemon
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_detach(nai_int_t daemon);

    /**
 * set fork handlers
 * @param   pre     pointer to the handler which is called before fork
 * @param   parent  pointer to the handler which is called after fork in parent
 * @param   child   pointer to the handler which is called after fork in child
 * @retval  >=0     on sucess
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the hander 'parent' and 'child' should use signal-safety 
 *          functions only
 */
    NAI_EXTERN
    nai_int_t nai_fork_at(nai_fork_cb_f pre, nai_fork_cb_f parent, nai_fork_cb_f child);

    /**
 * set the environment of the current process
 * @param   name    pointer to the name of env
 * @param   value   pointer to the new value of env, 
 *                  use NULL to remove the env.
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_set_env(const char* name, const char* value);

    /**
 * get the environment of the current process
 * @param   name    pointer to the name of env
 * @param   buf     pointer to the buffer to receive data
 * @param   buflen  the length of the buffer
 * @retval  >=0     the length of output without null-terminated is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function do not write more than 'buflen' bytes 
 *          (including the terminating null byte ('\0')).
 * @note    this function may not write the content into the buffer 
 *          when 'buflen' is less than the returned value.
 */
    NAI_EXTERN
    intptr_t nai_get_env(const char* name, char* buf, size_t buflen);

    /**
 * get the value of limit
 * @param   res     the resource option, see @ref NAI_RLIMIT
 * @param   limit   pointer to the limit
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function depends on the platform implementation. 
 *          if it fails and sets nai_errno to ENOTSUP, 
 *          it means that the platform does not support it.
 */
    NAI_EXTERN
    nai_int_t nai_get_rlimit(nai_int_t res, nai_rlimit_t* limit);

    /**
 * set the value of limit
 * @param   res     the resource option, see @ref NAI_RLIMIT
 * @param   limit   pointer to the value of limit
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function depends on the platform implementation. 
 *          if it fails and sets nai_errno to ENOTSUP, 
 *          it means that the platform does not support it.
 */
    NAI_EXTERN
    nai_int_t nai_set_rlimit(nai_int_t res, const nai_rlimit_t* limit);

    //////////////////////////////////////////////////////////////////////////////
    // signal

#if (NAI_HAVE_SIGNAL_H)
    #include <signal.h>
#endif

#if defined(_WIN32)
    #if !defined(SIGKILL)
        #define SIGKILL 9
    #endif
    #if !defined(SIGSTOP)
        #define SIGSTOP 17
    #endif
    #if !defined(SIGCONT)
        #define SIGCONT 19
    #endif
    #if !defined(SIGCHLD)
        #define SIGCHLD 20
    #endif
#endif

#ifndef _NAI_TYPEDEF_SIGHANDLE_F
    #define _NAI_TYPEDEF_SIGHANDLE_F
    typedef void (*nai_sighandle_f)(nai_int_t signum);
#endif

    /**
 * set signal handle
 * @param   signum  the number of signal, see SIG* in signal.h
 * @param   handle  the handle of signal
 * @return  the previous value of the signal handler.
 *          on failure, it returns SIG_ERR, and nai_errno is set to indicate 
 *          the error.
 */
    NAI_EXTERN
    nai_sighandle_f nai_signal(nai_int_t signum, nai_sighandle_f handle);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
