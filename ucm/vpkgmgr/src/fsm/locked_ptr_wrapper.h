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
/// @file       locked_ptr_wrapper.h
/// @brief      LockedPtrWrapper impl
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=LockedPtrWrapper
/// @unit_description=LockedPtrWrapper impl
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_LOCKED_PTR_WRAPPER_H
#define ARA_UCM_VPKGMGR_LOCKED_PTR_WRAPPER_H

#include <functional>
#include <memory>
#include <mutex>

namespace ara {
namespace ucm {
namespace vpkgmgr {

// This class represents a pointer wrapper that owns and manages another object, allowing that object to be accessed in exclusive mode.
/// @brief This class represents a pointer wrapper
/// that owns and manages another object
/// what can be accessed in the exclusive mode
///
/// @code{.isoft}
/// @tparam T pointer type
///
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00090
/// @trace_id_dd=DD_UCM_Master_00180
/// @needwork = ad
/// @endcode
template < typename T >
class LockedPtrWrapper
{
    /// @brief alias UPtr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using UPtr = std::unique_ptr< T >;
    /// @brief alias OnChangeHandler
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using OnChangeHandler = std::function< void(T const*, T const&) >;

    /// @brief Guard to synchronize pointer access
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00181
    /// @needwork = dda
    /// @endcode
    std::mutex lock_;
    /// @brief Internat pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00182
    /// @needwork = dda
    /// @endcode
    UPtr inner_;
    /// @brief Handler function what is called on internal pointer reset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00183
    /// @needwork = dda
    /// @endcode
    OnChangeHandler handler_;

public:
    /// @brief Initializes a new instance of this service with the given instance id
    /// @param ptr unique pointer to object to be stored
    /// @param handler function to be called on internal pointer reset
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00184
    /// @needwork = dda
    /// @endcode
    explicit LockedPtrWrapper(UPtr ptr, OnChangeHandler handler = OnChangeHandler())  // handler defaults to nullptr
        : inner_{std::move(ptr)}, handler_{handler}
    {
    }

    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00185
    /// @needwork = dda
    /// @endcode
    ~LockedPtrWrapper() = default;

    // copy and move operations are deleted
    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00186
    /// @needwork = dda
    /// @endcode
    LockedPtrWrapper(LockedPtrWrapper const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00187
    /// @needwork = dda
    /// @endcode
    LockedPtrWrapper& operator=(LockedPtrWrapper const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00188
    /// @needwork = dda
    /// @endcode
    LockedPtrWrapper(LockedPtrWrapper&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00189
    /// @needwork = dda
    /// @endcode
    LockedPtrWrapper& operator=(LockedPtrWrapper&& other) = delete;

    /// @brief Accessor class for managed object access in exclusive mode
    /// @code{.isoft}
    /// The Accessor class manages the managed object in exclusive mode; multiple threads cannot hold the object simultaneously
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00090
    /// @trace_id_dd=DD_UCM_Master_00190
    /// @needwork = dd
    /// @endcode
    class Accessor
    {
        /// @brief Internal guard for access synchronization
        /// This class guarantees an unlocked status on destruction (even if not called explicitly).
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00000
        /// @trace_id_dd=DD_UCM_Master_00191
        /// @needwork = dda
        /// @endcode
        std::unique_lock< std::mutex > guard_;

        /// @brief A reference to wrapper object
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00000
        /// @trace_id_dd=DD_UCM_Master_00192
        /// @needwork = dda
        /// @endcode
        std::reference_wrapper< LockedPtrWrapper > ptrWrapper_;

    public:
        /// @brief Creates an accessor
        /// @param lptr a pointer wrapper reference
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00000
        /// @trace_id_dd=DD_UCM_Master_00193
        /// @needwork = dda
        /// @endcode
        explicit Accessor(LockedPtrWrapper& lptr) : guard_{lptr.lock_}, ptrWrapper_{lptr} {}

        /// @brief destructor
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00000
        /// @trace_id_dd=DD_UCM_Master_00194
        /// @needwork = dda
        /// @endcode
        ~Accessor() = default;

        /// @brief copy constructor is deleted
        /// @param other
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00000
        /// @trace_id_dd=DD_UCM_Master_00195
        /// @needwork = dda
        /// @endcode
        Accessor(Accessor const& other) = delete;

        /// @brief move constructor
        /// @param other object to be moved
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00000
        /// @trace_id_dd=DD_UCM_Master_00196
        /// @needwork = dda
        /// @endcode
        Accessor(Accessor&& other) noexcept : guard_{std::move(other.guard_)}, ptrWrapper_{std::move(other.ptrWrapper_)}
        {
        }

        /// @brief copy assignment operator is deleted
        /// @param other
        /// @return ref
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00000
        /// @trace_id_dd=DD_UCM_Master_00197
        /// @needwork = dda
        /// @endcode
        Accessor& operator=(Accessor const& other) = delete;

        /// @brief Move assignment operator
        /// @param rhs object to be moved
        /// @return ref
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00000
        /// @trace_id_dd=DD_UCM_Master_00198
        /// @needwork = dda
        /// @endcode
        Accessor& operator=(Accessor&& rhs) noexcept
        {
            guard_      = std::move(rhs.guard_);
            ptrWrapper_ = std::move(rhs.ptrWrapper_);
        }

        /// @brief Provieds access to the object owned by wrapper
        /// @return A poineter to the object owned by wrapper
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00000
        /// @trace_id_dd=DD_UCM_Master_00199
        /// @needwork = dda
        /// @endcode
        T* operator->() const noexcept { return ptrWrapper_.get().inner_.get(); }

        /// @brief Replaces the managed object
        /// @param ptr pointer to a new object to manage
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00000
        /// @trace_id_dd=DD_UCM_Master_00200
        /// @needwork = dda
        /// @endcode
        void Reset(UPtr ptr) const
        {
            UPtr const prePtr{std::move(ptrWrapper_.get().inner_)};
            ptrWrapper_.get().inner_ = std::move(ptr);
            if (ptrWrapper_.get().handler_) {
                ptrWrapper_.get().handler_(prePtr.get(), *ptrWrapper_.get().inner_.get());
            }
        }
    };

    /// @brief Gets an access to a managed object
    /// @return Accessor object (used to call methods of managed object or replace object)
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00201
    /// @needwork = dda
    /// @endcode
    Accessor Get() { return Accessor{*this}; }
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_LOCKED_PTR_WRAPPER_H
