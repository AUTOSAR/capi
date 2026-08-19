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
/// @file       transfer_manager.h
/// @brief      This file provides transfer_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/transfer_managment
/// @interface_level=software
/// @trace_id_sr=413f148d-404c-457c-80d6-b49758149bc8
/// @unit_name=TransferManager
/// @unit_description=This file provides transfer_manager functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_DATATRANSFERSERVICE_H_
#define ISOFT_UDS_SERVER_SERVICE_DATATRANSFERSERVICE_H_

#include "authentication_management/auth_validator.h"
#include "authentication_management/authentication_manager.h"
#include "common/serialize.h"
#include "data_management/environmental_condition_manager.h"
#include "isoft/uds/transfer_managment/transfer_managment.h"
#include "security_access_management/securityLevel_validator.h"
#include "security_access_management/security_level_manager.h"
#include "session_management/session_manager.h"
#include "session_management/session_validator.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Request service type enum class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02421
/// @needwork = dd
/// @endcode
enum class RequestServerType : std::uint8_t
{
    kUnknow,
    kDownload,
    kUpload,
    kFileTransfer
};

/// @brief Status enum class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02422
/// @needwork = dd
/// @endcode
enum class State : std::uint8_t
{
    kIdle,
    kRequest,
    kTransfer,
    kExit,
};

/// @brief Mode enum class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02423
/// @needwork = dd
/// @endcode
enum class ModeOfOperation : std::uint8_t
{
    kReserved    = 0,
    kAddFile     = 1,
    kDeleteFile  = 2,
    kReplaceFile = 3,
    kReadFile    = 4,
    kReadDir     = 5,
    kResumeFile  = 6,
};

/// @brief Transfer manager class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02424
/// @needwork = dd
/// @endcode
class TransferManager
{
public:
    /// @brief Load configuration file
    /// @param[in] dmc Configuration file object
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01129
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(TransferManagment& config,
                            std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                            std::shared_ptr< SessionManager >& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr,
                            std::shared_ptr< EnvironmentalConditionManager >& envirConditionManagerPtr) noexcept;

    /// @brief Process function
    /// @param[in] requestContext Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01130
    /// @needwork = dd
    /// @endcode
    void Process(std::shared_ptr< RequestContext > requestContext) noexcept;

    /// @brief Get session ID validator
    /// @param[in] sid Session ID
    /// @return Session ID validator vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01131
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator(std::uint8_t sid) noexcept;

    /// @brief Confirmation function after sending response
    /// @param[in] request Request context
    /// @param status Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01132
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept;

    /// @brief Get service type of context
    /// @return Service type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01133
    /// @needwork = dd
    /// @endcode
    RequestServerType& GetRequestServerTpye() noexcept { return requestServerType_; }

private:
    /// @brief _fillMD5 function
    /// @param[in] vecMD5 MD5 value vector
    /// @param[in] serialize Serialization object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01134
    /// @needwork = dd
    /// @endcode
    void _fillMD5(std::vector< uint8_t > const& vecMD5, Serialize& serialize);

private:
    /// @brief Transfer manager instance structure
    /// @param[in] maxNumberOfBlockLength Maximum number of blocks
    /// @param[in] authValidator Authentication validator pointer
    /// @param[in] sessionValidator Session validator pointer
    /// @param[in] securityLevelValidator Security validator pointer
    /// @param[in] condition Environmental condition pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01135
    /// @needwork = dd
    /// @endcode
    struct Instance
    {
        std::uint32_t maxNumberOfBlockLength{0U};
        std::shared_ptr< ServerLevelAuthValidator< std::uint8_t > > authValidator{nullptr};
        std::shared_ptr< ServerLevelSessionValidator< std::uint8_t > > sessionValidator{nullptr};
        std::shared_ptr< ServerLevelSecurityLevelValidator< std::uint8_t > > securityLevelValidator{nullptr};
        std::shared_ptr< EnvironmentalCondition > condition{nullptr};
    };

    /// @brief Transfer information structure
    /// @param[in] blockSequenceCounter Block sequence counter
    /// @param[in] previousblockSequenceCounter Previous block sequence counter
    /// @param[in] transferRequestParameterRecord Transfer request parameter record
    /// @param[in] fileSize File size
    /// @param[in] transferSize Transfer size
    /// @param[in] memorySize Memory size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01136
    /// @needwork = dd
    /// @endcode
    struct TransferInfo
    {
        std::uint8_t blockSequenceCounter{1U};

        std::int16_t previousblockSequenceCounter{-1};

        ara::core::Vector< std::uint8_t > transferRequestParameterRecord{};

        std::uint64_t fileSize{0U};

        std::uint64_t transferSize{0U};

        std::uint32_t memorySize{0U};
    };
    /// @brief Initialization function
    /// @param[in] serverInstance Transfer manager instance
    /// @param[in] enableAuth Whether to enable authentication
    /// @param[in] accessPermissionSession Access permission session ID set
    /// @param[in] accessPermissionSecurityLevel Access permission security level set
    /// @param[in] accessPermissionEnvCondition Access permission environmental condition ID
    /// @param[in] authManagerPtr Authentication manager pointer
    /// @param[in] sessionManagerPtr Session manager pointer
    /// @param[in] securityLevelManagerPtr Security level manager pointer
    /// @param[in] envirConditionManagerPtr Environmental condition manager pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01137
    /// @needwork = dd
    /// @endcode
    std::int32_t _initialize(Instance& serverInstance,
                             EnableAuth const& enableAuth,
                             std::set< SessionId > const& accessPermissionSession,
                             std::set< SecurityLevelId > const& accessPermissionSecurityLevel,
                             EnvironmentalConditionId accessPermissionEnvCondition,
                             std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                             std::shared_ptr< SessionManager >& sessionManagerPtr,
                             std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr,
                             std::shared_ptr< EnvironmentalConditionManager >& envirConditionManagerPtr) noexcept;

    /// @brief Request download function
    /// @param[in] requestContext Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01138
    /// @needwork = dd
    /// @endcode
    void _requestDownload(std::shared_ptr< RequestContext >& requestContext) noexcept;

    /// @brief Request upload function
    /// @param[in] requestContext Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01139
    /// @needwork = dd
    /// @endcode
    void _requestUpload(std::shared_ptr< RequestContext >& requestContext) noexcept;

    /// @brief Transfer function
    /// @param[in] requestContext Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01140
    /// @needwork = dd
    /// @endcode
    void _transfer(std::shared_ptr< RequestContext >& requestContext) noexcept;

    /// @brief Transfer exit function
    /// @param[in] requestContext Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01141
    /// @needwork = dd
    /// @endcode
    void _transferExit(std::shared_ptr< RequestContext >& requestContext) noexcept;

    /// @brief Request file transfer function
    /// @param[in] requestContext Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01142
    /// @needwork = dd
    /// @endcode
    void _requestFileTransfer(std::shared_ptr< RequestContext >& requestContext) noexcept;

    /// @brief Clear transfer information function
    /// @param[in] requestContext Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01143
    /// @needwork = dd
    /// @endcode
    void _clearTransferInfo() noexcept;

    /// @brief Add file
    /// @param[in] context Request context
    /// @param[in] requestDeserialize Request deserialization object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01144
    /// @needwork = dd
    /// @endcode
    void _addFile(std::shared_ptr< RequestContext >& context, Serialize& requestDeserialize) noexcept;

    /// @brief Delete file
    /// @param[in] context Request context
    /// @param[in] requestDeserialize Request deserialization object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01145
    /// @needwork = dd
    /// @endcode
    void _deleteFile(std::shared_ptr< RequestContext >& context, Serialize& requestDeserialize) noexcept;

    /// @brief Replace file
    /// @param[in] context Request context
    /// @param[in] requestDeserialize Request deserialization object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01146
    /// @needwork = dd
    /// @endcode
    void _replaceFile(std::shared_ptr< RequestContext >& context, Serialize& requestDeserialize) noexcept;

    /// @brief Read file
    /// @param[in] context Request context
    /// @param[in] requestDeserialize Request deserialization object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01147
    /// @needwork = dd
    /// @endcode
    void _readFile(std::shared_ptr< RequestContext >& context, Serialize& requestDeserialize) noexcept;

    /// @brief Read folder
    /// @param[in] context Request context
    /// @param[in] requestDeserialize Request deserialization object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01148
    /// @needwork = dd
    /// @endcode
    void _readDir(std::shared_ptr< RequestContext >& context, Serialize& requestDeserialize) noexcept;

    /// @brief Append to file
    /// @param[in] context Request context
    /// @param[in] requestDeserialize Request deserialization object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01149
    /// @needwork = dd
    /// @endcode
    void _resumeFile(std::shared_ptr< RequestContext >& context, Serialize& requestDeserialize) noexcept;

private:
    /// @brief Download instance
    /// @name downloadInstance_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01150
    /// @needwork = dd
    /// @endcode
    Instance downloadInstance_{};
    /// @brief Upload instance
    /// @name uploadInstance_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01151
    /// @needwork = dd
    /// @endcode
    Instance uploadInstance_{};
    /// @brief File transfer instance
    /// @name fileTransferInstance_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01152
    /// @needwork = dd
    /// @endcode
    Instance fileTransferInstance_{};
    /// @brief Download interface pointer
    /// @name downloadPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01153
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DownloadInterface > downloadPtr_{nullptr};
    /// @brief Upload interface pointer
    /// @name uploadPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01154
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< UploadInterface > uploadPtr_{nullptr};
    /// @brief File transfer interface pointer
    /// @name fileTransferPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01155
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< FileTransferInterface > fileTransferPtr_{nullptr};
    /// @brief Transfer status
    /// @name state_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01156
    /// @needwork = dd
    /// @endcode
    State state_{State::kIdle};
    /// @brief Transfer mode
    /// @name modeOfOperation_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01157
    /// @needwork = dd
    /// @endcode
    ModeOfOperation modeOfOperation_{ModeOfOperation::kReserved};
    /// @brief Request server type
    /// @name requestServerType_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01158
    /// @needwork = dd
    /// @endcode
    RequestServerType requestServerType_{RequestServerType::kUnknow};
    /// @brief Transfer information
    /// @name transferInfo_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01159
    /// @needwork = dd
    /// @endcode
    TransferInfo transferInfo_{};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_DATATRANSFERSERVICE_H_