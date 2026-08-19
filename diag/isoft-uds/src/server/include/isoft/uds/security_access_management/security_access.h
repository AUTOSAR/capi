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
/// @file       security_access.h
/// @brief      This file provides security_access functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/security_access_management
/// @interface_level=software
/// @trace_id_sr=f1d36f78-6ef1-465e-9d90-a20fafa143e8
/// @unit_name=SecurityAccess
/// @unit_description=This file provides security_access functionality
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_SECURITYACCESS_H_
#define ISOFT_UDS_SERVER_SERVICE_SECURITYACCESS_H_
#include <functional>
#include <vector>

#include "isoft/uds/cancellation_handler.h"
#include "isoft/uds/common.h"
#include "isoft/uds/result.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Key comparison result type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01417
/// @trace_id_dd=DD_UDS_02709
/// @needwork = ad
/// @endcode
enum class KeyCompareResultType
{
    kKeyValid   = 0x00,  ///< Key is valid
    kKeyInvalid = 0x01,  ///< Key is invalid
};
/// @brief Security access interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01418
/// @trace_id_dd=DD_UDS_02710
/// @needwork = ad
/// @endcode
class SecurityAccessInterface
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01035
    /// @needwork = dda
    /// @endcode
    SecurityAccessInterface(SecurityAccessInterface const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01036
    /// @needwork = dda
    /// @endcode
    SecurityAccessInterface& operator=(SecurityAccessInterface const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01037
    /// @needwork = dda
    /// @endcode
    SecurityAccessInterface(SecurityAccessInterface&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01038
    /// @needwork = dda
    /// @endcode
    SecurityAccessInterface& operator=(SecurityAccessInterface&&) = delete;

public:
    /// @brief Security access status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01289
    /// @trace_id_dd=DD_UDS_02403
    /// @needwork = ad
    /// @endcode
    enum class State
    {
        kLock,
        kUnlock
    };

    /// @brief Constructor
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01290
    /// @trace_id_dd=DD_UDS_02404
    /// @needwork = ad
    /// @endcode
    SecurityAccessInterface() = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01291
    /// @trace_id_dd=DD_UDS_02405
    /// @needwork = ad
    /// @endcode
    virtual ~SecurityAccessInterface() noexcept = default;

    /// @brief Get security access seed
    /// @param[in] securityAccessDataRecord Security Access payload
    /// @param[in] metaInfo MetaInfoMap of the request
    /// @param[in] cancellationHandler Set when current session is cancelled
    /// @return Provided seed value, error codes include:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @retval kSubfunctionNotSupported: Subfunction not supported
    /// @retval kIncorrectMessageLengthOrInvalidFormat: Incorrect message length or invalid format
    /// @retval kConditionsNotCorrect: Conditions not correct
    /// @retval kRequestSequenceError: Request sequence error
    /// @retval kRequestOutOfRange: Request out of range
    /// @retval kInvalidKey: Invalid key
    /// @retval kExceedNumberOfAttempts: Exceed number of attempts
    /// @retval kRequiredTimeDelayNotExpired: Required time delay not expired
    /// @trace_id_sws={SWS_DM_00764}@tracestatus{draft}
    /// @endcode
    virtual Result< std::vector< std::uint8_t > > GetSeed(std::vector< std::uint8_t > const& securityAccessDataRecord,
                                                          MetaInfoMap& metaInfo,
                                                          CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Compare key
    /// @param[in] key Key to be verified
    /// @param[in] metaInfo Request MetaInfoMap
    /// @param[in] cancellationHandler Set when current session is cancelled
    /// @return Key verification result, error codes include:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @retval kSubfunctionNotSupported: Subfunction not supported
    /// @retval kIncorrectMessageLengthOrInvalidFormat: Incorrect message length or invalid format
    /// @retval kConditionsNotCorrect: Conditions not correct
    /// @retval kRequestSequenceError: Request sequence error
    /// @retval kRequestOutOfRange: Request out of range
    /// @retval kInvalidKey: Invalid key
    /// @retval kExceedNumberOfAttempts: Exceed number of attempts
    /// @retval kRequiredTimeDelayNotExpired: Required time delay not expired
    /// @trace_id_sws={SWS_DM_00765}@tracestatus{draft}
    /// @endcode
    virtual Result< KeyCompareResultType > CompareKey(std::vector< std::uint8_t > const& key,
                                                      MetaInfoMap& metaInfo,
                                                      CancellationHandler cancellationHandler) noexcept = 0;

private:
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  //ISOFT_UDS_SERVER_SERVICE_SECURITYACCESS_H_