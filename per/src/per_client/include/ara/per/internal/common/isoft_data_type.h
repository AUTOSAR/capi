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
/// @file       isoft_data_type.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Common data type definitions used by PER persistence
/// @date       2021-05-19
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2023-02-23  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_COMMON_PH_DATA_TYPE_H_
#define ARA_PER_COMMON_PH_DATA_TYPE_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>

namespace ara {
namespace per {
//********************************/
/// @brief QAC2428: Cannot use char directly
using char8_t = char;  // NOLINT
//********************************/
constexpr int32_t kInt_2{2};
constexpr int32_t kInt_3{3};
constexpr int32_t kInt_4{4};
constexpr int32_t kInt_5{5};
constexpr int32_t kInt_8{8};
constexpr int32_t kInt_9{9};
constexpr int32_t kInt_10{10};
constexpr int32_t kInt_16{16};
constexpr int32_t kInt_32{32};
constexpr int32_t kInt_64{64};
constexpr int32_t kInt_128{128};
constexpr int32_t kInt_0x10{0x10};
constexpr int32_t kInt_0x100{0x100};
constexpr int32_t kInt_0x10000{0x10000};
constexpr int32_t kInt_2E5{1 << 5};
constexpr int32_t kInt_2E11{1 << 11};

constexpr uint16_t kInt_0xFFFFU{0xFFFFU};
constexpr uint16_t kInt16_26U{26U};

constexpr uint32_t kInt_2U{2U};
constexpr uint32_t kInt_4U{4U};
constexpr uint32_t kInt_16U{16U};
constexpr uint32_t kInt32_32U{32U};
constexpr uint32_t kInt_64U{64U};
constexpr uint32_t kInt_256U{256U};
constexpr uint32_t kInt_512U{512U};
constexpr uint32_t kInt_1000U{1000U};
constexpr uint32_t kInt_1024U{1024U};
constexpr uint32_t kInt32_4096U{4096U};
constexpr uint32_t kInt_0x10U{static_cast< uint32_t >(kInt_0x10)};
constexpr uint32_t kInt_0x100U{static_cast< uint32_t >(kInt_0x100)};

constexpr uint64_t kInt64_4096U{4096U};
constexpr uint64_t kInt64_10240000U{10240000U};
constexpr uint64_t kInt_0x100000000U{0x100000000U};

constexpr char8_t kChar_a{'a'};
constexpr char8_t kChar_z{'z'};
constexpr char8_t kChar_A{'A'};
constexpr char8_t kChar_Z{'Z'};
constexpr char8_t kChar_0{'0'};
constexpr char8_t kChar_9{'9'};
constexpr char8_t kChar_Enter{'\n'};
constexpr char8_t kChar_Dollar{'$'};
constexpr char8_t kChar_Comma{','};
constexpr char8_t kChar_BraceEnd{'}'};
//********************************/

namespace isoftkv {
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
