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
/// @file       ptpcontext.h
/// @brief      PTP management class context
/// @details
/// @date       2023-01-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_PTP_CONTEXT_H_
#define ARA_TSYNC_INTERNAL_PTP_CONTEXT_H_

#include <chrono>
#include <cstdint>
#include <functional>

#include "ara/tsync/internal/ptp/configure.h"
#include "ara/tsync/internal/ptp/event.h"
#include "ara/tsync/internal/ptp/message/header.h"
#include "ara/tsync/internal/ptp/network.h"
#include "ara/tsync/internal/timedomain/tdcontext.h"
#include "isoft/naicpp/evloop.h"
#include "nai/io/nai_io.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {

/// @brief PTP context. Saves all information required for PTP protocol communication, including E2E/P2P timestamps, network addresses, etc.
class PtpContext final
{
public:
    /// @brief default constructor
    /// @exception exception specification
    PtpContext() = default;

    /// @brief destructor
    ~PtpContext() noexcept { timeDomainContext_ = nullptr; }

    /// @brief reset timer
    void ResetTimer() noexcept
    {
        timeDomainContext_ = nullptr;
        syncPeriodTimer_   = nullptr;
        syncLossTimer_     = nullptr;
        pdelayTimer_       = nullptr;
    }
    /// @brief copy constructor is prohibited
    /// @param other - other object
    PtpContext(PtpContext const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    PtpContext &operator=(PtpContext const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    PtpContext(PtpContext &&other) noexcept = delete;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    PtpContext &operator=(PtpContext &&other) &noexcept = delete;

    /// @brief check whether it is valid
    /// @returns true valid false invalid
    bool IsValid() const noexcept
    {
        if (nullptr != timeDomainContext_) {
            return true;
        }
        return false;
    }

    /// @traceid{PRS_TS_00097} DataID = DataIDList[Followup.seqID % 16];
    // FIXME: When the receiver verifies the Followup message, if the SeqId of the received fup packet does not match the SeqId of the current Sync packet, an error will occur.
    /// @brief get DataID. Calculation method: fupDataIDList[seqID % 16]
    /// @returns  DataID
    std::uint8_t GetDataID() const noexcept
    {
        if (nullptr == timeDomainContext_) {
            return 0U;
        }
        /// @brief vsize_type;
        using vsize_type = ara::core::Vector< std::uint8_t >::size_type;  // NOLINT
        vsize_type const kSid{timeDomainContext_->GetSyncSeqId()};
        vsize_type const fupIndex{kSid % 16U};
        if (fupIndex >= ptpConfig_.fupDataIDList.size()) {
            return 0U;
        }
        return ptpConfig_.fupDataIDList[fupIndex];
    }

    /// @brief get Sync periodic sending timer
    /// @returns reference to Sync periodic sending timer
    isoft::naicpp::EvLoop::TimerPtr &SyncPeriodTimer() noexcept { return syncPeriodTimer_; }

    /// @brief get Sync loss timeout detection timer
    /// @returns reference to Sync loss timeout detection timer
    isoft::naicpp::EvLoop::TimerPtr &SyncLossTimer() noexcept { return syncLossTimer_; }

    /// @brief get Pdelay timer
    /// @returns reference to Pdelay timer
    isoft::naicpp::EvLoop::TimerPtr &PdelayTimer() noexcept { return pdelayTimer_; }

    /// @brief set network communication address
    /// @param srcAddr network communication address
    void SetPeerAddr(Network::Address const &srcAddr) noexcept { peerAddr_ = srcAddr; }

    /// @brief get network communication address
    /// @returns network communication address
    Network::Address const &GetPeerAddr() const noexcept { return peerAddr_; }

    /// @brief set time domain context pointer
    /// @param pTDContext time domain context pointer
    void SetTimeDomainContext(std::shared_ptr< timedomain::TDContext > const &pTDContext) noexcept
    {
        timeDomainContext_ = pTDContext;
    }

    /// @brief get time domain context pointer
    /// @returns time domain context pointer
    std::shared_ptr< timedomain::TDContext > const GetTimeDomainContext() const noexcept { return timeDomainContext_; }

    /// @brief get time domain protocol layer related configuration
    /// @returns time domain protocol layer related configuration
    Configure const &PtpConfig() const noexcept { return ptpConfig_; }

    /// @brief get time domain protocol layer related configuration
    /// @returns time domain protocol layer related configuration
    Configure &PtpConfig() noexcept { return ptpConfig_; }
    /// @brief set protocol layer identifier
    /// @param ident protocol layer identifier
    void SetSrcPortIdentity(message::PortIdentity const &ident) noexcept { srcPortIdentity_ = ident; }

    /// @brief get protocol layer identifier
    /// @returns protocol layer identifier
    message::PortIdentity const &GetSrcPortIdentity() const noexcept { return srcPortIdentity_; }

private:
    /// @name syncPeriodTimer Sync periodic sending timer
    isoft::naicpp::EvLoop::TimerPtr syncPeriodTimer_{nullptr};
    /// @name syncLossTimer Sync loss timeout detection timer
    isoft::naicpp::EvLoop::TimerPtr syncLossTimer_{nullptr};
    /// @name pdelayTimer Pdelay timer
    isoft::naicpp::EvLoop::TimerPtr pdelayTimer_{nullptr};

    /// @name peerAddr network communication address, temporarily used to store the requester's address for individual response.
    Network::Address peerAddr_{};

    /// @name timeDomainContext  time domain context pointer
    std::shared_ptr< timedomain::TDContext > timeDomainContext_{nullptr};

    /// @name ptpConfig time domain protocol layer related configuration
    Configure ptpConfig_{};

    /// @name srcPortIdentity protocol layer
    struct message::PortIdentity srcPortIdentity_
    {
    };
};  /// class PtpContext

}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_PTP_CONTEXT_H_
