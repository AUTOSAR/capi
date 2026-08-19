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
/// @file       nai_script.h
/// @brief      
/// @details
/// @date       2021-09-14
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _SCRIPT_H_NAI
#define _SCRIPT_H_NAI

#pragma once

#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_rbtree.h"
#include "nai/runtime/nai_string.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

//////////////////////////////////////////////////////////////////////////////
// script

/**
 * @anchor  NAI_SCRIPT_VAR
 * @name    NAI_SCRIPT_VAR      the flags of the variable
 * @{
 */
#define NAI_SCRIPT_VAR_NOCACHEABLE 0x01
#define NAI_SCRIPT_VAR_PREFIX      0x02
    /** @} */

#ifndef _NAI_TYPEDEF_SCRIPT_VARTABLE_T
    #define _NAI_TYPEDEF_SCRIPT_VARTABLE_T
    typedef struct nai_script_vartable_s nai_script_vartable_t;
#endif
#ifndef _NAI_TYPEDEF_SCRIPT_VARIABLE_T
    #define _NAI_TYPEDEF_SCRIPT_VARIABLE_T
    typedef struct nai_script_variable_s nai_script_variable_t;
#endif
#ifndef _NAI_TYPEDEF_SCRIPT_T
    #define _NAI_TYPEDEF_SCRIPT_T
    typedef struct nai_script_s nai_script_t;
#endif
#ifndef _NAI_TYPEDEF_SCRIPT_CODE_T
    #define _NAI_TYPEDEF_SCRIPT_CODE_T
    typedef struct nai_script_code_s nai_script_code_t;
#endif
#ifndef _NAI_TYPEDEF_SCRIPT_EXPN_T
    #define _NAI_TYPEDEF_SCRIPT_EXPN_T
    typedef struct nai_script_expn_s nai_script_expn_t;
#endif
#ifndef _NAI_TYPEDEF_SCRIPT_VALUE_T
    #define _NAI_TYPEDEF_SCRIPT_VALUE_T
    typedef struct nai_script_value_s nai_script_value_t;
#endif

    /**
 * the handler of get value
 * @param   c       pointer to the context
 * @param   ud      the user data
 * @param   v       pointer to the value
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    typedef nai_sult_t (*nai_script_get_f)(void* c, uintptr_t ud, nai_script_value_t* v);

    /**
 * the handler of set value
 * @param   c       pointer to the context
 * @param   ud      the user data
 * @param   v       pointer to the value
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    typedef nai_sult_t (*nai_script_set_f)(void* c, uintptr_t ud, const nai_script_value_t* v);

    /**
 * the structure of the variable
 */
    struct nai_script_variable_s
    {
        nai_mem_t name;       /**< the name of the variable */
        nai_script_get_f get; /**< the get handler */
        nai_script_set_f set; /**< the set handler */
        uintptr_t ud;         /**< the user data */
        nai_int_t flags;      /**< the flags of the variable */
    };

    /**
 * the structure of the variable table
 */
    struct nai_script_vartable_s
    {
        nai_pool_t* pool;   /**< pointer to the pool */
        nai_rbtree_t map;   /**< the variable map */
        nai_array_t list;   /**< the array of indexed variable */
        nai_array_t prefix; /**< the array of prefix */
    };

/**
 * get the count of indexed variable
 * @param   s       pointer to the variable table
 * @return  the number of indexed variable
 */
