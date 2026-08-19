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
/// @file       uds_message.h
/// @brief      This file provides the definitions of UdsMessage and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_UDS_TRANSPORT_MESSAGE_H_
#define ARA_DIAG_UDS_TRANSPORT_MESSAGE_H_

#include <ara/core/map.h>
#include <ara/core/string.h>

#include <cstdint>
#include <memory>

#include "protocol_types.h"

namespace ara {
namespace diag {

namespace tp {
class UdsMessageWrapper;
}  // namespace tp

namespace uds_transport {

/// @brief class represents an UDS message exchanged between DM generic core (UdsTransportProtocolMgr) and
/// a specific implementation of UdsTransportProtocolHandler on diagnostic request reception path or
/// diagnostic response transmission path.
/// UdsMessage provides the storage for UDS requests/responses. Instances of UdsMessage (with optimized resource
/// allocation) are only created by DM generic core. UdsTransportProtocolHandler read/write on it.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00291}@tracestatus{draft}
class UdsMessage
{
public:
    /// @brief type for UDS source and target addresses
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00293}@tracestatus{draft}
    using Address = uint16_t;

    /// @brief Type for the meta information attached to a UdsMessage.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00294}@tracestatus{draft}
    using MetaInfoMap = ara::core::Map< ara::core::String, ara::core::String >;

    /// @brief type of target address in UdsMessage
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00296}@tracestatus{draft}
    enum class TargetAddressType : std::uint8_t
    {
        kPhysical   = 0,
        kFunctional = 1
    };

protected:
    /// @brief non public default ctor.
    /// The default ctor is protected as we want to forbid, that UdsTransportProtocol handlers do create
    /// UdsMessages on its own! Only DM is allowed to create and hands over UdsMessagePtrs to
    /// UdsTransportProtocolHandler.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_09012}@tracestatus{draft}
    UdsMessage() = default;

    /// @brief Copy constructing the uds message.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] other  Object to copy-construct from
    /// @threadsafety{reentrant}
    ///
    ///
    /// @traceid{SWS_DM_09011}@tracestatus{draft}
    UdsMessage(UdsMessage const& other) = default;

    /// @brief Move constructing the uds message.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] other  Object to move-construct from
    /// @threadsafety{reentrant}
    ///
    ///
    /// @traceid{SWS_DM_09013}@tracestatus{draft}
    UdsMessage(UdsMessage&& other) noexcept = default;

    /// @brief Copy assigning the uds message.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] other   Object to copy-assign from.
    /// @threadsafety{reentrant}
    ///
    ///
    /// @traceid{SWS_DM_09014}@tracestatus{draft}
    UdsMessage& operator=(UdsMessage const& other) = default;

    /// @brief Move assigning the uds message.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] other   Object to move-assign from.
    /// @threadsafety{reentrant}
    ///
    ///
    /// @traceid{SWS_DM_09018}@tracestatus{draft}
    UdsMessage& operator=(UdsMessage&& other) noexcept = default;

public:
    /// @brief Destructing the uds message.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_09010}@tracestatus{draft}
    virtual ~UdsMessage() = default;

    /// @brief Get the source address of the uds message.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return The source address of the uds message.
    /// @threadsafety{reentrant}
    ///
    ///
    /// @traceid{SWS_DM_00297}@tracestatus{draft}
    virtual Address GetSa() const noexcept;

    /// @brief Get the target address of the uds message.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return The target address of the uds message.
    /// @threadsafety{reentrant}
    ///
    ///
    /// @traceid{SWS_DM_00298}@tracestatus{draft}
    virtual Address GetTa() const noexcept;

    /// @brief Get the target address type (phys/func) of the uds message.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return The target address type of the uds message.
    /// @threadsafety{reentrant}
    ///
    ///
    /// @traceid{SWS_DM_00299}@tracestatus{draft}
    virtual TargetAddressType GetTaType() const noexcept;

    /// @brief Get the UDS message data starting with the SID (A_Data as per ISO)
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return The entire payload (A_Data)
    /// @threadsafety{unsafe}
    /// @note marked as "unsafe" with regard to threadsafety as implementation is allowed to
    /// do ressource allocation of buffer in the context of this call.
    ///
    ///
    /// @traceid{SWS_DM_00300}@tracestatus{draft}
    virtual ByteVector const& GetPayload() const noexcept;

    /// @brief return the underlying buffer for write access.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @note needed by UdsTransportProtocolHandler impl. to fill the UdsMessage with data in RX path.
    /// I.e. UdsTransportProtocolHandler impl. gets the UdsMessage instance from call to
    /// UdsTransportProtocolMgr::IndicateMessage() and then calls this method on it and write into returned
    /// uds_transport::ByteVector.
    /// @return payload of the UDSMessage starting from SID.
    /// @threadsafety{unsafe}
    /// @note marked as "unsafe" with regard to threadsafety as implementation is allowed to
    /// do ressource allocation of buffer in the context of this call.
    ///
    ///
    /// @traceid{SWS_DM_00301}@tracestatus{draft}
    virtual ByteVector& GetPayload() noexcept;

    /// @brief add new metaInfo to this message.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @note typically called by the transport plugin to add channel specific meta-info.
    /// (see SWS - there are already predefined meta-info keys for DoIP....)
    /// @param[in] metaInfo meta information relevant for UdsMessage
    /// @threadsafety{unsafe}
    ///
    ///
    /// @traceid{SWS_DM_00302}@tracestatus{draft}
    virtual void AddMetaInfo(MetaInfoMap metaInfo) noexcept;

private:
    tp::UdsMessageWrapper* wrapper_{};
    friend class tp::UdsMessageWrapper;
};
/// This is the unique_ptr for UdsMessages containing a custom deleter as provided by the generic/core DM part
/// towards the UdsTransportLayer-Plugin.
/// @note How the exact typedef for UdsMessagePtr looks like, is up to the DM product vendor. I.e. how f.i. the deleter
/// signature looks like ... basically the minimal agreement is: UdsMessagePtr shall behave like a
/// std::unique_ptr<UdsMessage>!
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00303}@tracestatus{draft}
using UdsMessagePtr = std::unique_ptr< UdsMessage >;
/// This is the unique_ptr for constant UdsMessages containing a custom deleter as provided by the generic/core DM part
/// towards the UdsTransportLayer-Plugin.
/// @note How the exact typedef for UdsMessageConstPtr looks like, is up to the DM product vendor. I.e. how f.i. the
/// deleter signature looks like ... basically the minimal agreement is: UdsMessageConstPtr shall behave like a
/// std::unique_ptr<const UdsMessage>!
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00304}@tracestatus{draft}
using UdsMessageConstPtr = std::unique_ptr< const UdsMessage >;

}  // namespace uds_transport
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_UDS_TRANSPORT_MESSAGE_H_