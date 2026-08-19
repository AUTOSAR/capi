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
/// @file       single_ton.h
/// @brief      Singleton pattern implementation
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Utils
/// @module_path=/UCM Master/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=Singleton
/// @unit_description=Singleton pattern implementation
/// @endcode
///
/// ================================================================

#ifndef ISOFT_SINGLE_TON_H_
#define ISOFT_SINGLE_TON_H_

#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Singleton base class
/// @code{.isoft}
/// @tparam T
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00408
/// @trace_id_dd=DD_UCM_Master_00809
/// @needwork = ad
/// @endcode
template < typename T >
/// @brief Singleton
class Singleton
{
public:
    /// @brief Create
    /// @param args
    /// @return
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00409
    /// @trace_id_dd=DD_UCM_Master_00810
    /// @needwork = ad
    /// @endcode
    template < typename... Args >
    static T* Create(Args&&... args)
    {
        static std::once_flag s_Flag;

        std::call_once(s_Flag, [&args...]() {
            /// @brief Enable_Make_Unique
            struct Enable_Make_Unique : public T
            {
                /// @brief Create
                /// @param args
                /// @throws no
                explicit Enable_Make_Unique(Args&&... args) : T(args...) {}
            };

            s_Instance_ = std::make_unique< Enable_Make_Unique >(std::forward< Args >(args)...);
        });

        return s_Instance_.get();
    }

    /// @brief GetInstance
    /// @return T*
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00410
    /// @trace_id_dd=DD_UCM_Master_00811
    /// @needwork = ad
    /// @endcode
    static T* GetInstance() noexcept { return s_Instance_.get(); }

    /// @brief Release
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00411
    /// @trace_id_dd=DD_UCM_Master_00812
    /// @needwork = ad
    /// @endcode
    static void Release() noexcept { s_Instance_.reset(); }

protected:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00813
    /// @needwork = dda
    /// @endcode
    Singleton(void) = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00814
    /// @needwork = dda
    /// @endcode
    ~Singleton(void) = default;

public:
    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00815
    /// @needwork = dda
    /// @endcode
    Singleton(Singleton const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00816
    /// @needwork = dda
    /// @endcode
    Singleton& operator=(Singleton const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00817
    /// @needwork = dda
    /// @endcode
    Singleton(Singleton&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00818
    /// @needwork = dda
    /// @endcode
    Singleton& operator=(Singleton&& other) = delete;

protected:
    /// @brief s_Instance_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00408
    /// @trace_id_dd=DD_UCM_Master_00819
    /// @needwork = dd
    /// @endcode
    static std::unique_ptr< T > s_Instance_;
};

/// @brief s_Instance_
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00408
/// @trace_id_dd=DD_UCM_Master_00820
/// @needwork = dd
/// @endcode
template < class T >
std::unique_ptr< T > Singleton< T >::s_Instance_{nullptr};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ISOFT_SINGLE_TON_H_
