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
/// @file       grant_check_phm.h
/// @brief      IAM-PHM module logic processing function
/// @details
/// @date       2025-04-14
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author       <th>Description
/// <tr><td>2025-04-14 <td>1.0.0 <td>Han Yuxin <td>Refactored based on
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-PHM
/// @interface_level=module
/// @trace_id_sr=SR_IAM_00301
/// @unit_name=IAM_PHM
/// @unit_description=Check verification provided by IAM for the PHM module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_PHM_GRANT_CHECK_PHM_H_
#define ARA_IAM_PHM_GRANT_CHECK_PHM_H_

#include <ara/core/result.h>
#include <ara/core/string.h>

#include "config_iam_phm.h"
namespace ara {
namespace iam {
namespace internal {
namespace phm {
//********************************/
/// @brief IAM-Phm checker
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PGrantCheck_Phm final
{
public:
    /// @brief Shared pointer
    using Uptr = std::shared_ptr< PGrantCheck_Phm >;

public:
    /// @brief PGrantCheck_Phm constructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PGrantCheck_Phm() = default;

    /// @brief PGrantCheck_Phm Destruction.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    ~PGrantCheck_Phm() = default;

    /// @brief move constructor of PGrantCheck_Phm.
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PGrantCheck_Phm(PGrantCheck_Phm &&other) = delete;

    /// @brief copy constructor of PGrantCheck_Phm.
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PGrantCheck_Phm(PGrantCheck_Phm const &other) = delete;

    /// @brief assignment operator of PGrantCheck_Phm.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PGrantCheck_Phm &operator=(PGrantCheck_Phm const &other) = delete;

    /// @brief move assignment operator of PGrantCheck_Phm.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PGrantCheck_Phm &operator=(PGrantCheck_Phm &&other) = delete;

    /// @brief Load Crypto configuration data
    /// @brief crypto init  load  data .

    /// @return load success/falied.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    bool Initialize() noexcept;
    /// @brief Clear  All GrantInfo
    /// @returns true
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    bool Deinitialize() noexcept;
    /// @brief Whether initialization is successful
    /// @return true if has init manifest sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool IsReady() const noexcept;
    /// @brief grant checkpoint check.
    /// @param stProcess  checkpoint fqn
    /// @param nFindID checkpoint id
    /// @return checkpoint check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_CheckPoint(ara::core::String const &stProcess, uint32_t const &nFindID) noexcept;
    /// @brief grant ChannelGrantCheck.
    /// @param stProcess channel fqn
    /// @param nFindID channel id
    /// @return Channel check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_Channel(ara::core::String const &stProcess, uint32_t const &nFindID) noexcept;

private:
    PConfigIam_Phm iamConfigPhm_{};
};
//********************************/
}  // namespace phm
}  // namespace internal
}  // namespace iam
}  // namespace ara
#endif  // ARA_IAM_PHM_GRANT_CHECK_PHM_H_