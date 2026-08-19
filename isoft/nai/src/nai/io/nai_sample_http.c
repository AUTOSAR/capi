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
/// @file       nai_sample_http.c
/// @brief      
/// @details
/// @date       2022-06-13
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_sample_http.h"


nai_sult_t nai_sample_parse_status_line(nai_sample_request_t* r)
{
    nai_sult_t rc;
    nai_sample_parse_state_t* s;
    nai_int_t state;
    char ch;
    char* cur;
    char* cend;

    enum {
        stStart = 0, 
        stHTTP_0, 
        stHTTP_1, 
        stHTTP_2, 
        stHTTP_3, 
        stSlash, 
        stMajor_0, 
        stMajor_1, 
        stMinor_0, 
        stMinor_1, 
        stBeforeStatus, 
        stStatus, 
        stBeforeText, 
        stText, 
        stEndline, 
        stEnd
    };


    cur = r->cur;
    cend = r->cend;
    s = &r->parse;
    state = s->state;
    for (;;) {
_loop:
        switch (state) {
        case stStart:
            state = stHTTP_1;
            /* fallthrough */

        case stHTTP_0:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;
            if (ch != 'H') {
                /* invalid http version of status line */
                rc = NAI_E_FAILED;
                goto _fail;
            };

            state = stHTTP_2;
            /* fallthrough */

        case stHTTP_1:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;
            if (ch != 'T') {
                /* invalid http version of status line */
                rc = NAI_E_FAILED;
                goto _fail;
            };

            state = stHTTP_2;
            /* fallthrough */

        case stHTTP_2:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;
            if (ch != 'T') {
                /* invalid http version of status line */
                rc = NAI_E_FAILED;
                goto _fail;
            };

            state = stHTTP_3;
            /* fallthrough */

        case stHTTP_3:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;
            if (ch != 'P') {
                /* invalid http version of status line */
                rc = NAI_E_FAILED;
                goto _fail;
            };

            state = stSlash;
            /* fallthrough */

        case stSlash:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;
            if (ch != '/') {
                /* invalid http version of status line */
                rc = NAI_E_FAILED;
                goto _fail;
            };

            r->version = 0;
            state = stMajor_0;
            /* fallthrough */

        case stMajor_0:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;
            if (ch < '0' || ch > '9') {
                /* invalid http version of status line */
                rc = NAI_E_FAILED;
                goto _fail;
            };

            r->major = ch - '0';
            state = stMajor_1;
            /* fallthrough */

        case stMajor_1:
            for ( ; cur < cend; ) {
                ch = *cur ++;
                if (ch == '.') {
                    state = stMinor_0;
                    goto _loop;
                } else if (ch < '0' || ch > '9') {
                    /* invalid http version of status line */
                    rc = NAI_E_FAILED;
                    goto _fail;
                };

                r->major = r->major * 10 + ch - '0';
                if (r->major > 1) {
                    rc = NAI_E_FAILED;
                    goto _fail;
                };
            };
            goto _loop;

        case stMinor_0:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;
            if (ch < '0' || ch > '9') {
                /* invalid http version of status line */
                rc = NAI_E_FAILED;
                goto _fail;
            };


            r->minor = ch - '0';
            state = stMinor_1;
            /* fallthrough */

        case stMinor_1:
            for ( ; cur < cend; ) {
                ch = *cur ++;
                switch (ch) {
                case ' ':
                case '\t':
                    state = stBeforeStatus;
                    break;
                default:
                    if (ch < '0' || ch > '9') {
                        /* invalid http version of request line */
                        rc = NAI_E_FAILED;
                        goto _fail;
                    };

                    r->minor = r->minor * 10 + ch - '0';
                    continue;
                };

                goto _loop;
            };
            goto _end;

        case stBeforeStatus:
            for ( ; cur < cend; ) {
                ch = *cur ++;
                switch (ch) {
                case '\0':
                    /** end string */
                    rc = NAI_E_FAILED;
                    goto _fail;
                case ' ':
                case '\t':
                    break;
                default:
                    if (ch < '0' || ch > '9') {
                        /* invalid http version of status line */
                        rc = NAI_E_FAILED;
                        goto _fail;
                    };

                    r->status = ch - '0';
                    state = stStatus;
                    goto _loop;
                };
            };
            goto _end;

        case stStatus:
            for ( ; cur < cend; ) {
                ch = *cur ++;
                switch (ch) {
                case '\0':
                    /** end string */
                    rc = NAI_E_FAILED;
                    goto _fail;
                case ' ':
                case '\t':
                    state = stBeforeText;
                    goto _loop;
                default:
                    if (ch < '0' || ch > '9') {
                        /* invalid http version of status line */
                        rc = NAI_E_FAILED;
                        goto _fail;
                    };

                    r->status = r->status * 10 + ch - '0';
                    break;
                };
            };
            goto _end;

        case stBeforeText:
            for ( ; cur < cend; ) {
                ch = *cur ++;
                switch (ch) {
                case '\0':
                    /** end string */
                    rc = NAI_E_FAILED;
                    goto _fail;
                case ' ':
                case '\t':
                    break;
                default:
                    state = stText;
                    goto _loop;
                };
            };
            goto _end;

        case stText:
            for ( ; cur < cend; ) {
                ch = *cur ++;
                switch (ch) {
                case '\0':
                    /** end string */
                    rc = NAI_E_FAILED;
                    goto _fail;

                case '\r':
                case '\n':
                    /* next */
                    if (ch == '\r') {
                        state = stEndline;
                    } else {
                        state = stEnd;
                    };
                    goto _loop;

                default:
                    break;
                };
            };
            goto _end;

        case stEndline:
            for ( ; cur < cend; /*cur ++*/) {
                ch = *cur;
                switch (ch) {
                case '\n':
                    cur ++;
                    state = stEnd;
                    goto _loop;
                default:
                    state = stEnd;
                    goto _loop;
                };
            };
            goto _end;

        case stEnd:
            assert(state == stEnd);
            goto _end;

        default:
            rc = NAI_E_FAILED;
            goto _fail;
        };
    };

_end:
    r->cur = cur;
    s->state = state;
    rc = state != stEnd ? NAI_E_AGAIN : 0;

_fail:
    return rc;
};


