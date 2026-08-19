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
/// @brief      pointer wrapper for exclusive access
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/Utils
/// @module_path=/UCM/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=LockedPtrWrapper
/// @unit_description=pointer wrapper for exclusive access
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_LOCKED_PTR_WRAPPER_H
#define ARA_UCM_PKGMGR_LOCKED_PTR_WRAPPER_H

#include <functional>
#include <memory>
#include <mutex>

namespace ara {
namespace ucm {
namespace pkgmgr {

// This class represents a pointer wrapper that owns and manages another object, allowing that object to be accessed in exclusive mode.
/// @brief This class represents a pointer wrapper
/// that owns and manages another object
/// what can be accessed in the exclusive mode
///
/// @code{.isoft}
/// @tparam T pointer type
///
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10322
/// @trace_id_dd=DD_UCM_10764
/// @needwork = ad
/// @endcode
template < typename T >
class LockedPtrWrapper
{
    /// @brief UPtr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using UPtr = std::unique_ptr< T >;
    /// @brief ChangeHandler
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using OnChangeHandler = std::function< void(T const&) >;

private:
    /// @brief Guard to synchronize pointer access
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10765
    /// @needwork = dda
    /// @endcode
    std::mutex lock_;
    /// @brief Internat pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10766
    /// @needwork = dda
    /// @endcode
    UPtr inner_;
    /// @brief Handler function what is called on internal pointer reset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10767
    /// @needwork = dda
    /// @endcode
    OnChangeHandler handler_;

public:
    /// @brief Initializes a new instance of this service with the given instance id
    ///
    /// @param ptr unique pointer to object to be stored
    /// @param handler function to be called on internal pointer reset
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10768
    /// @needwork = dda
    /// @endcode
    explicit LockedPtrWrapper(UPtr ptr,
                              OnChangeHandler handler = OnChangeHandler())  // default value for handler is nullptr
        : inner_{std::move(ptr)}, handler_{handler}
    {
        handler_(*inner_.get());  // first call after init
    }

    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10769
    /// @needwork = dda
    /// @endcode
    ~LockedPtrWrapper() = default;

    // copy and move operations are deleted
    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10770
    /// @needwork = dda
    /// @endcode
    LockedPtrWrapper(LockedPtrWrapper const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10771
    /// @needwork = dda
    /// @endcode
    LockedPtrWrapper& operator=(LockedPtrWrapper const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10772
    /// @needwork = dda
    /// @endcode
    LockedPtrWrapper(LockedPtrWrapper&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10773
    /// @needwork = dda
    /// @endcode
    LockedPtrWrapper& operator=(LockedPtrWrapper&& other) = delete;

    // The Accessor class accesses the managed object in exclusive mode
    /// @brief Accessor class for managed object access in exclusive mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10323
    /// @trace_id_dd=DD_UCM_10774
    /// @needwork = ad
    /// @endcode
    class Accessor
    {
    private:
        /// @brief Internal guard for access synchronization
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00000
        /// @trace_id_dd=DD_UCM_10775
        /// @needwork = dda
        /// @endcode
        std::unique_lock< std::mutex > guard_;
        // https://www.cplusplus.com/reference/mutex/unique_lock/:This class guarantees an unlocked status
        // on destruction (even if not called explicitly).

        /// @brief A reference to wrapper object
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00000
        /// @trace_id_dd=DD_UCM_10776
        /// @needwork = dda
        /// @endcode
        std::reference_wrapper< LockedPtrWrapper > ptrWrapper_;

    public:
        /// @brief Creates an accessor
        /// @param lptr a pointer wrapper reference
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00000
        /// @trace_id_dd=DD_UCM_10777
        /// @needwork = dda
        /// @endcode
        explicit Accessor(LockedPtrWrapper& lptr) : guard_{lptr.lock_}, ptrWrapper_{lptr} {}

        /// @brief Creates an accessor without lock
        /// @param lptr a pointer wrapper reference
        /// @param withLock not used
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00000
        /// @trace_id_dd=DD_UCM_10778
        /// @needwork = dda
        /// @endcode
        Accessor(LockedPtrWrapper& lptr, bool const withLock) noexcept : ptrWrapper_{lptr} { std::ignore = withLock; }

        /// @brief destructor
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00000
        /// @trace_id_dd=DD_UCM_10779
        /// @needwork = dda
        /// @endcode
        ~Accessor() = default;

        /// @brief copy constructor is deleted
        /// @param other
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00000
        /// @trace_id_dd=DD_UCM_10780
        /// @needwork = dda
        /// @endcode
        Accessor(Accessor const& other) = delete;

        /// @brief move constructor    --- The Accessor Get() below will theoretically call this move constructor (actually not called)
        ///
        /// @param other object to be moved
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00000
        /// @trace_id_dd=DD_UCM_10781
        /// @needwork = dda
        /// @endcode
        Accessor(Accessor&& other) noexcept : guard_{std::move(other.guard_)}, ptrWrapper_{std::move(other.ptrWrapper_)}
        {  // Note: This will call the copy constructor of reference_wrapper
            ///constuctor:reference_wrapper(const reference_wrapper&).
        }

        /// @brief copy assignment operator is deleted
        /// @param other
        /// @return ref
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00000
        /// @trace_id_dd=DD_UCM_10782
        /// @needwork = dda
        /// @endcode
        Accessor& operator=(Accessor const& other) = delete;

        /// @brief Move assignment operator   --- Not called in the code for now
        ///
        /// @param rhs object to be moved
        /// @return ref
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00000
        /// @trace_id_dd=DD_UCM_10783
        /// @needwork = dda
        /// @endcode
        Accessor& operator=(Accessor&& rhs) noexcept
        {
            guard_      = std::move(rhs.guard_);
            ptrWrapper_ = std::move(
                rhs.ptrWrapper_);  // Note: This will call the copy assignment operator of reference_wrapper:
                                   ///reference_wrapper& operator=(const reference_wrapper&)
        }

        /// @brief Provieds access to the object owned by wrapper
        /// @return A poineter to the object owned by wrapper
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00000
        /// @trace_id_dd=DD_UCM_10784
        /// @needwork = dda
        /// @endcode
        T* operator->() const noexcept { return ptrWrapper_.get().inner_.get(); }

        /// @brief Replaces the managed object
        /// @param ptr pointer to a new object to manage
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00000
        /// @trace_id_dd=DD_UCM_10785
        /// @needwork = dda
        /// @endcode
        void Reset(UPtr ptr) const
        {
            ptrWrapper_.get().inner_ = std::move(ptr);
            if (ptrWrapper_.get().handler_) {
                ptrWrapper_.get().handler_(*ptrWrapper_.get().inner_.get());
                // From https://en.cppreference.com/w/cpp/language/operator_precedence we know: Operator:
                // -> (Member access) has higher precedence than Operator: *a (Indirection (dereference))
            }
        }
    };

    /// @brief Gets an access to a managed object
    /// @return Accessor object (used to call methods of managed object or replace object)
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10786
    /// @needwork = dda
    /// @endcode
    Accessor Get() { return Accessor{*this}; }
    /// @brief Gets an access to a managed object without lock
    /// @return Accessor object
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10787
    /// @needwork = dda
    /// @endcode
    Accessor GetWithoutLock() noexcept { return {*this, false}; }
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_LOCKED_PTR_WRAPPER_H
