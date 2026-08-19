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
/// @file       e2e_config.h
/// @brief      header file
/// @details
/// @date       2022-10-14
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_E2E_E2E_CONFIG_H
#define ARA_COM_E2E_E2E_CONFIG_H

#include "ara/com/internal/log/log.h"
#include "ara/core/array.h"
#include "ara/core/map.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "isoft/manifestreader/manifest_reader.h"
#include "profiles_04.h"
#include "profiles_04m.h"
#include "profiles_05.h"
#include "profiles_06.h"
#include "profiles_07.h"
#include "profiles_07m.h"
#include "profiles_08.h"
#include "profiles_11.h"
#include "profiles_22.h"
#include "profiles_44.h"

namespace ara {
namespace com {
namespace e2exf {
static constexpr size_t kDataIdListCount{16U};

/// @brief Convert PROFILE string type name to enum
/// @param[in] profileName Profile configuration name
/// @return Result object -- empty/value or error
ara::core::Result< profile::ProfileName > StringToProfileName(ara::core::StringView const& profileName) noexcept;

/// @brief Convert dataIdMode string type name to enum
/// @param[in] dataIdMode
/// @return Result object -- empty/value or error
ara::core::Result< profile::DataIdMode > StringToDataIdMode(ara::core::StringView const& dataIdMode) noexcept;

/// @brief Event end-to-end protection dataID mapping configuration
struct End2EndEventDataIdMapping
{
    /// @brief Service ID
    uint16_t serviceId{};
    /// @brief Instance ID
    uint16_t instanceId{};
    /// @brief Event ID
    uint16_t eventId{};
    /// @brief dataID
    ara::core::Vector< uint32_t > dataId{};
    /// @brief Configuration reading
    /// @param[in] node Configuration node
    /// @return Configuration read success: 0 others: failure
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
    {
        if (isoft::kSuccess != node.Load("serviceId", serviceId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("instanceId", instanceId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("eventId", eventId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("dataId", dataId)) {
            return isoft::kFailure;
        }
        return isoft::kSuccess;
    }
};

/// @brief Method end-to-end protection dataID mapping configuration
struct End2EndMethodDataIdMapping
{
    /// @brief Service ID
    uint16_t serviceId{};
    /// @brief Instance ID
    uint16_t instanceId{};
    /// @brief Method ID
    uint16_t methodId{};
    /// @brief dataID
    ara::core::Vector< uint32_t > dataId{};
    /// @brief Unique numeric identifier for a specific transport source. In C/S communication, this ID uniquely identifies the client
    uint32_t sourceId{};
    /// @brief Configuration reading
    /// @param[in] node Configuration node
    /// @return Configuration read success: 0 others: failure
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
    {
        if (isoft::kSuccess != node.Load("serviceId", serviceId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("instanceId", instanceId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("methodId", methodId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("dataId", dataId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("sourceId", sourceId)) {
            return isoft::kFailure;
        }
        if (sourceId > 0x0FFFFFFF) {  // NOLINT -- TODO[magic-numbers]
            return -1;
        }
        return isoft::kSuccess;
    }
};

/// @brief Event end-to-end protection configuration
struct End2EndEventProtectionProps
{
    /// @brief dataID
    uint32_t dataId{};
    /// @brief dataIDlist
    ara::core::Array< uint8_t, kDataIdListCount > dataIdList{};
    /// @brief Length of the payload, including the E2E header (in bits).
    uint32_t dataLength{};
    /// @brief Maximum length of the payload including the E2E header (in bits)
    uint32_t maxDataLength{};
    /// @brief Minimum length of the payload including the E2E header (in bits)
    uint32_t minDataLength{};
    /// @brief The period in which the application processes E2E protected messages, reserved unused.
    double dataUpdatePeriod{};
    /// @brief Unique numeric identifier identifying a specific transport source. In C/S communication, this ID uniquely identifies the client
    // uint32_t sourceId;
    /// @brief Inclusion mode of implicit dataID in CRC
    profile::DataIdMode dataIdMode{profile::DataIdMode::kAll_16_Bit};
    /// @brief Maximum allowed difference between two counter values of two consecutively received valid messages
    uint32_t maxDeltaCounter{};
    /// @brief For state E2E_SM_INIT, determines the maximum number of checks where ProfileStatus equals E2E_P_ERROR.
    uint8_t maxErrorStateInit{};
    /// @brief For state E2E_SM_INVALID, determines the maximum number of checks where ProfileStatus equals E2E_P_ERROR
    uint8_t maxErrorStateInvalid{};
    /// @brief For state E2E_SM_VALID, determines the maximum number of checks where ProfileStatus equals E2E_P_ERROR
    uint8_t maxErrorStateValid{};
    /// @brief For state E2E_SM_INIT, determines the maximum number of checks where ProfileStatus equals E2E_P_OK.
    uint8_t minOkStateInit{};
    /// @brief For state E2E_SM_INVALID, determines the maximum number of checks where ProfileStatus equals E2E_P_OK
    uint8_t minOkStateInvalid{};
    /// @brief For state E2E_SM_VALID, determines the maximum number of checks where ProfileStatus equals E2E_P_OK
    uint8_t minOkStateValid{};
    /// @brief PROFILE name
    profile::ProfileName profileName{profile::ProfileName::kProfile_01};
    /// @brief Size of the monitoring window for the E2E state machine Init state.
    uint8_t windowSizeInit{};
    /// @brief Size of the monitoring window for the E2E state machine Invalid state.
    uint8_t windowSizeInvalid{};
    /// @brief Size of the monitoring window for the E2E state machine Valid state.
    uint8_t windowSizeValid{};
    /// @brief Clear monitoring window on transition from state Valid to state Invalid.
    bool clearFromValidToInvalid{false};
    /// @brief E2E state machine behavior regarding transition from NODATA/INIT to INVALID value=0 (false): No direct transition from NODATA to INVALID,
    /// no transition from INIT to INVALID due to counter-related faults (Autosar R19-11 or previous behavior)
    /// value=1 (true): Override direct transition from NODATA to INVALID, transition from INIT to INVALID due to overridden 
    /// counter-related faults (state machine extension)
    bool transitToInvalidExtended{false};

    /// @brief Configuration reading
    /// @param[in] node Configuration node
    /// @return Configuration read success: 0 others: failure
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
    {
        ara::core::Vector< uint8_t > tmpDataIdList;
        ara::core::String tmpDataIdMode;
        ara::core::String tmpProfileName;

        if (isoft::kSuccess != node.Load("dataId", dataId)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("dataIdList", tmpDataIdList)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("dataLength", dataLength)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("maxDataLength", maxDataLength)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("minDataLength", minDataLength)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("dataUpdatePeriod", dataUpdatePeriod)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("dataIdMode", tmpDataIdMode)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("maxDeltaCounter", maxDeltaCounter)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("maxErrorStateInit", maxErrorStateInit)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("maxErrorStateInvalid", maxErrorStateInvalid)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("maxErrorStateValid", maxErrorStateValid)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("minOkStateInit", minOkStateInit)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("minOkStateInvalid", minOkStateInvalid)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("minOkStateValid", minOkStateValid)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("profileName", tmpProfileName)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("windowSizeInit", windowSizeInit)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("windowSizeInvalid", windowSizeInvalid)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("windowSizeValid", windowSizeValid)) {
            return isoft::kFailure;
        }
        if (isoft::kSuccess != node.Load("clearFromValidToInvalid", clearFromValidToInvalid)) {
            return isoft::kFailure;
        }

        auto resultProfileName{StringToProfileName(tmpProfileName)};
        if (resultProfileName) {
            profileName = resultProfileName.Value();
        } else {
            return -1;
        }
        if (profile::ProfileName::kProfile_11 == profileName) {
            auto resultDataIdMode{StringToDataIdMode(tmpDataIdMode)};
            if (resultDataIdMode) {
                dataIdMode = resultDataIdMode.Value();
            } else {
                return -1;
            }
        }
        if (profile::ProfileName::kProfile_22 == profileName) {
            for (size_t i{}; i < tmpDataIdList.size() && i < kDataIdListCount; i++) {
                dataIdList[i] = tmpDataIdList[i];
            }
        }
        return isoft::kSuccess;
    }
};

/// @brief Method end-to-end protection configuration
struct End2EndMethodProtectionProps
{
    /// @brief dataID
    uint32_t dataId{};
    /// @brief dataIDlist
    ara::core::Array< uint8_t, kDataIdListCount > dataIdList{};
    /// @brief Length of the payload, including the E2E header (in bits).
    uint32_t dataLength{};
    /// @brief Maximum length of the payload including the E2E header (in bits)
    uint32_t maxDataLength{};
    /// @brief Minimum length of the payload including the E2E header (in bits)
    uint32_t minDataLength{};
    /// @brief The period in which the application processes E2E protected messages, reserved unused.
    double dataUpdatePeriod{};
    /// @brief Unique numeric identifier identifying a specific transport source. In C/S communication, this ID uniquely identifies the client
    // uint32_t sourceId;
    /// @brief Inclusion mode of implicit dataID in CRC
    profile::DataIdMode dataIdMode{profile::DataIdMode::kAll_16_Bit};
    /// @brief Maximum allowed difference between two counter values of two consecutively received valid messages
    uint32_t maxDeltaCounter{};
    /// @brief For state E2E_SM_INIT, determines the maximum number of checks where ProfileStatus equals E2E_P_ERROR.
    uint8_t maxErrorStateInit{};
    /// @brief For state E2E_SM_INVALID, determines the maximum number of checks where ProfileStatus equals E2E_P_ERROR
    uint8_t maxErrorStateInvalid{};
    /// @brief For state E2E_SM_VALID, determines the maximum number of checks where ProfileStatus equals E2E_P_ERROR
    uint8_t maxErrorStateValid{};
    /// @brief For state E2E_SM_INIT, determines the maximum number of checks where ProfileStatus equals E2E_P_OK.
    uint8_t minOkStateInit{};
    /// @brief For state E2E_SM_INVALID, determines the maximum number of checks where ProfileStatus equals E2E_P_OK
    uint8_t minOkStateInvalid{};
    /// @brief For state E2E_SM_VALID, determines the maximum number of checks where ProfileStatus equals E2E_P_OK
    uint8_t minOkStateValid{};
    /// @brief PROFILE name
    profile::ProfileName profileName{profile::ProfileName::kProfile_01};
    /// @brief Size of the monitoring window for the E2E state machine Init state.
    uint8_t windowSizeInit{};
    /// @brief Size of the monitoring window for the E2E state machine Invalid state.
    uint8_t windowSizeInvalid{};
    /// @brief Size of the monitoring window for the E2E state machine Valid state.
    uint8_t windowSizeValid{};
    /// @brief Clear monitoring window on transition from state Valid to state Invalid.
    bool clearFromValidToInvalid{false};
    /// @brief E2E state machine behavior regarding transition from NODATA/INIT to INVALID value=0 (false): No direct transition from NODATA to INVALID,
    /// no transition from INIT to INVALID due to counter-related faults (Autosar R19-11 or previous behavior)
    /// value=1 (true): Override direct transition from NODATA to INVALID, transition from INIT to INVALID due to overridden 
    /// counter-related faults (state machine extension)
    bool transitToInvalidExtended{false};
    /// @brief Configuration reading
    /// @param[in] node Configuration node
    /// @return Configuration read success: 0 others: failure
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
    {
        ara::core::Vector< uint8_t > tmpDataIdList;
        ara::core::String tmpDataIdMode;
        ara::core::String tmpProfileName;
        if (isoft::kSuccess == node.Load("dataId", dataId) && isoft::kSuccess == node.Load("dataIdList", tmpDataIdList)
            && isoft::kSuccess == node.Load("dataLength", dataLength)
            && isoft::kSuccess == node.Load("maxDataLength", maxDataLength)
            && isoft::kSuccess == node.Load("minDataLength", minDataLength)
            && isoft::kSuccess == node.Load("dataUpdatePeriod", dataUpdatePeriod) &&
            // isoft::kSuccess == node.Load("sourceId", sourceId) &&
            isoft::kSuccess == node.Load("dataIdMode", tmpDataIdMode)
            && isoft::kSuccess == node.Load("maxDeltaCounter", maxDeltaCounter)
            && isoft::kSuccess == node.Load("maxErrorStateInit", maxErrorStateInit)
            && isoft::kSuccess == node.Load("maxErrorStateInvalid", maxErrorStateInvalid)
            && isoft::kSuccess == node.Load("maxErrorStateValid", maxErrorStateValid)
            && isoft::kSuccess == node.Load("minOkStateInit", minOkStateInit)
            && isoft::kSuccess == node.Load("minOkStateInvalid", minOkStateInvalid)
            && isoft::kSuccess == node.Load("minOkStateValid", minOkStateValid)
            && isoft::kSuccess == node.Load("profileName", tmpProfileName)
            && isoft::kSuccess == node.Load("windowSizeInit", windowSizeInit)
            && isoft::kSuccess == node.Load("windowSizeInvalid", windowSizeInvalid)
            && isoft::kSuccess == node.Load("windowSizeValid", windowSizeValid)
            && isoft::kSuccess == node.Load("clearFromValidToInvalid", clearFromValidToInvalid)) {
            auto resultProfileName{StringToProfileName(tmpProfileName)};
            if (resultProfileName) {
                profileName = resultProfileName.Value();
            } else {
                return isoft::kFailure;
            }
            if (profile::ProfileName::kProfile_11 == profileName) {
                auto resultDataIdMode{StringToDataIdMode(tmpDataIdMode)};
                if (resultDataIdMode) {
                    dataIdMode = resultDataIdMode.Value();
                } else {
                    return isoft::kFailure;
                }
            }
            if (profile::ProfileName::kProfile_22 == profileName) {
                for (size_t i{}; i < tmpDataIdList.size() && i < kDataIdListCount; i++) {
                    dataIdList[i] = tmpDataIdList[i];
                }
            }
            return isoft::kSuccess;
        }

        return isoft::kFailure;
    }
};

/// @brief Create event protector
/// @param[in] props Event end-to-end protection configuration
/// @param[in] headerOffset Header offset
/// @return Pointer to the created event protector
std::shared_ptr< profile::ProtectorInterface > CreateProtector(End2EndEventProtectionProps const& props,
                                                               uint16_t const headerOffset) noexcept;

/// @brief Create event checker
/// @param[in] props Event end-to-end protection configuration
/// @param[in] headerOffset Header offset
/// @return Pointer to the created event checker
std::shared_ptr< profile::CheckerInterface > CreateChecker(End2EndEventProtectionProps const& props,
                                                           uint16_t const headerOffset) noexcept;

/// @brief Create method protector
/// @param[in] props Method end-to-end protection configuration
/// @param[in] headerOffset Header offset
/// @return Pointer to the created method protector
std::shared_ptr< profile::ProtectorInterface > CreateProtector(End2EndMethodProtectionProps const& props,
                                                               uint16_t const headerOffset) noexcept;

/// @brief Create method checker
/// @param[in] props Method end-to-end protection configuration
/// @param[in] headerOffset Header offset
/// @return Pointer to the created method checker
std::shared_ptr< profile::CheckerInterface > CreateChecker(End2EndMethodProtectionProps const& props,
                                                           uint16_t const headerOffset) noexcept;
}  // namespace e2exf
}  // namespace com
}  // namespace ara
#endif
