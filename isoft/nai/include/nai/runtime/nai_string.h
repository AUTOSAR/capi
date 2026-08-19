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
/// @file       nai_string.h
/// @brief      the functions of string and string view
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details the function names starting with nai_str_* are 
 *          used for string views, and the others are mainly 
 *          redefinitions of c-style string functions.
 */

#ifndef _STRING_H_NAI
#define _STRING_H_NAI

#pragma once

#include "nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <ctype.h>
#include <string.h>

#ifndef _NAI_TYPEDEF_MEM_T
    #define _NAI_TYPEDEF_MEM_T
    typedef struct nai_mem_s nai_mem_t;
#endif
#ifndef _NAI_TYPEDEF_STR_T
    #define _NAI_TYPEDEF_STR_T
    typedef struct nai_str_s nai_str_t;
#endif
#ifndef _NAI_TYPEDEF_RESERVED_T
    #define _NAI_TYPEDEF_RESERVED_T
    typedef struct nai_reserved_s nai_reserved_t;
#endif

    /**
 * the structure of a string view without null-terminated
 */
    struct nai_mem_s
    {
        char* ptr;     /**< pointer to the stirng view */
        uint32_t len;  /**< the length of the string */
        uint32_t hash; /**< the hash value of the string */
    };

    /**
 * the structure of a string view with null-terminated
 */
    struct nai_str_s
    {
        char* ptr;     /**< pointer to the stirng view */
        uint32_t len;  /**< the length of the string */
        uint32_t hash; /**< the hash value of the string */
    };

    /**
 * reserved pre-string and post-string lengths
 */
    struct nai_reserved_s
    {
        size_t pre;  /**< reserved bytes of pre-string */
        size_t post; /**< reserved bytes of post-string */
    };

/**
 * convert uppercase characters to lowercase
 * @param   c       the uppercase character
 * @return  the value of the lowercase character
 */
#define nai_A2a(c) ((c) + ('a' - 'A'))

/**
 * convert lowercase characters to uppercase
 * @param   c       the lowercase character
 * @return  the value of the uppercase character
 */
#define nai_a2A(c) ((c) + ('A' - 'a'))

/**
 * get the uppercase of a character
 * @param   c       the value of a character
 * @return  the value of the lowercase character
 */
#define nai_tolower(c) (((c) >= 'A' && (c) <= 'Z') ? nai_A2a(c) : (c))

/**
 * get the lowercase of a character
 * @param   c       the value of a character
 * @return  the value of the uppercase character
 */
#define nai_toupper(c) (((c) >= 'a' && (c) <= 'z') ? nai_a2A(c) : (c))

/**
 * test whether the character is a letter
 * @param   c       the value of a charater
 * @return  if it is a letter, return 1, otherwise return 0
 */
#define nai_isalpha(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

/**
 * test whether the character is a letter or number
 * @param   c       the value of a charater
 * @return  if it is a letter or number, return 1, otherwise return 0
 */
#define nai_isalnum(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') || ((c) >= '0' && (c) <= '9'))

/**
 * test whether the character is a space
 * @param   c       the value of a charater
 * @return  if it is a space, return 1, otherwise return 0
 */
#define nai_isspace(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')

/**
 * test whether the character is a uppercase character
 * @param   c       the value of a charater
 * @return  if it is a uppercase character, return 1, otherwise return 0
 */
#define nai_isupper(c) ((c) >= 'A' && (c) <= 'Z')

/**
 * test whether the character is a lowercase character
 * @param   c       the value of a charater
 * @return  if it is a lowercase character, return 1, otherwise return 0
 */
#define nai_islower(c) ((c) >= 'a' && (c) <= 'z')

/**
 * test whether the character is a decimal character
 * @param   c       the value of a charater
 * @return  if it is a decimal character, return 1, otherwise return 0
 */
#define nai_isdigit(c) ((c) >= '0' && (c) <= '9')