#define nai_script_count_variable(s) ((size_t)(s)->list.count)

    /**
 * initial the variable table
 * @param   s       pointer to the variable table
 * @param   pool    pointer to the pool
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_sult_t nai_script_vartable_init(nai_script_vartable_t* s, nai_pool_t* pool);

    /**
 * close the variable table
 * @param   s       pointer to the variable table
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_vartable_close(nai_script_vartable_t* s);

    /**
 * get the variable from the name
 * @param   s       pointer to the variable table
 * @param   name    pointer to the memory view of the name
 * @param   pv      pointer to the pointer of the variable
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_get_variable(nai_script_vartable_t* s, const nai_mem_t* name, nai_script_variable_t** pv);

    /**
 * add a new variable to the variable table
 * @param   s       pointer to the variable table
 * @param   name    pointer to the memory view of the name
 * @param   get     the get handler
 * @param   set     the set handler
 * @param   ud      the user data
 * @param   flags   the flags of the variable
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_add_variable(nai_script_vartable_t* s,
                                       const nai_mem_t* name,
                                       nai_script_get_f get,
                                       nai_script_set_f set,
                                       uintptr_t ud,
                                       nai_int_t flags);

    /**
 * add an array of the variables to the variable table
 * @param   s       pointer to the variable table
 * @param   v       pointer to the array of the variable
 * @param   count   the count of the array
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_add_variables(nai_script_vartable_t* s, const nai_script_variable_t* v, nai_int_t count);

    //////////////////////////////////////////////////////////////////////////////
    // script execute

    /**
 * the entry of the script code
 * @param   s       pointer to the script
 * @return  void
 */
    typedef void (*nai_script_code_f)(nai_script_t* s);

    /**
 * the sturcture of the script code
 */
    struct nai_script_code_s
    {
        nai_script_code_f fn; /**< the code entry */
    };

    /**
 * the structure of the script expression
 */
    struct nai_script_expn_s
    {
        nai_str_t value;            /**< the string view of the expression */
        nai_script_code_t* values;  /**< the codes of content fill */
        nai_script_code_t* lengths; /**< the codes of count length */
    };

    /**
 * the structure of the script value
 */
    struct nai_script_value_s
    {
        union
        {
            struct
            {
                uint32_t len : 29;         /**< the length of the value */
                uint32_t valid : 1;        /**< is valid value */
                uint32_t not_found : 1;    /**< is not exists */
                uint32_t no_cacheable : 1; /**< is no cacheable */
            };
            uint32_t info;
        };
        char* ptr; /**< pointer to the value */
    };

    /**
 * the structure of the script runtime
 */
    struct nai_script_s
    {
        nai_sult_t sult;            /**< the execute reuslt */
        nai_script_vartable_t* st;  /**< pointer to the variable table */
        nai_script_value_t* values; /**< pointer to the cached values */
        nai_script_value_t* sp;     /**< pointer to the stack */
        uint8_t* ip;                /**< pointer to the code */

        union
        {
            uint8_t* pos;  /**< pointer to the buffer */
            uintptr_t len; /**< the length of the buffer */
        };

        void* ctx; /**< pointer to the context */
    };

/**
 * return instruction
 * @param   s       pointer to the script
 * @param   rc      the result code
 * @return  void
 */
#define nai_script_return(s, rc)                                                                                       \
    {                                                                                                                  \
        (s)->sult = (rc);                                                                                              \
        (s)->ip   = (uint8_t*)&nai_script_code_return;                                                                 \
    }

/**
 * jump instruction
 * @param   s       pointer to the script
 * @param   c       pointer to the dest code
 * @return  void
 */
#define nai_script_jump(s, c)                                                                                          \
    {                                                                                                                  \
        (s)->ip = ((uint8_t*)&(c)->off + (c)->off);                                                                    \
    }

/**
 * test whether the string is an expression
 * @param   s       pointer to the stirng view
 * @return  if it is an expression, return 1, otherwise return 0
 */
#define nai_script_is_expn(s) (nai_strnchr(nai_str(s), nai_str_len(s), '$') != 0)

/**
 * initial the script expression
 * @param   e       pointer to the script expression
 * @return  void
 */
#define nai_script_expn_init(e)                                                                                        \
    {                                                                                                                  \
        nai_str_setn(&(e)->value);                                                                                     \
        (e)->values  = 0;                                                                                              \
        (e)->lengths = 0;                                                                                              \
    }

/**
 * test whether the expression is null
 * @param   e       pointer to the script expression
 * @return  if it is a null expression, return 1, otherwise return 0
 */
#define nai_script_expn_is_null(e) (nai_str(&(e)->value) == 0)

/**
 * test whether the expression is empty
 * @param   e       pointer to the script expression
 * @return  if it is an empty expression, return 1, otherwise return 0
 */
#define nai_script_expn_is_empty(e) (nai_str_len(&(e)->value) == 0)

/**
 * test whether the expression is variable
 * @param   e       pointer to the script expression
 * @return  if it is variable, return 1, otherwise return 0
 */
#define nai_script_expn_is_variable(e) ((e)->lengths != 0)

    NAI_EXTERN
    extern const nai_script_code_t nai_script_code_return;

    /**
 * compile the expression
 * @param   e       pointer to the script expression
 * @param   s       pointer to the variable table
 * @param   str     pointer to the string view of the expression
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_expn_compile(nai_script_expn_t* e, nai_script_vartable_t* s, const nai_str_t* str);

    /**
 * initial the script
 * @param   s       pointer to the script runtime
 * @param   st      pointer to the variable table
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_sult_t nai_script_init(nai_script_t* s, nai_script_vartable_t* st);

    /**
 * get the value of the indexed variable
 * @param   s       pointer to the script runtime
 * @param   index   the index of the variable
 * @param   v       pointer to the value
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_get_value(nai_script_t* s, nai_int_t index, nai_script_value_t* v);

    /**
 * set the value of the indexed variable
 * @param   s       pointer to the script runtime
 * @param   index   the index of the variable
 * @param   v       pointer to the value
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_set_value(nai_script_t* s, nai_int_t index, const nai_script_value_t* v);

    /**
 * execute the codes
 * @param   s       pointer to the script runtime
 * @param   code    pointer to the code
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_exec(nai_script_t* s, const void* code);

    /**
 * get the value from a script expression
 * @param   s       pointer to the script runtime
 * @param   e       pointer to the script expression
 * @param   v       pointer to the stirng view to receive data
 * @param   rev     pointer to the information of reserved space
 * @param   pool    pointer to the pool
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_expn_value(
        nai_script_t* s, const nai_script_expn_t* e, nai_str_t* v, const nai_reserved_t* rev, nai_pool_t* pool);

//////////////////////////////////////////////////////////////////////////////
// script code generate

/**
 * @anchor  NAI_SCRIPT_CODE
 * @name    NAI_SCRIPT_CODE     the type of script code
 * @{
 */
