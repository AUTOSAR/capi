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
/// @file       isoft_file_refuse_opt.h
/// @brief      =
/// @details
/// @date       2025-11-13
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2025-11-13  <td>1.0.0    <td>Niuliming   <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/File Operations
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PFileRefuseOpt
/// @unit_description=
/// @endcode
///
/// ================================================================

#include <iostream>
#include <memory>
#include <mutex>

#include "ara/core/map.h"
#include "ara/core/string_view.h"
#include "ara/per/internal/common/isoft_thread_lock.h"

namespace ara {
namespace per {
namespace isoftkv {
/// @brief File lock operation class
class PFileRefuseOpt
{
public:
    /// @brief
    /// @param
    PFileRefuseOpt(PFileRefuseOpt const&) = delete;
    /// @brief
    /// @param
    PFileRefuseOpt(PFileRefuseOpt&&) = delete;
    /// @brief
    /// @param
    /// @return
    PFileRefuseOpt& operator=(PFileRefuseOpt const&) = delete;
    /// @brief
    /// @param
    /// @return
    PFileRefuseOpt& operator=(PFileRefuseOpt&&) = delete;
    /// @brief
    ~PFileRefuseOpt() = default;

public:
    /// @brief Get singleton instance
    /// @return
    static std::shared_ptr< PFileRefuseOpt > GetInstance();
    /// @brief Acquire lock for operating file
    /// @param fileName File name
    /// @return
    bool AllowOperation(ara::core::StringView const& fileName);
    /// @brief Release lock for operating file after completion
    /// @param fileName File name
    void OperationComplete(ara::core::StringView const& fileName);

private:
    /// @brief Default constructor for singleton
    PFileRefuseOpt() = default;

private:
    /// @brief Mutex for singleton class thread safety
    static std::mutex s_SingleMutex_;
    /// @brief Mutex for creating locks thread safely
    std::mutex fileLockMutex_{};
    /// @brief Unique object of singleton class
    static std::shared_ptr< PFileRefuseOpt > s_SingleInstance_;
    /// @brief Mapping of files and locks
    ara::core::Map< std::string, std::shared_ptr< PThreadLock > > fileMapLock_;
};
}  // namespace isoftkv
}  // namespace per
}  // namespace ara