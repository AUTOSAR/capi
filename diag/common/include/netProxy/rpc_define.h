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
/// @file       rpc_define.h
/// @brief
/// @details
/// @date       2024-11-28
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#ifndef _RPC_DEFINE_H_
#define _RPC_DEFINE_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "serialization/serialization/common_data_type.h"

namespace isoft {
namespace dm {

enum class FuncType : uint8_t
{
    kFuncType_call = 0,
    kFuncType_result,
    kFuncType_notify,
    kFuncType_cancel,
    kFuncType_syncInfo
};

// Transport and network underlying error code, use negative values [-128, -1]
enum class InnerErrorCode : int32_t
{
    kConnectFailed = -128,
    kServiceNoOffer,
    kServiceNoFound,
    kFuncCallTimeout,
    kDataSerialize,
    kDataDeserialize,
    kExit
};

struct RetData
{
    int32_t errorCode{0};  // [== 0]: no error; [>0]: bussiness error code; [<0]:
                           // communication error code
    std::vector< uint8_t > retData{};
    std::vector< uint8_t >
        paramData{};  // Organize output parameters according to parameter structure (for code generation?), or integrate output parameter data into the returned data?

    using IsEnumerableTag = void;
    template < typename F >
    void enumerate(F &func)  // NOLINT
    {
        func(errorCode);
        func(retData);
        func(paramData);
    }
};

struct FuncData
{
    uint8_t callId;  // Identify a call
    uint8_t funcId;  // Function ID, identifies the function
    FuncType funcType;
    std::vector< uint8_t > data;  // params or RetData

    using IsEnumerableTag = void;
    template < typename F >
    void enumerate(F &func)  // NOLINT
    {
        func(callId);
        func(funcId);
        func(funcType);
        func(data);
    }
};

// Data storage, usage, deletion, timeout handling
struct RpcData
{
    uint8_t version{0};            // RPC version
    uint8_t connectId{0};          // Identify a connection
    uint16_t channelId{0};         // Identify logical pipe from client to serviceInstance
    uint16_t clientObjId{0};       // End object ID
    uint16_t serviceId{0};         // Identify a service
    uint16_t serviceInstancId{0};  // Service instance ID
    uint16_t handleTime{0};        // Server processing time (ms)
    FuncData funcData;

    using IsEnumerableTag = void;
    template < typename F >
    void enumerate(F &func)  // NOLINT
    {
        func(version);
        func(connectId);
        func(channelId);
        func(clientObjId);
        func(serviceId);
        func(serviceInstancId);
        func(handleTime);
        func(funcData);
    }
};

struct CallContextBase
{
    uint16_t clientId;  // Call client ID, unique identifier
};

struct CallRequest
{
    FuncData funcData;
    std::shared_ptr< CallContextBase > callContext{nullptr};
};

inline uint32_t MakeCallKey(uint16_t const &instanceId, uint8_t const &funcId, uint8_t const &callId)
{
    return ((instanceId << isoft::serialize::kInt32_16) | (funcId << isoft::serialize::kInt32_8) | callId);  // NOLINT
}

}  // namespace dm
}  // namespace isoft

#endif  // _RPC_DEFINE_H_