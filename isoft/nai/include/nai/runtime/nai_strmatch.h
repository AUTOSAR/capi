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
/// @file       nai_strmatch.h
/// @brief      wildcard matching
/// @details
/// @date       2021-02-26
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation is 
 *          shell-style pattern matching for ?, \, [], and * characters.
 *
 * @details the code example is as follows:
 *
 * @par     match a path
 * @code
 *          nai_int_t r;
 *          const char* text;
 *
 *          r = nai_wildmatch("/var/log/error.*", text, NAI_WMATCH_PATHNAME);
 * @endcode
 */

/*
 * the implementation of wildmatch 
 * from https://github.com/git/git which is GPLv2-licensed, 
 * replace it to https://github.com/davvid/wildmatch which is BSD-licensed.
 */

#ifndef _STRMATCH_H_NAI
#define _STRMATCH_H_NAI

#pragma once

#include "nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
 * @anchor  NAI_WMATCH_FLAG
 * @name    NAI_WMATCH_FLAG     the flag of the wildcard matching
 * @{
 */

#define NAI_WMATCH_CASELESS 1 /**< case insensitive */
#define NAI_WMATCH_PATHNAME 2 /**< to enable path mode */

    /** @} */

    /**
 * string wildcard matching
 * @param   pattern pointer to the string of pattern
 * @param   text    pointer to the string of text to match
 * @param   flags   the flags of match mode, see @ref NAI_WMATCH_FLAG
 * @return  matched return 1, otherwise return 0
 */
    NAI_EXTERN
    nai_int_t nai_wildmatch(const char* pattern, const char* text, nai_int_t flags);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