/**
 * test whether the character is a hexadecimal character
 * @param   c       the value of a charater
 * @return  if it is a hexadecimal character, return 1, otherwise return 0
 */
#define nai_isxdigit(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

/**
 * a constant string view construction
 * @param   c       the constant string expression
 * @return  the construct expression
 */
#define nai_strconst(c)                                                                                                \
    {                                                                                                                  \
        (c), sizeof(c) - 1, 0                                                                                          \
    }

/**
 * a null string view construction
 * @return  the construct expression
 */
#define nai_strnull()                                                                                                  \
    {                                                                                                                  \
        0, 0, 0                                                                                                        \
    }

/**
 * set a string without null-terminated to the string view
 * @param   s       pointer to the string view
 * @param   p       pointer to the string
 * @param   l       the length of the string
 * @return  void
 */
#define nai_str_setm(s, p, l)                                                                                          \
    {                                                                                                                  \
        (s)->ptr  = (char*)(p);                                                                                        \
        (s)->len  = (uint32_t)(l);                                                                                     \
        (s)->hash = 0;                                                                                                 \
    }

/**
 * set a string with null-terminated to the string view
 * @param   s       pointer to the string view
 * @param   p       pointer to the string
 * @return  void
 */
#define nai_str_sets(s, p) nai_str_setm(s, p, strlen(p))

/**
 * set a null string to the string view
 * @param   s       pointer to the string view
 * @return  void
 */
#define nai_str_setn(s) nai_str_setm(s, 0, 0)

/**
 * set a constant string to the string view
 * @param   s       pointer to the string view
 * @param   c       the constant string expression
 * @return  void
 */
#define nai_str_setc(s, c) nai_str_setm(s, c, sizeof(c) - 1)

/**
 * modify the pointer of the string view
 * @param   s       pointer to the string view
 * @param   p       pointer to the string
 * @return  void
 */
#define nai_str_setp(s, p) (s)->ptr = (char*)(p);

/**
 * modify the length of the string view
 * @param   s       pointer to the string view
 * @param   l       the length of the string
 * @return  void
 */
#define nai_str_setl(s, l) (s)->len = (uint32_t)(l);

/**
 * get the pointer of the stirng view
 * @param   s       pointer to the string view
 * @return  the address of the string view
 */
#define nai_str(s) (&*(s)->ptr)

/**
 * get the length of the stirng view
 * @param   s       pointer to the string view
 * @return  the length of the string view
 */
#define nai_str_len(s) ((uint32_t)(s)->len)

/**
 * get the character in the string view
 * @param   s       pointer to the string view
 * @param   n       the index of the character in the stirng view
 * @return  the value of the character
 */
#define nai_str_at(s, n) ((s)->ptr[(n)])

/**
 * compare two stirng views
 * @param   s1      pointer to the stirng view s1
 * @param   s2      pointer to the string view s2
 * @retval  >0      the s1 is greater than the s2
 * @retval  =0      the s1 is equal with the s2
 * @retval  <0      the s1 is less than the s2
 */
#define nai_str_cmp(s1, s2) nai_strn2cmp(nai_str(s1), nai_str_len(s1), nai_str(s2), nai_str_len(s2))

/**
 * compare two stirng views case-insensitively
 * @param   s1      pointer to the stirng view s1
 * @param   s2      pointer to the string view s2
 * @retval  >0      the s1 is greater than the s2
 * @retval  =0      the s1 is equal with the s2
 * @retval  <0      the s1 is less than the s2
 */
#define nai_str_casecmp(s1, s2) nai_strn2casecmp(nai_str(s1), nai_str_len(s1), nai_str(s2), nai_str_len(s2))

/**
 * test whether two string views are equal
 * @param   s1      pointer to the stirng view s1
 * @param   s2      pointer to the string view s2
 * @return  equal return 1, otherwise return 0
 */
#define nai_str_eq(s1, s2)                                                                                             \
    (nai_str_len(s1) == nai_str_len(s2) && memcmp(nai_str(s1), nai_str(s2), (nai_int_t)nai_str_len(s1)) == 0)

/**
 * test whether two string views are equal case-insensitively
 * @param   s1      pointer to the stirng view s1
 * @param   s2      pointer to the string view s2
 * @return  equal return 1, otherwise return 0
 */
#define nai_str_caseeq(s1, s2)                                                                                         \
    (nai_str_len(s1) == nai_str_len(s2) && nai_strncasecmp(nai_str(s1), nai_str(s2), (nai_int_t)nai_str_len(s1)) == 0)

/**
 * calculate the hash of the string and save it to the string view
 * @param   s       pointer to the stirng view
 * @param   c       whether to proceed in case-insensitive mode
 * @return  void
 */
#define nai_str_hash(s, c) ((s)->hash = nai_strnhash((s)->ptr, (s)->len, (c)))

/**
 * test with the hash whether two string views are equal
 * @param   s1      pointer to the stirng view s1
 * @param   s2      pointer to the string view s2
 * @return  equal return 1, otherwise return 0
 */
#define nai_str_hasheq(s1, s2) ((s1)->hash == (s2)->hash && nai_str_eq(s1, s2))

/**
 * test with the hash whether two string views are equal case-insensitively
 * @param   s1      pointer to the stirng view s1
 * @param   s2      pointer to the string view s2
 * @return  equal return 1, otherwise return 0
 */
#define nai_str_hashcaseeq(s1, s2) ((s1)->hash == (s2)->hash && nai_str_caseeq(s1, s2))

    /**
 * dupilcate a string to the string view
 * @param   d       pointer to the stirng view
 * @param   s       pointer to the string
 * @param   len     the length of the string, can be -1
 * @param   p       pointer to the memory pool, can be null
 * @retval  >=0     the number of characters are dupilcated
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_str_dup(nai_str_t* d, const char* s, size_t len, nai_pool_t* p);

    /**
 * dupilcate a string to the string view and reserve space
 * @param   d       pointer to the stirng view
 * @param   s       pointer to the string
 * @param   len     the length of the string, can be -1
 * @param   rev     pointer to the reserved information
 * @param   p       pointer to the memory pool, can be null
 * @retval  >=0     the number of characters are dupilcated
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_str_dup_r(nai_str_t* d, const char* s, size_t len, const nai_reserved_t* rev, nai_pool_t* p);

/**
 * redefinition of strlen
 */
#define nai_strlen strlen

/**
 * redefinition of strchr
 */
#define nai_strchr strchr

/**
 * search a character in the stirng without null-terminated
 * @param   s       pointer to the stirng
 * @param   l       the length of the string
 * @param   c       the value of the character to search
 * @return  the address that first contained the character on success, 
 *          null is returned on fails
 */
#define nai_strnchr(s, l, c) memchr((s), (c), (l))

/**
 * redefinition of strstr
 */
#define nai_strstr strstr

/**
 * redefinition of strnstr
 */
#define nai_strnstr strnstr

/**
 * redefinition of strcmp
 */
#define nai_strcmp strcmp

/**
 * redefinition of strncmp
 */
#define nai_strncmp strncmp

/**
 * redefinition of strcpy
 */
#define nai_strcpy strcpy

/**
 * redefinition of strncpy
 */
#define nai_strncpy strncpy

    /**
 * calculate the length of the string in the specified buffer
 * @param   s       pointer to the string buffer
 * @param   size    the length of the string buffer
 * @return  the number of character without null-terminated
 */
    NAI_EXTERN
    size_t nai_strnlen(const char* s, size_t size);

    /**
 * compare two string buffers
 * @param   s0      pointer to the string s0
 * @param   n0      the length of the string s0
 * @param   s1      pointer to the string s1
 * @param   n1      the length of the string s1
 * @retval  >0      the s0 is greater than the s1
 * @retval  =0      the s0 is equal with the s1
 * @retval  <0      the s0 is less than the s1
 */
    NAI_EXTERN
    nai_int_t nai_strn2cmp(const char* s0, size_t n0, const char* s1, size_t n1);

    /**
 * compare two strings case-insensitively
 * @param   s0      pointer to the string s0
 * @param   s1      pointer to the string s1
 * @retval  >0      the s0 is greater than the s1
 * @retval  =0      the s0 is equal with the s1
 * @retval  <0      the s0 is less than the s1
 */
    NAI_EXTERN
    nai_int_t nai_strcasecmp(const char* s0, const char* s1);

    /**
 * compare two strings in the specified length case-insensitively
 * @param   s0      pointer to the string s0
 * @param   s1      pointer to the string s1
 * @param   len     the specified length to compare
 * @retval  >0      the s0 is greater than the s1
 * @retval  =0      the s0 is equal with the s1
 * @retval  <0      the s0 is less than the s1
 */
    NAI_EXTERN
    nai_int_t nai_strncasecmp(const char* s0, const char* s1, size_t len);

    /**
 * compare two string buffers case-insensitively
 * @param   s0      pointer to the string s0
 * @param   n0      the length of the string s0
 * @param   s1      pointer to the string s1
 * @param   n1      the length of the string s1
 * @retval  >0      the s0 is greater than the s1
 * @retval  =0      the s0 is equal with the s1
 * @retval  <0      the s0 is less than the s1
*/
    NAI_EXTERN
    nai_int_t nai_strn2casecmp(const char* s0, size_t n0, const char* s1, size_t n1);

    /**
 * calculate the hash of the string
 * @param   s       pointer to the stirng
 * @param   cases   whether to proceed in case-insensitive mode
 * @return  the value of the hash
 */
    NAI_EXTERN
    uint32_t nai_strhash(const char* s, nai_int_t cases);

    /**
 * calculate the hash of the string buffer
 * @param   s       pointer to the stirng
 * @param   len     the length of the string
 * @param   cases   whether to proceed in case-insensitive mode
 * @return  the value of the hash
 */
    NAI_EXTERN
    uint32_t nai_strnhash(const char* s, size_t len, nai_int_t cases);

/**
 * redefinition of nai_atoi/l
 */
#if NAI_SIZEOF_SIZE_T == 4
    #define nai_atosz nai_atoi
#else
    #define nai_atosz nai_atol
#endif

/**
 * redefinition of nai_atoi/l
 */
#if NAI_SIZEOF_OFF_T == 4
    #define nai_atoof nai_atoi
#else
    #define nai_atoof nai_atol
#endif

#if (__darwin__) || (__sylixos__)

    #undef nai_atoof
    #undef nai_atosz

    /**
 * convert string to type 'off_t'
 * @param   pv      pointer to the integer of off_t
 * @param   s       pointer to the stirng
 * @param   len     the length of the string, <0 means null-terminated string
 * @param   pend    pointer to the char pointer to returned 
 *                  the address of the first non-numeric character
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_atoof(off_t* pv, const char* s, size_t len, const char** pend);

    /**
 * convert string to type 'ssize_t'
 * @param   pv      pointer to the integer of ssize_t
 * @param   s       pointer to the stirng
 * @param   len     the length of the string, <0 means null-terminated string
 * @param   pend    pointer to the char pointer to returned 
 *                  the address of the first non-numeric character
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_atosz(ssize_t* pv, const char* s, size_t len, const char** pend);

#endif

    /**
 * convert string to 32-bit integer
 * @param   pv      pointer to 32-bit integer
 * @param   s       pointer to the stirng
 * @param   len     the length of the string, <0 means null-terminated string
 * @param   pend    pointer to the char pointer to returned 
 *                  the address of the first non-numeric character
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_atoi(int32_t* pv, const char* s, size_t len, const char** pend);

    /**
 * convert string to 32-bit unsigned integer
 * @param   pv      pointer to 32-bit unsigned integer
 * @param   s       pointer to the stirng
 * @param   len     the length of the string, <0 means null-terminated string
 * @param   pend    pointer to the char pointer to returned 
 *                  the address of the first non-numeric character
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_atoui(uint32_t* pv, const char* s, size_t len, const char** pend);

    /**
 * convert string to 64-bit integer
 * @param   pv      pointer to 64-bit integer
 * @param   s       pointer to the stirng
 * @param   len     the length of the string, <0 means null-terminated string
 * @param   pend    pointer to the char pointer to returned 
 *                  the address of the first non-numeric character
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_atol(int64_t* pv, const char* s, size_t len, const char** pend);

    /**
 * convert string to 64-bit unsigned integer
 * @param   pv      pointer to 64-bit unsigned integer
 * @param   s       pointer to the stirng
 * @param   len     the length of the string, <0 means null-terminated string
 * @param   pend    pointer to the char pointer to returned 
 *                  the address of the first non-numeric character
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_atoul(uint64_t* pv, const char* s, size_t len, const char** pend);

    /**
 * @name    NAI_SPRINTF         the functions of sprintf
 * @{
 */

