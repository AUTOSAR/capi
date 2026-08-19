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
/// @file       bussiness_client_proxy.cpp
/// @brief
/// @details
/// @date       2024-11-28
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#include "netProxy/bussiness_client_proxy.h"

#include <future>

#include "ara/diag/diag_error_domain.h"
#include "log/log.h"
#include "serialization/serialization/common_data_type.h"
namespace isoft {
namespace dm {
namespace dic {

using ara::diag::DiagErrc;
using ara::diag::common::LogError;

ara::core::Result< void > BussinessClientProxy::RegisterCallBack(uint8_t const &funcId, uint8_t &callId) const
{
    // Encapsulate function data
    FuncData funcData;
    funcData.funcId   = funcId;
    funcData.funcType = FuncType::kFuncType_call;
    funcData.callId   = callId;
    ++callId;

    // send data
    std::future< std::vector< uint8_t > > futureRet{
        NetProxy::GetInstance().GetNetAccess()->CallFunc(mServiceInstanceId_, mClientInstanceId_, std::move(funcData))};
    std::vector< uint8_t > const result{futureRet.get()};

    if (result.empty()) {
        return ara::core::Result< void >::FromError(DiagErrc::kNotOffered);
    }

    // parse data
    RetData retData;
    int32_t nLen = serialize::Deserialize(result, retData);
    if (nLen < 0) {
        LogError() << "deserialize error : " << nLen;
        return ara::core::Result< void >::FromError(DiagErrc::kGenericError);
    }
    if (retData.errorCode != 0U) {
        return ara::core::Result< void >::FromError(static_cast< DiagErrc >(retData.errorCode));
    }

    // Function call logic, save callback function after success
    return ara::core::Result< void >{};
}

void BussinessClientProxy::RegisterOnReady(std::function< void(bool) > const &cb) const  //NOLINT
{
    NetProxy::GetInstance().GetNetAccess()->RegisterOnReady(cb);
}

void BussinessClientProxy::RegisterOnServiceReady(std::function< void(bool) > const &cb) const
{
    NetProxy::GetInstance().GetNetAccess()->RegisterOnServiceReady(mServiceInstanceId_, cb);
}

void BussinessClientProxy::RegisterNotificationCallBack(NotifyCallBack &&notify) const
{
    NetProxy::GetInstance().GetNetAccess()->RegisterNotificationCallBack(mClientInstanceId_, std::move(notify));
}

bool BussinessClientProxy::IsServiceReady() const
{
    return NetProxy::GetInstance().GetNetAccess()->IsServiceReady(mServiceInstanceId_);
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft