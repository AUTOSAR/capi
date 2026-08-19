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
/// @file       bussiness_client_proxy.h
/// @brief
/// @details
/// @date       2024-11-28
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#ifndef __BUSSINESS_CLIENT_PROXY__
#define __BUSSINESS_CLIENT_PROXY__

#include <mutex>
#include <set>
#include <unordered_map>

#include "ara/core/future.h"
#include "ara/core/result.h"
#include "ara/diag/diag_error_domain.h"
#include "cancellation/cancellation_event.h"
#include "net_proxy.h"
#include "rpc_define.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dic {

class BussinessClientProxy
{
public:
    explicit BussinessClientProxy(uint32_t serviceInstanceId, uint16_t clientInstanceId)
        : mServiceInstanceId_{serviceInstanceId}, mClientInstanceId_{clientInstanceId}
    {
    }
    virtual ~BussinessClientProxy()                          = default;
    BussinessClientProxy(const BussinessClientProxy& other)  = delete;
    BussinessClientProxy(const BussinessClientProxy&& other) = delete;
    BussinessClientProxy& operator=(const BussinessClientProxy& other) = delete;
    BussinessClientProxy& operator=(BussinessClientProxy const&& other) = delete;

    ara::core::Result< void > RegisterCallBack(uint8_t const& funcId, uint8_t& callId) const;
    void RegisterOnReady(std::function< void(bool) > const& cb) const;
    void RegisterOnServiceReady(std::function< void(bool) > const& cb) const;
    void RegisterNotificationCallBack(NotifyCallBack&& notify) const;
    bool IsServiceReady() const;

    template < typename T, typename... Args, typename std::enable_if< !std::is_void< T >::value, int >::type = 0 >
    ara::core::Result< T > SyncCallFunc(uint8_t const& funcId, uint8_t& callId, Args... args) const
    {
        if (!IsServiceReady()) {
            return std::move(
                ara::core::Result< T >::FromError(static_cast< ara::diag::DiagErrc >(InnerErrorCode::kServiceNoFound)));
        }

        // Encapsulate
        FuncData funcData;
        funcData.funcId   = funcId;
        funcData.funcType = FuncType::kFuncType_call;
        ++callId;
        funcData.callId = callId;
        std::ignore     = serialize::Serialize(funcData.data, std::forward< Args >(args)...);

        // send data
        std::future< std::vector< uint8_t > > futureRet{NetProxy::GetInstance().GetNetAccess()->CallFunc(
            mServiceInstanceId_, mClientInstanceId_, std::move(funcData))};
        std::vector< uint8_t > const result{futureRet.get()};

        // parse data
        RetData retData;
        if (serialize::Deserialize(result, retData) > 0) {
            if (retData.errorCode != 0U) {
                return std::move(
                    ara::core::Result< T >::FromError(static_cast< ara::diag::DiagErrc >(retData.errorCode)));
            }
            T val;
            if (serialize::Deserialize(retData.retData, val) > 0) {
                T const status{val};
                return std::move(ara::core::Result< T >::FromValue(status));
            }
        }
        return std::move(
            ara::core::Result< T >::FromError(static_cast< ara::diag::DiagErrc >(InnerErrorCode::kServiceNoFound)));
    }

    template < typename T, typename... Args, typename std::enable_if< std::is_void< T >::value, int >::type = 0 >
    ara::core::Result< T > SyncCallFunc(uint8_t const& funcId, uint8_t& callId, Args... args) const
    {
        if (!IsServiceReady()) {
            return std::move(
                ara::core::Result< T >::FromError(static_cast< ara::diag::DiagErrc >(InnerErrorCode::kServiceNoFound)));
        }

        // Encapsulate
        FuncData funcData;
        funcData.funcId   = funcId;
        funcData.funcType = FuncType::kFuncType_call;
        ++callId;
        funcData.callId = callId;
        std::ignore     = serialize::Serialize(funcData.data, std::forward< Args >(args)...);

        // send data
        std::future< std::vector< uint8_t > > futureRet{NetProxy::GetInstance().GetNetAccess()->CallFunc(
            mServiceInstanceId_, mClientInstanceId_, std::move(funcData))};
        std::vector< uint8_t > const result{futureRet.get()};

        // parse data
        RetData retData;
        if (serialize::Deserialize(result, retData) > 0) {
            if (retData.errorCode != 0U) {
                return std::move(
                    ara::core::Result< T >::FromError(static_cast< ara::diag::DiagErrc >(retData.errorCode)));
            }
        }
        return {};
    }

