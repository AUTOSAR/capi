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
/// @file       kv_object.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    KV system management base class
/// @date       2021-06-30
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Features
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PKvObject
/// @unit_description=Persistence module log output class
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-06-30 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#ifndef ARA_PER_PHKV_KV_OBJECT_H_
#define ARA_PER_PHKV_KV_OBJECT_H_

#include "ara/log/common.h"
#include "ara/per/internal/isoftkv/kv_error_code.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
class PKvObject : public PKvErrorCode
{
public:
    /// @brief
    explicit PKvObject() noexcept = default;
    /// @brief
    ~PKvObject() noexcept override = default;
    /// @brief
    /// @param a
    PKvObject(PKvObject const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvObject &operator=(PKvObject const &a) = delete;
    /// @brief
    /// @param a
    PKvObject(PKvObject &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvObject &operator=(PKvObject &&a) = delete;

public:
    /// @brief
    /// @return
    virtual bool IsAccessReady() const noexcept = 0;
    /// @brief
    /// @return
    virtual ara::core::StringView GetLogKvName() const noexcept = 0;

protected:
    /// @brief Output log level: ara::log::LogLevel::kVerbose
    /// @param pchLogKey
    /// @param pchMsg
    /// @param pchKey1
    /// @param nValue1
    /// @param pchKey2
    /// @param nValue2
    /// @param pchKey3
    /// @param nValue3
    /// @param pchKey4
    /// @param nValue4
    /// @param pchKey5
    /// @param nValue5
    /// @param pchKey6
    /// @param nValue6
    void _LogDetail(char8_t const *const pchLogKey,
                    char8_t const *const pchMsg,
                    char8_t const *const pchKey1 = nullptr,
                    int32_t const nValue1        = 0,
                    char8_t const *const pchKey2 = nullptr,
                    int32_t const nValue2        = 0,
                    char8_t const *const pchKey3 = nullptr,
                    int32_t const nValue3        = 0,
                    char8_t const *const pchKey4 = nullptr,
                    int32_t const nValue4        = 0,
                    char8_t const *const pchKey5 = nullptr,
                    int32_t const nValue5        = 0,
                    char8_t const *const pchKey6 = nullptr,
                    int32_t const nValue6        = 0) const noexcept;
    /// @brief Output log level: ara::log::LogLevel::kDebug
    /// @param pchLogKey
    /// @param pchMsg
    /// @param pchKey1
    /// @param nValue1
    /// @param pchKey2
    /// @param nValue2
    /// @param pchKey3
    /// @param nValue3
    /// @param pchKey4
    /// @param nValue4
    /// @param pchKey5
    /// @param nValue5
    /// @param pchKey6
    /// @param nValue6
    void _LogDebug(char8_t const *const pchLogKey,
                   char8_t const *const pchMsg,
                   char8_t const *const pchKey1 = nullptr,
                   int32_t const nValue1        = 0,
                   char8_t const *const pchKey2 = nullptr,
                   int32_t const nValue2        = 0,
                   char8_t const *const pchKey3 = nullptr,
                   int32_t const nValue3        = 0,
                   char8_t const *const pchKey4 = nullptr,
                   int32_t const nValue4        = 0,
                   char8_t const *const pchKey5 = nullptr,
                   int32_t const nValue5        = 0,
                   char8_t const *const pchKey6 = nullptr,
                   int32_t const nValue6        = 0) const noexcept;
    /// @brief Output log level: ara::log::LogLevel::kInfo
    /// @param pchLogKey
    /// @param pchMsg
    /// @param pchKey1
    /// @param nValue1
    /// @param pchKey2
    /// @param nValue2
    /// @param pchKey3
    /// @param nValue3
    /// @param pchKey4
    /// @param nValue4
    /// @param pchKey5
    /// @param nValue5
    /// @param pchKey6
    /// @param nValue6
    void _LogInfo(char8_t const *const pchLogKey,
                  char8_t const *const pchMsg,
                  char8_t const *const pchKey1 = nullptr,
                  int32_t const nValue1        = 0,
                  char8_t const *const pchKey2 = nullptr,
                  int32_t const nValue2        = 0,
                  char8_t const *const pchKey3 = nullptr,
                  int32_t const nValue3        = 0,
                  char8_t const *const pchKey4 = nullptr,
                  int32_t const nValue4        = 0,
                  char8_t const *const pchKey5 = nullptr,
                  int32_t const nValue5        = 0,
                  char8_t const *const pchKey6 = nullptr,
                  int32_t const nValue6        = 0) const noexcept;
    /// @brief Output log level: ara::log::LogLevel::kInfo
    /// @tparam T_Data
    /// @param pchMsg
    /// @param pchKey
    /// @param nValue
    template < typename T_Data >
    void _LogInfo(char8_t const *const pchMsg, char8_t const *const pchKey, T_Data const nValue) const noexcept;
    /// @brief Output log level: ara::log::LogLevel::kError
    /// @param pchLogKey
    /// @param pchMsg
    /// @param pchKey1
    /// @param nValue1
    void _LogError(char8_t const *const pchLogKey,
                   char8_t const *const pchMsg,
                   char8_t const *const pchKey1 = nullptr,
                   int32_t const nValue1        = 0) const noexcept;
    /// @brief Output log level: ara::log::LogLevel::kFatal
    /// @param pchLogKey
    /// @param pchMsg
    /// @param pchKey1
    /// @param nValue1
    void _LogFatal(char8_t const *const pchLogKey,
                   char8_t const *const pchMsg,
                   char8_t const *const pchKey1 = nullptr,
                   int32_t const nValue1        = 0) const noexcept;

protected:
    /// @brief Output log level: ara::log::LogLevel::
    /// @param nLogLevel
    /// @param pchLogKey
    /// @param pchMsg
    /// @param pchKey1
    /// @param nValue1
    /// @param pchKey2
    /// @param nValue2
    /// @param pchKey3
    /// @param nValue3
    /// @param pchKey4
    /// @param nValue4
    /// @param pchKey5
    /// @param nValue5
    /// @param pchKey6
    /// @param nValue6
    void _LogPrint(ara::log::LogLevel const nLogLevel,
                   char8_t const *const pchLogKey,
                   char8_t const *const pchMsg,
                   char8_t const *const pchKey1 = nullptr,
                   int32_t const nValue1        = 0,
                   char8_t const *const pchKey2 = nullptr,
                   int32_t const nValue2        = 0,
                   char8_t const *const pchKey3 = nullptr,
                   int32_t const nValue3        = 0,
                   char8_t const *const pchKey4 = nullptr,
                   int32_t const nValue4        = 0,
                   char8_t const *const pchKey5 = nullptr,
                   int32_t const nValue5        = 0,
                   char8_t const *const pchKey6 = nullptr,
                   int32_t const nValue6        = 0) const noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
