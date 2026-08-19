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
/// @file       result.h
/// @brief      This file provides result functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds
/// @interface_level=component
/// @trace_id_sr=78aa6839-9a51-4b3c-b259-504aea5cd8ab
/// @unit_name=result
/// @unit_description=This file provides result functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SRC_COMMON_RESULT_H_
#define ISOFT_UDS_SERVER_SRC_COMMON_RESULT_H_
#include <cstdint>
#include <memory>
namespace isoft {
namespace uds {
/// @brief Result template
/// @param T Result value type
/// @param E Error type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01313
/// @trace_id_dd=DD_UDS_02605
/// @needwork = ad
/// @endcode
template < typename T, typename E = std::int32_t >
class Result
{
private:
    /// @brief Result value pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00961
    /// @trace_id_dd=DD_UDS_02075
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< T > valuePtr_{nullptr};
    /// @brief Error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00962
    /// @trace_id_dd=DD_UDS_02076
    /// @needwork = ad
    /// @endcode
    E error_{0};

public:
    /// @brief Create result from result value
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @uptrace={SWS_CORE_00731, 1660713acd8eb4a730654c713c3c29501a69f39e}
    /// @endcode
    static Result FromValue(T const& t) { return Result(t); }
    /// @brief Create result from result value
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @uptrace={SWS_CORE_00732, e02fd637863a2d61455175d89d6c9ab50c8cf1f9}
    /// @endcode
    static Result FromValue(T&& t) { return Result(std::move(t)); }

    // ----------------------------------------
    /// @brief Create result from error code
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @uptrace={SWS_CORE_00734, 166384c8fd64b4d74a1f2bddc9b975f9a927956a}
    /// @endcode
    static Result FromError(E const& e) { return Result(e); }
    /// @brief Create result from error code
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @uptrace={SWS_CORE_00735, 1d6790688f3411cbc6040d8be27d2adbb017e58f}
    /// @endcode
    static Result FromError(E&& e) { return Result(std::move(e)); }
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00933
    /// @needwork = dda
    /// @endcode
    Result() noexcept = default;

    /// @brief Create result from result value
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00963
    /// @trace_id_dd=DD_UDS_02077
    /// @needwork = ad
    /// @endcode
    explicit Result(T const& value) noexcept : valuePtr_{std::make_shared< T >(value)} {}
    /// @brief Create result from error code
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00964
    /// @trace_id_dd=DD_UDS_02078
    /// @needwork = ad
    /// @endcode
    explicit Result(E const error) noexcept : error_{error} {}
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00965
    /// @trace_id_dd=DD_UDS_02079
    /// @needwork = ad
    /// @endcode
    ~Result() noexcept = default;

    /// @brief Copy constructor
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00966
    /// @trace_id_dd=DD_UDS_02080
    /// @needwork = ad
    /// @endcode
    Result(Result const& other) noexcept : valuePtr_{other.valuePtr_}, error_{other.error} {}

    /// @brief Move constructor
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00967
    /// @trace_id_dd=DD_UDS_02081
    /// @needwork = ad
    /// @endcode
    Result(Result&& other) noexcept(
        std::is_nothrow_move_constructible< T >::value&& std::is_nothrow_move_constructible< E >::value)
        = default;

    /// @brief Copy assignment operator
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00968
    /// @trace_id_dd=DD_UDS_02082
    /// @needwork = ad
    /// @endcode
    Result& operator=(Result const& other)
    {
        if (this != &other) {
            valuePtr_ = other.valuePtr_;
            error_    = other.error_;
        }
        return *this;
    }

    /// @brief Move assignment operator
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00969
    /// @trace_id_dd=DD_UDS_02083
    /// @needwork = ad
    /// @endcode
    Result& operator=(Result&& other) noexcept(
        std::is_nothrow_move_constructible< T >::value&& std::is_nothrow_move_assignable< T >::value&&
            std::is_nothrow_move_constructible< E >::value&& std::is_nothrow_move_assignable< E >::value)
        = default;

    /// @brief HasValue function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00970
    /// @trace_id_dd=DD_UDS_02084
    /// @needwork = ad
    /// @endcode
    bool HasValue() noexcept
    {
        if (error_ != 0) {
            return false;
        }
        return true;
    }
    /// @brief Error function
    /// @return Error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00971
    /// @trace_id_dd=DD_UDS_02085
    /// @needwork = ad
    /// @endcode
    E Error() noexcept { return error_; }

    /// @brief ValueOr function
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00972
    /// @trace_id_dd=DD_UDS_02086
    /// @needwork = ad
    /// @endcode
    void ValueOr(T& value) noexcept
    {
        valuePtr_ = std::make_shared< T >(value);
        error_    = 0;
    }

