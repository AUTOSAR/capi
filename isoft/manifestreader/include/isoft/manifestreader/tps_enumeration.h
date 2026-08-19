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
/// @file       tps_enumeration.h
/// @brief
/// @details
/// @date       2022-08-17
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_MANIFESTREADER_TPS_ENUMERATION_H_
#define ISOFT_MANIFESTREADER_TPS_ENUMERATION_H_

#include <cstring>

namespace isoft {
namespace manifestreader {
namespace tps {

enum class AccessControlEnum
{
    kModeled = 0,  // The access restriction to the resource is modeled in the AUTOSAR Application Design model or the
                   // AUTOSAR Deployment model.
    kCustom = 1,   // The access restriction to the resource is defined by a non-AUTOSAR process.
};

template < typename StringType >
bool FromString(const StringType& string, AccessControlEnum& value)
{
    if (std::strcmp(string.c_str(), "MODELED") == 0) {
        value = AccessControlEnum::kModeled;
        return true;
    } else if (std::strcmp(string.c_str(), "CUSTOM") == 0) {
        value = AccessControlEnum::kCustom;
        return true;
    } else
        return false;
}

inline const char* ToString(AccessControlEnum value)
{
    switch (value) {
        case AccessControlEnum::kModeled:
            return "MODELED";
        case AccessControlEnum::kCustom:
            return "CUSTOM";
        default:
            return nullptr;
    }
}

enum class AclScopeEnum
{
    kDependant = 0,  // This specifies that the AclPermission applies to dependant (in particular referenced) operations
                     // / objects as well. Note that this includes the descendant ones.
    kDescendant = 1,  // This specifies that the AclPermission applies to descendant  operations / objects as well.
    kExplicit   = 2,  // This is indicates that the AclPermission applies to explicit objects / operations only.
};

template < typename StringType >
bool FromString(const StringType& string, AclScopeEnum& value)
{
    if (std::strcmp(string.c_str(), "DEPENDANT") == 0) {
        value = AclScopeEnum::kDependant;
        return true;
    } else if (std::strcmp(string.c_str(), "DESCENDANT") == 0) {
        value = AclScopeEnum::kDescendant;
        return true;
    } else if (std::strcmp(string.c_str(), "EXPLICIT") == 0) {
        value = AclScopeEnum::kExplicit;
        return true;
    } else
        return false;
}

inline const char* ToString(AclScopeEnum value)
{
    switch (value) {
        case AclScopeEnum::kDependant:
            return "DEPENDANT";
        case AclScopeEnum::kDescendant:
            return "DESCENDANT";
        case AclScopeEnum::kExplicit:
            return "EXPLICIT";
        default:
            return nullptr;
    }
}

enum class AdditionalBindingTimeEnum
{
    kBlueprintDerivationTime = 0,  // The point in time when an object is created from a blueprint.
    kPostBuild               = 1,  // After the executable has been built.
};

template < typename StringType >
bool FromString(const StringType& string, AdditionalBindingTimeEnum& value)
{
    if (std::strcmp(string.c_str(), "BLUEPRINT-DERIVATION-TIME") == 0) {
        value = AdditionalBindingTimeEnum::kBlueprintDerivationTime;
        return true;
    } else if (std::strcmp(string.c_str(), "POST-BUILD") == 0) {
        value = AdditionalBindingTimeEnum::kPostBuild;
        return true;
    } else
        return false;
}

inline const char* ToString(AdditionalBindingTimeEnum value)
{
    switch (value) {
        case AdditionalBindingTimeEnum::kBlueprintDerivationTime:
            return "BLUEPRINT-DERIVATION-TIME";
        case AdditionalBindingTimeEnum::kPostBuild:
            return "POST-BUILD";
        default:
            return nullptr;
    }
}

enum class AlignEnum
{
    kCenter  = 0,  // The content of the table is horizontally centered.
    kJustify = 1,  // This indicates that the content of table cell shall be justified (rendered as a block where
                   // white-space is expanded such that all lines are filled up).
    kLeft  = 2,    // This indicates that the content of a table cell is left justified.
    kRight = 3,    // This indicates that the content of a table cell is left justified.
};

template < typename StringType >
bool FromString(const StringType& string, AlignEnum& value)
{
    if (std::strcmp(string.c_str(), "CENTER") == 0) {
        value = AlignEnum::kCenter;
        return true;
    } else if (std::strcmp(string.c_str(), "JUSTIFY") == 0) {
        value = AlignEnum::kJustify;
        return true;
    } else if (std::strcmp(string.c_str(), "LEFT") == 0) {
        value = AlignEnum::kLeft;
        return true;
    } else if (std::strcmp(string.c_str(), "RIGHT") == 0) {
        value = AlignEnum::kRight;
        return true;
    } else
        return false;
}

inline const char* ToString(AlignEnum value)
{
    switch (value) {
        case AlignEnum::kCenter:
            return "CENTER";
        case AlignEnum::kJustify:
            return "JUSTIFY";
        case AlignEnum::kLeft:
            return "LEFT";
        case AlignEnum::kRight:
            return "RIGHT";
        default:
            return nullptr;
    }
}

enum class ApiPrincipleEnum
{
    kCommon = 0,  // The Rte or SchM API is provided for the whole software component / BSW Module
    kPerExecutable
    = 1,  // The Rte or SchM API is provided for a specific ExecutableEntity of a software component / BSW Module
};

template < typename StringType >
bool FromString(const StringType& string, ApiPrincipleEnum& value)
{
    if (std::strcmp(string.c_str(), "COMMON") == 0) {
        value = ApiPrincipleEnum::kCommon;
        return true;
    } else if (std::strcmp(string.c_str(), "PER-EXECUTABLE") == 0) {
        value = ApiPrincipleEnum::kPerExecutable;
        return true;
    } else
        return false;
}

inline const char* ToString(ApiPrincipleEnum value)
{
    switch (value) {
        case ApiPrincipleEnum::kCommon:
            return "COMMON";
        case ApiPrincipleEnum::kPerExecutable:
            return "PER-EXECUTABLE";
        default:
            return nullptr;
    }
}

enum class ArgumentDirectionEnum
{
    kIn    = 0,  // The argument value is passed to the callee.
    kInout = 1,  // The argument value is passed to the callee but also passed back from the callee to the caller.
    kOut   = 2,  // The argument value is passed from the callee  to the caller.
};

template < typename StringType >
bool FromString(const StringType& string, ArgumentDirectionEnum& value)
{
    if (std::strcmp(string.c_str(), "IN") == 0) {
        value = ArgumentDirectionEnum::kIn;
        return true;
    } else if (std::strcmp(string.c_str(), "INOUT") == 0) {
        value = ArgumentDirectionEnum::kInout;
        return true;
    } else if (std::strcmp(string.c_str(), "OUT") == 0) {
        value = ArgumentDirectionEnum::kOut;
        return true;
    } else
        return false;
}

inline const char* ToString(ArgumentDirectionEnum value)
{
    switch (value) {
        case ArgumentDirectionEnum::kIn:
            return "IN";
        case ArgumentDirectionEnum::kInout:
            return "INOUT";
        case ArgumentDirectionEnum::kOut:
            return "OUT";
        default:
            return nullptr;
    }
}

enum class ArrayImplPolicyEnum
{
    kPayloadAsArray = 0,  // This configuration demands the implementation of the payload as an array.
    kPayloadAsPointerToArray
    = 1,  // This configuration demands the implementation of the payload as a pointer to an array.
};

template < typename StringType >
bool FromString(const StringType& string, ArrayImplPolicyEnum& value)
{
    if (std::strcmp(string.c_str(), "PAYLOAD-AS-ARRAY") == 0) {
        value = ArrayImplPolicyEnum::kPayloadAsArray;
        return true;
    } else if (std::strcmp(string.c_str(), "PAYLOAD-AS-POINTER-TO-ARRAY") == 0) {
        value = ArrayImplPolicyEnum::kPayloadAsPointerToArray;
        return true;
    } else
        return false;
}

inline const char* ToString(ArrayImplPolicyEnum value)
{
    switch (value) {
        case ArrayImplPolicyEnum::kPayloadAsArray:
            return "PAYLOAD-AS-ARRAY";
        case ArrayImplPolicyEnum::kPayloadAsPointerToArray:
            return "PAYLOAD-AS-POINTER-TO-ARRAY";
        default:
            return nullptr;
    }
}

enum class ArraySizeHandlingEnum
{
    kAllIndicesDifferentArraySize      = 0,  // All elements of the variable size array may have different sizes.
    kAllIndicesSameArraySize           = 1,  // All elements of the variable size array have the same size.
    kInheritedFromArrayElementTypeSize = 2,  // The size of all dimensions of the variable size array is determined by
                                             // the size of the contained array element.
};

template < typename StringType >
bool FromString(const StringType& string, ArraySizeHandlingEnum& value)
{
    if (std::strcmp(string.c_str(), "ALL-INDICES-DIFFERENT-ARRAY-SIZE") == 0) {
        value = ArraySizeHandlingEnum::kAllIndicesDifferentArraySize;
        return true;
    } else if (std::strcmp(string.c_str(), "ALL-INDICES-SAME-ARRAY-SIZE") == 0) {
        value = ArraySizeHandlingEnum::kAllIndicesSameArraySize;
        return true;
    } else if (std::strcmp(string.c_str(), "INHERITED-FROM-ARRAY-ELEMENT-TYPE-SIZE") == 0) {
        value = ArraySizeHandlingEnum::kInheritedFromArrayElementTypeSize;
        return true;
    } else
        return false;
}

inline const char* ToString(ArraySizeHandlingEnum value)
{
    switch (value) {
        case ArraySizeHandlingEnum::kAllIndicesDifferentArraySize:
            return "ALL-INDICES-DIFFERENT-ARRAY-SIZE";
        case ArraySizeHandlingEnum::kAllIndicesSameArraySize:
            return "ALL-INDICES-SAME-ARRAY-SIZE";
        case ArraySizeHandlingEnum::kInheritedFromArrayElementTypeSize:
            return "INHERITED-FROM-ARRAY-ELEMENT-TYPE-SIZE";
        default:
            return nullptr;
    }
}

enum class ArraySizeSemanticsEnum
{
    kFixedSize    = 0,  // This means that the ApplicationArrayDataType will always have a fixed number of elements.
    kVariableSize = 1,  // This implies that the actual number of elements in the ApplicationArrayDataType might vary at
                        // run-time. The value of arraySize represents the maximum number of elements in the array.
};

template < typename StringType >
bool FromString(const StringType& string, ArraySizeSemanticsEnum& value)
{
    if (std::strcmp(string.c_str(), "FIXED-SIZE") == 0) {
        value = ArraySizeSemanticsEnum::kFixedSize;
        return true;
    } else if (std::strcmp(string.c_str(), "VARIABLE-SIZE") == 0) {
        value = ArraySizeSemanticsEnum::kVariableSize;
        return true;
    } else
        return false;
}

inline const char* ToString(ArraySizeSemanticsEnum value)
{
    switch (value) {
        case ArraySizeSemanticsEnum::kFixedSize:
            return "FIXED-SIZE";
        case ArraySizeSemanticsEnum::kVariableSize:
            return "VARIABLE-SIZE";
        default:
            return nullptr;
    }
}

enum class AutoCollectEnum
{
    kRefAll = 0,   // All objects being referenced (recursively) from the objects mentioned directly in the collection
                   // are also considered as part of the collection.
    kRefNone = 1,  // This indicates that only those objects mentioned directly in the collection are part of the
                   // collection. No other objects are considered further.
    kRefNonStandard
    = 2,  // This indicates that non standard objects ([TPS_GST_00088]) referenced (recursively) by the objects
          // mentioned directly in the collection are also considered to be part of the collection.
};

template < typename StringType >
bool FromString(const StringType& string, AutoCollectEnum& value)
{
    if (std::strcmp(string.c_str(), "REF-ALL") == 0) {
        value = AutoCollectEnum::kRefAll;
        return true;
    } else if (std::strcmp(string.c_str(), "REF-NONE") == 0) {
        value = AutoCollectEnum::kRefNone;
        return true;
    } else if (std::strcmp(string.c_str(), "REF-NON-STANDARD") == 0) {
        value = AutoCollectEnum::kRefNonStandard;
        return true;
    } else
        return false;
}

inline const char* ToString(AutoCollectEnum value)
{
    switch (value) {
        case AutoCollectEnum::kRefAll:
            return "REF-ALL";
        case AutoCollectEnum::kRefNone:
            return "REF-NONE";
        case AutoCollectEnum::kRefNonStandard:
            return "REF-NON-STANDARD";
        default:
            return nullptr;
    }
}

enum class BindingTimeEnum
{
    kCodeGenerationTime
    = 0,  // * Coding by hand, based on requirements document. * Tool based code generation, e.g. from a model. * The
          // model may contain variants. * Only code for the selected variant(s) is actually generated.
    kLinkTime = 1,  // Configure what is included in object code, and what is omitted Based on which variant(s) are
                    // selected  E.g. for modules that are delivered as object code (as opposed to those that are
                    // delivered as source code)
    kPreCompileTime = 2,  // This is typically the C-Preprocessor. Exclude parts of the code from the compilation
                          // process, e.g., because they are not required for the selected variant, because they are
                          // incompatible with the selected variant, because they require resources that are not present
                          // in the selected variant. Object code is only generated for the selected variant(s). The
                          // code that is excluded at this stage code will not be available at later stages.
    kSystemDesignTime = 3,  // * Designing the VFB. * Software Component types (PortInterfaces). * SWC Prototypes and
                            // the Connections between SWCprototypes. * Designing the Topology * ECUs and
                            // interconnecting Networks * Designing the Communication Matrix and Data Mapping
};

template < typename StringType >
bool FromString(const StringType& string, BindingTimeEnum& value)
{
    if (std::strcmp(string.c_str(), "CODE-GENERATION-TIME") == 0) {
        value = BindingTimeEnum::kCodeGenerationTime;
        return true;
    } else if (std::strcmp(string.c_str(), "LINK-TIME") == 0) {
        value = BindingTimeEnum::kLinkTime;
        return true;
    } else if (std::strcmp(string.c_str(), "PRE-COMPILE-TIME") == 0) {
        value = BindingTimeEnum::kPreCompileTime;
        return true;
    } else if (std::strcmp(string.c_str(), "SYSTEM-DESIGN-TIME") == 0) {
        value = BindingTimeEnum::kSystemDesignTime;
        return true;
    } else
        return false;
}

inline const char* ToString(BindingTimeEnum value)
{
    switch (value) {
        case BindingTimeEnum::kCodeGenerationTime:
            return "CODE-GENERATION-TIME";
        case BindingTimeEnum::kLinkTime:
            return "LINK-TIME";
        case BindingTimeEnum::kPreCompileTime:
            return "PRE-COMPILE-TIME";
        case BindingTimeEnum::kSystemDesignTime:
            return "SYSTEM-DESIGN-TIME";
        default:
            return nullptr;
    }
}

enum class BswEntryKindEnum
{
    kAbstract = 0,  // This BswModuleEntry specifies an abstract signature of C-functions. The signature needs to be
                    // implemented by concrete BswModuleEntrys
    kConcrete = 1,  // This BswModuleEntry specifies a concrete C-function with its signature.
};

template < typename StringType >
bool FromString(const StringType& string, BswEntryKindEnum& value)
{
    if (std::strcmp(string.c_str(), "ABSTRACT") == 0) {
        value = BswEntryKindEnum::kAbstract;
        return true;
    } else if (std::strcmp(string.c_str(), "CONCRETE") == 0) {
        value = BswEntryKindEnum::kConcrete;
        return true;
    } else
        return false;
}

inline const char* ToString(BswEntryKindEnum value)
{
    switch (value) {
        case BswEntryKindEnum::kAbstract:
            return "ABSTRACT";
        case BswEntryKindEnum::kConcrete:
            return "CONCRETE";
        default:
            return nullptr;
    }
}

enum class BswEntryRelationshipEnum
{
    kDerivedFrom = 0,  // Describes that the BswModuleEntry referenced as "to" needs to have the same signature as the
                       // "abstract" BswModuleEntry referenced as "from".
};

template < typename StringType >
bool FromString(const StringType& string, BswEntryRelationshipEnum& value)
{
    if (std::strcmp(string.c_str(), "DERIVED-FROM") == 0) {
        value = BswEntryRelationshipEnum::kDerivedFrom;
        return true;
    } else
        return false;
}

inline const char* ToString(BswEntryRelationshipEnum value)
{
    switch (value) {
        case BswEntryRelationshipEnum::kDerivedFrom:
            return "DERIVED-FROM";
        default:
            return nullptr;
    }
}

enum class BuildTypeEnum
{
    kBuildTypeRelease = 0,  // Used for releasing.
    kBuildTypeDebug   = 1,  // Used for debugging.
};

template < typename StringType >
bool FromString(const StringType& string, BuildTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "BUILD-TYPE-RELEASE") == 0) {
        value = BuildTypeEnum::kBuildTypeRelease;
        return true;
    } else if (std::strcmp(string.c_str(), "BUILD-TYPE-DEBUG") == 0) {
        value = BuildTypeEnum::kBuildTypeDebug;
        return true;
    } else
        return false;
}

inline const char* ToString(BuildTypeEnum value)
{
    switch (value) {
        case BuildTypeEnum::kBuildTypeRelease:
            return "BUILD-TYPE-RELEASE";
        case BuildTypeEnum::kBuildTypeDebug:
            return "BUILD-TYPE-DEBUG";
        default:
            return nullptr;
    }
}

enum class ByteOrderEnum
{
    kMostSignificantByteFirst
    = 0,  // Most significant byte shall come at the lowest address (also known as BigEndian or as Motorola-Format)
    kMostSignificantByteLast
    = 1,          // Most significant byte shall come highest address (also known as LittleEndian or as Intel-Format)
    kOpaque = 2,  // For opaque data endianness conversion has to be configured to Opaque. See AUTOSAR COM Specification
                  // for more details.
};

template < typename StringType >
bool FromString(const StringType& string, ByteOrderEnum& value)
{
    if (std::strcmp(string.c_str(), "MOST-SIGNIFICANT-BYTE-FIRST") == 0) {
        value = ByteOrderEnum::kMostSignificantByteFirst;
        return true;
    } else if (std::strcmp(string.c_str(), "MOST-SIGNIFICANT-BYTE-LAST") == 0) {
        value = ByteOrderEnum::kMostSignificantByteLast;
        return true;
    } else if (std::strcmp(string.c_str(), "OPAQUE") == 0) {
        value = ByteOrderEnum::kOpaque;
        return true;
    } else
        return false;
}

inline const char* ToString(ByteOrderEnum value)
{
    switch (value) {
        case ByteOrderEnum::kMostSignificantByteFirst:
            return "MOST-SIGNIFICANT-BYTE-FIRST";
        case ByteOrderEnum::kMostSignificantByteLast:
            return "MOST-SIGNIFICANT-BYTE-LAST";
        case ByteOrderEnum::kOpaque:
            return "OPAQUE";
        default:
            return nullptr;
    }
}

enum class CalprmAxisCategoryEnum
{
    kCom_axis
    = 0,  // COM_AXIS is equal to an STD_AXIS, the difference is, that a COM_AXIS is an shared axis, that means this
          // axis can be used multiple times by different CURVEs,  MAPs, CUBOIDs, CUBE_4s, and CUBE_5s.
    kComAxis = 1,  // COM-AXIS is equal to an STD_AXIS, the difference is, that a COM-AXIS is an shared axis, that means
                   // this axis can be used multiple times by different curves or maps. This value is obsolete.
    kCurve_axis = 2,  // CURVE_AXIS uses a separate CURVE to rescale the axis. The referenced CURVE is used to lookup an
                      // axis index, and the index value is used by the controller to determine the operating point in
                      // the CURVE, MAP, CUBOID, CUBE_4, or CUBE_5.
    kCurveAxis = 3,   // CURVE-AXIS uses a separate CURVE to rescale the axis. The referenced CURVE is used to lookup an
                      // axis index, and the index value is used by the controller to determine the operating point in
                      // the CURVE or MAP. This value is obsolete.
    kFix_axis = 4,  // FIX_AXIS means that the input axis is not stored. The axis is calculated using parameters  and so
                    // on it is also not possible to modify the axis points.
    kFixAxis = 5,   // FIX-AXIS means that the input axis is not stored. The axis is calculated using parameters  and so
                    // on it is also not possible to modify the axis points. This value is obsolete.
    kRes_axis = 6,  // RES_AXIS is also an shared axis like COM_AXIS, the difference is that this kind of axis can be
                    // used for rescaling.
    kResAxis = 7,   // RES-AXIS is also an shared axis like COM_AXIS, the difference is that this kind of axis can be
                    // used for rescaling. This value is obsolete.
    kStd_axis = 8,  // STD_AXIS means that input and output axis definition are stored within this CURVE, MAP, CUBOID,
                    // CUBE_4, and CUBE_5.  There is no shared or calculated axis.
    kStdAxis = 9,   // STD-AXIS means that input and output axis definition are stored within this CURVE. There is no
                    // shared or calculated axis. This value is obsolete.
};

template < typename StringType >
bool FromString(const StringType& string, CalprmAxisCategoryEnum& value)
{
    if (std::strcmp(string.c_str(), "COM_AXIS") == 0) {
        value = CalprmAxisCategoryEnum::kCom_axis;
        return true;
    } else if (std::strcmp(string.c_str(), "COM-AXIS") == 0) {
        value = CalprmAxisCategoryEnum::kComAxis;
        return true;
    } else if (std::strcmp(string.c_str(), "CURVE_AXIS") == 0) {
        value = CalprmAxisCategoryEnum::kCurve_axis;
        return true;
    } else if (std::strcmp(string.c_str(), "CURVE-AXIS") == 0) {
        value = CalprmAxisCategoryEnum::kCurveAxis;
        return true;
    } else if (std::strcmp(string.c_str(), "FIX_AXIS") == 0) {
        value = CalprmAxisCategoryEnum::kFix_axis;
        return true;
    } else if (std::strcmp(string.c_str(), "FIX-AXIS") == 0) {
        value = CalprmAxisCategoryEnum::kFixAxis;
        return true;
    } else if (std::strcmp(string.c_str(), "RES_AXIS") == 0) {
        value = CalprmAxisCategoryEnum::kRes_axis;
        return true;
    } else if (std::strcmp(string.c_str(), "RES-AXIS") == 0) {
        value = CalprmAxisCategoryEnum::kResAxis;
        return true;
    } else if (std::strcmp(string.c_str(), "STD_AXIS") == 0) {
        value = CalprmAxisCategoryEnum::kStd_axis;
        return true;
    } else if (std::strcmp(string.c_str(), "STD-AXIS") == 0) {
        value = CalprmAxisCategoryEnum::kStdAxis;
        return true;
    } else
        return false;
}

inline const char* ToString(CalprmAxisCategoryEnum value)
{
    switch (value) {
        case CalprmAxisCategoryEnum::kCom_axis:
            return "COM_AXIS";
        case CalprmAxisCategoryEnum::kComAxis:
            return "COM-AXIS";
        case CalprmAxisCategoryEnum::kCurve_axis:
            return "CURVE_AXIS";
        case CalprmAxisCategoryEnum::kCurveAxis:
            return "CURVE-AXIS";
        case CalprmAxisCategoryEnum::kFix_axis:
            return "FIX_AXIS";
        case CalprmAxisCategoryEnum::kFixAxis:
            return "FIX-AXIS";
        case CalprmAxisCategoryEnum::kRes_axis:
            return "RES_AXIS";
        case CalprmAxisCategoryEnum::kResAxis:
            return "RES-AXIS";
        case CalprmAxisCategoryEnum::kStd_axis:
            return "STD_AXIS";
        case CalprmAxisCategoryEnum::kStdAxis:
            return "STD-AXIS";
        default:
            return nullptr;
    }
}

enum class CanFrameRxBehaviorEnum
{
    kAny   = 0,  // This CAN frame may be received as both, CAN 2.0 and CAN FD.
    kCan20 = 1,  // This CAN frame shall be received as CAN 2.0 only. In case the CAN frame is received as CAN FD it is
                 // discarded during reception.
    kCanFd = 2,  // This CAN frame shall be received as CAN FD only. In case the CAN frame is received as CAN 2.0 it is
                 // discarded during reception.
};

template < typename StringType >
bool FromString(const StringType& string, CanFrameRxBehaviorEnum& value)
{
    if (std::strcmp(string.c_str(), "ANY") == 0) {
        value = CanFrameRxBehaviorEnum::kAny;
        return true;
    } else if (std::strcmp(string.c_str(), "CAN-20") == 0) {
        value = CanFrameRxBehaviorEnum::kCan20;
        return true;
    } else if (std::strcmp(string.c_str(), "CAN-FD") == 0) {
        value = CanFrameRxBehaviorEnum::kCanFd;
        return true;
    } else
        return false;
}

inline const char* ToString(CanFrameRxBehaviorEnum value)
{
    switch (value) {
        case CanFrameRxBehaviorEnum::kAny:
            return "ANY";
        case CanFrameRxBehaviorEnum::kCan20:
            return "CAN-20";
        case CanFrameRxBehaviorEnum::kCanFd:
            return "CAN-FD";
        default:
            return nullptr;
    }
}

enum class CanFrameTxBehaviorEnum
{
    kCan20 = 0,  // This CAN frame shall be sent as CAN 2.0 only.
    kCanFd = 1,  // This CAN frame shall be sent as CAN FD.
};

template < typename StringType >
bool FromString(const StringType& string, CanFrameTxBehaviorEnum& value)
{
    if (std::strcmp(string.c_str(), "CAN-20") == 0) {
        value = CanFrameTxBehaviorEnum::kCan20;
        return true;
    } else if (std::strcmp(string.c_str(), "CAN-FD") == 0) {
        value = CanFrameTxBehaviorEnum::kCanFd;
        return true;
    } else
        return false;
}

inline const char* ToString(CanFrameTxBehaviorEnum value)
{
    switch (value) {
        case CanFrameTxBehaviorEnum::kCan20:
            return "CAN-20";
        case CanFrameTxBehaviorEnum::kCanFd:
            return "CAN-FD";
        default:
            return nullptr;
    }
}

enum class ClientIntentEnum
{
    kWillCall = 0,  // The client will call this method.
    kWontCall = 1,  // The client won't call this method.
};

template < typename StringType >
bool FromString(const StringType& string, ClientIntentEnum& value)
{
    if (std::strcmp(string.c_str(), "WILL-CALL") == 0) {
        value = ClientIntentEnum::kWillCall;
        return true;
    } else if (std::strcmp(string.c_str(), "WONT-CALL") == 0) {
        value = ClientIntentEnum::kWontCall;
        return true;
    } else
        return false;
}

inline const char* ToString(ClientIntentEnum value)
{
    switch (value) {
        case ClientIntentEnum::kWillCall:
            return "WILL-CALL";
        case ClientIntentEnum::kWontCall:
            return "WONT-CALL";
        default:
            return nullptr;
    }
}

enum class ContainedIPduCollectionSemanticsEnum
{
    kLastIsBest
    = 0,  // The ContainedIPdu data will be fetched via TriggerTransmit just before the transmission executes.
    kQueued
    = 1,  // The ContainedIPdu data will instantly be stored to the ContainerIPdu in the context of the Transmit API.
};

template < typename StringType >
bool FromString(const StringType& string, ContainedIPduCollectionSemanticsEnum& value)
{
    if (std::strcmp(string.c_str(), "LAST-IS-BEST") == 0) {
        value = ContainedIPduCollectionSemanticsEnum::kLastIsBest;
        return true;
    } else if (std::strcmp(string.c_str(), "QUEUED") == 0) {
        value = ContainedIPduCollectionSemanticsEnum::kQueued;
        return true;
    } else
        return false;
}

inline const char* ToString(ContainedIPduCollectionSemanticsEnum value)
{
    switch (value) {
        case ContainedIPduCollectionSemanticsEnum::kLastIsBest:
            return "LAST-IS-BEST";
        case ContainedIPduCollectionSemanticsEnum::kQueued:
            return "QUEUED";
        default:
            return nullptr;
    }
}

enum class ContainerIPduHeaderTypeEnum
{
    kLongHeader  = 0,  // Header size is 64 bit: * Header Id 32 bit * Dlc 32 bit
    kShortHeader = 1,  // Header size is 32 bit: * Header Id 24 bit * Dlc 8 bit.
    kNoHeader
    = 2,  // No Header is used and the location of each containedPdu in the ContainerPdu is statically configured.
};

template < typename StringType >
bool FromString(const StringType& string, ContainerIPduHeaderTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "LONG-HEADER") == 0) {
        value = ContainerIPduHeaderTypeEnum::kLongHeader;
        return true;
    } else if (std::strcmp(string.c_str(), "SHORT-HEADER") == 0) {
        value = ContainerIPduHeaderTypeEnum::kShortHeader;
        return true;
    } else if (std::strcmp(string.c_str(), "NO-HEADER") == 0) {
        value = ContainerIPduHeaderTypeEnum::kNoHeader;
        return true;
    } else
        return false;
}

inline const char* ToString(ContainerIPduHeaderTypeEnum value)
{
    switch (value) {
        case ContainerIPduHeaderTypeEnum::kLongHeader:
            return "LONG-HEADER";
        case ContainerIPduHeaderTypeEnum::kShortHeader:
            return "SHORT-HEADER";
        case ContainerIPduHeaderTypeEnum::kNoHeader:
            return "NO-HEADER";
        default:
            return nullptr;
    }
}

enum class ContainerIPduTriggerEnum
{
    kDefaultTrigger = 0,  // Defines that the transmission of the ContainerIPdu shall be requested when the default
                          // trigger conditions apply (e.g. timeout of threshold).
    kFirstContainedTrigger = 1,  // Defines that the transmission of the ContainerIPdu shall be requested right after
                                 // the first ContainedIPdu was put into the ContainerIPdu.
};

template < typename StringType >
bool FromString(const StringType& string, ContainerIPduTriggerEnum& value)
{
    if (std::strcmp(string.c_str(), "DEFAULT-TRIGGER") == 0) {
        value = ContainerIPduTriggerEnum::kDefaultTrigger;
        return true;
    } else if (std::strcmp(string.c_str(), "FIRST-CONTAINED-TRIGGER") == 0) {
        value = ContainerIPduTriggerEnum::kFirstContainedTrigger;
        return true;
    } else
        return false;
}

inline const char* ToString(ContainerIPduTriggerEnum value)
{
    switch (value) {
        case ContainerIPduTriggerEnum::kDefaultTrigger:
            return "DEFAULT-TRIGGER";
        case ContainerIPduTriggerEnum::kFirstContainedTrigger:
            return "FIRST-CONTAINED-TRIGGER";
        default:
            return nullptr;
    }
}

enum class CouplingElementEnum
{
    kHub = 0,  // A device that is used to connect segments of a LAN. In Hubs frames are "broadcasted" to every one of
               // its ports.
    kRouter = 1,  // A device that routes frames between different networks.
    kSwitch = 2,  // A device that filters and forwards frames between different LAN segments.
};

template < typename StringType >
bool FromString(const StringType& string, CouplingElementEnum& value)
{
    if (std::strcmp(string.c_str(), "HUB") == 0) {
        value = CouplingElementEnum::kHub;
        return true;
    } else if (std::strcmp(string.c_str(), "ROUTER") == 0) {
        value = CouplingElementEnum::kRouter;
        return true;
    } else if (std::strcmp(string.c_str(), "SWITCH") == 0) {
        value = CouplingElementEnum::kSwitch;
        return true;
    } else
        return false;
}

inline const char* ToString(CouplingElementEnum value)
{
    switch (value) {
        case CouplingElementEnum::kHub:
            return "HUB";
        case CouplingElementEnum::kRouter:
            return "ROUTER";
        case CouplingElementEnum::kSwitch:
            return "SWITCH";
        default:
            return nullptr;
    }
}

enum class CouplingPortRatePolicyActionEnum
{
    kDropFrame   = 0,  // If the rate policy is violated the frame shall be dropped.
    kBlockSource = 1,  // If the rate policy is violated the CouplingPort this CouplingPortRatePolicy is defined on
                       // shall block all frames from the MAC-Address the violation was caused by.
};

template < typename StringType >
bool FromString(const StringType& string, CouplingPortRatePolicyActionEnum& value)
{
    if (std::strcmp(string.c_str(), "DROP-FRAME") == 0) {
        value = CouplingPortRatePolicyActionEnum::kDropFrame;
        return true;
    } else if (std::strcmp(string.c_str(), "BLOCK-SOURCE") == 0) {
        value = CouplingPortRatePolicyActionEnum::kBlockSource;
        return true;
    } else
        return false;
}

inline const char* ToString(CouplingPortRatePolicyActionEnum value)
{
    switch (value) {
        case CouplingPortRatePolicyActionEnum::kDropFrame:
            return "DROP-FRAME";
        case CouplingPortRatePolicyActionEnum::kBlockSource:
            return "BLOCK-SOURCE";
        default:
            return nullptr;
    }
}

enum class CouplingPortRoleEnum
{
    kHostPort = 0,  // The hostPort is connected to an ECU (host ecu). The host ECU controls the connected
                    // CouplingElement (e.g. Ethernet switch).
    kUpLinkPort
    = 1,  // A CouplingPort can be connected to another CouplingPort of a CouplingElement located on the same ECU
          // (CouplingElement.ecuInstance) using the CouplingPortConnection. This is used to model a cascaded switch.
    kStandardPort = 2,  // A CoupingPort can be a standardPort that is used to connect the CouplingElement with
                        // CouplingPorts outside the ECU.
};

template < typename StringType >
bool FromString(const StringType& string, CouplingPortRoleEnum& value)
{
    if (std::strcmp(string.c_str(), "HOST-PORT") == 0) {
        value = CouplingPortRoleEnum::kHostPort;
        return true;
    } else if (std::strcmp(string.c_str(), "UP-LINK-PORT") == 0) {
        value = CouplingPortRoleEnum::kUpLinkPort;
        return true;
    } else if (std::strcmp(string.c_str(), "STANDARD-PORT") == 0) {
        value = CouplingPortRoleEnum::kStandardPort;
        return true;
    } else
        return false;
}

inline const char* ToString(CouplingPortRoleEnum value)
{
    switch (value) {
        case CouplingPortRoleEnum::kHostPort:
            return "HOST-PORT";
        case CouplingPortRoleEnum::kUpLinkPort:
            return "UP-LINK-PORT";
        case CouplingPortRoleEnum::kStandardPort:
            return "STANDARD-PORT";
        default:
            return nullptr;
    }
}

enum class CryptoCertificateAlgorithmFamilyEnum
{
    kRsa = 1,  // The cryptographic operations in the certificate are executed using the RSA approach.
    kEcc = 2,  // The cryptographic operations in the certificate are executed using elliptic curves (ecc)
};

template < typename StringType >
bool FromString(const StringType& string, CryptoCertificateAlgorithmFamilyEnum& value)
{
    if (std::strcmp(string.c_str(), "RSA") == 0) {
        value = CryptoCertificateAlgorithmFamilyEnum::kRsa;
        return true;
    } else if (std::strcmp(string.c_str(), "ECC") == 0) {
        value = CryptoCertificateAlgorithmFamilyEnum::kEcc;
        return true;
    } else
        return false;
}

inline const char* ToString(CryptoCertificateAlgorithmFamilyEnum value)
{
    switch (value) {
        case CryptoCertificateAlgorithmFamilyEnum::kRsa:
            return "RSA";
        case CryptoCertificateAlgorithmFamilyEnum::kEcc:
            return "ECC";
        default:
            return nullptr;
    }
}

enum class CryptoCertificateFormatEnum
{
    kX509 = 1,  // The certificate is created in X.509 format.
    kCvc  = 2,  // The certificate has been created in Card Verifiable Certificate (CVC) format
};

template < typename StringType >
bool FromString(const StringType& string, CryptoCertificateFormatEnum& value)
{
    if (std::strcmp(string.c_str(), "X-509") == 0) {
        value = CryptoCertificateFormatEnum::kX509;
        return true;
    } else if (std::strcmp(string.c_str(), "CVC") == 0) {
        value = CryptoCertificateFormatEnum::kCvc;
        return true;
    } else
        return false;
}

inline const char* ToString(CryptoCertificateFormatEnum value)
{
    switch (value) {
        case CryptoCertificateFormatEnum::kX509:
            return "X-509";
        case CryptoCertificateFormatEnum::kCvc:
            return "CVC";
        default:
            return nullptr;
    }
}

enum class CryptoKeySlotTypeEnum
{
    kMachine = 0,  // Key slot is used by platform modules only. The application manages the key but is not able to use
                   // the key.
    kApplication = 1,  // KeySlot is used and modified exclusively by the Application.
};

template < typename StringType >
bool FromString(const StringType& string, CryptoKeySlotTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "MACHINE") == 0) {
        value = CryptoKeySlotTypeEnum::kMachine;
        return true;
    } else if (std::strcmp(string.c_str(), "APPLICATION") == 0) {
        value = CryptoKeySlotTypeEnum::kApplication;
        return true;
    } else
        return false;
}

inline const char* ToString(CryptoKeySlotTypeEnum value)
{
    switch (value) {
        case CryptoKeySlotTypeEnum::kMachine:
            return "MACHINE";
        case CryptoKeySlotTypeEnum::kApplication:
            return "APPLICATION";
        default:
            return nullptr;
    }
}

enum class CryptoKeySlotUsageEnum
{
    kVerification = 0,  // Key slot usage for verification
    kEncryption   = 1,  // Key slot usage for enryption
};

template < typename StringType >
bool FromString(const StringType& string, CryptoKeySlotUsageEnum& value)
{
    if (std::strcmp(string.c_str(), "VERIFICATION") == 0) {
        value = CryptoKeySlotUsageEnum::kVerification;
        return true;
    } else if (std::strcmp(string.c_str(), "ENCRYPTION") == 0) {
        value = CryptoKeySlotUsageEnum::kEncryption;
        return true;
    } else
        return false;
}

inline const char* ToString(CryptoKeySlotUsageEnum value)
{
    switch (value) {
        case CryptoKeySlotUsageEnum::kVerification:
            return "VERIFICATION";
        case CryptoKeySlotUsageEnum::kEncryption:
            return "ENCRYPTION";
        default:
            return nullptr;
    }
}

enum class CryptoObjectTypeEnum
{
    kUndefined    = 0,  // Object type unknown
    kSymmetricKey = 1,  // cryp::SymmetricKey object
    kPrivateKey   = 2,  // cryp::PrivateKey object
    kPublicKey    = 3,  // cryp::PublicKey object
    kSignature    = 4,  // cryp::Signature object (asymmetric digital signature or symmetric MAC/HMAC)
    kSecretSeed   = 5,  // cryp::SecretSeed object
};

template < typename StringType >
bool FromString(const StringType& string, CryptoObjectTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "UNDEFINED") == 0) {
        value = CryptoObjectTypeEnum::kUndefined;
        return true;
    } else if (std::strcmp(string.c_str(), "SYMMETRIC-KEY") == 0) {
        value = CryptoObjectTypeEnum::kSymmetricKey;
        return true;
    } else if (std::strcmp(string.c_str(), "PRIVATE-KEY") == 0) {
        value = CryptoObjectTypeEnum::kPrivateKey;
        return true;
    } else if (std::strcmp(string.c_str(), "PUBLIC-KEY") == 0) {
        value = CryptoObjectTypeEnum::kPublicKey;
        return true;
    } else if (std::strcmp(string.c_str(), "SIGNATURE") == 0) {
        value = CryptoObjectTypeEnum::kSignature;
        return true;
    } else if (std::strcmp(string.c_str(), "SECRET-SEED") == 0) {
        value = CryptoObjectTypeEnum::kSecretSeed;
        return true;
    } else
        return false;
}

inline const char* ToString(CryptoObjectTypeEnum value)
{
    switch (value) {
        case CryptoObjectTypeEnum::kUndefined:
            return "UNDEFINED";
        case CryptoObjectTypeEnum::kSymmetricKey:
            return "SYMMETRIC-KEY";
        case CryptoObjectTypeEnum::kPrivateKey:
            return "PRIVATE-KEY";
        case CryptoObjectTypeEnum::kPublicKey:
            return "PUBLIC-KEY";
        case CryptoObjectTypeEnum::kSignature:
            return "SIGNATURE";
        case CryptoObjectTypeEnum::kSecretSeed:
            return "SECRET-SEED";
        default:
            return nullptr;
    }
}

enum class CryptoServiceKeyGenerationEnum
{
    kKeyDerivation = 0,  // This means that the crypto key is created by derivation from a master key.
    kKeyStorage = 1,  // This means that the crypto key is obtained from an external entity, e.g. a diagnostic tester.
};

template < typename StringType >
bool FromString(const StringType& string, CryptoServiceKeyGenerationEnum& value)
{
    if (std::strcmp(string.c_str(), "KEY-DERIVATION") == 0) {
        value = CryptoServiceKeyGenerationEnum::kKeyDerivation;
        return true;
    } else if (std::strcmp(string.c_str(), "KEY-STORAGE") == 0) {
        value = CryptoServiceKeyGenerationEnum::kKeyStorage;
        return true;
    } else
        return false;
}

inline const char* ToString(CryptoServiceKeyGenerationEnum value)
{
    switch (value) {
        case CryptoServiceKeyGenerationEnum::kKeyDerivation:
            return "KEY-DERIVATION";
        case CryptoServiceKeyGenerationEnum::kKeyStorage:
            return "KEY-STORAGE";
        default:
            return nullptr;
    }
}

enum class CsTransformerErrorReactionEnum
{
    kApplicationOnly = 0,  // The application is responsible for any error reaction. No autonomous error reaction of RTE
                           // and transformer.
    kAutonomous = 1,       // RTE and Transformer coordinate an autonomous error reaction on their own.
};

template < typename StringType >
bool FromString(const StringType& string, CsTransformerErrorReactionEnum& value)
{
    if (std::strcmp(string.c_str(), "APPLICATION-ONLY") == 0) {
        value = CsTransformerErrorReactionEnum::kApplicationOnly;
        return true;
    } else if (std::strcmp(string.c_str(), "AUTONOMOUS") == 0) {
        value = CsTransformerErrorReactionEnum::kAutonomous;
        return true;
    } else
        return false;
}

inline const char* ToString(CsTransformerErrorReactionEnum value)
{
    switch (value) {
        case CsTransformerErrorReactionEnum::kApplicationOnly:
            return "APPLICATION-ONLY";
        case CsTransformerErrorReactionEnum::kAutonomous:
            return "AUTONOMOUS";
        default:
            return nullptr;
    }
}

enum class DataFilterTypeEnum
{
    kAlways = 0,  // No filtering is performed so that the message always passes.
    kMaskedNewDiffersMaskedOld
    = 1,  // Pass messages where the masked value has changed.  (new_value&mask) !=(old_value&mask)  new_value:
          // current value of the message  old_value: last value of the message (initialized with the initial value of
          // the message, updated with new_value if the new message value is not filtered out)
    kMaskedNewDiffersX = 2,  // Pass messages whose masked value is not equal to a specific value x  (new_value&mask) !=
                             // x  new_value: current value of the message
    kMaskedNewEqualsX = 3,   // Pass messages whose masked value is equal to a specific value x  (new_value&mask) == x
                             // new_value: current value of the message
    kNever = 4,              // The filter removes all messages.
    kNewIsOutside
    = 5,  // Pass a message if its value is outside a predefined boundary.  (min > new_value) OR (new_value > max)
    kNewIsWithin = 6,  // Pass a message if its value is within a predefined boundary.  min <= new_value <= max
    kOneEveryN   = 7,  // Pass a message once every N message occurrences.  Algorithm: occurrence % period == offset
                       // Start: occurrence = 0.  Each time the message is received or transmitted, occurrence is
                       // incremented by 1 after filtering. Length of occurrence is 8 bit (minimum).
};

template < typename StringType >
bool FromString(const StringType& string, DataFilterTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "ALWAYS") == 0) {
        value = DataFilterTypeEnum::kAlways;
        return true;
    } else if (std::strcmp(string.c_str(), "MASKED-NEW-DIFFERS-MASKED-OLD") == 0) {
        value = DataFilterTypeEnum::kMaskedNewDiffersMaskedOld;
        return true;
    } else if (std::strcmp(string.c_str(), "MASKED-NEW-DIFFERS-X") == 0) {
        value = DataFilterTypeEnum::kMaskedNewDiffersX;
        return true;
    } else if (std::strcmp(string.c_str(), "MASKED-NEW-EQUALS-X") == 0) {
        value = DataFilterTypeEnum::kMaskedNewEqualsX;
        return true;
    } else if (std::strcmp(string.c_str(), "NEVER") == 0) {
        value = DataFilterTypeEnum::kNever;
        return true;
    } else if (std::strcmp(string.c_str(), "NEW-IS-OUTSIDE") == 0) {
        value = DataFilterTypeEnum::kNewIsOutside;
        return true;
    } else if (std::strcmp(string.c_str(), "NEW-IS-WITHIN") == 0) {
        value = DataFilterTypeEnum::kNewIsWithin;
        return true;
    } else if (std::strcmp(string.c_str(), "ONE-EVERY-N") == 0) {
        value = DataFilterTypeEnum::kOneEveryN;
        return true;
    } else
        return false;
}

inline const char* ToString(DataFilterTypeEnum value)
{
    switch (value) {
        case DataFilterTypeEnum::kAlways:
            return "ALWAYS";
        case DataFilterTypeEnum::kMaskedNewDiffersMaskedOld:
            return "MASKED-NEW-DIFFERS-MASKED-OLD";
        case DataFilterTypeEnum::kMaskedNewDiffersX:
            return "MASKED-NEW-DIFFERS-X";
        case DataFilterTypeEnum::kMaskedNewEqualsX:
            return "MASKED-NEW-EQUALS-X";
        case DataFilterTypeEnum::kNever:
            return "NEVER";
        case DataFilterTypeEnum::kNewIsOutside:
            return "NEW-IS-OUTSIDE";
        case DataFilterTypeEnum::kNewIsWithin:
            return "NEW-IS-WITHIN";
        case DataFilterTypeEnum::kOneEveryN:
            return "ONE-EVERY-N";
        default:
            return nullptr;
    }
}

enum class DataIdModeEnum
{
    kAll16Bit        = 0,  // Two bytes are included in the CRC (double ID configuration).
    kAlternating8Bit = 1,  // One of the two bytes byte is included, alternating high and low byte, depending on parity
                           // of the counter (alternating ID configuration). For even counter low byte is included; For
                           // odd counters the high byte is included.
    kLower12Bit = 2,  // The low byte is included in the implicit CRC calculation, the low nibble of the high byte is
                      // transmitted along with the data (i.e. it is explicitly included), the high nibble of the high
                      // byte is not used. This is applicable for the IDs up to 12 bits.
    kLower8Bit = 3,   // Only low byte is included, high byte is never used. This is applicable if the IDs in a
                      // particular system are 8 bits.
};

template < typename StringType >
bool FromString(const StringType& string, DataIdModeEnum& value)
{
    if (std::strcmp(string.c_str(), "ALL-16-BIT") == 0) {
        value = DataIdModeEnum::kAll16Bit;
        return true;
    } else if (std::strcmp(string.c_str(), "ALTERNATING-8-BIT") == 0) {
        value = DataIdModeEnum::kAlternating8Bit;
        return true;
    } else if (std::strcmp(string.c_str(), "LOWER-12-BIT") == 0) {
        value = DataIdModeEnum::kLower12Bit;
        return true;
    } else if (std::strcmp(string.c_str(), "LOWER-8-BIT") == 0) {
        value = DataIdModeEnum::kLower8Bit;
        return true;
    } else
        return false;
}

inline const char* ToString(DataIdModeEnum value)
{
    switch (value) {
        case DataIdModeEnum::kAll16Bit:
            return "ALL-16-BIT";
        case DataIdModeEnum::kAlternating8Bit:
            return "ALTERNATING-8-BIT";
        case DataIdModeEnum::kLower12Bit:
            return "LOWER-12-BIT";
        case DataIdModeEnum::kLower8Bit:
            return "LOWER-8-BIT";
        default:
            return nullptr;
    }
}

enum class DataLimitKindEnum
{
    kMax  = 0,  // Limitation to maximum value
    kMin  = 1,  // Limitation to minimum value
    kNone = 2,  // No limitation applicable
};

template < typename StringType >
bool FromString(const StringType& string, DataLimitKindEnum& value)
{
    if (std::strcmp(string.c_str(), "MAX") == 0) {
        value = DataLimitKindEnum::kMax;
        return true;
    } else if (std::strcmp(string.c_str(), "MIN") == 0) {
        value = DataLimitKindEnum::kMin;
        return true;
    } else if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = DataLimitKindEnum::kNone;
        return true;
    } else
        return false;
}

inline const char* ToString(DataLimitKindEnum value)
{
    switch (value) {
        case DataLimitKindEnum::kMax:
            return "MAX";
        case DataLimitKindEnum::kMin:
            return "MIN";
        case DataLimitKindEnum::kNone:
            return "NONE";
        default:
            return nullptr;
    }
}

enum class DataTransformationErrorHandlingEnum
{
    kNoTransformerErrorHandling = 0,  // A runnable does not handle transformer errors.
    kTransformerErrorHandling   = 1,  // The runnable implements the handling of transformer errors.
};

template < typename StringType >
bool FromString(const StringType& string, DataTransformationErrorHandlingEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-TRANSFORMER-ERROR-HANDLING") == 0) {
        value = DataTransformationErrorHandlingEnum::kNoTransformerErrorHandling;
        return true;
    } else if (std::strcmp(string.c_str(), "TRANSFORMER-ERROR-HANDLING") == 0) {
        value = DataTransformationErrorHandlingEnum::kTransformerErrorHandling;
        return true;
    } else
        return false;
}

inline const char* ToString(DataTransformationErrorHandlingEnum value)
{
    switch (value) {
        case DataTransformationErrorHandlingEnum::kNoTransformerErrorHandling:
            return "NO-TRANSFORMER-ERROR-HANDLING";
        case DataTransformationErrorHandlingEnum::kTransformerErrorHandling:
            return "TRANSFORMER-ERROR-HANDLING";
        default:
            return nullptr;
    }
}

enum class DataTransformationKindEnum
{
    kAsymmetricFromByteArray = 0,  // The DataTransformation shall only be applied to the receiving end only, i.e.
                                   // transform from byte array to data type.
    kAsymmetricToByteArray
    = 1,  // The DataTransformation shall be applied to the sending end only, i.e. from data type to byte array.
    kSymmetric
    = 2,  // The DataTransformation shall be applied at both the sending and the receiving end of the communication.
};

template < typename StringType >
bool FromString(const StringType& string, DataTransformationKindEnum& value)
{
    if (std::strcmp(string.c_str(), "ASYMMETRIC-FROM-BYTE-ARRAY") == 0) {
        value = DataTransformationKindEnum::kAsymmetricFromByteArray;
        return true;
    } else if (std::strcmp(string.c_str(), "ASYMMETRIC-TO-BYTE-ARRAY") == 0) {
        value = DataTransformationKindEnum::kAsymmetricToByteArray;
        return true;
    } else if (std::strcmp(string.c_str(), "SYMMETRIC") == 0) {
        value = DataTransformationKindEnum::kSymmetric;
        return true;
    } else
        return false;
}

inline const char* ToString(DataTransformationKindEnum value)
{
    switch (value) {
        case DataTransformationKindEnum::kAsymmetricFromByteArray:
            return "ASYMMETRIC-FROM-BYTE-ARRAY";
        case DataTransformationKindEnum::kAsymmetricToByteArray:
            return "ASYMMETRIC-TO-BYTE-ARRAY";
        case DataTransformationKindEnum::kSymmetric:
            return "SYMMETRIC";
        default:
            return nullptr;
    }
}

enum class DataTransformationStatusForwardingEnum
{
    kNoTransformerStatusForwarding = 0,  // The runnable is not able to forward a transformer status.
    kTransformerStatusForwarding   = 1,  // The runnable is able to forward a transformer status.
};

template < typename StringType >
bool FromString(const StringType& string, DataTransformationStatusForwardingEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-TRANSFORMER-STATUS-FORWARDING") == 0) {
        value = DataTransformationStatusForwardingEnum::kNoTransformerStatusForwarding;
        return true;
    } else if (std::strcmp(string.c_str(), "TRANSFORMER-STATUS-FORWARDING") == 0) {
        value = DataTransformationStatusForwardingEnum::kTransformerStatusForwarding;
        return true;
    } else
        return false;
}

inline const char* ToString(DataTransformationStatusForwardingEnum value)
{
    switch (value) {
        case DataTransformationStatusForwardingEnum::kNoTransformerStatusForwarding:
            return "NO-TRANSFORMER-STATUS-FORWARDING";
        case DataTransformationStatusForwardingEnum::kTransformerStatusForwarding:
            return "TRANSFORMER-STATUS-FORWARDING";
        default:
            return nullptr;
    }
}

enum class DataTypePolicyEnum
{
    kLegacy = 0,  // In case the System Description doesn't use a complete Software Component Description (VFB View)
                  // this value can be chosen. This supports the inclusion of legacy signals.   The aggregation of
                  // SwDataDefProps shall be used to configure the "ComSignalDataInvalidValue" and the Data Semantics.
    kNetworkRepresentationFromComSpec
    = 1,  // Ignore any networkRepresentationProps of this ISignal and use the networkRepresentation from the ComSpec.
          // Please note that the usage does not imply the existence of the SwDataDefProps in the role
          // networkRepresentation aggregated by the SenderComSpec or ReceiverComSpec if an ImplementationDataType is
          // defined.
    kOverride = 2,  // If this value is chosen the requirements specified in the ComSpec
                    // (networkRepresentationFromComSpec) are not fullfilled by the aggregated SwDataDefProps. In this
                    // case the networkRepresentation is specified by the aggregated swDataDefProps.
    kPortInterfaceDefinition
    = 3,  // This enumeration literal is deprecated and will be removed in future.  Old description: Ignore any
          // networkRepresentationProps of this ISignal and use the networkRepresentation specified in the
          // VariableDataPrototypes owned by PortInterface (portInterfaceDefinition).
    kTransformingISignal = 4,  // This literal indicates that a transformer chain shall be used to communicate the
                               // ISignal as UINT8_N over the bus.
};

template < typename StringType >
bool FromString(const StringType& string, DataTypePolicyEnum& value)
{
    if (std::strcmp(string.c_str(), "LEGACY") == 0) {
        value = DataTypePolicyEnum::kLegacy;
        return true;
    } else if (std::strcmp(string.c_str(), "NETWORK-REPRESENTATION-FROM-COM-SPEC") == 0) {
        value = DataTypePolicyEnum::kNetworkRepresentationFromComSpec;
        return true;
    } else if (std::strcmp(string.c_str(), "OVERRIDE") == 0) {
        value = DataTypePolicyEnum::kOverride;
        return true;
    } else if (std::strcmp(string.c_str(), "PORT-INTERFACE-DEFINITION") == 0) {
        value = DataTypePolicyEnum::kPortInterfaceDefinition;
        return true;
    } else if (std::strcmp(string.c_str(), "TRANSFORMING-I-SIGNAL") == 0) {
        value = DataTypePolicyEnum::kTransformingISignal;
        return true;
    } else
        return false;
}

inline const char* ToString(DataTypePolicyEnum value)
{
    switch (value) {
        case DataTypePolicyEnum::kLegacy:
            return "LEGACY";
        case DataTypePolicyEnum::kNetworkRepresentationFromComSpec:
            return "NETWORK-REPRESENTATION-FROM-COM-SPEC";
        case DataTypePolicyEnum::kOverride:
            return "OVERRIDE";
        case DataTypePolicyEnum::kPortInterfaceDefinition:
            return "PORT-INTERFACE-DEFINITION";
        case DataTypePolicyEnum::kTransformingISignal:
            return "TRANSFORMING-I-SIGNAL";
        default:
            return nullptr;
    }
}

enum class DefaultValueApplicationStrategyEnum
{
    kNoDefault               = 0,  // do not apply the AUTOSAR defined default value
    kDefaultIfRevisionUpdate = 1,  // If the AUTOSAR model is older than the Baseline of the Data Exchange Point and the
                                   // older version did not yet support the attribute, then the AUTOSAR defined default
                                   // value SHALL be applied before further validation or processing.
    kDefaultIfUndefined = 2,  // If the AUTOSAR model does not explicitly specify a value, then the apply the AUTOSAR
                              // defined default value  before further validation or processing.
};

template < typename StringType >
bool FromString(const StringType& string, DefaultValueApplicationStrategyEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-DEFAULT") == 0) {
        value = DefaultValueApplicationStrategyEnum::kNoDefault;
        return true;
    } else if (std::strcmp(string.c_str(), "DEFAULT-IF-REVISION-UPDATE") == 0) {
        value = DefaultValueApplicationStrategyEnum::kDefaultIfRevisionUpdate;
        return true;
    } else if (std::strcmp(string.c_str(), "DEFAULT-IF-UNDEFINED") == 0) {
        value = DefaultValueApplicationStrategyEnum::kDefaultIfUndefined;
        return true;
    } else
        return false;
}

inline const char* ToString(DefaultValueApplicationStrategyEnum value)
{
    switch (value) {
        case DefaultValueApplicationStrategyEnum::kNoDefault:
            return "NO-DEFAULT";
        case DefaultValueApplicationStrategyEnum::kDefaultIfRevisionUpdate:
            return "DEFAULT-IF-REVISION-UPDATE";
        case DefaultValueApplicationStrategyEnum::kDefaultIfUndefined:
            return "DEFAULT-IF-UNDEFINED";
        default:
            return nullptr;
    }
}

enum class DependencyUsageEnum
{
    kBuild          = 0,  // The object referred by the dependency is required during the build process.
    kCodegeneration = 1,  // The object referred by the dependency is required during code generation
    kCompile        = 2,  // The object referred by the dependency is required during compilation.
    kExecute        = 3,  // The object referred by the dependency is required at execution time.
    kLink           = 4,  // The object referred by the dependency is required during linking.
};

template < typename StringType >
bool FromString(const StringType& string, DependencyUsageEnum& value)
{
    if (std::strcmp(string.c_str(), "BUILD") == 0) {
        value = DependencyUsageEnum::kBuild;
        return true;
    } else if (std::strcmp(string.c_str(), "CODEGENERATION") == 0) {
        value = DependencyUsageEnum::kCodegeneration;
        return true;
    } else if (std::strcmp(string.c_str(), "COMPILE") == 0) {
        value = DependencyUsageEnum::kCompile;
        return true;
    } else if (std::strcmp(string.c_str(), "EXECUTE") == 0) {
        value = DependencyUsageEnum::kExecute;
        return true;
    } else if (std::strcmp(string.c_str(), "LINK") == 0) {
        value = DependencyUsageEnum::kLink;
        return true;
    } else
        return false;
}

inline const char* ToString(DependencyUsageEnum value)
{
    switch (value) {
        case DependencyUsageEnum::kBuild:
            return "BUILD";
        case DependencyUsageEnum::kCodegeneration:
            return "CODEGENERATION";
        case DependencyUsageEnum::kCompile:
            return "COMPILE";
        case DependencyUsageEnum::kExecute:
            return "EXECUTE";
        case DependencyUsageEnum::kLink:
            return "LINK";
        default:
            return nullptr;
    }
}

enum class DiagnosticAccessPermissionValidityEnum
{
    kAccesPerrmissionServiceClass
    = 0,  // This means that the DiagnosticServiceClass is in charge to define the accessPermission.
    kAccessPermissionInstanceOverridesClass
    = 1,  // This means that accessPermission set at the DiagnosticServiceInstance will override the accessPermission
          // defined at the DiagnosticServiceClass.
    kAccessPermissionServiceClass
    = 2,  // This means that the DiagnosticServiceClass is in charge to define the accessPermission.
    kAccessPermissionServiceInstance
    = 3,  // This means that the DiagnosticServiceInstance is in charge of defining the accessPermission
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticAccessPermissionValidityEnum& value)
{
    if (std::strcmp(string.c_str(), "ACCES-PERRMISSION-SERVICE-CLASS") == 0) {
        value = DiagnosticAccessPermissionValidityEnum::kAccesPerrmissionServiceClass;
        return true;
    } else if (std::strcmp(string.c_str(), "ACCESS-PERMISSION-INSTANCE-OVERRIDES-CLASS") == 0) {
        value = DiagnosticAccessPermissionValidityEnum::kAccessPermissionInstanceOverridesClass;
        return true;
    } else if (std::strcmp(string.c_str(), "ACCESS-PERMISSION-SERVICE-CLASS") == 0) {
        value = DiagnosticAccessPermissionValidityEnum::kAccessPermissionServiceClass;
        return true;
    } else if (std::strcmp(string.c_str(), "ACCESS-PERMISSION-SERVICE-INSTANCE") == 0) {
        value = DiagnosticAccessPermissionValidityEnum::kAccessPermissionServiceInstance;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticAccessPermissionValidityEnum value)
{
    switch (value) {
        case DiagnosticAccessPermissionValidityEnum::kAccesPerrmissionServiceClass:
            return "ACCES-PERRMISSION-SERVICE-CLASS";
        case DiagnosticAccessPermissionValidityEnum::kAccessPermissionInstanceOverridesClass:
            return "ACCESS-PERMISSION-INSTANCE-OVERRIDES-CLASS";
        case DiagnosticAccessPermissionValidityEnum::kAccessPermissionServiceClass:
            return "ACCESS-PERMISSION-SERVICE-CLASS";
        case DiagnosticAccessPermissionValidityEnum::kAccessPermissionServiceInstance:
            return "ACCESS-PERMISSION-SERVICE-INSTANCE";
        default:
            return nullptr;
    }
}

enum class DiagnosticAudienceEnum
{
    kAftermaket    = 0,  // The object is for free aftermarket service organizations.
    kAftermarket   = 1,  // The object is for free aftermarket service organizations.
    kAfterSales    = 2,  // The object is relevant for the OEM after-sales organization.
    kDevelopment   = 3,  // The object is relevant for engineering only.
    kManufacturing = 4,  // The object is relevant for manufacturing.
    kSupplier      = 5,  // The object is relevant for the ECU-supplier aftermarket organization.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticAudienceEnum& value)
{
    if (std::strcmp(string.c_str(), "AFTERMAKET") == 0) {
        value = DiagnosticAudienceEnum::kAftermaket;
        return true;
    } else if (std::strcmp(string.c_str(), "AFTERMARKET") == 0) {
        value = DiagnosticAudienceEnum::kAftermarket;
        return true;
    } else if (std::strcmp(string.c_str(), "AFTER-SALES") == 0) {
        value = DiagnosticAudienceEnum::kAfterSales;
        return true;
    } else if (std::strcmp(string.c_str(), "DEVELOPMENT") == 0) {
        value = DiagnosticAudienceEnum::kDevelopment;
        return true;
    } else if (std::strcmp(string.c_str(), "MANUFACTURING") == 0) {
        value = DiagnosticAudienceEnum::kManufacturing;
        return true;
    } else if (std::strcmp(string.c_str(), "SUPPLIER") == 0) {
        value = DiagnosticAudienceEnum::kSupplier;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticAudienceEnum value)
{
    switch (value) {
        case DiagnosticAudienceEnum::kAftermaket:
            return "AFTERMAKET";
        case DiagnosticAudienceEnum::kAftermarket:
            return "AFTERMARKET";
        case DiagnosticAudienceEnum::kAfterSales:
            return "AFTER-SALES";
        case DiagnosticAudienceEnum::kDevelopment:
            return "DEVELOPMENT";
        case DiagnosticAudienceEnum::kManufacturing:
            return "MANUFACTURING";
        case DiagnosticAudienceEnum::kSupplier:
            return "SUPPLIER";
        default:
            return nullptr;
    }
}

enum class DiagnosticClearDtcLimitationEnum
{
    kAllSupportedDtcs = 0,  // DEM_ClearDtc API accepts all supported DTC values.
    kClearAllDtcs     = 1,  // DEM_ClearDtc API accepts ClearAllDTCs only.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticClearDtcLimitationEnum& value)
{
    if (std::strcmp(string.c_str(), "ALL-SUPPORTED-DTCS") == 0) {
        value = DiagnosticClearDtcLimitationEnum::kAllSupportedDtcs;
        return true;
    } else if (std::strcmp(string.c_str(), "CLEAR-ALL-DTCS") == 0) {
        value = DiagnosticClearDtcLimitationEnum::kClearAllDtcs;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticClearDtcLimitationEnum value)
{
    switch (value) {
        case DiagnosticClearDtcLimitationEnum::kAllSupportedDtcs:
            return "ALL-SUPPORTED-DTCS";
        case DiagnosticClearDtcLimitationEnum::kClearAllDtcs:
            return "CLEAR-ALL-DTCS";
        default:
            return nullptr;
    }
}

enum class DiagnosticClearEventAllowedBehaviorEnum
{
    kNoStatusByteChange        = 0,  // The event status byte keeps unchanged.
    kOnlyThisCycleAndReadiness = 1,  // The OperationCycle and readiness bits of the event status byte are reset.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticClearEventAllowedBehaviorEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-STATUS-BYTE-CHANGE") == 0) {
        value = DiagnosticClearEventAllowedBehaviorEnum::kNoStatusByteChange;
        return true;
    } else if (std::strcmp(string.c_str(), "ONLY-THIS-CYCLE-AND-READINESS") == 0) {
        value = DiagnosticClearEventAllowedBehaviorEnum::kOnlyThisCycleAndReadiness;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticClearEventAllowedBehaviorEnum value)
{
    switch (value) {
        case DiagnosticClearEventAllowedBehaviorEnum::kNoStatusByteChange:
            return "NO-STATUS-BYTE-CHANGE";
        case DiagnosticClearEventAllowedBehaviorEnum::kOnlyThisCycleAndReadiness:
            return "ONLY-THIS-CYCLE-AND-READINESS";
        default:
            return nullptr;
    }
}

enum class DiagnosticClearEventBehaviorEnum
{
    kNoStatusByteChange        = 0,  // The event status byte keeps unchanged.
    kOnlyThisCycleAndReadiness = 1,  // The OperationCycle and readiness bits of the event status byte are reset.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticClearEventBehaviorEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-STATUS-BYTE-CHANGE") == 0) {
        value = DiagnosticClearEventBehaviorEnum::kNoStatusByteChange;
        return true;
    } else if (std::strcmp(string.c_str(), "ONLY-THIS-CYCLE-AND-READINESS") == 0) {
        value = DiagnosticClearEventBehaviorEnum::kOnlyThisCycleAndReadiness;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticClearEventBehaviorEnum value)
{
    switch (value) {
        case DiagnosticClearEventBehaviorEnum::kNoStatusByteChange:
            return "NO-STATUS-BYTE-CHANGE";
        case DiagnosticClearEventBehaviorEnum::kOnlyThisCycleAndReadiness:
            return "ONLY-THIS-CYCLE-AND-READINESS";
        default:
            return nullptr;
    }
}

enum class DiagnosticCompareTypeEnum
{
    kIsEqual          = 0,  // equal
    kIsNotEqual       = 1,  // not equal
    kIsLessThan       = 2,  // less than
    kIsLessOrEqual    = 3,  // less than or equal
    kIsGreaterThan    = 4,  // greater than
    kIsGreaterOrEqual = 5,  // greater than or equal
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticCompareTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "IS-EQUAL") == 0) {
        value = DiagnosticCompareTypeEnum::kIsEqual;
        return true;
    } else if (std::strcmp(string.c_str(), "IS-NOT-EQUAL") == 0) {
        value = DiagnosticCompareTypeEnum::kIsNotEqual;
        return true;
    } else if (std::strcmp(string.c_str(), "IS-LESS-THAN") == 0) {
        value = DiagnosticCompareTypeEnum::kIsLessThan;
        return true;
    } else if (std::strcmp(string.c_str(), "IS-LESS-OR-EQUAL") == 0) {
        value = DiagnosticCompareTypeEnum::kIsLessOrEqual;
        return true;
    } else if (std::strcmp(string.c_str(), "IS-GREATER-THAN") == 0) {
        value = DiagnosticCompareTypeEnum::kIsGreaterThan;
        return true;
    } else if (std::strcmp(string.c_str(), "IS-GREATER-OR-EQUAL") == 0) {
        value = DiagnosticCompareTypeEnum::kIsGreaterOrEqual;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticCompareTypeEnum value)
{
    switch (value) {
        case DiagnosticCompareTypeEnum::kIsEqual:
            return "IS-EQUAL";
        case DiagnosticCompareTypeEnum::kIsNotEqual:
            return "IS-NOT-EQUAL";
        case DiagnosticCompareTypeEnum::kIsLessThan:
            return "IS-LESS-THAN";
        case DiagnosticCompareTypeEnum::kIsLessOrEqual:
            return "IS-LESS-OR-EQUAL";
        case DiagnosticCompareTypeEnum::kIsGreaterThan:
            return "IS-GREATER-THAN";
        case DiagnosticCompareTypeEnum::kIsGreaterOrEqual:
            return "IS-GREATER-OR-EQUAL";
        default:
            return nullptr;
    }
}

enum class DiagnosticConnectedIndicatorBehaviorEnum
{
    kBlinkMode               = 0,  // The indicator blinks when the event has status FAILED.
    kBlinkOrContinuousOnMode = 1,  // The indicator is active and blinks when the event has status FAILED.
    kContinuousOnMode        = 2,  // The indicator is active when the event has status FAILED.
    kFastFlashingMode        = 3,  // Flash Indicator Lamp should be set to "Fast Flash".
    kSlowFlashingMode        = 4,  // Flash Indicator Lamp should be set to "Slow Flash".
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticConnectedIndicatorBehaviorEnum& value)
{
    if (std::strcmp(string.c_str(), "BLINK-MODE") == 0) {
        value = DiagnosticConnectedIndicatorBehaviorEnum::kBlinkMode;
        return true;
    } else if (std::strcmp(string.c_str(), "BLINK-OR-CONTINUOUS-ON-MODE") == 0) {
        value = DiagnosticConnectedIndicatorBehaviorEnum::kBlinkOrContinuousOnMode;
        return true;
    } else if (std::strcmp(string.c_str(), "CONTINUOUS-ON-MODE") == 0) {
        value = DiagnosticConnectedIndicatorBehaviorEnum::kContinuousOnMode;
        return true;
    } else if (std::strcmp(string.c_str(), "FAST-FLASHING-MODE") == 0) {
        value = DiagnosticConnectedIndicatorBehaviorEnum::kFastFlashingMode;
        return true;
    } else if (std::strcmp(string.c_str(), "SLOW-FLASHING-MODE") == 0) {
        value = DiagnosticConnectedIndicatorBehaviorEnum::kSlowFlashingMode;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticConnectedIndicatorBehaviorEnum value)
{
    switch (value) {
        case DiagnosticConnectedIndicatorBehaviorEnum::kBlinkMode:
            return "BLINK-MODE";
        case DiagnosticConnectedIndicatorBehaviorEnum::kBlinkOrContinuousOnMode:
            return "BLINK-OR-CONTINUOUS-ON-MODE";
        case DiagnosticConnectedIndicatorBehaviorEnum::kContinuousOnMode:
            return "CONTINUOUS-ON-MODE";
        case DiagnosticConnectedIndicatorBehaviorEnum::kFastFlashingMode:
            return "FAST-FLASHING-MODE";
        case DiagnosticConnectedIndicatorBehaviorEnum::kSlowFlashingMode:
            return "SLOW-FLASHING-MODE";
        default:
            return nullptr;
    }
}

enum class DiagnosticDataCaptureEnum
{
    kCaptureAsynchronouslyToReporting = 0,  // This represents the intention to capture the environment data
                                            // asynchronously after the actual capture API function terminated.
    kCaptureSynchronouslyToReporting = 1,   // This represents the intention to capture the environment data
                                            // synchronously within the capture API function.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticDataCaptureEnum& value)
{
    if (std::strcmp(string.c_str(), "CAPTURE-ASYNCHRONOUSLY-TO-REPORTING") == 0) {
        value = DiagnosticDataCaptureEnum::kCaptureAsynchronouslyToReporting;
        return true;
    } else if (std::strcmp(string.c_str(), "CAPTURE-SYNCHRONOUSLY-TO-REPORTING") == 0) {
        value = DiagnosticDataCaptureEnum::kCaptureSynchronouslyToReporting;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticDataCaptureEnum value)
{
    switch (value) {
        case DiagnosticDataCaptureEnum::kCaptureAsynchronouslyToReporting:
            return "CAPTURE-ASYNCHRONOUSLY-TO-REPORTING";
        case DiagnosticDataCaptureEnum::kCaptureSynchronouslyToReporting:
            return "CAPTURE-SYNCHRONOUSLY-TO-REPORTING";
        default:
            return nullptr;
    }
}

enum class DiagnosticDebounceBehaviorEnum
{
    kFreeze
    = 0,  // The event debounce counter will be frozen with the current value and will not change while a related
          // enable condition is not fulfilled or ControlDTCSetting of the related event is disabled. After all
          // related enable conditions are fulfilled and ControlDTCSetting of the related event is enabled again, the
          // event qualification will continue with the next report of the event (i.e. SetEventStatus).
    kReset = 1,  // The event debounce counter will be reset to initial value if a related enable condition is not
                 // fulfilled or ControlDTCSetting of the related event is disabled. The qualification of the event will
                 // be restarted with the next valid event report.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticDebounceBehaviorEnum& value)
{
    if (std::strcmp(string.c_str(), "FREEZE") == 0) {
        value = DiagnosticDebounceBehaviorEnum::kFreeze;
        return true;
    } else if (std::strcmp(string.c_str(), "RESET") == 0) {
        value = DiagnosticDebounceBehaviorEnum::kReset;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticDebounceBehaviorEnum value)
{
    switch (value) {
        case DiagnosticDebounceBehaviorEnum::kFreeze:
            return "FREEZE";
        case DiagnosticDebounceBehaviorEnum::kReset:
            return "RESET";
        default:
            return nullptr;
    }
}

enum class DiagnosticDenominatorConditionEnum
{
    kColdstart  = 0,  // Condition based on definition of "cold start" as defined for EU5+
    kEvap       = 1,  // Condition based on definition of "EVAP" conditions as defined for OBD2.
    k500Miles   = 2,  // Condition based on definition of 500miles conditions as defined for OBD2.
    kIndividual = 3,  // condition based on definition of individual requirements.
    kObd        = 4,  // Condition based on definition of OBD requirements.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticDenominatorConditionEnum& value)
{
    if (std::strcmp(string.c_str(), "COLDSTART") == 0) {
        value = DiagnosticDenominatorConditionEnum::kColdstart;
        return true;
    } else if (std::strcmp(string.c_str(), "EVAP") == 0) {
        value = DiagnosticDenominatorConditionEnum::kEvap;
        return true;
    } else if (std::strcmp(string.c_str(), "-500-MILES") == 0) {
        value = DiagnosticDenominatorConditionEnum::k500Miles;
        return true;
    } else if (std::strcmp(string.c_str(), "INDIVIDUAL") == 0) {
        value = DiagnosticDenominatorConditionEnum::kIndividual;
        return true;
    } else if (std::strcmp(string.c_str(), "OBD") == 0) {
        value = DiagnosticDenominatorConditionEnum::kObd;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticDenominatorConditionEnum value)
{
    switch (value) {
        case DiagnosticDenominatorConditionEnum::kColdstart:
            return "COLDSTART";
        case DiagnosticDenominatorConditionEnum::kEvap:
            return "EVAP";
        case DiagnosticDenominatorConditionEnum::k500Miles:
            return "-500-MILES";
        case DiagnosticDenominatorConditionEnum::kIndividual:
            return "INDIVIDUAL";
        case DiagnosticDenominatorConditionEnum::kObd:
            return "OBD";
        default:
            return nullptr;
    }
}

enum class DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum
{
    kClearDynamicallyDefineDataIdentifier = 0,  // Clear the specified dynamic data identifier.
    kDefineByIdentifier
    = 1,  // The definition of dynamic data identifier shall be done via a reference to a diagnostic data identifier.
    kDefineByMemoryAddress
    = 2,  // The definition of dynamic data identifier shall be done via a reference to a memory address.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum& value)
{
    if (std::strcmp(string.c_str(), "CLEAR-DYNAMICALLY-DEFINE-DATA-IDENTIFIER") == 0) {
        value = DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kClearDynamicallyDefineDataIdentifier;
        return true;
    } else if (std::strcmp(string.c_str(), "DEFINE-BY-IDENTIFIER") == 0) {
        value = DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kDefineByIdentifier;
        return true;
    } else if (std::strcmp(string.c_str(), "DEFINE-BY-MEMORY-ADDRESS") == 0) {
        value = DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kDefineByMemoryAddress;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum value)
{
    switch (value) {
        case DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kClearDynamicallyDefineDataIdentifier:
            return "CLEAR-DYNAMICALLY-DEFINE-DATA-IDENTIFIER";
        case DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kDefineByIdentifier:
            return "DEFINE-BY-IDENTIFIER";
        case DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kDefineByMemoryAddress:
            return "DEFINE-BY-MEMORY-ADDRESS";
        default:
            return nullptr;
    }
}

enum class DiagnosticEventClearAllowedEnum
{
    kAlways                    = 0,  // The clearing is allowed unconditionally.
    kNever                     = 1,  // The clearing is never allowed.
    kRequiresCallbackExecution = 2,  // In case the clearing of a Diagnostic Event has to be allowed or prohibited
                                     // through the SWC interface CallbackClearEventAllowed, the SWC has to indicate
                                     // this by defining appropriate ServiceNeeds (i.e. DiagnosticEventNeeds).
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticEventClearAllowedEnum& value)
{
    if (std::strcmp(string.c_str(), "ALWAYS") == 0) {
        value = DiagnosticEventClearAllowedEnum::kAlways;
        return true;
    } else if (std::strcmp(string.c_str(), "NEVER") == 0) {
        value = DiagnosticEventClearAllowedEnum::kNever;
        return true;
    } else if (std::strcmp(string.c_str(), "REQUIRES-CALLBACK-EXECUTION") == 0) {
        value = DiagnosticEventClearAllowedEnum::kRequiresCallbackExecution;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticEventClearAllowedEnum value)
{
    switch (value) {
        case DiagnosticEventClearAllowedEnum::kAlways:
            return "ALWAYS";
        case DiagnosticEventClearAllowedEnum::kNever:
            return "NEVER";
        case DiagnosticEventClearAllowedEnum::kRequiresCallbackExecution:
            return "REQUIRES-CALLBACK-EXECUTION";
        default:
            return nullptr;
    }
}

enum class DiagnosticEventDisplacementStrategyEnum
{
    kFull = 0,  // Event memory entry displacement is enabled, by consideration of priority active/passive status, and
                // occurrence.
    kNone    = 1,  // Event memory entry displacement is disabled.
    kPrioOcc = 2,  // Event memory entry displacement is enabled, by consideration of priority and occurrence (but
                   // without active/passive status).
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticEventDisplacementStrategyEnum& value)
{
    if (std::strcmp(string.c_str(), "FULL") == 0) {
        value = DiagnosticEventDisplacementStrategyEnum::kFull;
        return true;
    } else if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = DiagnosticEventDisplacementStrategyEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "PRIO-OCC") == 0) {
        value = DiagnosticEventDisplacementStrategyEnum::kPrioOcc;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticEventDisplacementStrategyEnum value)
{
    switch (value) {
        case DiagnosticEventDisplacementStrategyEnum::kFull:
            return "FULL";
        case DiagnosticEventDisplacementStrategyEnum::kNone:
            return "NONE";
        case DiagnosticEventDisplacementStrategyEnum::kPrioOcc:
            return "PRIO-OCC";
        default:
            return nullptr;
    }
}

enum class DiagnosticEventKindEnum
{
    kBsw = 0,  // The event is assigned to a BSW module.
    kSwc = 1,  // The event is assigned to a SWC.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticEventKindEnum& value)
{
    if (std::strcmp(string.c_str(), "BSW") == 0) {
        value = DiagnosticEventKindEnum::kBsw;
        return true;
    } else if (std::strcmp(string.c_str(), "SWC") == 0) {
        value = DiagnosticEventKindEnum::kSwc;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticEventKindEnum value)
{
    switch (value) {
        case DiagnosticEventKindEnum::kBsw:
            return "BSW";
        case DiagnosticEventKindEnum::kSwc:
            return "SWC";
        default:
            return nullptr;
    }
}

enum class DiagnosticEventWindowTimeEnum
{
    kEventWindowCurrentAndFollowingCycle = 0,  // This means that the window extends to this and the following cycle.
    kEventWindowCurrentCycle             = 1,  // This means that the window is limited to the current cycle.
    kEventWindowInfinite                 = 2,  // This means that the window extents without a border.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticEventWindowTimeEnum& value)
{
    if (std::strcmp(string.c_str(), "EVENT-WINDOW-CURRENT-AND-FOLLOWING-CYCLE") == 0) {
        value = DiagnosticEventWindowTimeEnum::kEventWindowCurrentAndFollowingCycle;
        return true;
    } else if (std::strcmp(string.c_str(), "EVENT-WINDOW-CURRENT-CYCLE") == 0) {
        value = DiagnosticEventWindowTimeEnum::kEventWindowCurrentCycle;
        return true;
    } else if (std::strcmp(string.c_str(), "EVENT-WINDOW-INFINITE") == 0) {
        value = DiagnosticEventWindowTimeEnum::kEventWindowInfinite;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticEventWindowTimeEnum value)
{
    switch (value) {
        case DiagnosticEventWindowTimeEnum::kEventWindowCurrentAndFollowingCycle:
            return "EVENT-WINDOW-CURRENT-AND-FOLLOWING-CYCLE";
        case DiagnosticEventWindowTimeEnum::kEventWindowCurrentCycle:
            return "EVENT-WINDOW-CURRENT-CYCLE";
        case DiagnosticEventWindowTimeEnum::kEventWindowInfinite:
            return "EVENT-WINDOW-INFINITE";
        default:
            return nullptr;
    }
}

enum class DiagnosticHandleDddiConfigurationEnum
{
    kNonVolatile = 0,  // This indicates that the configuration of DynamicallyDefineDataIdentifier shall be stored as
                       // non-volatile data.
    kVolatile = 1,     // This indicates that the configuration of DynamicallyDefineDataIdentifier shall be handled as
                       // volatile data.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticHandleDddiConfigurationEnum& value)
{
    if (std::strcmp(string.c_str(), "NON-VOLATILE") == 0) {
        value = DiagnosticHandleDddiConfigurationEnum::kNonVolatile;
        return true;
    } else if (std::strcmp(string.c_str(), "VOLATILE") == 0) {
        value = DiagnosticHandleDddiConfigurationEnum::kVolatile;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticHandleDddiConfigurationEnum value)
{
    switch (value) {
        case DiagnosticHandleDddiConfigurationEnum::kNonVolatile:
            return "NON-VOLATILE";
        case DiagnosticHandleDddiConfigurationEnum::kVolatile:
            return "VOLATILE";
        default:
            return nullptr;
    }
}

enum class DiagnosticIndicatorTypeEnum
{
    kAmberWarning = 0,  // Amber Warning Lamp
    kMalfunction  = 1,  // Malfunction Indicator Lamp
    kProtectLamp  = 2,  // Protect Lamp
    kRedStopLamp  = 3,  // Red Stop Lamp
    kWarning      = 4,  // Warning
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticIndicatorTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "AMBER-WARNING") == 0) {
        value = DiagnosticIndicatorTypeEnum::kAmberWarning;
        return true;
    } else if (std::strcmp(string.c_str(), "MALFUNCTION") == 0) {
        value = DiagnosticIndicatorTypeEnum::kMalfunction;
        return true;
    } else if (std::strcmp(string.c_str(), "PROTECT-LAMP") == 0) {
        value = DiagnosticIndicatorTypeEnum::kProtectLamp;
        return true;
    } else if (std::strcmp(string.c_str(), "RED-STOP-LAMP") == 0) {
        value = DiagnosticIndicatorTypeEnum::kRedStopLamp;
        return true;
    } else if (std::strcmp(string.c_str(), "WARNING") == 0) {
        value = DiagnosticIndicatorTypeEnum::kWarning;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticIndicatorTypeEnum value)
{
    switch (value) {
        case DiagnosticIndicatorTypeEnum::kAmberWarning:
            return "AMBER-WARNING";
        case DiagnosticIndicatorTypeEnum::kMalfunction:
            return "MALFUNCTION";
        case DiagnosticIndicatorTypeEnum::kProtectLamp:
            return "PROTECT-LAMP";
        case DiagnosticIndicatorTypeEnum::kRedStopLamp:
            return "RED-STOP-LAMP";
        case DiagnosticIndicatorTypeEnum::kWarning:
            return "WARNING";
        default:
            return nullptr;
    }
}

enum class DiagnosticInhibitionMaskEnum
{
    kLastFailed      = 0,  // This represents the inhibition mask behavior "last failed".
    kNotTested       = 1,  // This represents the inhibition mask behavior "not tested".
    kTestedAndFailed = 2,  // This represents the inhibition mask behavior "tested and failed".
    kTested          = 3,  // This represents the inhibition mask behavior "tested".
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticInhibitionMaskEnum& value)
{
    if (std::strcmp(string.c_str(), "LAST-FAILED") == 0) {
        value = DiagnosticInhibitionMaskEnum::kLastFailed;
        return true;
    } else if (std::strcmp(string.c_str(), "NOT-TESTED") == 0) {
        value = DiagnosticInhibitionMaskEnum::kNotTested;
        return true;
    } else if (std::strcmp(string.c_str(), "TESTED-AND-FAILED") == 0) {
        value = DiagnosticInhibitionMaskEnum::kTestedAndFailed;
        return true;
    } else if (std::strcmp(string.c_str(), "TESTED") == 0) {
        value = DiagnosticInhibitionMaskEnum::kTested;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticInhibitionMaskEnum value)
{
    switch (value) {
        case DiagnosticInhibitionMaskEnum::kLastFailed:
            return "LAST-FAILED";
        case DiagnosticInhibitionMaskEnum::kNotTested:
            return "NOT-TESTED";
        case DiagnosticInhibitionMaskEnum::kTestedAndFailed:
            return "TESTED-AND-FAILED";
        case DiagnosticInhibitionMaskEnum::kTested:
            return "TESTED";
        default:
            return nullptr;
    }
}

enum class DiagnosticInitialEventStatusEnum
{
    kReturnOnEventCleared = 0,  // This means that the ResponseOnEvent is initially cleared.
    kReturnOnEventStopped = 1,  // This means that the ResponseOnEvent is initially stopped.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticInitialEventStatusEnum& value)
{
    if (std::strcmp(string.c_str(), "RETURN-ON-EVENT-CLEARED") == 0) {
        value = DiagnosticInitialEventStatusEnum::kReturnOnEventCleared;
        return true;
    } else if (std::strcmp(string.c_str(), "RETURN-ON-EVENT-STOPPED") == 0) {
        value = DiagnosticInitialEventStatusEnum::kReturnOnEventStopped;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticInitialEventStatusEnum value)
{
    switch (value) {
        case DiagnosticInitialEventStatusEnum::kReturnOnEventCleared:
            return "RETURN-ON-EVENT-CLEARED";
        case DiagnosticInitialEventStatusEnum::kReturnOnEventStopped:
            return "RETURN-ON-EVENT-STOPPED";
        default:
            return nullptr;
    }
}

enum class DiagnosticJumpToBootLoaderEnum
{
    kNoBoot  = 0,  // This diagnostic session doesn't allow to jump to Bootloader.
    kOemBoot = 1,  // This diagnostic session allows to jump to OEM Bootloader. In this case the bootloader send the
                   // final response.
    kSystemSupplierBoot = 2,  // This diagnostic session allows to jump to System Supplier Bootloader.  In this case the
                              // bootloader send the final response.
    kOemBootRespApp
    = 3,  // This diagnostic session allows to jump to OEM Bootloader and application sends final response.
    kSystemSupplierBootRespApp = 4,  // This diagnostic session allows to jump to System Supplier Bootloader and
                                     // application sends final response.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticJumpToBootLoaderEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-BOOT") == 0) {
        value = DiagnosticJumpToBootLoaderEnum::kNoBoot;
        return true;
    } else if (std::strcmp(string.c_str(), "OEM-BOOT") == 0) {
        value = DiagnosticJumpToBootLoaderEnum::kOemBoot;
        return true;
    } else if (std::strcmp(string.c_str(), "SYSTEM-SUPPLIER-BOOT") == 0) {
        value = DiagnosticJumpToBootLoaderEnum::kSystemSupplierBoot;
        return true;
    } else if (std::strcmp(string.c_str(), "OEM-BOOT-RESP-APP") == 0) {
        value = DiagnosticJumpToBootLoaderEnum::kOemBootRespApp;
        return true;
    } else if (std::strcmp(string.c_str(), "SYSTEM-SUPPLIER-BOOT-RESP-APP") == 0) {
        value = DiagnosticJumpToBootLoaderEnum::kSystemSupplierBootRespApp;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticJumpToBootLoaderEnum value)
{
    switch (value) {
        case DiagnosticJumpToBootLoaderEnum::kNoBoot:
            return "NO-BOOT";
        case DiagnosticJumpToBootLoaderEnum::kOemBoot:
            return "OEM-BOOT";
        case DiagnosticJumpToBootLoaderEnum::kSystemSupplierBoot:
            return "SYSTEM-SUPPLIER-BOOT";
        case DiagnosticJumpToBootLoaderEnum::kOemBootRespApp:
            return "OEM-BOOT-RESP-APP";
        case DiagnosticJumpToBootLoaderEnum::kSystemSupplierBootRespApp:
            return "SYSTEM-SUPPLIER-BOOT-RESP-APP";
        default:
            return nullptr;
    }
}

enum class DiagnosticLogicalOperatorEnum
{
    kLogicalAnd = 0,  // Logical AND
    kLogicalOr  = 1,  // Logical OR
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticLogicalOperatorEnum& value)
{
    if (std::strcmp(string.c_str(), "LOGICAL-AND") == 0) {
        value = DiagnosticLogicalOperatorEnum::kLogicalAnd;
        return true;
    } else if (std::strcmp(string.c_str(), "LOGICAL-OR") == 0) {
        value = DiagnosticLogicalOperatorEnum::kLogicalOr;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticLogicalOperatorEnum value)
{
    switch (value) {
        case DiagnosticLogicalOperatorEnum::kLogicalAnd:
            return "LOGICAL-AND";
        case DiagnosticLogicalOperatorEnum::kLogicalOr:
            return "LOGICAL-OR";
        default:
            return nullptr;
    }
}

enum class DiagnosticMemoryEntryStorageTriggerEnum
{
    kConfirmed    = 0,  // Status information of UDS DTC status bit 3
    kFdcThreshold = 1,  // Threshold to allocate an event memory entry and to capture the Freeze Frame.
    kPending      = 2,  // Status information of UDS DTC status bit 2.
    kTestFailed   = 3,  // Status information of UDS DTC status bit 0.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticMemoryEntryStorageTriggerEnum& value)
{
    if (std::strcmp(string.c_str(), "CONFIRMED") == 0) {
        value = DiagnosticMemoryEntryStorageTriggerEnum::kConfirmed;
        return true;
    } else if (std::strcmp(string.c_str(), "FDC-THRESHOLD") == 0) {
        value = DiagnosticMemoryEntryStorageTriggerEnum::kFdcThreshold;
        return true;
    } else if (std::strcmp(string.c_str(), "PENDING") == 0) {
        value = DiagnosticMemoryEntryStorageTriggerEnum::kPending;
        return true;
    } else if (std::strcmp(string.c_str(), "TEST-FAILED") == 0) {
        value = DiagnosticMemoryEntryStorageTriggerEnum::kTestFailed;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticMemoryEntryStorageTriggerEnum value)
{
    switch (value) {
        case DiagnosticMemoryEntryStorageTriggerEnum::kConfirmed:
            return "CONFIRMED";
        case DiagnosticMemoryEntryStorageTriggerEnum::kFdcThreshold:
            return "FDC-THRESHOLD";
        case DiagnosticMemoryEntryStorageTriggerEnum::kPending:
            return "PENDING";
        case DiagnosticMemoryEntryStorageTriggerEnum::kTestFailed:
            return "TEST-FAILED";
        default:
            return nullptr;
    }
}

enum class DiagnosticMonitorUpdateKindEnum
{
    kAlways = 0,  // Dem shall accept every update.
    kSteady = 1,  // Dem shall only accept if debouncing is at the limit.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticMonitorUpdateKindEnum& value)
{
    if (std::strcmp(string.c_str(), "ALWAYS") == 0) {
        value = DiagnosticMonitorUpdateKindEnum::kAlways;
        return true;
    } else if (std::strcmp(string.c_str(), "STEADY") == 0) {
        value = DiagnosticMonitorUpdateKindEnum::kSteady;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticMonitorUpdateKindEnum value)
{
    switch (value) {
        case DiagnosticMonitorUpdateKindEnum::kAlways:
            return "ALWAYS";
        case DiagnosticMonitorUpdateKindEnum::kSteady:
            return "STEADY";
        default:
            return nullptr;
    }
}

enum class DiagnosticObdSupportEnum
{
    kMasterEcu    = 0,  // This represent the role "master ECU".
    kNoObdSupport = 1,  // This represents the ability to explicitly specify that no participation in OBD is foreseen.
    kPrimaryEcu   = 2,  // This represents the role "primary ECU".
    kSecondaryEcu = 3,  // This represents the role "secondary ECU".
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticObdSupportEnum& value)
{
    if (std::strcmp(string.c_str(), "MASTER-ECU") == 0) {
        value = DiagnosticObdSupportEnum::kMasterEcu;
        return true;
    } else if (std::strcmp(string.c_str(), "NO-OBD-SUPPORT") == 0) {
        value = DiagnosticObdSupportEnum::kNoObdSupport;
        return true;
    } else if (std::strcmp(string.c_str(), "PRIMARY-ECU") == 0) {
        value = DiagnosticObdSupportEnum::kPrimaryEcu;
        return true;
    } else if (std::strcmp(string.c_str(), "SECONDARY-ECU") == 0) {
        value = DiagnosticObdSupportEnum::kSecondaryEcu;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticObdSupportEnum value)
{
    switch (value) {
        case DiagnosticObdSupportEnum::kMasterEcu:
            return "MASTER-ECU";
        case DiagnosticObdSupportEnum::kNoObdSupport:
            return "NO-OBD-SUPPORT";
        case DiagnosticObdSupportEnum::kPrimaryEcu:
            return "PRIMARY-ECU";
        case DiagnosticObdSupportEnum::kSecondaryEcu:
            return "SECONDARY-ECU";
        default:
            return nullptr;
    }
}

enum class DiagnosticOccurrenceCounterProcessingEnum
{
    kConfirmedDtcBit = 0,  // The occurrence counter is triggered by the TestFailed bit if the fault confirmation was
                           // successful (ConfirmedDTC bit is set).
    kTestFailedBit = 1,    // The occurrence counter is only triggered by the TestFailed bit (and the fault confirmation
                           // is not considered).
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticOccurrenceCounterProcessingEnum& value)
{
    if (std::strcmp(string.c_str(), "CONFIRMED-DTC-BIT") == 0) {
        value = DiagnosticOccurrenceCounterProcessingEnum::kConfirmedDtcBit;
        return true;
    } else if (std::strcmp(string.c_str(), "TEST-FAILED-BIT") == 0) {
        value = DiagnosticOccurrenceCounterProcessingEnum::kTestFailedBit;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticOccurrenceCounterProcessingEnum value)
{
    switch (value) {
        case DiagnosticOccurrenceCounterProcessingEnum::kConfirmedDtcBit:
            return "CONFIRMED-DTC-BIT";
        case DiagnosticOccurrenceCounterProcessingEnum::kTestFailedBit:
            return "TEST-FAILED-BIT";
        default:
            return nullptr;
    }
}

enum class DiagnosticOperationCycleTypeEnum
{
    kIgnition        = 0,  // Ignition ON / OFF cycle
    kObdDrivingCycle = 1,  // OBD Driving cycle
    kOther           = 2,  // further operation cycle
    kPower           = 3,  // Power ON / OFF cycle
    kTime            = 4,  // Time based operation cycle
    kWarmup          = 5,  // OBD Warm up cycle
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticOperationCycleTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "IGNITION") == 0) {
        value = DiagnosticOperationCycleTypeEnum::kIgnition;
        return true;
    } else if (std::strcmp(string.c_str(), "OBD-DRIVING-CYCLE") == 0) {
        value = DiagnosticOperationCycleTypeEnum::kObdDrivingCycle;
        return true;
    } else if (std::strcmp(string.c_str(), "OTHER") == 0) {
        value = DiagnosticOperationCycleTypeEnum::kOther;
        return true;
    } else if (std::strcmp(string.c_str(), "POWER") == 0) {
        value = DiagnosticOperationCycleTypeEnum::kPower;
        return true;
    } else if (std::strcmp(string.c_str(), "TIME") == 0) {
        value = DiagnosticOperationCycleTypeEnum::kTime;
        return true;
    } else if (std::strcmp(string.c_str(), "WARMUP") == 0) {
        value = DiagnosticOperationCycleTypeEnum::kWarmup;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticOperationCycleTypeEnum value)
{
    switch (value) {
        case DiagnosticOperationCycleTypeEnum::kIgnition:
            return "IGNITION";
        case DiagnosticOperationCycleTypeEnum::kObdDrivingCycle:
            return "OBD-DRIVING-CYCLE";
        case DiagnosticOperationCycleTypeEnum::kOther:
            return "OTHER";
        case DiagnosticOperationCycleTypeEnum::kPower:
            return "POWER";
        case DiagnosticOperationCycleTypeEnum::kTime:
            return "TIME";
        case DiagnosticOperationCycleTypeEnum::kWarmup:
            return "WARMUP";
        default:
            return nullptr;
    }
}

enum class DiagnosticPeriodicRateCategoryEnum
{
    kPeriodicRateFast   = 0,  // This value represents a fast periodic rate.
    kPeriodicRateMedium = 1,  // This value represents a medium periodic rate.
    kPeriodicRateSlow   = 2,  // This value represents a slow periodic rate.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticPeriodicRateCategoryEnum& value)
{
    if (std::strcmp(string.c_str(), "PERIODIC-RATE-FAST") == 0) {
        value = DiagnosticPeriodicRateCategoryEnum::kPeriodicRateFast;
        return true;
    } else if (std::strcmp(string.c_str(), "PERIODIC-RATE-MEDIUM") == 0) {
        value = DiagnosticPeriodicRateCategoryEnum::kPeriodicRateMedium;
        return true;
    } else if (std::strcmp(string.c_str(), "PERIODIC-RATE-SLOW") == 0) {
        value = DiagnosticPeriodicRateCategoryEnum::kPeriodicRateSlow;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticPeriodicRateCategoryEnum value)
{
    switch (value) {
        case DiagnosticPeriodicRateCategoryEnum::kPeriodicRateFast:
            return "PERIODIC-RATE-FAST";
        case DiagnosticPeriodicRateCategoryEnum::kPeriodicRateMedium:
            return "PERIODIC-RATE-MEDIUM";
        case DiagnosticPeriodicRateCategoryEnum::kPeriodicRateSlow:
            return "PERIODIC-RATE-SLOW";
        default:
            return nullptr;
    }
}

enum class DiagnosticProcessingStyleEnum
{
    kProcessingStyleAsynchronous = 0,  // The software-component processes the request in background but still the Dcm
                                       // has to issue the call again to eventually obtain the result of the request.
    kProcessingStyleAsynchronousWithError
    = 1,  // The software-component processes the request in background but still the Dcm has to issue the call again
          // to eventually obtain the result of the request or handle error code.
    kProcessingStyleSynchronous = 2,  // The software-component is supposed to react synchronously on the request.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticProcessingStyleEnum& value)
{
    if (std::strcmp(string.c_str(), "PROCESSING-STYLE-ASYNCHRONOUS") == 0) {
        value = DiagnosticProcessingStyleEnum::kProcessingStyleAsynchronous;
        return true;
    } else if (std::strcmp(string.c_str(), "PROCESSING-STYLE-ASYNCHRONOUS-WITH-ERROR") == 0) {
        value = DiagnosticProcessingStyleEnum::kProcessingStyleAsynchronousWithError;
        return true;
    } else if (std::strcmp(string.c_str(), "PROCESSING-STYLE-SYNCHRONOUS") == 0) {
        value = DiagnosticProcessingStyleEnum::kProcessingStyleSynchronous;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticProcessingStyleEnum value)
{
    switch (value) {
        case DiagnosticProcessingStyleEnum::kProcessingStyleAsynchronous:
            return "PROCESSING-STYLE-ASYNCHRONOUS";
        case DiagnosticProcessingStyleEnum::kProcessingStyleAsynchronousWithError:
            return "PROCESSING-STYLE-ASYNCHRONOUS-WITH-ERROR";
        case DiagnosticProcessingStyleEnum::kProcessingStyleSynchronous:
            return "PROCESSING-STYLE-SYNCHRONOUS";
        default:
            return nullptr;
    }
}

enum class DiagnosticRecordTriggerEnum
{
    kConfirmed                    = 0,  // capture on "Confirmed"
    kFdcThreshold                 = 1,  // capture on "FDC Threshold"
    kPending                      = 2,  // capture on "Pending"
    kTestFailed                   = 3,  // capture on "Test Failed"
    kCustom                       = 4,  // implement custom capture
    kTestFailedThisOperationCycle = 5,  // Test Failed This Operation Cycle.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticRecordTriggerEnum& value)
{
    if (std::strcmp(string.c_str(), "CONFIRMED") == 0) {
        value = DiagnosticRecordTriggerEnum::kConfirmed;
        return true;
    } else if (std::strcmp(string.c_str(), "FDC-THRESHOLD") == 0) {
        value = DiagnosticRecordTriggerEnum::kFdcThreshold;
        return true;
    } else if (std::strcmp(string.c_str(), "PENDING") == 0) {
        value = DiagnosticRecordTriggerEnum::kPending;
        return true;
    } else if (std::strcmp(string.c_str(), "TEST-FAILED") == 0) {
        value = DiagnosticRecordTriggerEnum::kTestFailed;
        return true;
    } else if (std::strcmp(string.c_str(), "CUSTOM") == 0) {
        value = DiagnosticRecordTriggerEnum::kCustom;
        return true;
    } else if (std::strcmp(string.c_str(), "TEST-FAILED-THIS-OPERATION-CYCLE") == 0) {
        value = DiagnosticRecordTriggerEnum::kTestFailedThisOperationCycle;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticRecordTriggerEnum value)
{
    switch (value) {
        case DiagnosticRecordTriggerEnum::kConfirmed:
            return "CONFIRMED";
        case DiagnosticRecordTriggerEnum::kFdcThreshold:
            return "FDC-THRESHOLD";
        case DiagnosticRecordTriggerEnum::kPending:
            return "PENDING";
        case DiagnosticRecordTriggerEnum::kTestFailed:
            return "TEST-FAILED";
        case DiagnosticRecordTriggerEnum::kCustom:
            return "CUSTOM";
        case DiagnosticRecordTriggerEnum::kTestFailedThisOperationCycle:
            return "TEST-FAILED-THIS-OPERATION-CYCLE";
        default:
            return nullptr;
    }
}

enum class DiagnosticResponseOnEventActionEnum
{
    kStop   = 0,  // Stops the response on event service.
    kStart  = 1,  // Starts the response on event service.
    kClear  = 2,  // Clears the configured events.
    kReport = 3,  // Reports the activated events.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticResponseOnEventActionEnum& value)
{
    if (std::strcmp(string.c_str(), "STOP") == 0) {
        value = DiagnosticResponseOnEventActionEnum::kStop;
        return true;
    } else if (std::strcmp(string.c_str(), "START") == 0) {
        value = DiagnosticResponseOnEventActionEnum::kStart;
        return true;
    } else if (std::strcmp(string.c_str(), "CLEAR") == 0) {
        value = DiagnosticResponseOnEventActionEnum::kClear;
        return true;
    } else if (std::strcmp(string.c_str(), "REPORT") == 0) {
        value = DiagnosticResponseOnEventActionEnum::kReport;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticResponseOnEventActionEnum value)
{
    switch (value) {
        case DiagnosticResponseOnEventActionEnum::kStop:
            return "STOP";
        case DiagnosticResponseOnEventActionEnum::kStart:
            return "START";
        case DiagnosticResponseOnEventActionEnum::kClear:
            return "CLEAR";
        case DiagnosticResponseOnEventActionEnum::kReport:
            return "REPORT";
        default:
            return nullptr;
    }
}

enum class DiagnosticResponseToEcuResetEnum
{
    kRespondAfterReset  = 0,  // Answer to EcuReset service should come after the reset.
    kRespondBeforeReset = 1,  // Answer to EcuReset service should come before the reset.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticResponseToEcuResetEnum& value)
{
    if (std::strcmp(string.c_str(), "RESPOND-AFTER-RESET") == 0) {
        value = DiagnosticResponseToEcuResetEnum::kRespondAfterReset;
        return true;
    } else if (std::strcmp(string.c_str(), "RESPOND-BEFORE-RESET") == 0) {
        value = DiagnosticResponseToEcuResetEnum::kRespondBeforeReset;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticResponseToEcuResetEnum value)
{
    switch (value) {
        case DiagnosticResponseToEcuResetEnum::kRespondAfterReset:
            return "RESPOND-AFTER-RESET";
        case DiagnosticResponseToEcuResetEnum::kRespondBeforeReset:
            return "RESPOND-BEFORE-RESET";
        default:
            return nullptr;
    }
}

enum class DiagnosticRoutineTypeEnum
{
    kAsynchronous
    = 0,  // This indicates that the diagnostic server is not blocked while the diagnostic routine is running.
    kSynchronous = 1,  // This indicates that the diagnostic routine blocks the diagnostic server in the ECU while the
                       // routine is running.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticRoutineTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "ASYNCHRONOUS") == 0) {
        value = DiagnosticRoutineTypeEnum::kAsynchronous;
        return true;
    } else if (std::strcmp(string.c_str(), "SYNCHRONOUS") == 0) {
        value = DiagnosticRoutineTypeEnum::kSynchronous;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticRoutineTypeEnum value)
{
    switch (value) {
        case DiagnosticRoutineTypeEnum::kAsynchronous:
            return "ASYNCHRONOUS";
        case DiagnosticRoutineTypeEnum::kSynchronous:
            return "SYNCHRONOUS";
        default:
            return nullptr;
    }
}

enum class DiagnosticServiceRequestCallbackTypeEnum
{
    kRequestCallbackTypeManufacturer
    = 0,  // This represents the case that the usage of PortInterface ServiceRequestNotification has the
          // characteristics of being used by a manufacturer.
    kRequestCallbackTypeSupplier
    = 1,  // This represents the case that the usage of PortInterface ServiceRequestNotification has the
          // characteristics of being used by a supplier.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticServiceRequestCallbackTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "REQUEST-CALLBACK-TYPE-MANUFACTURER") == 0) {
        value = DiagnosticServiceRequestCallbackTypeEnum::kRequestCallbackTypeManufacturer;
        return true;
    } else if (std::strcmp(string.c_str(), "REQUEST-CALLBACK-TYPE-SUPPLIER") == 0) {
        value = DiagnosticServiceRequestCallbackTypeEnum::kRequestCallbackTypeSupplier;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticServiceRequestCallbackTypeEnum value)
{
    switch (value) {
        case DiagnosticServiceRequestCallbackTypeEnum::kRequestCallbackTypeManufacturer:
            return "REQUEST-CALLBACK-TYPE-MANUFACTURER";
        case DiagnosticServiceRequestCallbackTypeEnum::kRequestCallbackTypeSupplier:
            return "REQUEST-CALLBACK-TYPE-SUPPLIER";
        default:
            return nullptr;
    }
}

enum class DiagnosticSignificanceEnum
{
    kFault     = 0,  // Failure, which affects the component/ECU itself.
    kOccurence = 1,  // Issue, which indicates additional information concerning insufficient system behavior.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticSignificanceEnum& value)
{
    if (std::strcmp(string.c_str(), "FAULT") == 0) {
        value = DiagnosticSignificanceEnum::kFault;
        return true;
    } else if (std::strcmp(string.c_str(), "OCCURENCE") == 0) {
        value = DiagnosticSignificanceEnum::kOccurence;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticSignificanceEnum value)
{
    switch (value) {
        case DiagnosticSignificanceEnum::kFault:
            return "FAULT";
        case DiagnosticSignificanceEnum::kOccurence:
            return "OCCURENCE";
        default:
            return nullptr;
    }
}

enum class DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum
{
    kStatusBitAgingAndDisplacement = 0,
    kStatusBitNormal               = 1,
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum& value)
{
    if (std::strcmp(string.c_str(), "STATUS-BIT-AGING-AND-DISPLACEMENT") == 0) {
        value = DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum::kStatusBitAgingAndDisplacement;
        return true;
    } else if (std::strcmp(string.c_str(), "STATUS-BIT-NORMAL") == 0) {
        value = DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum::kStatusBitNormal;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum value)
{
    switch (value) {
        case DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum::kStatusBitAgingAndDisplacement:
            return "STATUS-BIT-AGING-AND-DISPLACEMENT";
        case DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum::kStatusBitNormal:
            return "STATUS-BIT-NORMAL";
        default:
            return nullptr;
    }
}

enum class DiagnosticStoreEventSupportEnum
{
    kNoStoreEvent = 0,  // The event terminates when the server is powered down.
    kStoreEvent   = 1,  // The event is persisted over a power down cycle.
    kAll          = 2,  // The server supports both, storing and not storing the event.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticStoreEventSupportEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-STORE-EVENT") == 0) {
        value = DiagnosticStoreEventSupportEnum::kNoStoreEvent;
        return true;
    } else if (std::strcmp(string.c_str(), "STORE-EVENT") == 0) {
        value = DiagnosticStoreEventSupportEnum::kStoreEvent;
        return true;
    } else if (std::strcmp(string.c_str(), "ALL") == 0) {
        value = DiagnosticStoreEventSupportEnum::kAll;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticStoreEventSupportEnum value)
{
    switch (value) {
        case DiagnosticStoreEventSupportEnum::kNoStoreEvent:
            return "NO-STORE-EVENT";
        case DiagnosticStoreEventSupportEnum::kStoreEvent:
            return "STORE-EVENT";
        case DiagnosticStoreEventSupportEnum::kAll:
            return "ALL";
        default:
            return nullptr;
    }
}

enum class DiagnosticTestResultUpdateEnum
{
    kAlways = 0,  // Any DTR result reported by the monitor is used by the Dem.
    kSteady = 1,  // The Dem accepts reported DTRs only when the configured debouncing mechanism is stable at the FAIL
                  // or PASS limit.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticTestResultUpdateEnum& value)
{
    if (std::strcmp(string.c_str(), "ALWAYS") == 0) {
        value = DiagnosticTestResultUpdateEnum::kAlways;
        return true;
    } else if (std::strcmp(string.c_str(), "STEADY") == 0) {
        value = DiagnosticTestResultUpdateEnum::kSteady;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticTestResultUpdateEnum value)
{
    switch (value) {
        case DiagnosticTestResultUpdateEnum::kAlways:
            return "ALWAYS";
        case DiagnosticTestResultUpdateEnum::kSteady:
            return "STEADY";
        default:
            return nullptr;
    }
}

enum class DiagnosticTroubleCodeJ1939DtcKindEnum
{
    kServiceOnly = 0,  // this represents a DTC that is only relevant for service in a garage, reported by e.g. DM53.
    kStandard    = 1,  // This represents a non-specific DTC reported by e.g. DM1.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticTroubleCodeJ1939DtcKindEnum& value)
{
    if (std::strcmp(string.c_str(), "SERVICE-ONLY") == 0) {
        value = DiagnosticTroubleCodeJ1939DtcKindEnum::kServiceOnly;
        return true;
    } else if (std::strcmp(string.c_str(), "STANDARD") == 0) {
        value = DiagnosticTroubleCodeJ1939DtcKindEnum::kStandard;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticTroubleCodeJ1939DtcKindEnum value)
{
    switch (value) {
        case DiagnosticTroubleCodeJ1939DtcKindEnum::kServiceOnly:
            return "SERVICE-ONLY";
        case DiagnosticTroubleCodeJ1939DtcKindEnum::kStandard:
            return "STANDARD";
        default:
            return nullptr;
    }
}

enum class DiagnosticTypeOfDtcSupportedEnum
{
    kIso119924  = 0,  // ISO11992-4 DTC format
    kIso142291  = 1,  // ISO14229-1 DTC format (3 byte format)
    kIso150316  = 2,  // ISO15031-6 DTC format (2 byte format)
    kSaeJ193973 = 3,  // SAEJ1939-73 DTC format
    kSaeJ2012Da = 4,  // SAE_J2012-DA_DTCFormat_00 (3 byte format)
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticTypeOfDtcSupportedEnum& value)
{
    if (std::strcmp(string.c_str(), "ISO-11992--4") == 0) {
        value = DiagnosticTypeOfDtcSupportedEnum::kIso119924;
        return true;
    } else if (std::strcmp(string.c_str(), "ISO-14229--1") == 0) {
        value = DiagnosticTypeOfDtcSupportedEnum::kIso142291;
        return true;
    } else if (std::strcmp(string.c_str(), "ISO-15031--6") == 0) {
        value = DiagnosticTypeOfDtcSupportedEnum::kIso150316;
        return true;
    } else if (std::strcmp(string.c_str(), "SAE-J-1939--73") == 0) {
        value = DiagnosticTypeOfDtcSupportedEnum::kSaeJ193973;
        return true;
    } else if (std::strcmp(string.c_str(), "SAE-J-2012--DA") == 0) {
        value = DiagnosticTypeOfDtcSupportedEnum::kSaeJ2012Da;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticTypeOfDtcSupportedEnum value)
{
    switch (value) {
        case DiagnosticTypeOfDtcSupportedEnum::kIso119924:
            return "ISO-11992--4";
        case DiagnosticTypeOfDtcSupportedEnum::kIso142291:
            return "ISO-14229--1";
        case DiagnosticTypeOfDtcSupportedEnum::kIso150316:
            return "ISO-15031--6";
        case DiagnosticTypeOfDtcSupportedEnum::kSaeJ193973:
            return "SAE-J-1939--73";
        case DiagnosticTypeOfDtcSupportedEnum::kSaeJ2012Da:
            return "SAE-J-2012--DA";
        default:
            return nullptr;
    }
}

enum class DiagnosticTypeOfFreezeFrameRecordNumerationEnum
{
    kCalculated = 0,  // Freeze frame records will be numbered consecutive starting by 1 in their chronological order.
    kConfigured
    = 1,  // Freeze frame records will be numbered based on the given configuration in their chronological order.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticTypeOfFreezeFrameRecordNumerationEnum& value)
{
    if (std::strcmp(string.c_str(), "CALCULATED") == 0) {
        value = DiagnosticTypeOfFreezeFrameRecordNumerationEnum::kCalculated;
        return true;
    } else if (std::strcmp(string.c_str(), "CONFIGURED") == 0) {
        value = DiagnosticTypeOfFreezeFrameRecordNumerationEnum::kConfigured;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticTypeOfFreezeFrameRecordNumerationEnum value)
{
    switch (value) {
        case DiagnosticTypeOfFreezeFrameRecordNumerationEnum::kCalculated:
            return "CALCULATED";
        case DiagnosticTypeOfFreezeFrameRecordNumerationEnum::kConfigured:
            return "CONFIGURED";
        default:
            return nullptr;
    }
}

enum class DiagnosticUdsSeverityEnum
{
    kCheckAtNextHalt = 0,  // Check at next halt.
    kImmediately     = 1,  // Check immediately.
    kMaintenanceOnly = 2,  // Maintenance required.
    kNoSeverity      = 3,  // No severity information available.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticUdsSeverityEnum& value)
{
    if (std::strcmp(string.c_str(), "CHECK-AT-NEXT-HALT") == 0) {
        value = DiagnosticUdsSeverityEnum::kCheckAtNextHalt;
        return true;
    } else if (std::strcmp(string.c_str(), "IMMEDIATELY") == 0) {
        value = DiagnosticUdsSeverityEnum::kImmediately;
        return true;
    } else if (std::strcmp(string.c_str(), "MAINTENANCE-ONLY") == 0) {
        value = DiagnosticUdsSeverityEnum::kMaintenanceOnly;
        return true;
    } else if (std::strcmp(string.c_str(), "NO-SEVERITY") == 0) {
        value = DiagnosticUdsSeverityEnum::kNoSeverity;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticUdsSeverityEnum value)
{
    switch (value) {
        case DiagnosticUdsSeverityEnum::kCheckAtNextHalt:
            return "CHECK-AT-NEXT-HALT";
        case DiagnosticUdsSeverityEnum::kImmediately:
            return "IMMEDIATELY";
        case DiagnosticUdsSeverityEnum::kMaintenanceOnly:
            return "MAINTENANCE-ONLY";
        case DiagnosticUdsSeverityEnum::kNoSeverity:
            return "NO-SEVERITY";
        default:
            return nullptr;
    }
}

enum class DiagnosticValueAccessEnum
{
    kReadOnly = 0,   // The access to the data element is limited to read-only. This is typically used to read-out
                     // diagnostic information (e.g. current values).
    kReadWrite = 1,  // The value of the diagnostic data element is classified as configurable (read and write access is
                     // possible).
    kWriteOnly = 2,  // The access to the data element is limited to write-only. This supports the use case where the
                     // Dcm just writes data to the application software without the intention to read it back,
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticValueAccessEnum& value)
{
    if (std::strcmp(string.c_str(), "READ-ONLY") == 0) {
        value = DiagnosticValueAccessEnum::kReadOnly;
        return true;
    } else if (std::strcmp(string.c_str(), "READ-WRITE") == 0) {
        value = DiagnosticValueAccessEnum::kReadWrite;
        return true;
    } else if (std::strcmp(string.c_str(), "WRITE-ONLY") == 0) {
        value = DiagnosticValueAccessEnum::kWriteOnly;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticValueAccessEnum value)
{
    switch (value) {
        case DiagnosticValueAccessEnum::kReadOnly:
            return "READ-ONLY";
        case DiagnosticValueAccessEnum::kReadWrite:
            return "READ-WRITE";
        case DiagnosticValueAccessEnum::kWriteOnly:
            return "WRITE-ONLY";
        default:
            return nullptr;
    }
}

enum class DiagnosticWwhObdDtcClassEnum
{
    kDemDtcWwhObdClassA             = 0,  // This attribute represents the severity class A.
    kDemDtcWwhObdClassB1            = 1,  // This attribute represents the severity class B1.
    kDemDtcWwhObdClassB2            = 2,  // This attribute represents the severity class B2.
    kDemDtcWwhObdClassC             = 3,  // This attribute represents the severity class C.
    kDemDtcWwhObdClassNoInformation = 4,  // This attribute represents the option to intentionally not describe a
                                          // dedicated severity class of an WWH-OBD DTC.
};

template < typename StringType >
bool FromString(const StringType& string, DiagnosticWwhObdDtcClassEnum& value)
{
    if (std::strcmp(string.c_str(), "DEM-DTC-WWH-OBD-CLASS-A") == 0) {
        value = DiagnosticWwhObdDtcClassEnum::kDemDtcWwhObdClassA;
        return true;
    } else if (std::strcmp(string.c_str(), "DEM-DTC-WWH-OBD-CLASS-B-1") == 0) {
        value = DiagnosticWwhObdDtcClassEnum::kDemDtcWwhObdClassB1;
        return true;
    } else if (std::strcmp(string.c_str(), "DEM-DTC-WWH-OBD-CLASS-B-2") == 0) {
        value = DiagnosticWwhObdDtcClassEnum::kDemDtcWwhObdClassB2;
        return true;
    } else if (std::strcmp(string.c_str(), "DEM-DTC-WWH-OBD-CLASS-C") == 0) {
        value = DiagnosticWwhObdDtcClassEnum::kDemDtcWwhObdClassC;
        return true;
    } else if (std::strcmp(string.c_str(), "DEM-DTC-WWH-OBD-CLASS-NO-INFORMATION") == 0) {
        value = DiagnosticWwhObdDtcClassEnum::kDemDtcWwhObdClassNoInformation;
        return true;
    } else
        return false;
}

inline const char* ToString(DiagnosticWwhObdDtcClassEnum value)
{
    switch (value) {
        case DiagnosticWwhObdDtcClassEnum::kDemDtcWwhObdClassA:
            return "DEM-DTC-WWH-OBD-CLASS-A";
        case DiagnosticWwhObdDtcClassEnum::kDemDtcWwhObdClassB1:
            return "DEM-DTC-WWH-OBD-CLASS-B-1";
        case DiagnosticWwhObdDtcClassEnum::kDemDtcWwhObdClassB2:
            return "DEM-DTC-WWH-OBD-CLASS-B-2";
        case DiagnosticWwhObdDtcClassEnum::kDemDtcWwhObdClassC:
            return "DEM-DTC-WWH-OBD-CLASS-C";
        case DiagnosticWwhObdDtcClassEnum::kDemDtcWwhObdClassNoInformation:
            return "DEM-DTC-WWH-OBD-CLASS-NO-INFORMATION";
        default:
            return nullptr;
    }
}

enum class DiscoveryTechnologyEnum
{
    kBonjour = 0,  // Bonjour Service Discovery
    kDlna    = 1,  // DLNA UPnP Device Control Protocol Framework
    kSlp     = 2,  // Service Location Protocol
    kSomeip  = 3,  // Header format to be used with Remote Procedure Call (RPC) Messages in Client/Server-Communication
                   // as well as Sender/Receiver Messages.
    kSsdp = 4,     // Simple Service Discovery Protocol (SSDP)
};

template < typename StringType >
bool FromString(const StringType& string, DiscoveryTechnologyEnum& value)
{
    if (std::strcmp(string.c_str(), "BONJOUR") == 0) {
        value = DiscoveryTechnologyEnum::kBonjour;
        return true;
    } else if (std::strcmp(string.c_str(), "DLNA") == 0) {
        value = DiscoveryTechnologyEnum::kDlna;
        return true;
    } else if (std::strcmp(string.c_str(), "SLP") == 0) {
        value = DiscoveryTechnologyEnum::kSlp;
        return true;
    } else if (std::strcmp(string.c_str(), "SOMEIP") == 0) {
        value = DiscoveryTechnologyEnum::kSomeip;
        return true;
    } else if (std::strcmp(string.c_str(), "SSDP") == 0) {
        value = DiscoveryTechnologyEnum::kSsdp;
        return true;
    } else
        return false;
}

inline const char* ToString(DiscoveryTechnologyEnum value)
{
    switch (value) {
        case DiscoveryTechnologyEnum::kBonjour:
            return "BONJOUR";
        case DiscoveryTechnologyEnum::kDlna:
            return "DLNA";
        case DiscoveryTechnologyEnum::kSlp:
            return "SLP";
        case DiscoveryTechnologyEnum::kSomeip:
            return "SOMEIP";
        case DiscoveryTechnologyEnum::kSsdp:
            return "SSDP";
        default:
            return nullptr;
    }
}

enum class DisplayPresentationEnum
{
    kPresentationContinuous = 0,  // The presentation of data shall form a continuous graph between data points.
    kPresentationDiscrete   = 1,  // The presentation of data shall be step-shaped between data points.
};

template < typename StringType >
bool FromString(const StringType& string, DisplayPresentationEnum& value)
{
    if (std::strcmp(string.c_str(), "PRESENTATION-CONTINUOUS") == 0) {
        value = DisplayPresentationEnum::kPresentationContinuous;
        return true;
    } else if (std::strcmp(string.c_str(), "PRESENTATION-DISCRETE") == 0) {
        value = DisplayPresentationEnum::kPresentationDiscrete;
        return true;
    } else
        return false;
}

inline const char* ToString(DisplayPresentationEnum value)
{
    switch (value) {
        case DisplayPresentationEnum::kPresentationContinuous:
            return "PRESENTATION-CONTINUOUS";
        case DisplayPresentationEnum::kPresentationDiscrete:
            return "PRESENTATION-DISCRETE";
        default:
            return nullptr;
    }
}

enum class DoIpEntityRoleEnum
{
    kEdgeNode = 0,  // Network node is a DoIP gateway that accepts external connections.
    kGateway  = 1,  // Network node is a Gateway between the DoIP network and other networks.
    kNode     = 2,  // Network node is a DoIp node.
};

template < typename StringType >
bool FromString(const StringType& string, DoIpEntityRoleEnum& value)
{
    if (std::strcmp(string.c_str(), "EDGE-NODE") == 0) {
        value = DoIpEntityRoleEnum::kEdgeNode;
        return true;
    } else if (std::strcmp(string.c_str(), "GATEWAY") == 0) {
        value = DoIpEntityRoleEnum::kGateway;
        return true;
    } else if (std::strcmp(string.c_str(), "NODE") == 0) {
        value = DoIpEntityRoleEnum::kNode;
        return true;
    } else
        return false;
}

inline const char* ToString(DoIpEntityRoleEnum value)
{
    switch (value) {
        case DoIpEntityRoleEnum::kEdgeNode:
            return "EDGE-NODE";
        case DoIpEntityRoleEnum::kGateway:
            return "GATEWAY";
        case DoIpEntityRoleEnum::kNode:
            return "NODE";
        default:
            return nullptr;
    }
}

enum class DtcFormatTypeEnum
{
    kJ1939 = 0,  // Defines the J1939 DTC format.
    kObd   = 1,  // Defines the OBD DTC format.
    kUds   = 2,  // Defines the UDS DTC format.
};

template < typename StringType >
bool FromString(const StringType& string, DtcFormatTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "J-1939") == 0) {
        value = DtcFormatTypeEnum::kJ1939;
        return true;
    } else if (std::strcmp(string.c_str(), "OBD") == 0) {
        value = DtcFormatTypeEnum::kObd;
        return true;
    } else if (std::strcmp(string.c_str(), "UDS") == 0) {
        value = DtcFormatTypeEnum::kUds;
        return true;
    } else
        return false;
}

inline const char* ToString(DtcFormatTypeEnum value)
{
    switch (value) {
        case DtcFormatTypeEnum::kJ1939:
            return "J-1939";
        case DtcFormatTypeEnum::kObd:
            return "OBD";
        case DtcFormatTypeEnum::kUds:
            return "UDS";
        default:
            return nullptr;
    }
}

enum class DtcKindEnum
{
    kEmissionRelatedDtc     = 0,  // This indicates that the monitor reports a OBD-relevant malfunction.
    kNonEmmissionRelatedDtc = 1,  // This indicates that the monitor reports a non-OBD-relevant malfunction.
};

template < typename StringType >
bool FromString(const StringType& string, DtcKindEnum& value)
{
    if (std::strcmp(string.c_str(), "EMISSION-RELATED-DTC") == 0) {
        value = DtcKindEnum::kEmissionRelatedDtc;
        return true;
    } else if (std::strcmp(string.c_str(), "NON-EMMISSION-RELATED-DTC") == 0) {
        value = DtcKindEnum::kNonEmmissionRelatedDtc;
        return true;
    } else
        return false;
}

inline const char* ToString(DtcKindEnum value)
{
    switch (value) {
        case DtcKindEnum::kEmissionRelatedDtc:
            return "EMISSION-RELATED-DTC";
        case DtcKindEnum::kNonEmmissionRelatedDtc:
            return "NON-EMMISSION-RELATED-DTC";
        default:
            return nullptr;
    }
}

enum class EEnum
{
    kBold       = 0,  // The emphasis is preferably represented in boldface font.
    kBolditalic = 1,  // The emphasis is preferably represented in boldface plus italic font.
    kItalic     = 2,  // The emphasis is preferably represented in italic font.
    kPlain = 3,  // The emphasis has no specific rendering. It is used if e.g. semantic information is applied to the
                 // emphasis text.
};

template < typename StringType >
bool FromString(const StringType& string, EEnum& value)
{
    if (std::strcmp(string.c_str(), "BOLD") == 0) {
        value = EEnum::kBold;
        return true;
    } else if (std::strcmp(string.c_str(), "BOLDITALIC") == 0) {
        value = EEnum::kBolditalic;
        return true;
    } else if (std::strcmp(string.c_str(), "ITALIC") == 0) {
        value = EEnum::kItalic;
        return true;
    } else if (std::strcmp(string.c_str(), "PLAIN") == 0) {
        value = EEnum::kPlain;
        return true;
    } else
        return false;
}

inline const char* ToString(EEnum value)
{
    switch (value) {
        case EEnum::kBold:
            return "BOLD";
        case EEnum::kBolditalic:
            return "BOLDITALIC";
        case EEnum::kItalic:
            return "ITALIC";
        case EEnum::kPlain:
            return "PLAIN";
        default:
            return nullptr;
    }
}

enum class EcucAffectionEnum
{
    kLtAffectsPb = 0,  // A link time parameter affecting one or several post-build time parameter(s).
    kNoAffect    = 1,  // no affect on any other parameter.
    kPcAffectsLt = 2,  // A pre-compile time parameter affecting one or several link time parameter(s).
    kPcAffectsLtAndPb
    = 3,  // A pre-compile time parameter affecting one or several link time and post-build time parameter(s)).
    kPcAffectsPb = 4,  // A pre-compile time parameter affecting one or several post build time parameter(s).
};

template < typename StringType >
bool FromString(const StringType& string, EcucAffectionEnum& value)
{
    if (std::strcmp(string.c_str(), "LT-AFFECTS-PB") == 0) {
        value = EcucAffectionEnum::kLtAffectsPb;
        return true;
    } else if (std::strcmp(string.c_str(), "NO-AFFECT") == 0) {
        value = EcucAffectionEnum::kNoAffect;
        return true;
    } else if (std::strcmp(string.c_str(), "PC-AFFECTS-LT") == 0) {
        value = EcucAffectionEnum::kPcAffectsLt;
        return true;
    } else if (std::strcmp(string.c_str(), "PC-AFFECTS-LT-AND-PB") == 0) {
        value = EcucAffectionEnum::kPcAffectsLtAndPb;
        return true;
    } else if (std::strcmp(string.c_str(), "PC-AFFECTS-PB") == 0) {
        value = EcucAffectionEnum::kPcAffectsPb;
        return true;
    } else
        return false;
}

inline const char* ToString(EcucAffectionEnum value)
{
    switch (value) {
        case EcucAffectionEnum::kLtAffectsPb:
            return "LT-AFFECTS-PB";
        case EcucAffectionEnum::kNoAffect:
            return "NO-AFFECT";
        case EcucAffectionEnum::kPcAffectsLt:
            return "PC-AFFECTS-LT";
        case EcucAffectionEnum::kPcAffectsLtAndPb:
            return "PC-AFFECTS-LT-AND-PB";
        case EcucAffectionEnum::kPcAffectsPb:
            return "PC-AFFECTS-PB";
        default:
            return nullptr;
    }
}

enum class EcucConfigurationClassEnum
{
    kLink       = 0,  // Link Time: parts of configuration are delivered from another object code file
    kPostBuild  = 1,  // PostBuildTime: after compilation a configuration parameter can be changed.
    kPreCompile = 2,  // PreCompile Time: after compilation a configuration parameter can not be changed any more.
    kPublishedInformation = 3,  // PublishedInformation is used to specify the fact that certain  information is fixed
                                // even before the pre-compile stage.
};

template < typename StringType >
bool FromString(const StringType& string, EcucConfigurationClassEnum& value)
{
    if (std::strcmp(string.c_str(), "LINK") == 0) {
        value = EcucConfigurationClassEnum::kLink;
        return true;
    } else if (std::strcmp(string.c_str(), "POST-BUILD") == 0) {
        value = EcucConfigurationClassEnum::kPostBuild;
        return true;
    } else if (std::strcmp(string.c_str(), "PRE-COMPILE") == 0) {
        value = EcucConfigurationClassEnum::kPreCompile;
        return true;
    } else if (std::strcmp(string.c_str(), "PUBLISHED-INFORMATION") == 0) {
        value = EcucConfigurationClassEnum::kPublishedInformation;
        return true;
    } else
        return false;
}

inline const char* ToString(EcucConfigurationClassEnum value)
{
    switch (value) {
        case EcucConfigurationClassEnum::kLink:
            return "LINK";
        case EcucConfigurationClassEnum::kPostBuild:
            return "POST-BUILD";
        case EcucConfigurationClassEnum::kPreCompile:
            return "PRE-COMPILE";
        case EcucConfigurationClassEnum::kPublishedInformation:
            return "PUBLISHED-INFORMATION";
        default:
            return nullptr;
    }
}

enum class EcucConfigurationVariantEnum
{
    kPreconfiguredConfiguration = 0,  // Preconfigured (i.e. fixed) configuration which cannot be changed.
    kRecommendedConfiguration   = 1,  // Recommended configuration for a module.
    kVariantLinkTime = 2,           // Specifies that the BSW Module implementation may use PreCompileTime and LinkTime
                                    // configuration parameters.
    kVariantPostBuild = 3,          // Specifies that the BSW Module implementation may use PreCompileTime, LinkTime and
                                    // PostBuild configuration parameters.
    kVariantPostBuildLoadable = 4,  // Specifies that the BSW Module implementation may use PreCompileTime, LinkTime and
                                    // PostBuild loadable configuration parameters (supported in the VSMD).  This
                                    // attribute is removed from the specifications and shall not be used.
    kVariantPostBuildSelectable = 5,  // Specifies that the BSW Module implementation may use PreCompileTime, LinkTime
                                      // and PostBuild selectable configuration parameters (supported in the VSMD). This
                                      // attribute is removed from the specifications and shall not be used.
    kVariantPreCompile
    = 6,  // Specifies that the BSW Module implementation uses only PreCompileTime configuration parameters.
};

template < typename StringType >
bool FromString(const StringType& string, EcucConfigurationVariantEnum& value)
{
    if (std::strcmp(string.c_str(), "PRECONFIGURED-CONFIGURATION") == 0) {
        value = EcucConfigurationVariantEnum::kPreconfiguredConfiguration;
        return true;
    } else if (std::strcmp(string.c_str(), "RECOMMENDED-CONFIGURATION") == 0) {
        value = EcucConfigurationVariantEnum::kRecommendedConfiguration;
        return true;
    } else if (std::strcmp(string.c_str(), "VARIANT-LINK-TIME") == 0) {
        value = EcucConfigurationVariantEnum::kVariantLinkTime;
        return true;
    } else if (std::strcmp(string.c_str(), "VARIANT-POST-BUILD") == 0) {
        value = EcucConfigurationVariantEnum::kVariantPostBuild;
        return true;
    } else if (std::strcmp(string.c_str(), "VARIANT-POST-BUILD-LOADABLE") == 0) {
        value = EcucConfigurationVariantEnum::kVariantPostBuildLoadable;
        return true;
    } else if (std::strcmp(string.c_str(), "VARIANT-POST-BUILD-SELECTABLE") == 0) {
        value = EcucConfigurationVariantEnum::kVariantPostBuildSelectable;
        return true;
    } else if (std::strcmp(string.c_str(), "VARIANT-PRE-COMPILE") == 0) {
        value = EcucConfigurationVariantEnum::kVariantPreCompile;
        return true;
    } else
        return false;
}

inline const char* ToString(EcucConfigurationVariantEnum value)
{
    switch (value) {
        case EcucConfigurationVariantEnum::kPreconfiguredConfiguration:
            return "PRECONFIGURED-CONFIGURATION";
        case EcucConfigurationVariantEnum::kRecommendedConfiguration:
            return "RECOMMENDED-CONFIGURATION";
        case EcucConfigurationVariantEnum::kVariantLinkTime:
            return "VARIANT-LINK-TIME";
        case EcucConfigurationVariantEnum::kVariantPostBuild:
            return "VARIANT-POST-BUILD";
        case EcucConfigurationVariantEnum::kVariantPostBuildLoadable:
            return "VARIANT-POST-BUILD-LOADABLE";
        case EcucConfigurationVariantEnum::kVariantPostBuildSelectable:
            return "VARIANT-POST-BUILD-SELECTABLE";
        case EcucConfigurationVariantEnum::kVariantPreCompile:
            return "VARIANT-PRE-COMPILE";
        default:
            return nullptr;
    }
}

enum class EcucDestinationUriNestingContractEnum
{
    kLeafOfTargetContainer = 0,  // EcucDestinationUriPolicy describes elements (subContainers, Parameters, References)
                                 // that are directly owned by the target container.
    kTargetContainer = 1,        // EcucDestinationUriPolicy describes the target container of EcucUriReferenceDef.
    kVertexOfTargetContainer
    = 2,  // EcucDestinationUriPolicy describes elements (subContainers, Parameters, References) of the target
          // container which can be defined in arbitrary nested subContainer structure.
};

template < typename StringType >
bool FromString(const StringType& string, EcucDestinationUriNestingContractEnum& value)
{
    if (std::strcmp(string.c_str(), "LEAF-OF-TARGET-CONTAINER") == 0) {
        value = EcucDestinationUriNestingContractEnum::kLeafOfTargetContainer;
        return true;
    } else if (std::strcmp(string.c_str(), "TARGET-CONTAINER") == 0) {
        value = EcucDestinationUriNestingContractEnum::kTargetContainer;
        return true;
    } else if (std::strcmp(string.c_str(), "VERTEX-OF-TARGET-CONTAINER") == 0) {
        value = EcucDestinationUriNestingContractEnum::kVertexOfTargetContainer;
        return true;
    } else
        return false;
}

inline const char* ToString(EcucDestinationUriNestingContractEnum value)
{
    switch (value) {
        case EcucDestinationUriNestingContractEnum::kLeafOfTargetContainer:
            return "LEAF-OF-TARGET-CONTAINER";
        case EcucDestinationUriNestingContractEnum::kTargetContainer:
            return "TARGET-CONTAINER";
        case EcucDestinationUriNestingContractEnum::kVertexOfTargetContainer:
            return "VERTEX-OF-TARGET-CONTAINER";
        default:
            return nullptr;
    }
}

enum class EcucScopeEnum
{
    kEcu   = 0,  // An element may be shared with other modules.
    kLocal = 1,  // An element is only be applicable for the module it is defined in.
};

template < typename StringType >
bool FromString(const StringType& string, EcucScopeEnum& value)
{
    if (std::strcmp(string.c_str(), "ECU") == 0) {
        value = EcucScopeEnum::kEcu;
        return true;
    } else if (std::strcmp(string.c_str(), "LOCAL") == 0) {
        value = EcucScopeEnum::kLocal;
        return true;
    } else
        return false;
}

inline const char* ToString(EcucScopeEnum value)
{
    switch (value) {
        case EcucScopeEnum::kEcu:
            return "ECU";
        case EcucScopeEnum::kLocal:
            return "LOCAL";
        default:
            return nullptr;
    }
}

enum class EndToEndProfileBehaviorEnum
{
    kPreR42 = 0,  // Check has the legacy behavior, before AUTOSAR Release 4.2.
    kR42    = 1,  // Check behaves like new P4/P5/P6 profiles introduced in AUTOSAR Release 4.2.
};

template < typename StringType >
bool FromString(const StringType& string, EndToEndProfileBehaviorEnum& value)
{
    if (std::strcmp(string.c_str(), "PRE--R-4--2") == 0) {
        value = EndToEndProfileBehaviorEnum::kPreR42;
        return true;
    } else if (std::strcmp(string.c_str(), "R-4--2") == 0) {
        value = EndToEndProfileBehaviorEnum::kR42;
        return true;
    } else
        return false;
}

inline const char* ToString(EndToEndProfileBehaviorEnum value)
{
    switch (value) {
        case EndToEndProfileBehaviorEnum::kPreR42:
            return "PRE--R-4--2";
        case EndToEndProfileBehaviorEnum::kR42:
            return "R-4--2";
        default:
            return nullptr;
    }
}

enum class EnvironmentCaptureToReportingEnum
{
    kCaptureAsynchronousToReporting
    = 0,  // The data capturing is postponed to the next cycle of the Dem_Mainfunction. (This means that there is a
          // minimum delay between report of the failure and capturing the data.
    kCaptureSynchronousToReporting = 1,  // The data is captured immediately within the reporting function (i.e. in the
                                         // context of the setEventStatus/reportErrorStatus function).
};

template < typename StringType >
bool FromString(const StringType& string, EnvironmentCaptureToReportingEnum& value)
{
    if (std::strcmp(string.c_str(), "CAPTURE-ASYNCHRONOUS-TO-REPORTING") == 0) {
        value = EnvironmentCaptureToReportingEnum::kCaptureAsynchronousToReporting;
        return true;
    } else if (std::strcmp(string.c_str(), "CAPTURE-SYNCHRONOUS-TO-REPORTING") == 0) {
        value = EnvironmentCaptureToReportingEnum::kCaptureSynchronousToReporting;
        return true;
    } else
        return false;
}

inline const char* ToString(EnvironmentCaptureToReportingEnum value)
{
    switch (value) {
        case EnvironmentCaptureToReportingEnum::kCaptureAsynchronousToReporting:
            return "CAPTURE-ASYNCHRONOUS-TO-REPORTING";
        case EnvironmentCaptureToReportingEnum::kCaptureSynchronousToReporting:
            return "CAPTURE-SYNCHRONOUS-TO-REPORTING";
        default:
            return nullptr;
    }
}

enum class EthGlobalTimeMessageFormatEnum
{
    kIeee8021as        = 0,  // Message format according to IEEE 802.1AS standard.
    kIeee8021asAutosar = 1,  // Message format according to IEEE 802.1AS standard with AUTOSAR extensions.
};

template < typename StringType >
bool FromString(const StringType& string, EthGlobalTimeMessageFormatEnum& value)
{
    if (std::strcmp(string.c_str(), "IEEE802-1AS") == 0) {
        value = EthGlobalTimeMessageFormatEnum::kIeee8021as;
        return true;
    } else if (std::strcmp(string.c_str(), "IEEE802-1AS-AUTOSAR") == 0) {
        value = EthGlobalTimeMessageFormatEnum::kIeee8021asAutosar;
        return true;
    } else
        return false;
}

inline const char* ToString(EthGlobalTimeMessageFormatEnum value)
{
    switch (value) {
        case EthGlobalTimeMessageFormatEnum::kIeee8021as:
            return "IEEE802-1AS";
        case EthGlobalTimeMessageFormatEnum::kIeee8021asAutosar:
            return "IEEE802-1AS-AUTOSAR";
        default:
            return nullptr;
    }
}

enum class EthernetConnectionNegotiationEnum
{
    kAuto   = 0,  // Automatic Negotiation
    kMaster = 1,  // Master
    kSlave  = 2,  // Slave
};

template < typename StringType >
bool FromString(const StringType& string, EthernetConnectionNegotiationEnum& value)
{
    if (std::strcmp(string.c_str(), "AUTO") == 0) {
        value = EthernetConnectionNegotiationEnum::kAuto;
        return true;
    } else if (std::strcmp(string.c_str(), "MASTER") == 0) {
        value = EthernetConnectionNegotiationEnum::kMaster;
        return true;
    } else if (std::strcmp(string.c_str(), "SLAVE") == 0) {
        value = EthernetConnectionNegotiationEnum::kSlave;
        return true;
    } else
        return false;
}

inline const char* ToString(EthernetConnectionNegotiationEnum value)
{
    switch (value) {
        case EthernetConnectionNegotiationEnum::kAuto:
            return "AUTO";
        case EthernetConnectionNegotiationEnum::kMaster:
            return "MASTER";
        case EthernetConnectionNegotiationEnum::kSlave:
            return "SLAVE";
        default:
            return nullptr;
    }
}

enum class EthernetCouplingPortSchedulerEnum
{
    kDeficitRoundRobin  = 0,  // Schedule algorithm "deficit round robin"
    kStrictPriority     = 1,  // Schedule algorithm "strict priority"
    kWeightedRoundRobin = 2,  // Schedule algorithm "weighted round robin"
};

template < typename StringType >
bool FromString(const StringType& string, EthernetCouplingPortSchedulerEnum& value)
{
    if (std::strcmp(string.c_str(), "DEFICIT-ROUND-ROBIN") == 0) {
        value = EthernetCouplingPortSchedulerEnum::kDeficitRoundRobin;
        return true;
    } else if (std::strcmp(string.c_str(), "STRICT-PRIORITY") == 0) {
        value = EthernetCouplingPortSchedulerEnum::kStrictPriority;
        return true;
    } else if (std::strcmp(string.c_str(), "WEIGHTED-ROUND-ROBIN") == 0) {
        value = EthernetCouplingPortSchedulerEnum::kWeightedRoundRobin;
        return true;
    } else
        return false;
}

inline const char* ToString(EthernetCouplingPortSchedulerEnum value)
{
    switch (value) {
        case EthernetCouplingPortSchedulerEnum::kDeficitRoundRobin:
            return "DEFICIT-ROUND-ROBIN";
        case EthernetCouplingPortSchedulerEnum::kStrictPriority:
            return "STRICT-PRIORITY";
        case EthernetCouplingPortSchedulerEnum::kWeightedRoundRobin:
            return "WEIGHTED-ROUND-ROBIN";
        default:
            return nullptr;
    }
}

enum class EthernetMacLayerTypeEnum
{
    kXMii   = 0,  // Mac layer interface (data) bandwith class 100Mbit/s and 10Mbit/s (e.g. RMII, RvMII, SMII, RvMII)
    kXgMii  = 1,  // Mac layer interface (data) bandwith class 1Gbit/s (e.g. GMII, RGMII, SGMII, RvGMII, USGMII)
    kXxgMii = 2,  // Mac layer interface (data) bandwith class 10Gbit/s
};

template < typename StringType >
bool FromString(const StringType& string, EthernetMacLayerTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "X-MII") == 0) {
        value = EthernetMacLayerTypeEnum::kXMii;
        return true;
    } else if (std::strcmp(string.c_str(), "XG-MII") == 0) {
        value = EthernetMacLayerTypeEnum::kXgMii;
        return true;
    } else if (std::strcmp(string.c_str(), "XXG-MII") == 0) {
        value = EthernetMacLayerTypeEnum::kXxgMii;
        return true;
    } else
        return false;
}

inline const char* ToString(EthernetMacLayerTypeEnum value)
{
    switch (value) {
        case EthernetMacLayerTypeEnum::kXMii:
            return "X-MII";
        case EthernetMacLayerTypeEnum::kXgMii:
            return "XG-MII";
        case EthernetMacLayerTypeEnum::kXxgMii:
            return "XXG-MII";
        default:
            return nullptr;
    }
}

enum class EthernetPhysicalLayerTypeEnum
{
    kBaseT       = 0,  // BaseT physical layer (10BaseT, 100BaseT, 1000BaseT)
    kBroadRReach = 1,  // BroadR-Reach physical layer
    kRtpge       = 2,  // Reduced Twisted Pair Gigabit Ethernet (RTPGE) physical layer
    kXMii        = 3,  // Media Independent Interface (MII) physical layer
    kXMmi = 4,  // This enumeration literal is set to obsolete and will be removed in future. Please use xMII instead.
                // Old description: Media Independent Interface (MII) physical layer
    k100baseTx = 5,    // Ethernet Standard (IEEE 802.3u) to support 100Mbit/s over two twisted pairs.
    k1000baseT = 6,    // Ethernet Standard (IEEE 802.3ab) to support 1Gbit/s over 4 twisted pairs.
    k100baseT1 = 7,    // Ethernet Standard (IEEE 802.3bw) to support 100Mbit/s over a single twisted pair cable.
                       // 100BASE-T1 is the IEEE Standardized version of BroadRReach.
    k1000baseT1 = 8,   // Ethernet Standard (IEEE 802.3bp) to support 1Gbit/s over a single twisted pair cable.
    kIeee80211p = 9,   // Ethernet Standard (IEEE 802.11p) to support wireless communication in vehicular environments.
    k10baseT1s  = 10,  // Physical layer interface 10BASE-T1S (10Mbit/s, 2 pairs). Used for automotive.
};

template < typename StringType >
bool FromString(const StringType& string, EthernetPhysicalLayerTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "BASE-T") == 0) {
        value = EthernetPhysicalLayerTypeEnum::kBaseT;
        return true;
    } else if (std::strcmp(string.c_str(), "BROAD-R-REACH") == 0) {
        value = EthernetPhysicalLayerTypeEnum::kBroadRReach;
        return true;
    } else if (std::strcmp(string.c_str(), "RTPGE") == 0) {
        value = EthernetPhysicalLayerTypeEnum::kRtpge;
        return true;
    } else if (std::strcmp(string.c_str(), "X-MII") == 0) {
        value = EthernetPhysicalLayerTypeEnum::kXMii;
        return true;
    } else if (std::strcmp(string.c_str(), "X-MMI") == 0) {
        value = EthernetPhysicalLayerTypeEnum::kXMmi;
        return true;
    } else if (std::strcmp(string.c_str(), "100BASE-TX") == 0) {
        value = EthernetPhysicalLayerTypeEnum::k100baseTx;
        return true;
    } else if (std::strcmp(string.c_str(), "1000BASE-T") == 0) {
        value = EthernetPhysicalLayerTypeEnum::k1000baseT;
        return true;
    } else if (std::strcmp(string.c_str(), "100BASE-T1") == 0) {
        value = EthernetPhysicalLayerTypeEnum::k100baseT1;
        return true;
    } else if (std::strcmp(string.c_str(), "1000BASE-T1") == 0) {
        value = EthernetPhysicalLayerTypeEnum::k1000baseT1;
        return true;
    } else if (std::strcmp(string.c_str(), "IEEE802-11P") == 0) {
        value = EthernetPhysicalLayerTypeEnum::kIeee80211p;
        return true;
    } else if (std::strcmp(string.c_str(), "10BASE-T1S") == 0) {
        value = EthernetPhysicalLayerTypeEnum::k10baseT1s;
        return true;
    } else
        return false;
}

inline const char* ToString(EthernetPhysicalLayerTypeEnum value)
{
    switch (value) {
        case EthernetPhysicalLayerTypeEnum::kBaseT:
            return "BASE-T";
        case EthernetPhysicalLayerTypeEnum::kBroadRReach:
            return "BROAD-R-REACH";
        case EthernetPhysicalLayerTypeEnum::kRtpge:
            return "RTPGE";
        case EthernetPhysicalLayerTypeEnum::kXMii:
            return "X-MII";
        case EthernetPhysicalLayerTypeEnum::kXMmi:
            return "X-MMI";
        case EthernetPhysicalLayerTypeEnum::k100baseTx:
            return "100BASE-TX";
        case EthernetPhysicalLayerTypeEnum::k1000baseT:
            return "1000BASE-T";
        case EthernetPhysicalLayerTypeEnum::k100baseT1:
            return "100BASE-T1";
        case EthernetPhysicalLayerTypeEnum::k1000baseT1:
            return "1000BASE-T1";
        case EthernetPhysicalLayerTypeEnum::kIeee80211p:
            return "IEEE802-11P";
        case EthernetPhysicalLayerTypeEnum::k10baseT1s:
            return "10BASE-T1S";
        default:
            return nullptr;
    }
}

enum class EthernetSwitchVlanEgressTaggingEnum
{
    kNotSent      = 0,  // will not be sent
    kSentTagged   = 1,  // sent with its VLAN tag
    kSentUntagged = 2,  // sent without a VLAN tag
};

template < typename StringType >
bool FromString(const StringType& string, EthernetSwitchVlanEgressTaggingEnum& value)
{
    if (std::strcmp(string.c_str(), "NOT-SENT") == 0) {
        value = EthernetSwitchVlanEgressTaggingEnum::kNotSent;
        return true;
    } else if (std::strcmp(string.c_str(), "SENT-TAGGED") == 0) {
        value = EthernetSwitchVlanEgressTaggingEnum::kSentTagged;
        return true;
    } else if (std::strcmp(string.c_str(), "SENT-UNTAGGED") == 0) {
        value = EthernetSwitchVlanEgressTaggingEnum::kSentUntagged;
        return true;
    } else
        return false;
}

inline const char* ToString(EthernetSwitchVlanEgressTaggingEnum value)
{
    switch (value) {
        case EthernetSwitchVlanEgressTaggingEnum::kNotSent:
            return "NOT-SENT";
        case EthernetSwitchVlanEgressTaggingEnum::kSentTagged:
            return "SENT-TAGGED";
        case EthernetSwitchVlanEgressTaggingEnum::kSentUntagged:
            return "SENT-UNTAGGED";
        default:
            return nullptr;
    }
}

enum class EthernetSwitchVlanIngressTagEnum
{
    kForwardAsIs  = 0,  // Forward with the same VLAN as received. Also untagged frames will be forwarded as untagged.
    kDropUntagged = 1,  // Drop if untagged.
};

template < typename StringType >
bool FromString(const StringType& string, EthernetSwitchVlanIngressTagEnum& value)
{
    if (std::strcmp(string.c_str(), "FORWARD-AS-IS") == 0) {
        value = EthernetSwitchVlanIngressTagEnum::kForwardAsIs;
        return true;
    } else if (std::strcmp(string.c_str(), "DROP-UNTAGGED") == 0) {
        value = EthernetSwitchVlanIngressTagEnum::kDropUntagged;
        return true;
    } else
        return false;
}

inline const char* ToString(EthernetSwitchVlanIngressTagEnum value)
{
    switch (value) {
        case EthernetSwitchVlanIngressTagEnum::kForwardAsIs:
            return "FORWARD-AS-IS";
        case EthernetSwitchVlanIngressTagEnum::kDropUntagged:
            return "DROP-UNTAGGED";
        default:
            return nullptr;
    }
}

enum class EventAcceptanceStatusEnum
{
    kEventAcceptanceDisabled = 0,  // Acceptance of a diagnostic event is disabled.
    kEventAcceptanceEnabled  = 1,  // Acceptance of a diagnostic event is enabled.
};

template < typename StringType >
bool FromString(const StringType& string, EventAcceptanceStatusEnum& value)
{
    if (std::strcmp(string.c_str(), "EVENT-ACCEPTANCE-DISABLED") == 0) {
        value = EventAcceptanceStatusEnum::kEventAcceptanceDisabled;
        return true;
    } else if (std::strcmp(string.c_str(), "EVENT-ACCEPTANCE-ENABLED") == 0) {
        value = EventAcceptanceStatusEnum::kEventAcceptanceEnabled;
        return true;
    } else
        return false;
}

inline const char* ToString(EventAcceptanceStatusEnum value)
{
    switch (value) {
        case EventAcceptanceStatusEnum::kEventAcceptanceDisabled:
            return "EVENT-ACCEPTANCE-DISABLED";
        case EventAcceptanceStatusEnum::kEventAcceptanceEnabled:
            return "EVENT-ACCEPTANCE-ENABLED";
        default:
            return nullptr;
    }
}

enum class EventGroupControlTypeEnum
{
    kActivationAndTriggerUnicast = 0,  // Activate the data path for unicast events and triggered unicast events that
                                       // are sent out after a client got subscribed.
    kActivationMulticast = 1,          // Activate the data path for multicast events of an EventGroup.
    kActivationUnicast   = 2,          // Activate the data path for unicast events of an EventGroup.
    kTriggerUnicast
    = 3,  // Activate the data path for triggered unicast events that are sent out after a client got subscribed.
};

template < typename StringType >
bool FromString(const StringType& string, EventGroupControlTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "ACTIVATION-AND-TRIGGER-UNICAST") == 0) {
        value = EventGroupControlTypeEnum::kActivationAndTriggerUnicast;
        return true;
    } else if (std::strcmp(string.c_str(), "ACTIVATION-MULTICAST") == 0) {
        value = EventGroupControlTypeEnum::kActivationMulticast;
        return true;
    } else if (std::strcmp(string.c_str(), "ACTIVATION-UNICAST") == 0) {
        value = EventGroupControlTypeEnum::kActivationUnicast;
        return true;
    } else if (std::strcmp(string.c_str(), "TRIGGER-UNICAST") == 0) {
        value = EventGroupControlTypeEnum::kTriggerUnicast;
        return true;
    } else
        return false;
}

inline const char* ToString(EventGroupControlTypeEnum value)
{
    switch (value) {
        case EventGroupControlTypeEnum::kActivationAndTriggerUnicast:
            return "ACTIVATION-AND-TRIGGER-UNICAST";
        case EventGroupControlTypeEnum::kActivationMulticast:
            return "ACTIVATION-MULTICAST";
        case EventGroupControlTypeEnum::kActivationUnicast:
            return "ACTIVATION-UNICAST";
        case EventGroupControlTypeEnum::kTriggerUnicast:
            return "TRIGGER-UNICAST";
        default:
            return nullptr;
    }
}

enum class EventOccurrenceKindEnum
{
    kMultipleOccurrences = 0,  // Specifies that an event may occur more than once in a given time interval.
    kSingleOccurrence    = 1,  // Specifies that an event shall occur only once in a given time interval.
};

template < typename StringType >
bool FromString(const StringType& string, EventOccurrenceKindEnum& value)
{
    if (std::strcmp(string.c_str(), "MULTIPLE-OCCURRENCES") == 0) {
        value = EventOccurrenceKindEnum::kMultipleOccurrences;
        return true;
    } else if (std::strcmp(string.c_str(), "SINGLE-OCCURRENCE") == 0) {
        value = EventOccurrenceKindEnum::kSingleOccurrence;
        return true;
    } else
        return false;
}

inline const char* ToString(EventOccurrenceKindEnum value)
{
    switch (value) {
        case EventOccurrenceKindEnum::kMultipleOccurrences:
            return "MULTIPLE-OCCURRENCES";
        case EventOccurrenceKindEnum::kSingleOccurrence:
            return "SINGLE-OCCURRENCE";
        default:
            return nullptr;
    }
}

enum class ExecutionOrderConstraintTypeEnum
{
    kHierarchicalEoc
    = 0,  // Specifies that the Execution Order Constraint specifies a hierarchical execution order constraint.
    kOrdinaryEoc
    = 1,  // Specifies that the Execution Order Constraint specifies an ordinary execution order constraint.
    kRepetitiveEoc
    = 2,  // Specifies that the Execution Order Constraint specifies a repetitive execution order constraint.
};

template < typename StringType >
bool FromString(const StringType& string, ExecutionOrderConstraintTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "HIERARCHICAL-EOC") == 0) {
        value = ExecutionOrderConstraintTypeEnum::kHierarchicalEoc;
        return true;
    } else if (std::strcmp(string.c_str(), "ORDINARY-EOC") == 0) {
        value = ExecutionOrderConstraintTypeEnum::kOrdinaryEoc;
        return true;
    } else if (std::strcmp(string.c_str(), "REPETITIVE-EOC") == 0) {
        value = ExecutionOrderConstraintTypeEnum::kRepetitiveEoc;
        return true;
    } else
        return false;
}

inline const char* ToString(ExecutionOrderConstraintTypeEnum value)
{
    switch (value) {
        case ExecutionOrderConstraintTypeEnum::kHierarchicalEoc:
            return "HIERARCHICAL-EOC";
        case ExecutionOrderConstraintTypeEnum::kOrdinaryEoc:
            return "ORDINARY-EOC";
        case ExecutionOrderConstraintTypeEnum::kRepetitiveEoc:
            return "REPETITIVE-EOC";
        default:
            return nullptr;
    }
}

enum class ExecutionStateReportingBehaviorEnum
{
    kReportsExecutionState = 0,  // The Executable shall report its execution state to the Execution Management.
    kDoesNotReportExecutionState
    = 1,  // The Executable shall not report its execution state to the Execution Management.
};

template < typename StringType >
bool FromString(const StringType& string, ExecutionStateReportingBehaviorEnum& value)
{
    if (std::strcmp(string.c_str(), "REPORTS-EXECUTION-STATE") == 0) {
        value = ExecutionStateReportingBehaviorEnum::kReportsExecutionState;
        return true;
    } else if (std::strcmp(string.c_str(), "DOES-NOT-REPORT-EXECUTION-STATE") == 0) {
        value = ExecutionStateReportingBehaviorEnum::kDoesNotReportExecutionState;
        return true;
    } else
        return false;
}

inline const char* ToString(ExecutionStateReportingBehaviorEnum value)
{
    switch (value) {
        case ExecutionStateReportingBehaviorEnum::kReportsExecutionState:
            return "REPORTS-EXECUTION-STATE";
        case ExecutionStateReportingBehaviorEnum::kDoesNotReportExecutionState:
            return "DOES-NOT-REPORT-EXECUTION-STATE";
        default:
            return nullptr;
    }
}

enum class ExecutionTimeTypeEnum
{
    kGross = 0,  // Indicates that the given execution time is the time used to execute the ExecutableEntity without any
                 // interruption and and including external calls.
    kNet = 1,    // Indicates that the given execution time is the time used to execute the ExecutableEntity without any
                 // interruption and without any external calls.
};

template < typename StringType >
bool FromString(const StringType& string, ExecutionTimeTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "GROSS") == 0) {
        value = ExecutionTimeTypeEnum::kGross;
        return true;
    } else if (std::strcmp(string.c_str(), "NET") == 0) {
        value = ExecutionTimeTypeEnum::kNet;
        return true;
    } else
        return false;
}

inline const char* ToString(ExecutionTimeTypeEnum value)
{
    switch (value) {
        case ExecutionTimeTypeEnum::kGross:
            return "GROSS";
        case ExecutionTimeTypeEnum::kNet:
            return "NET";
        default:
            return nullptr;
    }
}

enum class FieldAccessEnum
{
    kGetter       = 0,  // Access to the getter of the Field.
    kSetter       = 1,  // Access to the setter of the Field.
    kGetterSetter = 2,  // Access to getter and setter of the field
};

template < typename StringType >
bool FromString(const StringType& string, FieldAccessEnum& value)
{
    if (std::strcmp(string.c_str(), "GETTER") == 0) {
        value = FieldAccessEnum::kGetter;
        return true;
    } else if (std::strcmp(string.c_str(), "SETTER") == 0) {
        value = FieldAccessEnum::kSetter;
        return true;
    } else if (std::strcmp(string.c_str(), "GETTER-SETTER") == 0) {
        value = FieldAccessEnum::kGetterSetter;
        return true;
    } else
        return false;
}

inline const char* ToString(FieldAccessEnum value)
{
    switch (value) {
        case FieldAccessEnum::kGetter:
            return "GETTER";
        case FieldAccessEnum::kSetter:
            return "SETTER";
        case FieldAccessEnum::kGetterSetter:
            return "GETTER-SETTER";
        default:
            return nullptr;
    }
}

enum class FilterDebouncingEnum
{
    kDebounceData = 0,  // The signal is a mean value
    kRawData      = 1,  // Means that no modification of the signal has been applied. This is the default value
    kWaitTimeDate = 2,  // The signal is delivered by a GET operation after a certain amount of time
};

template < typename StringType >
bool FromString(const StringType& string, FilterDebouncingEnum& value)
{
    if (std::strcmp(string.c_str(), "DEBOUNCE-DATA") == 0) {
        value = FilterDebouncingEnum::kDebounceData;
        return true;
    } else if (std::strcmp(string.c_str(), "RAW-DATA") == 0) {
        value = FilterDebouncingEnum::kRawData;
        return true;
    } else if (std::strcmp(string.c_str(), "WAIT-TIME-DATE") == 0) {
        value = FilterDebouncingEnum::kWaitTimeDate;
        return true;
    } else
        return false;
}

inline const char* ToString(FilterDebouncingEnum value)
{
    switch (value) {
        case FilterDebouncingEnum::kDebounceData:
            return "DEBOUNCE-DATA";
        case FilterDebouncingEnum::kRawData:
            return "RAW-DATA";
        case FilterDebouncingEnum::kWaitTimeDate:
            return "WAIT-TIME-DATE";
        default:
            return nullptr;
    }
}

enum class FloatEnum
{
    kFloat = 0,  // This indicates that a page formatter is allowed to float the table to optimize the pagination. This
                 // is for example supported by TeX.
    kNoFloat
    = 1,  // This indicates that a page formatter is not allowed to float the object to optimize the pagination.
};

template < typename StringType >
bool FromString(const StringType& string, FloatEnum& value)
{
    if (std::strcmp(string.c_str(), "FLOAT") == 0) {
        value = FloatEnum::kFloat;
        return true;
    } else if (std::strcmp(string.c_str(), "NO-FLOAT") == 0) {
        value = FloatEnum::kNoFloat;
        return true;
    } else
        return false;
}

inline const char* ToString(FloatEnum value)
{
    switch (value) {
        case FloatEnum::kFloat:
            return "FLOAT";
        case FloatEnum::kNoFloat:
            return "NO-FLOAT";
        default:
            return nullptr;
    }
}

enum class FrameEnum
{
    kAll    = 0,  // Borders all around the table
    kBottom = 1,  // Border at the bottom of the table
    kNone   = 2,  // No borders around the table
    kSides  = 3,  // Borders at the sides of the table
    kTop    = 4,  // Border at the top of the table
    kTopbot = 5,  // Borders at the top and bottom of  the table
};

template < typename StringType >
bool FromString(const StringType& string, FrameEnum& value)
{
    if (std::strcmp(string.c_str(), "ALL") == 0) {
        value = FrameEnum::kAll;
        return true;
    } else if (std::strcmp(string.c_str(), "BOTTOM") == 0) {
        value = FrameEnum::kBottom;
        return true;
    } else if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = FrameEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "SIDES") == 0) {
        value = FrameEnum::kSides;
        return true;
    } else if (std::strcmp(string.c_str(), "TOP") == 0) {
        value = FrameEnum::kTop;
        return true;
    } else if (std::strcmp(string.c_str(), "TOPBOT") == 0) {
        value = FrameEnum::kTopbot;
        return true;
    } else
        return false;
}

inline const char* ToString(FrameEnum value)
{
    switch (value) {
        case FrameEnum::kAll:
            return "ALL";
        case FrameEnum::kBottom:
            return "BOTTOM";
        case FrameEnum::kNone:
            return "NONE";
        case FrameEnum::kSides:
            return "SIDES";
        case FrameEnum::kTop:
            return "TOP";
        case FrameEnum::kTopbot:
            return "TOPBOT";
        default:
            return nullptr;
    }
}

enum class FullBindingTimeEnum
{
    kBlueprintDerivationTime = 0,  // The point in time when an object is created from a blueprint.
    kSystemDesignTime = 1,  // * Designing the VFB. * Software Component types (PortInterfaces). * SWC Prototypes and
                            // the Connections between SWCprototypes. * Designing the Topology * ECUs and
                            // interconnecting Networks * Designing the Communication Matrix and Data Mapping
    kCodeGenerationTime
    = 2,  // * Coding by hand, based on requirements document. * Tool based code generation, e.g. from a model. * The
          // model may contain variants. * Only code for the selected variant(s) is actually generated.
    kPreCompileTime = 3,  // This is typically the C-Preprocessor. Exclude parts of the code from the compilation
                          // process, e.g., because they are not required for the selected variant, because they are
                          // incompatible with the selected variant, because they require resources that are not present
                          // in the selected variant. Object code is only generated for the selected variant(s). The
                          // code that is excluded at this stage code will not be available at later stages.
    kLinkTime = 4,   // Configure what is included in object code, and what is omitted Based on which variant(s) are
                     // selected E.g. for modules that are delivered as object code (as opposed to those that are
                     // delivered as source code)
    kPostBuild = 5,  // PostBuild is the binding time which is bound latest at startup of the ECU. In other words this
                     // is everything between creation of the executable program and startup of the ECU.
};

template < typename StringType >
bool FromString(const StringType& string, FullBindingTimeEnum& value)
{
    if (std::strcmp(string.c_str(), "BLUEPRINT-DERIVATION-TIME") == 0) {
        value = FullBindingTimeEnum::kBlueprintDerivationTime;
        return true;
    } else if (std::strcmp(string.c_str(), "SYSTEM-DESIGN-TIME") == 0) {
        value = FullBindingTimeEnum::kSystemDesignTime;
        return true;
    } else if (std::strcmp(string.c_str(), "CODE-GENERATION-TIME") == 0) {
        value = FullBindingTimeEnum::kCodeGenerationTime;
        return true;
    } else if (std::strcmp(string.c_str(), "PRE-COMPILE-TIME") == 0) {
        value = FullBindingTimeEnum::kPreCompileTime;
        return true;
    } else if (std::strcmp(string.c_str(), "LINK-TIME") == 0) {
        value = FullBindingTimeEnum::kLinkTime;
        return true;
    } else if (std::strcmp(string.c_str(), "POST-BUILD") == 0) {
        value = FullBindingTimeEnum::kPostBuild;
        return true;
    } else
        return false;
}

inline const char* ToString(FullBindingTimeEnum value)
{
    switch (value) {
        case FullBindingTimeEnum::kBlueprintDerivationTime:
            return "BLUEPRINT-DERIVATION-TIME";
        case FullBindingTimeEnum::kSystemDesignTime:
            return "SYSTEM-DESIGN-TIME";
        case FullBindingTimeEnum::kCodeGenerationTime:
            return "CODE-GENERATION-TIME";
        case FullBindingTimeEnum::kPreCompileTime:
            return "PRE-COMPILE-TIME";
        case FullBindingTimeEnum::kLinkTime:
            return "LINK-TIME";
        case FullBindingTimeEnum::kPostBuild:
            return "POST-BUILD";
        default:
            return nullptr;
    }
}

enum class GlobalTimeCrcSupportEnum
{
    kCrcNotSupported = 0,  // This indicates that CRC is not supported
    kCrcSupported    = 1,  // This indicates that CRC is supported
};

template < typename StringType >
bool FromString(const StringType& string, GlobalTimeCrcSupportEnum& value)
{
    if (std::strcmp(string.c_str(), "CRC-NOT-SUPPORTED") == 0) {
        value = GlobalTimeCrcSupportEnum::kCrcNotSupported;
        return true;
    } else if (std::strcmp(string.c_str(), "CRC-SUPPORTED") == 0) {
        value = GlobalTimeCrcSupportEnum::kCrcSupported;
        return true;
    } else
        return false;
}

inline const char* ToString(GlobalTimeCrcSupportEnum value)
{
    switch (value) {
        case GlobalTimeCrcSupportEnum::kCrcNotSupported:
            return "CRC-NOT-SUPPORTED";
        case GlobalTimeCrcSupportEnum::kCrcSupported:
            return "CRC-SUPPORTED";
        default:
            return nullptr;
    }
}

enum class GlobalTimeCrcValidationEnum
{
    kCrcIgnored      = 0,  // The CRC is supposed to be ignored
    kCrcNotValidated = 1,  // The CRC is not supposed to be present. If CRC is present the message is ignored.
    kCrcValidated    = 2,  // This CRC is supposed to be validated.
    kCrcOptional
    = 3,  // Either the CRC is present and then shall be validated or the CRC is not present and no CRC check is done.
};

template < typename StringType >
bool FromString(const StringType& string, GlobalTimeCrcValidationEnum& value)
{
    if (std::strcmp(string.c_str(), "CRC-IGNORED") == 0) {
        value = GlobalTimeCrcValidationEnum::kCrcIgnored;
        return true;
    } else if (std::strcmp(string.c_str(), "CRC-NOT-VALIDATED") == 0) {
        value = GlobalTimeCrcValidationEnum::kCrcNotValidated;
        return true;
    } else if (std::strcmp(string.c_str(), "CRC-VALIDATED") == 0) {
        value = GlobalTimeCrcValidationEnum::kCrcValidated;
        return true;
    } else if (std::strcmp(string.c_str(), "CRC-OPTIONAL") == 0) {
        value = GlobalTimeCrcValidationEnum::kCrcOptional;
        return true;
    } else
        return false;
}

inline const char* ToString(GlobalTimeCrcValidationEnum value)
{
    switch (value) {
        case GlobalTimeCrcValidationEnum::kCrcIgnored:
            return "CRC-IGNORED";
        case GlobalTimeCrcValidationEnum::kCrcNotValidated:
            return "CRC-NOT-VALIDATED";
        case GlobalTimeCrcValidationEnum::kCrcValidated:
            return "CRC-VALIDATED";
        case GlobalTimeCrcValidationEnum::kCrcOptional:
            return "CRC-OPTIONAL";
        default:
            return nullptr;
    }
}

enum class GraphicFitEnum
{
    kAsIs        = 0,  // This indicates that the image shall be incorporated as is without scaling, rotation etc.
    kFitToPage   = 1,  // Fit to the page
    kFitToText   = 2,  // fit to the text containing the graphic.
    kLimitToPage = 3,  // This indicates that the width of the graphic shall be limited to the '''page width'''. The
                       // image shall not be scaled down but cropped.
    kLimitToText = 4,  // This indicates that the width of the graphic shall be limited to the width of the current
                       // '''text flow'''. The image shall not be scaled down but cropped.
    kRotate180              = 5,   // Rotate 180 degree
    kRotate180LimitToText   = 6,   // Rotate 180 degree
    kRotate90Ccw            = 7,   // Rotate 90 degree counter clockwise
    kRotate90CcwFitToText   = 8,   // Rotate by 90 degree counter clock wise and then fit to text
    kRotate90CcwLimitToText = 9,   // Rotate by 90 degree counter clock wise and then fit to text
    kRotate90Cw             = 10,  // Rotate 90 degree clockwise
    kRotate90CwFitToText    = 11,  // Rotate by 90 degree and then fit to text
    kRotate90CwLimitToText  = 12,  // Rotate by 90 degree and then fit to text
};

template < typename StringType >
bool FromString(const StringType& string, GraphicFitEnum& value)
{
    if (std::strcmp(string.c_str(), "AS-IS") == 0) {
        value = GraphicFitEnum::kAsIs;
        return true;
    } else if (std::strcmp(string.c_str(), "FIT-TO-PAGE") == 0) {
        value = GraphicFitEnum::kFitToPage;
        return true;
    } else if (std::strcmp(string.c_str(), "FIT-TO-TEXT") == 0) {
        value = GraphicFitEnum::kFitToText;
        return true;
    } else if (std::strcmp(string.c_str(), "LIMIT-TO-PAGE") == 0) {
        value = GraphicFitEnum::kLimitToPage;
        return true;
    } else if (std::strcmp(string.c_str(), "LIMIT-TO-TEXT") == 0) {
        value = GraphicFitEnum::kLimitToText;
        return true;
    } else if (std::strcmp(string.c_str(), "ROTATE-180") == 0) {
        value = GraphicFitEnum::kRotate180;
        return true;
    } else if (std::strcmp(string.c_str(), "ROTATE-180-LIMIT-TO-TEXT") == 0) {
        value = GraphicFitEnum::kRotate180LimitToText;
        return true;
    } else if (std::strcmp(string.c_str(), "ROTATE-90-CCW") == 0) {
        value = GraphicFitEnum::kRotate90Ccw;
        return true;
    } else if (std::strcmp(string.c_str(), "ROTATE-90-CCW-FIT-TO-TEXT") == 0) {
        value = GraphicFitEnum::kRotate90CcwFitToText;
        return true;
    } else if (std::strcmp(string.c_str(), "ROTATE-90-CCW-LIMIT-TO-TEXT") == 0) {
        value = GraphicFitEnum::kRotate90CcwLimitToText;
        return true;
    } else if (std::strcmp(string.c_str(), "ROTATE-90-CW") == 0) {
        value = GraphicFitEnum::kRotate90Cw;
        return true;
    } else if (std::strcmp(string.c_str(), "ROTATE-90-CW-FIT-TO-TEXT") == 0) {
        value = GraphicFitEnum::kRotate90CwFitToText;
        return true;
    } else if (std::strcmp(string.c_str(), "ROTATE-90-CW-LIMIT-TO-TEXT") == 0) {
        value = GraphicFitEnum::kRotate90CwLimitToText;
        return true;
    } else
        return false;
}

inline const char* ToString(GraphicFitEnum value)
{
    switch (value) {
        case GraphicFitEnum::kAsIs:
            return "AS-IS";
        case GraphicFitEnum::kFitToPage:
            return "FIT-TO-PAGE";
        case GraphicFitEnum::kFitToText:
            return "FIT-TO-TEXT";
        case GraphicFitEnum::kLimitToPage:
            return "LIMIT-TO-PAGE";
        case GraphicFitEnum::kLimitToText:
            return "LIMIT-TO-TEXT";
        case GraphicFitEnum::kRotate180:
            return "ROTATE-180";
        case GraphicFitEnum::kRotate180LimitToText:
            return "ROTATE-180-LIMIT-TO-TEXT";
        case GraphicFitEnum::kRotate90Ccw:
            return "ROTATE-90-CCW";
        case GraphicFitEnum::kRotate90CcwFitToText:
            return "ROTATE-90-CCW-FIT-TO-TEXT";
        case GraphicFitEnum::kRotate90CcwLimitToText:
            return "ROTATE-90-CCW-LIMIT-TO-TEXT";
        case GraphicFitEnum::kRotate90Cw:
            return "ROTATE-90-CW";
        case GraphicFitEnum::kRotate90CwFitToText:
            return "ROTATE-90-CW-FIT-TO-TEXT";
        case GraphicFitEnum::kRotate90CwLimitToText:
            return "ROTATE-90-CW-LIMIT-TO-TEXT";
        default:
            return nullptr;
    }
}

enum class GraphicNotationEnum
{
    kBmp  = 0,  // bitmap image
    kEps  = 1,  // Encapsulated Postscript
    kGif  = 2,  // Graphics Interchange Format
    kJpg  = 3,  // "Joint Photographic Experts Group"  format
    kPdf  = 4,  // Portable Document Format
    kPng  = 5,  // Portable Network Graphics
    kSvg  = 6,  // scalable vector graphic
    kTiff = 7,  // Tagged Image File Format
};

template < typename StringType >
bool FromString(const StringType& string, GraphicNotationEnum& value)
{
    if (std::strcmp(string.c_str(), "BMP") == 0) {
        value = GraphicNotationEnum::kBmp;
        return true;
    } else if (std::strcmp(string.c_str(), "EPS") == 0) {
        value = GraphicNotationEnum::kEps;
        return true;
    } else if (std::strcmp(string.c_str(), "GIF") == 0) {
        value = GraphicNotationEnum::kGif;
        return true;
    } else if (std::strcmp(string.c_str(), "JPG") == 0) {
        value = GraphicNotationEnum::kJpg;
        return true;
    } else if (std::strcmp(string.c_str(), "PDF") == 0) {
        value = GraphicNotationEnum::kPdf;
        return true;
    } else if (std::strcmp(string.c_str(), "PNG") == 0) {
        value = GraphicNotationEnum::kPng;
        return true;
    } else if (std::strcmp(string.c_str(), "SVG") == 0) {
        value = GraphicNotationEnum::kSvg;
        return true;
    } else if (std::strcmp(string.c_str(), "TIFF") == 0) {
        value = GraphicNotationEnum::kTiff;
        return true;
    } else
        return false;
}

inline const char* ToString(GraphicNotationEnum value)
{
    switch (value) {
        case GraphicNotationEnum::kBmp:
            return "BMP";
        case GraphicNotationEnum::kEps:
            return "EPS";
        case GraphicNotationEnum::kGif:
            return "GIF";
        case GraphicNotationEnum::kJpg:
            return "JPG";
        case GraphicNotationEnum::kPdf:
            return "PDF";
        case GraphicNotationEnum::kPng:
            return "PNG";
        case GraphicNotationEnum::kSvg:
            return "SVG";
        case GraphicNotationEnum::kTiff:
            return "TIFF";
        default:
            return nullptr;
    }
}

enum class HandleInvalidEnum
{
    kDontInvalidate = 0,  // Invalidation is switched off.
    kExternalReplacement
    = 1,           // Replace a received invalidValue. The replacement value is sourced from the externalReplacement.
    kKeep = 2,     // The application software is supposed to  handle signal invalidation on RTE API level either by
                   // DataReceiveErrorEvent or check of error code on read access.
    kReplace = 3,  // Replace a received invalidValue. The replacement value is specified by the initValue.
};

template < typename StringType >
bool FromString(const StringType& string, HandleInvalidEnum& value)
{
    if (std::strcmp(string.c_str(), "DONT-INVALIDATE") == 0) {
        value = HandleInvalidEnum::kDontInvalidate;
        return true;
    } else if (std::strcmp(string.c_str(), "EXTERNAL-REPLACEMENT") == 0) {
        value = HandleInvalidEnum::kExternalReplacement;
        return true;
    } else if (std::strcmp(string.c_str(), "KEEP") == 0) {
        value = HandleInvalidEnum::kKeep;
        return true;
    } else if (std::strcmp(string.c_str(), "REPLACE") == 0) {
        value = HandleInvalidEnum::kReplace;
        return true;
    } else
        return false;
}

inline const char* ToString(HandleInvalidEnum value)
{
    switch (value) {
        case HandleInvalidEnum::kDontInvalidate:
            return "DONT-INVALIDATE";
        case HandleInvalidEnum::kExternalReplacement:
            return "EXTERNAL-REPLACEMENT";
        case HandleInvalidEnum::kKeep:
            return "KEEP";
        case HandleInvalidEnum::kReplace:
            return "REPLACE";
        default:
            return nullptr;
    }
}

enum class HandleOutOfRangeEnum
{
    kDefault             = 0,  // The RTE will use the initValue if the actual value is out of the specified bounds.
    kExternalReplacement = 1,  // This indicates that the value replacement is sourced from the attribute replaceWith.
    kIgnore = 2,    // The RTE will ignore any attempt to send or receive the corresponding dataElement if the value is
                    // out of the specified range.
    kInvalid  = 3,  // The RTE will use the invalidValue if the value is out of the specified bounds.
    kNone     = 4,  // A range check is not required.
    kSaturate = 5,  // The RTE will saturate the value of the dataElement such that it is limited to the applicable
                    // upper bound if it is greater than the upper bound. Consequently, it is limited to the applicable
                    // lower bound if the value is less than the lower bound.
};

template < typename StringType >
bool FromString(const StringType& string, HandleOutOfRangeEnum& value)
{
    if (std::strcmp(string.c_str(), "DEFAULT") == 0) {
        value = HandleOutOfRangeEnum::kDefault;
        return true;
    } else if (std::strcmp(string.c_str(), "EXTERNAL-REPLACEMENT") == 0) {
        value = HandleOutOfRangeEnum::kExternalReplacement;
        return true;
    } else if (std::strcmp(string.c_str(), "IGNORE") == 0) {
        value = HandleOutOfRangeEnum::kIgnore;
        return true;
    } else if (std::strcmp(string.c_str(), "INVALID") == 0) {
        value = HandleOutOfRangeEnum::kInvalid;
        return true;
    } else if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = HandleOutOfRangeEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "SATURATE") == 0) {
        value = HandleOutOfRangeEnum::kSaturate;
        return true;
    } else
        return false;
}

inline const char* ToString(HandleOutOfRangeEnum value)
{
    switch (value) {
        case HandleOutOfRangeEnum::kDefault:
            return "DEFAULT";
        case HandleOutOfRangeEnum::kExternalReplacement:
            return "EXTERNAL-REPLACEMENT";
        case HandleOutOfRangeEnum::kIgnore:
            return "IGNORE";
        case HandleOutOfRangeEnum::kInvalid:
            return "INVALID";
        case HandleOutOfRangeEnum::kNone:
            return "NONE";
        case HandleOutOfRangeEnum::kSaturate:
            return "SATURATE";
        default:
            return nullptr;
    }
}

enum class HandleOutOfRangeStatusEnum
{
    kIndicate = 0,  // The RTE sets the return status to RTE_E_OUT_OF_RANGE if the received value is out of range and
                    // the attribute handleOutOfRange is not set to "none" or "invalid".
    kSilent = 1,    // The RTE sets the return status to RTE_E_OK
};

template < typename StringType >
bool FromString(const StringType& string, HandleOutOfRangeStatusEnum& value)
{
    if (std::strcmp(string.c_str(), "INDICATE") == 0) {
        value = HandleOutOfRangeStatusEnum::kIndicate;
        return true;
    } else if (std::strcmp(string.c_str(), "SILENT") == 0) {
        value = HandleOutOfRangeStatusEnum::kSilent;
        return true;
    } else
        return false;
}

inline const char* ToString(HandleOutOfRangeStatusEnum value)
{
    switch (value) {
        case HandleOutOfRangeStatusEnum::kIndicate:
            return "INDICATE";
        case HandleOutOfRangeStatusEnum::kSilent:
            return "SILENT";
        default:
            return nullptr;
    }
}

enum class HandleTerminationAndRestartEnum
{
    kCanBeTerminated             = 0,  // Supports termination.
    kCanBeTerminatedAndRestarted = 1,  // Supports termination and restarting.
    kNoSupport                   = 2,  // Stop and restart is not supported at all.
};

template < typename StringType >
bool FromString(const StringType& string, HandleTerminationAndRestartEnum& value)
{
    if (std::strcmp(string.c_str(), "CAN-BE-TERMINATED") == 0) {
        value = HandleTerminationAndRestartEnum::kCanBeTerminated;
        return true;
    } else if (std::strcmp(string.c_str(), "CAN-BE-TERMINATED-AND-RESTARTED") == 0) {
        value = HandleTerminationAndRestartEnum::kCanBeTerminatedAndRestarted;
        return true;
    } else if (std::strcmp(string.c_str(), "NO-SUPPORT") == 0) {
        value = HandleTerminationAndRestartEnum::kNoSupport;
        return true;
    } else
        return false;
}

inline const char* ToString(HandleTerminationAndRestartEnum value)
{
    switch (value) {
        case HandleTerminationAndRestartEnum::kCanBeTerminated:
            return "CAN-BE-TERMINATED";
        case HandleTerminationAndRestartEnum::kCanBeTerminatedAndRestarted:
            return "CAN-BE-TERMINATED-AND-RESTARTED";
        case HandleTerminationAndRestartEnum::kNoSupport:
            return "NO-SUPPORT";
        default:
            return nullptr;
    }
}

enum class HandleTimeoutEnum
{
    kNone    = 0,  // If set to none no replacement shall take place.
    kReplace = 1,  // If set to replace, the replacement value shall be the  ComInitValue.
    kReplaceByTimeoutSubstitutionValue
    = 2,  // If set to replace, the replacement value shall be the timeout substitution value.
};

template < typename StringType >
bool FromString(const StringType& string, HandleTimeoutEnum& value)
{
    if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = HandleTimeoutEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "REPLACE") == 0) {
        value = HandleTimeoutEnum::kReplace;
        return true;
    } else if (std::strcmp(string.c_str(), "REPLACE-BY-TIMEOUT-SUBSTITUTION-VALUE") == 0) {
        value = HandleTimeoutEnum::kReplaceByTimeoutSubstitutionValue;
        return true;
    } else
        return false;
}

inline const char* ToString(HandleTimeoutEnum value)
{
    switch (value) {
        case HandleTimeoutEnum::kNone:
            return "NONE";
        case HandleTimeoutEnum::kReplace:
            return "REPLACE";
        case HandleTimeoutEnum::kReplaceByTimeoutSubstitutionValue:
            return "REPLACE-BY-TIMEOUT-SUBSTITUTION-VALUE";
        default:
            return nullptr;
    }
}

enum class HttpAcceptEncodingEnum
{
    kGzip    = 0,  // Use gzip pcompression.
    kDeflate = 1,  // Use deflate compression.
};

template < typename StringType >
bool FromString(const StringType& string, HttpAcceptEncodingEnum& value)
{
    if (std::strcmp(string.c_str(), "GZIP") == 0) {
        value = HttpAcceptEncodingEnum::kGzip;
        return true;
    } else if (std::strcmp(string.c_str(), "DEFLATE") == 0) {
        value = HttpAcceptEncodingEnum::kDeflate;
        return true;
    } else
        return false;
}

inline const char* ToString(HttpAcceptEncodingEnum value)
{
    switch (value) {
        case HttpAcceptEncodingEnum::kGzip:
            return "GZIP";
        case HttpAcceptEncodingEnum::kDeflate:
            return "DEFLATE";
        default:
            return nullptr;
    }
}

enum class IPduSignalProcessingEnum
{
    kDeferred  = 0,  // The signal indications / confirmations are deferred.
    kImmediate = 1,  // The signal indications / confirmations are performed.
};

template < typename StringType >
bool FromString(const StringType& string, IPduSignalProcessingEnum& value)
{
    if (std::strcmp(string.c_str(), "DEFERRED") == 0) {
        value = IPduSignalProcessingEnum::kDeferred;
        return true;
    } else if (std::strcmp(string.c_str(), "IMMEDIATE") == 0) {
        value = IPduSignalProcessingEnum::kImmediate;
        return true;
    } else
        return false;
}

inline const char* ToString(IPduSignalProcessingEnum value)
{
    switch (value) {
        case IPduSignalProcessingEnum::kDeferred:
            return "DEFERRED";
        case IPduSignalProcessingEnum::kImmediate:
            return "IMMEDIATE";
        default:
            return nullptr;
    }
}

enum class IPsecDpdActionEnum
{
    kClear   = 0,  // Deletes the SA.
    kTrap    = 1,  // tries to establish the connection after traffic is sent to the peer.
    kRestart = 2,  // Immediately tries to establish the connection.
};

template < typename StringType >
bool FromString(const StringType& string, IPsecDpdActionEnum& value)
{
    if (std::strcmp(string.c_str(), "CLEAR") == 0) {
        value = IPsecDpdActionEnum::kClear;
        return true;
    } else if (std::strcmp(string.c_str(), "TRAP") == 0) {
        value = IPsecDpdActionEnum::kTrap;
        return true;
    } else if (std::strcmp(string.c_str(), "RESTART") == 0) {
        value = IPsecDpdActionEnum::kRestart;
        return true;
    } else
        return false;
}

inline const char* ToString(IPsecDpdActionEnum value)
{
    switch (value) {
        case IPsecDpdActionEnum::kClear:
            return "CLEAR";
        case IPsecDpdActionEnum::kTrap:
            return "TRAP";
        case IPsecDpdActionEnum::kRestart:
            return "RESTART";
        default:
            return nullptr;
    }
}

enum class IPsecHeaderTypeEnum
{
    kAh   = 0,  // Authentication Header (AH)
    kEsp  = 1,  // Encapsulating Security Payloads (ESP)
    kNone = 2,  // No header
};

template < typename StringType >
bool FromString(const StringType& string, IPsecHeaderTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "AH") == 0) {
        value = IPsecHeaderTypeEnum::kAh;
        return true;
    } else if (std::strcmp(string.c_str(), "ESP") == 0) {
        value = IPsecHeaderTypeEnum::kEsp;
        return true;
    } else if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = IPsecHeaderTypeEnum::kNone;
        return true;
    } else
        return false;
}

inline const char* ToString(IPsecHeaderTypeEnum value)
{
    switch (value) {
        case IPsecHeaderTypeEnum::kAh:
            return "AH";
        case IPsecHeaderTypeEnum::kEsp:
            return "ESP";
        case IPsecHeaderTypeEnum::kNone:
            return "NONE";
        default:
            return nullptr;
    }
}

enum class IPsecIpProtocolEnum
{
    kUdp  = 0,  // UDP Protocol
    kTcp  = 1,  // TCP Protocol
    kIcmp = 2,  // Internet Control Message Protocol (ICMP)
    kAny  = 3,  // ANY protocol
};

template < typename StringType >
bool FromString(const StringType& string, IPsecIpProtocolEnum& value)
{
    if (std::strcmp(string.c_str(), "UDP") == 0) {
        value = IPsecIpProtocolEnum::kUdp;
        return true;
    } else if (std::strcmp(string.c_str(), "TCP") == 0) {
        value = IPsecIpProtocolEnum::kTcp;
        return true;
    } else if (std::strcmp(string.c_str(), "ICMP") == 0) {
        value = IPsecIpProtocolEnum::kIcmp;
        return true;
    } else if (std::strcmp(string.c_str(), "ANY") == 0) {
        value = IPsecIpProtocolEnum::kAny;
        return true;
    } else
        return false;
}

inline const char* ToString(IPsecIpProtocolEnum value)
{
    switch (value) {
        case IPsecIpProtocolEnum::kUdp:
            return "UDP";
        case IPsecIpProtocolEnum::kTcp:
            return "TCP";
        case IPsecIpProtocolEnum::kIcmp:
            return "ICMP";
        case IPsecIpProtocolEnum::kAny:
            return "ANY";
        default:
            return nullptr;
    }
}

enum class IPsecModeEnum
{
    kTunnel = 0,  // Signifying that the IPSec tunnel mode is used. With tunnel mode, the entire original IP packet is
                  // protected by IPSec. This means IPSec wraps the original packet, encrypts it, adds a new IP header
                  // and sends it to the other side.
    kTransport = 1,  // Signifying that the IPSec transport mode is used. With the transport mode the original IP header
                     // is retained and only the IP payload and ESP trailer is encrypted.
};

template < typename StringType >
bool FromString(const StringType& string, IPsecModeEnum& value)
{
    if (std::strcmp(string.c_str(), "TUNNEL") == 0) {
        value = IPsecModeEnum::kTunnel;
        return true;
    } else if (std::strcmp(string.c_str(), "TRANSPORT") == 0) {
        value = IPsecModeEnum::kTransport;
        return true;
    } else
        return false;
}

inline const char* ToString(IPsecModeEnum value)
{
    switch (value) {
        case IPsecModeEnum::kTunnel:
            return "TUNNEL";
        case IPsecModeEnum::kTransport:
            return "TRANSPORT";
        default:
            return nullptr;
    }
}

enum class IPsecPolicyEnum
{
    kIpsec       = 1,  // Signifying that packets should be protected.
    kPassthrough = 2,  // Signifying that no IPsec processing should be done at all.
    kDrop        = 3,  // Signifying that packets should be discarded
    kReject      = 4,  // Signifying that packets should be discarded and a diagnostic ICMP returned.
};

template < typename StringType >
bool FromString(const StringType& string, IPsecPolicyEnum& value)
{
    if (std::strcmp(string.c_str(), "IPSEC") == 0) {
        value = IPsecPolicyEnum::kIpsec;
        return true;
    } else if (std::strcmp(string.c_str(), "PASSTHROUGH") == 0) {
        value = IPsecPolicyEnum::kPassthrough;
        return true;
    } else if (std::strcmp(string.c_str(), "DROP") == 0) {
        value = IPsecPolicyEnum::kDrop;
        return true;
    } else if (std::strcmp(string.c_str(), "REJECT") == 0) {
        value = IPsecPolicyEnum::kReject;
        return true;
    } else
        return false;
}

inline const char* ToString(IPsecPolicyEnum value)
{
    switch (value) {
        case IPsecPolicyEnum::kIpsec:
            return "IPSEC";
        case IPsecPolicyEnum::kPassthrough:
            return "PASSTHROUGH";
        case IPsecPolicyEnum::kDrop:
            return "DROP";
        case IPsecPolicyEnum::kReject:
            return "REJECT";
        default:
            return nullptr;
    }
}

enum class ISignalTypeEnum
{
    kArray     = 0,  // ISignal shall be interpreted as an array (UINT8_N, UINT8_DYN)
    kPrimitive = 1,  // ISignal shall be interpreted as a primitive type (e.g. UINT_8, SINT_32)
};

template < typename StringType >
bool FromString(const StringType& string, ISignalTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "ARRAY") == 0) {
        value = ISignalTypeEnum::kArray;
        return true;
    } else if (std::strcmp(string.c_str(), "PRIMITIVE") == 0) {
        value = ISignalTypeEnum::kPrimitive;
        return true;
    } else
        return false;
}

inline const char* ToString(ISignalTypeEnum value)
{
    switch (value) {
        case ISignalTypeEnum::kArray:
            return "ARRAY";
        case ISignalTypeEnum::kPrimitive:
            return "PRIMITIVE";
        default:
            return nullptr;
    }
}

enum class IkeAuthenticationMethodEnum
{
    kPsk = 1,  // Pre-shared key authentication
    kDsa = 2,  // Digital Signature Authentication
};

template < typename StringType >
bool FromString(const StringType& string, IkeAuthenticationMethodEnum& value)
{
    if (std::strcmp(string.c_str(), "PSK") == 0) {
        value = IkeAuthenticationMethodEnum::kPsk;
        return true;
    } else if (std::strcmp(string.c_str(), "DSA") == 0) {
        value = IkeAuthenticationMethodEnum::kDsa;
        return true;
    } else
        return false;
}

inline const char* ToString(IkeAuthenticationMethodEnum value)
{
    switch (value) {
        case IkeAuthenticationMethodEnum::kPsk:
            return "PSK";
        case IkeAuthenticationMethodEnum::kDsa:
            return "DSA";
        default:
            return nullptr;
    }
}

enum class IntervalTypeEnum
{
    kClosed = 0,  // The area is limited by the value given. The value itself is included.
    kInfinite
    = 1,  // This indicates that the limit is infinite. Note, it is obsolete. Use INF / -INF as value of the limit.
    kOpen = 2,  // The area is limited by the value given. The value itself is not included.
};

template < typename StringType >
bool FromString(const StringType& string, IntervalTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "CLOSED") == 0) {
        value = IntervalTypeEnum::kClosed;
        return true;
    } else if (std::strcmp(string.c_str(), "INFINITE") == 0) {
        value = IntervalTypeEnum::kInfinite;
        return true;
    } else if (std::strcmp(string.c_str(), "OPEN") == 0) {
        value = IntervalTypeEnum::kOpen;
        return true;
    } else
        return false;
}

inline const char* ToString(IntervalTypeEnum value)
{
    switch (value) {
        case IntervalTypeEnum::kClosed:
            return "CLOSED";
        case IntervalTypeEnum::kInfinite:
            return "INFINITE";
        case IntervalTypeEnum::kOpen:
            return "OPEN";
        default:
            return nullptr;
    }
}

enum class IpAddressKeepEnum
{
    kForget            = 0,  // After a dynamic IP address has been assigned just use it for this session.
    kStorePersistently = 1,  // After a dynamic IP address has been assigned store the address persistently.
};

template < typename StringType >
bool FromString(const StringType& string, IpAddressKeepEnum& value)
{
    if (std::strcmp(string.c_str(), "FORGET") == 0) {
        value = IpAddressKeepEnum::kForget;
        return true;
    } else if (std::strcmp(string.c_str(), "STORE-PERSISTENTLY") == 0) {
        value = IpAddressKeepEnum::kStorePersistently;
        return true;
    } else
        return false;
}

inline const char* ToString(IpAddressKeepEnum value)
{
    switch (value) {
        case IpAddressKeepEnum::kForget:
            return "FORGET";
        case IpAddressKeepEnum::kStorePersistently:
            return "STORE-PERSISTENTLY";
        default:
            return nullptr;
    }
}

enum class IpTransportProtocolEnum
{
    kUdp = 0,  // User Datagram Protocol (UDP).
    kTcp = 1,  // Transmission Control Protocol (TCP)
};

template < typename StringType >
bool FromString(const StringType& string, IpTransportProtocolEnum& value)
{
    if (std::strcmp(string.c_str(), "UDP") == 0) {
        value = IpTransportProtocolEnum::kUdp;
        return true;
    } else if (std::strcmp(string.c_str(), "TCP") == 0) {
        value = IpTransportProtocolEnum::kTcp;
        return true;
    } else
        return false;
}

inline const char* ToString(IpTransportProtocolEnum value)
{
    switch (value) {
        case IpTransportProtocolEnum::kUdp:
            return "UDP";
        case IpTransportProtocolEnum::kTcp:
            return "TCP";
        default:
            return nullptr;
    }
}

enum class Ipv4AddressSourceEnum
{
    kAutoIp       = 0,  // AutoIP is used to dynamically assign IP addresses at device startup.
    kAutoIpdhcpv4 = 1,  // This enum literal is deprecated and will be removed in future.  Old description:  The
                        // IpAddress is declared via AutoIp or dhcp.
    kAutoIpDoip = 2,    // Linklocal IPv4 Address Assignment using DoIP Parameters
    kDhcpv4     = 3,    // DHCP is a service for the automatic IP configuration of a client.
    kFixed      = 4,    // The IP Address shall be declared manually.
};

template < typename StringType >
bool FromString(const StringType& string, Ipv4AddressSourceEnum& value)
{
    if (std::strcmp(string.c_str(), "AUTO-IP") == 0) {
        value = Ipv4AddressSourceEnum::kAutoIp;
        return true;
    } else if (std::strcmp(string.c_str(), "AUTO-IPDHCPV-4") == 0) {
        value = Ipv4AddressSourceEnum::kAutoIpdhcpv4;
        return true;
    } else if (std::strcmp(string.c_str(), "AUTO-IP--DOIP") == 0) {
        value = Ipv4AddressSourceEnum::kAutoIpDoip;
        return true;
    } else if (std::strcmp(string.c_str(), "DHCPV-4") == 0) {
        value = Ipv4AddressSourceEnum::kDhcpv4;
        return true;
    } else if (std::strcmp(string.c_str(), "FIXED") == 0) {
        value = Ipv4AddressSourceEnum::kFixed;
        return true;
    } else
        return false;
}

inline const char* ToString(Ipv4AddressSourceEnum value)
{
    switch (value) {
        case Ipv4AddressSourceEnum::kAutoIp:
            return "AUTO-IP";
        case Ipv4AddressSourceEnum::kAutoIpdhcpv4:
            return "AUTO-IPDHCPV-4";
        case Ipv4AddressSourceEnum::kAutoIpDoip:
            return "AUTO-IP--DOIP";
        case Ipv4AddressSourceEnum::kDhcpv4:
            return "DHCPV-4";
        case Ipv4AddressSourceEnum::kFixed:
            return "FIXED";
        default:
            return nullptr;
    }
}

enum class Ipv6AddressSourceEnum
{
    kDhcpv6    = 0,  // DHCP is a service for the automatic IP configuration of a client.
    kFixed     = 1,  // The IP Address shall be declared manually.
    kLinkLocal = 2,  // LinkLocal is intended only for communications within the segment of a local network (a link) or
                     // a point-to-point connection that a host is connected to.
    kLinkLocalDoip       = 3,  // Linklocal IPv6 Address Assignment using DoIP Parameters
    kRouterAdvertisement = 4,  // IPv6 Stateless Autoconfiguration.
};

template < typename StringType >
bool FromString(const StringType& string, Ipv6AddressSourceEnum& value)
{
    if (std::strcmp(string.c_str(), "DHCPV-6") == 0) {
        value = Ipv6AddressSourceEnum::kDhcpv6;
        return true;
    } else if (std::strcmp(string.c_str(), "FIXED") == 0) {
        value = Ipv6AddressSourceEnum::kFixed;
        return true;
    } else if (std::strcmp(string.c_str(), "LINK-LOCAL") == 0) {
        value = Ipv6AddressSourceEnum::kLinkLocal;
        return true;
    } else if (std::strcmp(string.c_str(), "LINK-LOCAL--DOIP") == 0) {
        value = Ipv6AddressSourceEnum::kLinkLocalDoip;
        return true;
    } else if (std::strcmp(string.c_str(), "ROUTER-ADVERTISEMENT") == 0) {
        value = Ipv6AddressSourceEnum::kRouterAdvertisement;
        return true;
    } else
        return false;
}

inline const char* ToString(Ipv6AddressSourceEnum value)
{
    switch (value) {
        case Ipv6AddressSourceEnum::kDhcpv6:
            return "DHCPV-6";
        case Ipv6AddressSourceEnum::kFixed:
            return "FIXED";
        case Ipv6AddressSourceEnum::kLinkLocal:
            return "LINK-LOCAL";
        case Ipv6AddressSourceEnum::kLinkLocalDoip:
            return "LINK-LOCAL--DOIP";
        case Ipv6AddressSourceEnum::kRouterAdvertisement:
            return "ROUTER-ADVERTISEMENT";
        default:
            return nullptr;
    }
}

enum class ItemLabelPosEnum
{
    kNewline            = 0,  // The label is renders in a new line.
    kNewlineIfNecessary = 1,  // The label is rendered in a new line if it is longer than the indentation.
    kNoNewline = 2,  // The label is rendered in one line with the item even if it is longer than the indentation.
};

template < typename StringType >
bool FromString(const StringType& string, ItemLabelPosEnum& value)
{
    if (std::strcmp(string.c_str(), "NEWLINE") == 0) {
        value = ItemLabelPosEnum::kNewline;
        return true;
    } else if (std::strcmp(string.c_str(), "NEWLINE-IF-NECESSARY") == 0) {
        value = ItemLabelPosEnum::kNewlineIfNecessary;
        return true;
    } else if (std::strcmp(string.c_str(), "NO-NEWLINE") == 0) {
        value = ItemLabelPosEnum::kNoNewline;
        return true;
    } else
        return false;
}

inline const char* ToString(ItemLabelPosEnum value)
{
    switch (value) {
        case ItemLabelPosEnum::kNewline:
            return "NEWLINE";
        case ItemLabelPosEnum::kNewlineIfNecessary:
            return "NEWLINE-IF-NECESSARY";
        case ItemLabelPosEnum::kNoNewline:
            return "NO-NEWLINE";
        default:
            return nullptr;
    }
}

enum class KeepWithPreviousEnum
{
    kKeep   = 0,  // This indicates that the block shall be kept together with the previous block.
    kNoKeep = 1,  // This indicates that there is no need to keep the block with the previous one. This is the same as
                  // if the attribute itself is missing.
};

template < typename StringType >
bool FromString(const StringType& string, KeepWithPreviousEnum& value)
{
    if (std::strcmp(string.c_str(), "KEEP") == 0) {
        value = KeepWithPreviousEnum::kKeep;
        return true;
    } else if (std::strcmp(string.c_str(), "NO-KEEP") == 0) {
        value = KeepWithPreviousEnum::kNoKeep;
        return true;
    } else
        return false;
}

inline const char* ToString(KeepWithPreviousEnum value)
{
    switch (value) {
        case KeepWithPreviousEnum::kKeep:
            return "KEEP";
        case KeepWithPreviousEnum::kNoKeep:
            return "NO-KEEP";
        default:
            return nullptr;
    }
}

enum class KeyUsageRestrictionEnum
{
    kGenerate          = 0,
    kGenerateAndVerify = 1,
    kVerify            = 2,
};

template < typename StringType >
bool FromString(const StringType& string, KeyUsageRestrictionEnum& value)
{
    if (std::strcmp(string.c_str(), "GENERATE") == 0) {
        value = KeyUsageRestrictionEnum::kGenerate;
        return true;
    } else if (std::strcmp(string.c_str(), "GENERATE-AND-VERIFY") == 0) {
        value = KeyUsageRestrictionEnum::kGenerateAndVerify;
        return true;
    } else if (std::strcmp(string.c_str(), "VERIFY") == 0) {
        value = KeyUsageRestrictionEnum::kVerify;
        return true;
    } else
        return false;
}

inline const char* ToString(KeyUsageRestrictionEnum value)
{
    switch (value) {
        case KeyUsageRestrictionEnum::kGenerate:
            return "GENERATE";
        case KeyUsageRestrictionEnum::kGenerateAndVerify:
            return "GENERATE-AND-VERIFY";
        case KeyUsageRestrictionEnum::kVerify:
            return "VERIFY";
        default:
            return nullptr;
    }
}

enum class LEnum
{
    kAa     = 0,    // Afar
    kAb     = 1,    // Abkhazian
    kAf     = 2,    // Afrikaans
    kAm     = 3,    // Amharic
    kAr     = 4,    // Arabic
    kAs     = 5,    // Assamese
    kAy     = 6,    // Aymara
    kAz     = 7,    // Azerbaijani
    kBa     = 8,    // Bashkir
    kBe     = 9,    // Byelorussian
    kBg     = 10,   // Bulgarian
    kBh     = 11,   // Bihari
    kBi     = 12,   // Bislama
    kBn     = 13,   // Bengali
    kBo     = 14,   // Tibetian
    kBr     = 15,   // Breton
    kCa     = 16,   // Catalan
    kCo     = 17,   // Corsican
    kCs     = 18,   // Czech
    kCy     = 19,   // Welsh
    kDa     = 20,   // Danish
    kDe     = 21,   // German
    kDz     = 22,   // Bhutani
    kEl     = 23,   // Greek
    kEn     = 24,   // English
    kEo     = 25,   // Esperanto
    kEs     = 26,   // Spanish
    kEt     = 27,   // Estonian
    kEu     = 28,   // Basque
    kFa     = 29,   // Persian
    kFi     = 30,   // Finnish
    kFj     = 31,   // Fiji
    kFo     = 32,   // Faeroese
    kForAll = 33,   // The content applies to all languages
    kFr     = 34,   // French
    kFy     = 35,   // Frisian
    kGa     = 36,   // Irish
    kGd     = 37,   // Scots Gaelic
    kGl     = 38,   // Galician
    kGn     = 39,   // Guarani
    kGu     = 40,   // Gjarati
    kHa     = 41,   // Hausa
    kHi     = 42,   // Hindi
    kHr     = 43,   // Croatian
    kHu     = 44,   // Hungarian
    kHy     = 45,   // Armenian
    kIa     = 46,   // Interlingua
    kIe     = 47,   // Interlingue
    kIk     = 48,   // Inupiak
    kIn     = 49,   // Indonesian
    kIs     = 50,   // Icelandic
    kIt     = 51,   // Italian
    kIw     = 52,   // Hebrew
    kJa     = 53,   // Japanese
    kJi     = 54,   // Yiddish
    kJw     = 55,   // Javanese
    kKa     = 56,   // Georgian
    kKk     = 57,   // Kazakh
    kKl     = 58,   // Greenlandic
    kKm     = 59,   // Cambodian
    kKn     = 60,   // Kannada
    kKo     = 61,   // Korean
    kKs     = 62,   // Kashmiri
    kKu     = 63,   // Kurdish
    kKy     = 64,   // Kirghiz
    kLa     = 65,   // Latin
    kLn     = 66,   // Lingala
    kLo     = 67,   // Laothian
    kLt     = 68,   // Lithuanian
    kLv     = 69,   // Lavian, Lettish
    kMg     = 70,   // Malagasy
    kMi     = 71,   // Maori
    kMk     = 72,   // Macedonian
    kMl     = 73,   // Malayalam
    kMn     = 74,   // Mongolian
    kMo     = 75,   // Moldavian
    kMr     = 76,   // Marathi
    kMs     = 77,   // Malay
    kMt     = 78,   // Maltese
    kMy     = 79,   // Burmese
    kNa     = 80,   // Nauru
    kNe     = 81,   // Nepali
    kNl     = 82,   // Dutch
    kNo     = 83,   // Norwegian
    kOc     = 84,   // Occitan
    kOm     = 85,   // (Afan) Oromo
    kOr     = 86,   // Oriya
    kPa     = 87,   // Punjabi
    kPl     = 88,   // Polish
    kPs     = 89,   // Pashto, Pushto
    kPt     = 90,   // Portuguese
    kQu     = 91,   // Quechua
    kRm     = 92,   // Rhaeto-Romance
    kRn     = 93,   // Kirundi
    kRo     = 94,   // Romanian
    kRu     = 95,   // Russian
    kRw     = 96,   // Kinyarwanda
    kSa     = 97,   // Sanskrit
    kSd     = 98,   // Sindhi
    kSg     = 99,   // Sangro
    kSh     = 100,  // Serbo-Croatian
    kSi     = 101,  // Singhalese
    kSk     = 102,  // Slovak
    kSl     = 103,  // Slovenian
    kSm     = 104,  // Samoan
    kSn     = 105,  // Shona
    kSo     = 106,  // Somali
    kSq     = 107,  // Albanian
    kSr     = 108,  // Serbian
    kSs     = 109,  // Siswati
    kSt     = 110,  // Sesotho
    kSu     = 111,  // Sundanese
    kSv     = 112,  // Swedish
    kSw     = 113,  // Swahili
    kTa     = 114,  // Tamil
    kTe     = 115,  // Tegulu
    kTg     = 116,  // Tajik
    kTh     = 117,  // Thai
    kTi     = 118,  // Tigrinya
    kTk     = 119,  // Turkmen
    kTl     = 120,  // Tagalog
    kTn     = 121,  // Setswana
    kTo     = 122,  // Tonga
    kTr     = 123,  // Turkish
    kTs     = 124,  // Tsonga
    kTt     = 125,  // Tatar
    kTw     = 126,  // Twi
    kUk     = 127,  // Ukrainian
    kUr     = 128,  // Urdu
    kUz     = 129,  // Uzbek
    kVi     = 130,  // Vietnamese
    kVo     = 131,  // Volapuk
    kWo     = 132,  // Wolof
    kXh     = 133,  // Xhosa
    kYo     = 134,  // Yoruba
    kZh     = 135,  // Chinese
    kZu     = 136,  // Zulu
};

template < typename StringType >
bool FromString(const StringType& string, LEnum& value)
{
    if (std::strcmp(string.c_str(), "AA") == 0) {
        value = LEnum::kAa;
        return true;
    } else if (std::strcmp(string.c_str(), "AB") == 0) {
        value = LEnum::kAb;
        return true;
    } else if (std::strcmp(string.c_str(), "AF") == 0) {
        value = LEnum::kAf;
        return true;
    } else if (std::strcmp(string.c_str(), "AM") == 0) {
        value = LEnum::kAm;
        return true;
    } else if (std::strcmp(string.c_str(), "AR") == 0) {
        value = LEnum::kAr;
        return true;
    } else if (std::strcmp(string.c_str(), "AS") == 0) {
        value = LEnum::kAs;
        return true;
    } else if (std::strcmp(string.c_str(), "AY") == 0) {
        value = LEnum::kAy;
        return true;
    } else if (std::strcmp(string.c_str(), "AZ") == 0) {
        value = LEnum::kAz;
        return true;
    } else if (std::strcmp(string.c_str(), "BA") == 0) {
        value = LEnum::kBa;
        return true;
    } else if (std::strcmp(string.c_str(), "BE") == 0) {
        value = LEnum::kBe;
        return true;
    } else if (std::strcmp(string.c_str(), "BG") == 0) {
        value = LEnum::kBg;
        return true;
    } else if (std::strcmp(string.c_str(), "BH") == 0) {
        value = LEnum::kBh;
        return true;
    } else if (std::strcmp(string.c_str(), "BI") == 0) {
        value = LEnum::kBi;
        return true;
    } else if (std::strcmp(string.c_str(), "BN") == 0) {
        value = LEnum::kBn;
        return true;
    } else if (std::strcmp(string.c_str(), "BO") == 0) {
        value = LEnum::kBo;
        return true;
    } else if (std::strcmp(string.c_str(), "BR") == 0) {
        value = LEnum::kBr;
        return true;
    } else if (std::strcmp(string.c_str(), "CA") == 0) {
        value = LEnum::kCa;
        return true;
    } else if (std::strcmp(string.c_str(), "CO") == 0) {
        value = LEnum::kCo;
        return true;
    } else if (std::strcmp(string.c_str(), "CS") == 0) {
        value = LEnum::kCs;
        return true;
    } else if (std::strcmp(string.c_str(), "CY") == 0) {
        value = LEnum::kCy;
        return true;
    } else if (std::strcmp(string.c_str(), "DA") == 0) {
        value = LEnum::kDa;
        return true;
    } else if (std::strcmp(string.c_str(), "DE") == 0) {
        value = LEnum::kDe;
        return true;
    } else if (std::strcmp(string.c_str(), "DZ") == 0) {
        value = LEnum::kDz;
        return true;
    } else if (std::strcmp(string.c_str(), "EL") == 0) {
        value = LEnum::kEl;
        return true;
    } else if (std::strcmp(string.c_str(), "EN") == 0) {
        value = LEnum::kEn;
        return true;
    } else if (std::strcmp(string.c_str(), "EO") == 0) {
        value = LEnum::kEo;
        return true;
    } else if (std::strcmp(string.c_str(), "ES") == 0) {
        value = LEnum::kEs;
        return true;
    } else if (std::strcmp(string.c_str(), "ET") == 0) {
        value = LEnum::kEt;
        return true;
    } else if (std::strcmp(string.c_str(), "EU") == 0) {
        value = LEnum::kEu;
        return true;
    } else if (std::strcmp(string.c_str(), "FA") == 0) {
        value = LEnum::kFa;
        return true;
    } else if (std::strcmp(string.c_str(), "FI") == 0) {
        value = LEnum::kFi;
        return true;
    } else if (std::strcmp(string.c_str(), "FJ") == 0) {
        value = LEnum::kFj;
        return true;
    } else if (std::strcmp(string.c_str(), "FO") == 0) {
        value = LEnum::kFo;
        return true;
    } else if (std::strcmp(string.c_str(), "FOR-ALL") == 0) {
        value = LEnum::kForAll;
        return true;
    } else if (std::strcmp(string.c_str(), "FR") == 0) {
        value = LEnum::kFr;
        return true;
    } else if (std::strcmp(string.c_str(), "FY") == 0) {
        value = LEnum::kFy;
        return true;
    } else if (std::strcmp(string.c_str(), "GA") == 0) {
        value = LEnum::kGa;
        return true;
    } else if (std::strcmp(string.c_str(), "GD") == 0) {
        value = LEnum::kGd;
        return true;
    } else if (std::strcmp(string.c_str(), "GL") == 0) {
        value = LEnum::kGl;
        return true;
    } else if (std::strcmp(string.c_str(), "GN") == 0) {
        value = LEnum::kGn;
        return true;
    } else if (std::strcmp(string.c_str(), "GU") == 0) {
        value = LEnum::kGu;
        return true;
    } else if (std::strcmp(string.c_str(), "HA") == 0) {
        value = LEnum::kHa;
        return true;
    } else if (std::strcmp(string.c_str(), "HI") == 0) {
        value = LEnum::kHi;
        return true;
    } else if (std::strcmp(string.c_str(), "HR") == 0) {
        value = LEnum::kHr;
        return true;
    } else if (std::strcmp(string.c_str(), "HU") == 0) {
        value = LEnum::kHu;
        return true;
    } else if (std::strcmp(string.c_str(), "HY") == 0) {
        value = LEnum::kHy;
        return true;
    } else if (std::strcmp(string.c_str(), "IA") == 0) {
        value = LEnum::kIa;
        return true;
    } else if (std::strcmp(string.c_str(), "IE") == 0) {
        value = LEnum::kIe;
        return true;
    } else if (std::strcmp(string.c_str(), "IK") == 0) {
        value = LEnum::kIk;
        return true;
    } else if (std::strcmp(string.c_str(), "IN") == 0) {
        value = LEnum::kIn;
        return true;
    } else if (std::strcmp(string.c_str(), "IS") == 0) {
        value = LEnum::kIs;
        return true;
    } else if (std::strcmp(string.c_str(), "IT") == 0) {
        value = LEnum::kIt;
        return true;
    } else if (std::strcmp(string.c_str(), "IW") == 0) {
        value = LEnum::kIw;
        return true;
    } else if (std::strcmp(string.c_str(), "JA") == 0) {
        value = LEnum::kJa;
        return true;
    } else if (std::strcmp(string.c_str(), "JI") == 0) {
        value = LEnum::kJi;
        return true;
    } else if (std::strcmp(string.c_str(), "JW") == 0) {
        value = LEnum::kJw;
        return true;
    } else if (std::strcmp(string.c_str(), "KA") == 0) {
        value = LEnum::kKa;
        return true;
    } else if (std::strcmp(string.c_str(), "KK") == 0) {
        value = LEnum::kKk;
        return true;
    } else if (std::strcmp(string.c_str(), "KL") == 0) {
        value = LEnum::kKl;
        return true;
    } else if (std::strcmp(string.c_str(), "KM") == 0) {
        value = LEnum::kKm;
        return true;
    } else if (std::strcmp(string.c_str(), "KN") == 0) {
        value = LEnum::kKn;
        return true;
    } else if (std::strcmp(string.c_str(), "KO") == 0) {
        value = LEnum::kKo;
        return true;
    } else if (std::strcmp(string.c_str(), "KS") == 0) {
        value = LEnum::kKs;
        return true;
    } else if (std::strcmp(string.c_str(), "KU") == 0) {
        value = LEnum::kKu;
        return true;
    } else if (std::strcmp(string.c_str(), "KY") == 0) {
        value = LEnum::kKy;
        return true;
    } else if (std::strcmp(string.c_str(), "LA") == 0) {
        value = LEnum::kLa;
        return true;
    } else if (std::strcmp(string.c_str(), "LN") == 0) {
        value = LEnum::kLn;
        return true;
    } else if (std::strcmp(string.c_str(), "LO") == 0) {
        value = LEnum::kLo;
        return true;
    } else if (std::strcmp(string.c_str(), "LT") == 0) {
        value = LEnum::kLt;
        return true;
    } else if (std::strcmp(string.c_str(), "LV") == 0) {
        value = LEnum::kLv;
        return true;
    } else if (std::strcmp(string.c_str(), "MG") == 0) {
        value = LEnum::kMg;
        return true;
    } else if (std::strcmp(string.c_str(), "MI") == 0) {
        value = LEnum::kMi;
        return true;
    } else if (std::strcmp(string.c_str(), "MK") == 0) {
        value = LEnum::kMk;
        return true;
    } else if (std::strcmp(string.c_str(), "ML") == 0) {
        value = LEnum::kMl;
        return true;
    } else if (std::strcmp(string.c_str(), "MN") == 0) {
        value = LEnum::kMn;
        return true;
    } else if (std::strcmp(string.c_str(), "MO") == 0) {
        value = LEnum::kMo;
        return true;
    } else if (std::strcmp(string.c_str(), "MR") == 0) {
        value = LEnum::kMr;
        return true;
    } else if (std::strcmp(string.c_str(), "MS") == 0) {
        value = LEnum::kMs;
        return true;
    } else if (std::strcmp(string.c_str(), "MT") == 0) {
        value = LEnum::kMt;
        return true;
    } else if (std::strcmp(string.c_str(), "MY") == 0) {
        value = LEnum::kMy;
        return true;
    } else if (std::strcmp(string.c_str(), "NA") == 0) {
        value = LEnum::kNa;
        return true;
    } else if (std::strcmp(string.c_str(), "NE") == 0) {
        value = LEnum::kNe;
        return true;
    } else if (std::strcmp(string.c_str(), "NL") == 0) {
        value = LEnum::kNl;
        return true;
    } else if (std::strcmp(string.c_str(), "NO") == 0) {
        value = LEnum::kNo;
        return true;
    } else if (std::strcmp(string.c_str(), "OC") == 0) {
        value = LEnum::kOc;
        return true;
    } else if (std::strcmp(string.c_str(), "OM") == 0) {
        value = LEnum::kOm;
        return true;
    } else if (std::strcmp(string.c_str(), "OR") == 0) {
        value = LEnum::kOr;
        return true;
    } else if (std::strcmp(string.c_str(), "PA") == 0) {
        value = LEnum::kPa;
        return true;
    } else if (std::strcmp(string.c_str(), "PL") == 0) {
        value = LEnum::kPl;
        return true;
    } else if (std::strcmp(string.c_str(), "PS") == 0) {
        value = LEnum::kPs;
        return true;
    } else if (std::strcmp(string.c_str(), "PT") == 0) {
        value = LEnum::kPt;
        return true;
    } else if (std::strcmp(string.c_str(), "QU") == 0) {
        value = LEnum::kQu;
        return true;
    } else if (std::strcmp(string.c_str(), "RM") == 0) {
        value = LEnum::kRm;
        return true;
    } else if (std::strcmp(string.c_str(), "RN") == 0) {
        value = LEnum::kRn;
        return true;
    } else if (std::strcmp(string.c_str(), "RO") == 0) {
        value = LEnum::kRo;
        return true;
    } else if (std::strcmp(string.c_str(), "RU") == 0) {
        value = LEnum::kRu;
        return true;
    } else if (std::strcmp(string.c_str(), "RW") == 0) {
        value = LEnum::kRw;
        return true;
    } else if (std::strcmp(string.c_str(), "SA") == 0) {
        value = LEnum::kSa;
        return true;
    } else if (std::strcmp(string.c_str(), "SD") == 0) {
        value = LEnum::kSd;
        return true;
    } else if (std::strcmp(string.c_str(), "SG") == 0) {
        value = LEnum::kSg;
        return true;
    } else if (std::strcmp(string.c_str(), "SH") == 0) {
        value = LEnum::kSh;
        return true;
    } else if (std::strcmp(string.c_str(), "SI") == 0) {
        value = LEnum::kSi;
        return true;
    } else if (std::strcmp(string.c_str(), "SK") == 0) {
        value = LEnum::kSk;
        return true;
    } else if (std::strcmp(string.c_str(), "SL") == 0) {
        value = LEnum::kSl;
        return true;
    } else if (std::strcmp(string.c_str(), "SM") == 0) {
        value = LEnum::kSm;
        return true;
    } else if (std::strcmp(string.c_str(), "SN") == 0) {
        value = LEnum::kSn;
        return true;
    } else if (std::strcmp(string.c_str(), "SO") == 0) {
        value = LEnum::kSo;
        return true;
    } else if (std::strcmp(string.c_str(), "SQ") == 0) {
        value = LEnum::kSq;
        return true;
    } else if (std::strcmp(string.c_str(), "SR") == 0) {
        value = LEnum::kSr;
        return true;
    } else if (std::strcmp(string.c_str(), "SS") == 0) {
        value = LEnum::kSs;
        return true;
    } else if (std::strcmp(string.c_str(), "ST") == 0) {
        value = LEnum::kSt;
        return true;
    } else if (std::strcmp(string.c_str(), "SU") == 0) {
        value = LEnum::kSu;
        return true;
    } else if (std::strcmp(string.c_str(), "SV") == 0) {
        value = LEnum::kSv;
        return true;
    } else if (std::strcmp(string.c_str(), "SW") == 0) {
        value = LEnum::kSw;
        return true;
    } else if (std::strcmp(string.c_str(), "TA") == 0) {
        value = LEnum::kTa;
        return true;
    } else if (std::strcmp(string.c_str(), "TE") == 0) {
        value = LEnum::kTe;
        return true;
    } else if (std::strcmp(string.c_str(), "TG") == 0) {
        value = LEnum::kTg;
        return true;
    } else if (std::strcmp(string.c_str(), "TH") == 0) {
        value = LEnum::kTh;
        return true;
    } else if (std::strcmp(string.c_str(), "TI") == 0) {
        value = LEnum::kTi;
        return true;
    } else if (std::strcmp(string.c_str(), "TK") == 0) {
        value = LEnum::kTk;
        return true;
    } else if (std::strcmp(string.c_str(), "TL") == 0) {
        value = LEnum::kTl;
        return true;
    } else if (std::strcmp(string.c_str(), "TN") == 0) {
        value = LEnum::kTn;
        return true;
    } else if (std::strcmp(string.c_str(), "TO") == 0) {
        value = LEnum::kTo;
        return true;
    } else if (std::strcmp(string.c_str(), "TR") == 0) {
        value = LEnum::kTr;
        return true;
    } else if (std::strcmp(string.c_str(), "TS") == 0) {
        value = LEnum::kTs;
        return true;
    } else if (std::strcmp(string.c_str(), "TT") == 0) {
        value = LEnum::kTt;
        return true;
    } else if (std::strcmp(string.c_str(), "TW") == 0) {
        value = LEnum::kTw;
        return true;
    } else if (std::strcmp(string.c_str(), "UK") == 0) {
        value = LEnum::kUk;
        return true;
    } else if (std::strcmp(string.c_str(), "UR") == 0) {
        value = LEnum::kUr;
        return true;
    } else if (std::strcmp(string.c_str(), "UZ") == 0) {
        value = LEnum::kUz;
        return true;
    } else if (std::strcmp(string.c_str(), "VI") == 0) {
        value = LEnum::kVi;
        return true;
    } else if (std::strcmp(string.c_str(), "VO") == 0) {
        value = LEnum::kVo;
        return true;
    } else if (std::strcmp(string.c_str(), "WO") == 0) {
        value = LEnum::kWo;
        return true;
    } else if (std::strcmp(string.c_str(), "XH") == 0) {
        value = LEnum::kXh;
        return true;
    } else if (std::strcmp(string.c_str(), "YO") == 0) {
        value = LEnum::kYo;
        return true;
    } else if (std::strcmp(string.c_str(), "ZH") == 0) {
        value = LEnum::kZh;
        return true;
    } else if (std::strcmp(string.c_str(), "ZU") == 0) {
        value = LEnum::kZu;
        return true;
    } else
        return false;
}

inline const char* ToString(LEnum value)
{
    switch (value) {
        case LEnum::kAa:
            return "AA";
        case LEnum::kAb:
            return "AB";
        case LEnum::kAf:
            return "AF";
        case LEnum::kAm:
            return "AM";
        case LEnum::kAr:
            return "AR";
        case LEnum::kAs:
            return "AS";
        case LEnum::kAy:
            return "AY";
        case LEnum::kAz:
            return "AZ";
        case LEnum::kBa:
            return "BA";
        case LEnum::kBe:
            return "BE";
        case LEnum::kBg:
            return "BG";
        case LEnum::kBh:
            return "BH";
        case LEnum::kBi:
            return "BI";
        case LEnum::kBn:
            return "BN";
        case LEnum::kBo:
            return "BO";
        case LEnum::kBr:
            return "BR";
        case LEnum::kCa:
            return "CA";
        case LEnum::kCo:
            return "CO";
        case LEnum::kCs:
            return "CS";
        case LEnum::kCy:
            return "CY";
        case LEnum::kDa:
            return "DA";
        case LEnum::kDe:
            return "DE";
        case LEnum::kDz:
            return "DZ";
        case LEnum::kEl:
            return "EL";
        case LEnum::kEn:
            return "EN";
        case LEnum::kEo:
            return "EO";
        case LEnum::kEs:
            return "ES";
        case LEnum::kEt:
            return "ET";
        case LEnum::kEu:
            return "EU";
        case LEnum::kFa:
            return "FA";
        case LEnum::kFi:
            return "FI";
        case LEnum::kFj:
            return "FJ";
        case LEnum::kFo:
            return "FO";
        case LEnum::kForAll:
            return "FOR-ALL";
        case LEnum::kFr:
            return "FR";
        case LEnum::kFy:
            return "FY";
        case LEnum::kGa:
            return "GA";
        case LEnum::kGd:
            return "GD";
        case LEnum::kGl:
            return "GL";
        case LEnum::kGn:
            return "GN";
        case LEnum::kGu:
            return "GU";
        case LEnum::kHa:
            return "HA";
        case LEnum::kHi:
            return "HI";
        case LEnum::kHr:
            return "HR";
        case LEnum::kHu:
            return "HU";
        case LEnum::kHy:
            return "HY";
        case LEnum::kIa:
            return "IA";
        case LEnum::kIe:
            return "IE";
        case LEnum::kIk:
            return "IK";
        case LEnum::kIn:
            return "IN";
        case LEnum::kIs:
            return "IS";
        case LEnum::kIt:
            return "IT";
        case LEnum::kIw:
            return "IW";
        case LEnum::kJa:
            return "JA";
        case LEnum::kJi:
            return "JI";
        case LEnum::kJw:
            return "JW";
        case LEnum::kKa:
            return "KA";
        case LEnum::kKk:
            return "KK";
        case LEnum::kKl:
            return "KL";
        case LEnum::kKm:
            return "KM";
        case LEnum::kKn:
            return "KN";
        case LEnum::kKo:
            return "KO";
        case LEnum::kKs:
            return "KS";
        case LEnum::kKu:
            return "KU";
        case LEnum::kKy:
            return "KY";
        case LEnum::kLa:
            return "LA";
        case LEnum::kLn:
            return "LN";
        case LEnum::kLo:
            return "LO";
        case LEnum::kLt:
            return "LT";
        case LEnum::kLv:
            return "LV";
        case LEnum::kMg:
            return "MG";
        case LEnum::kMi:
            return "MI";
        case LEnum::kMk:
            return "MK";
        case LEnum::kMl:
            return "ML";
        case LEnum::kMn:
            return "MN";
        case LEnum::kMo:
            return "MO";
        case LEnum::kMr:
            return "MR";
        case LEnum::kMs:
            return "MS";
        case LEnum::kMt:
            return "MT";
        case LEnum::kMy:
            return "MY";
        case LEnum::kNa:
            return "NA";
        case LEnum::kNe:
            return "NE";
        case LEnum::kNl:
            return "NL";
        case LEnum::kNo:
            return "NO";
        case LEnum::kOc:
            return "OC";
        case LEnum::kOm:
            return "OM";
        case LEnum::kOr:
            return "OR";
        case LEnum::kPa:
            return "PA";
        case LEnum::kPl:
            return "PL";
        case LEnum::kPs:
            return "PS";
        case LEnum::kPt:
            return "PT";
        case LEnum::kQu:
            return "QU";
        case LEnum::kRm:
            return "RM";
        case LEnum::kRn:
            return "RN";
        case LEnum::kRo:
            return "RO";
        case LEnum::kRu:
            return "RU";
        case LEnum::kRw:
            return "RW";
        case LEnum::kSa:
            return "SA";
        case LEnum::kSd:
            return "SD";
        case LEnum::kSg:
            return "SG";
        case LEnum::kSh:
            return "SH";
        case LEnum::kSi:
            return "SI";
        case LEnum::kSk:
            return "SK";
        case LEnum::kSl:
            return "SL";
        case LEnum::kSm:
            return "SM";
        case LEnum::kSn:
            return "SN";
        case LEnum::kSo:
            return "SO";
        case LEnum::kSq:
            return "SQ";
        case LEnum::kSr:
            return "SR";
        case LEnum::kSs:
            return "SS";
        case LEnum::kSt:
            return "ST";
        case LEnum::kSu:
            return "SU";
        case LEnum::kSv:
            return "SV";
        case LEnum::kSw:
            return "SW";
        case LEnum::kTa:
            return "TA";
        case LEnum::kTe:
            return "TE";
        case LEnum::kTg:
            return "TG";
        case LEnum::kTh:
            return "TH";
        case LEnum::kTi:
            return "TI";
        case LEnum::kTk:
            return "TK";
        case LEnum::kTl:
            return "TL";
        case LEnum::kTn:
            return "TN";
        case LEnum::kTo:
            return "TO";
        case LEnum::kTr:
            return "TR";
        case LEnum::kTs:
            return "TS";
        case LEnum::kTt:
            return "TT";
        case LEnum::kTw:
            return "TW";
        case LEnum::kUk:
            return "UK";
        case LEnum::kUr:
            return "UR";
        case LEnum::kUz:
            return "UZ";
        case LEnum::kVi:
            return "VI";
        case LEnum::kVo:
            return "VO";
        case LEnum::kWo:
            return "WO";
        case LEnum::kXh:
            return "XH";
        case LEnum::kYo:
            return "YO";
        case LEnum::kZh:
            return "ZH";
        case LEnum::kZu:
            return "ZU";
        default:
            return nullptr;
    }
}

enum class LatencyConstraintTypeEnum
{
    kAge
    = 0,  // In this case, the latency constraint is seen from the perspective of the response event of the associated
          // event chain. Given a certain response event, the age interval of the latest stimulus is constrained.
    kReaction
    = 1,  // In this case, the latency constraint is seen from the perspective of the stimulus event of the associated
          // event chain. Given a certain stimulus event, the reaction interval of the first response is constrained.
};

template < typename StringType >
bool FromString(const StringType& string, LatencyConstraintTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "AGE") == 0) {
        value = LatencyConstraintTypeEnum::kAge;
        return true;
    } else if (std::strcmp(string.c_str(), "REACTION") == 0) {
        value = LatencyConstraintTypeEnum::kReaction;
        return true;
    } else
        return false;
}

inline const char* ToString(LatencyConstraintTypeEnum value)
{
    switch (value) {
        case LatencyConstraintTypeEnum::kAge:
            return "AGE";
        case LatencyConstraintTypeEnum::kReaction:
            return "REACTION";
        default:
            return nullptr;
    }
}

enum class ListEnum
{
    kNumber   = 0,  // This indicates that the list is an numerated list.
    kUnnumber = 1,  // This indicates that it is an enumeration (bulleted list)
};

template < typename StringType >
bool FromString(const StringType& string, ListEnum& value)
{
    if (std::strcmp(string.c_str(), "NUMBER") == 0) {
        value = ListEnum::kNumber;
        return true;
    } else if (std::strcmp(string.c_str(), "UNNUMBER") == 0) {
        value = ListEnum::kUnnumber;
        return true;
    } else
        return false;
}

inline const char* ToString(ListEnum value)
{
    switch (value) {
        case ListEnum::kNumber:
            return "NUMBER";
        case ListEnum::kUnnumber:
            return "UNNUMBER";
        default:
            return nullptr;
    }
}

enum class LogTraceDefaultLogLevelEnum
{
    kFatal   = 0,  // Fatal error
    kError   = 1,  // Error with impact to correct functionality
    kWarn    = 2,  // Warning if correct behavior cannot be ensured
    kInfo    = 3,  // High level information
    kDebug   = 4,  // Detailed information for programmers
    kVerbose = 5,  // Verbose debug message
};

template < typename StringType >
bool FromString(const StringType& string, LogTraceDefaultLogLevelEnum& value)
{
    if (std::strcmp(string.c_str(), "FATAL") == 0) {
        value = LogTraceDefaultLogLevelEnum::kFatal;
        return true;
    } else if (std::strcmp(string.c_str(), "ERROR") == 0) {
        value = LogTraceDefaultLogLevelEnum::kError;
        return true;
    } else if (std::strcmp(string.c_str(), "WARN") == 0) {
        value = LogTraceDefaultLogLevelEnum::kWarn;
        return true;
    } else if (std::strcmp(string.c_str(), "INFO") == 0) {
        value = LogTraceDefaultLogLevelEnum::kInfo;
        return true;
    } else if (std::strcmp(string.c_str(), "DEBUG") == 0) {
        value = LogTraceDefaultLogLevelEnum::kDebug;
        return true;
    } else if (std::strcmp(string.c_str(), "VERBOSE") == 0) {
        value = LogTraceDefaultLogLevelEnum::kVerbose;
        return true;
    } else
        return false;
}

inline const char* ToString(LogTraceDefaultLogLevelEnum value)
{
    switch (value) {
        case LogTraceDefaultLogLevelEnum::kFatal:
            return "FATAL";
        case LogTraceDefaultLogLevelEnum::kError:
            return "ERROR";
        case LogTraceDefaultLogLevelEnum::kWarn:
            return "WARN";
        case LogTraceDefaultLogLevelEnum::kInfo:
            return "INFO";
        case LogTraceDefaultLogLevelEnum::kDebug:
            return "DEBUG";
        case LogTraceDefaultLogLevelEnum::kVerbose:
            return "VERBOSE";
        default:
            return nullptr;
    }
}

enum class LogTraceLogModeEnum
{
    kConsole = 0,  // Destination of log message will be the console output.
    kFile    = 1,  // Destination of log message will be a file on the file system.
    kNetwork = 2,  // Log message will be transmitted over the communication bus.
};

template < typename StringType >
bool FromString(const StringType& string, LogTraceLogModeEnum& value)
{
    if (std::strcmp(string.c_str(), "CONSOLE") == 0) {
        value = LogTraceLogModeEnum::kConsole;
        return true;
    } else if (std::strcmp(string.c_str(), "FILE") == 0) {
        value = LogTraceLogModeEnum::kFile;
        return true;
    } else if (std::strcmp(string.c_str(), "NETWORK") == 0) {
        value = LogTraceLogModeEnum::kNetwork;
        return true;
    } else
        return false;
}

inline const char* ToString(LogTraceLogModeEnum value)
{
    switch (value) {
        case LogTraceLogModeEnum::kConsole:
            return "CONSOLE";
        case LogTraceLogModeEnum::kFile:
            return "FILE";
        case LogTraceLogModeEnum::kNetwork:
            return "NETWORK";
        default:
            return nullptr;
    }
}

enum class LoggingBehaviorEnum
{
    kDoesNotUseLogging = 0,  // The Executable indicates its intention to not use logging.
    kUsesLogging       = 1,  // The Executable indicates its intention to use logging
};

template < typename StringType >
bool FromString(const StringType& string, LoggingBehaviorEnum& value)
{
    if (std::strcmp(string.c_str(), "DOES-NOT-USE-LOGGING") == 0) {
        value = LoggingBehaviorEnum::kDoesNotUseLogging;
        return true;
    } else if (std::strcmp(string.c_str(), "USES-LOGGING") == 0) {
        value = LoggingBehaviorEnum::kUsesLogging;
        return true;
    } else
        return false;
}

inline const char* ToString(LoggingBehaviorEnum value)
{
    switch (value) {
        case LoggingBehaviorEnum::kDoesNotUseLogging:
            return "DOES-NOT-USE-LOGGING";
        case LoggingBehaviorEnum::kUsesLogging:
            return "USES-LOGGING";
        default:
            return nullptr;
    }
}

enum class MappingDirectionEnum
{
    kBidirectional = 0,  // The TextTableMapping is applicable in both directions.
    kFirstToSecond
    = 1,  // The TextTableMapping is applicable in the direction from firstDataPrototype / firstOperationArgument
          // referring into the PortInterface of the PPortPrototype to secondDataPrototype / secondOperationArgument
          // referring into the PortInterface of the RPortPrototype.
    kSecondToFirst
    = 2,  // The TextTableMapping is applicable in the direction from secondDataPrototype / secondOperationArgument
          // referring into the PortInterface of the PPortPrototype to firstDataPrototype / firstOperationArgument
          // referring into the PortInterface of the RPortPrototype.
};

template < typename StringType >
bool FromString(const StringType& string, MappingDirectionEnum& value)
{
    if (std::strcmp(string.c_str(), "BIDIRECTIONAL") == 0) {
        value = MappingDirectionEnum::kBidirectional;
        return true;
    } else if (std::strcmp(string.c_str(), "FIRST-TO-SECOND") == 0) {
        value = MappingDirectionEnum::kFirstToSecond;
        return true;
    } else if (std::strcmp(string.c_str(), "SECOND-TO-FIRST") == 0) {
        value = MappingDirectionEnum::kSecondToFirst;
        return true;
    } else
        return false;
}

inline const char* ToString(MappingDirectionEnum value)
{
    switch (value) {
        case MappingDirectionEnum::kBidirectional:
            return "BIDIRECTIONAL";
        case MappingDirectionEnum::kFirstToSecond:
            return "FIRST-TO-SECOND";
        case MappingDirectionEnum::kSecondToFirst:
            return "SECOND-TO-FIRST";
        default:
            return nullptr;
    }
}

enum class MappingScopeEnum
{
    kMappingScopeCore      = 0,  // The mapping constraint applies to different Cores.
    kMappingScopeEcu       = 1,  // The mapping constraint applies to different Ecus.
    kMappingScopePartition = 2,  // The mapping constraint applies to different Partitions.
};

template < typename StringType >
bool FromString(const StringType& string, MappingScopeEnum& value)
{
    if (std::strcmp(string.c_str(), "MAPPING-SCOPE-CORE") == 0) {
        value = MappingScopeEnum::kMappingScopeCore;
        return true;
    } else if (std::strcmp(string.c_str(), "MAPPING-SCOPE-ECU") == 0) {
        value = MappingScopeEnum::kMappingScopeEcu;
        return true;
    } else if (std::strcmp(string.c_str(), "MAPPING-SCOPE-PARTITION") == 0) {
        value = MappingScopeEnum::kMappingScopePartition;
        return true;
    } else
        return false;
}

inline const char* ToString(MappingScopeEnum value)
{
    switch (value) {
        case MappingScopeEnum::kMappingScopeCore:
            return "MAPPING-SCOPE-CORE";
        case MappingScopeEnum::kMappingScopeEcu:
            return "MAPPING-SCOPE-ECU";
        case MappingScopeEnum::kMappingScopePartition:
            return "MAPPING-SCOPE-PARTITION";
        default:
            return nullptr;
    }
}

enum class MaxCommModeEnum
{
    kFull   = 0,  // Full communication is requested.
    kNone   = 1,  // No communication is requested.
    kSilent = 2,  // Silent communication is requested: Only listening but not "talking".
};

template < typename StringType >
bool FromString(const StringType& string, MaxCommModeEnum& value)
{
    if (std::strcmp(string.c_str(), "FULL") == 0) {
        value = MaxCommModeEnum::kFull;
        return true;
    } else if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = MaxCommModeEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "SILENT") == 0) {
        value = MaxCommModeEnum::kSilent;
        return true;
    } else
        return false;
}

inline const char* ToString(MaxCommModeEnum value)
{
    switch (value) {
        case MaxCommModeEnum::kFull:
            return "FULL";
        case MaxCommModeEnum::kNone:
            return "NONE";
        case MaxCommModeEnum::kSilent:
            return "SILENT";
        default:
            return nullptr;
    }
}

enum class ModeErrorReactionPolicyEnum
{
    kDefaultMode = 0,  // This represents the ability to switch to the defaultMode in case of a mode error.
    kLastMode    = 1,  // This represents the ability to keep the last mode in case of a mode error.
};

template < typename StringType >
bool FromString(const StringType& string, ModeErrorReactionPolicyEnum& value)
{
    if (std::strcmp(string.c_str(), "DEFAULT-MODE") == 0) {
        value = ModeErrorReactionPolicyEnum::kDefaultMode;
        return true;
    } else if (std::strcmp(string.c_str(), "LAST-MODE") == 0) {
        value = ModeErrorReactionPolicyEnum::kLastMode;
        return true;
    } else
        return false;
}

inline const char* ToString(ModeErrorReactionPolicyEnum value)
{
    switch (value) {
        case ModeErrorReactionPolicyEnum::kDefaultMode:
            return "DEFAULT-MODE";
        case ModeErrorReactionPolicyEnum::kLastMode:
            return "LAST-MODE";
        default:
            return nullptr;
    }
}

enum class ModificationTypeEnum
{
    kContentRelated
    = 0,  // The attribute contentRelated expresses, that a substantial change of the content was performed in the
          // object. Usually this means e.g. that the derived artifacts need to be regenerated (e.g. code generation).
    kDocRelated = 1,  // The attribute docRelated expresses, that a change was applied to the documentation or other
                      // informal aspects of the object. Usually this means e.g. that not all derived artifacts need to
                      // be regenerated (e.g. code generation).
};

template < typename StringType >
bool FromString(const StringType& string, ModificationTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "CONTENT-RELATED") == 0) {
        value = ModificationTypeEnum::kContentRelated;
        return true;
    } else if (std::strcmp(string.c_str(), "DOC-RELATED") == 0) {
        value = ModificationTypeEnum::kDocRelated;
        return true;
    } else
        return false;
}

inline const char* ToString(ModificationTypeEnum value)
{
    switch (value) {
        case ModificationTypeEnum::kContentRelated:
            return "CONTENT-RELATED";
        case ModificationTypeEnum::kDocRelated:
            return "DOC-RELATED";
        default:
            return nullptr;
    }
}

enum class MonotonyEnum
{
    kDecreasing = 0,  // This indicates that the related curve needs to be monotony decreasing.
    kIncreasing = 1,  // This indicates that the related curve needs to be monotony increasing.
    kMonotonous = 2,  // This indicates that the values shall be monotonously decreasing or increasing, depending on the
                      // trend set by the first values of the series.
    kNoMonotony         = 3,  // This indicates that the related curve needs not to be monotony.
    kStrictlyDecreasing = 4,  // This indicates that the related curve needs to be strictly monotony decreasing.
    kStrictlyIncreasing = 5,  // This indicates that the related curve needs to be strictly monotony increasing.
    kStrictMonotonous   = 6,  // This indicates that the values shall be strict monotonously decreasing or increasing,
                              // depending on the trend set by the first values of the series.
};

template < typename StringType >
bool FromString(const StringType& string, MonotonyEnum& value)
{
    if (std::strcmp(string.c_str(), "DECREASING") == 0) {
        value = MonotonyEnum::kDecreasing;
        return true;
    } else if (std::strcmp(string.c_str(), "INCREASING") == 0) {
        value = MonotonyEnum::kIncreasing;
        return true;
    } else if (std::strcmp(string.c_str(), "MONOTONOUS") == 0) {
        value = MonotonyEnum::kMonotonous;
        return true;
    } else if (std::strcmp(string.c_str(), "NO-MONOTONY") == 0) {
        value = MonotonyEnum::kNoMonotony;
        return true;
    } else if (std::strcmp(string.c_str(), "STRICTLY-DECREASING") == 0) {
        value = MonotonyEnum::kStrictlyDecreasing;
        return true;
    } else if (std::strcmp(string.c_str(), "STRICTLY-INCREASING") == 0) {
        value = MonotonyEnum::kStrictlyIncreasing;
        return true;
    } else if (std::strcmp(string.c_str(), "STRICT-MONOTONOUS") == 0) {
        value = MonotonyEnum::kStrictMonotonous;
        return true;
    } else
        return false;
}

inline const char* ToString(MonotonyEnum value)
{
    switch (value) {
        case MonotonyEnum::kDecreasing:
            return "DECREASING";
        case MonotonyEnum::kIncreasing:
            return "INCREASING";
        case MonotonyEnum::kMonotonous:
            return "MONOTONOUS";
        case MonotonyEnum::kNoMonotony:
            return "NO-MONOTONY";
        case MonotonyEnum::kStrictlyDecreasing:
            return "STRICTLY-DECREASING";
        case MonotonyEnum::kStrictlyIncreasing:
            return "STRICTLY-INCREASING";
        case MonotonyEnum::kStrictMonotonous:
            return "STRICT-MONOTONOUS";
        default:
            return nullptr;
    }
}

enum class NmCoordinatorRoleEnum
{
    kActive  = 0,  // Coordinator which "actively" performs NmCoordinator functionality at this channel
    kPassive = 1,  // Coordinator which "passively" performs NmCoordinator functionality at this channel - used at
                   // NmCoordinatorSync use case.
};

template < typename StringType >
bool FromString(const StringType& string, NmCoordinatorRoleEnum& value)
{
    if (std::strcmp(string.c_str(), "ACTIVE") == 0) {
        value = NmCoordinatorRoleEnum::kActive;
        return true;
    } else if (std::strcmp(string.c_str(), "PASSIVE") == 0) {
        value = NmCoordinatorRoleEnum::kPassive;
        return true;
    } else
        return false;
}

inline const char* ToString(NmCoordinatorRoleEnum value)
{
    switch (value) {
        case NmCoordinatorRoleEnum::kActive:
            return "ACTIVE";
        case NmCoordinatorRoleEnum::kPassive:
            return "PASSIVE";
        default:
            return nullptr;
    }
}

enum class NoteTypeEnum
{
    kCaution     = 0,  // This indicates that the note is an alert which shall be considered carefully.
    kExample     = 1,  // This indicates that the note represents an example, e.g. a code example etc.
    kExercise    = 2,  // This indicates that the note represents an exercise for the reader.
    kHint        = 3,  // This indicates that the note represents a hint which helps the user for better understanding.
    kInstruction = 4,  // This indicates that the note represents an instruction, e.g. a step by step procedure.
    kOther       = 5,  // This indicates that the note is something else. The particular type of the note shall then be
                       // specified in the label of the note.
    kTip = 6,  // This indicates that the note represents which is good to know. It is similar to a hint, but focuses
               // more to good practice than to better understanding.
};

template < typename StringType >
bool FromString(const StringType& string, NoteTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "CAUTION") == 0) {
        value = NoteTypeEnum::kCaution;
        return true;
    } else if (std::strcmp(string.c_str(), "EXAMPLE") == 0) {
        value = NoteTypeEnum::kExample;
        return true;
    } else if (std::strcmp(string.c_str(), "EXERCISE") == 0) {
        value = NoteTypeEnum::kExercise;
        return true;
    } else if (std::strcmp(string.c_str(), "HINT") == 0) {
        value = NoteTypeEnum::kHint;
        return true;
    } else if (std::strcmp(string.c_str(), "INSTRUCTION") == 0) {
        value = NoteTypeEnum::kInstruction;
        return true;
    } else if (std::strcmp(string.c_str(), "OTHER") == 0) {
        value = NoteTypeEnum::kOther;
        return true;
    } else if (std::strcmp(string.c_str(), "TIP") == 0) {
        value = NoteTypeEnum::kTip;
        return true;
    } else
        return false;
}

inline const char* ToString(NoteTypeEnum value)
{
    switch (value) {
        case NoteTypeEnum::kCaution:
            return "CAUTION";
        case NoteTypeEnum::kExample:
            return "EXAMPLE";
        case NoteTypeEnum::kExercise:
            return "EXERCISE";
        case NoteTypeEnum::kHint:
            return "HINT";
        case NoteTypeEnum::kInstruction:
            return "INSTRUCTION";
        case NoteTypeEnum::kOther:
            return "OTHER";
        case NoteTypeEnum::kTip:
            return "TIP";
        default:
            return nullptr;
    }
}

enum class NvBlockNeedsReliabilityEnum
{
    kErrorCorrection = 0,  // Errors shall be corrected
    kErrorDetection  = 1,  // Errors shall be detected
    kNoProtection    = 2,  // Data need not to be handled with protection
};

template < typename StringType >
bool FromString(const StringType& string, NvBlockNeedsReliabilityEnum& value)
{
    if (std::strcmp(string.c_str(), "ERROR-CORRECTION") == 0) {
        value = NvBlockNeedsReliabilityEnum::kErrorCorrection;
        return true;
    } else if (std::strcmp(string.c_str(), "ERROR-DETECTION") == 0) {
        value = NvBlockNeedsReliabilityEnum::kErrorDetection;
        return true;
    } else if (std::strcmp(string.c_str(), "NO-PROTECTION") == 0) {
        value = NvBlockNeedsReliabilityEnum::kNoProtection;
        return true;
    } else
        return false;
}

inline const char* ToString(NvBlockNeedsReliabilityEnum value)
{
    switch (value) {
        case NvBlockNeedsReliabilityEnum::kErrorCorrection:
            return "ERROR-CORRECTION";
        case NvBlockNeedsReliabilityEnum::kErrorDetection:
            return "ERROR-DETECTION";
        case NvBlockNeedsReliabilityEnum::kNoProtection:
            return "NO-PROTECTION";
        default:
            return nullptr;
    }
}

enum class NvBlockNeedsWritingPriorityEnum
{
    kHigh   = 0,  // Writing priority is high.
    kLow    = 1,  // Writing priority is low.
    kMedium = 2,  // Writing priority is medium.
};

template < typename StringType >
bool FromString(const StringType& string, NvBlockNeedsWritingPriorityEnum& value)
{
    if (std::strcmp(string.c_str(), "HIGH") == 0) {
        value = NvBlockNeedsWritingPriorityEnum::kHigh;
        return true;
    } else if (std::strcmp(string.c_str(), "LOW") == 0) {
        value = NvBlockNeedsWritingPriorityEnum::kLow;
        return true;
    } else if (std::strcmp(string.c_str(), "MEDIUM") == 0) {
        value = NvBlockNeedsWritingPriorityEnum::kMedium;
        return true;
    } else
        return false;
}

inline const char* ToString(NvBlockNeedsWritingPriorityEnum value)
{
    switch (value) {
        case NvBlockNeedsWritingPriorityEnum::kHigh:
            return "HIGH";
        case NvBlockNeedsWritingPriorityEnum::kLow:
            return "LOW";
        case NvBlockNeedsWritingPriorityEnum::kMedium:
            return "MEDIUM";
        default:
            return nullptr;
    }
}

enum class ObdRatioConnectionKindEnum
{
    kApiUse   = 0,  // The IUMPR service (of the DEM) uses an explicit API to connect to the component or module.
    kObserver = 1,  // The IUMPR service (of the Dem) uses no API but "observes"  the associated diagnostic event.
};

template < typename StringType >
bool FromString(const StringType& string, ObdRatioConnectionKindEnum& value)
{
    if (std::strcmp(string.c_str(), "API-USE") == 0) {
        value = ObdRatioConnectionKindEnum::kApiUse;
        return true;
    } else if (std::strcmp(string.c_str(), "OBSERVER") == 0) {
        value = ObdRatioConnectionKindEnum::kObserver;
        return true;
    } else
        return false;
}

inline const char* ToString(ObdRatioConnectionKindEnum value)
{
    switch (value) {
        case ObdRatioConnectionKindEnum::kApiUse:
            return "API-USE";
        case ObdRatioConnectionKindEnum::kObserver:
            return "OBSERVER";
        default:
            return nullptr;
    }
}

enum class OperationCycleTypeEnum
{
    kIgnition = 0,  // Ignition ON / OFF cycle.
    kObdDcy   = 1,  // OBD Driving cycle.
    kOther    = 2,  // Further operation cycle.
    kPower    = 3,  // Power ON / OFF cycle.
    kTime     = 4,  // Time based operation cycle.
    kWarmup   = 5,  // OBD Warm up cycle.
};

template < typename StringType >
bool FromString(const StringType& string, OperationCycleTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "IGNITION") == 0) {
        value = OperationCycleTypeEnum::kIgnition;
        return true;
    } else if (std::strcmp(string.c_str(), "OBD-DCY") == 0) {
        value = OperationCycleTypeEnum::kObdDcy;
        return true;
    } else if (std::strcmp(string.c_str(), "OTHER") == 0) {
        value = OperationCycleTypeEnum::kOther;
        return true;
    } else if (std::strcmp(string.c_str(), "POWER") == 0) {
        value = OperationCycleTypeEnum::kPower;
        return true;
    } else if (std::strcmp(string.c_str(), "TIME") == 0) {
        value = OperationCycleTypeEnum::kTime;
        return true;
    } else if (std::strcmp(string.c_str(), "WARMUP") == 0) {
        value = OperationCycleTypeEnum::kWarmup;
        return true;
    } else
        return false;
}

inline const char* ToString(OperationCycleTypeEnum value)
{
    switch (value) {
        case OperationCycleTypeEnum::kIgnition:
            return "IGNITION";
        case OperationCycleTypeEnum::kObdDcy:
            return "OBD-DCY";
        case OperationCycleTypeEnum::kOther:
            return "OTHER";
        case OperationCycleTypeEnum::kPower:
            return "POWER";
        case OperationCycleTypeEnum::kTime:
            return "TIME";
        case OperationCycleTypeEnum::kWarmup:
            return "WARMUP";
        default:
            return nullptr;
    }
}

enum class OrientEnum
{
    kLand = 0,  // This indicates that the table is rendered in landscape which results in turning the table 90 degree
                // clockwise.
    kPort = 1,  // This indicates that the table is rendered in portrait, which is the regular text flow.
};

template < typename StringType >
bool FromString(const StringType& string, OrientEnum& value)
{
    if (std::strcmp(string.c_str(), "LAND") == 0) {
        value = OrientEnum::kLand;
        return true;
    } else if (std::strcmp(string.c_str(), "PORT") == 0) {
        value = OrientEnum::kPort;
        return true;
    } else
        return false;
}

inline const char* ToString(OrientEnum value)
{
    switch (value) {
        case OrientEnum::kLand:
            return "LAND";
        case OrientEnum::kPort:
            return "PORT";
        default:
            return nullptr;
    }
}

enum class PduCollectionSemanticsEnum
{
    kLastIsBest = 0,  // Only the latest PDU instances are transmitted.
    kQueued     = 1,  // All instances of PDUs are transmitted.
};

template < typename StringType >
bool FromString(const StringType& string, PduCollectionSemanticsEnum& value)
{
    if (std::strcmp(string.c_str(), "LAST-IS-BEST") == 0) {
        value = PduCollectionSemanticsEnum::kLastIsBest;
        return true;
    } else if (std::strcmp(string.c_str(), "QUEUED") == 0) {
        value = PduCollectionSemanticsEnum::kQueued;
        return true;
    } else
        return false;
}

inline const char* ToString(PduCollectionSemanticsEnum value)
{
    switch (value) {
        case PduCollectionSemanticsEnum::kLastIsBest:
            return "LAST-IS-BEST";
        case PduCollectionSemanticsEnum::kQueued:
            return "QUEUED";
        default:
            return nullptr;
    }
}

enum class PduCollectionTriggerEnum
{
    kAlways = 0,  // Pdu will trigger the transmission of the data.
    kNever  = 1,  // Pdu will be buffered and will not trigger the transmission of the data.
};

template < typename StringType >
bool FromString(const StringType& string, PduCollectionTriggerEnum& value)
{
    if (std::strcmp(string.c_str(), "ALWAYS") == 0) {
        value = PduCollectionTriggerEnum::kAlways;
        return true;
    } else if (std::strcmp(string.c_str(), "NEVER") == 0) {
        value = PduCollectionTriggerEnum::kNever;
        return true;
    } else
        return false;
}

inline const char* ToString(PduCollectionTriggerEnum value)
{
    switch (value) {
        case PduCollectionTriggerEnum::kAlways:
            return "ALWAYS";
        case PduCollectionTriggerEnum::kNever:
            return "NEVER";
        default:
            return nullptr;
    }
}

enum class PersistencyCollectionLevelUpdateStrategyEnum
{
    kKeepExisting = 0,  // The update strategy is to keep the existing values on the level of the respective collection.
    kDelete       = 1,  // The update strategy is to delete all values on the level of the respective collection.
};

template < typename StringType >
bool FromString(const StringType& string, PersistencyCollectionLevelUpdateStrategyEnum& value)
{
    if (std::strcmp(string.c_str(), "KEEP-EXISTING") == 0) {
        value = PersistencyCollectionLevelUpdateStrategyEnum::kKeepExisting;
        return true;
    } else if (std::strcmp(string.c_str(), "DELETE") == 0) {
        value = PersistencyCollectionLevelUpdateStrategyEnum::kDelete;
        return true;
    } else
        return false;
}

inline const char* ToString(PersistencyCollectionLevelUpdateStrategyEnum value)
{
    switch (value) {
        case PersistencyCollectionLevelUpdateStrategyEnum::kKeepExisting:
            return "KEEP-EXISTING";
        case PersistencyCollectionLevelUpdateStrategyEnum::kDelete:
            return "DELETE";
        default:
            return nullptr;
    }
}

enum class PersistencyElementLevelUpdateStrategyEnum
{
    kOverwrite    = 0,  // The update strategy is to overwrite the respective data item.
    kKeepExisting = 1,  // The update strategy is to keep the existing value of the respective data item.
    kDelete       = 2,  // The update strategy is to delete the value of the respective data item.
};

template < typename StringType >
bool FromString(const StringType& string, PersistencyElementLevelUpdateStrategyEnum& value)
{
    if (std::strcmp(string.c_str(), "OVERWRITE") == 0) {
        value = PersistencyElementLevelUpdateStrategyEnum::kOverwrite;
        return true;
    } else if (std::strcmp(string.c_str(), "KEEP-EXISTING") == 0) {
        value = PersistencyElementLevelUpdateStrategyEnum::kKeepExisting;
        return true;
    } else if (std::strcmp(string.c_str(), "DELETE") == 0) {
        value = PersistencyElementLevelUpdateStrategyEnum::kDelete;
        return true;
    } else
        return false;
}

inline const char* ToString(PersistencyElementLevelUpdateStrategyEnum value)
{
    switch (value) {
        case PersistencyElementLevelUpdateStrategyEnum::kOverwrite:
            return "OVERWRITE";
        case PersistencyElementLevelUpdateStrategyEnum::kKeepExisting:
            return "KEEP-EXISTING";
        case PersistencyElementLevelUpdateStrategyEnum::kDelete:
            return "DELETE";
        default:
            return nullptr;
    }
}

enum class PersistencyRedundancyEnum
{
    kRedundant = 0,  // This value represents the requirement that redundancy measures are applied on persistency
                     // storage level.  The nature of the redundant persistent storage is not further qualified and
                     // subject to integrator decisions.
    kNone = 1,  // This value represents the requirement that redundancy measures are not applied on persistency storage
                // level.
    kRedundantPerElement
    = 2,  // This value represents the requirement that redundancy measures are applied on key-value level of a
          // key-value storage or on file level of a file storage.  The nature of the redundancy used on the
          // persistent storage is not further qualified and subject to integrator decisions.
};

template < typename StringType >
bool FromString(const StringType& string, PersistencyRedundancyEnum& value)
{
    if (std::strcmp(string.c_str(), "REDUNDANT") == 0) {
        value = PersistencyRedundancyEnum::kRedundant;
        return true;
    } else if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = PersistencyRedundancyEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "REDUNDANT-PER-ELEMENT") == 0) {
        value = PersistencyRedundancyEnum::kRedundantPerElement;
        return true;
    } else
        return false;
}

inline const char* ToString(PersistencyRedundancyEnum value)
{
    switch (value) {
        case PersistencyRedundancyEnum::kRedundant:
            return "REDUNDANT";
        case PersistencyRedundancyEnum::kNone:
            return "NONE";
        case PersistencyRedundancyEnum::kRedundantPerElement:
            return "REDUNDANT-PER-ELEMENT";
        default:
            return nullptr;
    }
}

enum class PersistencyRedundancyHandlingScopeEnum
{
    kPersistencyRedundancyHandlingScopeElement
    = 0,  // The redundancy handling shall be applied on element level (key-value pair and file).
    kPersistencyRedundancyHandlingScopeStorage
    = 1,  // The redundancy handling shall be applied on storage (key-value storage and file storage) level.
};

template < typename StringType >
bool FromString(const StringType& string, PersistencyRedundancyHandlingScopeEnum& value)
{
    if (std::strcmp(string.c_str(), "PERSISTENCY-REDUNDANCY-HANDLING-SCOPE-ELEMENT") == 0) {
        value = PersistencyRedundancyHandlingScopeEnum::kPersistencyRedundancyHandlingScopeElement;
        return true;
    } else if (std::strcmp(string.c_str(), "PERSISTENCY-REDUNDANCY-HANDLING-SCOPE-STORAGE") == 0) {
        value = PersistencyRedundancyHandlingScopeEnum::kPersistencyRedundancyHandlingScopeStorage;
        return true;
    } else
        return false;
}

inline const char* ToString(PersistencyRedundancyHandlingScopeEnum value)
{
    switch (value) {
        case PersistencyRedundancyHandlingScopeEnum::kPersistencyRedundancyHandlingScopeElement:
            return "PERSISTENCY-REDUNDANCY-HANDLING-SCOPE-ELEMENT";
        case PersistencyRedundancyHandlingScopeEnum::kPersistencyRedundancyHandlingScopeStorage:
            return "PERSISTENCY-REDUNDANCY-HANDLING-SCOPE-STORAGE";
        default:
            return nullptr;
    }
}

enum class PgwideEnum
{
    kNoPgwide = 0,  // This indicates that the table shall be fit in the current text flow.
    kPgwide   = 1,  // This indicates that the table may use the entire page width. This is in particular important in
                    // case of so called "side-head layouts" but also if the table is in a list or in a note.
};

template < typename StringType >
bool FromString(const StringType& string, PgwideEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-PGWIDE") == 0) {
        value = PgwideEnum::kNoPgwide;
        return true;
    } else if (std::strcmp(string.c_str(), "PGWIDE") == 0) {
        value = PgwideEnum::kPgwide;
        return true;
    } else
        return false;
}

inline const char* ToString(PgwideEnum value)
{
    switch (value) {
        case PgwideEnum::kNoPgwide:
            return "NO-PGWIDE";
        case PgwideEnum::kPgwide:
            return "PGWIDE";
        default:
            return nullptr;
    }
}

enum class PncGatewayTypeEnum
{
    kActive  = 0,  // The active PncGateway functionality shall be performed
    kNone    = 1,  // No PncGateway functionality shall be performed
    kPassive = 2,  // The passive PncGateway functionality shall be performed
};

template < typename StringType >
bool FromString(const StringType& string, PncGatewayTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "ACTIVE") == 0) {
        value = PncGatewayTypeEnum::kActive;
        return true;
    } else if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = PncGatewayTypeEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "PASSIVE") == 0) {
        value = PncGatewayTypeEnum::kPassive;
        return true;
    } else
        return false;
}

inline const char* ToString(PncGatewayTypeEnum value)
{
    switch (value) {
        case PncGatewayTypeEnum::kActive:
            return "ACTIVE";
        case PncGatewayTypeEnum::kNone:
            return "NONE";
        case PncGatewayTypeEnum::kPassive:
            return "PASSIVE";
        default:
            return nullptr;
    }
}

enum class ProcessingKindEnum
{
    kFiltered = 0,  // Indicates that a raw signal has been manipulated by some application software components by using
                    // filters.
    kNone = 1,      // Indicates that none of the other option apply.
    kRaw
    = 2,  // Specifies that a signal is taken directly from the basic software modules, i.e. from the ECU abstraction
          // layer. It indicates to a developer that the control algorithm in the software has to provide filters.
};

template < typename StringType >
bool FromString(const StringType& string, ProcessingKindEnum& value)
{
    if (std::strcmp(string.c_str(), "FILTERED") == 0) {
        value = ProcessingKindEnum::kFiltered;
        return true;
    } else if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = ProcessingKindEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "RAW") == 0) {
        value = ProcessingKindEnum::kRaw;
        return true;
    } else
        return false;
}

inline const char* ToString(ProcessingKindEnum value)
{
    switch (value) {
        case ProcessingKindEnum::kFiltered:
            return "FILTERED";
        case ProcessingKindEnum::kNone:
            return "NONE";
        case ProcessingKindEnum::kRaw:
            return "RAW";
        default:
            return nullptr;
    }
}

enum class ProgramminglanguageEnum
{
    kC    = 0,  // C language
    kCpp  = 1,  // C++ language
    kJava = 2,  // Java language
};

template < typename StringType >
bool FromString(const StringType& string, ProgramminglanguageEnum& value)
{
    if (std::strcmp(string.c_str(), "C") == 0) {
        value = ProgramminglanguageEnum::kC;
        return true;
    } else if (std::strcmp(string.c_str(), "CPP") == 0) {
        value = ProgramminglanguageEnum::kCpp;
        return true;
    } else if (std::strcmp(string.c_str(), "JAVA") == 0) {
        value = ProgramminglanguageEnum::kJava;
        return true;
    } else
        return false;
}

inline const char* ToString(ProgramminglanguageEnum value)
{
    switch (value) {
        case ProgramminglanguageEnum::kC:
            return "C";
        case ProgramminglanguageEnum::kCpp:
            return "CPP";
        case ProgramminglanguageEnum::kJava:
            return "JAVA";
        default:
            return nullptr;
    }
}

enum class PulseTestEnum
{
    kDisable = 0,  // Disables the pulse test
    kEnable  = 1,  // Enables the pulse test
};

template < typename StringType >
bool FromString(const StringType& string, PulseTestEnum& value)
{
    if (std::strcmp(string.c_str(), "DISABLE") == 0) {
        value = PulseTestEnum::kDisable;
        return true;
    } else if (std::strcmp(string.c_str(), "ENABLE") == 0) {
        value = PulseTestEnum::kEnable;
        return true;
    } else
        return false;
}

inline const char* ToString(PulseTestEnum value)
{
    switch (value) {
        case PulseTestEnum::kDisable:
            return "DISABLE";
        case PulseTestEnum::kEnable:
            return "ENABLE";
        default:
            return nullptr;
    }
}

enum class RamBlockStatusControlEnum
{
    kApi = 0,  // The ramBlock status is controlled via service interface by usage of the SetRamBlockStatus operation.
    kNvRamManager = 1,  // The ramBlock status is controlled exclusively by the Nv Ram Manager.
};

template < typename StringType >
bool FromString(const StringType& string, RamBlockStatusControlEnum& value)
{
    if (std::strcmp(string.c_str(), "API") == 0) {
        value = RamBlockStatusControlEnum::kApi;
        return true;
    } else if (std::strcmp(string.c_str(), "NV-RAM-MANAGER") == 0) {
        value = RamBlockStatusControlEnum::kNvRamManager;
        return true;
    } else
        return false;
}

inline const char* ToString(RamBlockStatusControlEnum value)
{
    switch (value) {
        case RamBlockStatusControlEnum::kApi:
            return "API";
        case RamBlockStatusControlEnum::kNvRamManager:
            return "NV-RAM-MANAGER";
        default:
            return nullptr;
    }
}

enum class ReceiverIntentEnum
{
    kWillReceive = 0,  // The receiver will receive the event or field notifier.
    kWontReceive = 1,  // The receiver won't receive the event or field notifier.
};

template < typename StringType >
bool FromString(const StringType& string, ReceiverIntentEnum& value)
{
    if (std::strcmp(string.c_str(), "WILL-RECEIVE") == 0) {
        value = ReceiverIntentEnum::kWillReceive;
        return true;
    } else if (std::strcmp(string.c_str(), "WONT-RECEIVE") == 0) {
        value = ReceiverIntentEnum::kWontReceive;
        return true;
    } else
        return false;
}

inline const char* ToString(ReceiverIntentEnum value)
{
    switch (value) {
        case ReceiverIntentEnum::kWillReceive:
            return "WILL-RECEIVE";
        case ReceiverIntentEnum::kWontReceive:
            return "WONT-RECEIVE";
        default:
            return nullptr;
    }
}

enum class ReentrancyLevelEnum
{
    kMulticoreReentrant = 0,  // Unlimited concurrent execution of this entity is possible, including preemption and
                              // parallel execution on multi core systems.
    kNonReentrant = 1,        // Concurrent execution of this entity is not possible.
    kSingleCoreReentrant
    = 2,  // Pseudo-concurrent execution (i.e. preemption) of this entity is possible on single core systems.
};

template < typename StringType >
bool FromString(const StringType& string, ReentrancyLevelEnum& value)
{
    if (std::strcmp(string.c_str(), "MULTICORE-REENTRANT") == 0) {
        value = ReentrancyLevelEnum::kMulticoreReentrant;
        return true;
    } else if (std::strcmp(string.c_str(), "NON-REENTRANT") == 0) {
        value = ReentrancyLevelEnum::kNonReentrant;
        return true;
    } else if (std::strcmp(string.c_str(), "SINGLE-CORE-REENTRANT") == 0) {
        value = ReentrancyLevelEnum::kSingleCoreReentrant;
        return true;
    } else
        return false;
}

inline const char* ToString(ReentrancyLevelEnum value)
{
    switch (value) {
        case ReentrancyLevelEnum::kMulticoreReentrant:
            return "MULTICORE-REENTRANT";
        case ReentrancyLevelEnum::kNonReentrant:
            return "NON-REENTRANT";
        case ReentrancyLevelEnum::kSingleCoreReentrant:
            return "SINGLE-CORE-REENTRANT";
        default:
            return nullptr;
    }
}

enum class RemotingTechnologyEnum
{
    kArClientServer = 0,
    kSomeip         = 1,
};

template < typename StringType >
bool FromString(const StringType& string, RemotingTechnologyEnum& value)
{
    if (std::strcmp(string.c_str(), "AR--CLIENT--SERVER") == 0) {
        value = RemotingTechnologyEnum::kArClientServer;
        return true;
    } else if (std::strcmp(string.c_str(), "SOMEIP") == 0) {
        value = RemotingTechnologyEnum::kSomeip;
        return true;
    } else
        return false;
}

inline const char* ToString(RemotingTechnologyEnum value)
{
    switch (value) {
        case RemotingTechnologyEnum::kArClientServer:
            return "AR--CLIENT--SERVER";
        case RemotingTechnologyEnum::kSomeip:
            return "SOMEIP";
        default:
            return nullptr;
    }
}

enum class ReportBehaviorEnum
{
    kReportAfterInit  = 0,  // This allows reporting related events after initialization
    kReportBeforeInit = 1,  // This allows reporting related events before initialization
};

template < typename StringType >
bool FromString(const StringType& string, ReportBehaviorEnum& value)
{
    if (std::strcmp(string.c_str(), "REPORT-AFTER-INIT") == 0) {
        value = ReportBehaviorEnum::kReportAfterInit;
        return true;
    } else if (std::strcmp(string.c_str(), "REPORT-BEFORE-INIT") == 0) {
        value = ReportBehaviorEnum::kReportBeforeInit;
        return true;
    } else
        return false;
}

inline const char* ToString(ReportBehaviorEnum value)
{
    switch (value) {
        case ReportBehaviorEnum::kReportAfterInit:
            return "REPORT-AFTER-INIT";
        case ReportBehaviorEnum::kReportBeforeInit:
            return "REPORT-BEFORE-INIT";
        default:
            return nullptr;
    }
}

enum class RequestMethodEnum
{
    kConnect = 0,
    kDelete  = 1,
    kGet     = 2,
    kHead    = 3,
    kOptions = 4,
    kPost    = 5,
    kPut     = 6,
    kTrace   = 7,
};

template < typename StringType >
bool FromString(const StringType& string, RequestMethodEnum& value)
{
    if (std::strcmp(string.c_str(), "CONNECT") == 0) {
        value = RequestMethodEnum::kConnect;
        return true;
    } else if (std::strcmp(string.c_str(), "DELETE") == 0) {
        value = RequestMethodEnum::kDelete;
        return true;
    } else if (std::strcmp(string.c_str(), "GET") == 0) {
        value = RequestMethodEnum::kGet;
        return true;
    } else if (std::strcmp(string.c_str(), "HEAD") == 0) {
        value = RequestMethodEnum::kHead;
        return true;
    } else if (std::strcmp(string.c_str(), "OPTIONS") == 0) {
        value = RequestMethodEnum::kOptions;
        return true;
    } else if (std::strcmp(string.c_str(), "POST") == 0) {
        value = RequestMethodEnum::kPost;
        return true;
    } else if (std::strcmp(string.c_str(), "PUT") == 0) {
        value = RequestMethodEnum::kPut;
        return true;
    } else if (std::strcmp(string.c_str(), "TRACE") == 0) {
        value = RequestMethodEnum::kTrace;
        return true;
    } else
        return false;
}

inline const char* ToString(RequestMethodEnum value)
{
    switch (value) {
        case RequestMethodEnum::kConnect:
            return "CONNECT";
        case RequestMethodEnum::kDelete:
            return "DELETE";
        case RequestMethodEnum::kGet:
            return "GET";
        case RequestMethodEnum::kHead:
            return "HEAD";
        case RequestMethodEnum::kOptions:
            return "OPTIONS";
        case RequestMethodEnum::kPost:
            return "POST";
        case RequestMethodEnum::kPut:
            return "PUT";
        case RequestMethodEnum::kTrace:
            return "TRACE";
        default:
            return nullptr;
    }
}

enum class RequestTypeEnum
{
    kPhysical   = 0,  // This enum literal defines a PHYSICAL DiagnosticMessage request.
    kFunctional = 1,  // This enum literal defines a FUNCTIONAL DiagnosticMessage request.
};

template < typename StringType >
bool FromString(const StringType& string, RequestTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "PHYSICAL") == 0) {
        value = RequestTypeEnum::kPhysical;
        return true;
    } else if (std::strcmp(string.c_str(), "FUNCTIONAL") == 0) {
        value = RequestTypeEnum::kFunctional;
        return true;
    } else
        return false;
}

inline const char* ToString(RequestTypeEnum value)
{
    switch (value) {
        case RequestTypeEnum::kPhysical:
            return "PHYSICAL";
        case RequestTypeEnum::kFunctional:
            return "FUNCTIONAL";
        default:
            return nullptr;
    }
}

enum class ResolutionPolicyEnum
{
    kNoSloppy = 0,  // The content of the xref element is '''not''' linked by a sloppy reference.
    kSloppy   = 1,  // The content of the xref element is linked by a sloppy reference.
};

template < typename StringType >
bool FromString(const StringType& string, ResolutionPolicyEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-SLOPPY") == 0) {
        value = ResolutionPolicyEnum::kNoSloppy;
        return true;
    } else if (std::strcmp(string.c_str(), "SLOPPY") == 0) {
        value = ResolutionPolicyEnum::kSloppy;
        return true;
    } else
        return false;
}

inline const char* ToString(ResolutionPolicyEnum value)
{
    switch (value) {
        case ResolutionPolicyEnum::kNoSloppy:
            return "NO-SLOPPY";
        case ResolutionPolicyEnum::kSloppy:
            return "SLOPPY";
        default:
            return nullptr;
    }
}

enum class RptAccessEnum
{
    kEnabled   = 0,  // The related data element is accessible by RP tool.
    kNone      = 1,  // The related data element is not accessible by RP tool.
    kProtected = 2,  // The data element is known to the RP tool however its usage for RP can be restricted. Use case:
                     // limitation based on access rights
};

template < typename StringType >
bool FromString(const StringType& string, RptAccessEnum& value)
{
    if (std::strcmp(string.c_str(), "ENABLED") == 0) {
        value = RptAccessEnum::kEnabled;
        return true;
    } else if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = RptAccessEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "PROTECTED") == 0) {
        value = RptAccessEnum::kProtected;
        return true;
    } else
        return false;
}

inline const char* ToString(RptAccessEnum value)
{
    switch (value) {
        case RptAccessEnum::kEnabled:
            return "ENABLED";
        case RptAccessEnum::kNone:
            return "NONE";
        case RptAccessEnum::kProtected:
            return "PROTECTED";
        default:
            return nullptr;
    }
}

enum class RptEnablerImplTypeEnum
{
    kNone                = 0,  // No "RP enabler" is implemented.
    kRptEnablerRam       = 1,  // "RP enabler" is implemented as a RAM variable
    kRptEnablerRom       = 2,  // "RP enabler" is implemented as a calibrateable ROM variable.
    kRptEnablerRamAndRom = 3,  // The RTE generator implements both the RAM and ROM "RP enabler".
};

template < typename StringType >
bool FromString(const StringType& string, RptEnablerImplTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = RptEnablerImplTypeEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "RPT-ENABLER-RAM") == 0) {
        value = RptEnablerImplTypeEnum::kRptEnablerRam;
        return true;
    } else if (std::strcmp(string.c_str(), "RPT-ENABLER-ROM") == 0) {
        value = RptEnablerImplTypeEnum::kRptEnablerRom;
        return true;
    } else if (std::strcmp(string.c_str(), "RPT-ENABLER-RAM-AND-ROM") == 0) {
        value = RptEnablerImplTypeEnum::kRptEnablerRamAndRom;
        return true;
    } else
        return false;
}

inline const char* ToString(RptEnablerImplTypeEnum value)
{
    switch (value) {
        case RptEnablerImplTypeEnum::kNone:
            return "NONE";
        case RptEnablerImplTypeEnum::kRptEnablerRam:
            return "RPT-ENABLER-RAM";
        case RptEnablerImplTypeEnum::kRptEnablerRom:
            return "RPT-ENABLER-ROM";
        case RptEnablerImplTypeEnum::kRptEnablerRamAndRom:
            return "RPT-ENABLER-RAM-AND-ROM";
        default:
            return nullptr;
    }
}

enum class RptExecutionControlEnum
{
    kConditional = 0,  // The ExecutableEntity is only executed when the rapid prototyping disable flag is NOT set.
    kNone        = 1,  // The ExecutableEntity is executed without specific rapid prototyping condition.
};

template < typename StringType >
bool FromString(const StringType& string, RptExecutionControlEnum& value)
{
    if (std::strcmp(string.c_str(), "CONDITIONAL") == 0) {
        value = RptExecutionControlEnum::kConditional;
        return true;
    } else if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = RptExecutionControlEnum::kNone;
        return true;
    } else
        return false;
}

inline const char* ToString(RptExecutionControlEnum value)
{
    switch (value) {
        case RptExecutionControlEnum::kConditional:
            return "CONDITIONAL";
        case RptExecutionControlEnum::kNone:
            return "NONE";
        default:
            return nullptr;
    }
}

enum class RptPreparationEnum
{
    kNone = 0,  // No RP preparation for VariableDataPrototype.
    kRptLevel1
    = 1,  // The RTE implementation uses an „RP global buffer" for measurement and post-build hooking purposes.
    kRptLevel2 = 2,  // As rpLevel1 but the RTE implementation also uses both „RP enabler flag" to permit RP overwrite
                     // at run-time.
    kRptLevel3 = 3,  // As rpLevel2 but the RTE implementation also uses "RP global measurement buffer" to record the
                     // original ECU-generated value in addition to the RP value.
};

template < typename StringType >
bool FromString(const StringType& string, RptPreparationEnum& value)
{
    if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = RptPreparationEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "RPT-LEVEL-1") == 0) {
        value = RptPreparationEnum::kRptLevel1;
        return true;
    } else if (std::strcmp(string.c_str(), "RPT-LEVEL-2") == 0) {
        value = RptPreparationEnum::kRptLevel2;
        return true;
    } else if (std::strcmp(string.c_str(), "RPT-LEVEL-3") == 0) {
        value = RptPreparationEnum::kRptLevel3;
        return true;
    } else
        return false;
}

inline const char* ToString(RptPreparationEnum value)
{
    switch (value) {
        case RptPreparationEnum::kNone:
            return "NONE";
        case RptPreparationEnum::kRptLevel1:
            return "RPT-LEVEL-1";
        case RptPreparationEnum::kRptLevel2:
            return "RPT-LEVEL-2";
        case RptPreparationEnum::kRptLevel3:
            return "RPT-LEVEL-3";
        default:
            return nullptr;
    }
}

enum class RptServicePointEnum
{
    kEnabled = 0,  // Enables generation of service points by the RTE generator.
    kNone    = 1,  // No Service Points are requested.
};

template < typename StringType >
bool FromString(const StringType& string, RptServicePointEnum& value)
{
    if (std::strcmp(string.c_str(), "ENABLED") == 0) {
        value = RptServicePointEnum::kEnabled;
        return true;
    } else if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = RptServicePointEnum::kNone;
        return true;
    } else
        return false;
}

inline const char* ToString(RptServicePointEnum value)
{
    switch (value) {
        case RptServicePointEnum::kEnabled:
            return "ENABLED";
        case RptServicePointEnum::kNone:
            return "NONE";
        default:
            return nullptr;
    }
}

enum class RteApiReturnValueProvisionEnum
{
    kReturnValueProvided   = 0,  // The RTE API shall provide a return value.
    kNoReturnValueProvided = 1,  // The RTE API shall not provide a return value.
};

template < typename StringType >
bool FromString(const StringType& string, RteApiReturnValueProvisionEnum& value)
{
    if (std::strcmp(string.c_str(), "RETURN-VALUE-PROVIDED") == 0) {
        value = RteApiReturnValueProvisionEnum::kReturnValueProvided;
        return true;
    } else if (std::strcmp(string.c_str(), "NO-RETURN-VALUE-PROVIDED") == 0) {
        value = RteApiReturnValueProvisionEnum::kNoReturnValueProvided;
        return true;
    } else
        return false;
}

inline const char* ToString(RteApiReturnValueProvisionEnum value)
{
    switch (value) {
        case RteApiReturnValueProvisionEnum::kReturnValueProvided:
            return "RETURN-VALUE-PROVIDED";
        case RteApiReturnValueProvisionEnum::kNoReturnValueProvided:
            return "NO-RETURN-VALUE-PROVIDED";
        default:
            return nullptr;
    }
}

enum class RuntimeAddressConfigurationEnum
{
    kNone = 0,  // Static configuration is used to obtain the address information.
    kSd   = 1,  // AUTOSAR Service Discovery is used to obtain the address information.
};

template < typename StringType >
bool FromString(const StringType& string, RuntimeAddressConfigurationEnum& value)
{
    if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = RuntimeAddressConfigurationEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "SD") == 0) {
        value = RuntimeAddressConfigurationEnum::kSd;
        return true;
    } else
        return false;
}

inline const char* ToString(RuntimeAddressConfigurationEnum value)
{
    switch (value) {
        case RuntimeAddressConfigurationEnum::kNone:
            return "NONE";
        case RuntimeAddressConfigurationEnum::kSd:
            return "SD";
        default:
            return nullptr;
    }
}

enum class RxAcceptContainedIPduEnum
{
    kAcceptAll = 0,  // No fixed set of containedIPdus is defined for reception, any known containedIPdu (based on
                     // headerId) shall be expected within this ContainerIPdu.
    kAcceptConfigured = 1,  // A fixed set of containedIPdus is defined for reception. Only these assigned
                            // containedIPdus (based on headerId) are expected in this ContainerIPdu. If a not assigned
                            // containedIPdu is received within this ContainerIPdu this containedIPdu is discarded.
};

template < typename StringType >
bool FromString(const StringType& string, RxAcceptContainedIPduEnum& value)
{
    if (std::strcmp(string.c_str(), "ACCEPT-ALL") == 0) {
        value = RxAcceptContainedIPduEnum::kAcceptAll;
        return true;
    } else if (std::strcmp(string.c_str(), "ACCEPT-CONFIGURED") == 0) {
        value = RxAcceptContainedIPduEnum::kAcceptConfigured;
        return true;
    } else
        return false;
}

inline const char* ToString(RxAcceptContainedIPduEnum value)
{
    switch (value) {
        case RxAcceptContainedIPduEnum::kAcceptAll:
            return "ACCEPT-ALL";
        case RxAcceptContainedIPduEnum::kAcceptConfigured:
            return "ACCEPT-CONFIGURED";
        default:
            return nullptr;
    }
}

enum class ScaleConstrValidityEnum
{
    kNotAvailable = 0,  // Currently invalid area The value usually is presented by the ECU but can currently not be
                        // performed due to e.g. initialization or temporary problems. Please note, that this behavior
                        // appears during runtime and cannot be handled while data is edited.
    kNotDefined = 1,    // Indicates an area which is marked in a specification (e.g. as reserved) Shall usually not be
                        // set by the ECU but is used by a tester to verify correct ECU.
    kNotValid = 2,      // The ECU cannot process the requested data.
    kValid    = 3,      // Current value is within a valid range and can be presented to user as is.
};

template < typename StringType >
bool FromString(const StringType& string, ScaleConstrValidityEnum& value)
{
    if (std::strcmp(string.c_str(), "NOT-AVAILABLE") == 0) {
        value = ScaleConstrValidityEnum::kNotAvailable;
        return true;
    } else if (std::strcmp(string.c_str(), "NOT-DEFINED") == 0) {
        value = ScaleConstrValidityEnum::kNotDefined;
        return true;
    } else if (std::strcmp(string.c_str(), "NOT-VALID") == 0) {
        value = ScaleConstrValidityEnum::kNotValid;
        return true;
    } else if (std::strcmp(string.c_str(), "VALID") == 0) {
        value = ScaleConstrValidityEnum::kValid;
        return true;
    } else
        return false;
}

inline const char* ToString(ScaleConstrValidityEnum value)
{
    switch (value) {
        case ScaleConstrValidityEnum::kNotAvailable:
            return "NOT-AVAILABLE";
        case ScaleConstrValidityEnum::kNotDefined:
            return "NOT-DEFINED";
        case ScaleConstrValidityEnum::kNotValid:
            return "NOT-VALID";
        case ScaleConstrValidityEnum::kValid:
            return "VALID";
        default:
            return nullptr;
    }
}

enum class SearchIntentionEnum
{
    kSearchForAll = 0,  // This value represents the intention to search for all instances of the given service
    kSearchForId  = 1,  // This value represents the intention to search for a dedicated instance of the given service.
};

template < typename StringType >
bool FromString(const StringType& string, SearchIntentionEnum& value)
{
    if (std::strcmp(string.c_str(), "SEARCH-FOR-ALL") == 0) {
        value = SearchIntentionEnum::kSearchForAll;
        return true;
    } else if (std::strcmp(string.c_str(), "SEARCH-FOR-ID") == 0) {
        value = SearchIntentionEnum::kSearchForId;
        return true;
    } else
        return false;
}

inline const char* ToString(SearchIntentionEnum value)
{
    switch (value) {
        case SearchIntentionEnum::kSearchForAll:
            return "SEARCH-FOR-ALL";
        case SearchIntentionEnum::kSearchForId:
            return "SEARCH-FOR-ID";
        default:
            return nullptr;
    }
}

enum class SecOcJobSemanticEnum
{
    kAuthenticate = 0,  // Authentication algorithm for Authenticator generation/verification.
    kVerify       = 1,  // Asymmetric cryptographic algorithm to generate/verify a signature
};

template < typename StringType >
bool FromString(const StringType& string, SecOcJobSemanticEnum& value)
{
    if (std::strcmp(string.c_str(), "AUTHENTICATE") == 0) {
        value = SecOcJobSemanticEnum::kAuthenticate;
        return true;
    } else if (std::strcmp(string.c_str(), "VERIFY") == 0) {
        value = SecOcJobSemanticEnum::kVerify;
        return true;
    } else
        return false;
}

inline const char* ToString(SecOcJobSemanticEnum value)
{
    switch (value) {
        case SecOcJobSemanticEnum::kAuthenticate:
            return "AUTHENTICATE";
        case SecOcJobSemanticEnum::kVerify:
            return "VERIFY";
        default:
            return nullptr;
    }
}

enum class SecuredPduHeaderEnum
{
    kNoHeader              = 0,  // No header included in the SecuredPdu.
    kSecuredPduHeader08Bit = 1,  // 8 Bit Secured I-PDU Header included in the Secured I-PDU.
    kSecuredPduHeader16Bit = 2,  // 16 Bit Secured I-PDU Header included in the Secured I-PDU.
    kSecuredPduHeader32Bit = 3,  // 32 Bit Secured I-PDU Header included in the Secured I-PDU.
};

template < typename StringType >
bool FromString(const StringType& string, SecuredPduHeaderEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-HEADER") == 0) {
        value = SecuredPduHeaderEnum::kNoHeader;
        return true;
    } else if (std::strcmp(string.c_str(), "SECURED-PDU-HEADER-08-BIT") == 0) {
        value = SecuredPduHeaderEnum::kSecuredPduHeader08Bit;
        return true;
    } else if (std::strcmp(string.c_str(), "SECURED-PDU-HEADER-16-BIT") == 0) {
        value = SecuredPduHeaderEnum::kSecuredPduHeader16Bit;
        return true;
    } else if (std::strcmp(string.c_str(), "SECURED-PDU-HEADER-32-BIT") == 0) {
        value = SecuredPduHeaderEnum::kSecuredPduHeader32Bit;
        return true;
    } else
        return false;
}

inline const char* ToString(SecuredPduHeaderEnum value)
{
    switch (value) {
        case SecuredPduHeaderEnum::kNoHeader:
            return "NO-HEADER";
        case SecuredPduHeaderEnum::kSecuredPduHeader08Bit:
            return "SECURED-PDU-HEADER-08-BIT";
        case SecuredPduHeaderEnum::kSecuredPduHeader16Bit:
            return "SECURED-PDU-HEADER-16-BIT";
        case SecuredPduHeaderEnum::kSecuredPduHeader32Bit:
            return "SECURED-PDU-HEADER-32-BIT";
        default:
            return nullptr;
    }
}

enum class SecurityEventContextDataSourceEnum
{
    kUseFirstContextData
    = 0,  // Context data of first received security event shall be used for resulting qualified security event.
    kUseLastContextData
    = 1,  // Context data of last received security event shall be used for resulting qualified security event.
};

template < typename StringType >
bool FromString(const StringType& string, SecurityEventContextDataSourceEnum& value)
{
    if (std::strcmp(string.c_str(), "USE-FIRST-CONTEXT-DATA") == 0) {
        value = SecurityEventContextDataSourceEnum::kUseFirstContextData;
        return true;
    } else if (std::strcmp(string.c_str(), "USE-LAST-CONTEXT-DATA") == 0) {
        value = SecurityEventContextDataSourceEnum::kUseLastContextData;
        return true;
    } else
        return false;
}

inline const char* ToString(SecurityEventContextDataSourceEnum value)
{
    switch (value) {
        case SecurityEventContextDataSourceEnum::kUseFirstContextData:
            return "USE-FIRST-CONTEXT-DATA";
        case SecurityEventContextDataSourceEnum::kUseLastContextData:
            return "USE-LAST-CONTEXT-DATA";
        default:
            return nullptr;
    }
}

enum class SecurityEventReportingModeEnum
{
    kOff   = 0,     // The reported security event is not further processed by the IdsM and therefore discarded.
    kBrief = 1,     // Only the main security event properties such as its ID are processed. Any additional context data
                    // (if existing) is discarded.
    kDetailed = 2,  // The main properties and the context data (if existing) of the reported security event are
                    // processed further.
    kBriefBypassingFilters = 3,     // The reported security event without its context data (if existing) is processed
                                    // further but the filter chain is bypassed.
    kDetailedBypassingFilters = 4,  // The reported security event including its context data (if existing) is processed
                                    // further but the filter chain is bypassed.
};

template < typename StringType >
bool FromString(const StringType& string, SecurityEventReportingModeEnum& value)
{
    if (std::strcmp(string.c_str(), "OFF") == 0) {
        value = SecurityEventReportingModeEnum::kOff;
        return true;
    } else if (std::strcmp(string.c_str(), "BRIEF") == 0) {
        value = SecurityEventReportingModeEnum::kBrief;
        return true;
    } else if (std::strcmp(string.c_str(), "DETAILED") == 0) {
        value = SecurityEventReportingModeEnum::kDetailed;
        return true;
    } else if (std::strcmp(string.c_str(), "BRIEF-BYPASSING-FILTERS") == 0) {
        value = SecurityEventReportingModeEnum::kBriefBypassingFilters;
        return true;
    } else if (std::strcmp(string.c_str(), "DETAILED-BYPASSING-FILTERS") == 0) {
        value = SecurityEventReportingModeEnum::kDetailedBypassingFilters;
        return true;
    } else
        return false;
}

inline const char* ToString(SecurityEventReportingModeEnum value)
{
    switch (value) {
        case SecurityEventReportingModeEnum::kOff:
            return "OFF";
        case SecurityEventReportingModeEnum::kBrief:
            return "BRIEF";
        case SecurityEventReportingModeEnum::kDetailed:
            return "DETAILED";
        case SecurityEventReportingModeEnum::kBriefBypassingFilters:
            return "BRIEF-BYPASSING-FILTERS";
        case SecurityEventReportingModeEnum::kDetailedBypassingFilters:
            return "DETAILED-BYPASSING-FILTERS";
        default:
            return nullptr;
    }
}

enum class SendIndicationEnum
{
    kNone = 1,  // This value represents the requirement that send operations of the Software Cluster are not indicated.
    kAnySendOperation
    = 2,  // This value represents the requirement that any send operation of the Software Cluster is indicated.
};

template < typename StringType >
bool FromString(const StringType& string, SendIndicationEnum& value)
{
    if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = SendIndicationEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "ANY-SEND-OPERATION") == 0) {
        value = SendIndicationEnum::kAnySendOperation;
        return true;
    } else
        return false;
}

inline const char* ToString(SendIndicationEnum value)
{
    switch (value) {
        case SendIndicationEnum::kNone:
            return "NONE";
        case SendIndicationEnum::kAnySendOperation:
            return "ANY-SEND-OPERATION";
        default:
            return nullptr;
    }
}

enum class SenderIntentEnum
{
    kWillSend = 0,  // The sender will send the event or field notifier.
    kWontSend = 1,  // The sender won't send the event or field notifier.
};

template < typename StringType >
bool FromString(const StringType& string, SenderIntentEnum& value)
{
    if (std::strcmp(string.c_str(), "WILL-SEND") == 0) {
        value = SenderIntentEnum::kWillSend;
        return true;
    } else if (std::strcmp(string.c_str(), "WONT-SEND") == 0) {
        value = SenderIntentEnum::kWontSend;
        return true;
    } else
        return false;
}

inline const char* ToString(SenderIntentEnum value)
{
    switch (value) {
        case SenderIntentEnum::kWillSend:
            return "WILL-SEND";
        case SenderIntentEnum::kWontSend:
            return "WONT-SEND";
        default:
            return nullptr;
    }
}

enum class SerializationTechnologyEnum
{
    kSomeip      = 0,  // SOME/IP Serializer
    kSignalBased = 1,  // Signal-Based serializer.
};

template < typename StringType >
bool FromString(const StringType& string, SerializationTechnologyEnum& value)
{
    if (std::strcmp(string.c_str(), "SOMEIP") == 0) {
        value = SerializationTechnologyEnum::kSomeip;
        return true;
    } else if (std::strcmp(string.c_str(), "SIGNAL-BASED") == 0) {
        value = SerializationTechnologyEnum::kSignalBased;
        return true;
    } else
        return false;
}

inline const char* ToString(SerializationTechnologyEnum value)
{
    switch (value) {
        case SerializationTechnologyEnum::kSomeip:
            return "SOMEIP";
        case SerializationTechnologyEnum::kSignalBased:
            return "SIGNAL-BASED";
        default:
            return nullptr;
    }
}

enum class ServerArgumentImplPolicyEnum
{
    kUseArgumentType
    = 0,  // The argument type of the RunnableEntity is derived from the AutosarDataType of the ArgumentPrototype.
    kUseArrayBaseType
    = 1,  // The argument type of the RunnableEntity is derived from the AutosarDataType of the elements of the array
          // that corresponds to the ArgumentPrototype. This represents the base type of the array in C.
    kUseVoid = 2,  // The argument type of the RunnableEntity is void.
};

template < typename StringType >
bool FromString(const StringType& string, ServerArgumentImplPolicyEnum& value)
{
    if (std::strcmp(string.c_str(), "USE-ARGUMENT-TYPE") == 0) {
        value = ServerArgumentImplPolicyEnum::kUseArgumentType;
        return true;
    } else if (std::strcmp(string.c_str(), "USE-ARRAY-BASE-TYPE") == 0) {
        value = ServerArgumentImplPolicyEnum::kUseArrayBaseType;
        return true;
    } else if (std::strcmp(string.c_str(), "USE-VOID") == 0) {
        value = ServerArgumentImplPolicyEnum::kUseVoid;
        return true;
    } else
        return false;
}

inline const char* ToString(ServerArgumentImplPolicyEnum value)
{
    switch (value) {
        case ServerArgumentImplPolicyEnum::kUseArgumentType:
            return "USE-ARGUMENT-TYPE";
        case ServerArgumentImplPolicyEnum::kUseArrayBaseType:
            return "USE-ARRAY-BASE-TYPE";
        case ServerArgumentImplPolicyEnum::kUseVoid:
            return "USE-VOID";
        default:
            return nullptr;
    }
}

enum class ServiceProviderEnum
{
    kAnyStandardized
    = 0,  // This value means that the specific nature is either unknown or it is not important for the given purpose.
          // This is also the default value for any attribute of type ServiceProviderEnum
    kBasicSoftwareModeManager       = 1,   // The service relates to the Basic Software Mode Manager (BswM)
    kComManager                     = 2,   // The service relates to the COM Manager (ComM).
    kCryptoServiceManager           = 3,   // The service relates to the Crypto Service Manager (CsM).
    kDefaultErrorTracer             = 4,   // The service relates to the Default Error Tracer (DET)
    kDevelopmentErrorTracer         = 5,   // The service relates to the Development Error Tracer (DET).
    kDiagnosticCommunicationManager = 6,   // The service relates to the Diagnostic Communication Manager (DCM).
    kDiagnosticEventManager         = 7,   // The service relates to the Diagnostic Event Manager (DEM).
    kDiagnosticLogAndTrace          = 8,   // The service relates to the Diagnostic Log and Trace (DLT).
    kEcuManager                     = 9,   // The service relates to the ECU Manager (EcuM).
    kFunctionInhibitionManager      = 10,  // The service relates to the Function Inhibition Manager (FIM).
    kJ1939RequestManager            = 11,  // The service relates to the J1939Rm.
    kNonVolatileRamManager          = 12,  // The service relates to the Non-Volatile RAM Manager (NvM).
    kOperatingSystem                = 13,  // The service relates to the Operating System (OS).
    kSecureOnBoardCommunication     = 14,  // The service relates to the SecOc module.
    kSyncBaseTimeManager            = 15,  // The service relates to the Sync Time Base Manager (StbM).
    kVendorSpecific                 = 16,  // This value denotes a vendor-specific service.
    kWatchDogManager                = 17,  // The service relates to the Watchdog Manager (WdgM).
    kErrorTracer                    = 18,  // This service relates to the error tracer.
    kHardwareTestManager            = 19,  // This service relates to the hardware test manager.
    kV2XFacilities                  = 20,  // This service relates to the Vehicle to X facilities.
    kV2XManagement                  = 21,  // This service relates to the Vehicle to X management.
    kJ1939Dcm                       = 22,
};

template < typename StringType >
bool FromString(const StringType& string, ServiceProviderEnum& value)
{
    if (std::strcmp(string.c_str(), "ANY-STANDARDIZED") == 0) {
        value = ServiceProviderEnum::kAnyStandardized;
        return true;
    } else if (std::strcmp(string.c_str(), "BASIC-SOFTWARE-MODE-MANAGER") == 0) {
        value = ServiceProviderEnum::kBasicSoftwareModeManager;
        return true;
    } else if (std::strcmp(string.c_str(), "COM-MANAGER") == 0) {
        value = ServiceProviderEnum::kComManager;
        return true;
    } else if (std::strcmp(string.c_str(), "CRYPTO-SERVICE-MANAGER") == 0) {
        value = ServiceProviderEnum::kCryptoServiceManager;
        return true;
    } else if (std::strcmp(string.c_str(), "DEFAULT-ERROR-TRACER") == 0) {
        value = ServiceProviderEnum::kDefaultErrorTracer;
        return true;
    } else if (std::strcmp(string.c_str(), "DEVELOPMENT-ERROR-TRACER") == 0) {
        value = ServiceProviderEnum::kDevelopmentErrorTracer;
        return true;
    } else if (std::strcmp(string.c_str(), "DIAGNOSTIC-COMMUNICATION-MANAGER") == 0) {
        value = ServiceProviderEnum::kDiagnosticCommunicationManager;
        return true;
    } else if (std::strcmp(string.c_str(), "DIAGNOSTIC-EVENT-MANAGER") == 0) {
        value = ServiceProviderEnum::kDiagnosticEventManager;
        return true;
    } else if (std::strcmp(string.c_str(), "DIAGNOSTIC-LOG-AND-TRACE") == 0) {
        value = ServiceProviderEnum::kDiagnosticLogAndTrace;
        return true;
    } else if (std::strcmp(string.c_str(), "ECU-MANAGER") == 0) {
        value = ServiceProviderEnum::kEcuManager;
        return true;
    } else if (std::strcmp(string.c_str(), "FUNCTION-INHIBITION-MANAGER") == 0) {
        value = ServiceProviderEnum::kFunctionInhibitionManager;
        return true;
    } else if (std::strcmp(string.c_str(), "J-1939-REQUEST-MANAGER") == 0) {
        value = ServiceProviderEnum::kJ1939RequestManager;
        return true;
    } else if (std::strcmp(string.c_str(), "NON-VOLATILE-RAM-MANAGER") == 0) {
        value = ServiceProviderEnum::kNonVolatileRamManager;
        return true;
    } else if (std::strcmp(string.c_str(), "OPERATING-SYSTEM") == 0) {
        value = ServiceProviderEnum::kOperatingSystem;
        return true;
    } else if (std::strcmp(string.c_str(), "SECURE-ON-BOARD-COMMUNICATION") == 0) {
        value = ServiceProviderEnum::kSecureOnBoardCommunication;
        return true;
    } else if (std::strcmp(string.c_str(), "SYNC-BASE-TIME-MANAGER") == 0) {
        value = ServiceProviderEnum::kSyncBaseTimeManager;
        return true;
    } else if (std::strcmp(string.c_str(), "VENDOR-SPECIFIC") == 0) {
        value = ServiceProviderEnum::kVendorSpecific;
        return true;
    } else if (std::strcmp(string.c_str(), "WATCH-DOG-MANAGER") == 0) {
        value = ServiceProviderEnum::kWatchDogManager;
        return true;
    } else if (std::strcmp(string.c_str(), "ERROR-TRACER") == 0) {
        value = ServiceProviderEnum::kErrorTracer;
        return true;
    } else if (std::strcmp(string.c_str(), "HARDWARE-TEST-MANAGER") == 0) {
        value = ServiceProviderEnum::kHardwareTestManager;
        return true;
    } else if (std::strcmp(string.c_str(), "V-2-X-FACILITIES") == 0) {
        value = ServiceProviderEnum::kV2XFacilities;
        return true;
    } else if (std::strcmp(string.c_str(), "V-2-X-MANAGEMENT") == 0) {
        value = ServiceProviderEnum::kV2XManagement;
        return true;
    } else if (std::strcmp(string.c_str(), "J-1939-DCM") == 0) {
        value = ServiceProviderEnum::kJ1939Dcm;
        return true;
    } else
        return false;
}

inline const char* ToString(ServiceProviderEnum value)
{
    switch (value) {
        case ServiceProviderEnum::kAnyStandardized:
            return "ANY-STANDARDIZED";
        case ServiceProviderEnum::kBasicSoftwareModeManager:
            return "BASIC-SOFTWARE-MODE-MANAGER";
        case ServiceProviderEnum::kComManager:
            return "COM-MANAGER";
        case ServiceProviderEnum::kCryptoServiceManager:
            return "CRYPTO-SERVICE-MANAGER";
        case ServiceProviderEnum::kDefaultErrorTracer:
            return "DEFAULT-ERROR-TRACER";
        case ServiceProviderEnum::kDevelopmentErrorTracer:
            return "DEVELOPMENT-ERROR-TRACER";
        case ServiceProviderEnum::kDiagnosticCommunicationManager:
            return "DIAGNOSTIC-COMMUNICATION-MANAGER";
        case ServiceProviderEnum::kDiagnosticEventManager:
            return "DIAGNOSTIC-EVENT-MANAGER";
        case ServiceProviderEnum::kDiagnosticLogAndTrace:
            return "DIAGNOSTIC-LOG-AND-TRACE";
        case ServiceProviderEnum::kEcuManager:
            return "ECU-MANAGER";
        case ServiceProviderEnum::kFunctionInhibitionManager:
            return "FUNCTION-INHIBITION-MANAGER";
        case ServiceProviderEnum::kJ1939RequestManager:
            return "J-1939-REQUEST-MANAGER";
        case ServiceProviderEnum::kNonVolatileRamManager:
            return "NON-VOLATILE-RAM-MANAGER";
        case ServiceProviderEnum::kOperatingSystem:
            return "OPERATING-SYSTEM";
        case ServiceProviderEnum::kSecureOnBoardCommunication:
            return "SECURE-ON-BOARD-COMMUNICATION";
        case ServiceProviderEnum::kSyncBaseTimeManager:
            return "SYNC-BASE-TIME-MANAGER";
        case ServiceProviderEnum::kVendorSpecific:
            return "VENDOR-SPECIFIC";
        case ServiceProviderEnum::kWatchDogManager:
            return "WATCH-DOG-MANAGER";
        case ServiceProviderEnum::kErrorTracer:
            return "ERROR-TRACER";
        case ServiceProviderEnum::kHardwareTestManager:
            return "HARDWARE-TEST-MANAGER";
        case ServiceProviderEnum::kV2XFacilities:
            return "V-2-X-FACILITIES";
        case ServiceProviderEnum::kV2XManagement:
            return "V-2-X-MANAGEMENT";
        case ServiceProviderEnum::kJ1939Dcm:
            return "J-1939-DCM";
        default:
            return nullptr;
    }
}

enum class ServiceVersionAcceptanceKindEnum
{
    kExactOrAnyMinorVersion = 0,  // Search for ANY or specific minor version service instance and select either ALL
                                  // returned service instances (in case of ANY) or exactly the specific minor version
                                  // service instances defined in requiredMinorVersion.
    kMinimumMinorVersion = 1,  // Search for ANY minor version service instance and select only those service instances
                               // which have an equal or greater minor version than given in requiredMinorVersion.
};

template < typename StringType >
bool FromString(const StringType& string, ServiceVersionAcceptanceKindEnum& value)
{
    if (std::strcmp(string.c_str(), "EXACT-OR-ANY-MINOR-VERSION") == 0) {
        value = ServiceVersionAcceptanceKindEnum::kExactOrAnyMinorVersion;
        return true;
    } else if (std::strcmp(string.c_str(), "MINIMUM-MINOR-VERSION") == 0) {
        value = ServiceVersionAcceptanceKindEnum::kMinimumMinorVersion;
        return true;
    } else
        return false;
}

inline const char* ToString(ServiceVersionAcceptanceKindEnum value)
{
    switch (value) {
        case ServiceVersionAcceptanceKindEnum::kExactOrAnyMinorVersion:
            return "EXACT-OR-ANY-MINOR-VERSION";
        case ServiceVersionAcceptanceKindEnum::kMinimumMinorVersion:
            return "MINIMUM-MINOR-VERSION";
        default:
            return nullptr;
    }
}

enum class SeverityEnum
{
    kInfo    = 0,  // Something was found that is worth mentioning. Low risk of interoperability issues.
    kWarning = 1,  // Something might be wrong depending on the context. Medium risk of interoperability issues.
    kError   = 2,  // Something is not right. High risk of interoperability issues.
};

template < typename StringType >
bool FromString(const StringType& string, SeverityEnum& value)
{
    if (std::strcmp(string.c_str(), "INFO") == 0) {
        value = SeverityEnum::kInfo;
        return true;
    } else if (std::strcmp(string.c_str(), "WARNING") == 0) {
        value = SeverityEnum::kWarning;
        return true;
    } else if (std::strcmp(string.c_str(), "ERROR") == 0) {
        value = SeverityEnum::kError;
        return true;
    } else
        return false;
}

inline const char* ToString(SeverityEnum value)
{
    switch (value) {
        case SeverityEnum::kInfo:
            return "INFO";
        case SeverityEnum::kWarning:
            return "WARNING";
        case SeverityEnum::kError:
            return "ERROR";
        default:
            return nullptr;
    }
}

enum class ShowContentEnum
{
    kNoShowContent = 0,  // The content of the Xref.label is '''not''' rendered at the place of the reference.
    kShowContent   = 1,  // The content of the element is rendered at the place of the reference.
};

template < typename StringType >
bool FromString(const StringType& string, ShowContentEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-SHOW-CONTENT") == 0) {
        value = ShowContentEnum::kNoShowContent;
        return true;
    } else if (std::strcmp(string.c_str(), "SHOW-CONTENT") == 0) {
        value = ShowContentEnum::kShowContent;
        return true;
    } else
        return false;
}

inline const char* ToString(ShowContentEnum value)
{
    switch (value) {
        case ShowContentEnum::kNoShowContent:
            return "NO-SHOW-CONTENT";
        case ShowContentEnum::kShowContent:
            return "SHOW-CONTENT";
        default:
            return nullptr;
    }
}

enum class ShowResourceAliasNameEnum
{
    kNoShowAliasName = 0,  // This indicates that alias names of the referenced object shall '''not''' be rendered at
                           // the place of the reference.
    kShowAliasName = 1,  // This indicates that the alias names of the referenced object shall be rendered at the place
                         // of the reference.
};

template < typename StringType >
bool FromString(const StringType& string, ShowResourceAliasNameEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-SHOW-ALIAS-NAME") == 0) {
        value = ShowResourceAliasNameEnum::kNoShowAliasName;
        return true;
    } else if (std::strcmp(string.c_str(), "SHOW-ALIAS-NAME") == 0) {
        value = ShowResourceAliasNameEnum::kShowAliasName;
        return true;
    } else
        return false;
}

inline const char* ToString(ShowResourceAliasNameEnum value)
{
    switch (value) {
        case ShowResourceAliasNameEnum::kNoShowAliasName:
            return "NO-SHOW-ALIAS-NAME";
        case ShowResourceAliasNameEnum::kShowAliasName:
            return "SHOW-ALIAS-NAME";
        default:
            return nullptr;
    }
}

enum class ShowResourceCategoryEnum
{
    kNoShowCategory = 0,  // The category of the target is '''not''' rendered at the place of the reference.
    kShowCategory   = 1,  // The category of the target is  rendered at the place of the reference.
};

template < typename StringType >
bool FromString(const StringType& string, ShowResourceCategoryEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-SHOW-CATEGORY") == 0) {
        value = ShowResourceCategoryEnum::kNoShowCategory;
        return true;
    } else if (std::strcmp(string.c_str(), "SHOW-CATEGORY") == 0) {
        value = ShowResourceCategoryEnum::kShowCategory;
        return true;
    } else
        return false;
}

inline const char* ToString(ShowResourceCategoryEnum value)
{
    switch (value) {
        case ShowResourceCategoryEnum::kNoShowCategory:
            return "NO-SHOW-CATEGORY";
        case ShowResourceCategoryEnum::kShowCategory:
            return "SHOW-CATEGORY";
        default:
            return nullptr;
    }
}

enum class ShowResourceLongNameEnum
{
    kNoShowLongName = 0,  // The long name of the target is '''not''' rendered at the place of the reference.
    kShowLongName   = 1,  // The long name of the target is rendered at the place of the reference.
};

template < typename StringType >
bool FromString(const StringType& string, ShowResourceLongNameEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-SHOW-LONG-NAME") == 0) {
        value = ShowResourceLongNameEnum::kNoShowLongName;
        return true;
    } else if (std::strcmp(string.c_str(), "SHOW-LONG-NAME") == 0) {
        value = ShowResourceLongNameEnum::kShowLongName;
        return true;
    } else
        return false;
}

inline const char* ToString(ShowResourceLongNameEnum value)
{
    switch (value) {
        case ShowResourceLongNameEnum::kNoShowLongName:
            return "NO-SHOW-LONG-NAME";
        case ShowResourceLongNameEnum::kShowLongName:
            return "SHOW-LONG-NAME";
        default:
            return nullptr;
    }
}

enum class ShowResourceNumberEnum
{
    kNoShowNumber = 0,  // The number of the target is '''not''' rendered at the place of the reference.
    kShowNumber   = 1,  // The number of the target is rendered at the place of the reference.
};

template < typename StringType >
bool FromString(const StringType& string, ShowResourceNumberEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-SHOW-NUMBER") == 0) {
        value = ShowResourceNumberEnum::kNoShowNumber;
        return true;
    } else if (std::strcmp(string.c_str(), "SHOW-NUMBER") == 0) {
        value = ShowResourceNumberEnum::kShowNumber;
        return true;
    } else
        return false;
}

inline const char* ToString(ShowResourceNumberEnum value)
{
    switch (value) {
        case ShowResourceNumberEnum::kNoShowNumber:
            return "NO-SHOW-NUMBER";
        case ShowResourceNumberEnum::kShowNumber:
            return "SHOW-NUMBER";
        default:
            return nullptr;
    }
}

enum class ShowResourcePageEnum
{
    kNoShowPage = 0,  // The page number  of the target is '''not''' rendered at the place of the reference.
    kShowPage   = 1,  // The page number  of the target is rendered at the place of the reference.
};

template < typename StringType >
bool FromString(const StringType& string, ShowResourcePageEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-SHOW-PAGE") == 0) {
        value = ShowResourcePageEnum::kNoShowPage;
        return true;
    } else if (std::strcmp(string.c_str(), "SHOW-PAGE") == 0) {
        value = ShowResourcePageEnum::kShowPage;
        return true;
    } else
        return false;
}

inline const char* ToString(ShowResourcePageEnum value)
{
    switch (value) {
        case ShowResourcePageEnum::kNoShowPage:
            return "NO-SHOW-PAGE";
        case ShowResourcePageEnum::kShowPage:
            return "SHOW-PAGE";
        default:
            return nullptr;
    }
}

enum class ShowResourceShortNameEnum
{
    kNoShowShortName = 0,  // The short name of the target is '''not''' rendered at the place of the reference.
    kShowShortName   = 1,  // The short name of the target is rendered at the place of the reference.
};

template < typename StringType >
bool FromString(const StringType& string, ShowResourceShortNameEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-SHOW-SHORT-NAME") == 0) {
        value = ShowResourceShortNameEnum::kNoShowShortName;
        return true;
    } else if (std::strcmp(string.c_str(), "SHOW-SHORT-NAME") == 0) {
        value = ShowResourceShortNameEnum::kShowShortName;
        return true;
    } else
        return false;
}

inline const char* ToString(ShowResourceShortNameEnum value)
{
    switch (value) {
        case ShowResourceShortNameEnum::kNoShowShortName:
            return "NO-SHOW-SHORT-NAME";
        case ShowResourceShortNameEnum::kShowShortName:
            return "SHOW-SHORT-NAME";
        default:
            return nullptr;
    }
}

enum class ShowResourceTypeEnum
{
    kNoShowType = 0,  // The type of the target is '''not''' rendered at the place of the reference.
    kShowType   = 1,  // The type of the target is rendered at the place of the reference.
};

template < typename StringType >
bool FromString(const StringType& string, ShowResourceTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-SHOW-TYPE") == 0) {
        value = ShowResourceTypeEnum::kNoShowType;
        return true;
    } else if (std::strcmp(string.c_str(), "SHOW-TYPE") == 0) {
        value = ShowResourceTypeEnum::kShowType;
        return true;
    } else
        return false;
}

inline const char* ToString(ShowResourceTypeEnum value)
{
    switch (value) {
        case ShowResourceTypeEnum::kNoShowType:
            return "NO-SHOW-TYPE";
        case ShowResourceTypeEnum::kShowType:
            return "SHOW-TYPE";
        default:
            return nullptr;
    }
}

enum class ShowSeeEnum
{
    kNoShowSee = 0,  // The word "see" is '''not''' rendered before  the reference.
    kShowSee   = 1,  // The word "see"is rendered before the reference.
};

template < typename StringType >
bool FromString(const StringType& string, ShowSeeEnum& value)
{
    if (std::strcmp(string.c_str(), "NO-SHOW-SEE") == 0) {
        value = ShowSeeEnum::kNoShowSee;
        return true;
    } else if (std::strcmp(string.c_str(), "SHOW-SEE") == 0) {
        value = ShowSeeEnum::kShowSee;
        return true;
    } else
        return false;
}

inline const char* ToString(ShowSeeEnum value)
{
    switch (value) {
        case ShowSeeEnum::kNoShowSee:
            return "NO-SHOW-SEE";
        case ShowSeeEnum::kShowSee:
            return "SHOW-SEE";
        default:
            return nullptr;
    }
}

enum class SignalFanEnum
{
    kNfold = 0,   // The connections internally in the CompositionSwComponentType via DelegationSwConnectors and
                  // AssemblySwConnectors are defined in a way that at least one data element present in the S/R
                  // interface or one ClientServerOperation in the C/S interface of the outer PortPrototype is involved
                  // in a 1:n or n:1 communication pattern.
    kSingle = 1,  // The connections internally in the CompositionSwComponentType via DelegationSwConnectors and
                  // AssemblySwConnectors are defined in a way that each VariableDataPrototype  present in the S/R
                  // interface or ClientServerOperation in the C/S interface of the outer PortPrototype is involved in a
                  // 1:1 communication pattern only.
};

template < typename StringType >
bool FromString(const StringType& string, SignalFanEnum& value)
{
    if (std::strcmp(string.c_str(), "NFOLD") == 0) {
        value = SignalFanEnum::kNfold;
        return true;
    } else if (std::strcmp(string.c_str(), "SINGLE") == 0) {
        value = SignalFanEnum::kSingle;
        return true;
    } else
        return false;
}

inline const char* ToString(SignalFanEnum value)
{
    switch (value) {
        case SignalFanEnum::kNfold:
            return "NFOLD";
        case SignalFanEnum::kSingle:
            return "SINGLE";
        default:
            return nullptr;
    }
}

enum class SignalServiceTranslationControlEnum
{
    kTranslationStart = 0,  // Defines the start of service control at translation start.
    kPartialNetwork   = 1,  // Defines the start of service control when specific partial networks are active.
    kServiceDiscovery = 2,  // Defines the start of service control when other service is available.
};

template < typename StringType >
bool FromString(const StringType& string, SignalServiceTranslationControlEnum& value)
{
    if (std::strcmp(string.c_str(), "TRANSLATION-START") == 0) {
        value = SignalServiceTranslationControlEnum::kTranslationStart;
        return true;
    } else if (std::strcmp(string.c_str(), "PARTIAL-NETWORK") == 0) {
        value = SignalServiceTranslationControlEnum::kPartialNetwork;
        return true;
    } else if (std::strcmp(string.c_str(), "SERVICE-DISCOVERY") == 0) {
        value = SignalServiceTranslationControlEnum::kServiceDiscovery;
        return true;
    } else
        return false;
}

inline const char* ToString(SignalServiceTranslationControlEnum value)
{
    switch (value) {
        case SignalServiceTranslationControlEnum::kTranslationStart:
            return "TRANSLATION-START";
        case SignalServiceTranslationControlEnum::kPartialNetwork:
            return "PARTIAL-NETWORK";
        case SignalServiceTranslationControlEnum::kServiceDiscovery:
            return "SERVICE-DISCOVERY";
        default:
            return nullptr;
    }
}

enum class SoftwareClusterDependencyLogicalOperatorEnum
{
    kLogicalAnd = 0,  // logical and
    kLogicalOr  = 1,  // logical or
};

template < typename StringType >
bool FromString(const StringType& string, SoftwareClusterDependencyLogicalOperatorEnum& value)
{
    if (std::strcmp(string.c_str(), "LOGICAL-AND") == 0) {
        value = SoftwareClusterDependencyLogicalOperatorEnum::kLogicalAnd;
        return true;
    } else if (std::strcmp(string.c_str(), "LOGICAL-OR") == 0) {
        value = SoftwareClusterDependencyLogicalOperatorEnum::kLogicalOr;
        return true;
    } else
        return false;
}

inline const char* ToString(SoftwareClusterDependencyLogicalOperatorEnum value)
{
    switch (value) {
        case SoftwareClusterDependencyLogicalOperatorEnum::kLogicalAnd:
            return "LOGICAL-AND";
        case SoftwareClusterDependencyLogicalOperatorEnum::kLogicalOr:
            return "LOGICAL-OR";
        default:
            return nullptr;
    }
}

enum class SoftwareClusterDependencyOperatorEnum
{
    kIsGreaterThan        = 0,  // greater than
    kIsEqual              = 1,  // equal
    kIsLessThan           = 2,  // less than
    kIsGreaterThanOrEqual = 3,  // greater than or equal
    kIsLessThanOrEqual    = 4,  // less than or equal
};

template < typename StringType >
bool FromString(const StringType& string, SoftwareClusterDependencyOperatorEnum& value)
{
    if (std::strcmp(string.c_str(), "IS-GREATER-THAN") == 0) {
        value = SoftwareClusterDependencyOperatorEnum::kIsGreaterThan;
        return true;
    } else if (std::strcmp(string.c_str(), "IS-EQUAL") == 0) {
        value = SoftwareClusterDependencyOperatorEnum::kIsEqual;
        return true;
    } else if (std::strcmp(string.c_str(), "IS-LESS-THAN") == 0) {
        value = SoftwareClusterDependencyOperatorEnum::kIsLessThan;
        return true;
    } else if (std::strcmp(string.c_str(), "IS-GREATER-THAN-OR-EQUAL") == 0) {
        value = SoftwareClusterDependencyOperatorEnum::kIsGreaterThanOrEqual;
        return true;
    } else if (std::strcmp(string.c_str(), "IS-LESS-THAN-OR-EQUAL") == 0) {
        value = SoftwareClusterDependencyOperatorEnum::kIsLessThanOrEqual;
        return true;
    } else
        return false;
}

inline const char* ToString(SoftwareClusterDependencyOperatorEnum value)
{
    switch (value) {
        case SoftwareClusterDependencyOperatorEnum::kIsGreaterThan:
            return "IS-GREATER-THAN";
        case SoftwareClusterDependencyOperatorEnum::kIsEqual:
            return "IS-EQUAL";
        case SoftwareClusterDependencyOperatorEnum::kIsLessThan:
            return "IS-LESS-THAN";
        case SoftwareClusterDependencyOperatorEnum::kIsGreaterThanOrEqual:
            return "IS-GREATER-THAN-OR-EQUAL";
        case SoftwareClusterDependencyOperatorEnum::kIsLessThanOrEqual:
            return "IS-LESS-THAN-OR-EQUAL";
        default:
            return nullptr;
    }
}

enum class SoftwareClusterDiagnosticAddressSemanticsEnum
{
    kPhysicalAddress   = 0,  // This address represents a physical address.
    kFunctionalAddress = 1,  // This address represents a functional address.
};

template < typename StringType >
bool FromString(const StringType& string, SoftwareClusterDiagnosticAddressSemanticsEnum& value)
{
    if (std::strcmp(string.c_str(), "PHYSICAL-ADDRESS") == 0) {
        value = SoftwareClusterDiagnosticAddressSemanticsEnum::kPhysicalAddress;
        return true;
    } else if (std::strcmp(string.c_str(), "FUNCTIONAL-ADDRESS") == 0) {
        value = SoftwareClusterDiagnosticAddressSemanticsEnum::kFunctionalAddress;
        return true;
    } else
        return false;
}

inline const char* ToString(SoftwareClusterDiagnosticAddressSemanticsEnum value)
{
    switch (value) {
        case SoftwareClusterDiagnosticAddressSemanticsEnum::kPhysicalAddress:
            return "PHYSICAL-ADDRESS";
        case SoftwareClusterDiagnosticAddressSemanticsEnum::kFunctionalAddress:
            return "FUNCTIONAL-ADDRESS";
        default:
            return nullptr;
    }
}

enum class SoftwarePackageActionTypeEnum
{
    kUpdate  = 0,
    kInstall = 1,
    kRemove  = 2,
};

template < typename StringType >
bool FromString(const StringType& string, SoftwarePackageActionTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "UPDATE") == 0) {
        value = SoftwarePackageActionTypeEnum::kUpdate;
        return true;
    } else if (std::strcmp(string.c_str(), "INSTALL") == 0) {
        value = SoftwarePackageActionTypeEnum::kInstall;
        return true;
    } else if (std::strcmp(string.c_str(), "REMOVE") == 0) {
        value = SoftwarePackageActionTypeEnum::kRemove;
        return true;
    } else
        return false;
}

inline const char* ToString(SoftwarePackageActionTypeEnum value)
{
    switch (value) {
        case SoftwarePackageActionTypeEnum::kUpdate:
            return "UPDATE";
        case SoftwarePackageActionTypeEnum::kInstall:
            return "INSTALL";
        case SoftwarePackageActionTypeEnum::kRemove:
            return "REMOVE";
        default:
            return nullptr;
    }
}

enum class SoftwarePackageActivationActionEnum
{
    kReboot             = 0,  // Reboot the whole Machine.
    kRestartApplication = 1,  // Restart the application software on the target Machine.
    kWaitForReboot = 2,  // The installation has no immediate consequences in terms of other software on the target.
};

template < typename StringType >
bool FromString(const StringType& string, SoftwarePackageActivationActionEnum& value)
{
    if (std::strcmp(string.c_str(), "REBOOT") == 0) {
        value = SoftwarePackageActivationActionEnum::kReboot;
        return true;
    } else if (std::strcmp(string.c_str(), "RESTART-APPLICATION") == 0) {
        value = SoftwarePackageActivationActionEnum::kRestartApplication;
        return true;
    } else if (std::strcmp(string.c_str(), "WAIT-FOR-REBOOT") == 0) {
        value = SoftwarePackageActivationActionEnum::kWaitForReboot;
        return true;
    } else
        return false;
}

inline const char* ToString(SoftwarePackageActivationActionEnum value)
{
    switch (value) {
        case SoftwarePackageActivationActionEnum::kReboot:
            return "REBOOT";
        case SoftwarePackageActivationActionEnum::kRestartApplication:
            return "RESTART-APPLICATION";
        case SoftwarePackageActivationActionEnum::kWaitForReboot:
            return "WAIT-FOR-REBOOT";
        default:
            return nullptr;
    }
}

enum class SoftwarePackageStoringEnum
{
    kNone      = 0,  // No storing in vehicle.
    kUcmMaster = 1,  // Storing in Ucm Master.
    kUcm       = 2,  // Storing in UCM (subordinate).
};

template < typename StringType >
bool FromString(const StringType& string, SoftwarePackageStoringEnum& value)
{
    if (std::strcmp(string.c_str(), "NONE") == 0) {
        value = SoftwarePackageStoringEnum::kNone;
        return true;
    } else if (std::strcmp(string.c_str(), "UCM-MASTER") == 0) {
        value = SoftwarePackageStoringEnum::kUcmMaster;
        return true;
    } else if (std::strcmp(string.c_str(), "UCM") == 0) {
        value = SoftwarePackageStoringEnum::kUcm;
        return true;
    } else
        return false;
}

inline const char* ToString(SoftwarePackageStoringEnum value)
{
    switch (value) {
        case SoftwarePackageStoringEnum::kNone:
            return "NONE";
        case SoftwarePackageStoringEnum::kUcmMaster:
            return "UCM-MASTER";
        case SoftwarePackageStoringEnum::kUcm:
            return "UCM";
        default:
            return nullptr;
    }
}

enum class SomeipMessageTypeEnum
{
    kError           = 0,  // The response containing an error.
    kNotification    = 1,  // A request of a notification expecting no response.
    kRequest         = 2,  // A request expecting a response.
    kRequestNoReturn = 3,  // A fire&forget request.
    kResponse        = 4,  // The response message.
};

template < typename StringType >
bool FromString(const StringType& string, SomeipMessageTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "ERROR") == 0) {
        value = SomeipMessageTypeEnum::kError;
        return true;
    } else if (std::strcmp(string.c_str(), "NOTIFICATION") == 0) {
        value = SomeipMessageTypeEnum::kNotification;
        return true;
    } else if (std::strcmp(string.c_str(), "REQUEST") == 0) {
        value = SomeipMessageTypeEnum::kRequest;
        return true;
    } else if (std::strcmp(string.c_str(), "REQUEST-NO-RETURN") == 0) {
        value = SomeipMessageTypeEnum::kRequestNoReturn;
        return true;
    } else if (std::strcmp(string.c_str(), "RESPONSE") == 0) {
        value = SomeipMessageTypeEnum::kResponse;
        return true;
    } else
        return false;
}

inline const char* ToString(SomeipMessageTypeEnum value)
{
    switch (value) {
        case SomeipMessageTypeEnum::kError:
            return "ERROR";
        case SomeipMessageTypeEnum::kNotification:
            return "NOTIFICATION";
        case SomeipMessageTypeEnum::kRequest:
            return "REQUEST";
        case SomeipMessageTypeEnum::kRequestNoReturn:
            return "REQUEST-NO-RETURN";
        case SomeipMessageTypeEnum::kResponse:
            return "RESPONSE";
        default:
            return nullptr;
    }
}

enum class SomeipTransformerSessionHandlingEnum
{
    kSessionHandlingActive   = 0,  // The SOME/IP Transformer shall use session handling
    kSessionHandlingInactive = 1,  // The SOME/IP Transformer doesn't use session handling
};

template < typename StringType >
bool FromString(const StringType& string, SomeipTransformerSessionHandlingEnum& value)
{
    if (std::strcmp(string.c_str(), "SESSION-HANDLING-ACTIVE") == 0) {
        value = SomeipTransformerSessionHandlingEnum::kSessionHandlingActive;
        return true;
    } else if (std::strcmp(string.c_str(), "SESSION-HANDLING-INACTIVE") == 0) {
        value = SomeipTransformerSessionHandlingEnum::kSessionHandlingInactive;
        return true;
    } else
        return false;
}

inline const char* ToString(SomeipTransformerSessionHandlingEnum value)
{
    switch (value) {
        case SomeipTransformerSessionHandlingEnum::kSessionHandlingActive:
            return "SESSION-HANDLING-ACTIVE";
        case SomeipTransformerSessionHandlingEnum::kSessionHandlingInactive:
            return "SESSION-HANDLING-INACTIVE";
        default:
            return nullptr;
    }
}

enum class StandardNameEnum
{
    kAp = 0,  // This values represents the Adaptive Platform.
    kCp = 1,  // This Value represents the Classic Platform.
    kFo = 2,  // This values represents the Foundation.
    kTa = 3,  // This Values represents the Testing of the Adaptive Platform.
    kTc = 4,  // This values represents the Testing of the Classic Platform.
};

template < typename StringType >
bool FromString(const StringType& string, StandardNameEnum& value)
{
    if (std::strcmp(string.c_str(), "AP") == 0) {
        value = StandardNameEnum::kAp;
        return true;
    } else if (std::strcmp(string.c_str(), "CP") == 0) {
        value = StandardNameEnum::kCp;
        return true;
    } else if (std::strcmp(string.c_str(), "FO") == 0) {
        value = StandardNameEnum::kFo;
        return true;
    } else if (std::strcmp(string.c_str(), "TA") == 0) {
        value = StandardNameEnum::kTa;
        return true;
    } else if (std::strcmp(string.c_str(), "TC") == 0) {
        value = StandardNameEnum::kTc;
        return true;
    } else
        return false;
}

inline const char* ToString(StandardNameEnum value)
{
    switch (value) {
        case StandardNameEnum::kAp:
            return "AP";
        case StandardNameEnum::kCp:
            return "CP";
        case StandardNameEnum::kFo:
            return "FO";
        case StandardNameEnum::kTa:
            return "TA";
        case StandardNameEnum::kTc:
            return "TC";
        default:
            return nullptr;
    }
}

enum class StorageConditionStatusEnum
{
    kEventStorageDisabled = 0,  // Storage of a diagnostic event is disabled.
    kEventStorageEnabled  = 1,  // Storage of a diagnostic event is enabled.
};

template < typename StringType >
bool FromString(const StringType& string, StorageConditionStatusEnum& value)
{
    if (std::strcmp(string.c_str(), "EVENT-STORAGE-DISABLED") == 0) {
        value = StorageConditionStatusEnum::kEventStorageDisabled;
        return true;
    } else if (std::strcmp(string.c_str(), "EVENT-STORAGE-ENABLED") == 0) {
        value = StorageConditionStatusEnum::kEventStorageEnabled;
        return true;
    } else
        return false;
}

inline const char* ToString(StorageConditionStatusEnum value)
{
    switch (value) {
        case StorageConditionStatusEnum::kEventStorageDisabled:
            return "EVENT-STORAGE-DISABLED";
        case StorageConditionStatusEnum::kEventStorageEnabled:
            return "EVENT-STORAGE-ENABLED";
        default:
            return nullptr;
    }
}

enum class SupportBufferLockingEnum
{
    kDoesNotSupportBufferLocking = 0,  // Buffer locking is not supported.
    kSupportsBufferLocking       = 1,  // Buffer locking is supported.
};

template < typename StringType >
bool FromString(const StringType& string, SupportBufferLockingEnum& value)
{
    if (std::strcmp(string.c_str(), "DOES-NOT-SUPPORT-BUFFER-LOCKING") == 0) {
        value = SupportBufferLockingEnum::kDoesNotSupportBufferLocking;
        return true;
    } else if (std::strcmp(string.c_str(), "SUPPORTS-BUFFER-LOCKING") == 0) {
        value = SupportBufferLockingEnum::kSupportsBufferLocking;
        return true;
    } else
        return false;
}

inline const char* ToString(SupportBufferLockingEnum value)
{
    switch (value) {
        case SupportBufferLockingEnum::kDoesNotSupportBufferLocking:
            return "DOES-NOT-SUPPORT-BUFFER-LOCKING";
        case SupportBufferLockingEnum::kSupportsBufferLocking:
            return "SUPPORTS-BUFFER-LOCKING";
        default:
            return nullptr;
    }
}

enum class SwCalibrationAccessEnum
{
    kNotAccessible = 0,  // The element will not be accessible via MCD tools, i.e. will not appear in the ASAP file.
    kReadOnly      = 1,  // The element will only appear as read-only in an ASAP file.
    kReadWrite     = 2,  // The element will appear in the ASAP file with both read and write access.
};

template < typename StringType >
bool FromString(const StringType& string, SwCalibrationAccessEnum& value)
{
    if (std::strcmp(string.c_str(), "NOT-ACCESSIBLE") == 0) {
        value = SwCalibrationAccessEnum::kNotAccessible;
        return true;
    } else if (std::strcmp(string.c_str(), "READ-ONLY") == 0) {
        value = SwCalibrationAccessEnum::kReadOnly;
        return true;
    } else if (std::strcmp(string.c_str(), "READ-WRITE") == 0) {
        value = SwCalibrationAccessEnum::kReadWrite;
        return true;
    } else
        return false;
}

inline const char* ToString(SwCalibrationAccessEnum value)
{
    switch (value) {
        case SwCalibrationAccessEnum::kNotAccessible:
            return "NOT-ACCESSIBLE";
        case SwCalibrationAccessEnum::kReadOnly:
            return "READ-ONLY";
        case SwCalibrationAccessEnum::kReadWrite:
            return "READ-WRITE";
        default:
            return nullptr;
    }
}

enum class SwImplPolicyEnum
{
    kConst = 0,  // forced implementation such that the  running software within the ECU  shall not modify it. For
                 // example implemented with the "const" modifier in C.  This can be applied for parameters (not for
                 // those in NVRAM) as well as argument  data prototypes.
    kFixed = 1,  // This data element is fixed. In particular this indicates, that it  might also be implemented  e.g.
                 // as in place data, (#DEFINE).
    kMeasurementPoint
    = 2,  // The data element is created for measurement purposes only.  The data element is never read directly
          // within the ECU software. In contrast to  a "standard" data element in an unconnected provide port is,
          // this unconnection  is guaranteed for  measurementPoint data elements.
    kQueued
    = 3,  // The content of the data element is queued and the data element has 'event' semantics, i.e. data elements
          // are stored in a queue and all data  elements are processed in 'first in first out' order.  The queuing is
          // intended to be implemented by RTE Generator. This value is not applicable for parameters.
    kStandard = 4,  // This is applicable for all kinds of data elements. For variable  data prototypes the 'last is
                    // best' semantics applies. For parameter there is no  specific implementation directive.
};

template < typename StringType >
bool FromString(const StringType& string, SwImplPolicyEnum& value)
{
    if (std::strcmp(string.c_str(), "CONST") == 0) {
        value = SwImplPolicyEnum::kConst;
        return true;
    } else if (std::strcmp(string.c_str(), "FIXED") == 0) {
        value = SwImplPolicyEnum::kFixed;
        return true;
    } else if (std::strcmp(string.c_str(), "MEASUREMENT-POINT") == 0) {
        value = SwImplPolicyEnum::kMeasurementPoint;
        return true;
    } else if (std::strcmp(string.c_str(), "QUEUED") == 0) {
        value = SwImplPolicyEnum::kQueued;
        return true;
    } else if (std::strcmp(string.c_str(), "STANDARD") == 0) {
        value = SwImplPolicyEnum::kStandard;
        return true;
    } else
        return false;
}

inline const char* ToString(SwImplPolicyEnum value)
{
    switch (value) {
        case SwImplPolicyEnum::kConst:
            return "CONST";
        case SwImplPolicyEnum::kFixed:
            return "FIXED";
        case SwImplPolicyEnum::kMeasurementPoint:
            return "MEASUREMENT-POINT";
        case SwImplPolicyEnum::kQueued:
            return "QUEUED";
        case SwImplPolicyEnum::kStandard:
            return "STANDARD";
        default:
            return nullptr;
    }
}

enum class SwServiceImplPolicyEnum
{
    kInline = 0,  // inline service definition.
    kInlineConditional
    = 1,  // The service (in AUTOSAR: BswModuleEntry) is implemented in a way that it either resolves to an inline
          // function or to a standard function depending on conditions set at a later point in time.  This could be
          // handled by using the AUTOSAR compiler abstraction macros (INLINE, LOCAL_INLINE) and/or by further
          // compiler switches depending on ECU configuration values.
    kMacro    = 2,  // macro service definition.
    kStandard = 3,  // Standard service and default value, if nothing is defined.
};

template < typename StringType >
bool FromString(const StringType& string, SwServiceImplPolicyEnum& value)
{
    if (std::strcmp(string.c_str(), "INLINE") == 0) {
        value = SwServiceImplPolicyEnum::kInline;
        return true;
    } else if (std::strcmp(string.c_str(), "INLINE-CONDITIONAL") == 0) {
        value = SwServiceImplPolicyEnum::kInlineConditional;
        return true;
    } else if (std::strcmp(string.c_str(), "MACRO") == 0) {
        value = SwServiceImplPolicyEnum::kMacro;
        return true;
    } else if (std::strcmp(string.c_str(), "STANDARD") == 0) {
        value = SwServiceImplPolicyEnum::kStandard;
        return true;
    } else
        return false;
}

inline const char* ToString(SwServiceImplPolicyEnum value)
{
    switch (value) {
        case SwServiceImplPolicyEnum::kInline:
            return "INLINE";
        case SwServiceImplPolicyEnum::kInlineConditional:
            return "INLINE-CONDITIONAL";
        case SwServiceImplPolicyEnum::kMacro:
            return "MACRO";
        case SwServiceImplPolicyEnum::kStandard:
            return "STANDARD";
        default:
            return nullptr;
    }
}

enum class SwServiceReentranceEnum
{
    kReentrance = 0,  // If this element is not defined the service cannot be invoked when it is executing.
};

template < typename StringType >
bool FromString(const StringType& string, SwServiceReentranceEnum& value)
{
    if (std::strcmp(string.c_str(), "REENTRANCE") == 0) {
        value = SwServiceReentranceEnum::kReentrance;
        return true;
    } else
        return false;
}

inline const char* ToString(SwServiceReentranceEnum value)
{
    switch (value) {
        case SwServiceReentranceEnum::kReentrance:
            return "REENTRANCE";
        default:
            return nullptr;
    }
}

enum class SwVariableAccessImplPolicyEnum
{
    kDirect = 0,  // Messages with DIRECT access are read but ignored by data consistency tool. Data consistency is not
                  // guaranteed.
    kOptimized = 1,   // A Tool handles data consistency.   In SwService, where a message is referenced, only OPTIMIZED
                      // access (also default value inside SwServcie) is possible.
    kSelectable = 2,  // The user can decide inside each single service, where these message is referenced, if access to
                      // that shall be OPTIMIZED or DIRECT.
};

template < typename StringType >
bool FromString(const StringType& string, SwVariableAccessImplPolicyEnum& value)
{
    if (std::strcmp(string.c_str(), "DIRECT") == 0) {
        value = SwVariableAccessImplPolicyEnum::kDirect;
        return true;
    } else if (std::strcmp(string.c_str(), "OPTIMIZED") == 0) {
        value = SwVariableAccessImplPolicyEnum::kOptimized;
        return true;
    } else if (std::strcmp(string.c_str(), "SELECTABLE") == 0) {
        value = SwVariableAccessImplPolicyEnum::kSelectable;
        return true;
    } else
        return false;
}

inline const char* ToString(SwVariableAccessImplPolicyEnum value)
{
    switch (value) {
        case SwVariableAccessImplPolicyEnum::kDirect:
            return "DIRECT";
        case SwVariableAccessImplPolicyEnum::kOptimized:
            return "OPTIMIZED";
        case SwVariableAccessImplPolicyEnum::kSelectable:
            return "SELECTABLE";
        default:
            return nullptr;
    }
}

enum class SwcToSwcOperationArgumentsDirectionEnum
{
    kIn  = 0,  // IN (all IN and INOUT arguments)
    kOut = 1,  // OUT (all OUT and INOUT arguments) .
};

template < typename StringType >
bool FromString(const StringType& string, SwcToSwcOperationArgumentsDirectionEnum& value)
{
    if (std::strcmp(string.c_str(), "IN") == 0) {
        value = SwcToSwcOperationArgumentsDirectionEnum::kIn;
        return true;
    } else if (std::strcmp(string.c_str(), "OUT") == 0) {
        value = SwcToSwcOperationArgumentsDirectionEnum::kOut;
        return true;
    } else
        return false;
}

inline const char* ToString(SwcToSwcOperationArgumentsDirectionEnum value)
{
    switch (value) {
        case SwcToSwcOperationArgumentsDirectionEnum::kIn:
            return "IN";
        case SwcToSwcOperationArgumentsDirectionEnum::kOut:
            return "OUT";
        default:
            return nullptr;
    }
}

enum class SynchronizationTypeEnum
{
    kResponseSynchronization
    = 0,  // In case that the Synchronization Timing Constraint is specified for event chains, the response events of
          // the associated event chains shall occur synchronously with respect to the specified tolerance. All
          // associated event chains shall have the same stimulus event.  In case that the Synchronization Timing
          // Constraint is specified for events, the associated events shall occur synchronously with respect to the
          // specified tolerance. All associated events represent the response events of a common stimulus event, even
          // such a stimulus event is not known yet or not available in the scope of the model.
    kStimulusSynchronization
    = 1,  // In case that the Synchronization Timing Constraint is specified for event chains, the stimulus events of
          // the associated event chains shall occur synchronously with respect to the specified tolerance. All
          // associated event chains shall have the same response event.  In case that the Synchronization Timing
          // Constraint is specified for events, the associated events shall occur synchronously with respect to the
          // specified tolerance. All associated events represent the stimulus events of a common response event, even
          // such a response event is not known yet or not available in the scope of the model.
};

template < typename StringType >
bool FromString(const StringType& string, SynchronizationTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "RESPONSE-SYNCHRONIZATION") == 0) {
        value = SynchronizationTypeEnum::kResponseSynchronization;
        return true;
    } else if (std::strcmp(string.c_str(), "STIMULUS-SYNCHRONIZATION") == 0) {
        value = SynchronizationTypeEnum::kStimulusSynchronization;
        return true;
    } else
        return false;
}

inline const char* ToString(SynchronizationTypeEnum value)
{
    switch (value) {
        case SynchronizationTypeEnum::kResponseSynchronization:
            return "RESPONSE-SYNCHRONIZATION";
        case SynchronizationTypeEnum::kStimulusSynchronization:
            return "STIMULUS-SYNCHRONIZATION";
        default:
            return nullptr;
    }
}

enum class TcpRoleEnum
{
    kConnect = 0,  // Connects the client to a remote TCP host.
    kListen  = 1,  // Socket is put into the server mode (listen for connections).
};

template < typename StringType >
bool FromString(const StringType& string, TcpRoleEnum& value)
{
    if (std::strcmp(string.c_str(), "CONNECT") == 0) {
        value = TcpRoleEnum::kConnect;
        return true;
    } else if (std::strcmp(string.c_str(), "LISTEN") == 0) {
        value = TcpRoleEnum::kListen;
        return true;
    } else
        return false;
}

inline const char* ToString(TcpRoleEnum value)
{
    switch (value) {
        case TcpRoleEnum::kConnect:
            return "CONNECT";
        case TcpRoleEnum::kListen:
            return "LISTEN";
        default:
            return nullptr;
    }
}

enum class TdEventBswInternalBehaviorTypeEnum
{
    kBswModuleEntityActivated = 0,  // A point in time where the associated BswModuleEntity has been activated, which
                                    // means that it has entered the state "to be started".
    kBswModuleEntityStarted = 1,    // A point in time where the associated BswModuleEntity has entered the state
                                    // "started" after its activation.
    kBswModuleEntityTerminated
    = 2,  // A point in time where the associated BswModuleEntity has terminated and entered the state "suspended"
};

template < typename StringType >
bool FromString(const StringType& string, TdEventBswInternalBehaviorTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "BSW-MODULE-ENTITY-ACTIVATED") == 0) {
        value = TdEventBswInternalBehaviorTypeEnum::kBswModuleEntityActivated;
        return true;
    } else if (std::strcmp(string.c_str(), "BSW-MODULE-ENTITY-STARTED") == 0) {
        value = TdEventBswInternalBehaviorTypeEnum::kBswModuleEntityStarted;
        return true;
    } else if (std::strcmp(string.c_str(), "BSW-MODULE-ENTITY-TERMINATED") == 0) {
        value = TdEventBswInternalBehaviorTypeEnum::kBswModuleEntityTerminated;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventBswInternalBehaviorTypeEnum value)
{
    switch (value) {
        case TdEventBswInternalBehaviorTypeEnum::kBswModuleEntityActivated:
            return "BSW-MODULE-ENTITY-ACTIVATED";
        case TdEventBswInternalBehaviorTypeEnum::kBswModuleEntityStarted:
            return "BSW-MODULE-ENTITY-STARTED";
        case TdEventBswInternalBehaviorTypeEnum::kBswModuleEntityTerminated:
            return "BSW-MODULE-ENTITY-TERMINATED";
        default:
            return nullptr;
    }
}

enum class TdEventBswModeDeclarationTypeEnum
{
    kModeDeclarationRequested
    = 0,  // A point in time where the associated ModeDeclarationGroupPrototype has been requested.
    kModeDeclarationSwitchCompleted
    = 1,  // A point in time where the switch to the associated ModeDeclarationGroupPrototype has been completed.
    kModeDeclarationSwitchInitiated = 2,  // A point in time where the switch to the associated
                                          // ModeDeclarationGroupPrototype has been initiated by the BswM.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventBswModeDeclarationTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "MODE-DECLARATION-REQUESTED") == 0) {
        value = TdEventBswModeDeclarationTypeEnum::kModeDeclarationRequested;
        return true;
    } else if (std::strcmp(string.c_str(), "MODE-DECLARATION-SWITCH-COMPLETED") == 0) {
        value = TdEventBswModeDeclarationTypeEnum::kModeDeclarationSwitchCompleted;
        return true;
    } else if (std::strcmp(string.c_str(), "MODE-DECLARATION-SWITCH-INITIATED") == 0) {
        value = TdEventBswModeDeclarationTypeEnum::kModeDeclarationSwitchInitiated;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventBswModeDeclarationTypeEnum value)
{
    switch (value) {
        case TdEventBswModeDeclarationTypeEnum::kModeDeclarationRequested:
            return "MODE-DECLARATION-REQUESTED";
        case TdEventBswModeDeclarationTypeEnum::kModeDeclarationSwitchCompleted:
            return "MODE-DECLARATION-SWITCH-COMPLETED";
        case TdEventBswModeDeclarationTypeEnum::kModeDeclarationSwitchInitiated:
            return "MODE-DECLARATION-SWITCH-INITIATED";
        default:
            return nullptr;
    }
}

enum class TdEventBswModuleTypeEnum
{
    kBswMEntryCalled       = 0,  // A point in time where the associated BswModuleEntry has been called.
    kBswMEntryCallReturned = 1,  // A point in time where the call of the associated BswModuleEntry has returned.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventBswModuleTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "BSW-M-ENTRY-CALLED") == 0) {
        value = TdEventBswModuleTypeEnum::kBswMEntryCalled;
        return true;
    } else if (std::strcmp(string.c_str(), "BSW-M-ENTRY-CALL-RETURNED") == 0) {
        value = TdEventBswModuleTypeEnum::kBswMEntryCallReturned;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventBswModuleTypeEnum value)
{
    switch (value) {
        case TdEventBswModuleTypeEnum::kBswMEntryCalled:
            return "BSW-M-ENTRY-CALLED";
        case TdEventBswModuleTypeEnum::kBswMEntryCallReturned:
            return "BSW-M-ENTRY-CALL-RETURNED";
        default:
            return nullptr;
    }
}

enum class TdEventFrameEthernetTypeEnum
{
    kFrameEthernetQueuedForTransmission
    = 0,  // A point in time where the Ethernet frame containing the specified PDUs is queued for transmission within
          // the corresponding Ethernet Communication Driver.
    kFrameEthernetReceivedByIf = 1,  // A point in time where the frame is pushed from the corresponding Ethernet
                                     // communication controller to the BSW Ethernet communication interface.
    kFrameEthernetReceivedOnBus
    = 2,  // A point in time where the receipt of the Ethernet frame/packet completes successfully on the recipient's
          // Ethernet communication controller. In other words, the Ethernet frame/packet has entered the recipient's
          // Ethernet communication controller which means the last bit of the Ethernet frame/packet has been
          // received.
    kFrameEthernetSentOnBus = 3,  // A point in time where the transmission of the Ethernet frame/packet completes
                                  // successfully on the physical Ethernet communication network. In other words, the
                                  // Ethernet frame/packet has left the sender's Ethernet communication controller,
                                  // which means that the last bit of the Ethernet frame/packet has been sent.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventFrameEthernetTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "FRAME-ETHERNET-QUEUED-FOR-TRANSMISSION") == 0) {
        value = TdEventFrameEthernetTypeEnum::kFrameEthernetQueuedForTransmission;
        return true;
    } else if (std::strcmp(string.c_str(), "FRAME-ETHERNET-RECEIVED-BY-IF") == 0) {
        value = TdEventFrameEthernetTypeEnum::kFrameEthernetReceivedByIf;
        return true;
    } else if (std::strcmp(string.c_str(), "FRAME-ETHERNET-RECEIVED-ON-BUS") == 0) {
        value = TdEventFrameEthernetTypeEnum::kFrameEthernetReceivedOnBus;
        return true;
    } else if (std::strcmp(string.c_str(), "FRAME-ETHERNET-SENT-ON-BUS") == 0) {
        value = TdEventFrameEthernetTypeEnum::kFrameEthernetSentOnBus;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventFrameEthernetTypeEnum value)
{
    switch (value) {
        case TdEventFrameEthernetTypeEnum::kFrameEthernetQueuedForTransmission:
            return "FRAME-ETHERNET-QUEUED-FOR-TRANSMISSION";
        case TdEventFrameEthernetTypeEnum::kFrameEthernetReceivedByIf:
            return "FRAME-ETHERNET-RECEIVED-BY-IF";
        case TdEventFrameEthernetTypeEnum::kFrameEthernetReceivedOnBus:
            return "FRAME-ETHERNET-RECEIVED-ON-BUS";
        case TdEventFrameEthernetTypeEnum::kFrameEthernetSentOnBus:
            return "FRAME-ETHERNET-SENT-ON-BUS";
        default:
            return nullptr;
    }
}

enum class TdEventFrameTypeEnum
{
    kFrameQueuedForTransmission = 0,  // A point in time where the frame containing the named signal / I-PDU is queued
                                      // for transmission within the related Communication Driver.
    kFrameReceivedByIf = 1,  // A point in time where the frame is pushed from the subscriber's communication controller
                             // to the corresponding (FlexRay / CAN / LIN) Interface BSW module.
    kFrameTransmittedOnBus = 2,  // A point in time where the transmission of the frame completes successfully, and the
                                 // subscriber's communication controller receives the frame from the bus.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventFrameTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "FRAME-QUEUED-FOR-TRANSMISSION") == 0) {
        value = TdEventFrameTypeEnum::kFrameQueuedForTransmission;
        return true;
    } else if (std::strcmp(string.c_str(), "FRAME-RECEIVED-BY-IF") == 0) {
        value = TdEventFrameTypeEnum::kFrameReceivedByIf;
        return true;
    } else if (std::strcmp(string.c_str(), "FRAME-TRANSMITTED-ON-BUS") == 0) {
        value = TdEventFrameTypeEnum::kFrameTransmittedOnBus;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventFrameTypeEnum value)
{
    switch (value) {
        case TdEventFrameTypeEnum::kFrameQueuedForTransmission:
            return "FRAME-QUEUED-FOR-TRANSMISSION";
        case TdEventFrameTypeEnum::kFrameReceivedByIf:
            return "FRAME-RECEIVED-BY-IF";
        case TdEventFrameTypeEnum::kFrameTransmittedOnBus:
            return "FRAME-TRANSMITTED-ON-BUS";
        default:
            return nullptr;
    }
}

enum class TdEventIPduTypeEnum
{
    kIPduReceivedByCom
    = 0,  // A point in time where the received frame is processed by the corresponding (FlexRay / CAN / LIN)
          // Interface BSW module, routed through the PDUR and the contained PDUs are pushed to the COM module.
    kIPduSentToIf = 1,  // A point in time where the carrier COM I-PDU is routed through the PDUR and is pushed to the
                        // bus specific (FlexRay / CAN / LIN) Interface BSW module.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventIPduTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "I-PDU-RECEIVED-BY-COM") == 0) {
        value = TdEventIPduTypeEnum::kIPduReceivedByCom;
        return true;
    } else if (std::strcmp(string.c_str(), "I-PDU-SENT-TO-IF") == 0) {
        value = TdEventIPduTypeEnum::kIPduSentToIf;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventIPduTypeEnum value)
{
    switch (value) {
        case TdEventIPduTypeEnum::kIPduReceivedByCom:
            return "I-PDU-RECEIVED-BY-COM";
        case TdEventIPduTypeEnum::kIPduSentToIf:
            return "I-PDU-SENT-TO-IF";
        default:
            return nullptr;
    }
}

enum class TdEventISignalTypeEnum
{
    kISignalAvailableForRte
    = 0,  // A point in time, where the COM module makes the contained signal / signal group available for the RTE and
          // the corresponding Rx Indication callout is generated (if configured).
    kISignalSentToCom = 1,  // A point in time, where a transmission request call is issued by the RTE on a named COM
                            // signal / signal group and the new value is stored to the carrier COM I-PDU buffer.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventISignalTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "I-SIGNAL-AVAILABLE-FOR-RTE") == 0) {
        value = TdEventISignalTypeEnum::kISignalAvailableForRte;
        return true;
    } else if (std::strcmp(string.c_str(), "I-SIGNAL-SENT-TO-COM") == 0) {
        value = TdEventISignalTypeEnum::kISignalSentToCom;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventISignalTypeEnum value)
{
    switch (value) {
        case TdEventISignalTypeEnum::kISignalAvailableForRte:
            return "I-SIGNAL-AVAILABLE-FOR-RTE";
        case TdEventISignalTypeEnum::kISignalSentToCom:
            return "I-SIGNAL-SENT-TO-COM";
        default:
            return nullptr;
    }
}

enum class TdEventModeDeclarationTypeEnum
{
    kModeDeclarationSwitchCompleted
    = 0,  // A point in time where the switch to the associated ModeDeclarationGroupPrototype has been completed.
    kModeDeclarationSwitchInitiated
    = 1,  // A point in time where the switch to the associated ModeDeclarationGroupPrototype has been initiated.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventModeDeclarationTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "MODE-DECLARATION-SWITCH-COMPLETED") == 0) {
        value = TdEventModeDeclarationTypeEnum::kModeDeclarationSwitchCompleted;
        return true;
    } else if (std::strcmp(string.c_str(), "MODE-DECLARATION-SWITCH-INITIATED") == 0) {
        value = TdEventModeDeclarationTypeEnum::kModeDeclarationSwitchInitiated;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventModeDeclarationTypeEnum value)
{
    switch (value) {
        case TdEventModeDeclarationTypeEnum::kModeDeclarationSwitchCompleted:
            return "MODE-DECLARATION-SWITCH-COMPLETED";
        case TdEventModeDeclarationTypeEnum::kModeDeclarationSwitchInitiated:
            return "MODE-DECLARATION-SWITCH-INITIATED";
        default:
            return nullptr;
    }
}

enum class TdEventOperationTypeEnum
{
    kOperationCalled = 0,  // A point in time where the referenced operation is called by the client SWC.
    kOperationCallReceived
    = 1,  // A point in time where the call of the referenced operation is received by the server SWC.
    kOperationCallResponseReceived
    = 2,  // A point in time where the client SWC has received the response of the referenced operation call.
    kOperationCallResponseSent = 3,  // A point in time where the server SWC has terminated with the execution of the
                                     // referenced operation, and has sent out a response.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventOperationTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "OPERATION-CALLED") == 0) {
        value = TdEventOperationTypeEnum::kOperationCalled;
        return true;
    } else if (std::strcmp(string.c_str(), "OPERATION-CALL-RECEIVED") == 0) {
        value = TdEventOperationTypeEnum::kOperationCallReceived;
        return true;
    } else if (std::strcmp(string.c_str(), "OPERATION-CALL-RESPONSE-RECEIVED") == 0) {
        value = TdEventOperationTypeEnum::kOperationCallResponseReceived;
        return true;
    } else if (std::strcmp(string.c_str(), "OPERATION-CALL-RESPONSE-SENT") == 0) {
        value = TdEventOperationTypeEnum::kOperationCallResponseSent;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventOperationTypeEnum value)
{
    switch (value) {
        case TdEventOperationTypeEnum::kOperationCalled:
            return "OPERATION-CALLED";
        case TdEventOperationTypeEnum::kOperationCallReceived:
            return "OPERATION-CALL-RECEIVED";
        case TdEventOperationTypeEnum::kOperationCallResponseReceived:
            return "OPERATION-CALL-RESPONSE-RECEIVED";
        case TdEventOperationTypeEnum::kOperationCallResponseSent:
            return "OPERATION-CALL-RESPONSE-SENT";
        default:
            return nullptr;
    }
}

enum class TdEventServiceInstanceDiscoveryTypeEnum
{
    kAdaptiveServiceFindStarted = 0,  // A point in time where a service subscriber starts to find a needed service.
    kAdaptiveServiceFindCompleted
    = 1,                               // A point in time where a service subscriber completes to find a needed service.
    kAdaptiveServiceOfferStarted = 2,  // A point in time where a service provider starts to offer a needed service.
    kAdaptiveServiceOfferCompleted
    = 3,  // A point in time where a service provider completes to offer a needed service.
    kAdaptiveServiceSubscriptionStarted
    = 4,  // A point in time where a service subscriber starts to subscribe to a needed service.
    kAdaptiveServiceSubscriptionCompleted
    = 5,  // A point in time where a service subscriber completes to subscribe to a needed service.
    kAdaptiveServiceSubscriptionAcknowledgeStarted
    = 6,  // A point in time where a service provider starts to acknowledge subscription to a needed service.
    kAdaptiveServiceSubscriptionAcknowledgeCompleted
    = 7,  // A point in time where a service provider completes to acknowledge subscription to a needed service.
    kAdaptiveServiceStopSubscriptionStarted
    = 8,  // A point in time where a service subscriber starts to stop subscribing to a needed service.
    kAdaptiveServiceStopSubscriptionCompleted
    = 9,  // A point in time where a service subscriber completes to stop subscribing to a needed service.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventServiceInstanceDiscoveryTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "ADAPTIVE-SERVICE-FIND-STARTED") == 0) {
        value = TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceFindStarted;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-SERVICE-FIND-COMPLETED") == 0) {
        value = TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceFindCompleted;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-SERVICE-OFFER-STARTED") == 0) {
        value = TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceOfferStarted;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-SERVICE-OFFER-COMPLETED") == 0) {
        value = TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceOfferCompleted;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-SERVICE-SUBSCRIPTION-STARTED") == 0) {
        value = TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceSubscriptionStarted;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-SERVICE-SUBSCRIPTION-COMPLETED") == 0) {
        value = TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceSubscriptionCompleted;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-SERVICE-SUBSCRIPTION-ACKNOWLEDGE-STARTED") == 0) {
        value = TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceSubscriptionAcknowledgeStarted;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-SERVICE-SUBSCRIPTION-ACKNOWLEDGE-COMPLETED") == 0) {
        value = TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceSubscriptionAcknowledgeCompleted;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-SERVICE-STOP-SUBSCRIPTION-STARTED") == 0) {
        value = TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceStopSubscriptionStarted;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-SERVICE-STOP-SUBSCRIPTION-COMPLETED") == 0) {
        value = TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceStopSubscriptionCompleted;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventServiceInstanceDiscoveryTypeEnum value)
{
    switch (value) {
        case TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceFindStarted:
            return "ADAPTIVE-SERVICE-FIND-STARTED";
        case TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceFindCompleted:
            return "ADAPTIVE-SERVICE-FIND-COMPLETED";
        case TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceOfferStarted:
            return "ADAPTIVE-SERVICE-OFFER-STARTED";
        case TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceOfferCompleted:
            return "ADAPTIVE-SERVICE-OFFER-COMPLETED";
        case TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceSubscriptionStarted:
            return "ADAPTIVE-SERVICE-SUBSCRIPTION-STARTED";
        case TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceSubscriptionCompleted:
            return "ADAPTIVE-SERVICE-SUBSCRIPTION-COMPLETED";
        case TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceSubscriptionAcknowledgeStarted:
            return "ADAPTIVE-SERVICE-SUBSCRIPTION-ACKNOWLEDGE-STARTED";
        case TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceSubscriptionAcknowledgeCompleted:
            return "ADAPTIVE-SERVICE-SUBSCRIPTION-ACKNOWLEDGE-COMPLETED";
        case TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceStopSubscriptionStarted:
            return "ADAPTIVE-SERVICE-STOP-SUBSCRIPTION-STARTED";
        case TdEventServiceInstanceDiscoveryTypeEnum::kAdaptiveServiceStopSubscriptionCompleted:
            return "ADAPTIVE-SERVICE-STOP-SUBSCRIPTION-COMPLETED";
        default:
            return nullptr;
    }
}

enum class TdEventServiceInstanceEventTypeEnum
{
    kAdaptiveEventSent = 0,  // A point in time where an event provided by a service is sent through the service port of
                             // the service provider.
    kAdaptiveEventReceived = 1,  // A point in time where an event required by a service subscriber is received through
                                 // the service port of the service subscriber.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventServiceInstanceEventTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "ADAPTIVE-EVENT-SENT") == 0) {
        value = TdEventServiceInstanceEventTypeEnum::kAdaptiveEventSent;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-EVENT-RECEIVED") == 0) {
        value = TdEventServiceInstanceEventTypeEnum::kAdaptiveEventReceived;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventServiceInstanceEventTypeEnum value)
{
    switch (value) {
        case TdEventServiceInstanceEventTypeEnum::kAdaptiveEventSent:
            return "ADAPTIVE-EVENT-SENT";
        case TdEventServiceInstanceEventTypeEnum::kAdaptiveEventReceived:
            return "ADAPTIVE-EVENT-RECEIVED";
        default:
            return nullptr;
    }
}

enum class TdEventServiceInstanceFieldTypeEnum
{
    kAdaptiveFieldNotificationSent = 0,  // A point in time where a field notification provided by a service is sent
                                         // through the service port of the service provider.
    kAdaptiveFieldNotificationReceived
    = 1,  // A point in time where a field notification required by a service subscriber is received through the
          // service port of the service subscriber.
    kAdaptiveFieldGetterCalled = 2,  // A point in time where a field getter of a service is called by a service
                                     // subscriber through the service port of the service subscriber.
    kAdaptiveFieldGetterCompleted
    = 3,  // A point in time where a field getter of a service is completed and the result of the field getter is
          // received through the service subscriber's service port.
    kAdaptiveFieldSetterCalled = 4,  // A point in time where a field setter of a service is called by a service
                                     // subscriber through the service port of the service subscriber.
    kAdaptiveFieldSetterCompleted
    = 5,  // A point in time where a field setter of a service is completed and the result of the field setter is
          // received through the service subscriber's service port.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventServiceInstanceFieldTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "ADAPTIVE-FIELD-NOTIFICATION-SENT") == 0) {
        value = TdEventServiceInstanceFieldTypeEnum::kAdaptiveFieldNotificationSent;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-FIELD-NOTIFICATION-RECEIVED") == 0) {
        value = TdEventServiceInstanceFieldTypeEnum::kAdaptiveFieldNotificationReceived;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-FIELD-GETTER-CALLED") == 0) {
        value = TdEventServiceInstanceFieldTypeEnum::kAdaptiveFieldGetterCalled;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-FIELD-GETTER-COMPLETED") == 0) {
        value = TdEventServiceInstanceFieldTypeEnum::kAdaptiveFieldGetterCompleted;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-FIELD-SETTER-CALLED") == 0) {
        value = TdEventServiceInstanceFieldTypeEnum::kAdaptiveFieldSetterCalled;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-FIELD-SETTER-COMPLETED") == 0) {
        value = TdEventServiceInstanceFieldTypeEnum::kAdaptiveFieldSetterCompleted;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventServiceInstanceFieldTypeEnum value)
{
    switch (value) {
        case TdEventServiceInstanceFieldTypeEnum::kAdaptiveFieldNotificationSent:
            return "ADAPTIVE-FIELD-NOTIFICATION-SENT";
        case TdEventServiceInstanceFieldTypeEnum::kAdaptiveFieldNotificationReceived:
            return "ADAPTIVE-FIELD-NOTIFICATION-RECEIVED";
        case TdEventServiceInstanceFieldTypeEnum::kAdaptiveFieldGetterCalled:
            return "ADAPTIVE-FIELD-GETTER-CALLED";
        case TdEventServiceInstanceFieldTypeEnum::kAdaptiveFieldGetterCompleted:
            return "ADAPTIVE-FIELD-GETTER-COMPLETED";
        case TdEventServiceInstanceFieldTypeEnum::kAdaptiveFieldSetterCalled:
            return "ADAPTIVE-FIELD-SETTER-CALLED";
        case TdEventServiceInstanceFieldTypeEnum::kAdaptiveFieldSetterCompleted:
            return "ADAPTIVE-FIELD-SETTER-COMPLETED";
        default:
            return nullptr;
    }
}

enum class TdEventServiceInstanceMethodTypeEnum
{
    kAdaptiveMethodCalled
    = 0,  // A point in time where a method of a service is called through the service subscriber's service port.
    kAdaptiveMethodCallReceived
    = 1,  // A point in time where a method call of a service is received through the service provider's service port.
    kAdaptiveMethodResponseSent = 2,  // A point in time where a response of a method call of a service is sent through
                                      // the service provider's service port.
    kAdaptiveMethodResponseReceived = 3,  // A point in time where a response of a method call of a service is received
                                          // through the service subscribers's service port.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventServiceInstanceMethodTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "ADAPTIVE-METHOD-CALLED") == 0) {
        value = TdEventServiceInstanceMethodTypeEnum::kAdaptiveMethodCalled;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-METHOD-CALL-RECEIVED") == 0) {
        value = TdEventServiceInstanceMethodTypeEnum::kAdaptiveMethodCallReceived;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-METHOD-RESPONSE-SENT") == 0) {
        value = TdEventServiceInstanceMethodTypeEnum::kAdaptiveMethodResponseSent;
        return true;
    } else if (std::strcmp(string.c_str(), "ADAPTIVE-METHOD-RESPONSE-RECEIVED") == 0) {
        value = TdEventServiceInstanceMethodTypeEnum::kAdaptiveMethodResponseReceived;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventServiceInstanceMethodTypeEnum value)
{
    switch (value) {
        case TdEventServiceInstanceMethodTypeEnum::kAdaptiveMethodCalled:
            return "ADAPTIVE-METHOD-CALLED";
        case TdEventServiceInstanceMethodTypeEnum::kAdaptiveMethodCallReceived:
            return "ADAPTIVE-METHOD-CALL-RECEIVED";
        case TdEventServiceInstanceMethodTypeEnum::kAdaptiveMethodResponseSent:
            return "ADAPTIVE-METHOD-RESPONSE-SENT";
        case TdEventServiceInstanceMethodTypeEnum::kAdaptiveMethodResponseReceived:
            return "ADAPTIVE-METHOD-RESPONSE-RECEIVED";
        default:
            return nullptr;
    }
}

enum class TdEventSwcInternalBehaviorTypeEnum
{
    kRunnableEntityActivated = 0,  // A point in time where the associated RunnableEntity has been activated, which
                                   // means that it has entered the state "to be started".
    kRunnableEntityStarted
    = 1,  // A point in time where the associated RunnableEntity has entered the state "started" after its activation.
    kRunnableEntityTerminated
    = 2,  // A point in time where the associated RunnableEntity has terminated and entered the state "suspended".
    kRunnableEntityVariableAccess = 3,  // A point in time where the associated variable is accessed.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventSwcInternalBehaviorTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "RUNNABLE-ENTITY-ACTIVATED") == 0) {
        value = TdEventSwcInternalBehaviorTypeEnum::kRunnableEntityActivated;
        return true;
    } else if (std::strcmp(string.c_str(), "RUNNABLE-ENTITY-STARTED") == 0) {
        value = TdEventSwcInternalBehaviorTypeEnum::kRunnableEntityStarted;
        return true;
    } else if (std::strcmp(string.c_str(), "RUNNABLE-ENTITY-TERMINATED") == 0) {
        value = TdEventSwcInternalBehaviorTypeEnum::kRunnableEntityTerminated;
        return true;
    } else if (std::strcmp(string.c_str(), "RUNNABLE-ENTITY-VARIABLE-ACCESS") == 0) {
        value = TdEventSwcInternalBehaviorTypeEnum::kRunnableEntityVariableAccess;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventSwcInternalBehaviorTypeEnum value)
{
    switch (value) {
        case TdEventSwcInternalBehaviorTypeEnum::kRunnableEntityActivated:
            return "RUNNABLE-ENTITY-ACTIVATED";
        case TdEventSwcInternalBehaviorTypeEnum::kRunnableEntityStarted:
            return "RUNNABLE-ENTITY-STARTED";
        case TdEventSwcInternalBehaviorTypeEnum::kRunnableEntityTerminated:
            return "RUNNABLE-ENTITY-TERMINATED";
        case TdEventSwcInternalBehaviorTypeEnum::kRunnableEntityVariableAccess:
            return "RUNNABLE-ENTITY-VARIABLE-ACCESS";
        default:
            return nullptr;
    }
}

enum class TdEventTriggerTypeEnum
{
    kTriggerActivated = 0,  // A point in time where the referenced trigger has been successfully released and is
                            // activating runnable entities of the receiving SW-C.
    kTriggerReleased
    = 1,  // A point in time where the referenced trigger has been successfully released by the emitting SW-C.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventTriggerTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "TRIGGER-ACTIVATED") == 0) {
        value = TdEventTriggerTypeEnum::kTriggerActivated;
        return true;
    } else if (std::strcmp(string.c_str(), "TRIGGER-RELEASED") == 0) {
        value = TdEventTriggerTypeEnum::kTriggerReleased;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventTriggerTypeEnum value)
{
    switch (value) {
        case TdEventTriggerTypeEnum::kTriggerActivated:
            return "TRIGGER-ACTIVATED";
        case TdEventTriggerTypeEnum::kTriggerReleased:
            return "TRIGGER-RELEASED";
        default:
            return nullptr;
    }
}

enum class TdEventVariableDataPrototypeTypeEnum
{
    kVariableDataPrototypeReceived
    = 0,  // A point in time where the referenced variable data prototype has been successfully transmitted and is
          // available in the related communication buffer (of the RTE) for the receiving SWC.
    kVariableDataPrototypeSent
    = 1,  // A point in time where the referenced variable data prototype has been successfully sent out by the
          // sending SWC, so that it is available in the related communication buffer (of the RTE) for transmission.
};

template < typename StringType >
bool FromString(const StringType& string, TdEventVariableDataPrototypeTypeEnum& value)
{
    if (std::strcmp(string.c_str(), "VARIABLE-DATA-PROTOTYPE-RECEIVED") == 0) {
        value = TdEventVariableDataPrototypeTypeEnum::kVariableDataPrototypeReceived;
        return true;
    } else if (std::strcmp(string.c_str(), "VARIABLE-DATA-PROTOTYPE-SENT") == 0) {
        value = TdEventVariableDataPrototypeTypeEnum::kVariableDataPrototypeSent;
        return true;
    } else
        return false;
}

inline const char* ToString(TdEventVariableDataPrototypeTypeEnum value)
{
    switch (value) {
        case TdEventVariableDataPrototypeTypeEnum::kVariableDataPrototypeReceived:
            return "VARIABLE-DATA-PROTOTYPE-RECEIVED";
        case TdEventVariableDataPrototypeTypeEnum::kVariableDataPrototypeSent:
            return "VARIABLE-DATA-PROTOTYPE-SENT";
        default:
            return nullptr;
    }
}

enum class TerminationBehaviorEnum
{
    kProcessIsNotSelfTerminating = 0,  // The Process terminates only on request from Execution Management.
    kProcessIsSelfTerminating    = 1,  // The Process is allowed to terminate without request from Execution Management.
};

template < typename StringType >
bool FromString(const StringType& string, TerminationBehaviorEnum& value)
{
    if (std::strcmp(string.c_str(), "PROCESS-IS-NOT-SELF-TERMINATING") == 0) {
        value = TerminationBehaviorEnum::kProcessIsNotSelfTerminating;
        return true;
    } else if (std::strcmp(string.c_str(), "PROCESS-IS-SELF-TERMINATING") == 0) {
        value = TerminationBehaviorEnum::kProcessIsSelfTerminating;
        return true;
    } else
        return false;
}

inline const char* ToString(TerminationBehaviorEnum value)
{
    switch (value) {
        case TerminationBehaviorEnum::kProcessIsNotSelfTerminating:
            return "PROCESS-IS-NOT-SELF-TERMINATING";
        case TerminationBehaviorEnum::kProcessIsSelfTerminating:
            return "PROCESS-IS-SELF-TERMINATING";
        default:
            return nullptr;
    }
}

enum class TimeSyncTechnologyEnum
{
    kAvbIeee8021As   = 0,  // Ethernet AVB compliant IEEE802.1AS Precision Time Protocol
    kNtpRfc958       = 1,  // Network Time Protocol (NTP)
    kPtpIeee15882002 = 2,  // Precision Time Protocol (PTP) IEEE 1588-2002
    kPtpIeee15882008 = 3,  // Precision Time Protocol (PTP) IEEE 1588-2008
};

template < typename StringType >
bool FromString(const StringType& string, TimeSyncTechnologyEnum& value)
{
    if (std::strcmp(string.c_str(), "AVB--IEEE-802--1-AS") == 0) {
        value = TimeSyncTechnologyEnum::kAvbIeee8021As;
        return true;
    } else if (std::strcmp(string.c_str(), "NTP--RFC-958") == 0) {
        value = TimeSyncTechnologyEnum::kNtpRfc958;
        return true;
    } else if (std::strcmp(string.c_str(), "PTP--IEEE-1588--2002") == 0) {
        value = TimeSyncTechnologyEnum::kPtpIeee15882002;
        return true;
    } else if (std::strcmp(string.c_str(), "PTP--IEEE-1588--2008") == 0) {
        value = TimeSyncTechnologyEnum::kPtpIeee15882008;
        return true;
    } else
        return false;
}

inline const char* ToString(TimeSyncTechnologyEnum value)
{
    switch (value) {
        case TimeSyncTechnologyEnum::kAvbIeee8021As:
            return "AVB--IEEE-802--1-AS";
        case TimeSyncTechnologyEnum::kNtpRfc958:
            return "NTP--RFC-958";
        case TimeSyncTechnologyEnum::kPtpIeee15882002:
            return "PTP--IEEE-1588--2002";
        case TimeSyncTechnologyEnum::kPtpIeee15882008:
            return "PTP--IEEE-1588--2008";
        default:
            return nullptr;
    }
}

enum class TimeSynchronizationKindEnum
{
    kSynchronized = 0,  // Defines that the requested time base shall be a synchronized time based.
    kOffset       = 1,  // Defines that the requested time base shall be an offset time based.
};

template < typename StringType >
bool FromString(const StringType& string, TimeSynchronizationKindEnum& value)
{
    if (std::strcmp(string.c_str(), "SYNCHRONIZED") == 0) {
        value = TimeSynchronizationKindEnum::kSynchronized;
        return true;
    } else if (std::strcmp(string.c_str(), "OFFSET") == 0) {
        value = TimeSynchronizationKindEnum::kOffset;
        return true;
    } else
        return false;
}

inline const char* ToString(TimeSynchronizationKindEnum value)
{
    switch (value) {
        case TimeSynchronizationKindEnum::kSynchronized:
            return "SYNCHRONIZED";
        case TimeSynchronizationKindEnum::kOffset:
            return "OFFSET";
        default:
            return nullptr;
    }
}

enum class TlsVersionEnum
{
    kTls12 = 0,  // TLS version 1.2
    kLts13 = 1,  // TLS version 1.3
    kTls13 = 2,  // TLS version 1.3
};

template < typename StringType >
bool FromString(const StringType& string, TlsVersionEnum& value)
{
    if (std::strcmp(string.c_str(), "TLS-12") == 0) {
        value = TlsVersionEnum::kTls12;
        return true;
    } else if (std::strcmp(string.c_str(), "LTS-13") == 0) {
        value = TlsVersionEnum::kLts13;
        return true;
    } else if (std::strcmp(string.c_str(), "TLS-13") == 0) {
        value = TlsVersionEnum::kTls13;
        return true;
    } else
        return false;
}

inline const char* ToString(TlsVersionEnum value)
{
    switch (value) {
        case TlsVersionEnum::kTls12:
            return "TLS-12";
        case TlsVersionEnum::kLts13:
            return "LTS-13";
        case TlsVersionEnum::kTls13:
            return "TLS-13";
        default:
            return nullptr;
    }
}

enum class TransferPropertyEnum
{
    kPending = 0,  // If the signal has the TransferProperty pending, then the function Com_SendSignal shall not perform
                   // a transmission of the IPdu associated with the signal.
    kTriggered = 1,  // The signal in the assigned IPdu is updated and a request for the IPdu's transmission is made.
    kTriggeredOnChange = 2,  // The signal in the assigned IPdu is updated and a request for the IPdus transmission is
                             // made only if the signal value is different from the already stored signal value.
    kTriggeredOnChangeWithoutRepetition
    = 3,  // The signal in the assigned IPdu is updated and a request for the IPdus transmission is made only if the
          // signal value is different from the already stored signal value.  In the DIRECT/N-TIMES or MIXED
          // transmission mode (EventControlledTiming) the IPdu will be transmitted just once without a repetition,
          // independent of the defined NumberOfRepeats.
    kTriggeredWithoutRepetition
    = 4,  // The signal in the assigned IPdu is updated and a request for the IPdu's transmission is made. In the
          // DIRECT/N-TIMES or MIXED transmission mode (EventControlledTiming) the IPdu will be transmitted just once
          // without a repetition, independent of the defined NumberOfRepeats.
};

template < typename StringType >
bool FromString(const StringType& string, TransferPropertyEnum& value)
{
    if (std::strcmp(string.c_str(), "PENDING") == 0) {
        value = TransferPropertyEnum::kPending;
        return true;
    } else if (std::strcmp(string.c_str(), "TRIGGERED") == 0) {
        value = TransferPropertyEnum::kTriggered;
        return true;
    } else if (std::strcmp(string.c_str(), "TRIGGERED-ON-CHANGE") == 0) {
        value = TransferPropertyEnum::kTriggeredOnChange;
        return true;
    } else if (std::strcmp(string.c_str(), "TRIGGERED-ON-CHANGE-WITHOUT-REPETITION") == 0) {
        value = TransferPropertyEnum::kTriggeredOnChangeWithoutRepetition;
        return true;
    } else if (std::strcmp(string.c_str(), "TRIGGERED-WITHOUT-REPETITION") == 0) {
        value = TransferPropertyEnum::kTriggeredWithoutRepetition;
        return true;
    } else
        return false;
}

inline const char* ToString(TransferPropertyEnum value)
{
    switch (value) {
        case TransferPropertyEnum::kPending:
            return "PENDING";
        case TransferPropertyEnum::kTriggered:
            return "TRIGGERED";
        case TransferPropertyEnum::kTriggeredOnChange:
            return "TRIGGERED-ON-CHANGE";
        case TransferPropertyEnum::kTriggeredOnChangeWithoutRepetition:
            return "TRIGGERED-ON-CHANGE-WITHOUT-REPETITION";
        case TransferPropertyEnum::kTriggeredWithoutRepetition:
            return "TRIGGERED-WITHOUT-REPETITION";
        default:
            return nullptr;
    }
}

enum class TransformerClassEnum
{
    kCustom     = 0,  // The transformer is a custom transformer.
    kSafety     = 1,  // The transformer is a safety transformer.
    kSecurity   = 2,  // The transformer is a security transformer.
    kSerializer = 3,  // The transformer is a serializing transformer.
};

template < typename StringType >
bool FromString(const StringType& string, TransformerClassEnum& value)
{
    if (std::strcmp(string.c_str(), "CUSTOM") == 0) {
        value = TransformerClassEnum::kCustom;
        return true;
    } else if (std::strcmp(string.c_str(), "SAFETY") == 0) {
        value = TransformerClassEnum::kSafety;
        return true;
    } else if (std::strcmp(string.c_str(), "SECURITY") == 0) {
        value = TransformerClassEnum::kSecurity;
        return true;
    } else if (std::strcmp(string.c_str(), "SERIALIZER") == 0) {
        value = TransformerClassEnum::kSerializer;
        return true;
    } else
        return false;
}

inline const char* ToString(TransformerClassEnum value)
{
    switch (value) {
        case TransformerClassEnum::kCustom:
            return "CUSTOM";
        case TransformerClassEnum::kSafety:
            return "SAFETY";
        case TransformerClassEnum::kSecurity:
            return "SECURITY";
        case TransformerClassEnum::kSerializer:
            return "SERIALIZER";
        default:
            return nullptr;
    }
}

enum class TransmissionModeDefinitionEnum
{
    kCyclic = 0,  // The data is assumed to be transmitted in a cyclic manner. The cycle is defined by dataUpdatePeriod.
    kTriggered = 1,          // The data is assumed to be transmitted in an arbitrary manner (minimumSendInterval to be
                             // respected, if defined).
    kCyclicAndOnChange = 2,  // The data is assumed to be transmitted in a cyclic manner (with cycle time
                             // dataUpdatePeriod) and additionally there may be arbitrary transmission if the data value
                             // changes (minimumSendInterval to be respected, if defined).
};

template < typename StringType >
bool FromString(const StringType& string, TransmissionModeDefinitionEnum& value)
{
    if (std::strcmp(string.c_str(), "CYCLIC") == 0) {
        value = TransmissionModeDefinitionEnum::kCyclic;
        return true;
    } else if (std::strcmp(string.c_str(), "TRIGGERED") == 0) {
        value = TransmissionModeDefinitionEnum::kTriggered;
        return true;
    } else if (std::strcmp(string.c_str(), "CYCLIC-AND-ON-CHANGE") == 0) {
        value = TransmissionModeDefinitionEnum::kCyclicAndOnChange;
        return true;
    } else
        return false;
}

inline const char* ToString(TransmissionModeDefinitionEnum value)
{
    switch (value) {
        case TransmissionModeDefinitionEnum::kCyclic:
            return "CYCLIC";
        case TransmissionModeDefinitionEnum::kTriggered:
            return "TRIGGERED";
        case TransmissionModeDefinitionEnum::kCyclicAndOnChange:
            return "CYCLIC-AND-ON-CHANGE";
        default:
            return nullptr;
    }
}

enum class TransportLayerProtocolEnum
{
    kUdp = 0,  // User datagram protocol
    kTcp = 1,  // Transmission control protocol
};

template < typename StringType >
bool FromString(const StringType& string, TransportLayerProtocolEnum& value)
{
    if (std::strcmp(string.c_str(), "UDP") == 0) {
        value = TransportLayerProtocolEnum::kUdp;
        return true;
    } else if (std::strcmp(string.c_str(), "TCP") == 0) {
        value = TransportLayerProtocolEnum::kTcp;
        return true;
    } else
        return false;
}

inline const char* ToString(TransportLayerProtocolEnum value)
{
    switch (value) {
        case TransportLayerProtocolEnum::kUdp:
            return "UDP";
        case TransportLayerProtocolEnum::kTcp:
            return "TCP";
        default:
            return nullptr;
    }
}

enum class TrustedPlatformExecutableLaunchBehaviorEnum
{
    kStrictMode               = 0,  // An Executable shall not launch if the corresponding authentication fails.
    kMonitorMode              = 1,  // An Executable shall always launch, even if the corresponding authentication fails
    kNoTrustedPlatformSupport = 2,  // This value shall be used if there is no TrustedPlatform support on the Machine
};

template < typename StringType >
bool FromString(const StringType& string, TrustedPlatformExecutableLaunchBehaviorEnum& value)
{
    if (std::strcmp(string.c_str(), "STRICT-MODE") == 0) {
        value = TrustedPlatformExecutableLaunchBehaviorEnum::kStrictMode;
        return true;
    } else if (std::strcmp(string.c_str(), "MONITOR-MODE") == 0) {
        value = TrustedPlatformExecutableLaunchBehaviorEnum::kMonitorMode;
        return true;
    } else if (std::strcmp(string.c_str(), "NO-TRUSTED-PLATFORM-SUPPORT") == 0) {
        value = TrustedPlatformExecutableLaunchBehaviorEnum::kNoTrustedPlatformSupport;
        return true;
    } else
        return false;
}

inline const char* ToString(TrustedPlatformExecutableLaunchBehaviorEnum value)
{
    switch (value) {
        case TrustedPlatformExecutableLaunchBehaviorEnum::kStrictMode:
            return "STRICT-MODE";
        case TrustedPlatformExecutableLaunchBehaviorEnum::kMonitorMode:
            return "MONITOR-MODE";
        case TrustedPlatformExecutableLaunchBehaviorEnum::kNoTrustedPlatformSupport:
            return "NO-TRUSTED-PLATFORM-SUPPORT";
        default:
            return nullptr;
    }
}

enum class UdpChecksumCalculationEnum
{
    kUdpChecksumEnabled  = 0,  // Udp checksum handling shall be enabled
    kUdpChecksumDisabled = 1,  // Udp checksum handling shall be disabled
};

template < typename StringType >
bool FromString(const StringType& string, UdpChecksumCalculationEnum& value)
{
    if (std::strcmp(string.c_str(), "UDP-CHECKSUM-ENABLED") == 0) {
        value = UdpChecksumCalculationEnum::kUdpChecksumEnabled;
        return true;
    } else if (std::strcmp(string.c_str(), "UDP-CHECKSUM-DISABLED") == 0) {
        value = UdpChecksumCalculationEnum::kUdpChecksumDisabled;
        return true;
    } else
        return false;
}

inline const char* ToString(UdpChecksumCalculationEnum value)
{
    switch (value) {
        case UdpChecksumCalculationEnum::kUdpChecksumEnabled:
            return "UDP-CHECKSUM-ENABLED";
        case UdpChecksumCalculationEnum::kUdpChecksumDisabled:
            return "UDP-CHECKSUM-DISABLED";
        default:
            return nullptr;
    }
}

enum class UdpCollectionTriggerEnum
{
    kAlways = 0,  // ServiceInterface element will trigger the transmission of the data.
    kNever  = 1,  // ServiceInterface element will be buffered and will not trigger the transmission of the data.
};

template < typename StringType >
bool FromString(const StringType& string, UdpCollectionTriggerEnum& value)
{
    if (std::strcmp(string.c_str(), "ALWAYS") == 0) {
        value = UdpCollectionTriggerEnum::kAlways;
        return true;
    } else if (std::strcmp(string.c_str(), "NEVER") == 0) {
        value = UdpCollectionTriggerEnum::kNever;
        return true;
    } else
        return false;
}

inline const char* ToString(UdpCollectionTriggerEnum value)
{
    switch (value) {
        case UdpCollectionTriggerEnum::kAlways:
            return "ALWAYS";
        case UdpCollectionTriggerEnum::kNever:
            return "NEVER";
        default:
            return nullptr;
    }
}

enum class V2XSupportEnum
{
    kV2XActiveSupported = 0,  // This means that the EcuInstance supports V2X communication as both the sender and the
                              // receiver of communication.
    kV2XNotSupported = 1,     // This EcuInstance does not support V2X communication
};

template < typename StringType >
bool FromString(const StringType& string, V2XSupportEnum& value)
{
    if (std::strcmp(string.c_str(), "V-2-X-ACTIVE-SUPPORTED") == 0) {
        value = V2XSupportEnum::kV2XActiveSupported;
        return true;
    } else if (std::strcmp(string.c_str(), "V-2-X-NOT-SUPPORTED") == 0) {
        value = V2XSupportEnum::kV2XNotSupported;
        return true;
    } else
        return false;
}

inline const char* ToString(V2XSupportEnum value)
{
    switch (value) {
        case V2XSupportEnum::kV2XActiveSupported:
            return "V-2-X-ACTIVE-SUPPORTED";
        case V2XSupportEnum::kV2XNotSupported:
            return "V-2-X-NOT-SUPPORTED";
        default:
            return nullptr;
    }
}

enum class ValignEnum
{
    kBottom = 0,  // The contents of the table cell is bottom aligned.
    kMiddle = 1,  // The contents of the table is vertically centered.
    kTop    = 2,  // The contents of the table cell is top aligned.
};

template < typename StringType >
bool FromString(const StringType& string, ValignEnum& value)
{
    if (std::strcmp(string.c_str(), "BOTTOM") == 0) {
        value = ValignEnum::kBottom;
        return true;
    } else if (std::strcmp(string.c_str(), "MIDDLE") == 0) {
        value = ValignEnum::kMiddle;
        return true;
    } else if (std::strcmp(string.c_str(), "TOP") == 0) {
        value = ValignEnum::kTop;
        return true;
    } else
        return false;
}

inline const char* ToString(ValignEnum value)
{
    switch (value) {
        case ValignEnum::kBottom:
            return "BOTTOM";
        case ValignEnum::kMiddle:
            return "MIDDLE";
        case ValignEnum::kTop:
            return "TOP";
        default:
            return nullptr;
    }
}

enum class VariableAccessScopeEnum
{
    kCommunicationInterEcu
    = 0,  // This case is foreseen to express that the corresponding communication shall be considered inter-ECU, i.e.
          // it will cross the ECU boundary. This is considered the default case.
    kCommunicationIntraPartition = 1,  // This case is foreseen to express that the corresponding communication shall
                                       // '''not''' cross the boundary of a partition.
    kInterPartitionIntraEcu = 2,  // In this case the communication shall cross the boundaries of partitions within one
                                  // ECU but it shall not cross the boundaries of the ECU itself.
};

template < typename StringType >
bool FromString(const StringType& string, VariableAccessScopeEnum& value)
{
    if (std::strcmp(string.c_str(), "COMMUNICATION-INTER-ECU") == 0) {
        value = VariableAccessScopeEnum::kCommunicationInterEcu;
        return true;
    } else if (std::strcmp(string.c_str(), "COMMUNICATION-INTRA-PARTITION") == 0) {
        value = VariableAccessScopeEnum::kCommunicationIntraPartition;
        return true;
    } else if (std::strcmp(string.c_str(), "INTER-PARTITION-INTRA-ECU") == 0) {
        value = VariableAccessScopeEnum::kInterPartitionIntraEcu;
        return true;
    } else
        return false;
}

inline const char* ToString(VariableAccessScopeEnum value)
{
    switch (value) {
        case VariableAccessScopeEnum::kCommunicationInterEcu:
            return "COMMUNICATION-INTER-ECU";
        case VariableAccessScopeEnum::kCommunicationIntraPartition:
            return "COMMUNICATION-INTRA-PARTITION";
        case VariableAccessScopeEnum::kInterPartitionIntraEcu:
            return "INTER-PARTITION-INTRA-ECU";
        default:
            return nullptr;
    }
}

enum class VehicleDriverNotificationEnum
{
    kTransfer = 0,  // Software shall be transferred to the vehicle.
    kProcess  = 1,  // Processing of software package shall be executed
    kActivate = 2,  // Software package shall be activated.
    kRollBack = 3,  // Software package shall be rolled back.
    kFinish   = 4,  // Finish notification
};

template < typename StringType >
bool FromString(const StringType& string, VehicleDriverNotificationEnum& value)
{
    if (std::strcmp(string.c_str(), "TRANSFER") == 0) {
        value = VehicleDriverNotificationEnum::kTransfer;
        return true;
    } else if (std::strcmp(string.c_str(), "PROCESS") == 0) {
        value = VehicleDriverNotificationEnum::kProcess;
        return true;
    } else if (std::strcmp(string.c_str(), "ACTIVATE") == 0) {
        value = VehicleDriverNotificationEnum::kActivate;
        return true;
    } else if (std::strcmp(string.c_str(), "ROLL-BACK") == 0) {
        value = VehicleDriverNotificationEnum::kRollBack;
        return true;
    } else if (std::strcmp(string.c_str(), "FINISH") == 0) {
        value = VehicleDriverNotificationEnum::kFinish;
        return true;
    } else
        return false;
}

inline const char* ToString(VehicleDriverNotificationEnum value)
{
    switch (value) {
        case VehicleDriverNotificationEnum::kTransfer:
            return "TRANSFER";
        case VehicleDriverNotificationEnum::kProcess:
            return "PROCESS";
        case VehicleDriverNotificationEnum::kActivate:
            return "ACTIVATE";
        case VehicleDriverNotificationEnum::kRollBack:
            return "ROLL-BACK";
        case VehicleDriverNotificationEnum::kFinish:
            return "FINISH";
        default:
            return nullptr;
    }
}

enum class XmlSpaceEnum
{
    kDefault = 0,  // The value "default" signals that applications' default white-space processing modes are acceptable
                   // for this element.
    kPreserve = 1,  // the value "preserve" indicates the intent that applications preserve all the white space.
};

template < typename StringType >
bool FromString(const StringType& string, XmlSpaceEnum& value)
{
    if (std::strcmp(string.c_str(), "default") == 0) {
        value = XmlSpaceEnum::kDefault;
        return true;
    } else if (std::strcmp(string.c_str(), "preserve") == 0) {
        value = XmlSpaceEnum::kPreserve;
        return true;
    } else
        return false;
}

inline const char* ToString(XmlSpaceEnum value)
{
    switch (value) {
        case XmlSpaceEnum::kDefault:
            return "default";
        case XmlSpaceEnum::kPreserve:
            return "preserve";
        default:
            return nullptr;
    }
}

}  // namespace tps
}  // namespace manifestreader
}  // namespace isoft

#endif
