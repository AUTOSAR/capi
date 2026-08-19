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
/// @file       unique_handle.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Exclusive access handle
/// @date       2021-04-28
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-04-28  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Pointer Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=UniqueHandle
/// @unit_description=Exclusive access handle
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_UNIQUE_HANDLE_H_
#define ARA_PER_UNIQUE_HANDLE_H_

#include <memory>

namespace ara {
namespace per {
//********************************/
/// @brief Encapsulation of exclusive pointer within persistence
/// @code{.isoft}
/// export_level=/Per
/// @traceid {SWS_PER_00359}
/// @endcode
/// @brief Handle to a ReadAccessor or ReadWriteAccessor.
/// A UniqueHandle is returned by the functions OpenFileReadOnly(), OpenFileWriteOnly(),
/// and OpenFileReadWrite().
template < typename T >
class UniqueHandle final
{
public:
    /// @brief Default constructor
    UniqueHandle() noexcept = default;
    /// @brief Default destructor
    ~UniqueHandle() noexcept = default;
    /// @brief
    /// @param[in] pHandle
    explicit UniqueHandle(std::unique_ptr< T >&& pHandle) noexcept : pHandle_{std::move(pHandle)} {}
    /// @brief Move constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00371}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Move constructor for UniqueHandle.
    /// The source handle object is invalidated and cannot be used anymore.
    /// The operator bool() shall be used to check the state of a handle object before
    /// using any other operators of the handle object.
    /// @param[in] uh The UniqueHandle object to be moved.
    UniqueHandle(UniqueHandle&& uh) noexcept : pHandle_{std::move(uh.pHandle_)} {}
    /// @brief Disable copy constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00373}
    /// @endcode
    /// @brief The copy constructor for UniqueHandle shall not be used.
    /// @param[in] a
    UniqueHandle(UniqueHandle const& a) = delete;
    /// @brief Move assignment function
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00372}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Move assignment operator for UniqueHandle.
    /// The source handle object is invalidated and cannot be used anymore.
    /// The operator bool() shall be used to check the state of a handle object before
    /// using any other operators of the handle object.
    /// @param[in] uh The UniqueHandle object to be moved.
    /// @returns The moved UniqueHandle object.
    UniqueHandle& operator=(UniqueHandle&& uh) & noexcept
    {
        pHandle_ = std::move(uh.pHandle_);
        return *this;
    }
    /// @brief Disable copy assignment function
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00374}
    /// @endcode
    /// @brief The copy assignment operator for UniqueHandle shall not be used.
    /// @param[in] a
    /// @return
    UniqueHandle& operator=(UniqueHandle const& a) = delete;
    /// @brief Overloaded bool operator function.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00399}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Handle state.
    /// True if the handle represents a valid object of the templated class,
    /// False if the handle is empty(e.g. after a move operation).
    /// Using other operators than bool() of an empty handle will result in undefined behavior.
    /// @return
    explicit operator bool() const noexcept { return (pHandle_) && (pHandle_.get()); }
    /// @brief Overloaded -> operator
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00360}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Non-constant arrow operator.
    /// @returns T*
    T* operator->() noexcept { return pHandle_.get(); }
    /// @brief Overloaded ->() const operator
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00361}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Constant arrow operator.
    /// @returns T const*
    T const* operator->() const noexcept { return pHandle_.get(); }
    /// @brief Overloaded * operator
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00400}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Non-constant dereference operator.
    /// @returns T &
    T& operator*() noexcept { return *(pHandle_.get()); }
    /// @brief Overloaded *() const operator
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00401}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Constant dereference operator.
    /// @returns T const &
    T const& operator*() const noexcept { return *(pHandle_.get()); }

private:
    /// @brief
    std::unique_ptr< T > pHandle_;
};
//********************************/
}  // namespace per
}  // namespace ara
#endif
