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
/// @file       secoc_signal.h
/// @brief      secoc for signal
/// @details
/// @date       2023-09-04
/// @author     shigang.zan
/// @version    1.2.0
///
/// ================================================================

#ifndef __SECOC_SIGNAL_H__
#define __SECOC_SIGNAL_H__

#include "ara/com/internal/secoc/secoc_config.h"
#include "ara/com/internal/secoc/secoc_protocol.h"
#include "ara/com/internal/secoc/secoc_statistics.h"
#include "ara/com/secoc/fvm.h"

namespace ara {
namespace com {
namespace internal {
namespace secoc {
/// @brief secoc signal ids mapping
struct SecOC4SignalIdsMapping
{
    /// @brief secoc data id
    uint16_t dataId;
    /// @brief pdu id
    /// @details from SOMEIP header Part I - messageId(serviceId+eventId)
    uint32_t pduId;

    /// @brief help to load correct config
    /// @param[in] node
    /// @return success or failure
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
    {
        ara::core::StringView const dataIdStr{"dataId"};
        ara::core::StringView const pduIdStr{"pduId"};
        if (isoft::kSuccess != node.Load(dataIdStr, dataId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(pduIdStr, pduId)) {
            return isoft::kFailure;
        }
        return isoft::kSuccess;
    }
};

/// @brief secoc for signal class
class SecOC4Signal
{
public:
    /// @brief load config
    /// @code{.isoft}
    /// export_level=/COM/Security/SecOC
    /// @endcode
    /// @param[in] path
    /// @return true / false
    static bool LoadConfig(ara::core::StringView const& path) noexcept { return Impl::Instance().ImplLoadConfig(path); }

    /// @brief get secoc data id for signal
    /// @code{.isoft}
    /// export_level=/COM/Security/SecOC
    /// @endcode
    /// @param[in] pduId
    /// @return data id non-0: enable SecOC 0: disabel SecOC
    static uint16_t GetSecOCDataId(uint32_t const pduId) noexcept { return Impl::Instance().ImplGetSecOCDataId(pduId); }

    ///    Signal MSG   |<----     Signal Payload     ----->|
    ///  ---------------------------------------------------
    /// | Header(64bit) |              Payload              |
    ///  ---------------------------------------------------
    ///    SECOC MSG    |<---------------   Header length    ---------------->|
    ///  ---------------------------------------------------------------------
    /// | Header(64bit)*|              Payload              | Tx FV*| Tx CMAC*|
    ///  ---------------------------------------------------------------------
    /// * : add / change
    /// Header: Message Id/PDU Id 32bit, Length 32bit.

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
                ComLogInfo("Can not load secoc manifest profile for Signal");
                return false;
            }
            ara::core::Vector< SecOC4SignalIdsMapping > cnf{};
            ara::core::StringView const secocProfile{"secoc_signal"};
            int32_t const ret{manifestRes.Value()->Load(secocProfile, cnf)};
            if (ret == isoft::kSuccess) {
                if (cnf.empty()) {
                    ComLogDebug("No secoc dataId confiured");
                    return false;
                }
                for (auto const& it : cnf) {
                    std::ignore = ids_.insert(std::make_pair(it.pduId, it.dataId));
                }
            } else {
                ComLogDebug("Load config error");
                return false;
            }
            configured_ = true;
            return true;
        }

        /// @brief get secoc data id for signal
        /// @param[in] pduId
        /// @return data id
        uint16_t ImplGetSecOCDataId(uint32_t pduId) noexcept
        {
            ara::core::Map< uint32_t, uint16_t >::iterator const itId{ids_.find(pduId & 0xFFFF7FFF)};
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
            if (SecOCProtocol::VerifyAuthInfo4SignalMsg(dataId, message, shouldDrop, verificationResult, authLength)) {
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
            if (!SecOCProtocol::GenerateAuthInfo4SignalMsg(dataId, message, authInfo)) {
                ComLogDebug("GenerateAuthInfo4SignalMsg met error");
                return -1;
            }
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
        ara::core::Map< uint32_t, uint16_t > ids_;
    };
};
}  // namespace secoc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif