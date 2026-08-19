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
/// @file       idsm_fg_state.h
/// @brief      Function group state access class
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/function group state pool
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0007
/// @unit_name=FunGrpState
/// @unit_description=Function group state access class
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_FG_STATE_H_
#define ARA_IDSM_FG_STATE_H_
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>

#include "ara/core/string.h"

namespace ara {
namespace idsm {
/// @brief Function group state pool
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00133
/// @trace_id_dd=DD_IDSM_00288
/// @needwork = ad
/// @endcode
class FunGrpState
{
public:
    /// @brief FunGrpState
    /// @return FunGrpState instance
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00134
    /// @trace_id_dd=DD_IDSM_00289
    /// @needwork = ad
    /// @endcode
    static std::shared_ptr< FunGrpState > GetInstance();
    /// @brief Set function group state
    /// @param fgName Function group name
    /// @param fgState Function group state
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00135
    /// @trace_id_dd=DD_IDSM_00290
    /// @needwork = ad
    /// @endcode
    void Set(ara::core::String const& fgName, ara::core::String const& fgState);
    /// @brief Get function group state
    /// @param fgName Function group name
    /// @return Function group state
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00136
    /// @trace_id_dd=DD_IDSM_00291
    /// @needwork = ad
    /// @endcode
    ara::core::String Get(ara::core::String const& fgName);

public:
    /// @brief Destructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00292
    /// @needwork = dda
    /// @endcode
    virtual ~FunGrpState() = default;

public:
    /// @brief Copy constructor
    /// @param pool Object to be copied
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00293
    /// @needwork = dda
    /// @endcode
    FunGrpState(FunGrpState const& pool) = delete;
    /// @brief Move constructor
    /// @param pool Object to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00294
    /// @needwork = dda
    /// @endcode
    FunGrpState(FunGrpState&& pool) = delete;
    /// @brief Copy assignment operator
    /// @param pool Object to be copied for assignment operator
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00295
    /// @needwork = dda
    /// @endcode
    FunGrpState& operator=(FunGrpState const& pool) = delete;
    /// @brief Move assignment operator
    /// @param pool Object to be moved for assignment operator
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00296
    /// @needwork = dda
    /// @endcode
    FunGrpState& operator=(FunGrpState&& pool) = delete;

private:
    /// @brief Constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00297
    /// @needwork = dda
    /// @endcode
    FunGrpState() = default;

private:
    /// @brief Unique instance of the function group state pool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00298
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< FunGrpState > s_SingleInstance_;
    /// @brief Singleton thread-safe mutex lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00299
    /// @needwork = dda
    /// @endcode
    static std::mutex s_SigleMutex_;
    /// @brief Buffer for temporarily storing events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00300
    /// @needwork = dda
    /// @endcode
    std::map< ara::core::String, ara::core::String > fgMap_;
};

}  // namespace idsm
}  // namespace ara

#endif