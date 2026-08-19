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
/// @file       request_context.h
/// @brief      This file provides request_context functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/common
/// @interface_level=software
/// @trace_id_sr=e81c720c-396c-4cab-b547-98c0a2aad431
/// @unit_name=RequestContext
/// @unit_description=This file provides request_context functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_REQUEST_CONTEXT_H_
#define ISOFT_UDS_SERVER_REQUEST_CONTEXT_H_
#include "common/timer.h"
#include "isoft/uds/cancellation_state.h"
#include "isoft/uds/channel.h"
#include "isoft/uds/common.h"
#include "isoft/uds/message.h"
#include "isoft/uds/uds_nrc_error_domain.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Response status type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02576
/// @needwork = dd
/// @endcode
enum class ResponseStatusType : std::uint8_t
{
    kResPosOk               = 0x00,  ///< Positive response has been sent out successfully
    kResPosNotOk            = 0x01,  ///< Positive response has not been sent out successfully
    kResNegOk               = 0x02,  ///< Negative response has been sent out successfull
    kResNegNotOk            = 0x03,  ///< Negative response has not been sent out successfully
    kResPosSuppressed       = 0x04,  ///< Positive answer suppressed
    kResNegSuppressed       = 0x05,  ///< Negative answer suppressed
    kCanceled               = 0x06,  ///< Processing is canceled
    kNoProcessingNoResponse = 0x07,  ///< Processing rejected in Validation
};
/// @brief EnumToString function
/// @param type Parameter
/// @return Return value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02577
/// @needwork = dd
/// @endcode
inline std::string EnumToString(ResponseStatusType type) noexcept
{
    std::string result;
    switch (type) {
        case ResponseStatusType::kResPosOk:
            result = "Positive response has been sent out successfully";
            break;
        case ResponseStatusType::kResPosNotOk:
            result = "Positive response has not been sent out successfully";
            break;
        case ResponseStatusType::kResNegOk:
            result = "Negative response has been sent out successfull";
            break;
        case ResponseStatusType::kResNegNotOk:
            result = "Negative response has not been sent out successfully";
            break;
        case ResponseStatusType::kResPosSuppressed:
            result = "Positive answer suppressed";
            break;
        case ResponseStatusType::kResNegSuppressed:
            result = "Negative answer suppressed";
            break;
        case ResponseStatusType::kCanceled:
            result = "Processing is canceled";
            break;
        case ResponseStatusType::kNoProcessingNoResponse:
            result = "Processing rejected in Validation";
            break;
        default:
            break;
    }
    return result;
}
/// @brief Service implementation class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02578
/// @needwork = dd
/// @endcode
class ServerImpl;

/// @brief Request context class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02579
/// @needwork = dd
/// @endcode
class RequestContext;

/// @brief Request context confirmation callback function
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02580
/// @needwork = dd
/// @endcode
using RequestContextConfirmCallback = std::function< void(RequestContext&, ResponseStatusType, bool) >;

