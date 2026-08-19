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
/// @file       nai_https_special_response.c
/// @brief      
/// @details
/// @date       2021-02-24
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai_https_core.h"


#define nai_https_page_body(name)                       \
    "<html>" CRLF                                       \
    "<head><title>" name "</title></head>" CRLF         \
    "<body>" CRLF                                       \
    "<center><h1>" name "</h1></center>" CRLF           \


#define nai_https_page_body_400(name)                   \
    "<html>" CRLF                                       \
    "<head><title>" name "</title></head>" CRLF         \
    "<body>" CRLF                                       \
    "<center><h1>400 Bad Request</h1></center>" CRLF    \
    "<center>" name "</center>" CRLF                    \


static char nai_https_page_tail[] = 
    "<hr><center>nhttp</center>" CRLF
    "</body>" CRLF
    "</html>" CRLF;

static char nai_https_page_301[] = 
    nai_https_page_body("301 Moved Permanently");

static char nai_https_page_302[] = 
    nai_https_page_body("302 Found");

static char nai_https_page_303[] = 
    nai_https_page_body("303 See Other");

static char nai_https_page_307[] = 
    nai_https_page_body("307 Temporary Redirect");

static char nai_https_page_308[] = 
    nai_https_page_body("308 Permanent Redirect");

static char nai_https_page_400[] = 
    nai_https_page_body("400 Bad Request");

static char nai_https_page_401[] = 
    nai_https_page_body("401 Authorization Required");

static char nai_https_page_402[] = 
    nai_https_page_body("402 Payment Required");

static char nai_https_page_403[] = 
    nai_https_page_body("403 Forbidden");

static char nai_https_page_404[] = 
    nai_https_page_body("404 Not Found");

static char nai_https_page_405[] = 
    nai_https_page_body("405 Not Allowed");

static char nai_https_page_406[] = 
    nai_https_page_body("406 Not Acceptable");

static char nai_https_page_408[] = 
    nai_https_page_body("408 Request Time-out");

static char nai_https_page_409[] = 
    nai_https_page_body("409 Conflict");

static char nai_https_page_410[] = 
    nai_https_page_body("410 Gone");

static char nai_https_page_411[] = 
    nai_https_page_body("411 Length Required");

static char nai_https_page_412[] = 
    nai_https_page_body("412 Precondition Failed");

static char nai_https_page_413[] = 
    nai_https_page_body("413 Request Entity Too Large");

static char nai_https_page_414[] = 
    nai_https_page_body("414 Request-URI Too Large");

static char nai_https_page_415[] = 
    nai_https_page_body("415 Unsupported Media Type");

static char nai_https_page_416[] = 
    nai_https_page_body("416 Requested Range Not Satisfiable");

static char nai_https_page_421[] = 
    nai_https_page_body("421 Misdirected Request");

static char nai_https_page_429[] = 
    nai_https_page_body("429 Too Many Requests");

static char nai_https_page_494[] = 
    nai_https_page_body_400("400 Request Header Or Cookie Too Large");

static char nai_https_page_495[] = 
    nai_https_page_body_400("400 The SSL certificate error");

static char nai_https_page_496[] = 
    nai_https_page_body_400("400 No required SSL certificate was sent");

static char nai_https_page_497[] = 
    nai_https_page_body_400("400 The plain HTTP request was sent to HTTPS port");

static char nai_https_page_500[] = 
    nai_https_page_body("500 Internal Server Error");

static char nai_https_page_501[] = 
    nai_https_page_body("501 Not Implemented");

static char nai_https_page_502[] = 
    nai_https_page_body("502 Bad Gateway");

static char nai_https_page_503[] = 
    nai_https_page_body("503 Service Temporarily Unavailable");

static char nai_https_page_504[] = 
    nai_https_page_body("504 Gateway Time-out");

static char nai_https_page_505[] = 
    nai_https_page_body("505 HTTP Version Not Supported");

