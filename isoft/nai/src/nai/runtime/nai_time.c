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
/// @file       nai_time.c
/// @brief      
/// @details
/// @date       2021-02-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================



#include "nai/runtime/nai_time.h"
#include "nai/runtime/nai_errno.h"
#include <stdlib.h>
#include <time.h>


static const char nai_month[12][4] = {
    "Jan", "Feb", "Mar", 
    "Apr", "May", "Jun", 
    "Jul", "Aug", "Sep", 
    "Oct", "Nov", "Dec"
};
static const char nai_day[7][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};


nai_int_t nai_time_to_str(time_t sec, char* buf, size_t len)
{
    nai_int_t r;
    nai_int_t year;
    struct tm t;
    const char* s;


    if (len < NAI_TIME_STR_LEN) {
        nai_errno = ERANGE;
        r = -1;
        goto _end;
    };

    /* example: "Wed Jun 30 21:49:08 1993" */
    /*           123456789012345678901234  */

#if (NAI_HAVE_GMTIME_R)
    gmtime_r(&sec, &t);
#else
    t = *gmtime(&sec);
#endif

    s = &nai_day[t.tm_wday][0];
    *buf ++ = *s ++;
    *buf ++ = *s ++;
    *buf ++ = *s ++;
    *buf ++ = ' ';
    s = &nai_month[t.tm_mon][0];
    *buf ++ = *s ++;
    *buf ++ = *s ++;
    *buf ++ = *s ++;
    *buf ++ = ' ';
    *buf ++ = t.tm_mday / 10 + '0';
    *buf ++ = t.tm_mday % 10 + '0';
    *buf ++ = ' ';
    *buf ++ = t.tm_hour / 10 + '0';
    *buf ++ = t.tm_hour % 10 + '0';
    *buf ++ = ':';
    *buf ++ = t.tm_min / 10 + '0';
    *buf ++ = t.tm_min % 10 + '0';
    *buf ++ = ':';
    *buf ++ = t.tm_sec / 10 + '0';
    *buf ++ = t.tm_sec % 10 + '0';
    *buf ++ = ' ';
    year = 1900 + t.tm_year;
    *buf ++ = year / 1000 + '0';
    *buf ++ = year % 1000 / 100 + '0';
    *buf ++ = year % 100 / 10 + '0';
    *buf ++ = year % 10 + '0';

    if (len > NAI_TIME_STR_LEN) {
        *buf ++ = 0;
    };

    r = NAI_TIME_STR_LEN;

_end:
    return r;
};


nai_int_t nai_time_to_rfc822(time_t sec, char* buf, size_t len)
{
    nai_int_t r;
    nai_int_t year;
    struct tm t;
    const char* s;


    if (len < NAI_TIME_RFC822_LEN) {
        nai_errno = ERANGE;
        r = -1;
        goto _end;
    };

#if (NAI_HAVE_GMTIME_R)
    gmtime_r(&sec, &t);
#else
    t = *gmtime(&sec);
#endif

    /* example: "Sat, 08 Jan 2000 18:31:41 GMT" */
    /*           12345678901234567890123456789  */

    s = &nai_day[t.tm_wday][0];
    *buf ++ = *s++;
    *buf ++ = *s++;
    *buf ++ = *s++;
    *buf ++ = ',';
    *buf ++ = ' ';
    *buf ++ = t.tm_mday / 10 + '0';
    *buf ++ = t.tm_mday % 10 + '0';
    *buf ++ = ' ';
    s = &nai_month[t.tm_mon][0];
    *buf ++ = *s++;
    *buf ++ = *s++;
    *buf ++ = *s++;
    *buf ++ = ' ';
    year = 1900 + t.tm_year;
    /* This routine isn't y10k ready. */
    *buf ++ = year / 1000 + '0';
    *buf ++ = year % 1000 / 100 + '0';
    *buf ++ = year % 100 / 10 + '0';
    *buf ++ = year % 10 + '0';
    *buf ++ = ' ';
    *buf ++ = t.tm_hour / 10 + '0';
    *buf ++ = t.tm_hour % 10 + '0';
    *buf ++ = ':';
    *buf ++ = t.tm_min / 10 + '0';
    *buf ++ = t.tm_min % 10 + '0';
    *buf ++ = ':';
    *buf ++ = t.tm_sec / 10 + '0';
    *buf ++ = t.tm_sec % 10 + '0';
    *buf ++ = ' ';
    *buf ++ = 'G';
    *buf ++ = 'M';
    *buf ++ = 'T';

    if (len > NAI_TIME_RFC822_LEN) {
        *buf ++ = 0;
    };

    r = NAI_TIME_RFC822_LEN;

_end:
    return r;
}


