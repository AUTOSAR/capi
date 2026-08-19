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
/// @file       context_data_provider.h
/// @brief      Abstract class for context data update
/// @details
/// @date       2025-06-12
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/lib client
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=ContextDataProvider
/// @unit_description=Abstract class for context data update
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_CONTEXT_DATA_PROVIDER_H_
#define ARA_IDSM_CONTEXT_DATA_PROVIDER_H_

#include <ara/core/result.h>

#include "ara/core/instance_specifier.h"
#include "ara/core/span.h"
#include "ara/idsm/common.h"

namespace ara {
namespace idsm {
/// @brief {SWS_AIDSM_10101} Security event reporting class.
/// @code{.isoft}
/// export_level=/IntrusionDetectionSystem
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class ContextDataProvider
{
public:
    /// @brief [SWS_AIDSM_10502] Destructor
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    virtual ~ContextDataProvider() noexcept = default;
    /// @brief [SWS_AIDSM_10504] Copy constructor
    /// @param object to be copied
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    ContextDataProvider(ContextDataProvider const &) = delete;
    /// @brief [SWS_AIDSM_10503] Move constructor
    /// @param object to be moved
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    ContextDataProvider(ContextDataProvider &&) noexcept;
    /// @brief [SWS_AIDSM_10506] Copy assignment operator
    /// @param object to be copied for assignment operator
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    ContextDataProvider &operator=(ContextDataProvider const &) = delete;
    /// @brief [SWS_AIDSM_10505] Move assignment operator
    /// @param object to be moved for assignment operator
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    ContextDataProvider &operator=(ContextDataProvider &&) noexcept;
    /// @brief  [SWS_AIDSM_10501]  Parameterized constructor
    /// @param instance Port instance identifier
    /// @param additionalBytes Additional byte count
    /// @param eventType Raw data offset
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    explicit ContextDataProvider(const ara::core::InstanceSpecifier &instance,
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
    virtual ara::core::Result< std::size_t > ModifyContextData(ara::core::Span< std::uint8_t > contextData,
                                                               EventIdType event)
        = 0;

    /// @brief  [SWS_AIDSM_10507] AA application starts context update service
    /// @return Service start status
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// export_level=/IntrusionDetectionSystem
    /// @endcode
    /// @needwork = ad
    ara::core::Result< void > Offer();

    /// @brief [SWS_AIDSM_10508]  AA application stops context update service
    /// @return Void return
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// export_level=/IntrusionDetectionSystem
    /// @endcode
    /// @needwork = ad
    void StopOffer();

private:
    /// @name instanceId_
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    ara::core::InstanceSpecifier instanceId_;
};

}  // namespace idsm

}  // namespace ara
#endif