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
/// @file       diagnostic_service_dynamic_access_list.h
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DIAGNOSTIC_SERVICE_DYNAMIC_ACCESS_LIST_H_
#define ARA_DIAG_DIAGNOSTIC_SERVICE_DYNAMIC_ACCESS_LIST_H_

#include <memory>

#include "ara/core/vector.h"
#include "ara/diag/dynamic_access_list_diag_service_builder.h"

namespace isoft {
namespace dm {
class ClientAuthenticationHandleImpl;
}  //  namespace dm
}  // namespace isoft

namespace ara {
namespace diag {
class ClientAuthenticationHandle;
/**
 * @brief Definition of the DiagnosticServiceDynamicAccessList class, which is
 * used by the application to build a DynamicAccessList
 *
 * @artraceid{SWS_DM_01156}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
class DiagnosticServiceDynamicAccessList final
{
public:
    /**
   * @brief Constructor of DiagnosticServiceDynamicAccessList
   *
   * @artraceid{SWS_DM_01157}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    DiagnosticServiceDynamicAccessList();

    /**
   * @brief Destructor of DiagnosticServiceDynamicAccessList
   *
   * @artraceid{SWS_DM_01158}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ~DiagnosticServiceDynamicAccessList() noexcept = default;

    /**
   * @brief Copy constructor of DiagnosticServiceDynamicAccessList
   *
   * @param[in] other Object to copy-construct from
   *
   * @artraceid{SWS_DM_01159}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    DiagnosticServiceDynamicAccessList(DiagnosticServiceDynamicAccessList const &other) noexcept;

    /**
   * @brief Move constructor of DiagnosticServiceDynamicAccessList
   *
   * @param[in] other Object to move-construct from
   *
   * @artraceid{SWS_DM_01160}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    DiagnosticServiceDynamicAccessList(DiagnosticServiceDynamicAccessList &&other) noexcept;

    /**
   * @brief Copy assignment operator of DiagnosticServiceDynamicAccessList
   *
   * @param[in] other Object to copy-assign from.
   *
   * @artraceid{SWS_DM_01161}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    auto operator=(DiagnosticServiceDynamicAccessList const &other) & -> DiagnosticServiceDynamicAccessList &;

    /**
   * @brief Move assignment operator of DiagnosticServiceDynamicAccessList
   *
   * @param[in] other Object to move-assign from.
   *
   * @artraceid{SWS_DM_01162}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    auto operator=(DiagnosticServiceDynamicAccessList &&other)  // NOLINT
        & -> DiagnosticServiceDynamicAccessList &;              // NOLINT

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
    void Reserve(std::size_t numberOfServiceHeads, std::size_t maxServiceHeadSize) noexcept;

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
    auto MakeServiceBuilder(DynamicAccessListDiagServiceBuilder::Byte sid) noexcept
        -> DynamicAccessListDiagServiceBuilder;

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
    auto MakeServiceBuilder(DynamicAccessListDiagServiceBuilder::ByteString serviceHead) noexcept
        -> DynamicAccessListDiagServiceBuilder;

private:
    using DiagnosticServiceDynamicAccess = ara::core::Vector< std::uint8_t >;
    friend isoft::dm::ClientAuthenticationHandleImpl;
    ara::core::Vector< DiagnosticServiceDynamicAccess > list_{};
    std::size_t numberOfServiceHeads_{0};
    std::size_t maxServiceHeadSize_{0};
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_DIAGNOSTIC_SERVICE_DYNAMIC_ACCESS_LIST_H_