static nai_int_t nai_time_fmtmatch(
    const char* s, const char* pattern, nai_int_t len)
{
    nai_int_t r;
    nai_int_t n;
    char ch;


    for (n = 0; n < len; n ++) {
        ch = s[n];
        switch (pattern[n]) {
        case '@':
            if (nai_isupper(ch)) {
                continue;
            };
            break;
        case '$':
            if (nai_islower(ch)) {
                continue;
            };
            break;
        case '#':
            if (nai_isdigit(ch)) {
                continue;
            };
            break;
        case '&':
            if (nai_isxdigit(ch)) {
                continue;
            };
            break;
        case '~':
            if (ch == ' ' || nai_isdigit(ch)) {
                continue;
            };
            break;
        case '*':
            r = 1;
            goto _end;
        case '\0':
            r = ch == 0;
            goto _end;
        default:
            if (ch == pattern[n]) {
                continue;
            };
            break;
        };

        r = 0;
        goto _end;
    };

    r = 1;

_end:
    return r;
};


#define PARSE_YEAR_1900(y, s) (y) = (                   \
    ((s)[0] - '0') * 10 + ((s)[1] - '0') - 19) * 100 +  \
    ((s)[2] - '0') * 10 + ((s)[3] - '0');               \

#define PARSE_YEAR_0(y, s)                              \
    (y) = ((s)[0] - '0') * 10 + ((s)[1] - '0');         \
    if ((y) < 70)                                       \
        (y) += 100;                                     \

#define PARSE_DAY(day, s)                               \
    (day) = ((s)[0] - '0') * 10 + ((s)[1] - '0');       \

#define PARSE_DAY_LOSER(day, lpsz)                      \
    (day) = ((s)[0] - '0')                              \

#define PARSE_TIME_(tm, h1, h2, m1, m2, s1, s2)       \
    (tm).tm_hour = ((h1) - '0') * 10 + ((h2) - '0');  \
    (tm).tm_min = ((m1) - '0') * 10 + ((m2) - '0');   \
    (tm).tm_sec = ((s1) - '0') * 10 + ((s2) - '0');   \

#define PARSE_TIME(tm, s)                             \
    PARSE_TIME_(tm, (s)[0], (s)[1],                   \
        (s)[3], (s)[4], (s)[6], (s)[7])                 \


