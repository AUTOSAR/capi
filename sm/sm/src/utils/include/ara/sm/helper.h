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
/// @file       helper.h
/// @brief      Define conversion functions from related structures to strings
/// @details
/// @date       2024-06-13
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Utils
/// @unit_name=Struct2String
/// @interface_level=module
/// @unit_description=Define conversion functions from related structures to strings
/// @trace_id_sr=SR_SM_01002, SR_SM_03002, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_10001
/// @endcode
///
/// ================================================================

#ifndef ARA_SM_HELPER_H_
#define ARA_SM_HELPER_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#ifdef ARA_WITH_DIAG
    #include <ara/diag/ecu_reset_request.h>
#endif
#include <ara/exec/execution_error_event.h>
#include <ara/exec/function_group.h>
#include <ara/exec/function_group_state.h>

#include <cctype>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <set>
#include <sstream>
#include <tuple>

#include "define.h"
#include "event.h"

namespace ara {
namespace sm {
namespace common {
/// @brief Define alias to resolve qac2428 item: Direct use of character type.
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using Char8_t = char;

/// @brief Define alias to resolve qac2428 item: Direct use of character type.
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using UChar8_t = unsigned char;

/// @brief Convert a numeric string to a number, exception-safe
/// @param numberStr The numeric string to be converted
/// @param number The converted number, only valid when the return value is True
/// @return true, conversion successful
/// @return false, conversion failed
/// @code{.isoft}
/// @tparam T Numeric type
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006
/// @trace_id_ad=AD_SM_00446
/// @trace_id_dd=DD_SM_00462
/// @needwork = ad
/// @endcode
template < typename T, std::enable_if_t< std::is_arithmetic< T >::value > * = nullptr >
bool String2Number(core::String const &numberStr, T &number) noexcept
{
    std::stringstream numberStream{numberStr.c_str()};
    numberStream >> number;
    if (numberStream.fail()) {
        return false;
    }
    if (!numberStream.eof()) {
        return false;
    }
    return true;
}

/// @brief Concatenate a String from a Vector<core::String>
/// @param strVec The Vector<string>
/// @returns The concatenated core::String
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_02001
/// @trace_id_ad=AD_SM_00447
/// @trace_id_dd=DD_SM_00463
/// @needwork = ad
/// @endcode
inline core::String ConcatenateStrings(core::Vector< core::String > const &strVec) noexcept
{
    core::String strRet;
    size_t const seperatorSize{strlen(common::GetkSeperator())};
    for (core::String const &s : strVec) {  // PRQA S 2961
        std::ignore = strRet.append(s);
        std::ignore = strRet.append(common::GetkSeperator());
    }
    if (seperatorSize + 1U < strRet.size()) {
        std::ignore = strRet.erase(strRet.size() - seperatorSize);
    }
    return strRet;
}

/// @brief Concatenate a String from a core::Map<core::String, core::String>
/// @param strMap Map<String, String>
/// @returns The concatenated core::String
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05003, SR_SM_05004, SR_SM_09004, SR_SM_09006
/// @trace_id_ad=AD_SM_00448
/// @trace_id_dd=DD_SM_00464
/// @needwork = ad
/// @endcode
inline core::String ConcatenateStrings(core::Map< core::String, core::String > const &strMap) noexcept
{
    core::String strRet;
    size_t const seperatorSize{strlen(common::GetkSeperatorSpace())};
    for (std::pair< ara::core::String const, ara::core::String > const &kv : strMap) {
        std::ignore = strRet.append(kv.first);
        std::ignore = strRet.append(":");
        std::ignore = strRet.append(kv.second);
        std::ignore = strRet.append(common::GetkSeperatorSpace());
    }
    if (seperatorSize + 1U < strRet.size()) {
        std::ignore = strRet.erase(strRet.size() - seperatorSize);
    }
    return strRet;
}

/// @brief Concatenate a String from a set<core::String>
/// @param strSet set<core::String>
/// @returns The concatenated core::String
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01002, SR_SM_03002, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_10001
/// @trace_id_ad=AD_SM_00449
/// @trace_id_dd=DD_SM_00465
/// @needwork = no
/// @endcode
inline core::String ConcatenateStrings(std::set< core::String > const &strSet) noexcept
{
    core::String strRet;
    size_t const seperatorSize{strlen(common::GetkSeperator())};
    for (core::String const &s : strSet) {
        std::ignore = strRet.append(s);
        std::ignore = strRet.append(common::GetkSeperator());
    }
    if (seperatorSize + 1U < strRet.size()) {
        std::ignore = strRet.erase(strRet.size() - seperatorSize);
    }
    return strRet;
}

/// @brief Concatenate a String from a core::Map<core::String, core::Vector<core::String>>
/// @param str2StrVec The Map<String, Vector<String>>
/// @returns The concatenated core::String
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05003, SR_SM_05004, SR_SM_09004, SR_SM_09006
/// @trace_id_ad=AD_SM_00450
/// @trace_id_dd=DD_SM_00466
/// @needwork = ad
/// @endcode
inline core::String ConcatenateStrings(
    core::Map< core::String, core::Vector< core::String > > const &str2StrVec) noexcept
{
    core::String strRet;
    size_t const seperatorSize{strlen(common::GetkSeperatorSpace())};
    for (std::pair< ara::core::String const, ara::sm::common::UcmFunctionGroupListInternal > const &pair :
         str2StrVec) {  // PRQA S 2961
        std::ignore = strRet.append(pair.first);
        std::ignore = strRet.append(":");
        std::ignore = strRet.append(ConcatenateStrings(pair.second));
        std::ignore = strRet.append(common::GetkSeperatorSpace());
    }
    if (seperatorSize + 1U < strRet.size()) {
        std::ignore = strRet.erase(strRet.size() - seperatorSize);
    }
    return strRet;
}

/// @brief Concatenate a String from a core::Map<core::String, core::Map<core::String, core::Vector<core::String>>>
/// @param str2Str2StrVec The Map<String, Map<String, Vector<String>>>
/// @returns The concatenated core::String
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05003, SR_SM_05004, SR_SM_09004, SR_SM_09006
/// @trace_id_ad=AD_SM_00451
/// @trace_id_dd=DD_SM_00467
/// @needwork = ad
/// @endcode
inline core::String ConcatenateStrings(
    core::Map< core::String, core::Map< core::String, core::Vector< core::String > > > const &str2Str2StrVec) noexcept
{
    core::String strRet;
    size_t const seperatorSize{strlen(common::GetkSeperatorSeperator())};
    for (std::pair< ara::core::String const, ara::core::Map< ara::core::String,
                                                             ara::sm::common::UcmFunctionGroupListInternal > > const
             &pair : str2Str2StrVec) {  // PRQA S 2961
        std::ignore = strRet.append(pair.first);
        std::ignore = strRet.append("::");
        std::ignore = strRet.append(ConcatenateStrings(pair.second));
        std::ignore = strRet.append(common::GetkSeperatorSeperator());
    }
    if (seperatorSize + 1U < strRet.size()) {
        std::ignore = strRet.erase(strRet.size() - seperatorSize);
    }
    return strRet;
}

/// @brief Concatenate a String from a core::Map<core::String, core::Map<core::String, core::Vector<core::String>>>
/// @param str2Str2Str The Map<String, Map<String, Vector<String>>>
/// @returns The concatenated core::String
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05003, SR_SM_05004, SR_SM_09004, SR_SM_09006
/// @trace_id_ad=AD_SM_00452
/// @trace_id_dd=DD_SM_00468
/// @needwork = ad
/// @endcode
inline core::String ConcatenateStrings(
    core::Map< core::String, core::Map< core::String, core::String > > const &str2Str2Str) noexcept
{
    core::String strRet;
    size_t const seperatorSize{strlen(common::GetkSeperatorSeperator())};
    for (std::pair< ara::core::String const, ara::core::Map< ara::core::String, ara::core::String > > const &pair :
         str2Str2Str) {  // PRQA S 2961
        std::ignore = strRet.append(pair.first);
        std::ignore = strRet.append("::");
        std::ignore = strRet.append(ConcatenateStrings(pair.second));
        std::ignore = strRet.append(common::GetkSeperatorSeperator());
    }
    if (seperatorSize + 1U < strRet.size()) {
        std::ignore = strRet.erase(strRet.size() - seperatorSize);
    }
    return strRet;
}

/// @brief Get a core::String from ara::exec::ExecutionErrorEvent
/// @param error the error to be converted
/// @return The converted result
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_06004, SR_SM_08001, SR_SM_09002
/// @trace_id_ad=AD_SM_00453
/// @trace_id_dd=DD_SM_00469
/// @needwork = ad
/// @endcode
inline core::String const ErrorEventToString(ara::exec::ExecutionErrorEvent const &error) noexcept
{
    return "executionError:" + std::to_string(error.executionError)
           + " functionGroup:" + core::String(error.functionGroup.GetMetaModelIdentifier());
}

/// @brief Converts a phm::TypeOfSupervision to its string representation
///
/// This function receives a TypeOfSupervision enumeration and returns the
/// corresponding textual representation. Its primary use-case is to make the
/// given type more human-readable.
///
/// @param type the TypeOfSupervision instance to be converted
/// @return The textual representation of the given TypeOfSupervision instance
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_00454
/// @trace_id_dd=DD_SM_00470
/// @needwork = ad
/// @endcode
inline core::String const TypeOfSupervisionToString(common::PhmTypeOfSupervisionInternal const &type) noexcept
{
    core::String strType{"kUnknown"};
    switch (type) {
        case common::PhmTypeOfSupervisionInternal::kAliveSupervision: {
            strType = "AliveSupervision";
        } break;

        case common::PhmTypeOfSupervisionInternal::kDeadlineSupervision: {
            strType = "DeadlineSupervision";
        } break;

        case common::PhmTypeOfSupervisionInternal::kLogicalSupervision: {
            strType = "LogicalSupervision";
        } break;

        default: {
            strType = "kUnknown";
        } break;
    }
    return strType;
}

/// @brief Converts a PHMHealthStatusType to its string representation
///
/// This function receives a PHMHealthStatusType enumeration and returns
/// the corresponding textual representation. This can be used to provide
/// a more human-readable information about the given health status type.
///
/// @param type the PHMHealthStatusType instance to be converted
/// @return The textual representation of the given PHMHealthStatusType instance
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_00455
/// @trace_id_dd=DD_SM_00471
/// @needwork = ad
/// @endcode
inline core::String const PHMHealthStatusTypeToString(PHMHealthStatusType const &type) noexcept
{
    core::String strType{"Unknown"};
    switch (type) {
        case PHMHealthStatusType::kTyrePressure: {
            strType = "kTyrePressure";
        } break;

        case PHMHealthStatusType::kVol: {
            strType = "kVol";
        } break;

        default: {
            strType = "kUnknown";
        } break;
    }
    return strType;
}

/// @brief Converts NetworkStateType enumeration value to a string
/// @param type The NetworkStateType enumeration value to convert
/// @return core::String The obtained string
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05003, SR_SM_05004
/// @trace_id_ad=AD_SM_00459
/// @trace_id_dd=DD_SM_00475
/// @needwork = ad
/// @endcode
inline core::String NetworkStateTypeToString(common::NetworkStateInternalType const &type) noexcept
{
    core::String strType{"kUnknown"};
    switch (type) {
        case common::NetworkStateInternalType::kNoCom: {
            strType = GetkNoComStr();
        } break;

        case common::NetworkStateInternalType::kFullCom: {
            strType = GetkFullComStr();
        } break;

        default: {
            strType = "kUnknown";
        } break;
    }
    return strType;
}
#ifdef ARA_WITH_DIAG
/// @brief Converts a ResetRequestType enumeration value to a string
/// @param type The ResetRequestType enumeration value to convert
/// @return core::String The obtained string
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_00460
/// @trace_id_dd=DD_SM_00476
/// @needwork = ad
/// @endcode
inline core::String ResetRequestTypeToString(common::DiagResetRequestInternalType const &type) noexcept
{
    core::String strType{"kUnknown"};
    switch (type) {
        case common::DiagResetRequestInternalType::kSoftReset: {
            strType = "kSoftReset";
        } break;

        case common::DiagResetRequestInternalType::kHardReset: {
            strType = "kHardReset";
        } break;

        case common::DiagResetRequestInternalType::kKeyOffOnReset: {
            strType = "kKeyOffOnReset";
        } break;

        case common::DiagResetRequestInternalType::kCustomReset: {
            strType = "kCustomReset";
        } break;
        default: {
            strType = "kUnknown";
        } break;
    }
    return strType;
}
#endif
/// @brief Converts all characters in a string to uppercase
/// @param s The string to transform
/// @return core::String The uppercase version of the original string
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006
/// @trace_id_ad=AD_SM_00463
/// @trace_id_dd=DD_SM_00479
/// @needwork = ad
/// @endcode
inline core::String ToUpper(core::String const &s) noexcept
{
    // correct
    core::String res;
    std::ignore = std::transform(s.begin(), s.end(), std::back_inserter(res), [](UChar8_t const c) noexcept {
        return static_cast< UChar8_t >(std::toupper(static_cast< int32_t >(c)));
    });
    return res;
}

/// @brief Splits a string into substrings based on a given delimiter
/// This function extracts substrings from the provided string 'str', divided by the specified delimiter 'deli'.
/// It then stores these substrings into a vector which is returned to the caller.
/// @param str The string to be sliced into substrings.
/// @param deli The delimiter used to distinguish the substrings.
/// @return Returns a vector of substrings obtained from 'str'.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00002
/// @trace_id_ad=AD_SM_00464
/// @trace_id_dd=DD_SM_00480
/// @needwork = ad
/// @endcode
inline core::Vector< core::String > SplitStr(core::String const &str, core::String const &deli) noexcept
{
    // Result to be returned
    core::Vector< core::String > res;

    // Start search position and found position
    core::String::size_type start{0U};
    core::String::size_type found{core::String::npos};
    do {
        found = str.find(deli, start);
        if (found == core::String::npos) {
            if (start != str.size()) {
                res.push_back(str.substr(start, found));
            }
        } else {
            res.push_back(str.substr(start, found - start));
            start = found + deli.size();
        }
    } while (found != core::String::npos);
    return res;
}

/// @brief Translates an instance of Event to a string
///
/// This function translates an Event instance into a readable string by first
/// using EventTypeToString() on the type field then depending on the type,
/// corresponding data field is converted to string using different functions.
///
/// @param type An instance of Event.
/// @return A string that represents the Event instance, or "Unknown" if the event type isn't recognized.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008,
/// SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001, SR_SM_09001, SR_SM_09002,
/// SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_00466
/// @trace_id_dd=DD_SM_00482
/// @needwork = ad
/// @endcode
inline core::String EventTypeToString(common::EventType const &type) noexcept
{
    core::String strType{"kUnknown"};
    switch (type) {
        case common::EventType::kInShellSetFGState: {
            strType = "kInShellSetFGState";
        } break;

        case common::EventType::kInShellGetFGState: {
            strType = "kInShellGetFGState";
        } break;

        case common::EventType::kInShellSetSMState: {
            strType = "kInShellSetSMState";
        } break;

        case common::EventType::kInShellGetSMState: {
            strType = "kInShellGetSMState";
        } break;

        case common::EventType::kInUCMResetMachine: {
            strType = "kInUCMResetMachine";
        } break;

        case common::EventType::kInUCMStartUpdateSession: {
            strType = "kInUCMStartUpdateSession";
        } break;

        case common::EventType::kInUCMStopUpdateSession: {
            strType = "kInUCMStopUpdateSession";
        } break;

        case common::EventType::kInUCMPrepareUpdate: {
            strType = "kInUCMPrepareUpdate";
        } break;

        case common::EventType::kInUCMVerifyUpdate: {
            strType = "kInUCMVerifyUpdate";
        } break;

        case common::EventType::kInUCMPrepareRollback: {
            strType = "kInUCMPrepareRollback";
        } break;

        case common::EventType::kInEMUndefinedStateCallback: {
            strType = "kInEMUndefinedStateCallback";
        } break;

        case common::EventType::kInPHMSupervisionRecoveryNotification: {
            strType = "kInPHMSupervisionRecoveryNotification";
        } break;

        case common::EventType::kInPHMHealthChannelRecoveryNotification: {
            strType = "kInPHMHealthChannelRecoveryNotification";
        } break;

        case common::EventType::kInNMNetworkStateChange: {
            strType = "kInNMNetworkStateChange";
        } break;

        case common::EventType::kOutNMSetNetworkState: {
            strType = "kOutNMSetNetworkState";
        } break;

        case common::EventType::kInAASetFGState: {
            strType = "kInAASetFGState";
        } break;

        case common::EventType::kInAAGetFGState: {
            strType = "kInAAGetFGState";
        } break;

        case common::EventType::kInGetAllFGState: {
            strType = "kInGetAllFGState";
        } break;

        case common::EventType::kInSMCSetSMState: {
            strType = "kInSMCSetSMState";
        } break;

        case common::EventType::kInGetSMState: {
            strType = "kInGetSMState";
        } break;

        case common::EventType::kInDiagEnableRapidShutdown: {
            strType = "kInDiagEnableRapidShutdown";
        } break;

        case common::EventType::kInDiagRequestReset: {
            strType = "kInDiagRequestReset";
        } break;

        case common::EventType::kOutNotifyFGStateChange: {
            strType = "kOutNotifyFGStateChange";
        } break;

        case common::EventType::kOutEMSetFGState: {
            strType = "kOutEMSetFGState";
        } break;

        case common::EventType::kOutNotifySMStateChange: {
            strType = "kOutNotifySMStateChange";
        } break;

        case common::EventType::kOutReportExecutionState: {
            strType = "kOutReportExecutionState";
        } break;

        case common::EventType::kInAASetEcuState: {
            strType = "kInAASetEcuState";
        } break;

        case common::EventType::kInGetEcuState: {
            strType = "kInGetEcuState";
        } break;

        case common::EventType::kOutNotifyEcuStateChange: {
            strType = "kOutNotifyEcuStateChange";
        } break;

        case common::EventType::kOutCheckInitialStateTransition: {
            strType = "kOutCheckInitialStateTransition";
        } break;

        case common::EventType::kInShellGetAllFGInfos: {
            strType = "kInShellGetAllFGInfos";
        } break;

        default: {
            strType = "Unknown";
        } break;
    }
    return strType;
}

/// @brief Constructs a string that represents an ara::sm::FGStateInfo instance
/// This function converts an ara::sm::FGStateInfo instance into a string by concatenating its Function Group (FG) name
/// and state.
/// @param info The ara::sm::FGStateInfo instance to be converted
/// @return A string of the form "fgFQN:[name] fgState:[state]"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01001, SR_SM_01003, SR_SM_02002
/// @trace_id_ad=AD_SM_00467
/// @trace_id_dd=DD_SM_00483
/// @needwork = ad
/// @endcode
inline core::String FGStateInfoToString(common::FGStateInfo const &info) noexcept
{
    return "fgFQN:" + info.fgFQN + " fgState:" + info.fgState;
}

/// @brief Constructs a string that represents an ara::sm::SMStateInfo instance
///
/// This function converts an ara::sm::SMStateInfo instance into a human-readable string format by concatenating its
/// State Machine (SM) name and state.
///
/// @param info The ara::sm::SMStateInfo instance to be converted
/// @return A string of the form "smName:[name] smState:[state]"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09008
/// @trace_id_ad=AD_SM_00468
/// @trace_id_dd=DD_SM_00484
/// @needwork = ad
/// @endcode
inline core::String SMStateInfoToString(common::SMStateInfo const &info) noexcept
{
    return "smFQN:" + info.smFQN + " smState:" + info.smState;
}

/// @brief Constructs a string representation of FGListInfo
/// This function uses ConcatenateStrings to join all elements in fgList into a single string.
/// @param info The ara::sm::FGListInfo instance to be converted
/// @return A string composed of all the elements in fgList concatenated into one
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_04007, SR_SM_04008, SR_SM_04009
/// @trace_id_ad=AD_SM_00469
/// @trace_id_dd=DD_SM_00485
/// @needwork = ad
/// @endcode
inline core::String FGListInfoToString(common::FGListInfo const &info) noexcept
{
    return ConcatenateStrings(info.fgList);
}

/// @brief Translates an instance of PHMSupervisionRecoveryNotificationInfo to a string
/// This function creates a string representation of a PHMSupervisionRecoveryNotificationInfo entity,
/// using 'executionError' and 'supervisionType' fields as input to other functions to build part of the string.
/// @param info An instance of PHMSupervisionRecoveryNotificationInfo
/// @return A string representation of the input, created by concatenating converted 'executionError' and
/// 'supervisionType'
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_00470
/// @trace_id_dd=DD_SM_00486
/// @needwork = ad
/// @endcode
inline core::String PHMSupervisionRecoveryNotificationInfoToString(
    common::PHMSupervisionRecoveryNotificationInfo const &info) noexcept
{
    return "executionError:(" + ErrorEventToString(info.executionError)
           + ") supervisionType:" + TypeOfSupervisionToString(info.supervisionType);
}

/// @brief Translates an instance of PHMHealthChannelRecoveryNotificationInfo to a string
/// This function creates a string representation of a PHMHealthChannelRecoveryNotificationInfo entity,
/// it uses 'healthStatusType' and 'healthStatus' fields and converts them to string.
/// @param info An instance of PHMHealthChannelRecoveryNotificationInfo
/// @return A string representation of the inputted information, assembled by converting 'healthStatusType' and 'healthStatus'
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_00471
/// @trace_id_dd=DD_SM_00487
/// @needwork = ad
/// @endcode
inline core::String PHMHealthChannelRecoveryNotificationInfoToString(
    common::PHMHealthChannelRecoveryNotificationInfo const &info) noexcept
{
    return "healthStatusType:" + PHMHealthStatusTypeToString(info.healthStatusType)
           + " healthStatus:" + std::to_string(info.healthStatus);
}

/// @brief Translates an instance of NetworkStateInfo to a string
/// This function creates a string representation of a NetworkStateInfo entity,
/// it uses the 'nmHandleName' and 'state' fields to form the string.
/// @param info An instance of NetworkStateInfo
/// @return A string representation of the inputted information, created by converting 'nmHandleName' and 'state'
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05003, SR_SM_05004
/// @trace_id_ad=AD_SM_00472
/// @trace_id_dd=DD_SM_00488
/// @needwork = ad
/// @endcode
inline core::String NetworkStateInfoToString(common::NetworkStateInfo const &info) noexcept
{
    return "nmHandleName:" + info.nmHandleName + " state:" + NetworkStateTypeToString(info.state);
}

#ifdef ARA_WITH_DIAG
/// @brief Constructs a string representation of a DiagRequestResetInfo instance.
///
/// This function takes a DiagRequestResetInfo instance as input and converts its 'resetType' and 'id' fields into a
/// string.
///
/// @param info The DiagRequestResetInfo instance to be translated.
/// @return A string that concatenates "resetType:", the reset type, "id:",
/// and the integer value of the 'id' field (or 0 if it does not have a value).
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_00473
/// @trace_id_dd=DD_SM_00489
/// @needwork = ad
/// @endcode
inline core::String DiagRequestResetInfoToString(common::DiagRequestResetInfo const &info) noexcept
{
    return "resetType: " + ResetRequestTypeToString(info.resetType) + ", addressType: " + info.addressType
           + ", address: " + core::to_string(info.address);
}
#endif
/// @brief Constructs a string that represents a SMStateRequestInfo instance
///
/// This function translates a SMStateRequestInfo instance into a readable string
/// by concatenating the smName and transitionRequest fields.
///
/// @param info The SMStateRequestInfo instance to be converted.
/// @return A string that concatenates "smName:", the smName field, "transitionRequest:",
/// and the transitionRequest field.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09001
/// @trace_id_ad=AD_SM_00474
/// @trace_id_dd=DD_SM_00490
/// @needwork = ad
/// @endcode
inline core::String SMStateRequestInfoToString(common::SMStateRequestInfo const &info) noexcept
{
    return "smName:" + info.smFQN + " transitionRequest:" + std::to_string(info.transitionRequest);
}

/// @brief Translates an instance of EventType to a string
///
/// This function switches through different possible values of the EventType enum, returning a string
/// representation for each recognized case.
///
/// @param event An EventType instance.
/// @return A string that represents the EventType, or "Unknown" if the passed value isn't recognized.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008,
/// SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001, SR_SM_09001, SR_SM_09002,
/// SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_00475
/// @trace_id_dd=DD_SM_00491
/// @needwork = ad
/// @endcode
inline core::String EventToString(common::Event const &event) noexcept
{
    ara::core::String const typeStr{"type:" + EventTypeToString(event.type)};
    ara::core::String strType{"Unknown"};
    switch (event.type) {
        case common::EventType::kInShellSetFGState:
        case common::EventType::kInAASetFGState:
        case common::EventType::kOutEMSetFGState:
        case common::EventType::kOutNotifyFGStateChange: {
            strType
                = typeStr + " data:(" + FGStateInfoToString(*static_cast< common::FGStateInfo * >(event.data)) + ")";
        } break;

        case common::EventType::kInShellSetSMState:
        case common::EventType::kOutNotifySMStateChange: {
            strType
                = typeStr + " data:(" + SMStateInfoToString(*static_cast< common::SMStateInfo * >(event.data)) + ")";
        } break;

        case common::EventType::kInSMCSetSMState: {
            strType = typeStr + " data:("
                      + SMStateRequestInfoToString(*static_cast< common::SMStateRequestInfo * >(event.data)) + ")";
        } break;

        case common::EventType::kInUCMResetMachine:
        case common::EventType::kInUCMStartUpdateSession:
        case common::EventType::kInUCMStopUpdateSession: {
            strType = typeStr + " data:()";
        } break;

        case common::EventType::kInUCMPrepareUpdate:
        case common::EventType::kInUCMVerifyUpdate:
        case common::EventType::kInUCMPrepareRollback: {
            strType = typeStr + " data:(" + FGListInfoToString(*static_cast< common::FGListInfo * >(event.data)) + ")";
        } break;

        case common::EventType::kInEMUndefinedStateCallback: {
            strType = typeStr + " data:("
                      + ErrorEventToString(*static_cast< ara::exec::ExecutionErrorEvent * >(event.data)) + ")";
        } break;

        case common::EventType::kInPHMSupervisionRecoveryNotification: {
            strType = typeStr + " data:("
                      + PHMSupervisionRecoveryNotificationInfoToString(
                          *static_cast< common::PHMSupervisionRecoveryNotificationInfo * >(event.data))
                      + ")";
        } break;

        case common::EventType::kInPHMHealthChannelRecoveryNotification: {
            strType = typeStr + " data:("
                      + PHMHealthChannelRecoveryNotificationInfoToString(
                          *static_cast< common::PHMHealthChannelRecoveryNotificationInfo * >(event.data))
                      + ")";
        } break;

        case common::EventType::kInNMNetworkStateChange:
        case common::EventType::kOutNMSetNetworkState: {
            strType = typeStr + " data:("
                      + NetworkStateInfoToString(*static_cast< common::NetworkStateInfo * >(event.data)) + ")";
        } break;

        case common::EventType::kInGetAllFGState:
        case common::EventType::kInGetEcuState:
        case common::EventType::kOutCheckInitialStateTransition: {
            strType = typeStr + " data:()";
        } break;

        case common::EventType::kInShellGetFGState:
        case common::EventType::kInShellGetSMState:
        case common::EventType::kInAAGetFGState:
        case common::EventType::kInAASetEcuState:
        case common::EventType::kInGetSMState:
        case common::EventType::kOutNotifyEcuStateChange: {
            strType = typeStr + " data:(" + *static_cast< core::String * >(event.data) + ")";
        } break;

        case common::EventType::kInDiagEnableRapidShutdown: {
            strType = typeStr + " data:()";
        } break;

        case common::EventType::kInDiagRequestReset: {
#ifdef ARA_WITH_DIAG
            strType = typeStr + " data:("
                      + DiagRequestResetInfoToString(*static_cast< common::DiagRequestResetInfo * >(event.data)) + ")";
#endif
        } break;

        case common::EventType::kInDiagExecuteReset: {
            strType = typeStr + " data:()";
        } break;

        case common::EventType::kOutReportExecutionState: {
            strType = typeStr + " data:(" + core::to_string(*(static_cast< int32_t * >(event.data))) + ")";
        } break;

        default: {
            strType = "Unknown";
        } break;
    }
    return strType;
}

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline core::String FunctionGroupsInfoToStr(core::Vector< common::FunctionGroupsInfoSM > const &infoList)
{
    core::String allRes;
    for (common::FunctionGroupsInfoSM const &info : infoList) {
        allRes = allRes + info.smFQN + GetkSeperator();
        allRes = allRes + ConcatenateStrings(info.smStates) + GetkSeperatorSeperator();
        for (common::FunctionGroupInfoSM const &fgInfo : info.fgInfos) {
            allRes = allRes + fgInfo.fgFQN + GetkSeperator();
            allRes = allRes + ConcatenateStrings(fgInfo.fgStates) + GetkSeperatorSeperator();
        }
        allRes = allRes + Getk2DotSeperator();
        for (common::TransitionItemSM const &item : info.transitionTable) {
            allRes = allRes + core::to_string(item.requestID) + GetkSeperator() + item.currentState + GetkSeperator()
                     + item.nextState;
            allRes = allRes + GetkSeperatorSeperator();
        }
        allRes = allRes + GetkSeperatorSpace();
    }
    return allRes;
}

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline core::Vector< common::FunctionGroupsInfoSM > StrToFunctionGroupsInfo(core::String const &str)
{
    core::Vector< common::FunctionGroupsInfoSM > infoList;
    core::Vector< core::String > infoStrList{SplitStr(str, GetkSeperatorSpace())};
    for (core::String const &infoStr : infoStrList) {
        common::FunctionGroupsInfoSM info;
        core::Vector< core::String > const allSubInfoStrList{SplitStr(infoStr, Getk2DotSeperator())};

        // Strings for state machine and its state, function group and its state
        core::String const otherInfoStr{allSubInfoStrList[0UL]};
        // The first element is the state machine FQN and state machine state string, subsequent elements are function group FQN and function group state
        core::Vector< core::String > subInfoStrList{SplitStr(otherInfoStr, GetkSeperatorSeperator())};
        core::String const smInfoStr{subInfoStrList[0UL]};
        core::Vector< core::String > smInfoStrList{SplitStr(smInfoStr, GetkSeperator())};
        info.smFQN = smInfoStrList[0UL];
        info.smStates.assign(smInfoStrList.begin() + 1, smInfoStrList.end());
        for (size_t i{ONE_UL}; i < subInfoStrList.size(); i++) {
            common::FunctionGroupInfoSM subfgInfo;
            core::Vector< core::String > fgInfoStrList{SplitStr(subInfoStrList[i], GetkSeperator())};
            subfgInfo.fgFQN = fgInfoStrList[0UL];
            subfgInfo.fgStates.assign(fgInfoStrList.begin() + 1, fgInfoStrList.end());
            info.fgInfos.push_back(std::move(subfgInfo));
        }
        if (info.smFQN.empty()) {
            info.isManagedBySm = false;
        } else {
            info.isManagedBySm = true;
            if (ONE_UL < allSubInfoStrList.size()) {
                // State transition table string
                core::String const transitonTableStr{allSubInfoStrList[ONE_UL]};
                core::Vector< core::String > const transitionItemStrList{
                    SplitStr(transitonTableStr, GetkSeperatorSeperator())};
                for (auto const &transitionItemStr : transitionItemStrList) {
                    common::TransitionItemSM itemInfo;
                    core::Vector< core::String > itemInfoStrList{SplitStr(transitionItemStr, GetkSeperator())};
                    std::ignore           = String2Number(itemInfoStrList[0UL], itemInfo.requestID);
                    itemInfo.currentState = itemInfoStrList[ONE_UL];
                    itemInfo.nextState    = itemInfoStrList[TWO_UL];
                    info.transitionTable.push_back(std::move(itemInfo));
                }
            }
        }
        infoList.push_back(std::move(info));
    }
    return infoList;
}
}  // namespace common
}  // namespace sm
}  // namespace ara

#endif  // ARA_SM_HELPER_H_
