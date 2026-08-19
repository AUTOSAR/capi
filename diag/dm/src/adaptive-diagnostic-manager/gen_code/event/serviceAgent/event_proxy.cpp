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
/// @file       event_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-17
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "event_proxy.h"

#include "netProxy/net_proxy.h"
#include "netProxy/rpc_define.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dis {

constexpr uint8_t kFuncIDNotifyEventStatus{1U};
constexpr uint8_t kFuncIDGetEventStatus{2U};
constexpr uint8_t kFuncIDGetLatchedWIRStatus{3U};
constexpr uint8_t kFuncIDSetLatchedWIRStatus{4U};
constexpr uint8_t kFuncIDGetDTCNumber{5U};
constexpr uint8_t kFuncIDGetDebouncingStatus{6U};
constexpr uint8_t kFuncIDGetFaultDetectionCounter{7U};
constexpr uint8_t kFuncIDGetTestComplete{8U};

EventProxy::EventProxy(uint32_t const &serviceInstanceId) : BussinessServiceProxy{serviceInstanceId}
{
    NetProxy::GetInstance().GetNetAccess()->RegisterService(
        serviceInstanceId, [this](CallRequest &&callData) { CallHandle(std::move(callData)); });
}

void EventProxy::CallHandle(CallRequest &&callData)
{
    CallRequest cr{std::move(callData)};

    switch (cr.funcData.funcId) {
        case kFuncIDNotifyEventStatus: {
            Subscribe(std::move(cr));
            break;
        }
        case kFuncIDGetEventStatus: {
            _getEventStatus(std::move(cr));
            break;
        }
        case kFuncIDGetLatchedWIRStatus: {
            _getLatchedWIRStatus(std::move(cr));
            break;
        }
        case kFuncIDSetLatchedWIRStatus: {
            _setLatchedWIRStatus(std::move(cr));
            break;
        }
        case kFuncIDGetDTCNumber: {
            _getDTCNumber(std::move(cr));
            break;
        }
        case kFuncIDGetDebouncingStatus: {
            _getDebouncingStatus(std::move(cr));
            break;
        }
        case kFuncIDGetFaultDetectionCounter: {
            _getFaultDetectionCounter(std::move(cr));
            break;
        }
        case kFuncIDGetTestComplete: {
            _getTestComplete(std::move(cr));
            break;
        }
        default: {
        } break;
    }
}

/// @brief Notify AA of event status change
/// @param status Event status
/// @return
void EventProxy::NotifyEventStatus(uint8_t const status)
{
    std::vector< uint8_t > payLoad;
    if (serialize::Serialize(payLoad, status) < 0) {
        return;
    }
    Notify(kFuncIDNotifyEventStatus, std::move(payLoad));
}

/// @brief Register callback for AA side to get event status
/// @param callBack Callback function to be registered
/// @return
void EventProxy::RegisterGetEventStatus(std::function< std::uint8_t() > const &callBack)
{
    getEventStatusCb_ = callBack;
}

/// @brief Register callback for AA side to get WIR status
/// @param callBack Callback function to be registered
/// @return
void EventProxy::RegisterGetLatchedWIRStatus(std::function< bool() > const &callBack)
{
    getLatchedWIRStatusCb_ = callBack;
}

/// @brief Register callback for AA side to set WIR status
/// @param callBack Callback function to be registered
/// @return
void EventProxy::RegisterSetLatchedWIRStatus(std::function< int32_t(bool const) > const &callBack)
{
    setLatchedWIRStatusCb_ = callBack;
}

/// @brief Register callback for AA side to get DTC number
/// @param callBack Callback function to be registered
/// @return
void EventProxy::RegisterGetDTCNumber(std::function< std::int32_t(std::uint8_t, std::uint32_t &) > const &callBack)
{
    getDTCNumberCb_ = callBack;
}

/// @brief Register callback for AA side to get debounce status
/// @param callBack Callback function to be registered
/// @return
void EventProxy::RegisterGetDebouncingStatus(std::function< uint8_t() > const &callBack)
{
    getDebouncingStatusCb_ = callBack;
}

/// @brief Register callback for AA side to get FDC
/// @param callBack Callback function to be registered
/// @return
void EventProxy::RegisterGetFaultDetectionCounter(std::function< std::int8_t() > const &callBack)
{
    getFaultDetectionCounterCb_ = callBack;
}

/// @brief Register callback for AA side to get event test status
/// @param callBack Callback function to be registered
/// @return
void EventProxy::RegisterGetTestComplete(std::function< bool() > const &callBack) { getTestCompleteCb_ = callBack; }

void EventProxy::_getEventStatus(CallRequest &&callData)
{
    if (getEventStatusCb_ == nullptr) {
        return;
    }
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};

    uint8_t const status = getEventStatusCb_();
    if (serialize::Serialize(buf, status) > 0) {
        retData.retData = std::move(buf);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataSerialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void EventProxy::_getLatchedWIRStatus(CallRequest &&callData)
{
    if (getLatchedWIRStatusCb_ == nullptr) {
        return;
    }
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};

    bool const status = getLatchedWIRStatusCb_();
    if (serialize::Serialize(buf, status) > 0) {
        retData.retData = std::move(buf);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataSerialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void EventProxy::_setLatchedWIRStatus(CallRequest &&callData)
{
    if (setLatchedWIRStatusCb_ == nullptr) {
        return;
    }
    bool status{};
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};
    if (serialize::Deserialize(cr.funcData.data, status) > 0) {
        uint32_t const res = setLatchedWIRStatusCb_(status);
        if (res != 0) {
            retData.errorCode = static_cast< int32_t >(res);
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void EventProxy::_getDTCNumber(CallRequest &&callData)
{
    if (getDTCNumberCb_ == nullptr) {
        return;
    }
    uint8_t dtcFormat{};
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};
    if (serialize::Deserialize(cr.funcData.data, dtcFormat) > 0) {
        uint32_t dtcNumber{};
        uint32_t const res = getDTCNumberCb_(dtcFormat, dtcNumber);
        if (res != 0) {
            retData.errorCode = static_cast< int32_t >(res);
        } else {
            if (serialize::Serialize(buf, dtcNumber) > 0) {
                retData.retData = std::move(buf);
            } else {
                retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataSerialize);
            }
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void EventProxy::_getDebouncingStatus(CallRequest &&callData)
{
    if (getDebouncingStatusCb_ == nullptr) {
        return;
    }
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};

    uint8_t const status = getDebouncingStatusCb_();
    if (serialize::Serialize(buf, status) > 0) {
        retData.retData = std::move(buf);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataSerialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void EventProxy::_getFaultDetectionCounter(CallRequest &&callData)
{
    if (getFaultDetectionCounterCb_ == nullptr) {
        return;
    }
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};

    int8_t const counter = getFaultDetectionCounterCb_();
    if (serialize::Serialize(buf, counter) > 0) {
        retData.retData = std::move(buf);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataSerialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void EventProxy::_getTestComplete(CallRequest &&callData)
{
    if (getTestCompleteCb_ == nullptr) {
        return;
    }
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};

    bool const testRes = getTestCompleteCb_();
    if (serialize::Serialize(buf, testRes) > 0) {
        retData.retData = std::move(buf);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataSerialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft