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
/// @file       isoft_kv_debug.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Puhua KV System: Collection of test functions
/// @date       2023-07-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Features
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=Convenient for debugging, print relevant information
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2023-07-18 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#ifndef ARA_PER_PHKV_DEBUG_H_
#define ARA_PER_PHKV_DEBUG_H_

#include "ara/per/internal/isoftkv/kv_system.h"
#include "ara/per/internal/isoftkv/page_opt_key_hash.h"
#include "ara/per/internal/isoftkv/page_opt_kv_store.h"
#include "ara/per/internal/isoftkv/page_opt_page_boss.h"
#include "ara/per/internal/isoftkv/page_opt_page_group.h"
#include "ara/per/internal/isoftkv/page_opt_value.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/

/// @brief Convenient for debugging, print relevant information: PPageOptBase
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return
bool Debug_PrintPage(PConsoleLog &consoleLog, PPageOptBase const &optPage) noexcept;
/// @brief Convenient for debugging, print relevant information: PPageOptValue
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return
bool Debug_PrintPage(PConsoleLog &consoleLog, PPageOptValue const &optPage) noexcept;
/// @brief Print Hash table
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
bool Debug_PrintHash(PConsoleLog &consoleLog, PPageOptKeyHash const &optPage) noexcept;
/// @brief Convenient for debugging, print relevant information: PPageOptKeyHash
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return
bool Debug_PrintPage(PConsoleLog &consoleLog, PPageOptKeyHash const &optPage) noexcept;
/// @brief Convenient for debugging, print relevant information: PPageOptPageGroup
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return
bool Debug_PrintPage(PConsoleLog &consoleLog, PPageOptPageGroup const &optPage) noexcept;
/// @brief Convenient for debugging, print relevant information: PPageOptKvStore
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return
bool Debug_PrintPage(PConsoleLog &consoleLog, PPageOptKvStore const &optPage) noexcept;
/// @brief Convenient for debugging, print relevant information: PPageOptPageBoss
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return
bool Debug_PrintPage(PConsoleLog &consoleLog, PPageOptPageBoss const &optPage) noexcept;
//********************************/
/// @brief Traverse all pages and print relevant information
/// @param kvSystem
void Debug_PrintPage(PKvSystem const &kvSystem,
                     uint8_t nPrintType = static_cast< uint8_t >(EBlockType::kBlend)) noexcept;
/// @brief Traverse all pages and print BLOCK information
/// @param kvSystem
void Debug_PrintPage(PKvSystem const &kvSystem,
                     uint32_t nPageID,
                     uint8_t nPrintType = static_cast< uint8_t >(EBlockType::kBlend)) noexcept;
/// @brief Print all Keys
/// @param kvSystem
void Debug_PrintAllKey(PKvSystem &kvSystem) noexcept;
/// @brief Print the content of KvSystem
/// @param stWorkPath
/// @param stKvName
/// @param func
void PrintKvSystem(ara::core::String const &stWorkPath,
                   ara::core::String const &stKvName,
                   std::function< bool(PKvSystem &kvSystem, ara::core::String const &stKey) > const &func);
/// @brief Print the content of KvSystem
/// @param kvSystem
/// @param func
void PrintKvSystem(PKvSystem &kvSystem,
                   std::function< bool(PKvSystem &kvSystem, ara::core::String const &stKey) > const &func);
/// @brief Print the content of KvSystem
/// @param kvSystemWork
void PrintKvSystem(PKvSystem &kvSystemWork);
/// @brief  Print the UpdateFile of the KV library, output version information and Key values but not Value values, and perform CRC check
/// @param stFileName
/// @return
bool Debug_PrintUpdateFile(ara::core::StringView const &stFileName);
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
