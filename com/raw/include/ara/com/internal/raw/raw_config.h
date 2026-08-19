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
/// @file       raw_config.h
/// @brief      Raw data stream config header file
/// @details
/// @date       2021-12-02
/// @author     cuijiusen
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_INTERNAL_RAW_RAW_CONFIG_H
#define ARA_COM_INTERNAL_RAW_RAW_CONFIG_H

#include "ara/com/internal/log/log.h"
#include "ara/core/array.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/map.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "isoft/define.h"
#include "isoft/manifestreader/manifest_node.h"
#include "isoft/manifestreader/manifest_reader.h"

namespace ara {
namespace com {
namespace internal {
namespace raw {
/// @brief Raw configuration class
struct RawConfig final
{
public:
    /// @brief RawConfig
    RawConfig() = default;
    /// @brief ~RawConfig
    ~RawConfig() = default;
    /// @brief Copy constructor
    /// @param[in] other the object to be copy.
    RawConfig(RawConfig const& other) = default;
    /// @brief Copy assignment function
    /// @param[in] other the object to be copy.
    /// @return RawConfig object
    RawConfig& operator=(RawConfig const& other) = default;
    /// @brief Move constructor
    /// @param[in] other the object to be move.
    RawConfig(RawConfig&& other) = default;
    /// @brief Move assignment function
    /// @param[in] other the object to be move.
    /// @return RawConfig object
    RawConfig& operator=(RawConfig&& other) = default;

public:
    /// @brief Instance Specifier
    ara::core::String instanceSpecifier{};
    /// @brief socket options
    ara::core::Vector< ara::core::String > socketOption{};
    /// @brief server bind tcp port
    uint16_t tcpPort{};
    /// @brief server bind udp port
    uint16_t udpPort{};
    /// @brief multicast port
    uint16_t multicastUdp{};
    /// @brief max transmission Unit
    uint16_t maximumTransmissionUnit{};
    /// @brief pant mtu timeout
    uint16_t pathMtuTimeout{};
    /// @brief local ip address
    ara::core::String localAddress{};
    /// @brief remote ip address
    ara::core::String remoteAddress{};
    /// @brief multicast ip address
    ara::core::String multicastAddress{};
    /// @brief ca path
    ara::core::String ca{};
    /// @brief certificate path
    ara::core::String crt{};
    /// @brief key path
    ara::core::String key{};
    /// @brief ca password
    ara::core::String password{};

    /// @brief Parse config node
    /// @param[in] node Manifest Node class
    /// @return 0 if successfull, -1 error.
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
    {
        ara::core::StringView const stInstanceSpecifier{"instance_specifier"};
        ara::core::StringView const stSocketOption{"socketOption"};
        ara::core::StringView const stMaximumTransmissionUnit{"maximumTransmissionUnit"};
        ara::core::StringView const stTcpPort{"tcpPort"};
        ara::core::StringView const stMulticastUdp{"multicastUdp"};
        ara::core::StringView const stUdpPort{"udpPort"};
        ara::core::StringView const stPathMtuTimeout{"pathMtuTimeout"};
        ara::core::StringView const stMulticastAddress{"ipAddress"};
        ara::core::StringView const stRemoteAddress{"ipAddress"};
        ara::core::StringView const stLocalAddress{"ipAddress"};
        ara::core::StringView const stCa{"ca"};
        ara::core::StringView const stCrt{"crt"};
        ara::core::StringView const stKey{"key"};
        ara::core::StringView const stPassword{"password"};
        if (isoft::kSuccess != node.Load(stInstanceSpecifier, instanceSpecifier)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(stSocketOption, socketOption)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(stMaximumTransmissionUnit, maximumTransmissionUnit)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(stTcpPort, tcpPort)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(stMulticastUdp, multicastUdp)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(stUdpPort, udpPort)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(stPathMtuTimeout, pathMtuTimeout)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(stMulticastAddress, multicastAddress)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(stRemoteAddress, remoteAddress)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(stLocalAddress, localAddress)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(stCa, ca)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(stCrt, crt)) {
            return isoft::kFailure;
        }

        if (isoft::kSuccess != node.Load(stKey, key)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load(stPassword, password)) {
            return isoft::kFailure;
        }
        return isoft::kSuccess;
    }
};
/// @brief Raw mapping config management
class RawMappingManager final
{
    /// @brief the pointer type of RawConfig
    using RawConfigPtr = std::shared_ptr< RawConfig >;

    /// @brief raw config map
    using RawConfigMapping = ara::core::Map< ara::core::InstanceSpecifier, RawConfigPtr >;

    /// @brief ara::core::Map<ara::core::InstanceSpecifier, RawConfigPtr>::iterator
    using RawConfigMappingIterator = ara::core::Map< ara::core::InstanceSpecifier, RawConfigPtr >::iterator;

public:
    /// @brief Get raw instance descriptor
    /// @param[in] instanceSpecifier raw instance specifier
    /// @return RawConfig point successfull ,nullptr error
    static RawConfigPtr GetRawConfig(ara::core::InstanceSpecifier const& instanceSpecifier) noexcept
    {
        return Impl::Instance().GetRawConfigImpl(instanceSpecifier);
    }

    /// @brief Load raw configuration
    /// @param[in] path raw config file path
    /// @return true successfull ,false error.
    static bool LoadConfig(ara::core::StringView const& path) noexcept { return Impl::Instance().LoadConfigImpl(path); }

private:
    /// @brief impl class
    class Impl final
    {
    public:
        /// @brief get the impl instance
        /// @return impl instance
        static Impl& Instance() noexcept
        {
            static Impl s_Instance;
            return s_Instance;
        }
        /// @brief Load raw configuration implementation interface
        /// @param[in] path the config file path
        /// @return ture load config succed ,false failed
        inline bool LoadConfigImpl(ara::core::StringView const& path) noexcept
        {
            /// @brief the unique pointer of isoft::manifestreader::Manifest
            using ManifestResPtr = std::unique_ptr< isoft::manifestreader::Manifest >;
            if (configured_) {
                ComLogInfo("raw was already configured, skipping");
                return true;
            }
            ara::core::Result< ManifestResPtr > const manifestRes{isoft::manifestreader::OpenManifest(path)};
            if (!manifestRes) {
                return false;
            }
            ara::core::Vector< RawConfig > cnf{};
            ara::core::StringView const kKey{"ethernet_raw_data_stream_mapping"};
            int32_t ret{manifestRes.Value()->Load(kKey, cnf)};
            if (ret == isoft::kSuccess) {
                ComLogInfo("raw load success!");
                for (auto const& it : cnf) {
                    ara::core::StringView const instanceSpecifier{it.instanceSpecifier.c_str()};
                    ara::core::InstanceSpecifier const instanceIdentifier{instanceSpecifier};
                    rawConfigMappings_[instanceIdentifier] = std::make_shared< RawConfig >(it);
                }
                configured_ = true;
                return true;
            }
            ComLogError("LoadConfigImpl:raw load failed!");
            configured_ = false;
            return false;
        }
        /// @brief Get raw configuration instance descriptor
        /// @param[in] instanceSpecifier raw instance specifier
        /// @return RawConfig point successfull ,nullptr error
        RawConfigPtr GetRawConfigImpl(ara::core::InstanceSpecifier const& instanceSpecifier) noexcept
        {
            RawConfigPtr ret{};
            RawConfigMappingIterator const it{rawConfigMappings_.find(instanceSpecifier)};
            if (it != rawConfigMappings_.end()) {
                ret = it->second;
                ComLogInfo("GetRawConfigImpl find instance!");
            } else {
                ComLogError("GetRawConfigImpl not find instance!");
            }
            return ret;
        }

    private:
        /// @brief false config is not load
        bool configured_{false};

        /// @brief raw config map
        RawConfigMapping rawConfigMappings_;
    };
};
}  // namespace raw
}  // namespace internal
}  // namespace com
}  // namespace ara

#endif