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
/// @file       nai_log.h
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _LOG_H_NAI
#define _LOG_H_NAI

#pragma once

#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <stdarg.h>
#include <stdio.h>

    /** 
 * @anchor  NAI_LOG_LEVEL
 * @name    NAI_LOG_LEVEL       the level of log
 * @{
 */

#define NAI_LOG_EMERG  1
#define NAI_LOG_ALERT  2 /* ie. memory, resource */
#define NAI_LOG_CRIT   3 /* ie. stat, read, write */
#define NAI_LOG_ERROR  4 /* runtime, logic */
#define NAI_LOG_WARN   5
#define NAI_LOG_NOTICE 6
#define NAI_LOG_INFO   7
#define NAI_LOG_DEBUG  8

#if defined(_DEBUG)
    #define NAI_LOG_DEFAULT NAI_LOG_DEBUG
#else
    #define NAI_LOG_DEFAULT NAI_LOG_ERROR
#endif

/** @} */

/** simplified parameters */
#define NAI_LOG_ARGS(l) (l), __FUNCTION__, __LINE__

/** simplified parameters of default log */
#define NAI_LOG_CORE NAI_LOG_ARGS(&nai_log_core)

#ifndef _NAI_TYPEDEF_LOG_T
    #define _NAI_TYPEDEF_LOG_T
    typedef struct nai_log_s nai_log_t;
#endif

#ifndef _NAI_TYPEDEF_LOG_OUT_T
    #define _NAI_TYPEDEF_LOG_OUT_T
    typedef nai_int_t (*nai_log_out_f)(
        nai_log_t* l, const char* func, nai_int_t line, nai_int_t level, nai_int_t err, const char* fmt, va_list va);
#endif

    /**
 * the structure of the log
 */
    struct nai_log_s
    {
        union
        {
            struct
            {
                uint32_t level : 8;  /**< the log level */
                uint32_t freg : 1;   /**< is a normal file */
                uint32_t fdown : 1;  /**< is own file */
                uint32_t inloop : 1; /**< is used in event loop */
            };
            uint32_t flags;
        };

        nai_fd_t fd;       /**< the file descriptor */
        nai_log_out_f cb;  /**< pointer to the user defined output */
        void* ud;          /**< pointer to the user data */
        const char* fpath; /**< pointer to the file path */
        uint64_t fno;      /**< the unique value of inode */
        uint64_t ctime;    /**< last checking time */
        uint64_t dtime;    /**< last diskfull time */
    };

/**
 * the default log level
 * deprecated, use #nai_log_set_level(&nai_log_core) to instead
 */
#define nai_log_level (nai_log_core.level)

    /** 
 * the default log
 */
    NAI_EXTERN
    extern nai_log_t nai_log_core;

/**
 * construct expression of log pointer to stderr
 * @return  the construct expression
 */
#define nai_log_from_stderr()                                                                                          \
    {                                                                                                                  \
        .level = NAI_LOG_DEFAULT, .fd = NAI_FD_INVALID, .freg = 0, .fdown = 0, .inloop = 0, .cb = 0, .ud = 0,          \
        .fpath = 0, .fno = 0, .ctime = 0, .dtime = 0,                                                                  \
    }

/**
 * initial the log
 * @param   l       pointer to the log
 * @return  void
 */
#define nai_log_init(l)                                                                                                \
    {                                                                                                                  \
        (l)->level  = NAI_LOG_DEFAULT;                                                                                 \
        (l)->fd     = NAI_FD_INVALID;                                                                                  \
        (l)->freg   = 0;                                                                                               \
        (l)->fdown  = 0;                                                                                               \
        (l)->inloop = 0;                                                                                               \
        (l)->cb     = 0;                                                                                               \
        (l)->ud     = 0;                                                                                               \
        (l)->fpath  = 0;                                                                                               \
        (l)->fno    = 0;                                                                                               \
        (l)->ctime  = 0;                                                                                               \
        (l)->dtime  = 0;                                                                                               \
    }