#define NAI_SCRIPT_CODE_NORMAL   0x01
#define NAI_SCRIPT_CODE_IF       0x02
#define NAI_SCRIPT_CODE_ELIF     0x03
#define NAI_SCRIPT_CODE_ELSE     0x04
#define NAI_SCRIPT_CODE_LOOP     0x05
#define NAI_SCRIPT_CODE_BREAK    0x06
#define NAI_SCRIPT_CODE_CONTINUE 0x07
    /** @} */

#ifndef _NAI_TYPEDEF_SCRIPT_STMT_T
    #define _NAI_TYPEDEF_SCRIPT_STMT_T
    typedef struct nai_script_stmt_s nai_script_stmt_t;
#endif
#ifndef _NAI_TYPEDEF_SCRIPT_CODEGEN_T
    #define _NAI_TYPEDEF_SCRIPT_CODEGEN_T
    typedef struct nai_script_codegen_s nai_script_codegen_t;
#endif
#ifndef _NAI_TYPEDEF_SCRIPT_CODE_JUMP_T
    #define _NAI_TYPEDEF_SCRIPT_CODE_JUMP_T
    typedef struct nai_script_code_jump_s nai_script_code_jump_t;
#endif
#ifndef _NAI_TYPEDEF_SCRIPT_CODE_LOOP_T
    #define _NAI_TYPEDEF_SCRIPT_CODE_LOOP_T
    typedef struct nai_script_code_loop_s nai_script_code_loop_t;
#endif

    /**
 * the structure of the script code generator
 */
    struct nai_script_codegen_s
    {
        nai_array_t* code;       /**< pointer to the output array */
        nai_pool_t* pool_tmp;    /**< pointer to the temporary pool */
        nai_script_stmt_t* top;  /**< pointer to the top statment */
        nai_script_stmt_t* free; /**< pointer to the first free statment */

        union
        {
            struct
            {
                uint32_t end : 1;     /**< is finsihed */
                uint32_t last_if : 1; /**< is last code is 'if' */
            };
            uint32_t flags;
        };
    };

    /**
 * the structure of jump code
 */
    struct nai_script_code_jump_s
    {
        nai_script_code_f fn; /**< the code entry of condition */
        intptr_t off;         /**< the offset of next code */
    };

    /**
 * the structure of loop code
 */
    struct nai_script_code_loop_s
    {
        nai_script_code_f fn;   /**< the code entry of condition */
        nai_script_code_f next; /**< the code entry of iterator */
        intptr_t off;           /**< the offset of next code */
    };

    /**
 * initial the script code generator
 * @param   c       pointer to the script code generator
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_sult_t nai_script_codegen_init(nai_script_codegen_t* c);

    /**
 * open the script code generator
 * @param   c       pointer to the script code generator
 * @param   out     pointer to the output array
 * @param   tmp     pointer to the temporary pool
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_codegen_open(nai_script_codegen_t* c, nai_array_t* out, nai_pool_t* tmp);

    /**
 * close the script code generator
 * @param   c       pointer to the script code generator
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_codegen_close(nai_script_codegen_t* c);

    /**
 * add a code
 * @param   c       pointer to the script code generator
 * @param   fn      the code entry
 * @param   type    the type of the code
 * @param   size    the size of the code
 * @param   pv      pointer to the pointer of the code
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_add_code(
        nai_script_codegen_t* c, nai_script_code_f fn, uint32_t type, size_t size, nai_script_code_t** pv);

    /**
 * add a loop code
 * @param   c       pointer to the script code generator
 * @param   fn      the code entry of condition
 * @param   next    the code entry of iterator
 * @param   size    the size of the code
 * @param   pv      pointer to the pointer of the code
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_add_loop(
        nai_script_codegen_t* c, nai_script_code_f fn, nai_script_code_f next, size_t size, nai_script_code_t** pv);

    /**
 * finish current code statment, ie. if/else if/else/loop
 * @param   c       pointer to the script code generator
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_end_stmt(nai_script_codegen_t* c);

    /**
 * finish code generation
 * @param   c       pointer to the script code generator
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_script_end_code(nai_script_codegen_t* c);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
