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
/// @file       isoft_file_refuse_opt.cpp
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
/// <tr><td>2025-11-13  <td>1.0.0    <td>niuliming   <td>Create initial version
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

#include "ara/per/internal/common/isoft_file_refuse_opt.h"

#include <memory>

namespace ara {
namespace per {
namespace isoftkv {
std::shared_ptr< PFileRefuseOpt > PFileRefuseOpt::s_SingleInstance_{nullptr};  // NOLINT
std::mutex PFileRefuseOpt::s_SingleMutex_{};

std::shared_ptr< PFileRefuseOpt > PFileRefuseOpt::GetInstance()
{
    if (s_SingleInstance_ == nullptr) {
        /// niuliming: Thread-safe singleton class
        std::unique_lock< std::mutex > const lck{s_SingleMutex_};
        if (s_SingleInstance_ == nullptr) {
            s_SingleInstance_.reset(new PFileRefuseOpt);
        }
    }
    return s_SingleInstance_;
}

bool PFileRefuseOpt::AllowOperation(ara::core::StringView const& fileNameStrView)
{
    ara::core::Map< std::string, std::shared_ptr< PThreadLock > >::iterator it;
    /// 2025-11-14 niuliming: StringView is a string view, does not own exclusive memory.
    std::string fileNameStr{fileNameStrView.data(), fileNameStrView.size()};

    it = fileMapLock_.find(fileNameStr);
    if (it == fileMapLock_.end()) {
        /// 2025-11-13 niuliming: File has no corresponding lock; ensure thread safety when creating new lock
        std::unique_lock< std::mutex > const lck{fileLockMutex_};
        it = fileMapLock_.find(fileNameStr);
        if (it == fileMapLock_.end()) {
            std::shared_ptr< PThreadLock > fileLock{std::make_shared< PThreadLock >()};
            bool res{fileLock->TryLock()};
            fileMapLock_.insert(std::pair< std::string, std::shared_ptr< PThreadLock > >(fileNameStr, fileLock));
            return res;
        }
    }

    /// 2025-11-14 niuliming: Value type in Map is smart pointer, operates on same object
    return it->second->TryLock();
}

void PFileRefuseOpt::OperationComplete(ara::core::StringView const& fileNameStrView)
{
    ara::core::Map< std::string, std::shared_ptr< PThreadLock > >::iterator it;
    /// 2025-11-14 niuliming: StringView is a string view, does not own exclusive memory.
    std::string fileNameStr{fileNameStrView.data(), fileNameStrView.size()};

    it = fileMapLock_.find(fileNameStr);
    if (it == fileMapLock_.end()) {
        /// 2025-11-14 niuliming: Lock not found when releasing lock; this situation should not occur
        return;
    }

    /// 2025-11-14 niuliming: Value type in Map is smart pointer, operates on same object
    it->second->UnLock();
    return;
}
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