/**
 * set the specified file descriptor to the log
 * @param   l       pointer to the log
 * @param   f       the specified file descriptor
 * @param   o       whether auto close the file descriptor when log close
 * @return  void
 */
#define nai_log_set_fd(l, f, o)                                                                                        \
    {                                                                                                                  \
        (l)->fd    = (f);                                                                                              \
        (l)->freg  = 0;                                                                                                \
        (l)->fdown = !!(o);                                                                                            \
        (l)->fno   = 0;                                                                                                \
    }

/**
 * set the specified file path to the log
 * @param   l       pointer to the log
 * @param   p       pointer to the specified file path
 * @return  void
 * @note    the log will automatically open the path and check if the file 
 *          pointed to by the path has been moved every 60 seconds
 */
#define nai_log_set_fpath(l, p)                                                                                        \
    {                                                                                                                  \
        (l)->fpath = (p);                                                                                              \
    }

/**
 * set user-defined output function
 * @param   l       pointer to the log
 * @param   f       pointer to the user function, see #nai_log_out_f
 * @param   u       pointer to the user data
 * @return  void
 * @note    when user-defined functions are enabled, the log will not be 
 *          output to the specified file or path, the behavior is entirely 
 *          up to the user
 */
#define nai_log_set_outfn(l, f, u)                                                                                     \
    {                                                                                                                  \
        (l)->cb = (f);                                                                                                 \
        (l)->ud = (u);                                                                                                 \
    }

/**
 * tell the log whether it is used in the event loop
 * @param   l       pointer to the log
 * @param   in      whether the log is used in the event loop
 * @return  void
 * @note    when it is enabled, the log will use the time cache
 */
#define nai_log_set_inloop(l, in)                                                                                      \
    {                                                                                                                  \
        (l)->inloop = !!(in);                                                                                          \
    }

/**
 * set the logging level
 * @param   l       pointer to the log
 * @param   lv      the log level, see @ref NAI_LOG_LEVEL
 * @return  void
 */
#define nai_log_set_level(l, lv)                                                                                       \
    {                                                                                                                  \
        (l)->level = (lv);                                                                                             \
    }

/**
 * get the logging level
 * @param   l       pointer to the log
 * @return  the log level, see @ref NAI_LOG_LEVEL
 */
#define nai_log_get_level(l) ((nai_int_t)(l)->level)

/**
 * test if level 'debug' is enabled
 * @param   l       pointer to the log
 * @return  if it is enabled, return 1, otherwise return 0
 */
#define nai_log_is_enabled_debug(l) ((l)->level >= NAI_LOG_DEBUG)

/**
 * test if level 'info' is enabled
 * @param   l       pointer to the log
 * @return  if it is enabled, return 1, otherwise return 0
 */
#define nai_log_is_enabled_info(l) ((l)->level >= NAI_LOG_INFO)

/**
 * test if level 'notice' is enabled
 * @param   l       pointer to the log
 * @return  if it is enabled, return 1, otherwise return 0
 */
#define nai_log_is_enabled_notice(l) ((l)->level >= NAI_LOG_NOTICE)

/**
 * test if level 'warn' is enabled
 * @param   l       pointer to the log
 * @return  if it is enabled, return 1, otherwise return 0
 */
#define nai_log_is_enabled_warn(l) ((l)->level >= NAI_LOG_WARN)

/**
 * test if level 'error' is enabled
 * @param   l       pointer to the log
 * @return  if it is enabled, return 1, otherwise return 0
 */
#define nai_log_is_enabled_error(l) ((l)->level >= NAI_LOG_ERROR)

/**
 * test if level 'crit' is enabled
 * @param   l       pointer to the log
 * @return  if it is enabled, return 1, otherwise return 0
 */
#define nai_log_is_enabled_crit(l) ((l)->level >= NAI_LOG_CRIT)

/**
 * test if level 'alert' is enabled
 * @param   l       pointer to the log
 * @return  if it is enabled, return 1, otherwise return 0
 */
#define nai_log_is_enabled_alert(l) ((l)->level >= NAI_LOG_ALERT)

