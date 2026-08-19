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
/// @file       e2e_config.cpp
/// @brief
/// @details
/// @date       2022-10-14
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/e2e/e2e_config.h"

#include "ara/com/e2e/e2e_error_domain.h"

namespace ara {
namespace com {
namespace e2exf {
/// @brief
/// @param[in] profileName
/// @return Result object -- empty/value or error
ara::core::Result< profile::ProfileName > StringToProfileName(ara::core::StringView const& profileName) noexcept
{
    using Result = ara::core::Result< profile::ProfileName >;
    if (profileName.compare("PROFILE_01") == 0) {
        return Result::FromValue(profile::ProfileName::kProfile_01);
    }
    if (profileName.compare("PROFILE_02") == 0) {
        return Result::FromValue(profile::ProfileName::kProfile_02);
    }
    if (profileName.compare("PROFILE_04") == 0) {
        return Result::FromValue(profile::ProfileName::kProfile_04);
    }
    if (profileName.compare("PROFILE_05") == 0) {
        return Result::FromValue(profile::ProfileName::kProfile_05);
    }
    if (profileName.compare("PROFILE_06") == 0) {
        return Result::FromValue(profile::ProfileName::kProfile_06);
    }
    if (profileName.compare("PROFILE_07") == 0) {
        return Result::FromValue(profile::ProfileName::kProfile_07);
    }
    if (profileName.compare("PROFILE_08") == 0) {
        return Result::FromValue(profile::ProfileName::kProfile_08);
    }
    if (profileName.compare("PROFILE_11") == 0) {
        return Result::FromValue(profile::ProfileName::kProfile_11);
    }
    if (profileName.compare("PROFILE_22") == 0) {
        return Result::FromValue(profile::ProfileName::kProfile_22);
    }
    if (profileName.compare("PROFILE_44") == 0) {
        return Result::FromValue(profile::ProfileName::kProfile_44);
    }
    if (profileName.compare("PROFILE_04m") == 0) {
        return Result::FromValue(profile::ProfileName::kProfile_04m);
    }
    if (profileName.compare("PROFILE_07m") == 0) {
        return Result::FromValue(profile::ProfileName::kProfile_07m);
    }
    return Result::FromError(ara::com::e2e::E2EErrc::kUnknownError);
}

/// @brief
/// @param[in] dataIdMode
/// @return Result object -- empty/value or error
ara::core::Result< profile::DataIdMode > StringToDataIdMode(ara::core::StringView const& dataIdMode) noexcept
{
    profile::DataIdMode mode;
    if (dataIdMode.compare("ALL-16-BIT") == 0) {
        mode = profile::DataIdMode::kAll_16_Bit;
    } else if (dataIdMode.compare("ALTERNATING-8-BIT") == 0) {
        mode = profile::DataIdMode::kAlternating_8_Bit;
    } else if (dataIdMode.compare("LOWER-12-BIT") == 0) {
        mode = profile::DataIdMode::kLower_12_Bit;
    } else if (dataIdMode.compare("LOWER-8-BIT") == 0) {
        mode = profile::DataIdMode::kLower_8_Bit;
    } else {
        return ara::core::Result< profile::DataIdMode >::FromError(ara::com::e2e::E2EErrc::kUnknownError);
    }
    return ara::core::Result< profile::DataIdMode >::FromValue(mode);
}

/// @brief
/// @param[in] props
/// @param[in] headerOffset
/// @return
std::shared_ptr< profile::ProtectorInterface > CreateProtector(End2EndEventProtectionProps const& props,
                                                               uint16_t headerOffset) noexcept
{
    std::shared_ptr< profile::ProtectorInterface > ret;
    switch (props.profileName) {
        case profile::ProfileName::kProfile_04: {
            using ara::com::profile_04::Config;
            using ara::com::profile_04::CounterType;
            using ara::com::profile_04::IdType;
            using ara::com::profile_04::LengthType;
            using ara::com::profile_04::Protector;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_05: {
            using ara::com::profile_05::Config;
            using ara::com::profile_05::CounterType;
            using ara::com::profile_05::IdType;
            using ara::com::profile_05::LengthType;
            using ara::com::profile_05::Protector;
            Config config{static_cast< CounterType >(headerOffset), static_cast< LengthType >(props.dataLength),
                          static_cast< IdType >(props.dataId), static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_06: {
            using ara::com::profile_06::Config;
            using ara::com::profile_06::CounterType;
            using ara::com::profile_06::IdType;
            using ara::com::profile_06::LengthType;
            using ara::com::profile_06::Protector;
            Config config{static_cast< CounterType >(headerOffset), static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength), static_cast< IdType >(props.dataId),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_07: {
            using ara::com::profile_07::Config;
            using ara::com::profile_07::CounterType;
            using ara::com::profile_07::IdType;
            using ara::com::profile_07::LengthType;
            using ara::com::profile_07::Protector;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_08: {
            using ara::com::profile_08::Config;
            using ara::com::profile_08::CounterType;
            using ara::com::profile_08::IdType;
            using ara::com::profile_08::LengthType;
            using ara::com::profile_08::Protector;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_11: {
            using ara::com::profile_11::Config;
            using ara::com::profile_11::CounterType;
            using ara::com::profile_11::IdType;
            using ara::com::profile_11::LengthType;
            using ara::com::profile_11::Protector;
            Config config{
                static_cast< LengthType >(props.dataLength),       static_cast< IdType >(props.dataId),
                static_cast< CounterType >(props.maxDeltaCounter), static_cast< E2E_P11DataIDMode >(props.dataIdMode),
                static_cast< LengthType >(headerOffset + 0),
                static_cast< LengthType >(headerOffset + 8),    // NOLINT -- TODO[magic-numbers]
                static_cast< LengthType >(headerOffset + 12)};  // NOLINT -- TODO[magic-numbers]
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_22: {
            using ara::com::profile_22::Config;
            using ara::com::profile_22::CounterType;
            using ara::com::profile_22::IdType;
            using ara::com::profile_22::LengthType;
            using ara::com::profile_22::Protector;
            Config config{static_cast< LengthType >(props.dataLength), const_cast< uint8_t* >(props.dataIdList.data()),
                          static_cast< CounterType >(props.maxDeltaCounter), static_cast< LengthType >(headerOffset)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_44: {
            using ara::com::profile_44::Config;
            using ara::com::profile_44::CounterType;
            using ara::com::profile_44::IdType;
            using ara::com::profile_44::LengthType;
            using ara::com::profile_44::Protector;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        default: {
            ret = nullptr;
        } break;
    }
    return ret;
}

/// @brief
/// @param[in] props
/// @param[in] headerOffset
/// @return
std::shared_ptr< profile::CheckerInterface > CreateChecker(End2EndEventProtectionProps const& props,
                                                           uint16_t headerOffset) noexcept
{
    std::shared_ptr< profile::CheckerInterface > ret;
    switch (props.profileName) {
        case profile::ProfileName::kProfile_04: {
            using ara::com::profile_04::Checker;
            using ara::com::profile_04::Config;
            using ara::com::profile_04::CounterType;
            using ara::com::profile_04::IdType;
            using ara::com::profile_04::LengthType;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_05: {
            using ara::com::profile_05::Checker;
            using ara::com::profile_05::Config;
            using ara::com::profile_05::CounterType;
            using ara::com::profile_05::IdType;
            using ara::com::profile_05::LengthType;
            Config config{static_cast< CounterType >(headerOffset), static_cast< LengthType >(props.dataLength),
                          static_cast< IdType >(props.dataId), static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_06: {
            using ara::com::profile_06::Checker;
            using ara::com::profile_06::Config;
            using ara::com::profile_06::CounterType;
            using ara::com::profile_06::IdType;
            using ara::com::profile_06::LengthType;
            Config config{static_cast< CounterType >(headerOffset), static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength), static_cast< IdType >(props.dataId),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_07: {
            using ara::com::profile_07::Checker;
            using ara::com::profile_07::Config;
            using ara::com::profile_07::CounterType;
            using ara::com::profile_07::IdType;
            using ara::com::profile_07::LengthType;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_08: {
            using ara::com::profile_08::Checker;
            using ara::com::profile_08::Config;
            using ara::com::profile_08::CounterType;
            using ara::com::profile_08::IdType;
            using ara::com::profile_08::LengthType;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_11: {
            using ara::com::profile_11::Checker;
            using ara::com::profile_11::Config;
            using ara::com::profile_11::CounterType;
            using ara::com::profile_11::IdType;
            using ara::com::profile_11::LengthType;
            Config config{
                static_cast< LengthType >(props.dataLength),       static_cast< IdType >(props.dataId),
                static_cast< CounterType >(props.maxDeltaCounter), static_cast< E2E_P11DataIDMode >(props.dataIdMode),
                static_cast< LengthType >(headerOffset + 0),
                static_cast< LengthType >(headerOffset + 8),    // NOLINT -- TODO[magic-numbers]
                static_cast< LengthType >(headerOffset + 12)};  // NOLINT -- TODO[magic-numbers]
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_22: {
            using ara::com::profile_22::Checker;
            using ara::com::profile_22::Config;
            using ara::com::profile_22::CounterType;
            using ara::com::profile_22::IdType;
            using ara::com::profile_22::LengthType;
            Config config{static_cast< LengthType >(props.dataLength), const_cast< uint8_t* >(props.dataIdList.data()),
                          static_cast< CounterType >(props.maxDeltaCounter), static_cast< LengthType >(headerOffset)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_44: {
            using ara::com::profile_44::Checker;
            using ara::com::profile_44::Config;
            using ara::com::profile_44::CounterType;
            using ara::com::profile_44::IdType;
            using ara::com::profile_44::LengthType;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        default: {
            ret = nullptr;
        } break;
    }
    return ret;
}

/// @brief
/// @param[in] props
/// @param[in] headerOffset
/// @return
std::shared_ptr< profile::ProtectorInterface > CreateProtector(End2EndMethodProtectionProps const& props,
                                                               uint16_t headerOffset) noexcept
{
    std::shared_ptr< profile::ProtectorInterface > ret;
    switch (props.profileName) {
        case profile::ProfileName::kProfile_04: {
            using ara::com::profile_04::Config;
            using ara::com::profile_04::CounterType;
            using ara::com::profile_04::IdType;
            using ara::com::profile_04::LengthType;
            using ara::com::profile_04::Protector;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_05: {
            using ara::com::profile_05::Config;
            using ara::com::profile_05::CounterType;
            using ara::com::profile_05::IdType;
            using ara::com::profile_05::LengthType;
            using ara::com::profile_05::Protector;
            Config config{static_cast< CounterType >(headerOffset), static_cast< LengthType >(props.dataLength),
                          static_cast< IdType >(props.dataId), static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_06: {
            using ara::com::profile_06::Config;
            using ara::com::profile_06::CounterType;
            using ara::com::profile_06::IdType;
            using ara::com::profile_06::LengthType;
            using ara::com::profile_06::Protector;
            Config config{static_cast< CounterType >(headerOffset), static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength), static_cast< IdType >(props.dataId),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_07: {
            using ara::com::profile_07::Config;
            using ara::com::profile_07::CounterType;
            using ara::com::profile_07::IdType;
            using ara::com::profile_07::LengthType;
            using ara::com::profile_07::Protector;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_11: {
            using ara::com::profile_11::Config;
            using ara::com::profile_11::CounterType;
            using ara::com::profile_11::IdType;
            using ara::com::profile_11::LengthType;
            using ara::com::profile_11::Protector;
            Config config{
                static_cast< LengthType >(props.dataLength),       static_cast< IdType >(props.dataId),
                static_cast< CounterType >(props.maxDeltaCounter), static_cast< E2E_P11DataIDMode >(props.dataIdMode),
                static_cast< LengthType >(headerOffset + 0),
                static_cast< LengthType >(headerOffset + 8),    // NOLINT -- TODO[magic-numbers]
                static_cast< LengthType >(headerOffset + 12)};  // NOLINT -- TODO[magic-numbers]
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_22: {
            using ara::com::profile_22::Config;
            using ara::com::profile_22::CounterType;
            using ara::com::profile_22::IdType;
            using ara::com::profile_22::LengthType;
            using ara::com::profile_22::Protector;
            Config config{static_cast< LengthType >(props.dataLength), nullptr,
                          static_cast< CounterType >(props.maxDeltaCounter), static_cast< LengthType >(headerOffset)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_04m: {
            using ara::com::profile_04m::Config;
            using ara::com::profile_04m::CounterType;
            using ara::com::profile_04m::IdType;
            using ara::com::profile_04m::LengthType;
            using ara::com::profile_04m::Protector;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_07m: {
            using ara::com::profile_07m::Config;
            using ara::com::profile_07m::CounterType;
            using ara::com::profile_07m::IdType;
            using ara::com::profile_07m::LengthType;
            using ara::com::profile_07m::Protector;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Protector >(std::move(config));
        } break;
        default: {
            ret = nullptr;
        } break;
    }
    return ret;
}

/// @brief
/// @param[in] props
/// @param[in] headerOffset
/// @return
std::shared_ptr< profile::CheckerInterface > CreateChecker(End2EndMethodProtectionProps const& props,
                                                           uint16_t headerOffset) noexcept
{
    std::shared_ptr< profile::CheckerInterface > ret;
    switch (props.profileName) {
        case profile::ProfileName::kProfile_04: {
            using ara::com::profile_04::Checker;
            using ara::com::profile_04::Config;
            using ara::com::profile_04::CounterType;
            using ara::com::profile_04::IdType;
            using ara::com::profile_04::LengthType;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_05: {
            using ara::com::profile_05::Checker;
            using ara::com::profile_05::Config;
            using ara::com::profile_05::CounterType;
            using ara::com::profile_05::IdType;
            using ara::com::profile_05::LengthType;
            Config config{static_cast< CounterType >(headerOffset), static_cast< LengthType >(props.dataLength),
                          static_cast< IdType >(props.dataId), static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_06: {
            using ara::com::profile_06::Checker;
            using ara::com::profile_06::Config;
            using ara::com::profile_06::CounterType;
            using ara::com::profile_06::IdType;
            using ara::com::profile_06::LengthType;
            Config config{static_cast< CounterType >(headerOffset), static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength), static_cast< IdType >(props.dataId),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_07: {
            using ara::com::profile_07::Checker;
            using ara::com::profile_07::Config;
            using ara::com::profile_07::CounterType;
            using ara::com::profile_07::IdType;
            using ara::com::profile_07::LengthType;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_11: {
            using ara::com::profile_11::Checker;
            using ara::com::profile_11::Config;
            using ara::com::profile_11::CounterType;
            using ara::com::profile_11::IdType;
            using ara::com::profile_11::LengthType;
            Config config{
                static_cast< LengthType >(props.dataLength),       static_cast< IdType >(props.dataId),
                static_cast< CounterType >(props.maxDeltaCounter), static_cast< E2E_P11DataIDMode >(props.dataIdMode),
                static_cast< LengthType >(headerOffset + 0),
                static_cast< LengthType >(headerOffset + 8),    // NOLINT -- TODO[magic-numbers]
                static_cast< LengthType >(headerOffset + 12)};  // NOLINT -- TODO[magic-numbers]
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_22: {
            using ara::com::profile_22::Checker;
            using ara::com::profile_22::Config;
            using ara::com::profile_22::CounterType;
            using ara::com::profile_22::IdType;
            using ara::com::profile_22::LengthType;
            Config config{static_cast< LengthType >(props.dataLength), nullptr,
                          static_cast< CounterType >(props.maxDeltaCounter), static_cast< LengthType >(headerOffset)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_04m: {
            using ara::com::profile_04m::Checker;
            using ara::com::profile_04m::Config;
            using ara::com::profile_04m::CounterType;
            using ara::com::profile_04m::IdType;
            using ara::com::profile_04m::LengthType;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        case profile::ProfileName::kProfile_07m: {
            using ara::com::profile_07m::Checker;
            using ara::com::profile_07m::Config;
            using ara::com::profile_07m::CounterType;
            using ara::com::profile_07m::IdType;
            using ara::com::profile_07m::LengthType;
            Config config{static_cast< IdType >(props.dataId), static_cast< LengthType >(headerOffset),
                          static_cast< LengthType >(props.minDataLength),
                          static_cast< LengthType >(props.maxDataLength),
                          static_cast< CounterType >(props.maxDeltaCounter)};
            ret = std::make_shared< Checker >(std::move(config));
        } break;
        default: {
            ret = nullptr;
        } break;
    }
    return ret;
}
}  // namespace e2exf
}  // namespace com
}  // namespace ara