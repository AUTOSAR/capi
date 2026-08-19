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
/// @file       diagnostic_service_dynamic_access_list.cpp
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include <ara/diag/diagnostic_service_dynamic_access_list.h>

namespace ara {
namespace diag {
/**
 * @brief Constructor of DiagnosticServiceDynamicAccessList
 *
 * @artraceid{SWS_DM_01157}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
DiagnosticServiceDynamicAccessList::DiagnosticServiceDynamicAccessList()  // NOLINT
{
}

/**
 * @brief Copy constructor of DiagnosticServiceDynamicAccessList
 *
 * @param[in] other Object to copy-construct from
 *
 * @artraceid{SWS_DM_01159}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
DiagnosticServiceDynamicAccessList::DiagnosticServiceDynamicAccessList(
    DiagnosticServiceDynamicAccessList const &other) noexcept
{
    list_                 = other.list_;
    numberOfServiceHeads_ = other.numberOfServiceHeads_;
    maxServiceHeadSize_   = other.maxServiceHeadSize_;
}

/**
 * @brief Move constructor of DiagnosticServiceDynamicAccessList
 *
 * @param[in] other Object to move-construct from
 *
 * @artraceid{SWS_DM_01160}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
DiagnosticServiceDynamicAccessList::DiagnosticServiceDynamicAccessList(
    DiagnosticServiceDynamicAccessList &&other) noexcept
{
    list_                 = std::move(other.list_);
    numberOfServiceHeads_ = other.numberOfServiceHeads_;
    maxServiceHeadSize_   = other.maxServiceHeadSize_;
}

/**
 * @brief Copy assignment operator of DiagnosticServiceDynamicAccessList
 *
 * @param[in] other Object to copy-assign from.
 *
 * @artraceid{SWS_DM_01161}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
auto DiagnosticServiceDynamicAccessList::operator=(  /// NOLINT
    DiagnosticServiceDynamicAccessList const &other) & -> DiagnosticServiceDynamicAccessList &
{
    list_                 = other.list_;
    numberOfServiceHeads_ = other.numberOfServiceHeads_;
    maxServiceHeadSize_   = other.maxServiceHeadSize_;
    return *this;
}

/**
 * @brief Move assignment operator of DiagnosticServiceDynamicAccessList
 *
 * @param[in] other Object to move-assign from.
 *
 * @artraceid{SWS_DM_01162}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
auto DiagnosticServiceDynamicAccessList::operator=(  /// NOLINT
    DiagnosticServiceDynamicAccessList &&other) & -> DiagnosticServiceDynamicAccessList &
{
    list_                 = std::move(other.list_);
    numberOfServiceHeads_ = other.numberOfServiceHeads_;
    maxServiceHeadSize_   = other.maxServiceHeadSize_;
    return *this;
}

/**
 * @brief Preallocates memory for all service heads to fit into the
 * DynamicAccessList. The preallocation can be just estimated and may
 * calculate just the worst case of memory needed, not exact memory size
 * needed for data numberOfServiceHeads
 *
 * @returns void
 *
 * @param[in] numberOfServiceHeads The number of diagnostic service patterns
 * @param[in] maxServiceHeadSize The expected maximum number of diagnostic
 * service bytes in a single pattern
 *
 * @artraceid{SWS_DM_01163}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
void DiagnosticServiceDynamicAccessList::Reserve(std::size_t numberOfServiceHeads,
                                                 std::size_t maxServiceHeadSize) noexcept
{
    numberOfServiceHeads_ = numberOfServiceHeads;
    maxServiceHeadSize_   = maxServiceHeadSize;
}

/**
 * @brief This function is used by the Application to construct a pattern for
 * the DynamicAccessList using only the SID
 *
 * @returns An instance of a diagnostic service pattern builder
 *
 * @param[in] sid The diagnostic service identifier
 *
 * @artraceid{SWS_DM_01164}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
auto DiagnosticServiceDynamicAccessList::MakeServiceBuilder(DynamicAccessListDiagServiceBuilder::Byte sid) noexcept
    -> DynamicAccessListDiagServiceBuilder
{
    DiagnosticServiceDynamicAccess content{};
    if (maxServiceHeadSize_ > 0U) {
        content.reserve(maxServiceHeadSize_);
    }
    list_.push_back(content);

    return {sid, list_.back()};
}

/**
 * @brief This function is used by the Application to construct a pattern for
 * the DynamicAccessList using a string of bytes
 *
 * @returns An instance of a diagnostic service pattern builder
 *
 * @param[in] serviceHead A string of bytes to start the DynamicAccess
 * pattern-match
 *
 * @artraceid{SWS_DM_01165}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
auto DiagnosticServiceDynamicAccessList::MakeServiceBuilder(
    DynamicAccessListDiagServiceBuilder::ByteString serviceHead) noexcept -> DynamicAccessListDiagServiceBuilder
{
    DiagnosticServiceDynamicAccess content{};
    if (maxServiceHeadSize_ > 0U) {
        content.reserve(maxServiceHeadSize_);
    }
    list_.push_back(content);
    DynamicAccessListDiagServiceBuilder builder{serviceHead[0], list_.back()};
    for (size_t i = 1; i < serviceHead.size(); i++) {
        builder.Add(serviceHead[i]);
    }

    return builder;
}
}  // namespace diag
}  // namespace ara
