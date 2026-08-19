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
/// @file       conversation_trait.h
/// @brief      This file provides the definitions of ConversationTrait and related types.
/// @details
/// @date       2022-08-02
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_INTERNAL_CONVERSATION_TRAIT_H_
#define ARA_DIAG_INTERNAL_CONVERSATION_TRAIT_H_

#include <cstdint>

namespace ara {
namespace diag {
namespace internal {
/// @brief Define conversation trait data
struct ConversationTrait
{
    /// @brief Identifier union
    union IdentifierLayout
    {
        /// @name value_
        std::uint64_t value;

        struct
        {
            std::uint16_t testerAddress;
            std::uint8_t reserved;
            std::uint8_t handlerId;
            std::uint32_t channelId;
        }  /// @name field_
        field;
    };
    /// @brief Operation definition
    enum Operation
    {
        kUnknown           = 0,
        kSetIdentifier     = 1,
        kSetActivityStatus = 2,
        kSetSession        = 3,
        kSetSecurityLevel  = 4,
    };
};

}  // namespace internal
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_INTERNAL_CONVERSATION_TRAIT_H_