nai_int_t nai_time_parse(time_t* sec, const char* str, size_t len)
{
    nai_int_t r;
    nai_str_t match;
    nai_int_t month;
    time_t tval;
    const char* s;
    const char* pmonth = 0;
    const char* ptime;
    struct tm tm;

    /* RFC 1123 format with two day */
    /* ## @$$ #### ##:##:## * */
    /* RFC 1123 format with one day */
    /* # @$$ #### ##:##:## * */
    /* RFC 850 format */
    /* ##-@$$-## ##:##:## * */
    /* asctime format */
    /* @$$ ~# ##:##:## ####* */

    /* match first 3 char */
    if ((intptr_t)len < 0) {
        len = nai_strlen(str);
    };
    if (len < 3) {
        goto _fail;
    };

    s = str;
    if (nai_isdigit(s[0]) && nai_isdigit(s[1])) {
        if (s[2] == ' ') {
            nai_str_setc(&match, "## @$$ #### ##:##:## *");
            if (len >= match.len-1 && 
                nai_time_fmtmatch(s+3, match.ptr+3, match.len-3)) {
                /* RFC 1123 format with two day */
                PARSE_YEAR_1900(tm.tm_year, s + 7);
                PARSE_DAY(tm.tm_mday, s);

                pmonth = s + 3;
                ptime = s + 12;
            };
        } else if (s[2] == '-') {
            nai_str_setc(&match, "##-@$$-## ##:##:## *");
            if (len >= match.len-1 && 
                nai_time_fmtmatch(s+3, match.ptr+3, match.len-3)) {
                /* RFC 850 format */
                PARSE_YEAR_0(tm.tm_year, s + 7);
                PARSE_DAY(tm.tm_mday, s);

                pmonth = s + 3;
                ptime = s + 10;
            };
        };
    } else if (nai_isdigit(s[0]) && s[1] == ' ') {
        nai_str_setc(&match, "# @$$ #### ##:##:## *");
        if (len >= match.len-1 && 
            nai_time_fmtmatch(s+2, match.ptr+2, match.len-2)) {
            /* RFC 1123 format with one day */
            PARSE_YEAR_1900(tm.tm_year, s + 6);
            PARSE_DAY_LOSER(tm.tm_mday, s);

            pmonth = s + 2;
            ptime = s + 11;
        };
    } else if (nai_isupper(s[0]) && nai_islower(s[1]) && nai_islower(s[2])) {
        nai_str_setc(&match, "@$$ ~# ##:##:## ####*");
        if (len >= match.len-1 && 
            nai_time_fmtmatch(s+3, match.ptr+3, match.len-3)) {
            /* asctime format */
            PARSE_YEAR_1900(tm.tm_year, s + 16);

            if (s[4] == ' ')
                tm.tm_mday = 0;
            else
                tm.tm_mday = (s[4] - '0') * 10;
            tm.tm_mday += (s[5] - '0');

            pmonth = s;
            ptime = s + 7;
        };
    };

    if (pmonth == 0) {
        goto _fail;
    };

    if (tm.tm_year < 0 || 
        tm.tm_mday <= 0 || tm.tm_mday > 31) {
        goto _fail;
    };

    PARSE_TIME(tm, ptime);
    if (tm.tm_hour > 23 || 
        tm.tm_min > 59 || tm.tm_sec > 61) {
        goto _fail;
    };

    switch (*pmonth) {
    case 'J':
        month = *(pmonth + 1) == 'a' ? 0 : *(pmonth + 2) == 'n' ? 5 : 6;
        break;
    case 'F':
        month = 1;
        break;
    case 'M':
        month = *(pmonth + 2) == 'r' ? 2 : 4;
        break;
    case 'A':
        month = *(pmonth + 1) == 'p' ? 3 : 7;
        break;
    case 'S':
        month = 8;
        break;
    case 'O':
        month = 9;
        break;
    case 'N':
        month = 10;
        break;
    case 'D':
        month = 11;
        break;
    default:
        goto _fail;
    };
    if (pmonth[1] != nai_month[month][1] || 
        pmonth[2] != nai_month[month][2]) {
        goto _fail;
    };

    tm.tm_mon = month;

    if ((tm.tm_mday == 31 && 
        (month == 3 || month == 5 || month == 8 || month == 10)) || 
        (month == 1 && 
        (tm.tm_mday > 29 || 
        (tm.tm_mday == 29 && 
        ((tm.tm_year & 3) || 
        ((tm.tm_year % 100) == 0 && 
        (tm.tm_year % 400) != 100)))))) {
        goto _fail;
    };

    tm.tm_isdst = -1;
#if 0
    tm.tm_zone = 0;
    tm.tm_gmtoff = 0;
#endif

    tval = mktime(&tm);
    if (tval == -1) {
        goto _fail;
    };

    sec[0] = tval;
    r = 0;

_end:
    return r;

_fail:
    nai_errno = EINVAL;
    r = -1;
    goto _end;
};


