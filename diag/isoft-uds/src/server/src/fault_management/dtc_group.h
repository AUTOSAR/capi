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
/// @file       dtc_group.h
/// @brief      This file provides dtc_group functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/fault_management
/// @interface_level=software
/// @trace_id_sr=a595503d-c2f2-4934-996f-e408435e9fb5
/// @unit_name=DtcGroup
/// @unit_description=This file provides dtc_group functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_DTC_GROUP_H__
#define __ISOFT_UDS_FAULT_DTC_GROUP_H__

#include <map>
#include <memory>

#include "isoft/uds/fault_management/fault_management.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Dtc class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02463
/// @needwork = dd
/// @endcode
class Dtc;

/// @brief DtcGroup class
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
class DtcGroup final
{
public:
    /// @brief Default constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01538
    /// @needwork = dd
    /// @endcode
    DtcGroup() = default;

    /// @brief Copy constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01539
    /// @needwork = dd
    /// @endcode
    DtcGroup(DtcGroup const& other) = default;

    /// @brief Move constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01540
    /// @needwork = dd
    /// @endcode
    DtcGroup(DtcGroup&& other) = default;

    /// @brief Copy assignment operator
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01541
    /// @needwork = dd
    /// @endcode
    DtcGroup& operator=(DtcGroup const& other) = default;

    /// @brief Move assignment operator
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01542
    /// @needwork = dd
    /// @endcode
    DtcGroup& operator=(DtcGroup&& other) = default;

    /// @brief Constructor
    /// @param[in] groupNumber dtc group number
    /// @param[out] dtcs All dtcs in this group
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01543
    /// @needwork = dd
    /// @endcode
    DtcGroup(uint32_t const groupNumber, std::map< uint32_t, std::shared_ptr< Dtc > >& dtcs);

    /// @brief Constructor
    /// @param[in] groupNumber dtc group number
    /// @param[out] dtcs All dtcs in this group
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01544
    /// @needwork = dd
    /// @endcode
    DtcGroup(uint32_t const groupNumber,
             std::set< uint32_t >& dtcGroupList,
             std::map< uint32_t, std::shared_ptr< Dtc > >& dtcs);

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01545
    /// @needwork = dd
    /// @endcode
    ~DtcGroup() = default;

    /// @brief Implementation function to clear a dtc group, initiate clear operation for all dtcs in the group one by one
    /// @param[in] isFromUds Whether the clear operation is initiated from UDS service
    /// @return Return value
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void Clear(bool const isFromUds) const;

    /// @brief Check whether this group is forbidden clear, it is true if all dtcs in this group are forbidden clear
    /// @return true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01546
    /// @needwork = dd
    /// @endcode
    bool IsForbiddenClear() const noexcept;

    /// @brief Check whether this group contains all dtcs
    /// @return true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01547
    /// @needwork = dd
    /// @endcode
    bool IsAllGroup() const noexcept { return (dtcGroupNumber_ == kGroupOfAllDTCs); }

    /// @brief Get group number
    /// @return Return number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01548
    /// @needwork = dd
    /// @endcode
    std::uint32_t GetGroupNumber() const noexcept { return dtcGroupNumber_; }

    /// @brief Get the number of dtcs in this group
    /// @return Return number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01549
    /// @needwork = dd
    /// @endcode
    std::size_t GetDtcCount() noexcept { return dtcs_.size(); }

    /// @brief Return the start iterator of this group, convenient for external traversal of this group to process dtcs
    /// @return Return iterator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01550
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< Dtc > >::const_iterator Begin() noexcept { return std::move(dtcs_.begin()); }

    /// @brief Return the end iterator of this group, convenient for external traversal of this group to process dtcs
    /// @return Return iterator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01551
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< Dtc > >::const_iterator End() noexcept { return std::move(dtcs_.end()); }

private:
    /// @name dtcGroupNumber_
    /// dtc group number
    /// [SWS_DM_00064].
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01552
    /// @needwork = dd
    /// @endcode
    uint32_t dtcGroupNumber_{0U};
    /// @name dtcs_
    /// All dtcs in this group
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01553
    /// @needwork = dd
    /// @endcode
    std::map< std::uint32_t, std::shared_ptr< Dtc > > dtcs_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_DTC_GROUP_H__