/**
 * test if level 'emerg' is enabled
 * @param   l       pointer to the log
 * @return  if it is enabled, return 1, otherwise return 0
 */
#define nai_log_is_enabled_emerg(l) ((l)->level >= NAI_LOG_EMERG)

#if !(NAI_HAVE_C99) || defined(DOXYGEN)

    /**
 * output ‘debug' level log
 * @param   l       pointer to the log
 * @param   func    pointer to the function name
 * @param   line    the number of line
 * @param   err     the error code, see #nai_errno
 * @param   fmt     pointer to the formating string
 * @param   ...     the argrments of formating
 * @return  void
 */
    NAI_EXTERN
    void nai_log_debug(nai_log_t* l, const char* func, nai_int_t line, nai_int_t err, const char* fmt, ...)
        nai_fmt_attribute(5, 6);

    /**
 * output ‘info' level log
 * @param   l       pointer to the log
 * @param   func    pointer to the function name
 * @param   line    the number of line
 * @param   err     the error code, see #nai_errno
 * @param   fmt     pointer to the formating string
 * @param   ...     the argrments of formating
 * @return  void
 */
    NAI_EXTERN
    void nai_log_info(nai_log_t* l, const char* func, nai_int_t line, nai_int_t err, const char* fmt, ...)
        nai_fmt_attribute(5, 6);

    /**
 * output ‘notice' level log
 * @param   l       pointer to the log
 * @param   func    pointer to the function name
 * @param   line    the number of line
 * @param   err     the error code, see #nai_errno
 * @param   fmt     pointer to the formating string
 * @param   ...     the argrments of formating
 * @return  void
 */
    NAI_EXTERN
    void nai_log_notice(nai_log_t* l, const char* func, nai_int_t line, nai_int_t err, const char* fmt, ...)
        nai_fmt_attribute(5, 6);

    /**
 * output ‘warn' level log
 * @param   l       pointer to the log
 * @param   func    pointer to the function name
 * @param   line    the number of line
 * @param   err     the error code, see #nai_errno
 * @param   fmt     pointer to the formating string
 * @param   ...     the argrments of formating
 * @return  void
 */
    NAI_EXTERN
    void nai_log_warn(nai_log_t* l, const char* func, nai_int_t line, nai_int_t err, const char* fmt, ...)
        nai_fmt_attribute(5, 6);

    /**
 * output ‘error' level log
 * @param   l       pointer to the log
 * @param   func    pointer to the function name
 * @param   line    the number of line
 * @param   err     the error code, see #nai_errno
 * @param   fmt     pointer to the formating string
 * @param   ...     the argrments of formating
 * @return  void
 */
    NAI_EXTERN
    void nai_log_error(nai_log_t* l, const char* func, nai_int_t line, nai_int_t err, const char* fmt, ...)
        nai_fmt_attribute(5, 6);

    /**
 * output ‘crit' level log
 * @param   l       pointer to the log
 * @param   func    pointer to the function name
 * @param   line    the number of line
 * @param   err     the error code, see #nai_errno
 * @param   fmt     pointer to the formating string
 * @param   ...     the argrments of formating
 * @return  void
 */
    NAI_EXTERN
    void nai_log_crit(nai_log_t* l, const char* func, nai_int_t line, nai_int_t err, const char* fmt, ...)
        nai_fmt_attribute(5, 6);

    /**
 * output ‘alert' level log
 * @param   l       pointer to the log
 * @param   func    pointer to the function name
 * @param   line    the number of line
 * @param   err     the error code, see #nai_errno
 * @param   fmt     pointer to the formating string
 * @param   ...     the argrments of formating
 * @return  void
 */
    NAI_EXTERN
    void nai_log_alert(nai_log_t* l, const char* func, nai_int_t line, nai_int_t err, const char* fmt, ...)
        nai_fmt_attribute(5, 6);

    /**
 * output ‘emerg' level log
 * @param   l       pointer to the log
 * @param   func    pointer to the function name
 * @param   line    the number of line
 * @param   err     the error code, see #nai_errno
 * @param   fmt     pointer to the formating string
 * @param   ...     the argrments of formating
 * @return  void
 */
    NAI_EXTERN
    void nai_log_emerg(nai_log_t* l, const char* func, nai_int_t line, nai_int_t err, const char* fmt, ...)
        nai_fmt_attribute(5, 6);

