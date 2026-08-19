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
/// @file       raw_data_element.h
/// @brief      This file provides the definitions of RawDataElement and related types.
/// @details
/// @date       2022-08-15
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_INTERNAL_RAW_DATA_ELEMENT_H_
#define ARA_DIAG_INTERNAL_RAW_DATA_ELEMENT_H_

#include <ara/core/future.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/vector.h>
#include <ara/diag/cancellation_handler.h>
#include <ara/diag/meta_info.h>
#include <ara/diag/reentrancy.h>

#include <cstdint>
#include <memory>

#include "types.h"

namespace isoft {
namespace dm {
namespace dis {
/// @brief RawDataElementAgent
class RawDataElementAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft
namespace ara {
namespace diag {

namespace internal {
/// @brief RawDataElement
class RawDataElement
{
public:
    /// @brief Copy constructor is prohibited
    /// @param
    RawDataElement(RawDataElement const&) = delete;
    /// @brief Copy assignment constructor is prohibited
    /// @param
    /// @return
    RawDataElement& operator=(RawDataElement const&) = delete;
    /// @brief Move constructor is prohibited
    /// @param
    RawDataElement(RawDataElement&&) = default;
    /// @brief Move assignment operator is prohibited
    /// @param
    /// @return
    RawDataElement& operator=(RawDataElement&&) = default;

    /// @brief RawDataElement constructor
    /// @param[in] specifier Instance identifier
    /// @param[in] reentrancyType Reentrancy type
    /// @throws on overflow
    RawDataElement(ara::core::InstanceSpecifier specifier, ReentrancyType reentrancyType);

    /// @brief RawDataElement destructor
    virtual ~RawDataElement() = default;

    /// @brief Offer service
    /// @return Result
    /// @throws on overflow
    ara::core::Result< void > Offer();

    /// @brief Stop offering service
    /// @throws on overflow
    void StopOffer();

    /// @brief Read data
    /// @param[in] metaInfo Meta information
    /// @param[in] cancellationHandler Read service cancellation handler
    /// @return Read result
    virtual ara::core::Future< Chunk > RawRead(MetaInfo& metaInfo, CancellationHandler cancellationHandler) = 0;

private:
    /// @name specifier_
    ara::core::InstanceSpecifier specifier_;
    /// @name reentrancyType_
    ReentrancyType reentrancyType_;
    std::shared_ptr< isoft::dm::dis::RawDataElementAgent >
        /// @name skeleton_
        skeleton_{};
};

}  // namespace internal
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_INTERNAL_RAW_DATA_ELEMENT_H_