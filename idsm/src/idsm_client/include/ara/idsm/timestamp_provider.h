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
/// @file       timestamp_provider.h
/// @brief      Base class for AA application timestamp provider
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
/// @trace_id_sr=SRS_IDSM_0005
/// @unit_description=Base class for AA application timestamp provider
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_TIMESTAMP_PROVIDER_H_
#define ARA_IDSM_TIMESTAMP_PROVIDER_H_

#include <functional>

#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"
#include "ara/core/span.h"
#include "ara/idsm/common.h"

namespace ara {
namespace idsm {

/// @brief  [SWS_AIDSM_20101] Register a callback for providing timestamps to the IdsM .
/// @param callback Timestamp callback function
/// @exception Stack overflow exception
/// @code{.isoft}
/// @unit_name=RegisterTimestampProvider
/// @interface_level=unit
/// @needwork = ad
/// @endcode
void RegisterTimestampProvider(std::function< TimestampType() > const &callback);

/// @brief {SWS_AIDSM_10101} Security event reporting class.
/// @code{.isoft}
/// export_level=/IntrusionDetectionSystem
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class TimestampProvider
{
private:
public:
    /// @brief [SWS_AIDSM_10402] Destructor
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    virtual ~TimestampProvider() noexcept = default;
    /// @brief [SWS_AIDSM_10404] Copy constructor
    /// @param object to be copied
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    TimestampProvider(TimestampProvider const &) = delete;
    /// @brief [SWS_AIDSM_10403] Move constructor
    /// @param object to be moved
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    TimestampProvider(TimestampProvider &&) noexcept;
    /// @brief [SWS_AIDSM_10406] Copy assignment operator
    /// @param object to be copied for assignment operator
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    TimestampProvider &operator=(TimestampProvider const &) = delete;
    /// @brief [SWS_AIDSM_10405] Move assignment operator
    /// @param object to be moved for assignment operator
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    TimestampProvider &operator=(TimestampProvider &&) noexcept;
    /// @brief  [SWS_AIDSM_10401] Parameterized constructor
    /// @param instance Port instance identifier
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @endcode
    /// @needwork = dda
    explicit TimestampProvider(const ara::core::InstanceSpecifier &instance);

    /// @brief [SWS_AIDSM_10407] Get timestamp
    /// @return Timestamp
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// export_level=/IntrusionDetectionSystem
    /// @endcode
    /// @needwork = ad
    virtual TimestampType GetTimestamp() = 0;

    /// @brief [SWS_AIDSM_10408] AA application starts timestamp providing service
    /// @return Service start status
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// export_level=/IntrusionDetectionSystem
    /// @endcode
    /// @needwork = ad
    ara::core::Result< void > Offer();

    /// @brief [SWS_AIDSM_10409] AA application stops timestamp providing service
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

#endif  // ARA_IDSM_TIMESTAMP_PROVIDER_H_