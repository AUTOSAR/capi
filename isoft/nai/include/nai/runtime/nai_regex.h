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
/// @file       nai_regex.h
/// @brief      the regular expression
/// @details
/// @date       2021-01-15
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation is perl compatible regular expressions, 
 *          the pcre library is used internally.
 *
 * @details the code example is as follows:
 *
 * @par     compile regular expression
 * @code
 *          nai_int_t r;
 *          const char* pattern;
 *          nai_regex_t re;
 *          nai_regex_errinfo_t e;
 *
 *          r = nai_regex_compile(&re, pattern, NAI_REGEX_JIT, &e, 0);
 *          if (r < 0) {
 *              printf("compile error %s, at %.10s\n", 
 *                  e.str, pattern + e.offset);
 *              goto _fail;
 *          };
 *
 * @endcode
 *
 * @par     perform regular matching
 * @code
 *          nai_int_t r;
 *          nai_int_t n;
 *          nai_int_t len;
 *          nai_int_t* m;
 *          const char* match;
 *          nai_regex_t* re;
 *
 *          len = nai_regex_match_size(re.sub);
 *          m = nai_malloc(sizeof(nai_int_t) * len);
 *          if (m == 0) {
 *              goto _fail;
 *          };
 *
 *          r = nai_regex_exec(re, match, -1, m, len);
 *          if (r < NAI_REGEX_NO_MATCH) {
 *              // an error occurred, see #nai_errno
 *              goto _fail;
 *          };
 *          if (r == NAI_REGEX_NO_MATCH) {
 *              // no match
 *              goto _done;
 *          };
 *
 *          for (n = 0; n < r; n ++) {
 *              printf("match %d: %.*s\n", 
 *                  n, m[n*2+1] - m[n*2], match + m[n*2]);
 *          };
 *
 * @endcode
 *
 * @par     enumerate named variable in the named table
 * @code
 *          nai_int_t r;
 *          nai_int_t n;
 *          uint8_t* p;
 *          nai_regex_t* re;
 *          nai_regex_named_t named;
 *
 *          r = nai_regex_named(re, &named);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          for (n = 0; n < named.count; n ++) {
 *              p = nai_regex_named_at(&named, n);
 *              printf("name: %s, id: %d\n", 
 *                  nai_regex_named_str(p), nai_regex_named_id(p));
 *          };
 * @endcode
 */

#ifndef _REGEX_H_NAI
#define _REGEX_H_NAI

#pragma once

#include "nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // Regular expressions

#if (NAI_HAVE_PCRE)
    #define NAI_HAVE_REGEX 1
#endif

#if (NAI_HAVE_REGEX)

    /**
 * @anchor  NAI_REGEX_FLAG
 * @name    NAI_REGEX_FLAG      the flag of the regex
 * @{
 */
    #define NAI_REGEX_CASELESS       0x01 /**< character case sensitivity */
    #define NAI_REGEX_NEWLINE        0x02 /**< to enable multiline mode */
    #define NAI_REGEX_DOTALL         0x04 /**< dot matches newline characters */
    #define NAI_REGEX_DOLLAR_ENDONLY 0x08 /**< $ match at the end of string */
    #define NAI_REGEX_JIT            0x10 /**< to enable jit compile */
    /** @} */

    /**
 * no matching return value
 */
    #define NAI_REGEX_NO_MATCH -1

    #ifndef _NAI_TYPEDEF_REGEX_NAMED_T
        #define _NAI_TYPEDEF_REGEX_NAMED_T
    typedef struct nai_regex_named_s nai_regex_named_t;
    #endif

    /**
 * the structure of the table of the named variables
 */
    struct nai_regex_named_s
    {
        nai_int_t count;   /**< the count of the named variables */
        nai_int_t eltsize; /**< the size of the named variable */
        uint8_t* data;     /**< pointer to the table */
    };

    /**
 * get the named variable which specified by the index
 * @param   ns      pointer to the named table
 * @param   n       the index of the named variable
 * @return  the address of the named variable
 */
    #define nai_regex_named_at(ns, n) ((ns)->data + (ns)->eltsize * (n))

    /**
 * get the id of the named variable
 * @param   p       pointer to the named variable
 * @return  the id of the named variable
 */
    #define nai_regex_named_id(p) (((p)[0] << 8) + (p)[1])

    /**
 * get the name of the named variable
 * @param   p       pointer to the named variable
 * @return  the address of the name string
 */
    #define nai_regex_named_str(p) ((const char*)(p) + 2)

    #ifndef _NAI_TYPEDEF_REGEX_ERRINFO_T
        #define _NAI_TYPEDEF_REGEX_ERRINFO_T
    typedef struct nai_regex_errinfo_s nai_regex_errinfo_t;
    #endif
    #ifndef _NAI_TYPEDEF_REGEX_T
        #define _NAI_TYPEDEF_REGEX_T
    typedef struct nai_regex_s nai_regex_t;
    #endif

    /**
 * the structure of the error information
 */
    struct nai_regex_errinfo_s
    {
        const char* str;  /**< pointer to a textual error message */
        nai_int_t offset; /**< offset of the character 
                                    where the error was discovered */
    };

    /**
 * the structure of the regex
 */
    struct nai_regex_s
    {
        nai_pool_t* pool; /**< pointer to the memory pool */
        void* reg;        /**< pointer to the handle of the regex */
        void* extra;      /**< pointer to the memory of jit compile */
        nai_int_t sub;    /**< the count of the submatch */
    };

    /**
 * initial the regex
 * @param   re      pointer to the regex
 * @return  void
 */
    #define nai_regex_init(re)                                                                                         \
        {                                                                                                              \
            (re)->pool  = 0;                                                                                           \
            (re)->reg   = 0;                                                                                           \
            (re)->extra = 0;                                                                                           \
            (re)->sub   = 0;                                                                                           \
        }

    /**
 * get the count of the integer buffer to receive match result
 * @param   n       the count of the submatch
 * @return  the count of the integer buffer
 */
    #define nai_regex_match_size(n) ((n)*3)

    /**
 * compile regular expression
 * @param   re      pointer to the regex
 * @param   pattern pointer to the pattern string
 * @param   flags   the flags of compile, see @ref NAI_REGEX_FLAG
 * @param   e       pointer to the error information
 * @param   p       pointer to the memory pool, can be null
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_regex_compile(
        nai_regex_t* re, const char* pattern, nai_int_t flags, nai_regex_errinfo_t* e, nai_pool_t* p);

    /**
 * perform regular matching
 * @param   re      pointer to the regex
 * @param   string  pointer to the match string
 * @param   len     the length of the string, -1 means with null-terminated
 * @param   mbuf    pointer to the match result buffer
 * @param   msize   the length of the match result
 * @retval  >=0     on success
 * @retval  =NAI_REGEX_NO_MATCH no match
 * @retval  <NAI_REGEX_NO_MATCH an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_regex_exec(nai_regex_t* re, const char* string, size_t len, nai_int_t* mbuf, nai_int_t msize);

    /**
 * get the named table of the regex
 * @param   re      pointer to the regex
 * @param   named   pointer to the named table
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_regex_get_named(nai_regex_t* re, nai_regex_named_t* named);

    /**
 * close the regex
 * @param   re      pointer to the regex
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_regex_close(nai_regex_t* re);

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