nai_int_t nai_time_parse_rfc(time_t* sec, const char* str, size_t len)
{
    nai_int_t r;
    nai_int_t month;
    nai_int_t offset;
    nai_int_t offset_gtm;
    time_t tval;
    nai_str_t match;
    const char* s;
    const char* pmonth = 0;
    const char* pgtm;
    struct tm tm;


    /* match first 3 char */
    if ((intptr_t)len < 0) {
        len = nai_strlen(str);
    }
    if (len < 3) {
        goto _fail;
    };

    s = str;
    if (nai_isdigit(s[0]) && nai_isdigit(s[1])) {
        if (s[2] == ' ') {
            if (len <= 11) {
                goto _fail;
            };
            switch (s[11]) {
            case ' ':
                nai_str_setc(&match, "## @$$ #### ##:##:## *");
                if (len >= match.len-1 && 
                    nai_time_fmtmatch(s+3, match.ptr+3, match.len-3)) {
                    /* RFC 1123 format */
                    PARSE_YEAR_1900(tm.tm_year, s + 7);
                    PARSE_DAY(tm.tm_mday, s);
                    PARSE_TIME(tm, s + 12);

                    pmonth = s + 3;
                    pgtm = s + 20;
                };
                break;
            case ':':
                nai_str_setc(&match, "## @$$ ## #:##:## *");
                if (len >= match.len-1 && 
                    nai_time_fmtmatch(s+3, match.ptr+3, match.len-3)) {
                    /* Loser format.  This is quite bogus.  */
                    PARSE_YEAR_0(tm.tm_year, s + 7);
                    PARSE_DAY(tm.tm_mday, s);
                    PARSE_TIME_(tm, '0', s[10], 
                        s[12], s[13], s[15], s[16]);

                    pmonth = s + 3;
                    pgtm = s + 18;
                };
                break;
            default:
                if (!nai_isdigit(s[11])) {
                    break;
                };
                nai_str_setc(&match, "## @$$ ## ##:##:## *");
                if (len >= match.len-1 && 
                    nai_time_fmtmatch(s+3, match.ptr+3, match.len-3)) {
                    /* This is the old RFC 1123 tm format */
                    PARSE_YEAR_0(tm.tm_year, s + 7);
                    PARSE_DAY(tm.tm_mday, s);
                    PARSE_TIME(tm, s + 10);

                    pmonth = s + 3;
                    pgtm = s + 19;
                    break;
                };
                nai_str_setc(&match, "## @$$ ## ##:## *");
                if (len >= match.len-1 && 
                    nai_time_fmtmatch(s+3, match.ptr+3, match.len-3)) {
                    /* Loser format.  This is quite bogus.  */
                    PARSE_YEAR_0(tm.tm_year, s + 7);
                    PARSE_DAY(tm.tm_mday, s);
                    PARSE_TIME_(tm, s[10], 
                        s[11], s[13], s[14], '0', '0');

                    pmonth = s + 3;
                    pgtm = 0;
                    break;
                };
                break;
            };
        } else if (s[2] == '-') {
            if (len <= 9) {
                goto _fail;
            };
            if (s[9] == ' ') {
                nai_str_setc(&match, "##-@$$-## ##:##:## *");
                if (len >= match.len-1 && 
                    nai_time_fmtmatch(s+3, match.ptr+3, match.len-3)) {
                    /* RFC 850 format  */
                    PARSE_YEAR_0(tm.tm_year, s + 7);
                    PARSE_DAY(tm.tm_mday, s);
                    PARSE_TIME(tm, s + 10);

                    pmonth = s + 3;
                    pgtm = s + 19;
                };
            } else if (nai_isdigit(s[9])) {
                nai_str_setc(&match, "##-@$$-#### ##:##:## *");
                if (len >= match.len-1 && 
                    nai_time_fmtmatch(s+3, match.ptr+3, match.len-3)) {
                    /* RFC 1123 with dashes instead of spaces 
                     * between tm/month/year
                     * This also looks like RFC 850 with four digit years.
                     */
                    PARSE_YEAR_1900(tm.tm_year, s + 7);
                    PARSE_DAY(tm.tm_mday, s);
                    PARSE_TIME(tm, s + 12);

                    pmonth = s + 3;
                    pgtm = s + 20;
                };
            };
        };
    } else if (nai_isdigit(s[0]) && s[1] == ' ') {
        if (len <= 10) {
            goto _fail;
        };
        switch (s[10]) {
        case ' ':
            nai_str_setc(&match, "# @$$ #### ##:##:## *");
            if (len >= match.len-1 && 
                nai_time_fmtmatch(s+2, match.ptr+2, match.len-2)) {
                /* RFC 1123 format*/
                PARSE_YEAR_1900(tm.tm_year, s + 6);
                PARSE_DAY_LOSER(tm.tm_mday, s);
                PARSE_TIME(tm, s + 11);

                pmonth = s + 2;
                pgtm = s + 20;
            };
            break;
        case ':':
            nai_str_setc(&match, "# @$$ ## #:##:## *");
            if (len >= match.len-1 && 
                nai_time_fmtmatch(s+2, match.ptr+2, match.len-2)) {
                /* Loser format.  This is quite bogus.  */
                PARSE_YEAR_0(tm.tm_year, s + 6);
                PARSE_DAY_LOSER(tm.tm_mday, s);
                PARSE_TIME_(tm, '0', s[9], s[11], s[12], s[14], s[15]);

                pmonth = s + 2;
                pgtm = s + 17;
            };
            break;
        default:
            if (!nai_isdigit(s[10])) {
                break;
            };
            nai_str_setc(&match, "# @$$ ## ##:##:## *");
            if (len >= match.len-1 && 
                nai_time_fmtmatch(s+2, match.ptr+2, match.len-2)) {
                /* This is the old RFC 1123 tm format */
                PARSE_YEAR_0(tm.tm_year, s + 6);
                PARSE_DAY_LOSER(tm.tm_mday, s);
                PARSE_TIME(tm, s + 9);

                pmonth = s + 2;
                pgtm = s + 18;
                break;
            };
            nai_str_setc(&match, "# @$$ ## ##:## *");
            if (len >= match.len-1 && 
                nai_time_fmtmatch(s+2, match.ptr+2, match.len-2)) {
                /* Loser format.  This is quite bogus.  */
                PARSE_YEAR_0(tm.tm_year, s + 6);
                PARSE_DAY_LOSER(tm.tm_mday, s);
                PARSE_TIME_(tm, s[9], s[10], s[12], s[13], '0', '0');

                pmonth = s + 2;
                pgtm = 0;
                break;
            };
        };
    } else if (nai_isupper(s[0]) && nai_islower(s[1]) && nai_islower(s[2])) {
        nai_str_setc(&match, "@$$ ~# ##:##:## ####*");
        if (len >= match.len-1 && 
            nai_time_fmtmatch(s+3, match.ptr+3, match.len-3)) {
            /* asctime format */
            PARSE_YEAR_1900(tm.tm_year, s + 16);
            PARSE_TIME(tm, s + 7);

            if (s[4] == ' ')
                tm.tm_mday = 0;
            else
                tm.tm_mday = (s[4] - '0') * 10;
            tm.tm_mday += (s[5] - '0');

            pmonth = s + 3;
            pgtm = 0;
        };
    } else if (s[0] == ' ' && nai_isdigit(s[1]) && s[2] == ' ') {
        nai_str_setc(&match, " # @$$ #### ##:##:## *");
        if (len >= match.len-1 && 
            nai_time_fmtmatch(s+3, match.ptr+3, match.len-3)) {
            /* RFC 1123 format with a space instead of a leading zero. */
            PARSE_YEAR_1900(tm.tm_year, s + 7);
            PARSE_DAY_LOSER(tm.tm_mday, s + 1);
            PARSE_TIME(tm, s + 12);

            pmonth = s + 3;
            pgtm = s + 20;
        };
    };

    if (pmonth == 0) {
        goto _fail;
    };

    if (tm.tm_year < 0 || 
        tm.tm_mday <= 0 || tm.tm_mday > 31) {
        goto _fail;
    };


    switch (*pmonth) {
    case 'J':
        month = *(pmonth + 1) == 'a' ? 0 : *(pmonth + 2) == 'n' ? 5 : 6;
        break;
    case 'F':
        month = 1;
        break;
    case 'M':
        month = *(pmonth + 2) == 'r' ? 2 : 4;
        break;
    case 'A':
        month = *(pmonth + 1) == 'p' ? 3 : 7;
        break;
    case 'S':
        month = 8;
        break;
    case 'O':
        month = 9;
        break;
    case 'N':
        month = 10;
        break;
    case 'D':
        month = 11;
        break;
    default:
        goto _fail;
    };
    if (pmonth[1] != nai_month[month][1] || 
        pmonth[2] != nai_month[month][2]) {
        goto _fail;
    };

    tm.tm_mon = month;

    if ((tm.tm_mday == 31 && 
        (month == 3 || month == 5 || month == 8 || month == 10)) || 
        (month == 1 && 
        (tm.tm_mday > 29 || 
        (tm.tm_mday == 29 && 
        ((tm.tm_year & 3) || 
        ((tm.tm_year % 100) == 0 && 
        (tm.tm_year % 400) != 100)))))) {
        goto _fail;
    };


    tm.tm_isdst = -1;
#if 0
    tm.tm_zone = 0;
    tm.tm_gmtoff = 0;
#endif

    tval = mktime(&tm);
    if (tval == -1) {
        goto _fail;
    };

    /* The number of seconds off of GMT the time */
    if (pgtm && pgtm[0] != '\0' && 
        pgtm[1] != '\0') {

        offset_gtm = 0;
        pgtm ++;
        switch (*(pgtm ++)) {
        case '-':
            offset = atoi(pgtm);
            offset_gtm -= (offset / 100) * 60 * 60;
            offset_gtm -= (offset % 100) * 60;
            break;
        case '+':
            offset = atoi(pgtm);
            offset_gtm += (offset / 100) * 60 * 60;
            offset_gtm += (offset % 100) * 60;
            break;
        default:
            break;
        };
        tval -= offset_gtm;
    };

    sec[0] = tval;
    r = 0;

_end:
    return r;

_fail:
    nai_errno = EINVAL;
    r = -1;
    goto _end;
};


