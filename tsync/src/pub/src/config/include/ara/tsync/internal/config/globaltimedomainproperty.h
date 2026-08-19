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
/// @file       globaltimedomainproperty.h
/// @brief      global time domain property configuration management class
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

#ifndef ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMEDOMAINPROPERTY_H_
#define ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMEDOMAINPROPERTY_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>

#include <cstdint>
#include <cstring>

#include "ara/tsync/internal/config/common.h"
#include "ara/tsync/internal/config/crcflags.h"
#include "ara/tsync/internal/config/managedcouplingport.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief time domain message format enumeration definition
enum class GlobalTimeMessageFormat : std::uint8_t
{
    kIEEE802_1AS         = 0,
    kIEEE802_1AS_AUTOSAR = 1
};

/// @brief GlobalTimeDomainProperty
class GlobalTimeDomainProperty final
{
public:
    /// @brief load function required by manifestReader interface
    /// @param node - manifest node
    /// @return kSucess/ErrorCode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        /// @name kCrcFlags
        static constexpr ara::core::StringView kCrcFlags{"crcFlags"};
        /// @name kDestinationPhysicalAddress
        static constexpr ara::core::StringView kDestinationPhysicalAddress{"destinationPhysicalAddress"};
        /// @name kFupDataIDList
        static constexpr ara::core::StringView kFupDataIDList{"fupDataIDList"};
        /// @name kManagedCouplingPort
        static constexpr ara::core::StringView kManagedCouplingPort{"managedCouplingPort"};
        /// @name kMessageCompliance
        static constexpr ara::core::StringView kMessageCompliance{"messageCompliance"};
        /// @name kVlanPriority
        static constexpr ara::core::StringView kVlanPriority{"vlanPriority"};

        static_cast< void >(node.Load(kCrcFlags, this->crcFlags_));
        this->destinationPhysicalAddress_ = node.GetValue(kDestinationPhysicalAddress, ara::core::String());
        static_cast< void >(node.Load(kFupDataIDList, this->fupDataIDList_));
        static_cast< void >(node.Load(kManagedCouplingPort, this->managedCouplingPort_));
        ara::core::String const msg{node.GetValue(kMessageCompliance, ara::core::String())};
        ara::core::StringView const svMsg{msg.c_str(), msg.length()};
        if (svMsg == _getKeyIEEE802_1AS_AUTOSAR()) {
            this->messageCompliance_ = GlobalTimeMessageFormat::kIEEE802_1AS_AUTOSAR;
        } else if (svMsg == _getKeyIEEE802_1AS()) {
            this->messageCompliance_ = GlobalTimeMessageFormat::kIEEE802_1AS;
        } else {
            LOG().Error() << "invalid " << kMessageCompliance << "{" << msg << "}";
            return kRET_E1;
        }
        std::int8_t const defVal{-1};
        this->vlanPriority_ = node.GetValue(kVlanPriority, defVal);

        return isoft::kSuccess;
    };

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        std::cout << "crcFlags: " << std::endl;
        crcFlags_.Debug();
        std::cout << "destinationPhysicalAddress: " << destinationPhysicalAddress_ << std::endl;
        std::cout << "fupDataIDList: ";
        for (std::uint8_t const dataId : fupDataIDList_) {
            std::cout << dataId << " ";
        }
        std::cout << std::endl;
        std::cout << "managedCouplingPort: " << std::endl;
        managedCouplingPort_.Debug();
        std::cout << "messageCompliance:";
        if (GlobalTimeMessageFormat::kIEEE802_1AS == messageCompliance_) {
            std::cout << _getKeyIEEE802_1AS();
        } else {
            std::cout << _getKeyIEEE802_1AS_AUTOSAR();
        }
        std::cout << std::endl;
        std::cout << "vlanPriority: " << vlanPriority_ << std::endl;
#endif
    };

    /// @brief get time domain master name
    /// @traceid {}
    /// @return time domain master name
    CrcFlags const &GetCrcFlags() const noexcept { return crcFlags_; }

    /// @brief get time domain master name
    /// @traceid {}
    /// @return time domain master name
    ara::core::String const &GetDestinationPhysicalAddress() const noexcept { return destinationPhysicalAddress_; }

    /// @brief get time domain master name
    /// @return time domain master name
    ara::core::Vector< std::uint8_t > const &GetFupDataIDList() const noexcept { return fupDataIDList_; }

    /// @brief get the managed coupling port configuration of the time domain master
    /// @traceid {}
    /// @return managed coupling port configuration
    ManagedCouplingPort const &GetManagedCouplingPort() const noexcept { return managedCouplingPort_; }

    /// @brief get time domain master name
    /// @traceid {}
    /// @return time domain master name
    GlobalTimeMessageFormat GetMessageCompliance() const noexcept { return messageCompliance_; }

    /// @brief get 802.1q priority
    /// @traceid {}
    /// @return 802.1q priority
    std::int8_t GetVlanPriority() const noexcept { return vlanPriority_; }

private:
    inline ara::core::StringView _getKeyIEEE802_1AS() const noexcept { return ara::core::StringView{"IEEE802-1AS"}; }
    inline ara::core::StringView _getKeyIEEE802_1AS_AUTOSAR() const noexcept
    {
        return ara::core::StringView{"IEEE802-1AS-AUTOSAR"};
    }

private:
    /// @name crcFlags_
    CrcFlags crcFlags_;
    /// @name destinationPhysicalAddress_
    ara::core::String destinationPhysicalAddress_;
    /// @name fupDataIDList_
    ara::core::Vector< std::uint8_t > fupDataIDList_;
    /// @name managedCouplingPort_
    ManagedCouplingPort managedCouplingPort_;
    /// @name messageCompliance_
    GlobalTimeMessageFormat messageCompliance_;
    /// @name vlanPriority_
    std::int8_t vlanPriority_;
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMEDOMAINPROPERTY_H_
