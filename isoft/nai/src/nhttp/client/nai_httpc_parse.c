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
/// @file       nai_httpc_parse.c
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai_httpc_core.h"


nai_sult_t nai_httpc_parse_status_line(nai_httpc_request_t* r)
{
    nai_sult_t rc;
    nai_str_t str;
    nai_buf_t* b;
    nai_httpc_proto_v1_t* p;
    nai_httpc_parse_state_t* s;
    intptr_t n;
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


    p = (nai_httpc_proto_v1_t*)r->proto;
    b = p->head;
    cur = (char*)nai_buf_ptr(b);
    cend = cur + nai_buf_size(b);
    s = &p->parse;
    state = s->state;
    for (;;) {
_loop:
        switch (state) {
        case stStart:
            nai_str_setn(&s->cur);
            state = stHTTP_1;
            /* fallthrough */

        case stHTTP_0:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;
            if (ch != 'H') {
                /* invalid http version of status line */
                rc = NAI_HTTPC_BAD_RESPONSE;
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
                rc = NAI_HTTPC_BAD_RESPONSE;
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
                rc = NAI_HTTPC_BAD_RESPONSE;
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
                rc = NAI_HTTPC_BAD_RESPONSE;
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
                rc = NAI_HTTPC_BAD_RESPONSE;
                goto _fail;
            };

            r->headers_in.version = 0;
            state = stMajor_0;
            /* fallthrough */

        case stMajor_0:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;
            if (ch < '0' || ch > '9') {
                /* invalid http version of status line */
                rc = NAI_HTTPC_BAD_RESPONSE;
                goto _fail;
            };

            r->headers_in.major = ch - '0';
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
                    rc = NAI_HTTPC_BAD_RESPONSE;
                    goto _fail;
                };

                r->headers_in.major = r->headers_in.major * 10 + ch - '0';
                if (r->headers_in.major > 1) {
                    rc = NAI_HTTPC_BAD_RESPONSE;
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
                rc = NAI_HTTPC_BAD_RESPONSE;
                goto _fail;
            };


            r->headers_in.minor = ch - '0';
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
                        rc = NAI_HTTPC_BAD_RESPONSE;
                        goto _fail;
                    };

                    r->headers_in.minor = r->headers_in.minor * 10 + ch - '0';
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
                    rc = NAI_HTTPC_BAD_RESPONSE;
                    goto _fail;
                case ' ':
                case '\t':
                    break;
                default:
                    if (ch < '0' || ch > '9') {
                        /* invalid http version of status line */
                        rc = NAI_HTTPC_BAD_RESPONSE;
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
                    rc = NAI_HTTPC_BAD_RESPONSE;
                    goto _fail;
                case ' ':
                case '\t':
                    state = stBeforeText;
                    goto _loop;
                default:
                    if (ch < '0' || ch > '9') {
                        /* invalid http version of status line */
                        rc = NAI_HTTPC_BAD_RESPONSE;
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
                    rc = NAI_HTTPC_BAD_RESPONSE;
                    goto _fail;
                case ' ':
                case '\t':
                    break;
                default:
                    nai_str_setm(&s->cur, cur-1, 0);
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
                    rc = NAI_HTTPC_BAD_RESPONSE;
                    goto _fail;

                case '\r':
                case '\n':
                    nai_str_setm(&str, 
                        nai_str(&s->cur), cur - nai_str(&s->cur) - 1);
                    nai_str_at(&str, nai_str_len(&str)) = 0;

                    /* set status text */
                    if (r->copy_headers) {
                        n = nai_str_dup(&r->status_text, 
                            nai_str(&str), nai_str_len(&str), r->pool);
                        if (n < 0) {
                            rc = nai_sult_from_errno();
                            nai_log_alert(NAI_LOG_HTTPC, 
                                nai_sult_to_errno(rc), 
                                "dupilcate status text failed");
                            goto _fail;
                        };
                    } else {
                        r->status_text = str;
                    };

                    /* next */
                    nai_str_setn(&s->cur);
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
            rc = NAI_HTTPC_INTERNAL_ERROR;
            goto _fail;
        };
    };

_end:
    nai_buf_rcommit(b, cur - (char*)nai_buf_ptr(b));
    s->state = state;
    if (nai_str(&s->cur)) {
        nai_str_setl(&s->cur, cur - nai_str(&s->cur));
    };
    rc = state != stEnd ? NAI_E_AGAIN : 0;

_fail:
    return rc;
};


nai_sult_t nai_httpc_parse_headers(nai_httpc_request_t* r)
{
    nai_sult_t rc;
    nai_buf_t* b;
    nai_mem_t key, value;
    nai_httpc_proto_v1_t* p;
    nai_httpc_parse_state_t* s;
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


    p = (nai_httpc_proto_v1_t*)r->proto;
    b = p->head;
    cur = (char*)nai_buf_ptr(b);
    cend = cur + nai_buf_size(b);
    s = &p->parse;
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
                    rc = NAI_HTTPC_BAD_RESPONSE;
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
                    rc = NAI_HTTPC_BAD_RESPONSE;
                    goto _fail;
                case ':':
                case ' ':
                case '\t':
                    nai_str_setl(&s->cur, cur - nai_str(&s->cur));
                    if (nai_str_len(&s->cur) <= 0) {
                        rc = NAI_HTTPC_BAD_RESPONSE;
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
                        rc = NAI_HTTPC_BAD_RESPONSE;
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
                    rc = NAI_HTTPC_BAD_RESPONSE;
                    goto _fail;
                case '\r':
                case '\n':
                    /** end of line without value */
                    rc = NAI_HTTPC_BAD_RESPONSE;
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
                    rc = NAI_HTTPC_BAD_RESPONSE;
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
                    rc = NAI_HTTPC_BAD_RESPONSE;
                    goto _fail;

                case '\r':
                case '\n':
                    nai_str_setm(&key, 
                        nai_str(&s->stash), nai_str_len(&s->stash));
                    nai_str_setm(&value, 
                        nai_str(&s->cur), cur - nai_str(&s->cur));

                    nai_str_at(&key, nai_str_len(&key)) = 0;
                    nai_str_at(&value, nai_str_len(&value)) = 0;

                    /* add key now */
                    rc = nai_httpc_headers_in_add(r, 
                        (nai_str_t*)&key, (nai_str_t*)&value, 
                        r->copy_headers ? NAI_HTTP_DUP_HEADER : 0);
                    if (rc < 0) {
                        nai_log_error(NAI_LOG_HTTPC, 
                            nai_sult_to_errno(rc), 
                            "add response header failed");
                        goto _fail;
                    };

                    /* next */
                    nai_str_setn(&s->cur);
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
            rc = NAI_HTTPC_INTERNAL_ERROR;
            goto _fail;
        };
    };

_end:
    nai_buf_rcommit(b, cur - (char*)nai_buf_ptr(b));
    s->state = state;
    if (nai_str(&s->cur)) {
        nai_str_setl(&s->cur, cur - nai_str(&s->cur));
    };
    rc = state != stEnd ? NAI_E_AGAIN : 0;

_fail:
    return rc;
};


nai_sult_t nai_httpc_parse_chunked(nai_httpc_request_t* r, nai_buf_t* body)
{
    nai_sult_t rc;
    nai_buf_t* b;
    nai_httpc_proto_v1_t* p;
    nai_httpc_parse_state_t* s;
    nai_int_t state;
    char ch;
    char* cur;
    char* cend;
    uint64_t chunk;

    enum {
        stStart = 0, 
        stSize, 
        stExternion, 
        stEndline, 
        stEnd, 
        stData, 
        stDataEndline, 
        stDataEnd, 
        stTrailer, 
        stTrailerLine, 
        stTrailerEndline, 
        stTrailerEndlast, 
        stTrailerEnd, 
        stDone, 
    };

    p = (nai_httpc_proto_v1_t*)r->proto;
    b = body;
    cur = (char*)nai_buf_ptr(b);
    cend = cur + nai_buf_size(b);
    s = &p->parse;
    state = s->state;
    chunk = s->chunk;

    for (;;) {
_loop:
        switch (state) {
        case stStart:
            state = stSize;
            chunk = 0;
            /* fallthrough */

        case stSize:
            for (; cur < cend;) {
                if (chunk > NAI_OFF_T_MAX / 16) {
                    rc = NAI_HTTPC_BAD_RESPONSE;
                    goto _fail;
                };

                ch = *cur ++;

                if (ch >= '0' && ch <= '9') {
                    chunk = chunk * 16 + ch - '0';
                    continue;
                };
                if (ch >= 'a' && ch <= 'z') {
                    chunk = chunk * 16 + 10 + ch - 'a';
                    continue;
                };
                if (ch >= 'A' && ch <= 'Z') {
                    chunk = chunk * 16 + 10 + ch - 'A';
                    continue;
                };

                switch (ch) {
                case '\r':
                    state = stEndline;
                    goto _loop;
                case '\n':
                    state = stEnd;
                    goto _loop;
                case ';':
                case ' ':
                case '\t':
                    state = stExternion;
                    break;
                default:
                    cur --;
                    rc = NAI_HTTPC_BAD_RESPONSE;
                    goto _fail;
                };
            };

            goto _end;

        case stExternion:
            for (; cur < cend; cur ++) {
                ch = *cur;

                switch (ch) {
                case '\r':
                    cur ++;
                    state = stEndline;
                    goto _loop;
                case '\n':
                    cur ++;
                    state = stEnd;
                    goto _loop;
                default:
                    break;
                };
            };

            goto _end;

        case stEndline:
            for (; cur < cend; /*cur ++*/) {
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
            if (chunk == 0) {
                state = stTrailer;
                goto _loop;
            };

            state = stData;
            goto _end;

        case stData:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;

            switch (ch) {
            case '\r':
                break;
            case '\n':
                state = stDataEnd;
                goto _loop;
            default:
                rc = NAI_HTTPC_BAD_RESPONSE;
                goto _fail;
            };

            state = stDataEndline;
            /* fallthrough */

        case stDataEndline:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur;
            if (ch == '\n') {
                cur ++;
            };

            state = stDataEnd;
            /* fallthrough */

        case stDataEnd:
            chunk = 0;
            state = stSize;
            goto _loop;

        case stTrailer:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;

            switch (ch) {
            case '\r':
                state = stTrailerEndlast;
                goto _loop;
            case '\n':
                state = stTrailerEnd;
                goto _loop;
            default:
                break;
            };

            state = stTrailerLine;
            /* fallthrough */

        case stTrailerLine:
            for (; cur < cend; cur ++) {
                ch = *cur;

                switch (ch) {
                case '\r':
                    cur ++;
                    state = stTrailerEndline;
                    goto _loop;
                case '\n':
                    cur ++;
                    state = stTrailer;
                    goto _loop;
                default:
                    break;
                };
            };

            goto _end;

        case stTrailerEndline:
            for (; cur < cend; /* cur ++ */) {
                ch = *cur;

                switch (ch) {
                case '\n':
                    cur ++;
                    state = stTrailer;
                    goto _loop;
                default:
                    state = stTrailer;
                    goto _loop;
                };
            };

            goto _end;

        case stTrailerEndlast:
            for (; cur < cend; /* cur ++ */) {
                ch = *cur;

                switch (ch) {
                case '\n':
                    cur ++;
                    state = stTrailerEnd;
                    goto _loop;
                default:
                    state = stTrailerEnd;
                    goto _loop;
                };
            };

            goto _end;

        case stTrailerEnd:
            state = stDone;
            /* fallthrough */

        case stDone:
            goto _end;

        default:
            break;
        };
    };

_end:
    switch (state) {
    case stData:
    case stDone:
        p->read_left = chunk;
        rc = 0;
        break;
    default:
        rc = NAI_E_AGAIN;
        break;
    };

_fail:
    nai_buf_rcommit(b, cur - (char*)nai_buf_ptr(b));
    s->state = state;
    s->chunk = chunk;
    return rc;
};


