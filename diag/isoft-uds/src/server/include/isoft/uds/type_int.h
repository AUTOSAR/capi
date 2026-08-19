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
/// @file       type_int.h
/// @brief      This file provides type_int functionality
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
/// @trace_id_sr=f5cc2363-9a6c-4c71-9eea-6579e5e8b151
/// @unit_name=TypeInt
/// @unit_description=This file provides type_int functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_TYPE_INT_H_
#define ISOFT_UDS_TYPE_INT_H_

#include <cinttypes>

namespace isoft {
namespace uds {
namespace server {
//********************************/
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00891
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_1U{1U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00892
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_3U{3U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00893
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_5U{5U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00894
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_6U{6U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00895
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_7U{7U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00896
/// @needwork = dd
/// @endcode
constexpr uint8_t kSessionControlId{0x10U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00897
/// @needwork = dd
/// @endcode
constexpr uint8_t kEcuResetId{0x11U};
// /// @brief
// constexpr uint8_t kInt8_0x12U{0x12U};
// /// @brief
// constexpr uint8_t kInt8_0x13U{0x13U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00898
/// @needwork = dd
/// @endcode
constexpr uint8_t kClearInformationId{0x14U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00899
/// @needwork = dd
/// @endcode
constexpr uint8_t kReadDtcInformationId{0x19U};
// /// @brief
// constexpr uint8_t kInt8_0x21U{0x21U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00900
/// @needwork = dd
/// @endcode
constexpr uint8_t kReadDataByIdentifierId{0x22U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00901
/// @needwork = dd
/// @endcode
constexpr uint8_t kCommunicationControlId{0x28U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00902
/// @needwork = dd
/// @endcode
constexpr uint8_t kReadDataByPeriodicIdentifierId{0x2AU};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00903
/// @needwork = dd
/// @endcode
constexpr uint8_t kDynamicallyDefineDataIdentifierId{0x2CU};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00904
/// @needwork = dd
/// @endcode
constexpr uint8_t kWriteDataByIdentifierId{0x2EU};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00905
/// @needwork = dd
/// @endcode
constexpr uint8_t kSecurityAccessId{0x27U};

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00906
/// @needwork = dd
/// @endcode
constexpr uint8_t kAuthenticationId{0x29U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00907
/// @needwork = dd
/// @endcode
constexpr uint8_t kRoutineControlId{0x31U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00908
/// @needwork = dd
/// @endcode
constexpr uint8_t kRequestFileTransferId{0x38U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00909
/// @needwork = dd
/// @endcode
constexpr uint8_t kTesterPresentId{0x3EU};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00910
/// @needwork = dd
/// @endcode
constexpr uint8_t kResponseServiceIdMask{0x40U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00911
/// @needwork = dd
/// @endcode
constexpr uint8_t kNegativeResponseServiceId{0x7FU};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00912
/// @needwork = dd
/// @endcode
constexpr uint8_t kSuppressPosRspMsgIndicationMask{0x80U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00913
/// @needwork = dd
/// @endcode
constexpr uint8_t kControlDTCSettingId{0x85U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00914
/// @needwork = dd
/// @endcode
constexpr uint8_t kResponseOnEventId{0x86U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00915
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_0xFFU{0xFFU};
//********************************/
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00916
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt32_5U{5U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00917
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt32_7U{7U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00918
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt32_8U{8U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00919
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt32_10U{10U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00920
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt32_16U{16U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00921
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt32_24U{24U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00922
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt32_255U{255U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00923
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt32_256U{256U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00924
/// @needwork = dd
/// @endcode
constexpr uint32_t kMinPeriodicDataIdentifier{0xF200U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00925
/// @needwork = dd
/// @endcode
constexpr uint32_t kMaxPeriodicDataIdentifier{0xF2FFU};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00926
/// @needwork = dd
/// @endcode
constexpr uint32_t kMaxDynamicallyDefinedDataIdentifier{0xF3FFU};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00927
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt32_1000U{1000U};
//********************************/
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00928
/// @needwork = dd
/// @endcode
constexpr int32_t kInt32_5{5};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00929
/// @needwork = dd
/// @endcode
constexpr int32_t kInt32_6{6};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00930
/// @needwork = dd
/// @endcode
constexpr int32_t kInt32_8{8};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00931
/// @needwork = dd
/// @endcode
constexpr int32_t kInt32_16{16};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00932
/// @needwork = dd
/// @endcode
constexpr int32_t kInt32_112{112};
//********************************/
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif
