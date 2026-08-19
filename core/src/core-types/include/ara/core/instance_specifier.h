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
/// @file       instance_specifier.h
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_INSTANCE_SPECIFIER_H
#define ARA_CORE_INSTANCE_SPECIFIER_H

#include <algorithm>

#include "ara/core/core_error_domain.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"

namespace ara {
namespace core {

///
/// @brief class representing an AUTOSAR Instance Specifier, which is basically an AUTOSAR shortname-path wrapper.
///
/// @uptrace{SWS_CORE_08001, e320e65a1bc909cc92e80e9cd3730dfd1caeac1d}
class InstanceSpecifier final
{
private:
    ara::core::String mMetaModelIdentifier_;

    /// @uptrace{SWS_CORE_10200, f12c2b46d02b66e0b3b680d0657bc1a02c319345}
    /// @uptrace{SWS_CORE_10201, 5af116820cf10f17c2d7844bfda56dee39ba5ba0}
    static Result< void > Validate(StringView metaModelIdentifier) noexcept  // NOLINT
    {
        size_t endOffset = static_cast< size_t >(-1);  // point this to a "virtual leading '/'"
        do {
            size_t const currentOffset = endOffset + 1;  // skip the '/'
            endOffset                  = metaModelIdentifier.find('/', currentOffset);
            size_t const len           = (endOffset == StringView::npos) ? StringView::npos : endOffset - currentOffset;
            StringView const pathComponent = metaModelIdentifier.substr(currentOffset, len);

            if (pathComponent.empty()) {
                return Result< void >::FromError(CoreErrc::kInvalidMetaModelPath);
            }
            if (!IsValidMetamodelIdentifierPart(pathComponent)) {
                return Result< void >::FromError(CoreErrc::kInvalidMetaModelShortname);
            }
        } while (endOffset != StringView::npos);

        return Result< void >();
    }

    constexpr static bool IsValidStartChar(char ch) noexcept
    {
        return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
    }

    constexpr static bool IsValidChar(char ch) noexcept
    {
        return IsValidStartChar(ch) || (ch >= '0' && ch <= '9') || (ch == '_');
    }

    constexpr static bool IsValidMetamodelIdentifierPart(StringView text) noexcept
    {
        if (text.empty()) {
            return false;
        }
        return IsValidStartChar(text[0]) && std::all_of(text.begin() + 1, text.end(), IsValidChar);
    }

    InstanceSpecifier(StringView metaModelIdentifier, bool /*tag*/) : mMetaModelIdentifier_(metaModelIdentifier) {}

public:
    /**
     * @uptrace{SWS_CORE_08032, 1070f620952101386e6bca8b1c9abddef1c96012}
     * @uptrace{SWS_CORE_10202, c6d7d3447586777bb34ea6b3771bcbdf7cc53e28}
     */
    static Result< InstanceSpecifier > Create(StringView metaModelIdentifier)
    {
        Result< void > ret = Validate(metaModelIdentifier);
        if (!ret) {
            return Result< InstanceSpecifier >::FromError(ret.Error());
        }

        return Result< InstanceSpecifier >::FromValue(InstanceSpecifier(metaModelIdentifier, true));
    }

    /**
     * @brief throwing ctor from meta-model string
     * @param metaModelIdentifier  stringified meta model identifier (short name path) where path separator is '/'.
     *        Lifetime of underlying string has to exceed the lifetime of the constructed InstanceSpecifier.
     * @throws InstanceSpecifierException in case the given metamodel_identifier isn't a valid meta-model
     * identifier/short name path.
     *
     * @uptrace{SWS_CORE_08021, 2281591a03abc3905a95d8a33ee4c8252b78b35c}
     * @uptrace{SWS_CORE_10202, c6d7d3447586777bb34ea6b3771bcbdf7cc53e28}
     */
    explicit InstanceSpecifier(StringView metaModelIdentifier)
        : InstanceSpecifier(Create(metaModelIdentifier).ValueOrThrow())
    {
    }

    /// @uptrace{SWS_CORE_08022, 90b5d83002e262d5d3111c42cab220cca8d3dc81}
    InstanceSpecifier(InstanceSpecifier const& other) = default;

    /// @uptrace{SWS_CORE_08023, 5a6679305a064c96d336ac3a924fbc58b850dfc4}
    InstanceSpecifier(InstanceSpecifier&& other) noexcept = default;

    /// @uptrace{SWS_CORE_08024, f3f7154bea0ee63eb3b6251c09b52df7c91dd16d}
    InstanceSpecifier& operator=(InstanceSpecifier const& other) = default;

