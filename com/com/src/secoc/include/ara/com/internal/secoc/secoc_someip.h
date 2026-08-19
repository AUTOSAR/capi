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
/// @file       secoc_someip.h
/// @brief      secoc for someip
/// @details
/// @date       2023-09-04
/// @author     shigang.zan
/// @version    1.2.0
///
/// ================================================================

#ifndef __SECOC_SOMEIP_H__
#define __SECOC_SOMEIP_H__

#include <tuple>

#include "ara/com/internal/log/log.h"
#include "ara/com/internal/secoc/secoc_config.h"
#include "ara/com/internal/secoc/secoc_helper.h"
#include "ara/com/internal/secoc/secoc_protocol.h"
#include "ara/com/internal/secoc/secoc_statistics.h"
#include "ara/com/internal/secoc/secoc_type.h"
#include "ara/com/secoc/fvm.h"
#include "ara/core/map.h"
#include "ara/core/string_view.h"
#include "isoft/manifestreader/manifest.h"
#include "isoft/manifestreader/manifest_node.h"
#include "isoft/manifestreader/manifest_reader.h"
#include "isoft/manifestreader/tps_enumeration.h"
#include "nsomeip/net/nsi_message.h"

namespace ara {
namespace com {
namespace internal {
namespace secoc {

/// @brief secoc someip ids mapping
struct SecOC4SomeipIdsMapping
{
    /// @brief secoc data id
    uint16_t dataId;
    /// @brief someip service id
    uint16_t serviceId;
    /// @brief someip instance id
    uint16_t instanceId;
    /// @brief event / method deployment id
    uint16_t eventOrMethodId;
    /// @brief type 0:notification 1:request 2:response
    uint16_t type;

