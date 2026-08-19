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
/// @file       crcflags.h
/// @brief      crc flag configuration acquisition class
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

#ifndef ARA_TSYNC_INTERNAL_CONFIG_CRCFLAGS_H_
#define ARA_TSYNC_INTERNAL_CONFIG_CRCFLAGS_H_

#include <ara/core/string_view.h>
#include <isoft/manifestreader/manifest_node.h>

#include <cstdint>
#include <cstring>

#include "ara/tsync/internal/TimeDomainId.h"
#include "ara/tsync/internal/config/common.h"
namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief CrcFlags
class CrcFlags final
{
public:
    /// @brief load function required by manifestReader interface
    /// @param node - manifest node
    /// @return kSucess/ErrorCode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        /// @name kCrcCorrectionField
        static constexpr ara::core::StringView kCrcCorrectionField{"crcCorrectionField"};
        /// @name kCrcDomainNumber
        static constexpr ara::core::StringView kCrcDomainNumber{"crcDomainNumber"};
        /// @name kCrcMessageLength
        static constexpr ara::core::StringView kCrcMessageLength{"crcMessageLength"};
        /// @name kCrcPreciseOriginTimestamp
        static constexpr ara::core::StringView kCrcPreciseOriginTimestamp{"crcPreciseOriginTimestamp"};
        /// @name kCrcSequenceId
        static constexpr ara::core::StringView kCrcSequenceId{"crcSequenceId"};
        /// @name kCrcSourcePortIdentity
        static constexpr ara::core::StringView kCrcSourcePortIdentity{"crcSourcePortIdentity"};
        crcCorrectionField_        = node.GetValue(kCrcCorrectionField, false);
        crcDomainNumber_           = node.GetValue(kCrcDomainNumber, false);
        crcMessageLength_          = node.GetValue(kCrcMessageLength, false);
        crcPreciseOriginTimestamp_ = node.GetValue(kCrcPreciseOriginTimestamp, false);
        crcSequenceId_             = node.GetValue(kCrcSequenceId, false);
        crcSourcePortIdentity_     = node.GetValue(kCrcSourcePortIdentity, false);
        return isoft::kSuccess;
    };

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        std::cout << "crcCorrectionField: " << crcCorrectionField_ << std::endl;
        std::cout << "crcDomainNumber: " << crcDomainNumber_ << std::endl;
        std::cout << "crcMessageLength: " << crcMessageLength_ << std::endl;
        std::cout << "crcPreciseOriginTimestamp: " << crcPreciseOriginTimestamp_ << std::endl;
        std::cout << "crcSequenceId: " << crcSequenceId_ << std::endl;
        std::cout << "crcSourcePortIdentity: " << crcSourcePortIdentity_ << std::endl;
#endif
    };

    /// @brief get whether CRC checks CorrectionField
    /// @traceid {}
    /// @return whether CRC checks CorrectionField
    bool GetCrcCorrectionField() const noexcept { return crcCorrectionField_; }

    /// @brief get whether CRC checks domain ID
    /// @traceid {}
    /// @return whether CRC checks domain ID
    bool GetCrcDomainNumber() const noexcept { return crcDomainNumber_; }

    /// @brief get whether CRC checks message length
    /// @traceid {}
    /// @return whether CRC checks domain ID
    bool GetCrcMessageLength() const noexcept { return crcMessageLength_; }

    /// @brief get whether CRC checks the origin timestamp field
    /// @traceid {}
    /// @return whether CRC checks the origin timestamp field
    bool GetCrcPreciseOriginTimestamp() const noexcept { return crcPreciseOriginTimestamp_; }

    /// @brief get whether CRC checks sequence number
    /// @traceid {}
    /// @return whether CRC checks sequence number
    bool GetCrcSequenceId() const noexcept { return crcSequenceId_; }

    /// @brief get whether CRC checks SourcePortIdentity
    /// @traceid {}
    /// @return whether CRC checks SourcePortIdentity
    bool GetCrcSourcePortIdentity() const noexcept { return crcSourcePortIdentity_; }

private:
    /// @name crcCorrectionField
    bool crcCorrectionField_{false};
    /// @name crcDomainNumber
    bool crcDomainNumber_{false};
    /// @name crcMessageLength
    bool crcMessageLength_{false};
    /// @name crcPreciseOriginTimestamp
    bool crcPreciseOriginTimestamp_{false};
    /// @name crcSequenceId
    bool crcSequenceId_{false};
    /// @name crcSourcePortIdentity
    bool crcSourcePortIdentity_{false};
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_CONFIG_CRCFLAGS_H_
