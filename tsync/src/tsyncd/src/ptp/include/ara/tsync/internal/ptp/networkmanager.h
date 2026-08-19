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
/// @file       networkmanager.h
/// @brief      network management class
/// @details
/// @date       2023-01-11
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/Ptp
/// module_path=/TimeSync/Ptp
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_PTP_NETWORKMANAGER_H_
#define ARA_TSYNC_INTERNAL_PTP_NETWORKMANAGER_H_

#include <ara/core/map.h>
#include <isoft/naicpp/global_evloop.h>

#include <chrono>
#include <cstdint>
#include <memory>

#include "ara/tsync/internal/config/configmanager.h"
#include "ara/tsync/internal/ptp/network.h"
#include "ara/tsync/internal/timedomain/manager.h"
#include "isoft/osi/network/device.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {

/// @brief network management class
class NetworkManager final
{
public:
    /// @brief Address
    using Address = isoft::osi::network::Ether::Address;

    /// @brief constructor
    /// @param timeDomain
    explicit NetworkManager(std::shared_ptr< timedomain::TDManager > timeDomain) noexcept
        : timeDomainManager_{std::move(timeDomain)}, networkMap_{}
    {
    }

    /// @brief destructor
    ~NetworkManager() noexcept
    {
        for (auto const &item : networkMap_) {
            networkMap_[item.first] = nullptr;
        }
        networkMap_.clear();
        timeDomainManager_ = nullptr;
    };

    /// @brief copy constructor is prohibited
    /// @param other - other object
    NetworkManager(NetworkManager const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    NetworkManager &operator=(NetworkManager const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    NetworkManager(NetworkManager &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    NetworkManager &operator=(NetworkManager &&) &noexcept = default;

    /// @brief create network manager
    /// @param timeDomain - time domain management handle
    /// @return handle, or nullptr.
    static std::shared_ptr< NetworkManager > CreateManager(
        std::shared_ptr< timedomain::TDManager > const &timeDomain) noexcept;

    /// @brief MessageRecvedHandler declaration
    using MessageRecvedHandler = Network::MessageRecvedHandler;

    /// @brief asynchronously receive network data packet.
    /// @param connector network card
    /// @param cb receive callback function.
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    void OnRecv(ara::core::String const &connector, MessageRecvedHandler const &cb) noexcept
    {
        networkMap_[connector]->OnRecv(cb);
    }

    /// @brief synchronously send network data packet.
    /// @param mesg - message buffer to send.
    /// @param mesgSize - length of message to send.
    /// @param destAddr - destination address.
    /// @param vlanPri - VLAN priority, if -1, no VLAN tag is set.
    /// @param vlanId - VLAN id。
    /// @param connector - network card.
    /// @param sendTimeStamp - system timestamp at sending completion, returned to user.
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    std::int32_t Send(void const *const mesg,
                      std::uint16_t const mesgSize,
                      Address const &destAddr,
                      std::int8_t const vlanPri,
                      std::int16_t const vlanId,
                      ara::core::String const &connector,
                      std::chrono::nanoseconds &sendTimeStamp) noexcept
    {
        return networkMap_[connector]->Send(mesg, mesgSize, destAddr, vlanPri, vlanId, sendTimeStamp);
    }
    /// @brief expected phc clock value when Sync is received, used to adjust phc clock
    /// @param type - clock type
    /// @param timeExpect - expected phc clock value
    /// @param connector - network card.
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    std::int32_t AdjustClockTime(ClockAdjust type,
                                 std::int64_t const timeExpect,
                                 ara::core::String const &connector) noexcept
    {
        return networkMap_[connector]->AdjustClockTime(type, timeExpect);
    }

private:
    /// @brief initialize
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _init() noexcept;

    /// @brief initialize context
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _initContext() noexcept;

private:
    /// @name timeDomainManager_ - time domain manager handle
    std::shared_ptr< timedomain::TDManager > timeDomainManager_;

    ara::core::Map< ara::core::String, std::shared_ptr< Network > >
        /// @name networkMap_ network handle
        networkMap_;
};  /// class NetworkManager

}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_PTP_NETWORKMANAGER_H_
