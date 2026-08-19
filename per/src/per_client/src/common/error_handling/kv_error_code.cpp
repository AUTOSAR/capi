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
/// @file       kv_error_code.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Error codes
/// @date       2021-06-09
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Error Handling
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-06-09 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/isoftkv/kv_error_code.h"

#include "ara/per/internal/isoftkv/kv_log_api.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/

/// @brief
/// @param pchFile
/// @param nLineNum
/// @param eErrorCode
/// @param nLogLevel
void PKvErrorCode::OnOptFailed(char8_t const* const pchFile,
                               int32_t const nLineNum,
                               EErrorPHKV const eErrorCode,
                               ara::log::LogLevel const nLogLevel) noexcept
{
    if (eErrorCode != EErrorPHKV::kSuccess) {
        std::unique_ptr< PPrintLog > pPrintLog{nullptr};
        switch (nLogLevel) {
            case ara::log::LogLevel::kFatal: {
                pPrintLog = std::make_unique< PPrintLog >(ara::log::LogLevel::kError, "<PH_FATAL>");
            } break;
            case ara::log::LogLevel::kError: {
                pPrintLog = std::make_unique< PPrintLog >(ara::log::LogLevel::kError, "<PH_ERROR>");
            } break;
            case ara::log::LogLevel::kWarn: {
                pPrintLog = std::make_unique< PPrintLog >(ara::log::LogLevel::kError, "<PH_WARN >");
            } break;
            case ara::log::LogLevel::kInfo: {
                pPrintLog = std::make_unique< PPrintLog >(ara::log::LogLevel::kError, "<PH_INFO >");
            } break;
            case ara::log::LogLevel::kDebug: {
                pPrintLog = std::make_unique< PPrintLog >(ara::log::LogLevel::kError, "<PH_DEBUG>");
            } break;
            case ara::log::LogLevel::kVerbose: {
                pPrintLog = std::make_unique< PPrintLog >(ara::log::LogLevel::kError, "<PH_VERBOSE>");
            } break;
            default: {
                pPrintLog = std::make_unique< PPrintLog >(ara::log::LogLevel::kError, "<PH_DEBUG>");
            } break;
        }
        if (nullptr != pPrintLog) {
            uint8_t chBuff[kInt_1024U]{0U};
            std::ignore
                = snprintf(T_TransChar(static_cast< uint8_t* >(chBuff)), kInt_1024U, "< %s:%d >", pchFile, nLineNum);
            if (0U == chBuff[0]) {  // This output parameter value is not subsequently checked.
                chBuff[0] = 0U;
            }
            pPrintLog->PrintLog(T_TransChar(static_cast< uint8_t* >(chBuff)));
            pPrintLog->PrintLog(" ErrorCode = ", static_cast< int32_t >(eErrorCode));
        }
    }
    return;
}
/// @brief
/// @param pBData
/// @param nDataLen
/// @return
bool PKvErrorCode::CheckParam(uint8_t const* const pBData, int32_t const nDataLen) noexcept
{
    if ((nullptr != pBData) && (nDataLen > 0)) {
        return true;
    }
    return false;
}
/// @brief
/// @param nDataLen
/// @return
bool PKvErrorCode::CheckParam(int32_t const nDataLen) noexcept
{
    if (nDataLen > 0) {
        return true;
    }
    return false;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
