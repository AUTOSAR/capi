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
/// @file       npc_types.cpp
/// @brief      Binding layer type implementation file
/// @details
/// @date       2023-10-10
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/internal/npc/npc_types.h"

namespace ara {
namespace com {
namespace internal {
namespace npc {
constexpr Message::ServiceId Message::kServiceIdReserved;
constexpr Message::ServiceId Message::kServiceIdReservedMin4OEMTest;
constexpr Message::ServiceId Message::kServiceIdReservedMax4OEMTest;
constexpr Message::ServiceId Message::kServiceIdReservedMin4Tier1Test;
constexpr Message::ServiceId Message::kServiceIdReservedMax4Tier1Test;
constexpr Message::ServiceId Message::kServiceIdReservedMin4Tier1ECU;
constexpr Message::ServiceId Message::kServiceIdReservedMax4Tier1ECU;
constexpr Message::ServiceId Message::kServiceIdReservedNonService;
constexpr Message::ServiceId Message::kServiceIdReservedSpecialService;
constexpr Message::ServiceId Message::kServiceIdAny;
constexpr Message::InstanceId Message::kInstanceIdReserved;
constexpr Message::InstanceId Message::kInstanceIdReservedAll;
constexpr Message::InstanceId Message::kInstanceIdAny;
constexpr Message::MajorVersion Message::kMajorVersionDefault;
constexpr Message::MajorVersion Message::kMajorVersionAny;
constexpr Message::MajorVersion Message::kMajorVersionUndefinedBlacklisted;
constexpr Message::MinorVersion Message::kMinorVersionDefault;
constexpr Message::MinorVersion Message::kMinorVersionAny;
constexpr Message::EventGroupId Message::kEventGroupIdReserved;
constexpr Message::EventGroupId Message::kEventGroupIdReservedAll;
constexpr Message::MethodId Message::kMethodIdReservedMin4Method;
constexpr Message::MethodId Message::kMethodIdReservedMax4Method;
constexpr Message::MethodId Message::kMethodIdReservedMin4Event;
constexpr Message::MethodId Message::kMethodIdReservedMax4Event;
constexpr Message::MethodId Message::kMethodIdReservedMCMethod;
constexpr Message::MethodId Message::kMethodIdReservedMCEvent;
constexpr Message::MethodId Message::kMethodIdReservedSDEvents;
constexpr Message::MethodId Message::kMethodIdAny;
constexpr Message::Length Message::kLengthMin;
constexpr Message::ProtocolVersion Message::kProtocolVersionDefault;
constexpr Message::MessageType Message::kMessageTypeUnknown;
constexpr Message::MessageType Message::kMessageTypeRequest;
constexpr Message::MessageType Message::kMessageTypeRequestNoReturn;
constexpr Message::MessageType Message::kMessageTypeNotification;
constexpr Message::MessageType Message::kMessageTypeResponse;
constexpr Message::MessageType Message::kMessageTypeError;
constexpr Message::MessageType Message::kMessageTypeTpRequest;
constexpr Message::MessageType Message::kMessageTypeTpRequestNoReturn;
constexpr Message::MessageType Message::kMessageTypeTpNotification;
constexpr Message::MessageType Message::kMessageTypeTpResponse;
constexpr Message::MessageType Message::kMessageTypeTpError;
constexpr Message::ReturnCode Message::kReturnCodeOk;
constexpr Message::ReturnCode Message::kReturnCodeNotOk;
constexpr Message::ReturnCode Message::kReturnCodeUnknownService;
constexpr Message::ReturnCode Message::kReturnCodeUnknownMethod;
constexpr Message::ReturnCode Message::kReturnCodeNotReady;
constexpr Message::ReturnCode Message::kReturnCodeNotReachable;
constexpr Message::ReturnCode Message::kReturnCodeTimeout;
constexpr Message::ReturnCode Message::kReturnCodeWrongProtocolVersion;
constexpr Message::ReturnCode Message::kReturnCodeWrongInterfaceVersion;
constexpr Message::ReturnCode Message::kReturnCodeMalformedMessage;
constexpr Message::ReturnCode Message::kReturnCodeWrongMessageType;
constexpr Message::ReturnCode Message::kReturnCodeE2ERepeated;
constexpr Message::ReturnCode Message::kReturnCodeE2EWrongSequence;
constexpr Message::ReturnCode Message::kReturnCodeE2E;
constexpr Message::ReturnCode Message::kReturnCodeE2ENotAvailable;
constexpr Message::ReturnCode Message::kReturnCodeE2ENoNewData;
constexpr Message::ReturnCode Message::kReturnCodeReservedMin4Generic;
constexpr Message::ReturnCode Message::kReturnCodeReservedMax4Generic;
constexpr Message::ReturnCode Message::kReturnCodeReservedMin4Specific;
constexpr Message::ReturnCode Message::kReturnCodeReservedMax4Specific;
constexpr Message::ReturnCode Message::kReturnCodeReservedMask;
constexpr Message::ReturnCode Message::kReturnCodeValidMask;
constexpr Message::TransportProtocol Message::kTransportProtocolUnknown;
constexpr Message::TransportProtocol Message::kTransportProtocolTCP;
constexpr Message::TransportProtocol Message::kTransportProtocolUDP;
constexpr Message::TransportProtocol Message::kTransportProtocolBoth;
constexpr Message::SessionHandling Message::kSessionHandlingUnknown;
constexpr Message::SessionHandling Message::kSessionHandlingActive;
constexpr Message::SessionHandling Message::kSessionHandlingInactive;
namespace types {
ara::core::String& ToString(ara::core::String& r, npc_message_t const& m) noexcept
{
    switch (m.hdr.type) {
        case Message::kMessageTypeNotification: {
            return internal::format::FormatToString(
                r, GenK2V0("service", m.hdr.serv), GenK2V("instance", m.inst), GenK2V("reliable", m.reliable),
                GenK2V("s->notification", m.hdr.method), GenK2V("session", m.hdr.session), GenK2V("length", m.hdr.len));
        } break;
        case Message::kMessageTypeRequest:
        case Message::kMessageTypeRequestNoReturn: {
            return internal::format::FormatToString(r, GenK2V0("service", m.hdr.serv), GenK2V("instance", m.inst),
                                                    GenK2V("reliable", m.reliable), GenK2V("p->request", m.hdr.method),
                                                    GenK2V("session", m.hdr.session));
        } break;
        case Message::kMessageTypeResponse: {
            return internal::format::FormatToString(r, GenK2V0("service", m.hdr.serv), GenK2V("instance", m.inst),
                                                    GenK2V("reliable", m.reliable), GenK2V("s->response", m.hdr.method),
                                                    GenK2V("session", m.hdr.session));
        } break;
        case Message::kMessageTypeError: {
            return internal::format::FormatToString(r, GenK2V0("service", m.hdr.serv), GenK2V("instance", m.inst),
                                                    GenK2V("reliable", m.reliable), GenK2V("s->response", m.hdr.method),
                                                    GenK2V("session", m.hdr.session), GenK2V("error", m.hdr.code));
        } break;
        default: {
            return internal::format::FormatToString(
                r, GenK2V0("service", m.hdr.serv), GenK2V("instance", m.inst), GenK2V("reliable", m.reliable),
                GenK2V("method", m.hdr.method), GenK2V("session", m.hdr.session), GenK2V("type(unknown)", m.hdr.type));
        } break;
    }
}
}  // namespace types
}  // namespace npc
}  // namespace internal
}  // namespace com
}  // namespace ara