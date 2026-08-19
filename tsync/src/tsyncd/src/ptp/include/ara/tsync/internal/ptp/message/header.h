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
/// @file       header.h
/// @brief
/// @details
/// @date       2021-10-14
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_INTERNAL_PTP_MESSAGE_HEADER_H_
#define _ARA_TSYNC_INTERNAL_PTP_MESSAGE_HEADER_H_

#include <cstdint>

#include "ara/tsync/internal/TimeDomainId.h"
#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/timevalue.h"
#include "isoft/util/endian.h"

#define PACKED __attribute__((packed))

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

// PTP protocol frames can be encapsulated in two ways: directly based on Ethernet frames, or via UDP frames.
// When directly based on Ethernet frames, EtherType = 0x88F7 (PTP frame),
// When based on UDP, EtherType = 0x0800 (IP frame),

// PTP messages are divided into two categories: event messages and general messages.
// The sending and receiving of event messages require recording precise timestamps, while general messages do not.

// Keep 1-byte alignment
#pragma pack(push, 1)

/// @brief ControlFieldType
enum class ControlFieldType : std::uint8_t
{
    kCtrlSync       = 0x00,
    kCtrlDelayReq   = 0x01,
    kCtrlFollowup   = 0x02,
    kCtrlDelayResp  = 0x03,
    kCtrlManagement = 0x04,
    kCtrlAllOther   = 0x05,
};

/// @brief Type declaration
using Type = ara::tsync::internal::GptpType;

/// @brief PortIdentity
struct PortIdentity final
{
    /// @name clockIdentity
    uint64_t clockIdentity{0U};
    /// @name portNumber
    uint16_t portNumber{0U};
} PACKED;

/// @brief Header
class Header
{
public:
    /// @name kPtpTransportSpecific
    static constexpr std::uint8_t kPtpTransportSpecific{0x1U};  // 1 for IEEE 802.1AS-2011, 0 for 1588
    /// @name kPtpVersion
    static constexpr std::uint8_t kPtpVersion{0x02U};  // 10.5.2.2.3 versionPTP in IEEE 802.1AS-2011
    /// @brief kTypeFlag constant declaration
    static constexpr std::uint8_t kTypeFlag{0x0FU};

protected:
    /// @brief destructor
    ~Header() = default;

    /// @brief constructor
    Header() = default;

    /// @brief initialization function
    /// @param type type
    void _InitHeader(Type const type) noexcept
    {
        static_cast< void >(memset(&tsmt_, 0, sizeof(tsmt_)));
        SetType(type);
        SetTransportSpecific();
        SetPTPVersion();
    }

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    Header &operator=(Header const &other) = default;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    Header &operator=(Header &&other) = default;

    /// @brief copy constructor
    /// @param other - other object
    Header(Header const &other) = default;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    Header(Header &&) noexcept = default;

public:
    /// @brief
    void SetTransportSpecific() noexcept { tsmt_.tranSpec = kPtpTransportSpecific; }

    /// @brief set type
    /// @param messageType type
    void SetType(Type const messageType) noexcept
    {
        tsmt_.msgType = static_cast< std::uint8_t >(static_cast< std::uint8_t >(messageType) & kTypeFlag);
    }

    /// @brief set ptp version
    void SetPTPVersion() noexcept { ptpVersion_ = kPtpVersion; }

    /// @brief set message length
    /// @param messageLength message length
    void SetMessageLength(std::uint16_t const messageLength) noexcept
    {
        mesgLength_ = isoft::util::HtoN16(messageLength);
    }

    /// @brief set time domain id
    /// @param domainId time domain id
    void SetDomainId(internal::TimeDomainId const &domainId) noexcept { domainNumber_ = domainId.ToUint8(); }

    /// @brief
    /// @param byte0
    /// @param byte1
    void SetFlagField(std::uint8_t const byte0, std::uint8_t const byte1) noexcept
    {
        flagField_ = static_cast< uint16_t >(static_cast< std::uint32_t >(byte0)
                                             | (static_cast< std::uint32_t >(byte1) << kTS_NUM_8));
    }

    /// @brief
    /// @param correction
    void SetCorrectionField(std::uint64_t const correction) noexcept
    {
        correctionField_ = isoft::util::HtoN64(correction);
    }

    /// @brief
    /// @param clockIdentity
    void SetClockIdentity(std::uint64_t const clockIdentity) noexcept
    {
        sourcePortIdentity_.clockIdentity = isoft::util::HtoN64(clockIdentity);
    }

    /// @brief set source port number
    /// @param sourcePortNumber source port number
    void SetSourcePortNumber(std::uint16_t const sourcePortNumber) noexcept
    {
        sourcePortIdentity_.portNumber = isoft::util::HtoN16(sourcePortNumber);
    }

    /// @brief set sequence number
    /// @param sequenceNumber sequence number
    void SetSequenceID(std::uint16_t const sequenceNumber) noexcept
    {
        sequenceId_ = isoft::util::HtoN16(sequenceNumber);
    }

    /// @brief set control field type
    /// @param type control field type
    void SetControlField(ControlFieldType const type) noexcept { controlField_ = static_cast< std::uint8_t >(type); }

    /// @brief set log message interval
    /// @param messageInterval log message interval
    void SetLogMessageInterval(std::int8_t const messageInterval) noexcept { logMessageInterval_ = messageInterval; }

    //////////////////////////////////////////////////////////////////////////
    /// @brief
    /// @returns
    uint8_t GetTransportSpecific() const noexcept { return static_cast< uint8_t >(tsmt_.tranSpec); }

    /// @brief get type
    /// @returns type
    Type GetType() const noexcept { return static_cast< Type >(tsmt_.msgType); }

    /// @brief get ptp version
    /// @returns ptp version
    std::uint8_t GetPTPVersion() const noexcept { return ptpVersion_; }

    /// @brief get message length
    /// @returns message length
    std::uint16_t GetMessageLength() const noexcept { return isoft::util::NtoH16(mesgLength_); }

    /// @brief get time domain id
    /// @returns time domain id
    internal::TimeDomainId GetDomainId() const noexcept { return internal::TimeDomainId(domainNumber_); }

    /// @brief
    /// @returns
    std::uint16_t GetFlagField() const noexcept { return flagField_; }

    /// @brief
    /// @returns
    std::uint64_t GetCorrectionField() const noexcept { return isoft::util::NtoH64(correctionField_); }

    /// @brief
    /// @returns
    std::uint64_t GetClockIdentity() const noexcept { return isoft::util::NtoH64(sourcePortIdentity_.clockIdentity); }

    /// @brief get source port number
    /// @returns source port number
    std::uint16_t GetSourcePortNumber() const noexcept { return isoft::util::NtoH16(sourcePortIdentity_.portNumber); }

    /// @brief get sequence number
    /// @returns sequence number
    std::uint16_t GetSequenceID() const noexcept { return isoft::util::NtoH16(sequenceId_); }

    /// @brief
    /// @returns
    ControlFieldType GetControlField() const noexcept { return static_cast< ControlFieldType >(controlField_); }

    /// @brief
    /// @returns
    std::int8_t GetLogMessageInterval() const noexcept { return logMessageInterval_; }

    /// @brief print debug information
    void HeaderDebug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        std::cout << "=== PTPMessageHeader ===" << std::endl;
        std::cout << "MesgType: ";
        switch (GetType()) {
            case Type::kSync: {
                std::cout << "sync";
                break;
            }
            case Type::kFollowup: {
                std::cout << "folloup";
                break;
            }
            case Type::kPdelayReq: {
                std::cout << "pdelay_req";
                break;
            }
            case Type::kPdelayResp: {
                std::cout << "pdelay_resp";
                break;
            }
            default: {
                break;
            }
        }
        std::cout << std::endl;
        std::cout << "Version: " << std::dec << static_cast< std::int32_t >(GetPTPVersion()) << std::endl;
        std::cout << "MesgLen: " << std::dec << static_cast< std::int32_t >(GetMessageLength()) << std::endl;
        std::cout << "Domain:  " << std::dec << static_cast< std::int32_t >(GetDomainId().ToUint8()) << std::endl;
        std::cout << "Sequence:" << std::dec << static_cast< std::int32_t >(GetSequenceID()) << std::endl;
#endif
    }

private:
    /// @traceid{PRS_TS_00028} Unless otherwise specified, the message format should follow IEEE802.1AS.
    /// @traceid{PRS_TS_00181} The byte order of multi-byte data is big-endian.
    struct Msg final
    {
        uint8_t msgType : 4;
        uint8_t tranSpec : 4;
    };
    /// @name tsmt_ transportSpecific | messageType
    Msg tsmt_{};
    /// @name ptpVersion_ 4bit
    uint8_t ptpVersion_{2U};
    /// @name mesgLength_
    uint16_t mesgLength_{0U};
    /// @name domainNumber_
    uint8_t domainNumber_{0U};
    /// @name reserved1_
    uint8_t reserved1_{0U};
    /// @name flagField_
    uint16_t flagField_{0U};
    /// @name correctionField_
    uint64_t correctionField_{0U};
    /// @name reserved2_
    uint32_t reserved2_{0U};
    /// @name sourcePortIdentity_
    PortIdentity sourcePortIdentity_{};
    /// @name sequenceId_
    uint16_t sequenceId_{0U};
    /// @name controlField_
    uint8_t controlField_{0U};
    /// @name logMessageInterval_
    int8_t logMessageInterval_{0U};

};  // class Header
#pragma pack(pop)

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  //_ARA_TSYNC_INTERNAL_PTP_MESSAGE_HEADER_H_
