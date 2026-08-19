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
/// @file       isoft_assert.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Assertion macro definition
/// @date       2021-07-14
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-07-14  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=Error assertion
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_COMMON_PH_ASSERT_H_
#define ARA_PER_COMMON_PH_ASSERT_H_

#include <cassert>
#include <tuple>

#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"

//********************************/
#ifndef PER_Assert
    #define PER_Assert(exp)                                                                                            \
        {                                                                                                              \
            bool const bAssert{exp};                                                                                   \
            if (false == bAssert) {                                                                                    \
                std::ignore = fprintf(stderr, "PER_Assert : %s:%d\n", __FILE__, static_cast< int32_t >(__LINE__));     \
                std::ignore = ara::per::isoftkv::LogFatal() << "PER_Assert : " << __FILE__ << __LINE__;                \
            }                                                                                                          \
            assert(bAssert);                                                                                           \
        }                                                                                                              \
        std::ignore = ara::per::isoftkv::T_Void(0)
#endif
#ifndef PER_Assert_LOG
    #define PER_Assert_LOG(exp)                                                                                        \
        {                                                                                                              \
            bool const bAssert{exp};                                                                                   \
            if (false == bAssert) {                                                                                    \
                std::ignore = fprintf(stderr, "PER_Assert : %s:%d\n", __FILE__, static_cast< int32_t >(__LINE__));     \
                std::ignore = LogInfo() << "PER_Assert : " << __FILE__ << __LINE__;                                    \
            }                                                                                                          \
            assert(bAssert);                                                                                           \
        }                                                                                                              \
        std::ignore = ara::per::isoftkv::T_Void(0)
#endif

//********************************/
#endif
