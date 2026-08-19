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
/// @file       dtc_information_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-17
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "dtc_information_proxy.h"

#include "netProxy/net_proxy.h"
#include "netProxy/rpc_define.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dis {

constexpr uint8_t kFuncIDGetCurrentStatus{1U};
constexpr uint8_t kFuncIDSetDTCStatusChangedNotifier{2U};
constexpr uint8_t kFuncIDSetSnapshotRecordUpdatedNotifier{3U};
constexpr uint8_t kFuncIDGetNumberOfStoredEntries{4U};
constexpr uint8_t kFuncIDSetNumberOfStoredEntriesNotifier{5U};
constexpr uint8_t kFuncIDGetEventMemoryOverflow{6U};
constexpr uint8_t kFuncIDSetEventMemoryOverflowNotifier{7U};
constexpr uint8_t kFuncIDClear{8U};
constexpr uint8_t kFuncIDGetControlDTCStatus{9U};
constexpr uint8_t kFuncIDSetControlDtcStatusNotifier{10U};
constexpr uint8_t kFuncIDEnableControlDtc{11U};
constexpr uint8_t kFuncIDGetDtcSuppression{12U};
constexpr uint8_t kFuncIDSetDtcSuppression{13U};
DtcInformationProxy::DtcInformationProxy(uint32_t const &serviceInstanceId) : BussinessServiceProxy{serviceInstanceId}
{
    NetProxy::GetInstance().GetNetAccess()->RegisterService(
        serviceInstanceId, [this](CallRequest &&callData) { CallHandle(std::move(callData)); });
}

void DtcInformationProxy::CallHandle(CallRequest &&callData)
{
    CallRequest cr{std::move(callData)};
    switch (cr.funcData.funcId) {
        case kFuncIDGetCurrentStatus: {
            _getCurrentStatus(std::move(cr));
            break;
        }
        case kFuncIDGetNumberOfStoredEntries: {
            _getNumberOfStoredEntries(std::move(cr));
            break;
        }
        case kFuncIDGetEventMemoryOverflow: {
            _getEventMemoryOverflow(std::move(cr));
            break;
        }
        case kFuncIDClear: {
            _clear(std::move(cr));
            break;
        }
        case kFuncIDGetControlDTCStatus: {
            _getControlDTCStatus(std::move(cr));
            break;
        }
        case kFuncIDEnableControlDtc: {
            _enableControlDtc(std::move(cr));
            break;
        }
        case kFuncIDGetDtcSuppression: {
            _getDtcSuppression(std::move(cr));
            break;
        }
        case kFuncIDSetDtcSuppression: {
            _setDtcSuppression(std::move(cr));
            break;
        }
        case kFuncIDSetDTCStatusChangedNotifier:
        case kFuncIDSetSnapshotRecordUpdatedNotifier:
        case kFuncIDSetNumberOfStoredEntriesNotifier:
        case kFuncIDSetEventMemoryOverflowNotifier:
        case kFuncIDSetControlDtcStatusNotifier: {
            Subscribe(std::move(cr));
            break;
        }
        default: {
        } break;
    }
}

/// @brief Notify DTC status change
/// @param dtcStatus DTC status
/// @return
void DtcInformationProxy::NotifyDTCStatusChanged(std::uint32_t dtc,
                                                 std::uint8_t udsStatusByteOld,
                                                 std::uint8_t udsStatusByteNew)
{
    std::vector< uint8_t > payLoad;
    if (serialize::Serialize(payLoad, dtc, udsStatusByteOld, udsStatusByteNew) < 0) {
        return;
    }
    Notify(kFuncIDSetDTCStatusChangedNotifier, std::move(payLoad));
}

/// @brief Notify snapshot data change
/// @param dtcSnapshot Snapshot
/// @return
void DtcInformationProxy::NotifySnapshotRecordUpdated(SnapshotRecordUpdatedType const &dtcSnapshot)
{
    std::vector< uint8_t > payLoad;
    if (serialize::Serialize(payLoad, dtcSnapshot) < 0) {
        return;
    }
    Notify(kFuncIDSetSnapshotRecordUpdatedNotifier, std::move(payLoad));
}

/// @brief Notify the change of the number of entities stored in the database
/// @param numberOfStoredEntries Number of entities
/// @return
void DtcInformationProxy::NotifyNumberOfStoredEntries(std::uint32_t const numberOfStoredEntries)
{
    std::vector< uint8_t > payLoad;
    if (serialize::Serialize(payLoad, numberOfStoredEntries) < 0) {
        return;
    }
    Notify(kFuncIDSetNumberOfStoredEntriesNotifier, std::move(payLoad));
}

/// @brief Notify whether the database overflows
/// @param isOverflow Overflow flag
/// @return
void DtcInformationProxy::NotifyEventMemoryOverflow(bool const isOverflow)
{
    std::vector< uint8_t > payLoad;
    if (serialize::Serialize(payLoad, isOverflow) < 0) {
        return;
    }
    Notify(kFuncIDSetEventMemoryOverflowNotifier, std::move(payLoad));
}

/// @brief Notify the change of the DTC control flag
/// @param dtcStatusType Flag
/// @return
void DtcInformationProxy::NotifyControlDtcStatus(uint8_t const dtcStatusType)
{
    std::vector< uint8_t > payLoad;
    if (serialize::Serialize(payLoad, dtcStatusType) < 0) {
        return;
    }
    Notify(kFuncIDSetControlDtcStatusNotifier, std::move(payLoad));
}

