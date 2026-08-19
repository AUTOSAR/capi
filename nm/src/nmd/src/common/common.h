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
/// @file       common.h
/// @brief      Public constants and type definitions
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/utils
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=utils
/// @unit_description=Public constants and type definitions
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_COMMON_H_
#define _ARA_NM_COMMON_H_

#include <ara/core/map.h>

#include <cmath>
#include <cstdint>

namespace ara {
namespace nm {
#ifdef ARA_NM_WITHOUT_COM
enum class NetworkStateType : std::uint8_t
{
    kNoCom              = 0U,
    kFullCom            = 1U,
    kBusSleep           = 2U,
    kPrepareBusSleep    = 3U,
    kRepeatMessageState = 4U,
    kNormalOperation    = 5U,
    kReadySleep         = 6U
};
enum class MessageCtrlType : std::uint8_t
{
    kEnableAll     = 0U,
    kEnableReceive = 1U,
    kEnableSend    = 2U,
    kDisableAll    = 3U
};
#endif
namespace internal {

/// @brief kSocketExtBuffer  Supplementary length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100006
/// @trace_id_dd=DD_NM_00745
/// @needwork = ad
/// @endcode
std::uint32_t const kSocketExtBuffer{200U};
/// @brief kPncVectorOffsetMin  Minimum PNC offset
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100007
/// @trace_id_dd=DD_NM_00746
/// @needwork = ad
/// @endcode
std::int32_t const kPncVectorOffsetMin{1};
/// @brief kPncVectorOffsetMax  Maximum PNC offset
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100008
/// @trace_id_dd=DD_NM_00747
/// @needwork = ad
/// @endcode
std::int32_t const kPncVectorOffsetMax{63};
/// @brief kPncVectorLengthMin  Minimum PNC length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100009
/// @trace_id_dd=DD_NM_00748
/// @needwork = ad
/// @endcode
std::int32_t const kPncVectorLengthMin{1};
/// @brief kPncVectorLengthMax  Maximum length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100010
/// @trace_id_dd=DD_NM_00749
/// @needwork = ad
/// @endcode
std::int32_t const kPncVectorLengthMax{63};

/// @brief kUdpNmRepeatMessageRequest  RMR request
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100011
/// @trace_id_dd=DD_NM_00750
/// @needwork = ad
/// @endcode
std::uint8_t const kUdpNmRepeatMessageRequest{0x01U};
/// @brief kUdpNmPniBitSet  PNI set
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100012
/// @trace_id_dd=DD_NM_00751
/// @needwork = ad
/// @endcode
std::uint8_t const kUdpNmPniBitSet{0x40U};
/// @brief kUdpNmRMRBitSet  Supplementary length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100013
/// @trace_id_dd=DD_NM_00752
/// @needwork = ad
/// @endcode
std::uint8_t const kUdpNmRMRBitSet{0x01U};
/// @brief kUdpNmActiveWakeBitSet  Wakeup set in CBV
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100014
/// @trace_id_dd=DD_NM_00753
/// @needwork = ad
/// @endcode
std::uint8_t const kUdpNmActiveWakeBitSet{0x10U};
/// @brief kTimerMillSecond  Minimum timer value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100015
/// @trace_id_dd=DD_NM_00754
/// @needwork = ad
/// @endcode
double const kTimerMillSecond{0.001};
/// @brief kDoubleNegactiveOne  For -1 judgment
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100016
/// @trace_id_dd=DD_NM_00755
/// @needwork = ad
/// @endcode
double const kDoubleNegactiveOne{-1.0};
/// @brief kInvalidPnID  Invalid PN value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100017
/// @trace_id_dd=DD_NM_00756
/// @needwork = ad
/// @endcode
std::uint16_t const kInvalidPnID{0xFFFFU};
/// @brief kDTCTimerDelay  Delay reporting DTC, delay duration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100018
/// @trace_id_dd=DD_NM_00757
/// @needwork = ad
/// @endcode
std::int32_t const kDTCTimerDelay{2000};
/// @brief kInitCheckCount  Initialization check count
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100019
/// @trace_id_dd=DD_NM_00758
/// @needwork = ad
/// @endcode
std::uint16_t const kInitCheckCount{3U};

/// @brief kNmStateBusSleep2RepeatMessage
/// Default value of nmstate in user data from bus sleep to repeat message
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100020
/// @trace_id_dd=DD_NM_00759
/// @needwork = ad
/// @endcode
std::uint8_t const kNmStateBusSleep2RepeatMessage{0X01U};

/// @brief kNmStatePrepareBusSleep2RepeatMessage
/// Default value of nmstate in user data from prepare bus sleep to repeat message
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100021
/// @trace_id_dd=DD_NM_00760
/// @needwork = ad
/// @endcode
std::uint8_t const kNmStatePrepareBusSleep2RepeatMessage{0X02U};

/// @brief kNmStateRepeatMessage2NormalOperation
/// Default value of nmstate in user data from repeat message to normal operation state
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100022
/// @trace_id_dd=DD_NM_00761
/// @needwork = ad
/// @endcode
std::uint8_t const kNmStateRepeatMessage2NormalOperation{0X04U};

/// @brief kNmStateReadySleep2NormalOperation
/// Default value of nmstate in user data from sleep ready to repeat message
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100023
/// @trace_id_dd=DD_NM_00762
/// @needwork = ad
/// @endcode
std::uint8_t const kNmStateReadySleep2NormalOperation{0X08U};

/// @brief kNmStateReadySleep2RepeatMessage
/// Default value of nmstate in user data from sleep ready to repeat message
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100024
/// @trace_id_dd=DD_NM_00763
/// @needwork = ad
/// @endcode
std::uint8_t const kNmStateReadySleep2RepeatMessage{0X10U};

/// @brief kNmStateNormalOperation2RepeatMessage
/// Default value of nmstate in user data from normal operation state to repeat message
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100025
/// @trace_id_dd=DD_NM_00764
/// @needwork = ad
/// @endcode
std::uint8_t const kNmStateNormalOperation2RepeatMessage{0X20U};

/// @brief kPncIDMax  Maximum PN ID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100026
/// @trace_id_dd=DD_NM_00765
/// @needwork = ad
/// @endcode
std::uint16_t const kPncIDMax{511U};

/// @brief kNmEtherInitErr  Initialization error code
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100027
/// @trace_id_dd=DD_NM_00766
/// @needwork = ad
/// @endcode
std::uint32_t const kNmEtherInitErr{1U};

/// @brief kNmConst8U  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100028
/// @trace_id_dd=DD_NM_00767
/// @needwork = ad
/// @endcode
std::size_t const kNmConst8U{8U};

/// @brief kNmConst1U  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100029
/// @trace_id_dd=DD_NM_00768
/// @needwork = ad
/// @endcode
std::size_t const kNmConst1U{1U};

/// @brief kNmConst2U  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100030
/// @trace_id_dd=DD_NM_00769
/// @needwork = ad
/// @endcode
std::size_t const kNmConst2U{2U};

/// @brief kNmConst3U  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100031
/// @trace_id_dd=DD_NM_00770
/// @needwork = ad
/// @endcode
std::size_t const kNmConst3U{3U};

/// @brief kNmConst4U  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100032
/// @trace_id_dd=DD_NM_00771
/// @needwork = ad
/// @endcode
std::size_t const kNmConst4U{4U};

/// @brief kNmConst5U  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100033
/// @trace_id_dd=DD_NM_00772
/// @needwork = ad
/// @endcode
std::size_t const kNmConst5U{5U};

/// @brief kNmConstMacByte2  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100034
/// @trace_id_dd=DD_NM_00773
/// @needwork = ad
/// @endcode
std::uint8_t const kNmConstMacByte2{0x5EU};

/// @brief kNmConstNegactive2  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100035
/// @trace_id_dd=DD_NM_00774
/// @needwork = ad
/// @endcode
std::int32_t const kNmConstNegactive2{-2};

/// @brief kNmConstNegactive3  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100036
/// @trace_id_dd=DD_NM_00775
/// @needwork = ad
/// @endcode
std::int32_t const kNmConstNegactive3{-3};

/// @brief kNmConstNegactive4  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100037
/// @trace_id_dd=DD_NM_00776
/// @needwork = ad
/// @endcode
std::int32_t const kNmConstNegactive4{-4};

/// @brief kNmConstNegactive5  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100038
/// @trace_id_dd=DD_NM_00777
/// @needwork = ad
/// @endcode
std::int32_t const kNmConstNegactive5{-5};

/// @brief kNmConstMacFF3  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100039
/// @trace_id_dd=DD_NM_00778
/// @needwork = ad
/// @endcode
std::uint32_t const kNmConstMacFF3{0x00FF0000U};

/// @brief kNmConstMacFF4  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100040
/// @trace_id_dd=DD_NM_00779
/// @needwork = ad
/// @endcode
std::uint32_t const kNmConstMacFF4{0x00000FF00U};

/// @brief kNmConstMacFF5  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100041
/// @trace_id_dd=DD_NM_00780
/// @needwork = ad
/// @endcode
std::uint32_t const kNmConstMacFF5{0x0000000FFU};

/// @brief kNmMacLen  const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100042
/// @trace_id_dd=DD_NM_00781
/// @needwork = ad
/// @endcode
std::size_t const kNmMacLen{6U};

/// @brief NmChar
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100003
/// @trace_id_dd=DD_NM_00956
/// @needwork = ad
/// @endcode
using NmChar = char;

/// @brief Nm Operation Code class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100043
/// @trace_id_dd=DD_NM_00782
/// @needwork = ad
/// @endcode
enum class NmOperCode : std::int8_t
{
    /// @brief Internal operation status, operation failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00957
    /// @needwork = dda
    /// @endcode
    kNmOperError = -1,
    /// @brief Internal operation status, operation succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00958
    /// @needwork = dda
    /// @endcode
    kNmOperOK = 0,
};

/// @brief ESMModeType class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100044
/// @trace_id_dd=DD_NM_00783
/// @needwork = ad
/// @endcode
enum class ESMModeType : std::uint8_t
{
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00959
    /// @needwork = dda
    /// @endcode
    kModeBusSleep = 1,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00960
    /// @needwork = dda
    /// @endcode
    kModePrepareBusSleep = 2,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00961
    /// @needwork = dda
    /// @endcode
    kModeNetwork = 3,
};

/// @brief EthernetStateMachineStateType class
/// The value remains the same as the service value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100045
/// @trace_id_dd=DD_NM_00784
/// @needwork = ad
/// @endcode
enum class EthernetStateMachineStateType : std::uint8_t
{
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00962
    /// @needwork = dda
    /// @endcode
    kStateBusSleep = 1,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00963
    /// @needwork = dda
    /// @endcode
    kStatePrepareBusSleep = 2,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00964
    /// @needwork = dda
    /// @endcode
    kStateReadySleep = 3,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00965
    /// @needwork = dda
    /// @endcode
    kStateNormalOperation = 4,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00966
    /// @needwork = dda
    /// @endcode
    kStateRepeatMessage = 5,
};

/// @brief PNCStateMachineStateType class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100046
/// @trace_id_dd=DD_NM_00785
/// @needwork = ad
/// @endcode
enum class PNCSMStateType : std::uint8_t
{
    kNmPncRequest         = 1,
    kNmPncReadySleep      = 2,
    kNmPncPrepareSleep    = 3,
    kNmPncNoCommunication = 4,
};
/// @brief kNmDot  Initialization check count
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100047
/// @trace_id_dd=DD_NM_00786
/// @needwork = ad
/// @endcode
NmChar const kNmDot{'.'};

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  // _ARA_NM_COMMON_H_
