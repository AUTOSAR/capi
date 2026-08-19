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
/// @file       msg_process.h
/// @brief      IPC data processing
/// @details    IPC data processing
/// @date       2025-03-19
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/FW/serialization
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0011
/// @unit_name=Msg_Process
/// @unit_description=fw client and server communication serialization
/// processing class.
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_MESSAGE_PROCESS_H_
#define _ARA_FW_MESSAGE_PROCESS_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/serialize/serialize.h>

#include <cstdint>
#include <utility>

#include "ara/fw/common/common.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief Client event processing class.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00363
/// @trace_id_dd=DD_FW_00491
/// @needwork = ad
/// @endcode
class ClientEventMsg final
{
public:
    /// @brief Serialize
    /// @return Serialized byte stream
    /// @exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00364
    /// @trace_id_dd=DD_FW_00492
    /// @needwork = ad
    /// @endcode
    inline Chunk Dump() const
    {
        Chunk payload;
        isoft::serialize::Buffer< Chunk > buffer{payload};
        isoft::serialize::Buffer< Chunk >::result_t const status{
            isoft::serialize::Serialize(buffer, instanceId_, fwStatus_)};
        if (buffer.First() == 0U) {
        }
        if (status < 0) {
            payload.clear();
        }
        return payload;
    }
    /// @brief Deserialize
    /// @param chunk Data byte stream, deserialization data source
    /// @return Deserialized data structure
    /// @exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00365
    /// @trace_id_dd=DD_FW_00493
    /// @needwork = ad
    /// @endcode
    inline uint32_t Load(Chunk &&chunk)
    {
        isoft::serialize::Buffer< Chunk > const buffer{chunk};
        if (isoft::serialize::Deserialize(buffer, instanceId_, fwStatus_) < 0) {
            return 1U;
        }
        std::ignore = std::move(chunk);
        return 0U;
    }

public:
    /// @brief Set instance
    /// @param strId Service instance ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00366
    /// @trace_id_dd=DD_FW_00494
    /// @needwork = ad
    /// @endcode
    void SetInstanceId(ara::core::String const &strId) noexcept
    {
        instanceId_ = strId;
        return;
    }

    /// @brief Get instance
    /// @return Return instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00367
    /// @trace_id_dd=DD_FW_00495
    /// @needwork = ad
    /// @endcode
    ara::core::String GetInstanceId() const noexcept { return instanceId_; }

    /// @brief Set firewall state
    /// @param status Firewall state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00368
    /// @trace_id_dd=DD_FW_00496
    /// @needwork = ad
    /// @endcode
    void SetFwStatus(uint32_t const status) noexcept
    {
        fwStatus_ = status;
        return;
    }

    /// @brief Get fw corresponding state
    /// @return fw state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00369
    /// @trace_id_dd=DD_FW_00497
    /// @needwork = ad
    /// @endcode
    uint32_t GetFwStatus() const noexcept { return fwStatus_; }

public:
    /// @brief Parameterless constructor
    /// @exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00370
    /// @trace_id_dd=DD_FW_00498
    /// @needwork = ad
    /// @endcode
    ClientEventMsg() = default;

    /// @brief Parameterless constructor
    /// @exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00371
    /// @trace_id_dd=DD_FW_00499
    /// @needwork = ad
    /// @endcode
    ~ClientEventMsg() = default;

    /// @brief Copy constructor
    /// @param client Client instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00372
    /// @trace_id_dd=DD_FW_00500
    /// @needwork = ad
    /// @endcode
    ClientEventMsg(ClientEventMsg const &client) = delete;

    /// @brief Move constructor
    /// @param client Client instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00373
    /// @trace_id_dd=DD_FW_00501
    /// @needwork = ad
    /// @endcode
    ClientEventMsg(ClientEventMsg &&client) = delete;

    /// @brief Copy assignment operator
    /// @param client Client instance
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00374
    /// @trace_id_dd=DD_FW_00502
    /// @needwork = ad
    /// @endcode
    ClientEventMsg &operator=(ClientEventMsg const &client) = delete;

    /// @brief Move assignment operator
    /// @param client Client instance
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00375
    /// @trace_id_dd=DD_FW_00503
    /// @needwork = ad
    /// @endcode
    ClientEventMsg &operator=(ClientEventMsg &&client) = delete;

    /// @brief
    /// @param instId inst instance ID
    /// @param status FW state
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00376
    /// @trace_id_dd=DD_FW_00504
    /// @needwork = ad
    /// @endcode
    ClientEventMsg(ara::core::StringView const &instId, uint32_t const status) noexcept
        : instanceId_{instId}, fwStatus_{status}
    {
    }

private:
    /// @name instanceId
    /// @brief Port instance descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00505
    /// @needwork = dda
    /// @endcode
    ara::core::String instanceId_{""};
    /// @name fwStatus
    /// @brief Firewall state value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00506
    /// @needwork = dda
    /// @endcode
    uint32_t fwStatus_{0U};
};
/// @brief
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00377
/// @trace_id_dd=DD_FW_00507
/// @needwork = ad
/// @endcode
class ClientEventParse
{
public:
    /// @brief Parameterless constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00378
    /// @trace_id_dd=DD_FW_00508
    /// @needwork = ad
    /// @endcode
    ClientEventParse() = default;
    /// @brief Client data parsing
    /// @param l Copy constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00379
    /// @trace_id_dd=DD_FW_00509
    /// @needwork = ad
    /// @endcode
    ClientEventParse(ClientEventParse const &l) = default;
    /// @brief Move constructor
    /// @param l Move constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00380
    /// @trace_id_dd=DD_FW_00510
    /// @needwork = ad
    /// @endcode
    ClientEventParse(ClientEventParse &&l) = default;
    /// @brief Operator processing
    /// @param l client instance
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00381
    /// @trace_id_dd=DD_FW_00511
    /// @needwork = ad
    /// @endcode
    ClientEventParse &operator=(ClientEventParse const &l) = default;
    /// @brief Operator processing
    /// @param l client instance
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00382
    /// @trace_id_dd=DD_FW_00512
    /// @needwork = ad
    /// @endcode
    ClientEventParse &operator=(ClientEventParse &&l) = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00383
    /// @trace_id_dd=DD_FW_00513
    /// @needwork = ad
    /// @endcode
    virtual ~ClientEventParse() = default;
    /// @brief Serialize
    /// @param instanceId Port instance identifier
    /// @param fwStatus Firewall state
    /// @return Serialized byte stream
    /// @exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00384
    /// @trace_id_dd=DD_FW_00514
    /// @needwork = ad
    /// @endcode
    static Chunk Serialize(ara::core::StringView const &instanceId, uint32_t const fwStatus);
    /// @brief Serialize
    /// @param clientEvMsg Client security event to be serialized
    /// @return Serialized byte stream
    /// @exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00385
    /// @trace_id_dd=DD_FW_00515
    /// @needwork = ad
    /// @endcode
    static Chunk Serialize(ClientEventMsg const &clientEvMsg);
    /// @brief Deserialize
    /// @param data Deserialization data source
    /// @param dataSize Data source length
    /// @param msg Client security event
    /// @exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00386
    /// @trace_id_dd=DD_FW_00516
    /// @needwork = ad
    /// @endcode
    static void Deserialize(uint8_t *const data, uint32_t const dataSize, ClientEventMsg *const msg);
};

}  // namespace internal
}  // namespace fw
}  // namespace ara

#endif  // _ARA_FW_MESSAGE_PROCESS_H_