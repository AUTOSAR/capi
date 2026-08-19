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
/// @file       globaltimemaster.h
/// @brief      global time domain master configuration management class
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

#ifndef ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMEMASTER_H_
#define ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMEMASTER_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <isoft/manifestreader/manifest_node.h>

#include <cstdint>
#include <cstring>

#include "ara/tsync/internal/config/common.h"
#include "ara/tsync/internal/config/subtlvconfig.h"
namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief time domain CRC support enumeration definition
enum class GlobalTimeCrcSupport : std::uint8_t
{
    kCrcSupported = 1,
    kCrcNotSupported
};

/// @brief GlobalTimeMaster
class GlobalTimeMaster final
{
public:
    /// @brief load function required by manifestReader interface
    /// @param node - manifest node
    /// @return kSucess/ErrorCode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        this->name_                         = node.GetValue(_getKeyName(), ara::core::String());
        this->immediateResumeTime_          = node.GetValue(_getKeyImmediateResumeTime(), 0.0);
        this->isSystemWideGlobalTimeMaster_ = node.GetValue(_getKeyIsSystemWideGlobalTimeMaster(), false);
        this->syncPeriod_                   = node.GetValue(_getKeySyncPeriod(), 0.0);
        this->ethernetConnector_            = node.GetValue(_getKeyEthernetConnector(), ara::core::String());
        std::int16_t const defValId{-1};
        this->vlanId_ = node.GetValue(_getKeyVlanId(), defValId);

        ara::core::String const crc{node.GetValue(_getKeyCrcSecured(), ara::core::String())};
        ara::core::StringView const svCrc{crc.c_str(), crc.length()};
        if (0 == svCrc.compare(_getKeyCrcSupported())) {
            this->crcSecured_ = GlobalTimeCrcSupport::kCrcSupported;
        } else if (0 == svCrc.compare(_getKeyCrcNotSupported())) {
            this->crcSecured_ = GlobalTimeCrcSupport::kCrcNotSupported;
        } else {
            LOG().Warn() << "lack of {" << _getKeyCrcSecured() << "}, using default value.";
        }

        static_cast< void >(node.Load(_getKeySubTlvConfig(), this->subTlvConfig_));
        return isoft::kSuccess;
    };

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        std::cout << "name: " << name_ << std::endl;
        std::cout << "immediateResumeTime: " << immediateResumeTime_ << std::endl;
        std::cout << "isSystemWideGlobalTimeMaster: " << isSystemWideGlobalTimeMaster_ << std::endl;
        std::cout << "syncPeriod: " << syncPeriod_ << std::endl;
        std::cout << "crcSecured: ";
        switch (crcSecured_) {
            case GlobalTimeCrcSupport::kCrcSupported: {
                std::cout << _getKeyCrcSupported() << std::endl;
                break;
            }
            case GlobalTimeCrcSupport::kCrcNotSupported: {
                std::cout << _getKeyCrcNotSupported() << std::endl;
                break;
            }
        }
        std::cout << "subTlvConfig: " << std::endl;
        subTlvConfig_.Debug();
#endif
    };

    /// @brief get time domain master name
    /// @traceid {}
    /// @return time domain master name
    ara::core::String const &GetName() const noexcept { return name_; }

    /// @brief get the minimum time interval of the time domain master (minimum interval between SetTime() sending messages and periodic sending)
    /// @traceid {}
    /// @return immediate time minimum interval (minimum interval between SetTime() sending messages and periodic sending)
    double GetImmediateResumeTime() const noexcept { return immediateResumeTime_; }

    /// @brief get whether the time domain master is the global master
    /// @traceid {}
    /// @return whether it is the global master
    bool IsSystemWideGlobalTimeMaster() const noexcept { return isSystemWideGlobalTimeMaster_; }

    /// @brief get the Sync transmission period of the time domain master
    /// @traceid {}
    /// @return Sync transmission period
    double GetSyncPeriod() const noexcept { return syncPeriod_; }

    /// @brief get whether the time domain master supports crc security
    /// @traceid {}
    /// @return whether crc security is supported
    GlobalTimeCrcSupport GetCrcSecured() const noexcept { return crcSecured_; }

    /// @brief get the subtlv configuration of the time domain master
    /// @traceid {}
    /// @return subtlv configuration
    SubTlvConfig const &GetSubTlvConfig() const noexcept { return subTlvConfig_; }

    /// @brief get the network card IP of the time domain master
    /// @traceid {}
    /// @return subtlv configuration
    ara::core::String const &GetEthernetConnector() const noexcept { return ethernetConnector_; }

    /// @brief get the 802.1q vlanid of the message encapsulation of the time domain master
    /// @traceid {}
    /// @return 802.1q vlanid of the message encapsulation
    std::int16_t GetVlanId() const noexcept { return vlanId_; }

private:
    inline ara::core::StringView _getKeyName() const noexcept { return ara::core::StringView{"name"}; }
    inline ara::core::StringView _getKeyImmediateResumeTime() const noexcept
    {
        return ara::core::StringView{"immediateResumeTime"};
    }
    inline ara::core::StringView _getKeyIsSystemWideGlobalTimeMaster() const noexcept
    {
        return ara::core::StringView{"isSystemWideGlobalTimeMaster"};
    }
    inline ara::core::StringView _getKeySyncPeriod() const noexcept { return ara::core::StringView{"syncPeriod"}; }
    inline ara::core::StringView _getKeyCrcSecured() const noexcept { return ara::core::StringView{"crcSecured"}; }
    inline ara::core::StringView _getKeySubTlvConfig() const noexcept { return ara::core::StringView{"subTlvConfig"}; }
    inline ara::core::StringView _getKeyCrcSupported() const noexcept { return ara::core::StringView{"CRC-SUPPORTED"}; }
    inline ara::core::StringView _getKeyCrcNotSupported() const noexcept
    {
        return ara::core::StringView{"CRC-NOT-SUPPORTED"};
    }
    inline ara::core::StringView _getKeyEthernetConnector() const noexcept
    {
        return ara::core::StringView{"ethernetConnector"};
    }
    inline ara::core::StringView _getKeyVlanId() const noexcept { return ara::core::StringView{"vlanId"}; }

private:
    /// @name name
    ara::core::String name_;
    /// @name immediateResumeTime_
    double immediateResumeTime_{0.0};
    /// @name isSystemWideGlobalTimeMaster_
    bool isSystemWideGlobalTimeMaster_{false};
    /// @name syncPeriod_
    double syncPeriod_{0.0};
    /// @name crcSecured_
    GlobalTimeCrcSupport crcSecured_{GlobalTimeCrcSupport::kCrcNotSupported};
    /// @name subTlvConfig_
    SubTlvConfig subTlvConfig_;

    /// @name ethernetConnector_
    ara::core::String ethernetConnector_;

    /// @name vlanId_
    std::int16_t vlanId_;
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMEMASTER_H_
