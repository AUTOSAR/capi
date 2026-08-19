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
/// @file       message_process.h
/// @brief      Data serialization implementation when AA application reports events
/// @details
/// @date       2022-12-27
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/lib client protocol
/// @interface_level=module
/// @trace_id_sr=SRS_IDSM_0005
/// @unit_description=Data serialization implementation when AA application reports events
/// @endcode
///
/// ================================================================

#ifndef _ARA_IDSC_MESSAGE_PROCESS_H_
#define _ARA_IDSC_MESSAGE_PROCESS_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/serialize/serialize.h>

#include <cstdint>
#include <utility>

#include "ara/idsm/common.h"

namespace ara {
namespace idsm {
/// @brief Type redefinition: define enumeration type for timestamp source
/// @code{.isoft}
/// @unit_name=TimestampSource
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00026
/// @trace_id_dd=DD_IDSM_00059
/// @needwork = ad
/// @endcode
enum class TimestampSource : uint8_t
{
    /// @brief Provided by AA-side application when calling ReportEvent interface
    kAppReport = 1,
    /// @brief AA-side application provides callback function, timestamp obtained from callback
    kAppCallback,
    /// @brief Provided by ara::tsync time synchronization module
    kAraTsync,
    /// @brief OEM type timestamp
    kAppOem,
    /// @brief Unknown timestamp source
    kUnknown
};
/// @brief Type redefinition: define data structure for byte stream
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00027
/// @trace_id_dd=DD_IDSM_00060
/// @needwork = ad
/// @endcode
using Chunk = ara::core::Vector< std::uint8_t >;

/// @brief Security event serialization
/// @code{.isoft}
/// @unit_name=ClientEventMsg
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00028
/// @trace_id_dd=DD_IDSM_00061
/// @needwork = ad
/// @endcode
class ClientEventMsg final
{
public:
    /// @brief Serialization
    /// @return Byte stream after serialization
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00029
    /// @trace_id_dd=DD_IDSM_00062
    /// @needwork = ad
    /// @endcode
    inline Chunk Dump() const
    {
        Chunk payload;
        isoft::serialize::Buffer< Chunk > buffer{payload};
        isoft::serialize::Buffer< Chunk >::result_t const status{isoft::serialize::Serialize(
            buffer, instanceId_, processId_, contextData_, timestamp_, timeSource_, counter_)};
        if (buffer.First() == 0U) {
        }
        if (status < 0) {
            payload.clear();
        }
        return payload;
    }
    /// @brief Deserialization
    /// @param chunk Data byte stream, source data for deserialization
    /// @return Data structure after deserialization
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00030
    /// @trace_id_dd=DD_IDSM_00063
    /// @needwork = ad
    /// @endcode
    inline uint32_t Load(Chunk&& chunk)
    {
        isoft::serialize::Buffer< Chunk > const buffer{chunk};
        if (isoft::serialize::Deserialize(buffer, instanceId_, processId_, contextData_, timestamp_, timeSource_,
                                          counter_)
            < 0) {
            return 1U;
        }
        return 0U;
    }

public:
    /// @brief Set instance ID
    /// @param strId Instance ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00031
    /// @trace_id_dd=DD_IDSM_00064
    /// @needwork = ad
    /// @endcode
    void SetInstanceId(ara::core::String const& strId) noexcept { instanceId_ = strId; }
    /// @brief Get instance ID
    /// @return Instance ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00032
    /// @trace_id_dd=DD_IDSM_00065
    /// @needwork = ad
    /// @endcode
    ara::core::String GetInstanceId() const noexcept { return instanceId_; }
    /// @brief Set process ID
    /// @param id Process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00033
    /// @trace_id_dd=DD_IDSM_00066
    /// @needwork = ad
    /// @endcode
    void SetProcessId(uint32_t const id) noexcept { processId_ = id; }
    /// @brief Get process ID
    /// @return Process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00034
    /// @trace_id_dd=DD_IDSM_00067
    /// @needwork = ad
    /// @endcode
    uint32_t GetProcessId() const noexcept { return processId_; }
    /// @brief Set context data
    /// @param ctxData Context data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00035
    /// @trace_id_dd=DD_IDSM_00068
    /// @needwork = ad
    /// @endcode
    void SetContextData(ContextDataType& ctxData) noexcept { contextData_.swap(ctxData); }
    /// @brief After GetContextData interface is called, ContextData will be cleared
    /// @param ctxData Context data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00036
    /// @trace_id_dd=DD_IDSM_00069
    /// @needwork = ad
    /// @endcode
    void GetContextData(ContextDataType& ctxData) noexcept
    {
        ctxData.clear();
        ctxData.swap(contextData_);
    }
    /// @brief Get context data length
    /// @return Context data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00037
    /// @trace_id_dd=DD_IDSM_00070
    /// @needwork = ad
    /// @endcode
    size_t GetContextDataSize() const noexcept { return contextData_.size(); }
    /// @brief Set timestamp
    /// @param tStamp Timestamp
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00038
    /// @trace_id_dd=DD_IDSM_00071
    /// @needwork = ad
    /// @endcode
    void SetTimeStamp(TimestampType const tStamp) noexcept { timestamp_ = tStamp; }
    /// @brief Get timestamp
    /// @return Timestamp
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00039
    /// @trace_id_dd=DD_IDSM_00072
    /// @needwork = ad
    /// @endcode
    TimestampType GetTimeStamp() const noexcept { return timestamp_; }
    /// @brief Set timestamp source
    /// @param source Timestamp source
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00040
    /// @trace_id_dd=DD_IDSM_00073
    /// @needwork = ad
    /// @endcode
    void SetTimeStampSource(TimestampSource const source) noexcept { timeSource_ = source; }
    /// @brief Get timestamp source
    /// @return Timestamp source
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00041
    /// @trace_id_dd=DD_IDSM_00074
    /// @needwork = ad
    /// @endcode
    TimestampSource GetTimeStampSource() const noexcept { return timeSource_; }
    /// @brief Set event counter
    /// @param num Event counter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00042
    /// @trace_id_dd=DD_IDSM_00075
    /// @needwork = ad
    /// @endcode
    void SetCounter(CountType const num) noexcept { counter_ = num; }
    /// @brief Get event counter
    /// @return Event counter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00043
    /// @trace_id_dd=DD_IDSM_00076
    /// @needwork = ad
    /// @endcode
    CountType GetCounter() const noexcept { return counter_; }

public:
    /// @brief Parameterless constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00077
    /// @needwork = dda
    /// @endcode
    ClientEventMsg() = default;
    /// @brief Parameterized constructor
    /// @param id Port instance identifier
    /// @param p Process ID
    /// @param t Timestamp
    /// @param s Timestamp source
    /// @param num Security event counter
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00044
    /// @trace_id_dd=DD_IDSM_00078
    /// @needwork = ad
    /// @endcode
    ClientEventMsg(ara::core::StringView const& id,
                   uint32_t const p,
                   TimestampType const t,
                   TimestampSource const s,
                   CountType const num)
        : ClientEventMsg{id, p, ContextDataType{}, t, s, num}
    {
    }
    /// @brief Parameterized constructor
    /// @param id Port instance descriptor
    /// @param p Process ID
    /// @param t Timestamp
    /// @param s Timestamp source
    /// @param v Context data
    /// @param num Security event counter
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00045
    /// @trace_id_dd=DD_IDSM_00079
    /// @needwork = ad
    /// @endcode
    ClientEventMsg(ara::core::StringView const& id,
                   uint32_t const p,
                   TimestampType const t,
                   TimestampSource const s,
                   ara::core::Vector< uint8_t > const& v,
                   CountType const num)
        : ClientEventMsg{id, p, v, t, s, num}
    {
    }

private:
    /// @brief Parameterized constructor
    /// @param instId Instance ID
    /// @param proId Process ID
    /// @param data Context data
    /// @param stamp Timestamp
    /// @param source Timestamp source
    /// @param num Event counter
    /// @code{.isoft}
    /// @throw Stack overflow exception
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00080
    /// @needwork = dda
    /// @endcode
    ClientEventMsg(ara::core::StringView const& instId,
                   uint32_t const proId,
                   ara::core::Vector< uint8_t > data,
                   TimestampType const stamp,
                   TimestampSource const source,
                   CountType const num)
        : instanceId_{instId}
        , processId_{proId}
        , contextData_{std::move(data)}
        , timestamp_{stamp}
        , timeSource_{source}
        , counter_{num}
    {
    }

private:
    /// @name instanceId_
    /// @brief Port instance descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00081
    /// @needwork = dda
    /// @endcode
    ara::core::String instanceId_{};
    /// @name processId_
    /// @brief Process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00082
    /// @needwork = dda
    /// @endcode
    uint32_t processId_{0U};
    /// @name contextData_
    /// @brief Context data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00083
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< uint8_t > contextData_{};
    /// @name timestamp_
    /// @brief Timestamp
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00084
    /// @needwork = dda
    /// @endcode
    TimestampType timestamp_{0U};
    /// @name timeSource_
    /// @brief Timestamp source
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00085
    /// @needwork = dda
    /// @endcode
    TimestampSource timeSource_{TimestampSource::kUnknown};
    /// @name counter_
    /// @brief Event counter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00086
    /// @needwork = dda
    /// @endcode
    CountType counter_{1U};
};
/// @brief Security event parsing
/// @code{.isoft}
/// @unit_name=ClientEventParse
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00046
/// @trace_id_dd=DD_IDSM_00087
/// @needwork = ad
/// @endcode
class ClientEventParse
{
public:
    /// @brief Parameterless constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00088
    /// @needwork = dda
    /// @endcode
    ClientEventParse() = default;
    /// @brief Copy constructor
    /// @param left Object to be copied
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00089
    /// @needwork = dda
    /// @endcode
    ClientEventParse(ClientEventParse const& l) = default;
    /// @brief Move constructor
    /// @param left Object to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00090
    /// @needwork = dda
    /// @endcode
    ClientEventParse(ClientEventParse&& l) = default;
    /// @brief Copy assignment operator
    /// @param left Object to be copied
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00091
    /// @needwork = dda
    /// @endcode
    ClientEventParse& operator=(ClientEventParse const& l) = default;
    /// @brief Move assignment operator
    /// @param left Object to be moved
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00092
    /// @needwork = dda
    /// @endcode
    ClientEventParse& operator=(ClientEventParse&& l) = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00093
    /// @needwork = dda
    /// @endcode
    virtual ~ClientEventParse() = default;
    /// @brief Serialization
    /// @param instanceId Port instance identifier
    /// @param processId Process ID
    /// @param timestamp Timestamp
    /// @param timeSource Timestamp source
    /// @param count Security event counter
    /// @return Byte stream after serialization
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00047
    /// @trace_id_dd=DD_IDSM_00094
    /// @needwork = ad
    /// @endcode
    static Chunk Serialize(ara::core::StringView const& instanceId,
                           uint32_t const processId,
                           TimestampType const timestamp,
                           TimestampSource const timeSource,
                           CountType const count);
    /// @brief Serialization
    /// @param instanceId Port instance identifier
    /// @param processId Process ID
    /// @param contextData Context data
    /// @param timestamp Timestamp
    /// @param timeSource Timestamp source
    /// @param count Security event counter
    /// @return Byte stream after serialization
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00048
    /// @trace_id_dd=DD_IDSM_00095
    /// @needwork = ad
    /// @endcode
    static Chunk Serialize(ara::core::StringView const& instanceId,
                           uint32_t const processId,
                           ContextDataType const& contextData,
                           TimestampType const timestamp,
                           TimestampSource const timeSource,
                           CountType const count);
    /// @brief Serialization
    /// @param clientEvMsg Client security event to be serialized
    /// @return Byte stream after serialization
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00049
    /// @trace_id_dd=DD_IDSM_00096
    /// @needwork = ad
    /// @endcode
    static Chunk Serialize(ClientEventMsg const& clientEvMsg);
    /// @brief Deserialization
    /// @param data Source data for deserialization
    /// @param dataSize Length of source data
    /// @param msg Client security event
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00050
    /// @trace_id_dd=DD_IDSM_00097
    /// @needwork = ad
    /// @endcode
    static void Deserialize(uint8_t* const data, uint32_t const dataSize, ClientEventMsg* const msg);
};

}  // namespace idsm
}  // namespace ara

#endif  // _ARA_IDSC_MESSAGE_PROCESS_H_