static char nai_https_page_507[] = 
    nai_https_page_body("507 Insufficient Storage");



static nai_str_t nai_https_pages[] = {

#define NAI_HTTPS_2XX_LAST  202

    nai_strnull(),                      /* 201, 204 */

#define NAI_HTTPS_3XX_OFF   (NAI_HTTPS_2XX_LAST - 201)
#define NAI_HTTPS_3XX_LAST  309

    /* nai_strnull(), */                /* 300 */
    nai_strconst(nai_https_page_301), 
    nai_strconst(nai_https_page_302), 
    nai_strconst(nai_https_page_303), 
    nai_strnull(),                      /* 304 */
    nai_strnull(),                      /* 305 */
    nai_strnull(),                      /* 306 */
    nai_strconst(nai_https_page_307), 
    nai_strconst(nai_https_page_308), 

#define NAI_HTTPS_4XX_OFF   (NAI_HTTPS_3XX_LAST - 301 + NAI_HTTPS_3XX_OFF)
#define NAI_HTTPS_4XX_LAST  430

    nai_strconst(nai_https_page_400), 
    nai_strconst(nai_https_page_401), 
    nai_strconst(nai_https_page_402), 
    nai_strconst(nai_https_page_403), 
    nai_strconst(nai_https_page_404), 
    nai_strconst(nai_https_page_405), 
    nai_strconst(nai_https_page_406), 
    nai_strnull(),                      /* 407 */
    nai_strconst(nai_https_page_408), 
    nai_strconst(nai_https_page_409), 
    nai_strconst(nai_https_page_410), 
    nai_strconst(nai_https_page_411), 
    nai_strconst(nai_https_page_412), 
    nai_strconst(nai_https_page_413), 
    nai_strconst(nai_https_page_414), 
    nai_strconst(nai_https_page_415), 
    nai_strconst(nai_https_page_416), 
    nai_strnull(),                      /* 417 */
    nai_strnull(),                      /* 418 */
    nai_strnull(),                      /* 419 */
    nai_strnull(),                      /* 420 */
    nai_strconst(nai_https_page_421),
    nai_strnull(),                      /* 422 */
    nai_strnull(),                      /* 423 */
    nai_strnull(),                      /* 424 */
    nai_strnull(),                      /* 425 */
    nai_strnull(),                      /* 426 */
    nai_strnull(),                      /* 427 */
    nai_strnull(),                      /* 428 */
    nai_strconst(nai_https_page_429),

#define NAI_HTTPS_5XX_OFF   (NAI_HTTPS_4XX_LAST - 400 + NAI_HTTPS_4XX_OFF)
#define NAI_HTTPS_5XX_LAST  508

    nai_strconst(nai_https_page_494),   /* 494, request header too large */
    nai_strconst(nai_https_page_495),   /* 495, https certificate error */
    nai_strconst(nai_https_page_496),   /* 496, https no certificate */
    nai_strconst(nai_https_page_497),   /* 497, http to https */
    nai_strconst(nai_https_page_404),   /* 498, canceled */
    nai_strnull(),                      /* 499, client has closed connection */

    nai_strconst(nai_https_page_500), 
    nai_strconst(nai_https_page_501), 
    nai_strconst(nai_https_page_502), 
    nai_strconst(nai_https_page_503), 
    nai_strconst(nai_https_page_504), 
    nai_strconst(nai_https_page_505), 
    nai_strnull(),                      /* 506 */
    nai_strconst(nai_https_page_507), 
};



static 
nai_sult_t nai_https_send_special_response(
    nai_https_request_t* r, nai_sult_t sult)
{
    nai_int_t index;
    nai_sult_t rc;
    nai_buf_t* b;
    nai_str_t body;
    nai_str_t tail;
    nai_str_t type;


    if (sult == NAI_HTTP_CREATED) {
        /* 201 */
        index = 0;

    } else if (sult == NAI_HTTP_NO_CONTENT) {
        /* 204 */
        index = 0;

    } else if (sult >= NAI_HTTP_MOVED_PERMANENTLY && 
        sult < NAI_HTTPS_3XX_LAST) {
        /* 3XX */
        index = sult - NAI_HTTP_MOVED_PERMANENTLY + NAI_HTTPS_3XX_OFF;

    } else if (sult >= NAI_HTTP_BAD_REQUEST && 
        sult < NAI_HTTPS_4XX_LAST) {
        /* 4XX */
        index = sult - NAI_HTTP_BAD_REQUEST + NAI_HTTPS_4XX_OFF;

    } else if (sult >= NAI_HTTP_HEADER_TOO_LARGE && 
        sult < NAI_HTTPS_5XX_LAST) {
        /* 49X, 5XX */
        index = sult - NAI_HTTP_HEADER_TOO_LARGE + NAI_HTTPS_5XX_OFF;
        switch (sult) {
        case NAI_HTTP_TO_HTTPS:
        case NAI_HTTP_CERT_ERROR:
        case NAI_HTTP_NO_CERT:
        case NAI_HTTP_HEADER_TOO_LARGE:
            sult = NAI_HTTP_BAD_REQUEST;
            break;
        default:
            break;
        };

    } else {
        /* unknown code, zero body */
        index = 0;
    };


    body = nai_https_pages[index];
    if (nai_str_len(&body) <= 0) {
        r->headers_out.chunked = 0;
        r->headers_out.content_length_n = 0;
    } else {
        nai_str_setc(&tail, nai_https_page_tail);

        r->headers_out.chunked = 0;
        r->headers_out.content_length_n = 
            nai_str_len(&body) + nai_str_len(&tail);


        b = nai_buf_from_rmemory(r->bufpool, 
            nai_str(&body), nai_str_len(&body), 0);
        if (b == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "alloc buffer failed when send specical response");
            goto _end;
        };

        nai_buflist_insert_tail(&r->content, b);

        b = nai_buf_from_rmemory(r->bufpool, 
            nai_str(&tail), nai_str_len(&tail), 0);
        if (b == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "alloc buffer failed when send specical response");
            goto _end;
        };

        if (r == nai_https_request_main(r)) {
            b->eos = 1;
        };

        nai_buflist_insert_tail(&r->content, b);
    };

    r->test_expect = 0;

    r->status = sult;
    nai_str_setc(&type, "text/html");
    nai_https_headers_out_set(r, content_type, &type, 0);


    rc = nai_https_response(r);
    if (rc < 0) {
        goto _end;
    };

    if (r->header_only == 0) {
        rc = (nai_sult_t)nai_https_write(r, &r->content, -1);
        if (rc < 0) {
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_https_request_special_response(
    nai_https_request_t* r, nai_sult_t sult)
{
    nai_sult_t rc;


    r->err_status = sult;

    if (r->keepalive) {
        switch (sult) {
        case NAI_HTTP_BAD_REQUEST:
        case NAI_HTTP_REQUEST_ENTITY_TOO_LARGE:
        case NAI_HTTP_REQUEST_URI_TOO_LARGE:
        case NAI_HTTP_TO_HTTPS:
        case NAI_HTTP_CERT_ERROR:
        case NAI_HTTP_NO_CERT:
        case NAI_HTTP_INTERNAL_SERVER_ERROR:
        case NAI_HTTP_NOT_IMPLEMENTED:
            r->keepalive = 0;
            break;
        default:
            break;
        };
    };
    if (r->lingering_close) {
        switch (sult) {
        case NAI_HTTP_BAD_REQUEST:
        case NAI_HTTP_TO_HTTPS:
        case NAI_HTTP_CERT_ERROR:
        case NAI_HTTP_NO_CERT:
            r->lingering_close = 0;
            break;
        default:
            break;
        };
    };

    rc = nai_https_send_special_response(r, sult);

    return rc;
};


