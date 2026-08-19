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
/// @file       nai_https_parse.c
/// @brief      
/// @details
/// @date       2021-02-15
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai_https_core.h"


#if !(NAI_HAVE_BIG_ENDIAN) && !(NAI_HAVE_ALIGNED)


#define nai_str3_cmp(m, c0, c1, c2, c3)                                 \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)       \

#define nai_str3Ocmp(m, c0, c1, c2, c3)                                 \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)       \

#define nai_str4cmp(m, c0, c1, c2, c3)                                  \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)       \

#define nai_str5cmp(m, c0, c1, c2, c3, c4)                              \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)       \
    && m[4] == c4                                                       \

#define nai_str6cmp(m, c0, c1, c2, c3, c4, c5)                          \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0) &&    \
    (((uint32_t *) m)[1] & 0xffff) == ((c5 << 8) | c4)                  \

#define nai_str7_cmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                 \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0) &&    \
    ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)   \

#define nai_str8cmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                  \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0) &&    \
    ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)   \

#define nai_str9cmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8)              \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0) &&    \
    ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)   \
    && m[8] == c8                                                       \


#else


#define nai_str3_cmp(m, c0, c1, c2, c3)                                 \
    m[0] == c0 && m[1] == c1 && m[2] == c2                              \

#define nai_str3Ocmp(m, c0, c1, c2, c3)                                 \
    m[0] == c0 && m[2] == c2 && m[3] == c3                              \

#define nai_str4cmp(m, c0, c1, c2, c3)                                  \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3                \

#define nai_str5cmp(m, c0, c1, c2, c3, c4)                              \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 && m[4] == c4  \

#define nai_str6cmp(m, c0, c1, c2, c3, c4, c5)                          \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 &&             \
    m[4] == c4 && m[5] == c5                                            \

#define nai_str7_cmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                 \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 &&             \
    m[4] == c4 && m[5] == c5 && m[6] == c6                              \

#define nai_str8cmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                  \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 &&             \
    m[4] == c4 && m[5] == c5 && m[6] == c6 && m[7] == c7                \

#define nai_str9cmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8)              \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 &&             \
    m[4] == c4 && m[5] == c5 && m[6] == c6 && m[7] == c7 && m[8] == c8  \


#endif


enum {
    dotp =          0x01,
    slash =         0x02,
    question =      0x04,
    hashnil =       0x08,
    escapeScheme =  0x10,
    escapeHost =    0x20, 
    escapePath =    0x40,
    escapeArgu =    0x80
};


typedef struct nai_https_uri_s {
    nai_int_t inited;
    uint8_t delims[256];
} nai_https_uri_t;


static nai_https_uri_t nai_https_uri;


nai_sult_t nai_https_uri_delims_init()
{
    nai_int_t ch;
    uint8_t* delims;


    if (nai_https_uri.inited) {
        goto _end;
    };

    delims = nai_https_uri.delims;
    delims[0] = hashnil;
    delims['#'] = hashnil;
    delims['?'] = question;
    delims['.'] = dotp;
    delims['/'] = slash;
    delims['\\'] = slash;

    /* unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
     * reserved      = gen-delims / sub-delims
     * gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
     * sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
     *               / "*" / "+" / "," / ";" / "="
     * userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
     * host          = IP-literal / IPv4address / reg-name
     * port          = *DIGIT
     * reg-name      = *( unreserved / pct-encoded / sub-delims )
     * pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
     * query         = *( pchar / "/" / "?" )
     * fragment      = *( pchar / "/" / "?" )
     */

    for (ch = 0; ch < (nai_int_t)sizeof(nai_https_uri.delims); ch ++) {
        /* alpha, num and +-. */
        if (!isalnum(ch) && !strchr("+-.", ch)) {
            delims[ch] |= escapeScheme;
        };
        /* alpha, num and -._~!$&'()*+,;= */
        if (!isalnum(ch) &&
            !strchr("-._~!$&'()*+,;=", ch)) {
            delims[ch] |= escapeHost;
        };
        /* alpha, num and -._~/?#[]@!$&'()*+,;=% */
        if (!isalnum(ch) &&
            !strchr("-._~/?#[]@!$&'()*+,;=%", ch)) {
            delims[ch] |= escapePath;
        };
        if (ch < 32 || ch > 126 || strchr("", ch)) {
            delims[ch] |= escapeArgu;
        };
        /* if (ch < 32 || ch > 126 || */
        /*     strchr(";\\?@&=+$, <>#%\"{}|^[]`", ch)) { */
        /*     delims[ch] |= escapeArg;    [> uri args case, from atl <] */
        /* }; */
    };

    nai_https_uri.inited = 1;

_end:
    return 0;
};


nai_sult_t nai_https_parse_request_line(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_mem_t host;
    nai_buf_t* b;
    nai_https_proto_v1_t* p;
    nai_https_parse_state_t* s;
    nai_int_t state;
    char* m;
    char* cur;
    char* cend;
    uint8_t ch;
    uint8_t* delims = nai_https_uri.delims;


    enum {
        stStart = 0, 
        stMethod,
        stBeforeUri, 
        stSchema, 
        stSchemaSlash_0, 
        stSchemaSlash_1, 
        stHost, 
        stPath, 
        stQuery, 
        stFragment, 
        stBeforeHTTP, 
        stHTTP_1, 
        stHTTP_2, 
        stHTTP_3, 
        stSlash, 
        stMajor_0, 
        stMajor_1, 
        stMinor_0, 
        stMinor_1, 
        stEndline, 
        stEnd
    };

    p = (nai_https_proto_v1_t*)r->proto;
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
            state = stMethod;
            /* fallthrough */

        case stMethod:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                if (ch == ' ') {
                    m = nai_str(&s->cur);
                    nai_str_setm(&r->method_name, m, cur - m);

                    switch (cur - m) {
                    case 3:
                        if (nai_str3_cmp(m, 'G', 'E', 'T', ' ')) {
                            r->method = NAI_HTTP_GET;
                            break;
                        }
                        if (nai_str3_cmp(m, 'P', 'U', 'T', ' ')) {
                            r->method = NAI_HTTP_PUT;
                            break;
                        }
                        break;

                    case 4:
                        if (m[1] == 'O') {
                            if (nai_str3Ocmp(m, 'P', 'O', 'S', 'T')) {
                                r->method = NAI_HTTP_POST;
                                break;
                            }
                            if (nai_str3Ocmp(m, 'C', 'O', 'P', 'Y')) {
                                r->method = NAI_HTTP_COPY;
                                break;
                            }
                            if (nai_str3Ocmp(m, 'M', 'O', 'V', 'E')) {
                                r->method = NAI_HTTP_MOVE;
                                break;
                            }
                            if (nai_str3Ocmp(m, 'L', 'O', 'C', 'K')) {
                                r->method = NAI_HTTP_LOCK;
                                break;
                            }

                        } else {
                            if (nai_str4cmp(m, 'H', 'E', 'A', 'D')) {
                                r->method = NAI_HTTP_HEAD;
                                break;
                            }
                        }
                        break;

                    case 5:
                        if (nai_str5cmp(m, 'M', 'K', 'C', 'O', 'L')) {
                            r->method = NAI_HTTP_MKCOL;
                            break;
                        }
                        if (nai_str5cmp(m, 'P', 'A', 'T', 'C', 'H')) {
                            r->method = NAI_HTTP_PATCH;
                            break;
                        }
                        if (nai_str5cmp(m, 'T', 'R', 'A', 'C', 'E')) {
                            r->method = NAI_HTTP_TRACE;
                            break;
                        }
                        break;

                    case 6:
                        if (nai_str6cmp(m, 'D', 'E', 'L', 'E', 'T', 'E')) {
                            r->method = NAI_HTTP_DELETE;
                            break;
                        }
                        if (nai_str6cmp(m, 'U', 'N', 'L', 'O', 'C', 'K')) {
                            r->method = NAI_HTTP_UNLOCK;
                            break;
                        }
                        break;

                    case 7:
                        if (nai_str7_cmp(m, 
                            'O', 'P', 'T', 'I', 'O', 'N', 'S', ' ')) {
                            r->method = NAI_HTTP_OPTIONS;
                        }
                        break;

                    case 8:
                        if (nai_str8cmp(m, 
                            'P', 'R', 'O', 'P', 'F', 'I', 'N', 'D')) {
                            r->method = NAI_HTTP_PROPFIND;
                        }
                        break;

                    case 9:
                        if (nai_str9cmp(m,
                            'P', 'R', 'O', 'P', 'P', 'A', 'T', 'C', 'H')) {
                            r->method = NAI_HTTP_PROPPATCH;
                        }
                        break;

                    default:
                        break;
                    };

                    cur ++;
                    state = stBeforeUri;
                    goto _loop;
                };

                if ((ch < 'A' || ch > 'Z') && ch != '-' && ch != '_') {
                    rc = NAI_HTTP_BAD_REQUEST;
                    goto _fail;
                };
            };
            goto _end;

        case stBeforeUri:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case ' ':
                    continue;

                case '/':
                    nai_str_setm(&r->unparsed_uri, cur, 0);
                    nai_str_setm(&r->parsed_uri.path, cur, 0);
                    state = stPath;
                    break;

                default:
                    if (!nai_isalpha(ch)) {
                        rc = NAI_HTTP_BAD_REQUEST;
                        goto _fail;
                    };
                    nai_str_setm(&r->unparsed_uri, cur, 0);
                    nai_str_setm(&r->parsed_uri.scheme, cur, 0);
                    state = stSchema;
                    break;
                };

                cur ++;
                goto _loop;
            };
            goto _end;

        case stSchema:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case ':':
                    nai_str_setl(&r->parsed_uri.scheme, 
                        cur - nai_str(&r->parsed_uri.scheme));
                    state = stSchemaSlash_0;
                    break;

                default:
                    if (delims[ch] & escapeScheme) {
                        rc = NAI_HTTP_BAD_REQUEST;
                        goto _fail;
                    };
                    continue;
                };

                cur ++;
                goto _loop;
            };
            goto _end;

        case stSchemaSlash_0:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;
            if (ch != '/') {
                rc = NAI_HTTP_BAD_REQUEST;
                goto _fail;
            };

            state = stSchemaSlash_1;
            /* fallthrough */

        case stSchemaSlash_1:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;
            if (ch != '/') {
                rc = NAI_HTTP_BAD_REQUEST;
                goto _fail;
            };

            nai_str_setm(&r->parsed_uri.host, cur, 0);
            state = stHost;
            /* fallthrough */

        case stHost:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case ' ':
                case '\t':
                    state = stBeforeHTTP;
                    break;
                case '\r':
                    r->version = NAI_HTTP_09;
                    state = stEndline;
                    break;
                case '\n':
                    r->version = NAI_HTTP_09;
                    state = stEnd;
                    break;
                case '/':
                    nai_str_setm(&r->parsed_uri.path, cur, 0);
                    state = stPath;
                    break;
                case '?':
                    r->parsed_uri.path_empty = 1;
                    nai_str_setm(&r->parsed_uri.query, cur+1, 0);
                    state = stQuery;
                    break;
                case '#':
                    r->parsed_uri.path_empty = 1;
                    nai_str_setm(&r->parsed_uri.fragment, cur+1, 0);
                    state = stFragment;
                    break;

                case ':':
                case '[':
                case ']':
                case '@':
                    r->host_complex = 1;
                    continue;

                default:
                    if (delims[ch] & escapeHost) {
                        rc = NAI_HTTP_BAD_REQUEST;
                        goto _fail;
                    };
                    continue;
                };

                switch (state) {
                case stBeforeHTTP:
                case stEndline:
                case stEnd:
                    r->parsed_uri.path_empty = 1;
                    nai_str_setl(&r->unparsed_uri, 
                        cur - nai_str(&r->unparsed_uri));
                    break;
                default:
                    break;
                };

                nai_str_setl(&r->parsed_uri.host, 
                    cur - nai_str(&r->parsed_uri.host));

                if (!r->host_complex) {
                    r->parsed_uri.hostname = r->parsed_uri.host;
                } else {
                    host = r->parsed_uri.host;
                    rc = nai_uri_parse(&r->parsed_uri, 
                        nai_str(&host), 
                        nai_str_len(&host), NAI_URI_STRICT|NAI_URI_HOSTONLY);
                    if (rc < 0) {
                        rc = nai_sult_from_errno();
                        goto _fail;
                    };
                };

                cur ++;
                goto _loop;
            };
            goto _end;

        case stPath:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case ' ':
                case '\t':
                    state = stBeforeHTTP;
                    break;
                case '\r':
                    r->version = NAI_HTTP_09;
                    state = stEndline;
                    break;
                case '\n':
                    r->version = NAI_HTTP_09;
                    state = stEnd;
                    break;
                case '?':
                    nai_str_setm(&r->parsed_uri.query, cur+1, 0);
                    state = stQuery;
                    break;
                case '#':
                    nai_str_setm(&r->parsed_uri.fragment, cur+1, 0);
                    state = stFragment;
                    break;
                case '%':
                    r->parsed_uri.path_escape = 1;
                    continue;
                case '/':
                    r->exten.ptr = 0;
                    r->parsed_uri.path_complex = 1;
                    break;
                case '.':
                    r->exten.ptr = cur + 1;
                    r->parsed_uri.path_complex = 1;
                    break;
                default:
                    if (delims[ch] & escapePath) {
                        rc = NAI_HTTP_BAD_REQUEST;
                        goto _fail;
                    };
                    continue;
                };

                nai_str_setl(&r->parsed_uri.path, 
                    cur - nai_str(&r->parsed_uri.path));

                if (nai_str(&r->exten)) {
                    nai_str_setl(&r->exten, cur - nai_str(&r->exten));
                };

                if (state != stQuery && state != stFragment) {
                    nai_str_setl(&r->unparsed_uri, 
                        cur - nai_str(&r->unparsed_uri));
                };

                cur ++;
                goto _loop;
            };
            goto _end;

        case stQuery:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case ' ':
                case '\t':
                    state = stBeforeHTTP;
                    break;
                case '\r':
                    r->version = NAI_HTTP_09;
                    state = stEndline;
                    break;
                case '\n':
                    r->version = NAI_HTTP_09;
                    state = stEnd;
                    break;
                case '#':
                    nai_str_setm(&r->parsed_uri.fragment, cur+1, 0);
                    state = stFragment;
                    break;
                case '%':
                    r->parsed_uri.query_escape = 1;
                    continue;
                default:
                    if (delims[ch] & escapeArgu) {
                        rc = NAI_HTTP_BAD_REQUEST;
                        goto _fail;
                    };
                    continue;
                };

                nai_str_setl(&r->parsed_uri.query, 
                    cur - nai_str(&r->parsed_uri.query));

                if (state != stFragment) {
                    nai_str_setl(&r->unparsed_uri, 
                        cur - nai_str(&r->unparsed_uri));
                };

                cur ++;
                goto _loop;
            };
            goto _end;

        case stFragment:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case ' ':
                case '\t':
                    state = stBeforeHTTP;
                    break;
                case '\r':
                    r->version = NAI_HTTP_09;
                    state = stEndline;
                    break;
                case '\n':
                    r->version = NAI_HTTP_09;
                    state = stEnd;
                    break;
                default:
                    if (delims[ch] & escapeArgu) {
                        rc = NAI_HTTP_BAD_REQUEST;
                        goto _fail;
                    };
                    continue;
                };

                nai_str_setl(&r->parsed_uri.fragment, 
                    cur - nai_str(&r->parsed_uri.fragment));

                nai_str_setl(&r->unparsed_uri, 
                    cur - nai_str(&r->unparsed_uri));

                cur ++;
                goto _loop;
            };
            goto _end;

        case stBeforeHTTP:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case ' ':
                case '\t':
                    continue;
                case '\r':
                    r->version = NAI_HTTP_09;
                    state = stEndline;
                    break;
                case '\n':
                    r->version = NAI_HTTP_09;
                    state = stEnd;
                    break;
                case 'H':
                    state = stHTTP_1;
                    break;

                default:
                    /* invalid http version of request line */
                    rc = NAI_HTTP_BAD_REQUEST;
                    goto _fail;
                };

                cur ++;
                goto _loop;
            };
            goto _end;

        case stHTTP_1:
            if (cur >= cend) {
                goto _end;
            };

            ch = *cur ++;
            if (ch != 'T') {
                /* invalid http version of request line */
                rc = NAI_HTTP_BAD_REQUEST;
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
                /* invalid http version of request line */
                rc = NAI_HTTP_BAD_REQUEST;
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
                /* invalid http version of request line */
                rc = NAI_HTTP_BAD_REQUEST;
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
                rc = NAI_HTTP_BAD_REQUEST;
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
            if (ch < '1' || ch > '9') {
                /* invalid http version of request line */
                rc = NAI_HTTP_BAD_REQUEST;
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
                    /* invalid http version of request line */
                    rc = NAI_HTTP_BAD_REQUEST;
                    goto _fail;
                };

                r->major = r->major * 10 + ch - '0';
                if (r->major > 1) {
                    rc = NAI_HTTP_BAD_REQUEST;
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
                /* invalid http version of request line */
                rc = NAI_HTTP_BAD_REQUEST;
                goto _fail;
            };


            r->minor = ch - '0';
            state = stMinor_1;
            /* fallthrough */

        case stMinor_1:
            for ( ; cur < cend; ) {
                ch = *cur ++;
                switch (ch) {
                case '\r':
                    state = stEndline;
                    break;
                case '\n':
                    state = stEnd;
                    break;
                default:
                    if (ch < '0' || ch > '9') {
                        /* invalid http version of request line */
                        rc = NAI_HTTP_BAD_REQUEST;
                        goto _fail;
                    };

                    r->minor = r->minor * 10 + ch - '0';
                    continue;
                };

                goto _loop;
            };
            goto _end;

        case stEndline:
            for ( ; cur < cend; ) {
                ch = *cur ++;
                switch (ch) {
                case '\n':
                    nai_str_setm(&r->request_line,
                        nai_str(&s->cur), 
                        cur - 2 - nai_str(&s->cur));

                    nai_str_setn(&s->cur);
                    state = stEnd;
                    goto _end;

                default:
                    rc = NAI_HTTP_BAD_REQUEST;
                    goto _fail;
                };
            };
            goto _end;

        case stEnd:
            nai_str_setm(&r->request_line,
                nai_str(&s->cur), 
                cur - 1 - nai_str(&s->cur));

            nai_str_setn(&s->cur);
            goto _end;

        default:
            assert(0);
            break;
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


nai_sult_t nai_https_parse_headers(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_buf_t* b;
    nai_mem_t key, value;
    nai_https_proto_v1_t* p;
    nai_https_parse_state_t* s;
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


    p = (nai_https_proto_v1_t*)r->proto;
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

        case stKey:
            for ( ; cur < cend; cur ++) {
                ch = *cur;
                switch (ch) {
                case '\0':
                    /** end string */
                    rc = NAI_HTTP_BAD_REQUEST;
                    goto _fail;
                case '\r':
                case '\n':
                    nai_str_setl(&s->cur, cur - nai_str(&s->cur));
                    if (nai_str_len(&s->cur) <= 0) {
                        /* end of header */
                        if (ch == '\r') {
                            state = stEndheader;
                            goto _loop;
                        } else {
                            cur ++;
                            state = stEnd;
                            goto _end;
                        };
                    };

                    /* end of line without value */
                    rc = NAI_HTTP_BAD_REQUEST;
                    goto _fail;
                case ':':
                case ' ':
                case '\t':
                    nai_str_setl(&s->cur, cur - nai_str(&s->cur));
                    if (nai_str_len(&s->cur) <= 0) {
                        rc = NAI_HTTP_BAD_REQUEST;
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
                        rc = NAI_HTTP_BAD_REQUEST;
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
                    rc = NAI_HTTP_BAD_REQUEST;
                    goto _fail;
                case '\r':
                case '\n':
                    /** end of line without value */
                    rc = NAI_HTTP_BAD_REQUEST;
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
                    rc = NAI_HTTP_BAD_REQUEST;
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
                    rc = NAI_HTTP_BAD_REQUEST;
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
                    rc = nai_https_headers_in_add(r, 
                        (nai_str_t*)&key, (nai_str_t*)&value, 0);
                    if (rc < 0) {
                        nai_log_error(NAI_LOG_HTTPS, 
                            nai_sult_to_errno(rc), 
                            "add request header failed");
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
            assert(state != stEnd);
            goto _end;

        default:
            rc = NAI_HTTP_INTERNAL_SERVER_ERROR;
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


nai_sult_t nai_https_parse_chunked(nai_https_request_t* r, nai_buf_t* body)
{
    nai_sult_t rc;
    nai_buf_t* b;
    nai_https_proto_v1_t* p;
    nai_https_parse_state_t* s;
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

    p = (nai_https_proto_v1_t*)r->proto;
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
                    rc = NAI_HTTP_BAD_REQUEST;
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
                    rc = NAI_HTTP_BAD_REQUEST;
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
                rc = NAI_HTTP_BAD_REQUEST;
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