/// @brief Register callback for AA side to get the number of stored entities in the database
/// @param callBack Callback function to be registered
/// @return
void DtcInformationProxy::RegisterGetNumberOfStoredEntries(std::function< std::uint32_t() > const &callBack)
{
    getNumberOfStoredEntriesCb_ = callBack;
}

/// @brief Register callback for AA side to clear DTC
/// @param callBack Callback function to be registered
/// @return
void DtcInformationProxy::RegisterClear(std::function< int32_t(std::uint32_t const) > const &callBack)
{
    clearCb_ = callBack;
}

/// @brief Register callback for AA side to get DTC control status
/// @param callBack Callback function to be registered
/// @return
void DtcInformationProxy::RegisterGetControlDTCStatus(std::function< std::uint8_t() > const &callBack)
{
    getControlDTCStatusCb_ = callBack;
}

/// @brief Register callback for AA side to restore DTC status change
/// @param callBack Callback function to be registered
/// @return
void DtcInformationProxy::RegisterEnableControlDtc(std::function< std::int32_t() > const &callBack)
{
    enableControlDtcCb_ = callBack;
}

/// @brief Register callback for AA side to get current DTC status code
/// @param callBack Callback function to be registered
/// @return
void DtcInformationProxy::RegisterGetCurrentStatus(
    std::function< std::int32_t(std::uint32_t const, uint8_t &) > const &callBack)
{
    getCurrentStatusCb_ = callBack;
}

/// @brief Register callback for AA side to get database overflow flag
/// @param callBack Callback function to be registered
/// @return
void DtcInformationProxy::RegisterGetEventMemoryOverflow(std::function< bool() > const &callBack)
{
    getEventMemoryOverflowCb_ = callBack;
}
/// @brief Register callback to get DTC suppression status
/// @param callback Callback function to be registered
void DtcInformationProxy::RegisterGetDtcSuppression(
    std::function< std::int32_t(std::uint32_t, std::uint8_t &) > const &callback) noexcept
{
    getDtcSuppressionCb_ = callback;
}

/// @brief Register callback to set DTC suppression status
/// @param callback Callback function to be registered
void DtcInformationProxy::RegisterSetDtcSuppression(
    std::function< std::int32_t(std::uint32_t, std::uint8_t) > const &callback) noexcept
{
    setDtcSuppressionCb_ = callback;
}

void DtcInformationProxy::_getNumberOfStoredEntries(CallRequest &&callData)
{
    if (getNumberOfStoredEntriesCb_ == nullptr) {
        return;
    }
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};

    uint32_t const count = getNumberOfStoredEntriesCb_();
    if (serialize::Serialize(buf, count) > 0) {
        retData.retData = std::move(buf);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void DtcInformationProxy::_clear(CallRequest &&callData)
{
    uint32_t dtc{};
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};
    if (serialize::Deserialize(cr.funcData.data, dtc) > 0) {
        retData.errorCode = clearCb_(dtc);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void DtcInformationProxy::_getControlDTCStatus(CallRequest &&callData)
{
    if (getControlDTCStatusCb_ == nullptr) {
        return;
    }
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};

    uint8_t const status = getControlDTCStatusCb_();
    if (serialize::Serialize(buf, status) > 0) {
        retData.retData = std::move(buf);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void DtcInformationProxy::_enableControlDtc(CallRequest &&callData)
{
    if (enableControlDtcCb_ == nullptr) {
        return;
    }
    RetData retData;
    CallRequest const cr{std::move(callData)};
    retData.errorCode = enableControlDtcCb_();
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void DtcInformationProxy::_getCurrentStatus(CallRequest &&callData)
{
    if (getCurrentStatusCb_ == nullptr) {
        return;
    }
    uint32_t dtc{};
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};
    if (serialize::Deserialize(cr.funcData.data, dtc) > 0) {
        uint8_t status{};
        int32_t const error = getCurrentStatusCb_(dtc, status);
        if (error == 0) {
            if (serialize::Serialize(buf, status) > 0) {
                retData.retData = std::move(buf);
            } else {
                retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataSerialize);
            }
        } else {
            retData.errorCode = error;
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void DtcInformationProxy::_getEventMemoryOverflow(CallRequest &&callData)
{
    if (getEventMemoryOverflowCb_ == nullptr) {
        return;
    }
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};

    uint8_t const status = static_cast< uint8_t >(getEventMemoryOverflowCb_());
    if (serialize::Serialize(buf, status) > 0) {
        retData.retData = std::move(buf);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataSerialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void DtcInformationProxy::_getDtcSuppression(CallRequest &&callData)
{
    if (getDtcSuppressionCb_ == nullptr) {
        return;
    }
    uint32_t dtc{};
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};
    if (serialize::Deserialize(cr.funcData.data, dtc) > 0) {
        uint8_t status{};
        int32_t const error = getDtcSuppressionCb_(dtc, status);
        if (error == 0) {
            if (serialize::Serialize(buf, status) > 0) {
                retData.retData = std::move(buf);
            } else {
                retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataSerialize);
            }
        } else {
            retData.errorCode = error;
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}
void DtcInformationProxy::_setDtcSuppression(CallRequest &&callData)
{
    if (setDtcSuppressionCb_ == nullptr) {
        return;
    }
    uint32_t dtc{};
    uint8_t suppressionStatus{};
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};
    if (serialize::Deserialize(cr.funcData.data, dtc, suppressionStatus) > 0) {
        int32_t const error = setDtcSuppressionCb_(dtc, suppressionStatus);
        if (error != 0) {
            retData.errorCode = error;
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}
}  // namespace dis
}  // namespace dm
}  // namespace isoft