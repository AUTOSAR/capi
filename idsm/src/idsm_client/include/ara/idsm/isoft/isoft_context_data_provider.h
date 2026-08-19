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
/// @file       isoft_context_data_provider.h
/// @brief      Context data change class
/// @details
/// @date       2022-12-27
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/lib client
/// @interface_level=module
/// @unit_name=EventReporter
/// @unit_description=Context data change class
/// @trace_id_sr=SR_IDSM_
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_PUHUA_CONTEXT_DATA_PROVIDER_H_
#define ARA_IDSM_PUHUA_CONTEXT_DATA_PROVIDER_H_

#include "ara/idsm/context_data_provider.h"

namespace ara {
namespace idsm {
namespace isoft_def {
/// @brief {SWS_AIDSM_10101} Security event reporting class.
/// @code{.isoft}
/// export_level=/IntrusionDetectionSystem
/// @endcode
/// @needwork = ad
class PuhuaContextDataProvider : public ContextDataProvider
{
public:
    /// @brief [SWS_AIDSM_10502] Destructor
    /// @code{.isoft}
    /// @endcode
    /// @needwork = ad
    ~PuhuaContextDataProvider() noexcept override = default;
    /// @brief [SWS_AIDSM_10504] Copy constructor
    /// @param object to be copied
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @endcode
    /// @needwork = ad
    PuhuaContextDataProvider(PuhuaContextDataProvider const &) = delete;
    /// @brief [SWS_AIDSM_10503] Move constructor
    /// @param object to be moved
    /// @code{.isoft}
    /// @endcode
    /// @needwork = ad
    PuhuaContextDataProvider(PuhuaContextDataProvider &&) noexcept;
    /// @brief [SWS_AIDSM_10506] Copy assignment operator
    /// @param object to be copied for assignment operator
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @endcode
    /// @needwork = ad
    PuhuaContextDataProvider &operator=(PuhuaContextDataProvider const &) = delete;
    /// @brief [SWS_AIDSM_10505] Move assignment operator
    /// @param object to be moved for assignment operator
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @endcode
    /// @needwork = ad
    PuhuaContextDataProvider &operator=(PuhuaContextDataProvider &&) noexcept;
    /// @brief  [SWS_AIDSM_10501]  Parameterized constructor
    /// @param instance Port instance identifier
    /// @param additionalBytes Additional byte count
    /// @param eventType Raw data offset
    /// @code{.isoft}
    /// @endcode
    /// @needwork = ad
    explicit PuhuaContextDataProvider(const ara::core::InstanceSpecifier &instance,
                                      std::size_t additionalBytes,
                                      std::size_t originalContextDataOffset) noexcept;

    /// @brief  [SWS_AIDSM_10509] Get security event context
    /// @param contextData Context data
    /// @param event Security event
    /// @return Length of context data
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// export_level=/IntrusionDetectionSystem
    /// @endcode
    /// @needwork = ad
    ara::core::Result< std::size_t > ModifyContextData(ara::core::Span< std::uint8_t > contextData,
                                                       EventIdType event) override;
};

}  // namespace isoft_def

}  // namespace idsm

}  // namespace ara
#endif