    /// @brief ErrorOr function
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00973
    /// @trace_id_dd=DD_UDS_02087
    /// @needwork = ad
    /// @endcode
    void ErrorOr(E const& error) noexcept
    {
        error_ = error;
        valuePtr_.reset();
    }
    /// @brief Value function
    /// @return Result value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00974
    /// @trace_id_dd=DD_UDS_02088
    /// @needwork = ad
    /// @endcode
    T& Value() noexcept { return *valuePtr_; }
};

/// @brief Result template
/// @param T Result value type
/// @param E Error code type
/// @interface_level=unit
/// @needwork=ad
/// @endcode
template < typename E >
class Result< void, E >
{
private:
    /// @brief Result value type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00975
    /// @trace_id_dd=DD_UDS_02089
    /// @needwork = ad
    /// @endcode
    struct Dummy
    {
    };
    /// @brief Result value type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00976
    /// @trace_id_dd=DD_UDS_02090
    /// @needwork = ad
    /// @endcode
    using T = Dummy;
    /// @brief Result value pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00977
    /// @trace_id_dd=DD_UDS_02091
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< T > valuePtr_{nullptr};
    /// @brief Error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00978
    /// @trace_id_dd=DD_UDS_02092
    /// @needwork = ad
    /// @endcode
    E error_{0};

public:
    /// @brief FromError function
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00979
    /// @trace_id_dd=DD_UDS_02093
    /// @needwork = ad
    /// @endcode
    static Result FromError(E const& e) { return Result(e); }

    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00980
    /// @trace_id_dd=DD_UDS_02094
    /// @needwork = ad
    /// @endcode
    Result() noexcept : valuePtr_{std::make_shared< T >()} {}

    /// @brief Constructor
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00981
    /// @trace_id_dd=DD_UDS_02095
    /// @needwork = ad
    /// @endcode
    explicit Result(E const error) noexcept : error_{error} {}

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00982
    /// @trace_id_dd=DD_UDS_02096
    /// @needwork = ad
    /// @endcode
    ~Result() noexcept = default;

    /// @brief Copy constructor
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @uptrace={SWS_CORE_00725, fd10125d250eb93b548c059aa7fdd3fa6c4a5307}
    /// @endcode
    Result(Result const& other) noexcept : valuePtr_{other.valuePtr_}, error_{other.error} {}

    /// @brief Move assignment operator
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @uptrace={SWS_CORE_00726, 60104d6ed6c104b6696daddf1a8e677725969382}
    /// @endcode
    Result(Result&& other) noexcept(
        std::is_nothrow_move_constructible< T >::value&& std::is_nothrow_move_constructible< E >::value)
        = default;

    /// @brief Copy assignment operator
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @uptrace={SWS_CORE_00741, b030501271a04d3434e86495c1c6548681bd50d6}
    /// @endcode
    Result& operator=(Result const& other)
    {
        if (this != &other) {
            valuePtr_ = other.valuePtr_;
            error_    = other.error_;
        }
        return *this;
    }

    /// @brief Move assignment operator
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @uptrace={SWS_CORE_00742, 8a8653c286b4f9aa83d7876a553b9999cab19fc0}
    /// @endcode
    Result& operator=(Result&& other) noexcept(
        std::is_nothrow_move_constructible< T >::value&& std::is_nothrow_move_assignable< T >::value&&
            std::is_nothrow_move_constructible< E >::value&& std::is_nothrow_move_assignable< E >::value)
        = default;

    /// @brief HasValue function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00983
    /// @trace_id_dd=DD_UDS_02097
    /// @needwork = ad
    /// @endcode
    bool HasValue() noexcept
    {
        if (error_ != 0) {
            return false;
        }
        return true;
    }

    /// @brief Error function
    /// @return Error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00984
    /// @trace_id_dd=DD_UDS_02098
    /// @needwork = ad
    /// @endcode
    E Error() noexcept { return error_; }

    /// @brief FromValue function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00985
    /// @trace_id_dd=DD_UDS_02099
    /// @needwork = ad
    /// @endcode
    static Result FromValue() noexcept { return Result(); }

    /// @brief ErrorOr function
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00986
    /// @trace_id_dd=DD_UDS_02100
    /// @needwork = ad
    /// @endcode
    void ErrorOr(E& error) noexcept
    {
        error_ = error;
        valuePtr_.reset();
    }
};
}  // namespace uds
}  // namespace isoft
#endif  /// ISOFT_UDS_SERVER_SRC_COMMON_RESULT_H_