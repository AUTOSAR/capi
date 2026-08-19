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
/// @file       types.h
/// @brief
/// @details
/// @date       2021-11-10
/// @author     wei.jia
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_COMMON_INCLUDE_PUBLIC_ISOFT_TYPES_H_
#define ISOFT_COMMON_INCLUDE_PUBLIC_ISOFT_TYPES_H_

#include <string>

///
/// @brief Common type definitions used by isoft-ap
///
namespace isoft {
///
/// @brief
/// Example of external type wrapper, for demonstration only. The fact is that ara::core::String already exists,
/// and external APIs all use ara::core. Therefore, isoft-ap should prioritize using existing types in ara::core to reduce type inconsistency issues
///
using string = ::std::string;

}  // namespace isoft
#endif  // ISOFT_COMMON_INCLUDE_PUBLIC_ISOFT_TYPES_H_