#if (NAI_HAVE_THIRDPARTY_PRINTF)

    #include <stdarg.h>

    NAI_EXTERN
    nai_int_t nai_sprintf(char* s, const char* fmt, ...);

    NAI_EXTERN
    nai_int_t nai_vsprintf(char* s, const char* fmt, va_list va);

    NAI_EXTERN
    nai_int_t nai_snprintf(char* s, size_t size, const char* fmt, ...);

    NAI_EXTERN
    nai_int_t nai_vsnprintf(char* s, size_t size, const char* fmt, va_list va);

#else

    #define nai_sprintf  sprintf
    #define nai_vsprintf vsprintf

    #if !defined(_MSC_VER) || _MSC_VER >= 1900

        #define nai_snprintf  snprintf
        #define nai_vsnprintf vsnprintf

    #else

NAI_EXTERN
nai_int_t nai_snprintf(char* s, size_t size, const char* fmt, ...);

NAI_EXTERN
nai_int_t nai_vsnprintf(char* s, size_t size, const char* fmt, va_list va);

    #endif

#endif

/** @} */

/**
 * count the length of output base64 string
 * @param   len     the length of source string
 * @return  the length of output base64 string
 */
#define nai_encode_base64_len(len) ((((len) + 2) / 3) * 4)

/**
 * count the length of output source string
 * @param   len     the length of base64 string
 * @return  the length of output source string
 */
#define nai_decode_base64_len(len) ((((len) + 3) / 4) * 3)

    /**
 * encode a string to the base64 string
 * @param   dst     pointer to the dest buffer
 * @param   dstlen  the size of dest buffer
 * @param   src     pointer to the source string
 * @param   srclen  the size of dest string
 * @retval  >=0     the length of output string is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_encode_base64(char* dst, size_t dstlen, const char* src, size_t srclen);

    /**
 * encode a string to the base64 string
 * @param   dst     pointer to the dest buffer
 * @param   dstlen  the size of dest buffer
 * @param   src     pointer to the source string
 * @param   srclen  the size of source string
 * @retval  >=0     the length of output string is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_encode_base64uri(char* dst, size_t dstlen, const char* src, size_t srclen);

    /**
 * decode a base64 string to the source string
 * @param   dst     pointer to the dest buffer
 * @param   dstlen  the size of dest buffer
 * @param   src     pointer to the base64 string
 * @param   srclen  the length of base64 string
 * @retval  >=0     the length of output string is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_decode_base64(char* dst, size_t dstlen, const char* src, size_t srclen);

    /**
 * decode a base64 string to the source string
 * @param   dst     pointer to the dest buffer
 * @param   dstlen  the size of dest buffer
 * @param   src     pointer to the base64 string
 * @param   srclen  the length of base64 string
 * @retval  >=0     the length of output string is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_decode_base64uri(char* dst, size_t dstlen, const char* src, size_t srclen);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