nai_sult_t nai_sample_parse_header_line(
    nai_sample_request_t* r, nai_mem_t* k, nai_mem_t* v)
{
    nai_sult_t rc;
    nai_sample_parse_state_t* s;
    nai_int_t state;
    char ch;
    char* cur;
    char* cend;

    enum {
        stStart = 0, 
        stKey, 
        stBeforeColon, 
        stColon, 
        stAfterColon, 
        stValue, 
        stEndline, 
        stEndheader, 
        stEnd
    };


    cur = r->cur;
    cend = r->cend;
    s = &r->parse;
    state = s->state;
    for (;;) {
_loop:
        switch (state) {
        case stStart:
            nai_str_setm(&s->cur, cur, 0);
            state = stKey;
            /* fallthrough */

        case stKey:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case '\0':
                    /** end string */
                    rc = NAI_E_FAILED;
                    goto _fail;
                case '\r':
                case '\n':
                    nai_str_setl(&s->cur, cur - nai_str(&s->cur));
                    if (nai_str_len(&s->cur) <= 0) {
                        /* end of header */
                        if (ch == '\r') {
                            cur ++;
                            state = stEndheader;
                            goto _loop;
                        } else {
                            cur ++;
                            state = stEnd;
                            goto _end;
                        };
                    };

                    /* end of line without value */
                    rc = NAI_E_FAILED;
                    goto _fail;
                case ':':
                case ' ':
                case '\t':
                    nai_str_setl(&s->cur, cur - nai_str(&s->cur));
                    if (nai_str_len(&s->cur) <= 0) {
                        rc = NAI_E_FAILED;
                        goto _fail;
                    };
                    s->stash = s->cur;
                    nai_str_setn(&s->cur);
                    cur ++;
                    if (ch == ':') {
                        state = stAfterColon;
                    } else {
                        state = stBeforeColon;
                    };
                    goto _loop;

                case '-':
                    break;
                default:
                    if (!nai_isalnum(ch)) {
                        /* invalid char of header name */
                        rc = NAI_E_FAILED;
                        goto _fail;
                    };
                    break;
                };
            };
            goto _end;

        case stBeforeColon:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case '\0':
                    /** end string */
                    rc = NAI_E_FAILED;
                    goto _fail;
                case '\r':
                case '\n':
                    /** end of line without value */
                    rc = NAI_E_FAILED;
                    goto _fail;
                case ' ':
                case '\t':
                    break;
                case ':':
                    cur ++;
                    state = stAfterColon;
                    goto _loop;
                default:
                    break;
                };
            };
            goto _end;

        case stAfterColon:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case '\0':
                    /** end string */
                    rc = NAI_E_FAILED;
                    goto _fail;
                case ' ':
                case '\t':
                    break;
                default:
                    nai_str_setm(&s->cur, cur, 0);
                    state = stValue;
                    goto _loop;
                };
            };
            goto _end;

        case stValue:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case '\0':
                    /** end string */
                    rc = NAI_E_FAILED;
                    goto _fail;

                case '\r':
                case '\n':
                    nai_str_setl(&s->cur, cur - nai_str(&s->cur));
                    /* next */
                    cur ++;
                    if (ch == '\r') {
                        state = stEndline;
                    } else {
                        state = stStart;
                        goto _out;
                    };
                    goto _loop;

                default:
                    break;
                };
            };
            goto _end;

        case stEndline:
            for ( ; cur < cend; /*cur ++*/) {
                ch = *cur;
                switch (ch) {
                case '\n':
                    cur ++;
                    state = stStart;
                    goto _out;
                default:
                    state = stStart;
                    goto _out;
                };
            };
            goto _end;

        case stEndheader:
            for ( ; cur < cend; /*cur ++*/) {
                ch = *cur;
                switch (ch) {
                case '\n':
                    cur ++;
                    state = stEnd;
                    goto _end;
                default:
                    state = stEnd;
                    goto _end;
                };
            };
            goto _end;

        case stEnd:
            assert(state == stEnd);
            goto _end;

        default:
            rc = NAI_E_FAILED;
            goto _fail;
        };
    };

_out:
    *k = s->stash;
    *v = s->cur;
    nai_str_setn(&s->cur);

_end:
    r->cur = cur;
    s->state = state;
    if (nai_str(&s->cur)) {
        nai_str_setl(&s->cur, cur - nai_str(&s->cur));
    };

    if (state == stStart) {
        rc = NAI_OK;
    } else {
        rc = state != stEnd ? NAI_E_AGAIN : NAI_DONE;
    };

_fail:
    return rc;
};


nai_sult_t nai_sample_parse_header_skip(nai_sample_request_t* r)
{
    nai_sult_t rc;
    nai_sample_parse_state_t* s;
    nai_int_t state;
    char ch;
    char* cur;
    char* cend;

    enum {
        stStart = 0, 
        stLine, 
        stEndline, 
        stEndheader, 
        stEnd
    };


    cur = r->cur;
    cend = r->cend;
    s = &r->parse;
    state = s->state;
    for (;;) {
_loop:
        switch (state) {
        case stStart:
            if (cur >= cend) {
                goto _end;
            };

            /* check first char */
            ch = *cur;
            switch (ch) {
            case '\0':
                /** end string */
                rc = NAI_E_FAILED;
                goto _fail;
            case '\r':
            case '\n':
                /* end of header */
                if (ch == '\r') {
                    cur ++;
                    state = stEndheader;
                } else {
                    cur ++;
                    state = stEnd;
                };
                goto _loop;

            default:
                cur ++;
                break;
            };

            state = stLine;
            /* fallthrough */

        case stLine:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case '\0':
                    /** end string */
                    rc = NAI_E_FAILED;
                    goto _fail;

                case '\r':
                case '\n':
                    /* next */
                    cur ++;
                    if (ch == '\r') {
                        state = stEndline;
                    } else {
                        state = stStart;
                    };
                    goto _loop;

                default:
                    break;
                };
            };
            goto _end;

        case stEndline:
            for ( ; cur < cend; /*cur ++*/) {
                ch = *cur;
                switch (ch) {
                case '\n':
                    cur ++;
                    state = stStart;
                    goto _loop;
                default:
                    state = stStart;
                    goto _loop;
                };
            };
            goto _end;

        case stEndheader:
            for ( ; cur < cend; /*cur ++*/) {
                ch = *cur;
                switch (ch) {
                case '\n':
                    cur ++;
                    state = stEnd;
                    goto _end;
                default:
                    state = stEnd;
                    goto _end;
                };
            };
            goto _end;

        case stEnd:
            assert(state == stEnd);
            goto _end;

        default:
            rc = NAI_E_FAILED;
            goto _fail;
        };
    };

_end:
    r->cur = cur;
    s->state = state;

    rc = state != stEnd ? NAI_E_AGAIN : NAI_DONE;

_fail:
    return rc;
};


