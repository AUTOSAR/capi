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
/// @file       nai_log.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_time.h"
#include "nai/os/nai_stat.h"
#include "nai/os/nai_system.h"
#include "nai/os/nai_tlocal.h"
#include "nai/io/nai_event.h"
#include <stdio.h>
#include <time.h>



nai_log_t nai_log_core = nai_log_from_stderr();


static const char* nai_levels[] = {
    "EMERG", 
    "ALERT", 
    "CRIT", 
    "ERROR", 
    "WARN", 
    "NOTICE", 
    "INFO", 
    "DEBUG"
};


static const char* nai_strerror(nai_int_t err)
{
    const char* s;


    if (err < NAI_ECUSTOM_START) {
        s = strerror(err);
        goto _end;
    };

    switch (err) {
    case NAI_EFAILED:
        s = "Generally failed";
        break;
    case NAI_ETYPE:
        s = "Invalid type";
        break;
    case NAI_EOPERATION:
        s = "Invalid operation";
        break;
    case NAI_ESETTING:
        s = "Invalid setting";
        break;
    case NAI_ECLOSED:
        s = "Already closed";
        break;
    case NAI_EINTERNAL:
        s = "Internal error";
        break;
    case NAI_ECORRUPTION:
        s = "Data corruption";
        break;
    case NAI_ESINGLETON:
        s = "Singleton";
        break;
    case NAI_ENODRI:
        s = "No driver";
        break;
    case NAI_EDEVICE_FUNCTION:
        s = "Invalid device function";
        break;
    case NAI_EDRIVER_INTERNAL:
        s = "Driver internal error";
        break;
    case NAI_EDRIVER_INITIALIZE:
        s = "Driver uninitialize";
        break;
    case NAI_EARCHIVE:
        s = "Archive error";
        break;
    case NAI_EVERSION:
        s = "Version error";
        break;
#if !defined(ESHUTDOWN)
    case NAI_ESHUTDOWN:
        s = "Can't send after socket shutdown";
        break;
#endif
    default:
        s = 0;
        break;
    };

_end:
    return s;
};


static nai_int_t nai_log_fmtv_impl(nai_log_t* l, 
    const char* func, nai_int_t line, nai_int_t level, nai_int_t err, 
    const char* fmt, va_list va)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t len;
    nai_int_t cpy;
    uint64_t t;
    uint32_t usec;
    struct timespec;
    const char* name;
    const char* errstr;
    nai_stat_t st;
    nai_timefmts_t* tf;
    char buf[1024];
    char* mem;


    if (level > (nai_int_t)nai_countof(nai_levels)) {
        level = nai_countof(nai_levels);
    };


    /* save errno */
    ec = nai_errno;

    if (l->cb) {
        l->cb(l, func, line, level, err, fmt, va);
    } else {

        /* prepare log header */
        if (l->inloop) {
            /* is disk full now, don't output */
            t = nai_timecache_to_usec();
            if (l->dtime == t) {
                r = 0;
                goto _end;
            };

            tf = nai_timecache_fmts();
            len = sizeof(tf->rfc822_time)-1;
            nai_memcpy(buf, tf->rfc822_time, len);
        } else {
            /* is disk full now, don't output */
            t = nai_time();
            if (l->dtime == t) {
                r = 0;
                goto _end;
            };

            len = nai_time_to_rfc822(t / 1000000, buf, sizeof(buf));
        };
        len -= 4;
        buf[len ++] = '.';
        usec = (uint32_t)(t % 1000000);

        /* prepare log header */
        if (level < 1) {
            if (err == 0) {
                len += nai_snprintf(buf+len, sizeof(buf)-len, 
                    "%06d %s@%d: ", usec, func, line);
            } else if ((errstr = nai_strerror(err)) != 0) {
                len += nai_snprintf(buf+len, sizeof(buf)-len, 
                    "%06d %s@%d: errno %d, %s, ", 
                    usec, func, line, err, errstr);
            } else {
                len += nai_snprintf(buf+len, sizeof(buf)-len, 
                    "%06d %s@%d: errno %d, ", 
                    usec, func, line, err);
            };
        } else {
            name = nai_levels[level-1];
            if (err == 0) {
                len += nai_snprintf(buf+len, sizeof(buf)-len, 
                    "%06d [%s] %s@%d: ", usec, name, func, line);
            } else if ((errstr = nai_strerror(err)) != 0) {
                len += nai_snprintf(buf+len, sizeof(buf)-len, 
                    "%06d [%s] %s@%d: errno %d, %s, ", 
                    usec, name, func, line, err, errstr);
            } else {
                len += nai_snprintf(buf+len, sizeof(buf)-len, 
                    "%06d [%s] %s@%d: errno %d, ", 
                    usec, name, func, line, err);
            };
        };

        do {
            /* output to stdout or stderr */
            if (l->fd == NAI_FD_INVALID && l->fpath == 0) {

                cpy = (nai_int_t)nai_strlen(fmt);
                if (len + cpy + 2 <= (nai_int_t)sizeof(buf)) {
                    mem = buf;
                } else {
                    /* get buffer */
                    mem = (char*)nai_thread_local_mem(len + cpy + 2);
                    if (mem == 0) {
                        /* fixme: ignored error */
                        break;
                    };

                    nai_memcpy(mem, buf, cpy);
                };

                nai_memcpy(buf+len, fmt, cpy);
                len += cpy;

                buf[len] = '\n';
                buf[len+1] = '\0';
                len += 2;

                r = vfprintf(stderr, buf, va);
                if (r < 0) {
                    if (nai_errno == ENOSPC) {
                        l->dtime = t;
                    };
                };
                break;
            };

            /* check specified file every 60 secs */
            if (l->ctime <= t && 
                l->ctime + (60 * 1000000) > t) {

                if (l->fd == NAI_FD_INVALID) {
                    /* can't output */
                    break;
                };

            } else {

                l->ctime = t;

                if (l->fd != NAI_FD_INVALID && l->fpath != 0) {
                    if (l->fno == 0) {
                        r = nai_file_stat(l->fd, &st, NAI_STAT_BASIC);
                        if (r < 0) {
                            /* fixme: ignored error */
                            break;
                        };

                        l->fno = st.st_ino;
                        l->freg = !!(st.st_mode & NAI_S_IFREG);
                    };

                    if (l->freg) {
                        st.st_ino = 0;
                        r = nai_stat(l->fpath, &st, NAI_STAT_BASIC);
                        if (r < 0 && 
                            nai_errno != ENOENT) {
                            /* fixme: ignored error */
                            break;
                        };

                        /* if file is deleted, then need reopen it */
                        if (st.st_ino != l->fno) {
                            if (l->fdown) {
                                nai_file_close(l->fd);
                            };

                            l->fd = NAI_FD_INVALID;
                            l->fno = 0;
                            l->freg = 0;
                            l->fdown = 0;
                        };
                    };
                };
                if (l->fd == NAI_FD_INVALID) {
                    l->fd = nai_file_open(l->fpath, 
                        NAI_O_CREAT | NAI_O_WRONLY | NAI_O_APPEND, 0644);
                    if (l->fd == NAI_FD_INVALID) {
                        /* fixme: ignored error */
                        break;
                    };

                    l->fdown = 1;
                    l->fno = 0;
                    l->freg = 0;

                    /* force check on next time */
                    l->ctime = 0;
                };
            };


            /* output to specified file */
            cpy = len;
            len = nai_vsnprintf(buf+cpy, sizeof(buf)-cpy, fmt, va);
            if (len + cpy + 1 <= (nai_int_t)sizeof(buf)) {
                mem = buf;
            } else {
                /* get buffer */
                mem = (char*)nai_thread_local_mem(len + cpy + 1);
                if (mem == 0) {
                    /* fixme: ignored error */
                    break;
                };

                nai_memcpy(mem, buf, cpy);
                nai_vsnprintf(mem + cpy, len, fmt, va);
            };

            len += cpy;
            mem[len] = '\n';
            len += 1;

            r = (nai_int_t)nai_file_write(l->fd, mem, len);
            if (r < 0) {
                if (nai_errno == ENOSPC) {
                    l->dtime = t;
                };
            };

        } while (0);
    };

    /* restore errno */
    nai_errno = ec;
    r = 0;

_end:
    return r;
};


void nai_log_fmt(nai_log_t* l, 
    const char* func, nai_int_t line, nai_int_t level, nai_int_t err, 
    const char* fmt, ...)
{
    nai_int_t r;
    va_list va;


    if (level > (nai_int_t)l->level) {
        goto _end;
    };

    va_start(va, fmt);
    r = nai_log_fmtv_impl(l, func, line, level, err, fmt, va);
    va_end(va);

    (void)r;

_end:
    return;
};


void nai_log_fmtv(nai_log_t* l, 
    const char* func, nai_int_t line, nai_int_t level, nai_int_t err, 
    const char* fmt, va_list va)
{
    nai_int_t r;


    if (level > (nai_int_t)l->level) {
        goto _end;
    };

    r = nai_log_fmtv_impl(l, func, line, level, err, fmt, va);

    (void)r;

_end:
    return;
};