    /// @brief help to load correct config
    /// @param[in] node
    /// @return success or failure
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
    {
        ara::core::StringView const dataIdStr{"dataId"};
        ara::core::StringView const serviceIdStr{"serviceId"};
        ara::core::StringView const instanceIdStr{"instanceId"};
        ara::core::StringView const eventOrMethodIdStr{"eventOrMethodId"};
        ara::core::StringView const typeStr{"type"};
        if (isoft::kSuccess != node.Load(eventOrMethodIdStr, eventOrMethodId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(instanceIdStr, instanceId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(serviceIdStr, serviceId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(dataIdStr, dataId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(typeStr, type)) {
            return isoft::kFailure;
        }
        return isoft::kSuccess;
    }
};

/// @brief secoc for someip class
class SecOC4SOMEIP
{
public:
    /// @brief load config
    /// @code{.isoft}
    /// export_level=/COM/Security/SecOC
    /// @endcode
    /// @param[in] path
    /// @return true / false
    static bool LoadConfig(ara::core::StringView const& path) noexcept { return Impl::Instance().ImplLoadConfig(path); }

    /// @brief get secoc data id for someip method/event/field
    /// @code{.isoft}
    /// export_level=/COM/Security/SecOC
    /// @endcode
    /// @param[in] serviceId
    /// @param[in] instanceId
    /// @param[in] eventOrMethodId
    /// @return data id non-0: enable SecOC 0: disabel SecOC
    static uint16_t GetSecOCDataId(uint16_t const serviceId,
                                   uint16_t const instanceId,
                                   uint16_t const eventOrMethodId,
                                   uint16_t const msgType) noexcept
    {
        return Impl::Instance().ImplGetSecOCDataId(serviceId, instanceId, eventOrMethodId, msgType);
    }

    ///   SOME/IP MSG   |<----  SecOC Secured Payload ----->|
    ///  ---------------------------------------------------
    /// | Header part I | Header part II |  SOME/IP Payload |
    ///  ---------------------------------------------------
    ///   SECOC MSG     |<---------------Header part I length---------------->|
    ///  ---------------------------------------------------------------------
    /// |Header part I *| Header part II |  SOME/IP Payload | Tx FV*| Tx CMAC*|
    ///  ---------------------------------------------------------------------
    /// * : add / change
    /// SOME/IP header part I : Message Id 32bit, Length 32bit.
    /// SOME/IP header part II: Request Id 32bit, Protocol Version 8bit, Interface Version 8bit, Message Type 8bit,
    /// Return Code 8bit.

    /// @brief Received message processing flow
    /// @code{.isoft}
    /// export_level=/COM/Security/SecOC
    /// @endcode
    /// @param[in] dataId
    /// @param[in] message Received message, before E2E and SOME/IP deserialization
    /// @return -1 represents failure or discard message, 0 represents success
    /// @details Step 1. Assemble (Header part II+SOME/IP Payload), call SecOC protocol processing verification process
    ///          Step 2. If step 1 fails, return -1; if verification suggests discarding message, return -2; if successful, proceed to step 3
    ///          Step 3. If message successfully restored, modify SOME/IP Header Part I length
    static int32_t SecOCRxHandle(uint16_t const dataId, nsi_message_t* const message) noexcept
    {
        return Impl::ImplSecOCRxHandle(dataId, message);
    }

    /// @brief Sent message processing flow
    /// @code{.isoft}
    /// export_level=/COM/Security/SecOC
    /// @endcode
    /// @param[in] dataId
    /// @param[in] message Message to be sent, after E2E and SOME/IP serialization
    /// @return -1 represents failure, 0 represents success
    /// @details Step 1. Assemble (Header part II+SOME/IP Payload), call SecOC protocol to try to get Auth information
    ///          Step 2. If step 1 fails, return -1; if successful, proceed to step 3
    ///          Step 3. Modify SOME/IP Header Part I length, append Auth information at the end of SOME/IP Payload
    static int32_t SecOCTxHandle(uint16_t const dataId, nsi_message_t* const message) noexcept
    {
        return Impl::ImplSecOCTxHandle(dataId, message);
    }

private:
    /// @brief impl class
    class Impl
    {
    public:
        /// @brief get instance
        /// @return impl instance
        static Impl& Instance() noexcept
        {
            static Impl s_Instance;
            return s_Instance;
        }

        /// @brief load config
        /// @param[in] path
        /// @return true / false
        bool ImplLoadConfig(ara::core::StringView const& path) noexcept
        {
            if (configured_) {
                ComLogInfo("secoc was already configured, skipping");
                return false;
            }
            ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > const manifestRes{
                isoft::manifestreader::OpenManifest(path)};
            if (!manifestRes) {
                ComLogInfo("Can not load secoc manifest profile for SOME/IP");
                return false;
            }
            ara::core::Vector< SecOC4SomeipIdsMapping > cnf{};
            ara::core::StringView const secocProfile{"secoc_someip"};
            int32_t const ret{manifestRes.Value()->Load(secocProfile, cnf)};
            if (ret == isoft::kSuccess) {
                if (cnf.empty()) {
                    ComLogDebug("No secoc dataId confiured");
                    return true;
                }
                for (auto const& it : cnf) {
                    std::ignore
                        = ids_.insert(std::make_pair(std::tuple< uint16_t, uint16_t, uint16_t, uint16_t >(
                                                         it.serviceId, it.instanceId, it.eventOrMethodId, it.type),
                                                     it.dataId));
                }
            } else {
                ComLogDebug("Load config error");
                return false;
            }
            configured_ = true;
            return true;
        }

        /// @brief get secoc data id for someip method/event/field
        /// @param[in] serviceId
        /// @param[in] instanceId
        /// @param[in] eventOrMethodId
        /// @param[in] msgType
        /// @return data id
        uint16_t ImplGetSecOCDataId(uint16_t serviceId,
                                    uint16_t instanceId,
                                    uint16_t eventOrMethodId,
                                    uint16_t msgType) noexcept
        {
            // transfer SOMEIP message type
            uint16_t type{0x00FF};  // NOLINT -- TODO[magic-numbers]
            if ((msgType == NSI_MT_REQUEST) || (msgType == NSI_MT_REQUEST_NO_RETURN)) {
                // request
                type = 1;
            } else if (msgType == NSI_MT_NOTIFICATION) {
                // notification
                type = 0;
            } else if ((msgType == NSI_MT_RESPONSE) || (msgType == NSI_MT_ERROR)) {
                // response
                type = 2;
            }
            ara::core::Map< std::tuple< uint16_t, uint16_t, uint16_t, uint16_t >, uint16_t >::iterator const itId{
                ids_.find(std::move(std::make_tuple(serviceId, instanceId, eventOrMethodId, type)))};
            if (itId != ids_.end()) {
                return itId->second;
            }
            return 0;
        }

        /// @brief Received message processing flow
        /// @param[in] dataId
        /// @param[in] message Received message, before E2E and SOME/IP deserialization
        /// @return -1 represents failure or discard message, 0 represents success
        /// @details Step 1. Assemble (Header part II+SOME/IP Payload), call SecOC protocol processing verification process
        ///          Step 2. If step 1 fails, return -1; if verification suggests discarding message, return -2; if successful, proceed to step 3
        ///          Step 3. If message successfully restored, modify SOME/IP Header Part I length
        static int32_t ImplSecOCRxHandle(uint16_t const dataId, nsi_message_t* const message) noexcept
        {
            bool shouldDrop{false};
            uint16_t authLength{};
            VerificationStatusResultEnum verificationResult;
            // DumpSomeIpMsg(message);
            ComLogDebug("Verify SecOc Msg");
            if (SecOCProtocol::VerifyAuthInfo4SomeIpMsg(dataId, message, shouldDrop, verificationResult, authLength)) {
                if (!shouldDrop) {
                    std::ignore = nai_buflist_rcommit_last(&message->payload, static_cast< size_t >(authLength));
                    message->hdr.len -= authLength;
                    return 0;
                }
                return -2;
            }
            return -1;
        }

        /// @brief Sent message processing flow
        /// @param[in] dataId
        /// @param[in] message Message to be sent, after E2E and SOME/IP serialization
        /// @return -1 represents failure, 0 represents success
        /// @details Step 1. Assemble (Header part II+SOME/IP Payload), call SecOC protocol to try to get Auth information
        ///          Step 2. If step 1 fails, return -1; if successful, proceed to step 3
        ///          Step 3. Modify SOME/IP Header Part I length, append Auth information at the end of SOME/IP Payload
        static int32_t ImplSecOCTxHandle(uint16_t const dataId, nsi_message_t* const message) noexcept
        {
            ara::core::Vector< uint8_t > authInfo;
            if (!SecOCProtocol::GenerateAuthInfo4SomeIpMsg(dataId, message, authInfo)) {
                ComLogDebug("GenerateAuthInfo4SomeIpMsg met error");
                return -1;
            }
            ComLogDebug("Build SecOc Msg");
            // copy auth info for someip and the length will adjust automatically.
            nsi_message_io_t io;
            std::ignore = nsi_message_write_start(&io, message);
            std::ignore = nsi_message_write(&io, authInfo.data(), authInfo.size());
            std::ignore = nsi_message_write_end(&io);
            return 0;
        }

        /// @brief impl()
        Impl() noexcept = default;
        /// @brief ~Impl()
        virtual ~Impl() noexcept = default;
        /// @brief Impl()
        /// @param[in] other
        Impl(Impl const& other) noexcept = default;
        /// @brief Impl operator =
        /// @param[in] other
        /// @return Impl &
        Impl& operator=(Impl const& other) noexcept = default;
        /// @brief Impl()
        /// @param[in] other
        Impl(Impl&& other) noexcept = default;
        /// @brief Impl operartor =
        /// @param[in] other
        /// @return Impl &
        Impl& operator=(Impl&& other) noexcept = default;

    private:
        /// @brief configured or not
        bool configured_{false};
        /// @brief secoc someip ids mapping
        ara::core::Map< std::tuple< uint16_t, uint16_t, uint16_t, uint16_t >, uint16_t > ids_;
    };
};

}  // namespace secoc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif