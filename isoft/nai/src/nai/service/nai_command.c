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
/// @file       nai_command.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/service/nai_command.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include <stdlib.h>


nai_int_t nai_parse_off(nai_off_t* p, const char* s, size_t len)
{
    nai_int_t r;
    const char* end;
    nai_off_t v;
    nai_off_t scale;


    if ((intptr_t)len < 0) {
        len = nai_strlen(s);
    };

    scale = 1;
    if (len > 0) {
        end = s + len - 1;
        switch (*end) {
        case 'k':
        case 'K':
            scale = 1024;
            len --;
            break;
        case 'm':
        case 'M':
            scale = 1024*1024;
            len --;
            break;
        case 'g':
        case 'G':
            scale = 1024*1024*1024;
            len --;
            break;
        default:
            break;
        };
    };

    r = nai_atoof(&v, s, len, 0);
    if (r < 0) {
        goto _end;
    };
    if (v > ((nai_off_t)NAI_OFF_T_MAX/scale)) {
        nai_errno = ERANGE;
        r = -1;
        goto _end;
    };

    p[0] = (nai_off_t)(v*scale);
    r = 0;

_end:
    return r;
};


nai_int_t nai_parse_size(size_t* p, const char* s, size_t len)
{
    nai_int_t r;
    const char* end;
    ssize_t v;
    ssize_t scale;


    if ((intptr_t)len < 0) {
        len = nai_strlen(s);
    };

    scale = 1;
    if (len > 0) {
        end = s + len - 1;
        switch (*end) {
        case 'k':
        case 'K':
            scale = 1024;
            len --;
            break;
        case 'm':
        case 'M':
            scale = 1024*1024;
            len --;
            break;
        case 'g':
        case 'G':
            scale = 1024*1024*1024;
            len --;
            break;
        default:
            break;
        };
    };

    r = nai_atosz(&v, s, len, 0);
    if (r < 0) {
        goto _end;
    };
    if (v > ((ssize_t)-1/scale)) {
        nai_errno = ERANGE;
        r = -1;
        goto _end;
    };

    p[0] = (size_t)(v*scale);
    r = 0;

_end:
    return r;
};


nai_int_t nai_parse_timeval(uint64_t* v, const char* s, size_t len)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t stat;
    char ch;
    const char* p;
    const char* pend;
    uint64_t scale;
    uint64_t val;
    uint64_t msec;

    enum {
        year, month, day, hour, 
        minite, second, msecond, done, 
    };
    const int64_t oneday = 24*60*60*1000;


    if ((intptr_t)len < 0) {
        len = nai_strlen(s);
    };

    p = s;
    pend = p + len;

    n = 0;
    val = 0;
    msec = 0;
    stat = year;
    for ( ; ; p ++) {
        if (p >= pend) {
            ch = 0;
        } else {
            ch = *p;
        };

        if (ch >= '0' && ch <= '9') {
            val *= 10;
            val += ch - '0';
            n ++;
            continue;
        };
        if (n <= 0) {
            if (ch == 0) {
                break;
            };
            goto _fail;
        };

        switch (ch) {
        case 'y':
            if (stat > year) {
                goto _fail;
            };
            scale = 356*oneday;
            stat = month;
            break;
        case 'M':
            if (stat > month) {
                goto _fail;
            };
            scale = 30*oneday;
            stat = day;
            break;
        case 'd':
            if (stat > day) {
                goto _fail;
            };
            scale = oneday;
            stat = hour;
            break;
        case 'h':
            if (stat > hour) {
                goto _fail;
            };
            scale = 60*60*1000;
            stat = minite;
            break;
        case 'm':
            if (p + 1 < pend && p[0] == 's') {
                if (stat > msecond) {
                    goto _fail;
                };
                scale = 1;
                stat = done;
            } else {
                if (stat > minite) {
                    goto _fail;
                };
                scale = 60*1000;
                stat = second;
            };
            break;
        case 's':
        case '\0':
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            if (stat > second) {
                goto _fail;
            };
            scale = 1000;
            stat = msecond;
            break;

        default:
            goto _fail;
        };

        msec += val * scale;
        if (ch == '\0' || nai_isspace(ch)) {
            break;
        };

        val = 0;
        n = 0;
        continue;

_fail:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    v[0] = msec;
    r = 0;

_end:
    return r;
};


static nai_command_optinfo_t nai_option_onoff[] = {
    { nai_strconst("on"), 1 }, 
    { nai_strconst("off"),  0 },
    { nai_strnull(), 0 }
};


nai_sult_t nai_command_set_string(nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_str_t str;


    rc = nai_value_dups(&args->argv[0], &str, args->pool);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' get string failed", c->name);
        goto _end;
    };


    *(nai_str_t*)(args->data + c->offset) = str;

_end:
    return rc;
};


nai_sult_t nai_command_set_array(nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t n;
    nai_sult_t rc;
    nai_str_t* elts;
    nai_array_t* array;


    array = (nai_array_t*)(args->data + c->offset);
    if (array->eltsize != sizeof(*elts)) {
        rc = nai_errno_to_sult(NAI_ETYPE);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' offer an invalid array type failed", c->name);
        goto _end;
    };

    array->count = 0;
    if (args->argc > 0) {
        elts = (nai_str_t*)nai_array_push_n(array, args->argc);
        for (n = 0; n < args->argc; n ++) {
            rc = nai_value_dups(&args->argv[n], &elts[n], args->pool);
            if (rc < 0) {
                nai_log_error(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), 
                    "command '%s' get array failed", c->name);
                goto _end;
            };
        };
    };

    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_command_set_off(nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_mem_t str;
    nai_off_t size;


    if (args->argv[0].type != NAI_VALUE_STRING && 
        args->argv[0].type != NAI_VALUE_MEMORY) {
#if NAI_SIZEOF_OFF_T == 4
        rc = nai_value_geti(&args->argv[0], (nai_off_t*)&size);
#else
        rc = nai_value_geti64(&args->argv[0], (nai_off_t*)&size);
#endif
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "invalid argument of command '%s'", c->name);
            goto _end;
        };
    } else {
        rc = nai_value_getm(&args->argv[0], &str);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "command '%s' get string failed", c->name);
            goto _end;
        };

        rc = nai_parse_off(&size, nai_str(&str), nai_str_len(&str));
        if (rc < 0) {
            rc = nai_sult_from_errno();
            if (rc != nai_errno_to_sult(ERANGE)) {
                nai_log_error(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), 
                    "invalid off format of command '%s'", c->name);
                goto _end;
            } else {
                rc = nai_errno_to_sult(ERANGE);
                nai_log_error(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), 
                    "the value of off is too large in command '%s' ", 
                    c->name);
            };
            goto _end;
        };
    };

    *(nai_off_t*)(args->data + c->offset) = size;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_command_set_size(nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t ec;
    size_t size;
    nai_sult_t rc;
    nai_mem_t str;


    if (args->argv[0].type != NAI_VALUE_STRING && 
        args->argv[0].type != NAI_VALUE_MEMORY) {
        rc = nai_value_getiptr(&args->argv[0], (intptr_t*)&size);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "invalid argument of command '%s'", c->name);
            goto _end;
        };
    } else {
        rc = nai_value_getm(&args->argv[0], &str);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "command '%s' get string failed", c->name);
            goto _end;
        };

        rc = nai_parse_size(&size, nai_str(&str), nai_str_len(&str));
        if (rc < 0) {
            ec = nai_errno;
            if (ec != ERANGE) {
                rc = nai_errno_to_sult(ec);
                nai_log_error(NAI_LOG_CORE, ec, 
                    "invalid size format of command '%s'", c->name);
                goto _end;
            } else {
                rc = nai_errno_to_sult(ERANGE);
                nai_log_error(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), 
                    "the value of size is too large in command '%s' ", 
                    c->name);
            };
            goto _end;
        };
    };

    *(size_t*)(args->data + c->offset) = size;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_command_set_bufsize(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t val;
    nai_sult_t rc;


    rc = nai_value_geti(&args->argv[0], &val);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument of command '%s'", c->name);
        goto _end;
    };
    if (val <= 0) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid bufnum of command '%s'", c->name);
        goto _end;
    };

    args->argv[0] = args->argv[1];
    rc = nai_command_set_size(c, args);
    if (rc >= 0) {
        ((nai_bufsize_t*)(args->data + c->offset))->num = val;
    };

_end:
    return rc;
};