#elif (NAI_HAVE_C99)

    #define nai_log_debug(...)                  nai_log_debug_(__VA_ARGS__)
    #define nai_log_debug_(l, fn, ln, err, ...) nai_log_fmt_((l), (fn), (ln), NAI_LOG_DEBUG, (err), __VA_ARGS__)

    #define nai_log_info(...)                  nai_log_info_(__VA_ARGS__)
    #define nai_log_info_(l, fn, ln, err, ...) nai_log_fmt_((l), (fn), (ln), NAI_LOG_INFO, (err), __VA_ARGS__)

    #define nai_log_notice(...)                  nai_log_notice_(__VA_ARGS__)
    #define nai_log_notice_(l, fn, ln, err, ...) nai_log_fmt_((l), (fn), (ln), NAI_LOG_NOTICE, (err), __VA_ARGS__)

    #define nai_log_warn(...)                  nai_log_warn_(__VA_ARGS__)
    #define nai_log_warn_(l, fn, ln, err, ...) nai_log_fmt_((l), (fn), (ln), NAI_LOG_WARN, (err), __VA_ARGS__)

    #define nai_log_error(...)                  nai_log_error_(__VA_ARGS__)
    #define nai_log_error_(l, fn, ln, err, ...) nai_log_fmt_((l), (fn), (ln), NAI_LOG_ERROR, (err), __VA_ARGS__)

    #define nai_log_crit(...)                  nai_log_crit_(__VA_ARGS__)
    #define nai_log_crit_(l, fn, ln, err, ...) nai_log_fmt_((l), (fn), (ln), NAI_LOG_CRIT, (err), __VA_ARGS__)

    #define nai_log_alert(...)                  nai_log_alert_(__VA_ARGS__)
    #define nai_log_alert_(l, fn, ln, err, ...) nai_log_fmt_((l), (fn), (ln), NAI_LOG_ALERT, (err), __VA_ARGS__)

    #define nai_log_emerg(...)                  nai_log_emerg_(__VA_ARGS__)
    #define nai_log_emerg_(l, fn, ln, err, ...) nai_log_fmt_((l), (fn), (ln), NAI_LOG_EMERG, (err), __VA_ARGS__)

    #define nai_log_fmt_(l, fn, ln, lv, err, ...)                                                                      \
        {                                                                                                              \
            nai_log_t* _l_ = (l);                                                                                      \
            if (_l_->level >= (lv)) {                                                                                  \
                nai_log_fmt(_l_, (fn), (ln), (lv), (err), __VA_ARGS__);                                                \
            };                                                                                                         \
        }

#endif

    /**
 * output formarting log
 * @param   l       pointer to the log
 * @param   func    pointer to the function name
 * @param   line    the number of line
 * @param   level   the logging level
 * @param   err     the error code, see #nai_errno
 * @param   fmt     pointer to the formating string
 * @param   ...     the argrments of formating
 * @return  void
 */
    NAI_EXTERN
    void nai_log_fmt(
        nai_log_t* l, const char* func, nai_int_t line, nai_int_t level, nai_int_t err, const char* fmt, ...)
        nai_fmt_attribute(6, 7);

    /**
 * output formarting log
 * @param   l       pointer to the log
 * @param   func    pointer to the function name
 * @param   line    the number of line
 * @param   level   the logging level
 * @param   err     the error code, see #nai_errno
 * @param   fmt     pointer to the formating string
 * @param   va      the argrments of formating
 * @return  void
 */
    NAI_EXTERN
    void nai_log_fmtv(
        nai_log_t* l, const char* func, nai_int_t line, nai_int_t level, nai_int_t err, const char* fmt, va_list va);

    /**
 * close the log
 * @param   l       pointer to the log
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_log_close(nai_log_t* l);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