    template < typename T, typename... Args, typename std::enable_if< !std::is_void< T >::value, int >::type = 0 >
    ara::core::Future< T > AsyncCallFunc(uint8_t const& funcId, uint8_t& callId, Args... args) const
    {
        std::shared_ptr< ara::core::Promise< T > > ret{std::move(std::make_shared< ara::core::Promise< T > >())};
        if (!IsServiceReady()) {
            ret->SetError(static_cast< ara::diag::DiagErrc >(InnerErrorCode::kServiceNoFound));
            return ret->get_future();
        }
        // Encapsulate
        FuncData funcData;
        funcData.funcId   = funcId;
        funcData.funcType = FuncType::kFuncType_call;
        ++callId;
        funcData.callId = callId;
        std::ignore     = serialize::Serialize(funcData.data, std::forward< Args >(args)...);
        // send data
        NetProxy::GetInstance().GetNetAccess()->CallFunc(
            mServiceInstanceId_, mClientInstanceId_, std::move(funcData),
            std::move([ret](std::vector< uint8_t > const& result) {
                // parse data
                RetData retData;
                if (serialize::Deserialize(result, retData) > 0) {
                    if (retData.errorCode != 0U) {
                        ret->SetError(static_cast< ara::diag::DiagErrc >(retData.errorCode));
                    } else {
                        T val;
                        if (serialize::Deserialize(retData.retData, val) > 0) {
                            ret->set_value(val);
                        }
                    }
                }
            }));
        return std::move(ret->get_future());
    }

    template < typename T, typename... Args, typename std::enable_if< std::is_void< T >::value, int >::type = 0 >
    ara::core::Future< void > AsyncCallFunc(uint8_t const& funcId, uint8_t& callId, Args... args) const
    {
        std::shared_ptr< ara::core::Promise< void > > ret{std::make_shared< ara::core::Promise< void > >()};
        if (!IsServiceReady()) {
            ret->SetError(static_cast< ara::diag::DiagErrc >(InnerErrorCode::kServiceNoFound));
            return ret->get_future();
        }
        // Encapsulate
        FuncData funcData;
        funcData.funcId   = funcId;
        funcData.funcType = FuncType::kFuncType_call;
        ++callId;
        funcData.callId = callId;
        std::ignore     = serialize::Serialize(funcData.data, std::forward< Args >(args)...);
        // send data
        NetProxy::GetInstance().GetNetAccess()->CallFunc(
            mServiceInstanceId_, mClientInstanceId_, std::move(funcData),
            std::move([ret](std::vector< uint8_t > const& result) {
                // parse data
                RetData retData;
                if (serialize::Deserialize(result, retData) > 0) {
                    if (retData.errorCode != 0U) {
                        ret->SetError(static_cast< ara::diag::DiagErrc >(retData.errorCode));
                    } else {
                        ret->set_value();
                    }
                }
            }));
        return ret->get_future();
    }

    template < typename T, typename... Args, typename std::enable_if< !std::is_void< T >::value, int >::type = 0 >
    ara::core::Future< T > AsyncCallFunc(uint8_t const& funcId,
                                         uint8_t& callId,
                                         std::shared_ptr< CancellationEvent > const& cancelEventPtr,
                                         Args... args) const
    {
        std::shared_ptr< ara::core::Promise< T > > ret{std::move(std::make_shared< ara::core::Promise< T > >())};
        if (!IsServiceReady()) {
            ret->SetError(static_cast< ara::diag::DiagErrc >(InnerErrorCode::kServiceNoFound));
            return ret->get_future();
        }
        // Encapsulate
        FuncData funcData;
        funcData.funcId   = funcId;
        funcData.funcType = FuncType::kFuncType_call;
        ++callId;
        funcData.callId = callId;
        std::ignore     = serialize::Serialize(funcData.data, std::forward< Args >(args)...);
        std::shared_ptr< FuncData > funcDataPtr{std::make_shared< FuncData >()};
        *funcDataPtr = funcData;
        // set cancel callback
        cancelEventPtr->SetNotifier([this, funcDataPtr]() mutable {
            NetProxy::GetInstance().GetNetAccess()->CancelFunctionCall(mServiceInstanceId_, mClientInstanceId_,
                                                                       std::move(*funcDataPtr));
        });
        // send data
        NetProxy::GetInstance().GetNetAccess()->CallFunc(
            mServiceInstanceId_, mClientInstanceId_, std::move(funcData),
            std::move([ret](std::vector< uint8_t > const& result) {
                // parse data
                RetData retData;

                int32_t nLen = serialize::Deserialize(result, retData);
                if (nLen > 0) {
                    if (retData.errorCode != 0U) {
                        ret->SetError(static_cast< ara::diag::DiagErrc >(retData.errorCode));
                    } else {
                        T val;
                        nLen = serialize::Deserialize(retData.retData, val);
                        if (nLen > 0) {
                            ret->set_value(val);
                        }
                    }
                    return;
                }
                ret->SetError(static_cast< ara::diag::DiagErrc >(nLen));
            }));
        return std::move(ret->get_future());
    }

    template < typename T, typename... Args, typename std::enable_if< std::is_void< T >::value, int >::type = 0 >
    ara::core::Future< void > AsyncCallFunc(uint8_t const& funcId,
                                            uint8_t& callId,
                                            std::shared_ptr< CancellationEvent > const& cancelEventPtr,
                                            Args... args) const
    {
        std::shared_ptr< ara::core::Promise< void > > ret{std::make_shared< ara::core::Promise< void > >()};
        if (!IsServiceReady()) {
            ret->SetError(static_cast< ara::diag::DiagErrc >(InnerErrorCode::kServiceNoFound));
            return ret->get_future();
        }
        // Encapsulate
        FuncData funcData;
        funcData.funcId   = funcId;
        funcData.funcType = FuncType::kFuncType_call;
        ++callId;
        funcData.callId = callId;
        std::ignore     = serialize::Serialize(funcData.data, std::forward< Args >(args)...);
        // set cancel callback
        cancelEventPtr->SetNotifier([this, funcData]() mutable {
            NetProxy::GetInstance().GetNetAccess()->CancelFunctionCall(mServiceInstanceId_, mClientInstanceId_,
                                                                       std::move(funcData));
        });
        // send data
        NetProxy::GetInstance().GetNetAccess()->CallFunc(
            mServiceInstanceId_, mClientInstanceId_, std::move(funcData),
            std::move([ret](std::vector< uint8_t > const& result) {
                // parse data
                RetData retData;
                int32_t nLen = serialize::Deserialize(result, retData);
                if (nLen > 0) {
                    if (retData.errorCode != 0U) {
                        ret->SetError(static_cast< ara::diag::DiagErrc >(retData.errorCode));
                    } else {
                        ret->set_value();
                    }
                } else {
                    ret->SetError(static_cast< ara::diag::DiagErrc >(nLen));
                }
            }));
        return ret->get_future();
    }

protected:
    uint32_t mServiceInstanceId_{0};  //NOLINT
    uint16_t mClientInstanceId_{0};   //NOLINT
};

}  // namespace dic
}  // namespace dm
}  // namespace isoft

#endif  // __BUSSINESS_CLIENT_PROXY__