nai_int_t nai_log_close(nai_log_t* l)
{
    nai_int_t r;


    if (l->fdown && l->fd != NAI_FD_INVALID) {
        nai_file_close(l->fd);
    };

    l->fd = NAI_FD_INVALID;
    l->fdown = 0;
    r = 0;

    return r;
};


#if !(NAI_HAVE_C99)


void nai_log_debug(nai_log_t* l, 
    const char* func, nai_int_t line, nai_int_t err, 
    const char* fmt, ...)
{
    nai_int_t r;
    nai_int_t level = NAI_LOG_DEBUG;
    va_list va;


    if (level > (nai_int_t)l->level) {
        goto _end;
    };

    va_start(va, fmt);
    r = nai_log_fmtv_impl(l, func, line, level, err, fmt, va);
    va_end(va);

    (void)r;

_end:
    return;
};


void nai_log_info(nai_log_t* l, 
    const char* func, nai_int_t line, nai_int_t err, 
    const char* fmt, ...)
{
    nai_int_t r;
    nai_int_t level = NAI_LOG_INFO;
    va_list va;


    if (level > (nai_int_t)l->level) {
        goto _end;
    };

    va_start(va, fmt);
    r = nai_log_fmtv_impl(l, func, line, level, err, fmt, va);
    va_end(va);

    (void)r;

_end:
    return;
};


void nai_log_notice(nai_log_t* l, 
    const char* func, nai_int_t line, nai_int_t err, 
    const char* fmt, ...)
{
    nai_int_t r;
    nai_int_t level = NAI_LOG_NOTICE;
    va_list va;


    if (level > (nai_int_t)l->level) {
        goto _end;
    };

    va_start(va, fmt);
    r = nai_log_fmtv_impl(l, func, line, level, err, fmt, va);
    va_end(va);

    (void)r;

_end:
    return;
};


void nai_log_warn(nai_log_t* l, 
    const char* func, nai_int_t line, nai_int_t err, 
    const char* fmt, ...)
{
    nai_int_t r;
    nai_int_t level = NAI_LOG_WARN;
    va_list va;


    if (level > (nai_int_t)l->level) {
        goto _end;
    };

    va_start(va, fmt);
    r = nai_log_fmtv_impl(l, func, line, level, err, fmt, va);
    va_end(va);

    (void)r;

_end:
    return;
};


void nai_log_error(nai_log_t* l, 
    const char* func, nai_int_t line, nai_int_t err, 
    const char* fmt, ...)
{
    nai_int_t r;
    nai_int_t level = NAI_LOG_ERROR;
    va_list va;


    if (level > (nai_int_t)l->level) {
        goto _end;
    };

    va_start(va, fmt);
    r = nai_log_fmtv_impl(l, func, line, level, err, fmt, va);
    va_end(va);

    (void)r;

_end:
    return;
};


void nai_log_crit(nai_log_t* l, 
    const char* func, nai_int_t line, nai_int_t err, 
    const char* fmt, ...)
{
    nai_int_t r;
    nai_int_t level = NAI_LOG_CRIT;
    va_list va;


    if (level > (nai_int_t)l->level) {
        goto _end;
    };

    va_start(va, fmt);
    r = nai_log_fmtv_impl(l, func, line, level, err, fmt, va);
    va_end(va);

    (void)r;

_end:
    return;
};


void nai_log_alert(nai_log_t* l, 
    const char* func, nai_int_t line, nai_int_t err, 
    const char* fmt, ...)
{
    nai_int_t r;
    nai_int_t level = NAI_LOG_ALERT;
    va_list va;


    if (level > (nai_int_t)l->level) {
        goto _end;
    };

    va_start(va, fmt);
    r = nai_log_fmtv_impl(l, func, line, level, err, fmt, va);
    va_end(va);

    (void)r;

_end:
    return;
};


void nai_log_emerg(nai_log_t* l, 
    const char* func, nai_int_t line, nai_int_t err, 
    const char* fmt, ...)
{
    nai_int_t r;
    nai_int_t level = NAI_LOG_EMERG;
    va_list va;


    if (level > (nai_int_t)l->level) {
        goto _end;
    };

    va_start(va, fmt);
    r = nai_log_fmtv_impl(l, func, line, level, err, fmt, va);
    va_end(va);

    (void)r;

_end:
    return;
};


#endif


void nai_log_term()
{
    nai_log_close(&nai_log_core);
};


