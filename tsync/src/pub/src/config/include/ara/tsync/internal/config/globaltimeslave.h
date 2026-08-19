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
/// @file       globaltimeslave.h
/// @brief      global time domain slave configuration management class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/Config
/// module_path=/TimeSync/Config
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMESLAVE_H_
#define ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMESLAVE_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <isoft/manifestreader/manifest_node.h>

#include <cstdint>
#include <cstring>

#include "ara/tsync/internal/config/common.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief GlobalTimeSlave
class GlobalTimeSlave final
{
public:
    /// @brief get time domain slave name
    /// @traceid {}
    /// @return time domain slave name
    ara::core::String const& GetName() const noexcept { return name_; }

    /// @brief get followup message timeout for time domain slave
    /// @traceid {}
    /// @return followup message timeout
    double GetFollowUpTimeoutValue() const noexcept { return followUpTimeoutValue_; }

    /// @brief get jump to future threshold for time domain slave
    /// @traceid {}
    /// @return jump to future threshold
    double GetTimeLeapFutureThreshold() const noexcept { return timeLeapFutureThreshold_; }

    /// @brief get jump to past threshold for time domain slave
    /// @traceid {}
    /// @return jump to past threshold
    double GetTimeLeapPastThreshold() const noexcept { return timeLeapPastThreshold_; }

    /// @brief get the number of consecutive times below threshold required to jump to none
    /// @traceid {}
    /// @return the number of consecutive times below threshold required to jump to none
    std::uint16_t GetTimeLeapHealingCounter() const noexcept { return timeLeapHealingCounter_; }

    /// @brief get crc verification type for time domain slave
    /// @traceid {}
    /// @return crc verification type
    GlobalTimeCrcValidation GetCrcValidated() const noexcept { return crcValidated_; }

    /// @brief get network card IP for time domain slave
    /// @traceid {}
    /// @return network card IP for time domain slave
    ara::core::String const& GetEthernetConnector() const noexcept { return ethernetConnector_; }

    /// @brief get the 802.1q vlanid of the message encapsulation header for time domain slave
    /// @traceid {}
    /// @return 802.1q vlanid of the message encapsulation header
    std::int16_t GetVlanId() const noexcept { return vlanId_; }

    /// @brief load function required by manifestReader interface
    /// @param node - manifest node
    /// @return kSucess/ErrorCode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
    {
        this->name_                    = node.GetValue(_getKeyName(), ara::core::String());
        this->followUpTimeoutValue_    = node.GetValue(_getKeyFollowupTimeoutValue(), 0.0);
        this->timeLeapFutureThreshold_ = node.GetValue(_getKeyTimeLeapFutureThreshold(), 0.0);
        this->timeLeapPastThreshold_   = node.GetValue(_getKeyTimeLeapPastThreshold(), 0.0);
        std::uint16_t const defVal{0U};
        this->timeLeapHealingCounter_ = node.GetValue(_getKeyTimeLeapHealingCounter(), defVal);
        ara::core::String const crc{node.GetValue(_getKeyKeyCrcValidated(), ara::core::String())};
        ara::core::StringView const svCrc{crc.c_str(), crc.length()};
        if (0 == svCrc.compare(_getKeyCrcValidated())) {
            this->crcValidated_ = GlobalTimeCrcValidation::kCrcValidated;
        } else if (0 == svCrc.compare(_getKeyCrcNotValidated())) {
            this->crcValidated_ = GlobalTimeCrcValidation::kCrcNotValidated;
        } else if (0 == svCrc.compare(_getKeyCrcOptional())) {
            this->crcValidated_ = GlobalTimeCrcValidation::kCrcOptional;
        } else {
            this->crcValidated_ = GlobalTimeCrcValidation::kCrcIgnord;
            LOG().Warn() << "lack of {" << _getKeyCrcValidated() << "}, using default value {_getKeyCrcIgnord()}";
        }
        this->ethernetConnector_ = node.GetValue(_getKeyEthernetConnector(), ara::core::String());
        std::int16_t const defValId{-1};
        this->vlanId_ = node.GetValue(_getKeyVlanId(), defValId);

        return isoft::kSuccess;
    }

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        std::cout << "name: " << name_ << std::endl;
        std::cout << "followUpTimeoutValue: " << followUpTimeoutValue_ << std::endl;
        std::cout << "timeLeapFutureThreshold: " << timeLeapFutureThreshold_ << std::endl;
        std::cout << "timeLeapPastThreshold: " << timeLeapPastThreshold_ << std::endl;
        std::cout << "timeLeapHealingCounter: " << timeLeapHealingCounter_ << std::endl;
        std::cout << "crcValidated: ";
        switch (crcValidated_) {
            case GlobalTimeCrcValidation::kCrcIgnord: {
                std::cout << "CrcIgnord";
                break;
            }
            case GlobalTimeCrcValidation::kCrcNotValidated: {
                std::cout << "_getKeyCrcNotValidated()";
                break;
            }
            case GlobalTimeCrcValidation::kCrcOptional: {
                std::cout << "_getKeyCrcOptional()";
                break;
            }
            case GlobalTimeCrcValidation::kCrcValidated: {
                std::cout << "_getKeyCrcValidated()";
                break;
            }
        }
        std::cout << std::endl;
#endif
    }

private:
    inline ara::core::StringView _getKeyName() const noexcept { return ara::core::StringView{"name"}; }
    inline ara::core::StringView _getKeyFollowupTimeoutValue() const noexcept
    {
        return ara::core::StringView{"followUpTimeoutValue"};
    }
    inline ara::core::StringView _getKeyTimeLeapFutureThreshold() const noexcept
    {
        return ara::core::StringView{"timeLeapFutureThreshold"};
    }
    inline ara::core::StringView _getKeyTimeLeapPastThreshold() const noexcept
    {
        return ara::core::StringView{"timeLeapPastThreshold"};
    }
    inline ara::core::StringView _getKeyTimeLeapHealingCounter() const noexcept
    {
        return ara::core::StringView{"timeLeapHealingCounter"};
    }
    inline ara::core::StringView _getKeyKeyCrcValidated() const noexcept
    {
        return ara::core::StringView{"crcValidated"};
    }
    inline ara::core::StringView _getKeyCrcValidated() const noexcept { return ara::core::StringView{"CRC-VALIDATED"}; }
    inline ara::core::StringView _getKeyCrcNotValidated() const noexcept
    {
        return ara::core::StringView{"CRC-NOT-VALIDATED"};
    }
    inline ara::core::StringView _getKeyCrcIgnord() const noexcept { return ara::core::StringView{"CRC-IGNORD"}; }
    inline ara::core::StringView _getKeyCrcOptional() const noexcept { return ara::core::StringView{"CRC-OPTIONAL"}; }
    inline ara::core::StringView _getKeyEthernetConnector() const noexcept
    {
        return ara::core::StringView{"ethernetConnector"};
    }
    inline ara::core::StringView _getKeyVlanId() const noexcept { return ara::core::StringView{"vlanId"}; }

private:
    /// @name name
    ara::core::String name_;
    /// @name followUpTimeoutValue
    double followUpTimeoutValue_;
    /// @name timeLeapFutureThreshold
    double timeLeapFutureThreshold_;
    /// @name timeLeapPastThreshold
    double timeLeapPastThreshold_;
    /// @name timeLeapHealingCounter
    std::uint16_t timeLeapHealingCounter_;
    /// @name crcValidated
    GlobalTimeCrcValidation crcValidated_;

    /// @name ethernetConnector
    ara::core::String ethernetConnector_;

    /// @name vlanId
    std::int16_t vlanId_;
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMESLAVE_H_
