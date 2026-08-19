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
/// @file       shared_handle.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Shared access handle
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
/// @unit_name=SharedHandle
/// @unit_description=Shared access handle
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_SHARED_HANDLE_H_
#define ARA_PER_SHARED_HANDLE_H_

#include <memory>

namespace ara {
namespace per {
//********************************/
/// @brief Encapsulation of shared pointer within persistence
/// @code{.isoft}
/// export_level=/Per
/// @traceid {SWS_PER_00362}
/// @endcode
/// @brief Handle to a File Storage or Key-Value Storage.
/// A SharedHandle is returned by the functions OpenFileStorage() and OpenKeyValueStorage()
/// and can be passed between threads as needed.
/// It provides the abstraction that is necessary to allow thread-safe implementation
/// of OpenFileStorage() and OpenKeyValueStorage().
template < typename T >
class SharedHandle final
{
public:
    /// @brief
    /// @param[in] pHandle
    explicit SharedHandle(std::shared_ptr< T > const &pHandle) noexcept : pHandle_{pHandle} {}
    /// @brief Default constructor
    SharedHandle() noexcept = default;
    /// @brief Default destructor
    ~SharedHandle() noexcept = default;
    /// @brief Move constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00367}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Move constructor for SharedHandle.
    /// The source handle object is invalidated and cannot be used anymore.
    /// The operator bool() shall be used to check the state of a handle object before
    /// using any other operators of the handle object.
    /// @param[in] sh The SharedHandle object to be moved.
    SharedHandle(SharedHandle &&sh) noexcept : pHandle_{std::move(sh.pHandle_)} {}
    /// @brief Copy constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00369}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Copy constructor for SharedHandle.
    /// @param[in] sh The SharedHandle object to be copied.
    SharedHandle(SharedHandle const &sh) noexcept : pHandle_{nullptr} { pHandle_ = sh.pHandle_; }
    /// @brief Move assignment function
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00368}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Move assignment operator for SharedHandle.
    /// The source handle object is invalidated and cannot be used anymore.
    /// The operator bool() shall be used to check the state of a handle object before
    /// using any other operators of the handle object.
    /// @param[in] sh The SharedHandle object to be moved.
    /// @returns The moved SharedHandle object.
    SharedHandle &operator=(SharedHandle &&sh) &noexcept
    {
        pHandle_ = std::move(sh.pHandle_);
        return *this;
    }
    /// @brief Copy assignment function
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00370}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Copy assignment operator for SharedHandle.
    /// @param[in] sh The SharedHandle object to be copied.
    /// @returns The moved SharedHandle object.
    SharedHandle &operator=(SharedHandle const &sh) &noexcept
    {
        if (this == &sh) {
            return *this;
        }
        pHandle_ = sh.pHandle_;
        return *this;
    }
    /// @brief Overloaded bool operator function.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00398}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Handle state.
    /// True if the handle represents a valid object of the templated class,
    /// False if the handle is empty(e.g. after a move operation).
    /// Using other operators than bool() of an empty handle will result in undefined behavior.
    /// @return
    explicit operator bool() const noexcept { return pHandle_.operator bool(); }
    /// @brief Overloaded -> operator
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00363}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Non-constant arrow operator.
    /// @returns T*
    T *operator->() noexcept { return pHandle_.get(); }
    /// @brief Overloaded ->() const operator
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00364}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Constant arrow operator.
    /// @returns T const*
    T const *operator->() const noexcept { return pHandle_.get(); }
    /// @brief Overloaded * operator
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00402}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Non-constant dereference operator.
    /// @returns T &
    T &operator*() noexcept { return *(pHandle_.get()); }
    /// @brief Overloaded *() const operator
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00403}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Constant dereference operator.
    /// @returns T const &
    T const &operator*() const noexcept { return *(pHandle_.get()); }

private:
    /// @brief
    std::shared_ptr< T > pHandle_;
};
//********************************/
}  // namespace per
}  // namespace ara

#endif