nai_sult_t nai_command_set_opts(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t n, m;
    nai_int_t found;
    nai_int_t optval = 0;
    nai_sult_t rc;
    nai_mem_t name;
    nai_command_optinfo_t* opts;


    opts = (nai_command_optinfo_t*)c->ctx;
    if (opts == 0) {
        opts = nai_option_onoff;
    };

    for (n = 0; n < args->argc; n ++) {
        rc = nai_value_getm(&args->argv[n], &name);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "invalid argument %d of command '%s'", n+1, c->name);
            goto _end;
        };

        found = 0;
        for (m = 0; opts[m].name.ptr; m ++) {
            if (nai_str_caseeq(&name, &opts[m].name)) {
                optval |= opts[m].value;
                found = 1;
                break;
            };
        };
        if (!found) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "invalid option value '%.*s' of command '%s'", 
                nai_str_len(&name), nai_str(&name), c->name);
            goto _end;
        };
    };

    *(nai_int_t*)(args->data + c->offset) = optval;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_command_set_int32(nai_command_t* c, nai_command_args_t* args)
{
    int32_t val;
    intptr_t cmp;
    nai_sult_t rc;


    rc = nai_value_geti(&args->argv[0], &val);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument of command '%s'", c->name);
        goto _end;
    };

    if (c->ctx != 0) {
        cmp = (intptr_t)c->ctx;
        switch (cmp & 3) {
        case 1:
            cmp >>= 2;
            if (val < cmp) {
                rc = nai_errno_to_sult(EINVAL);
                nai_log_error(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), 
                    "the value of command '%s' must greater than %d", 
                    c->name, (nai_int_t)cmp);
                goto _end;
            };
            break;
        case 2:
            cmp >>= 2;
            if (val > cmp) {
                rc = nai_errno_to_sult(EINVAL);
                nai_log_error(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), 
                    "the value of command '%s' must less than %d", 
                    c->name, (nai_int_t)cmp);
                goto _end;
            };
        default:
            /* any value */
            break;
        };
    };

    *(int32_t*)(args->data + c->offset) = val;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_command_set_int64(nai_command_t* c, nai_command_args_t* args)
{
    int64_t val;
    intptr_t cmp;
    nai_sult_t rc;


    rc = nai_value_geti64(&args->argv[0], &val);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument of command '%s'", c->name);
        goto _end;
    };

    if (c->ctx != 0) {
        cmp = (intptr_t)c->ctx;
        switch (cmp & 3) {
        case 1:
            cmp >>= 2;
            if (val < cmp) {
                rc = nai_errno_to_sult(EINVAL);
                nai_log_error(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), 
                    "the value of command '%s' must greater than %d", 
                    c->name, (nai_int_t)cmp);
                goto _end;
            };
            break;
        case 2:
            cmp >>= 2;
            if (val > cmp) {
                rc = nai_errno_to_sult(EINVAL);
                nai_log_error(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), 
                    "the value of command '%s' must less than %d", 
                    c->name, (nai_int_t)cmp);
                goto _end;
            };
            break;
        default:
            /* any value */
            break;
        };
    };

    *(int64_t*)(args->data + c->offset) = val;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_command_set_sec(nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t r;
    nai_int_t ec;
    uint64_t val;
    uint64_t sec;
    nai_sult_t rc;
    nai_mem_t str;


    if (args->argv[0].type != NAI_VALUE_STRING && 
        args->argv[0].type != NAI_VALUE_MEMORY) {
        rc = nai_value_geti64(&args->argv[0], (int64_t*)&val);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "invalid argument of command '%s'", c->name);
            goto _end;
        };
        sec = val;
    } else {
        rc = nai_value_getm(&args->argv[0], &str);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "command '%s' get string failed", c->name);
            goto _end;
        };

        r = nai_parse_timeval(&sec, nai_str(&str), nai_str_len(&str));
        if (r < 0) {
            ec = nai_errno;
            nai_log_error(NAI_LOG_CORE, ec, 
                "command '%s' get invalid timeval format", c->name);
            rc = nai_errno_to_sult(ec);
            goto _end;
        };

        sec /= 1000;
    };

    *(time_t*)(args->data + c->offset) = sec;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_command_set_msec(nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t r;
    nai_int_t ec;
    uint64_t val;
    uint64_t msec;
    nai_sult_t rc;
    nai_mem_t str;


    if (args->argv[0].type != NAI_VALUE_STRING && 
        args->argv[0].type != NAI_VALUE_MEMORY) {
        rc = nai_value_geti64(&args->argv[0], (int64_t*)&val);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "invalid argument of command '%s'", c->name);
            goto _end;
        };
        msec = val*1000;
    } else {
        rc = nai_value_getm(&args->argv[0], &str);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "command '%s' get string failed", c->name);
            goto _end;
        };

        r = nai_parse_timeval(&msec, nai_str(&str), nai_str_len(&str));
        if (r < 0) {
            ec = nai_errno;
            nai_log_error(NAI_LOG_CORE, ec, 
                "command '%s' get invalid timeval format", c->name);
            rc = nai_errno_to_sult(ec);
            goto _end;
        };
    };

    *(int64_t*)(args->data + c->offset) = msec;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_command_set_msec32(nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t ec;
    uint64_t val;
    uint64_t msec;
    nai_sult_t rc;
    nai_mem_t str;


    if (args->argv[0].type != NAI_VALUE_STRING && 
        args->argv[0].type != NAI_VALUE_MEMORY) {
        rc = nai_value_geti64(&args->argv[0], (int64_t*)&val);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "invalid argument of command '%s'", c->name);
            goto _end;
        };
        msec = val*1000;
    } else {
        rc = nai_value_getm(&args->argv[0], &str);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "command '%s' get string failed", c->name);
            goto _end;
        };

        rc = nai_parse_timeval(&msec, nai_str(&str), nai_str_len(&str));
        if (rc < 0) {
            ec = nai_errno;
            nai_log_error(NAI_LOG_CORE, ec, 
                "command '%s' get invalid timeval format", c->name);
            rc = nai_errno_to_sult(ec);
            goto _end;
        };
    };

    if (msec != (uint64_t)-1 && 
        msec > (uint32_t)-1) {
        ec = ERANGE;
        nai_log_error(NAI_LOG_CORE, ec, 
            "command '%s' get timeval is too large", c->name);
        rc = nai_errno_to_sult(ec);
        goto _end;
    };

    *(int64_t*)(args->data + c->offset) = msec;
    rc = 0;

_end:
    return rc;
};