#if defined(_WIN32)


#include <timezoneapi.h>


nai_int_t nai_time_get_zone(nai_int_t isdst)
{
    nai_int_t r;
    DWORD n;
    TIME_ZONE_INFORMATION  tz;


    (void)isdst;

    n = GetTimeZoneInformation(&tz);
    switch (n) {
    case TIME_ZONE_ID_UNKNOWN:
        r = -(tz.Bias);
        break;

    case TIME_ZONE_ID_STANDARD:
        r = -(tz.Bias + tz.StandardBias);
        break;

    case TIME_ZONE_ID_DAYLIGHT:
        r = -(tz.Bias + tz.DaylightBias);
        break;

    default: /* TIME_ZONE_ID_INVALID */
        r = 0;
        break;
    };

    return r;
};


nai_int_t nai_time_update_zone()
{
    return 0;
};


#else


nai_int_t nai_time_get_zone(nai_int_t isdst)
{
    nai_int_t r;

#if (NAI_HAVE_TM_GMTOFF)

    time_t s;
    struct tm t;


    s = time(0);
    t = *localtime(&s);

    if (!!isdst == !!t.tm_isdst) {
        r = t.tm_gmtoff;
    } else if (isdst && !t.tm_isdst) {
        r = t.tm_gmtoff - 3600;
    } else {
        r = t.tm_gmtoff + 3600;
    };


#elif (__solaris__)
    r = -(isdst ? altzone : timezone);
#else
    r = -(isdst ? timezone + 3600 : timezone);
#endif

    return r;
};


nai_int_t nai_time_update_zone()
{
#if (__freebsd__)

    if (getenv("TZ")) {
        goto _end;
    };

    putenv("TZ=UTC");

    tzset();

    unsetenv("TZ");

    tzset();

_end:

#elif (__linux__)

    time_t      s;
    struct tm  *t;
    char        buf[4];

    s = time(0);

    t = localtime(&s);

    strftime(buf, 4, "%H", t);

#endif

    return 0;
};


#endif


