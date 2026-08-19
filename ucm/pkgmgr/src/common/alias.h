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
/// @file       alias.h
/// @brief      type alias
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
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=Alias
/// @unit_description=Type alias
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_COMMON_ALIAS_H_
#define ARA_UCM_PKGMGR_COMMON_ALIAS_H_

#include <ara/core/future.h>
#include <ara/core/map.h>
#include <ara/core/optional.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <functional>
#include <list>

#include "ara/ucm/pkgmgr/packagemanagement_skeleton.h"  // skeleton::PackageManagementSkeleton  // generated

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief fundamental type wrapper
/// @brief char wrapper
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using char8_t = char;  // NOLINT
/// @brief unsigned char wrapper
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using uchar8_t = unsigned char;  // NOLINT

/// @brief std function wrapper
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using FunctionVoid = std::function< void() >;

constexpr int32_t kInt32_10{10};
constexpr int32_t kInt32_100{100};
constexpr int32_t kInt32_0755{0755};  // NOLINT

/// @brief basic type defined in ara
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using AraByte = ara::core::Byte;
/// @brief AraResult
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
template < typename T >
using AraResult = ara::core::Result< T >;
/// @brief AraResultVoid
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using AraResultVoid = AraResult< void >;

/// @brief basic container type defined in ara
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using AraStringView = ara::core::StringView;
/// @brief AraString
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using AraString = ara::core::String;
/// @brief AraVector
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
template < typename T >
using AraVector = ara::core::Vector< T >;
/// @brief AraVectorString
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using AraVectorString = std::list< AraString >;

/// @brief AraList
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
template < typename T >
using AraList = std::list< T >;
/// @brief AraMap
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
template < typename Key, typename Value >
using AraMap = ara::core::Map< Key, Value >;

/// @brief future type defined in ara
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
template < typename T >
using AraFuture = ara::core::Future< T >;
/// @brief AraFutureStatus
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using AraFutureStatus = ara::core::future_status;
/// @brief AraFutureVoid
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using AraFutureVoid = AraFuture< void >;
/// @brief FutureVoid
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using FutureVoid = AraFutureVoid;
/// @brief AraPromise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
template < typename T >
using AraPromise = ara::core::Promise< T >;
/// @brief AraPromiseVoid
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using AraPromiseVoid = AraPromise< void >;

/// @brief return value wrapper type defined in ara
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
template < typename T >
using AraOptional = ara::core::Optional< T >;

/// @brief ucm type wrapper
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using PMSkeleton = ara::ucm::pkgmgr::skeleton::PackageManagementSkeleton;

// for software package manager
/// @brief GetSwPackagesOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetSwPackagesOutput = PMSkeleton::GetSwPackagesOutput;
/// @brief FutureGetSwPackages
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using FutureGetSwPackages = AraFuture< GetSwPackagesOutput >;
/// @brief GetSwProcessProgressOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetSwProcessProgressOutput = PMSkeleton::GetSwProcessProgressOutput;
/// @brief FutureGetSwProcessProgress
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using FutureGetSwProcessProgress = AraFuture< PMSkeleton::GetSwProcessProgressOutput >;
/// using FutureDeleteTransfer = AraFutureVoid;
/// using FutureTransferData = AraFutureVoid;
/// using FutureTransferExit = AraFutureVoid;
/// @brief TransferStartOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using TransferStartOutput = PMSkeleton::TransferStartOutput;
/// @brief FutureTransferStart
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using FutureTransferStart = AraFuture< PMSkeleton::TransferStartOutput >;
// for software cluster manager
/// @brief GetSwClusterChangeInfoOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetSwClusterChangeInfoOutput = PMSkeleton::GetSwClusterChangeInfoOutput;
/// @brief FutureGetSwClusterChangeInfo
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using FutureGetSwClusterChangeInfo = AraFuture< GetSwClusterChangeInfoOutput >;
/// @brief GetSwClusterDescriptionOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetSwClusterDescriptionOutput = PMSkeleton::GetSwClusterDescriptionOutput;
/// @brief FutureGetSwClusterDescription
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using FutureGetSwClusterDescription = AraFuture< GetSwClusterDescriptionOutput >;
/// @brief GetSwClusterInfoOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetSwClusterInfoOutput = PMSkeleton::GetSwClusterInfoOutput;
/// @brief FutureGetSwClusterInfo
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using FutureGetSwClusterInfo = AraFuture< GetSwClusterInfoOutput >;
/// @brief GetHistoryOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetHistoryOutput = PMSkeleton::GetHistoryOutput;
/// @brief FutureGetHistory
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using FutureGetHistory = AraFuture< GetHistoryOutput >;
/// @brief GetIdOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetIdOutput = PMSkeleton::GetIdOutput;
/// @brief FutureGetId
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using FutureGetId = AraFuture< GetIdOutput >;

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_COMMON_ALIAS_H_
