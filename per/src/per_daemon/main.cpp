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
/// @file       main.cpp
/// @brief      Process entry function
/// @details
/// @date       2023-01-16
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2023-01-16 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include <iostream>
#include <tuple>
namespace {
/// @brief Define the char_8 data type
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = no
/// @endcode
using char_8 = char;  // NOLINT
}  // namespace

/// @brief Process entry function
/// @param argc Number of process arguments
/// @param argv Process arguments
/// @return Process execution result, 0 for normal, non-zero for abnormal
/// @code{.isoft}
/// @unit_name=main
/// @interface_level=unit
/// @needwork = no
/// @endcode
int32_t main(int32_t const argc, char_8* argv[]) noexcept
{
    std::ignore = argc;
    std::ignore = argv;
    return 0;
}