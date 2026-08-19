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
/// @file       defines.h
/// @brief
/// @details
/// @date       2023-04-17
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef GEN_DEFINES_H_
#define GEN_DEFINES_H_
#include <ara/core/array.h>
#include <ara/core/map.h>
#include <ara/core/span.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
namespace isoft {
namespace dm {
using MetaInfoMap = ara::core::Map< ara::core::String, ara::core::String >;
std::int32_t const kGeneralReject{0x10};
struct ComCtrlRequestParamsType
{
    /// @brief Control type of CommunicationControl service
    std::uint8_t controlType;
    /// @brief Communication type of CommunicationControl service
    std::uint8_t communicationType;
    /// @brief Node identification number
    std::uint16_t nodeIdentificationNumber;
    using IsEnumerableTag = void;
    template < typename F >
    void enumerate(F& func)  // NOLINT
    {
        func(controlType);
        func(communicationType);
        func(nodeIdentificationNumber);
    }
};

struct FileSizes
{
    std::uint64_t uncompressedSize;
    std::uint64_t compressedSize;
    ara::core::Vector< uint8_t > vecMD5{};
    using IsEnumerableTag = void;
    template < typename F >
    void enumerate(F& func)  // NOLINT
    {
        func(uncompressedSize);
        func(compressedSize);
        func(vecMD5);
    }
};

struct RequestFileResponse
{
    std::uint64_t fileSize;
    ara::core::Vector< uint8_t > vecMD5;
    using IsEnumerableTag = void;
    template < typename F >
    void enumerate(F& func)  // NOLINT
    {
        func(fileSize);
        func(vecMD5);
    }
};

struct CounterBased
{
    /// @brief Threshold until qualified failed
    ///
    /// @traceid{SWS_DM_00621}@tracestatus{draft}
    std::int16_t failedThreshold;
    /// @brief Threshold until qualified passed
    ///
    /// @traceid{SWS_DM_00622}@tracestatus{draft}
    std::int16_t passedThreshold;
    /// @brief Stepsize per pre-failed report
    ///
    /// @traceid{SWS_DM_00623}@tracestatus{draft}
    std::uint16_t failedStepsize;
    /// @brief Stepsize per pre-passed report
    ///
    /// @traceid{SWS_DM_00624}@tracestatus{draft}
    std::uint16_t passedStepsize;
    /// @brief failed to jump value
    ///
    /// @traceid{SWS_DM_00625}@tracestatus{draft}
    std::int16_t failedJumpValue;
    /// @brief passed to jump value
    ///
    /// @traceid{SWS_DM_00626}@tracestatus{draft}
    std::int16_t passedJumpValue;
    /// @brief is jump supported
    ///
    /// @traceid{SWS_DM_00627}@tracestatus{draft}
    bool useJumpToFailed;
    /// @brief is jump supported
    ///
    /// @traceid{SWS_DM_00628}@tracestatus{draft}
    bool useJumpToPassed;

    using IsEnumerableTag = void;
    template < typename F >
    void enumerate(F& func)  // NOLINT
    {
        func(failedThreshold);
        func(passedThreshold);
        func(failedStepsize);
        func(passedStepsize);
        func(failedJumpValue);
        func(passedJumpValue);
        func(useJumpToFailed);
        func(useJumpToPassed);
    }
};

struct TimeBased
{
    /// @brief time until failed in (ms)
    ///
    /// @traceid{SWS_DM_00629}@tracestatus{draft}
    std::uint32_t failedMs;
    /// @brief time until passed in (ms)
    ///
    /// @traceid{SWS_DM_00630}@tracestatus{draft}
    std::uint32_t passedMs;

    using IsEnumerableTag = void;
    template < typename F >
    void enumerate(F& func)  // NOLINT
    {
        func(failedMs);
        func(passedMs);
    }
};

struct SnapshotDataIdentifierType
{
    /// @name dataIdentifier
    std::uint16_t dataIdentifier;
    ara::core::Vector< std::uint8_t > data;

    using IsEnumerableTag = void;

    template < typename F >
    void enumerate(F& func)  // NOLINT
    {
        func(dataIdentifier);
        func(data);
    }
};
struct SnapshotDataRecordType
{
    /// @name snapshotRecordNumber
    std::uint8_t snapshotRecordNumber;
    ara::core::Vector< SnapshotDataIdentifierType > snapshotDataIdentifiers;

    using IsEnumerableTag = void;

    template < typename F >
    void enumerate(F& func)  // NOLINT
    {
        func(snapshotRecordNumber);
        func(snapshotDataIdentifiers);
    }
};
struct SnapshotRecordUpdatedType
{
    /// @name DTC
    /// DTC which is changed
    uint32_t dtc;
    ara::core::Vector< SnapshotDataRecordType >
        /// @name ToBeReplaced
        toBeReplaced;

    using IsEnumerableTag = void;

    template < typename F >
    void enumerate(F& func)  // NOLINT
    {
        func(dtc);
        func(toBeReplaced);
    }
};

constexpr uint32_t kInt32_6U_GidArray{6U};
struct GidStatus
{
    /// @name groupIdentification
    /// Value of gid
    ara::core::Array< std::uint8_t, kInt32_6U_GidArray > groupIdentification;

    /// @name furtherActionRequired
    /// Next method to execute
    std::uint8_t furtherActionRequired;

    /// @name syncStatus
    /// Synchronization status with other gids
    std::uint8_t syncStatus;

    using IsEnumerableTag = void;

    template < typename F >
    void enumerate(F& func)  // NOLINT
    {
        func(groupIdentification);
        func(furtherActionRequired);
        func(syncStatus);
    }
};

struct VerifyCertificateUnidirectionalResponse
{
    /// @brief Identity challenge
    ara::core::Vector< std::uint8_t > challengeServer{};
    /// @brief Temporary public key
    ara::core::Vector< std::uint8_t > ephemeralPublicKeyServer{};

    using IsEnumerableTag = void;

    template < typename F >
    void enumerate(F& func)  // NOLINT
    {
        func(challengeServer);
        func(ephemeralPublicKeyServer);
    }
};
struct VerifyCertificateBidirectionalResponse
{
    /// @brief Identity challenge
    ara::core::Vector< std::uint8_t > challengeServer{};
    /// @brief Server certificate
    ara::core::Vector< std::uint8_t > certificateServer{};
    /// @brief Server proof of ownership
    ara::core::Vector< std::uint8_t > proofOfOwnershipServer{};
    /// @brief Temporary public key
    ara::core::Vector< std::uint8_t > ephemeralPublicKeyServer{};

    using IsEnumerableTag = void;

    template < typename F >
    void enumerate(F& func)  // NOLINT
    {
        func(challengeServer);
        func(certificateServer);
        func(proofOfOwnershipServer);
        func(ephemeralPublicKeyServer);
    }
};
}  // namespace dm
}  // namespace isoft
#endif  // GEN_DEFINES_H_