    /// @uptrace{SWS_CORE_08025, 433ba680898abfbf3d290dbfcdd09051bd926f13}
    InstanceSpecifier& operator=(InstanceSpecifier&& other) = default;

    /**
     * @brief Destructor
     *
     * @uptrace{SWS_CORE_08029, 4f87695dbab65c47a9136941dec30ae3fa3f7c17}
     */
    ~InstanceSpecifier() noexcept = default;

    /**
     * @brief method to return the stringified form of InstanceSpecifier
     * @return stringified form of InstanceSpecifier. Lifetime of the underlying string is only guaranteed for the
     * lifetime of the underlying string of the StringView passed to the constructor.
     *
     * @uptrace{SWS_CORE_08041, c2555614be0fe0f38337c7039f29220b745ebf58}
     */
    StringView ToString() const noexcept { return mMetaModelIdentifier_; }

    /**
     * @brief eq operator to compare with other InstanceSpecifier instance.
     * @param other InstanceSpecifier instance to compare this one with.
     * @return true in case both InstanceSpecifiers are denoting exactly the same model element, false else.
     *
     * @uptrace{SWS_CORE_08042, a7ceeaa56ee6b931b3aa98a927575b7eaa68dbc7}
     */
    bool operator==(InstanceSpecifier const& other) const noexcept
    {
        return StringView{mMetaModelIdentifier_} == other.ToString();
    }

    /**
     * @brief eq operator to compare with other InstanceSpecifier instance.
     * @param other string representation to compare this one with.
     * @return true in case this InstanceSpecifiers is denoting exactly the same model element as other, false else.
     *
     * @uptrace{SWS_CORE_08043, de0e26513698fffe626ae8a6d16f9a55c3ea93a6}
     */
    bool operator==(StringView other) const noexcept { return StringView{mMetaModelIdentifier_} == other; }

    /**
     * @brief uneq operator to compare with other InstanceSpecifier instance.
     * @param other InstanceSpecifier instance to compare this one with.
     * @return false in case both InstanceSpecifiers are denoting exactly the same model element, true else.
     *
     * @uptrace{SWS_CORE_08044, 84c4e9d20680f8bf1c1c20f5bf8a7ef6614f662f}
     */
    bool operator!=(InstanceSpecifier const& other) const noexcept
    {
        return StringView{mMetaModelIdentifier_} != other.ToString();
    }

    /**
     * @brief uneq operator to compare with other InstanceSpecifier string representation.
     * @param other string representation to compare this one with.
     * @return false in case this InstanceSpecifiers is denoting exactly the same model element as other, true else.
     *
     * @uptrace{SWS_CORE_08045, 8119c472c8ed399ec9541e63155648d65540cdaa}
     */
    bool operator!=(StringView other) const noexcept { return StringView{mMetaModelIdentifier_} != other; }

    /**
     * @brief lower than operator to compare with other InstanceSpecifier for ordering purposes (f.i. when collecting
     * identifiers in maps).
     * @param other InstanceSpecifier instance to compare this one with.
     * @return true in case this InstanceSpecifiers is lexically lower than other, false else.
     *
     * @uptrace{SWS_CORE_08046, fe5cd331811aff5f0f38f227e28a5170b02bbd6f}
     */
    bool operator<(InstanceSpecifier const& other) const noexcept
    {
        return mMetaModelIdentifier_ < other.mMetaModelIdentifier_;
    }
};

/**
 * @brief non-member function op to allow char* on lhs.
 *
 * @param lhs  stringified form of a InstanceSpecifier
 * @param rhs  an InstanceSpecifier
 * @returns true in case rhs string representation equals lhs
 *
 * @uptrace{SWS_CORE_08081, 817fe4809108d2004494047ad99843a1a2b5b42b}
 */
inline bool operator==(StringView lhs, InstanceSpecifier const& rhs) noexcept { return lhs == rhs.ToString(); }

/**
 * @brief non-member function op to allow char* on lhs.
 *
 * @param lhs  stringified form of a InstanceSpecifier
 * @param rhs  an InstanceSpecifier
 * @returns true in case rhs string representation not equals lhs
 *
 * @uptrace{SWS_CORE_08082, 2d67bd3ec3f205a83065a86cbe4a1908de5d4e1d}
 */
inline bool operator!=(StringView lhs, InstanceSpecifier const& rhs) noexcept { return lhs != rhs.ToString(); }

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_INSTANCE_SPECIFIER_H