/// @brief Request context save callback function
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02581
/// @needwork = dd
/// @endcode
using RequestContextSaveCallback = std::function< void(RequestContext&) >;
/// @brief Provides data response function and data service verification
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02582
/// @needwork = dd
/// @endcode
class RequestContext final
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00831
    /// @needwork = dda
    /// @endcode
    RequestContext(RequestContext const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00832
    /// @needwork = dda
    /// @endcode
    RequestContext& operator=(RequestContext const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00833
    /// @needwork = dda
    /// @endcode
    RequestContext(RequestContext&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00834
    /// @needwork = dda
    /// @endcode
    RequestContext& operator=(RequestContext&&) = delete;

    /// @brief Constructor
    /// @param[in] impl Service implementation class
    /// @param[in] request Request message object
    /// @param[in] serverAddress Service logical address
    /// @param[in] functionalAddress Functional address
    /// @param[in] channelPtr Channel pointer
    /// @param[in] metaInfo Meta information mapping
    /// @param[in] priority Priority
    /// @param[in] confirmCallback Confirmation callback function
    /// @param[in] saveCallback Save callback function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01968
    /// @needwork = dd
    /// @endcode
    RequestContext(ServerImpl& impl,
                   MessagePtr request,
                   Address serverAddress,
                   std::vector< Address > functionalAddress,
                   std::shared_ptr< Channel > channelPtr,
                   MetaInfoMap metaInfo,
                   Priority priority,
                   RequestContextConfirmCallback confirmCallback,
                   RequestContextSaveCallback saveCallback) noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_02002
    /// @needwork = dd
    /// @endcode
    ~RequestContext() noexcept = default;

    /// @brief Start p2 timer
    /// @param[in] p2ServerMax 2P server maximum timeout
    /// @param[in] p2StarServerMax 2P star server maximum timeout
    /// @param[in] maxNumberOfRequestCorrectlyReceivedResponsePending Maximum number of correctly received response pending requests
    /// @return Return code
    std::int32_t StartP2ServerTimer(std::uint32_t p2ServerMax,
                                    std::uint32_t p2StarServerMax,
                                    std::uint8_t maxNumberOfRequestCorrectlyReceivedResponsePending) noexcept;

    /// @brief Stop p2 timer
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t StopP2ServerTimer() noexcept;

    /// @brief Notify
    /// @param const Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void NotifyRequestContextState(NotifyRequestContextStateCallback const& callback) noexcept;

    /// @brief Get original request message
    /// @return Original request message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    inline MessagePtr& GetRequest() noexcept { return request_; }

    /// @brief Set Sid
    /// @param[in] sid Unsigned integer
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void SetSid(std::uint8_t sid) noexcept { sid_ = sid; }

    /// @brief Get Sid
    /// @return Unsigned integer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t GetSid() const noexcept { return sid_; }

    /// @brief Get Priority
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Priority GetPriority() const noexcept { return priority_; }

    /// @return Original response message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    MessagePtr CreateResponse() noexcept;

    /// @param[in] nrc Error code
    /// @return Original response message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    MessagePtr CreateResponse(std::uint8_t const nrc) noexcept;

    /// @param[in] nrc Error code
    /// @return Original response message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    MessagePtr CreateResponse(NrcErrc const nrc) noexcept;

    /// @brief Respond function
    /// @param[in] nrc Error code value
    /// @param[in] ignoreServiceConfirm Whether to ignore service confirmation callback
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Respond(std::uint8_t const nrc, bool ignoreServiceConfirm = false) noexcept;

    /// @brief Respond function
    /// @param[in] nrc Error code value
    /// @param[in] ignoreServiceConfirm Whether to ignore service confirmation callback
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Respond(NrcErrc const nrc, bool ignoreServiceConfirm = false) noexcept;

    /// @brief Respond function
    /// @param[in] msgPtr
    /// @param[in] handler Processing result callback
    /// @param[in] ignoreServiceConfirm Whether to ignore service confirmation callback
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Respond(MessagePtr&& msgPtr, bool ignoreServiceConfirm = false) noexcept;

    /// @brief Respond function
    /// @param[in] msgPtrTable Message pointer table
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Respond(std::vector< MessagePtr >& msgPtrTable) noexcept;

    /// @brief Cancel call
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Cancelled() noexcept;

    /// @brief Whether the call is cancelled
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool IsCancelled() noexcept;

    /// @brief Get meta information mapping
    /// @return Meta information mapping
    /// @brief Get meta information mapping
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    MetaInfoMap GetMetaInfoMap() noexcept { return metaInfo_; }

    /// @brief ReestablishAfterRestarted function
    /// @param const Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool ReestablishAfterRestarted(uint16_t const ta) noexcept;

    /// @brief Get cancellation status
    /// @return Cancellation status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< CancellationState > GetCancellationState() noexcept { return cancellationState_; }

    /// @brief Save request context
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Save() noexcept;

    /// @brief End request context
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void End() noexcept;

    /// @brief Get MaxPayloadLength
    /// @return Size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::size_t GetMaxPayloadLength() noexcept;

private:
    /// @brief _suppressNegtive function
    /// @param const Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool _suppressNegtive(std::uint8_t const nrc) noexcept;
    /// @brief _suppressPositive function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool _suppressPositive() noexcept;
    /// @brief _isNegtiveType function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool _isNegtiveType(std::shared_ptr< Message > const& msgPtr) noexcept;
    /// @brief _changeState function
    /// @param state Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _changeState(RequestContextState state) noexcept;

    /// @brief _isFunctionalAddress function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool _isFunctionalAddress() const noexcept;

    /// @brief _timeout function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _timeout() noexcept;

    /// @brief _notify function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _notify() noexcept;

    /// @brief _p4Timeout function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _p4Timeout() noexcept;

    /// @brief _startP4ServerMax function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _startP4ServerMax() noexcept;

    /// @brief _stopP4ServerMax function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _stopP4ServerMax() noexcept;

private:
    /// @brief Server implementation reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    ServerImpl& impl_;

    /// @brief Request pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    MessagePtr request_{nullptr};

    /// @brief Server address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Address serverAddress_{0U};

    /// @brief Functional address vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::vector< Address > functionalAddress_{};

    /// @brief Service ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t sid_{0U};

    /// @brief Channel pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Channel > channelPtr_{nullptr};

    /// @brief Meta information mapping
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    MetaInfoMap metaInfo_{};

    /// @brief Priority
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Priority priority_{0U};

    /// @brief Request context status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00835
    /// @needwork = dda
    /// @endcode
    RequestContextState state_{RequestContextState::kRequest};

    /// @brief Notification callback table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::vector< NotifyRequestContextStateCallback > notifyCallbackTable_{};

    /// @brief Confirmation callback function pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    RequestContextConfirmCallback confirmCallback_{nullptr};

    /// @brief Save callback function pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_
    /// @trace_id_dd=DD_UDS_00836
    /// @needwork = dd
    /// @endcode
    RequestContextSaveCallback saveCallback_{nullptr};

    /// @brief P2 timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Timer p2Timer_{};

    /// @brief P2 timer maximum value, unit milliseconds
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint32_t p2ServerMax_{0U};

    /// @brief P2Star timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint32_t p2StarServerMax_{0U};

    /// @brief P4 timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Timer p4Timer_{};

    /// @brief P4 timer maximum value, unit milliseconds
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint32_t p4ServerMax_{0U};

    /// @brief Maximum number of response pending
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t maxNumberOfRequestCorrectlyReceivedResponsePending_{0U};

    /// @brief Current number of response pending
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t numberOfRequestCorrectlyReceivedResponsePending_{0U};

    /// @brief Cancellation state pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @interface_level=unit
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_
    /// @trace_id_dd=DD_UDS_00837
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< CancellationState > cancellationState_{nullptr};

    /// @brief Mutex lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00838
    /// @needwork = dda
    /// @endcode
    std::mutex lock_{};
    /// @brief Whether it is pending state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00839
    /// @needwork = dda
    /// @endcode
    bool isPending_{false};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_REQUEST_CONTEXT_H_