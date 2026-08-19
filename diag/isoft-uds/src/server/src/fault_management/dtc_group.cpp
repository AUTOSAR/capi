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
/// @file       dtc_group.cpp
/// @brief      This file implements dtc_group functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=a595503d-c2f2-4934-996f-e408435e9fb5
/// @unit_name=DtcGroup
/// @unit_description=This file provides dtc_group functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/dtc_group.h"

#include "fault_management/dtc.h"
#include "fault_management/public.h"

namespace isoft {
namespace uds {
namespace server {

DtcGroup::DtcGroup(uint32_t const groupNumber, std::map< uint32_t, std::shared_ptr< Dtc > >& dtcs)
{
    dtcGroupNumber_ = groupNumber;
    for (std::map< uint32_t, std::shared_ptr< Dtc > >::const_iterator it{std::move(dtcs.begin())}; it != dtcs.end();
         ++it) {
        dtcs_[it->first] = it->second;
    }
}


DtcGroup::DtcGroup(uint32_t const groupNumber,
                   std::set< uint32_t >& dtcGroupList,
                   std::map< uint32_t, std::shared_ptr< Dtc > >& dtcs)
{
    dtcGroupNumber_ = groupNumber;
    for (auto const& id : dtcGroupList) {
        std::map< uint32_t, std::shared_ptr< Dtc > >::const_iterator const itrMap{std::move(dtcs.find(id))};
        if (itrMap == dtcs.cend()) {
            LogWarn() << "DtcGroup::DtcGroup -> no find dtc id =" << id;
        }
        dtcs_[id] = itrMap->second;
    }
}


void DtcGroup::Clear(bool const isFromUds) const
{
    LogVerbose() << "DtcGroup::Clear enter. id=" << dtcGroupNumber_;
    for (std::map< std::uint32_t, std::shared_ptr< Dtc > >::const_iterator it{dtcs_.cbegin()}; it != dtcs_.cend();
         ++it) {
        it->second->Clear(isFromUds);
    }
}


bool DtcGroup::IsForbiddenClear() const noexcept
{
    bool isForbidden{true};
    for (std::map< std::uint32_t, std::shared_ptr< Dtc > >::const_iterator it{dtcs_.cbegin()}; it != dtcs_.cend();
         ++it) {
        if (!it->second->IsForbiddenClear()) {
            isForbidden = false;
            break;
        }
    }

    return isForbidden;
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
