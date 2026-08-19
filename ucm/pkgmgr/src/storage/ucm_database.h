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
/// @file       ucm_database.h
/// @brief      Definition of the database for UCM execution actions
/// @details
/// @date       2022-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00032
/// @unit_name=UcmDatabase
/// @unit_description=An interface to a database where UCM stores all actions it has performed since its first execution
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_UCM_DATABASE_H_
#define ARA_UCM_PKGMGR_UCM_DATABASE_H_

#include "ara/ucm/pkgmgr/impl_type_gethistoryvectortype.h"
#include "common/alias.h"
#include "storage/reversible_action.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief An interface to a database where UCM stores all
/// actions it has performed since its first execution.
/// First implementation will simply use a Json file as a
/// persistent storage for the sake of simplicity.
/// In the future, PER lib might be used instead.
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00115, 245afb4f017b33c4f234c4c6971f8909056dfe8e}
/// @uptrace={SWS_UCM_00160, 86ad8df5a74be8af738fbee74671cd671a3def16}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10492
/// @needwork = ad
/// @endcode
class UcmDatabase
{
public:
    /// @brief Initializes a new instance of this class
    /// which allows to access the UCM database file
    /// @param historyPath history path
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10493
    /// @needwork = dda
    /// @endcode
    explicit UcmDatabase(AraString historyPath) : kHistoryFile{std::move(historyPath)}, lastSnapshot_{}, fullHistory_{}
    {
    }
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10494
    /// @needwork = dda
    /// @endcode
    ~UcmDatabase() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10495
    /// @needwork = dda
    /// @endcode
    UcmDatabase(UcmDatabase const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10496
    /// @needwork = dda
    /// @endcode
    UcmDatabase& operator=(UcmDatabase const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10497
    /// @needwork = dda
    /// @endcode
    UcmDatabase(UcmDatabase&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10498
    /// @needwork = dda
    /// @endcode
    UcmDatabase& operator=(UcmDatabase&& other) = delete;

    /// @brief Save the actions performed by the UCM
    /// during the current Update sequence
    ///
    /// @param actionsOfThisUpdateSequence history actions
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00115}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10499
    /// @needwork = dda
    /// @endcode
    AraResultVoid SaveLastSnapshot(AraList< GetHistoryType > const& actionsOfThisUpdateSequence) const;

    /// @brief Retrieve all the actions (and related information) performed by the UCM
    /// from the 1st execution of the UCM until the call to this method
    /// @throws no
    /// @return history actions
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10500
    /// @needwork = dda
    /// @endcode
    AraResult< GetHistoryVectorType > GetFullHistory();

private:
    /// @brief The file used as our UCM database
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10501
    /// @needwork = dda
    /// @endcode
    AraString const kHistoryFile;

    /// @brief The history information of all the Actions performed during this update sequence
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10502
    /// @needwork = dda
    /// @endcode
    GetHistoryVectorType lastSnapshot_;  // Not used for now

    /// @brief The list of all actions (and related information) ever executed by UCM
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10503
    /// @needwork = dda
    /// @endcode
    GetHistoryVectorType
        fullHistory_;  // Since a new UcmDatabase object is used each time currently, it is fine that fullHistory_ is not cleared
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_UCM_DATABASE_H_
