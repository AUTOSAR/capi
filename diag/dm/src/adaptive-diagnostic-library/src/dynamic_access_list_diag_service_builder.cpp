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
/// @file       dynamic_access_list_diag_service_builder.cpp
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include <ara/diag/dynamic_access_list_diag_service_builder.h>

namespace ara {
namespace diag {

/**
 * @brief Constructs a value range
 *
 * @param[in] min The minimum value to match
 * @param[in] max The maximum value to match
 *
 * @artraceid{SWS_DM_01183}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
DynamicAccessListDiagServiceBuilder::ByteRange::ByteRange(Byte min, Byte max) noexcept : min_{min}, max_{max} {}

/**
 * @brief Copy constructor of ByteRange
 *
 * @artraceid{SWS_DM_01184}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
DynamicAccessListDiagServiceBuilder::ByteRange::ByteRange(ByteRange const &other) noexcept
{
    min_ = other.min_;
    max_ = other.max_;
}

/**
 * @brief Move constructor of ByteRange
 *
 * @artraceid{SWS_DM_01185}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
DynamicAccessListDiagServiceBuilder::ByteRange::ByteRange(ByteRange &&other) noexcept
{
    min_ = other.min_;
    max_ = other.max_;
}

/**
 * @brief Copy assignment operator of ByteRange
 *
 * @artraceid{SWS_DM_01186}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
auto DynamicAccessListDiagServiceBuilder::ByteRange::operator=(  // NOLINT
    ByteRange const &other) & -> ByteRange &                     // NOLINT
{
    min_ = other.min_;
    max_ = other.max_;
    return *this;
}

/**
 * @brief Move assignment operator of ByteRange
 *
 * @artraceid{SWS_DM_01187}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
auto DynamicAccessListDiagServiceBuilder::ByteRange::operator=(  // NOLINT
    ByteRange &&other) & -> ByteRange &                          // NOLINT
{
    min_ = other.min_;
    max_ = other.max_;
    return *this;
}

/**
 * @brief Destructor of ByteRange
 *
 * @returns The the lowest value
 *
 * @artraceid{SWS_DM_01188}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
DynamicAccessListDiagServiceBuilder::ByteRange::~ByteRange() noexcept { min_ = max_ = 0; }

/**
 * @brief Reports the lowest value to match
 *
 * @returns The the lowest value
 *
 * @artraceid{SWS_DM_01189}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
auto DynamicAccessListDiagServiceBuilder::ByteRange::GetMin() const noexcept -> Byte { return min_; }

/**
 * @brief Reports the highest value to match
 *
 * @returns The the highest value
 *
 * @artraceid{SWS_DM_01190}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
auto DynamicAccessListDiagServiceBuilder::ByteRange::GetMax() const noexcept -> Byte { return max_; }

DynamicAccessListDiagServiceBuilder::Value::Value(Byte byte) noexcept : byte_{byte} {}
DynamicAccessListDiagServiceBuilder::Value::Value(ByteRange byteRange) noexcept
    : type_{Type::kByteRange}, byteRange_{std::move(byteRange)}
{
}
DynamicAccessListDiagServiceBuilder::Value::Value() noexcept : type_{Type::kWildcard} {}

ara::core::Vector< std::uint8_t > DynamicAccessListDiagServiceBuilder::Value::Serialize() noexcept
{
    ara::core::Vector< std::uint8_t > result{};
    std::uint8_t uType{static_cast< std::uint8_t >(type_)};
    result.push_back(uType);
    switch (type_) {
        case Type::kByte:
            result.push_back(byte_);
            break;
        case Type::kByteRange:
            result.push_back(byteRange_.GetMin());
            result.push_back(byteRange_.GetMax());
            break;
        default:
            break;
    }
    return result;
}

/**
 * @brief Move constructor of DynamicAccessListDiagServiceBuilder
 *
 * @param[in] other Object to move-construct from
 *
 * @artraceid{SWS_DM_01170}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
DynamicAccessListDiagServiceBuilder::DynamicAccessListDiagServiceBuilder(
    DynamicAccessListDiagServiceBuilder &&other) noexcept
    : content_{other.content_}
{
    valueList_.swap(other.valueList_);
}

/**
 * @brief Destructor of DynamicAccessListDiagServiceBuilder
 *
 * @artraceid{SWS_DM_01174}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
DynamicAccessListDiagServiceBuilder::~DynamicAccessListDiagServiceBuilder() noexcept { valueList_.clear(); }

/**
 * @brief This function is used by the application to add a single byte to a
 * DynamicAccessListPattern
 *
 * @returns The instance of the same object to allow fluent API usage
 *
 * @param[in] value The byte value to add to the DynamicAccessList. The value
 * will be used to check for an exact match during evaluation of the diagnostic
 * service access rights
 *
 * @artraceid{SWS_DM_01175}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
auto DynamicAccessListDiagServiceBuilder::Add(Byte value) noexcept -> DynamicAccessListDiagServiceBuilder &
{
    Value pattern{value};
    valueList_.push_back(pattern);
    return *this;
}

/**
 * @brief This function is used by the application to add a string of bytes to
 * a DynamicAccessListPattern
 *
 * @returns The instance of the same object to allow fluent API usage
 *
 * @param[in] values The byte sequence to add to the DynamicAccessList. The
 * values will be used to check for an exact match during evaluation of the
 * diagnostic service access rights
 *
 * @artraceid{SWS_DM_01176}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
auto DynamicAccessListDiagServiceBuilder::Add(ByteString values) noexcept -> DynamicAccessListDiagServiceBuilder &
{
    for (auto &&value : values) {
        Value pattern{value};
        valueList_.push_back(pattern);
    }

    return *this;
}

/**
 * @brief This function is used by the application to add a range of bytes to
 * a DynamicAccessListPattern
 *
 * @returns The instance of the same object to allow fluent API usage
 *
 * @param[in] range The range of byte values to add to the DynamicAccessList.
 * The range will be used to check for a match during evaluation of the
 * diagnostic service access rights
 *
 * @artraceid{SWS_DM_01177}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
auto DynamicAccessListDiagServiceBuilder::Add(ByteRange range) noexcept -> DynamicAccessListDiagServiceBuilder &
{
    Value pattern{std::move(range)};
    valueList_.push_back(pattern);
    return *this;
}

/**
 * @brief This function is used by the application to define a wildcard, i.e,
 * to define a set of bytes that will be ignored in the DynamicAccessList
 * pattern being created
 *
 * @returns The instance of the same object to allow fluent API usage
 *
 * @param[in] numberOfBytesToIgnore The number of bytes to ignore
 *
 * @artraceid{SWS_DM_01178}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
auto DynamicAccessListDiagServiceBuilder::Any(std::size_t numberOfBytesToIgnore) noexcept
    -> DynamicAccessListDiagServiceBuilder &
{
    std::size_t pos{0};
    while (pos != numberOfBytesToIgnore) {
        Value pattern{};
        valueList_.push_back(pattern);
        pos++;
    }
    return *this;
}

/**
 * @brief This function is used by the application to specify a single byte
 * value at the end of the DynamicAccessList pattern
 *
 * @returns void
 *
 * @param[in] value The byte value to end the DynamicAccessList. The value will
 * be used to check for an exact match during evaluation of the diagnostic
 * service access rights
 *
 * @artraceid{SWS_DM_01179}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
void DynamicAccessListDiagServiceBuilder::EndsWith(Byte value) noexcept
{
    Value pattern{value};
    valueList_.push_back(pattern);
}

/**
 * @brief This function is used by the application to specify a closed range
 * of bytes at the end of the DynamicAccessList pattern
 *
 * @returns void
 *
 * @param[in] range The range of byte values to end the DynamicAccessList. The
 * range will be used to check for a match during evaluation of the diagnostic
 * service access rights
 *
 * @artraceid{SWS_DM_01180}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
void DynamicAccessListDiagServiceBuilder::EndsWith(ByteRange range) noexcept
{
    Value pattern{std::move(range)};
    valueList_.push_back(pattern);
}

/**
 * @brief Finalizes building the DynamicAccessList. Must be called before
 * destroying the ServiceBuilder object
 *
 * @returns void
 *
 * @artraceid{SWS_DM_01181}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
void DynamicAccessListDiagServiceBuilder::Build() noexcept
{
    for (auto &&value : valueList_) {
        ara::core::Vector< std::uint8_t > raw{value.Serialize()};
        content_.insert(content_.end(), raw.begin(), raw.end());
    }
}

}  // namespace diag
}  // namespace ara
