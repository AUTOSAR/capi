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
/// @file       profiles.h
/// @brief      E2E profiles header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_E2E_PROFILES_H
#define ARA_COM_E2E_PROFILES_H

#include <cstring>

#include "ara/com/e2e/e2e_error_domain.h"
#include "ara/com/internal/log/log.h"
#include "ara/core/vector.h"
#include "nai/runtime/nai_types.h"
#include "state_machine.h"

namespace ara {
namespace com {
namespace profile {
/// @brief Supported E2E profile types
struct PEvent
{
};
struct PMethod
{
};
template < typename T >
struct HasOffset : std::true_type
{
};

template < typename T >
struct HasDataID : std::true_type
{
};
/// @brief Profile configuration enumeration
enum class ProfileName : uint8_t
{
    kProfile_01,
    kProfile_02,
    kProfile_04,
    kProfile_05,
    kProfile_06,
    kProfile_07,
    kProfile_08,
    kProfile_11,
    kProfile_22,
    kProfile_44,
    kProfile_04m,
    kProfile_07m
};

/// @brief Method message type
enum class MessageType : uint8_t
{
    /// @brief The type of message is a request message sent from the client to the server
    kMessageTypeRequest = STD_MESSAGETYPE_REQUEST,
    /// @brief The type of message is a response message sent from the server to the client.
    kMessageTypeResponse = STD_MESSAGETYPE_RESPONSE
};

/// @brief Method reply type
enum class MessageResult : uint8_t
{
    /// @brief The result type in the response message is normal (i.e., non-error) result. This value is also used for request messages, where the value of this field is fixed to this value.
    kMessageResultOk = STD_MESSAGERESULT_OK,
    /// @brief The result type in the response message is an error (i.e., error) result.
    kMessageResultError = STD_MESSAGERESULT_ERROR
};

/// @brief Inclusion mode for implicit two-byte data ID in one-byte CRC
/// @ref [SWS_CM_90403] -- When E2E protected events trigger E2E protection, the data identifier of the corresponding attribute should be passed
enum class DataIdMode : uint8_t
{
    /// @brief Two bytes are included in the CRC (double ID configuration)
    kAll_16_Bit,
    /// @brief One of the two bytes byte is included, alternating high and low byte, depending on parity of the counter
    /// (alternating ID configuration). For even counter low byte is included; For odd counters the high byte is
    /// included.
    kAlternating_8_Bit,
    /// @brief The low byte is included in the implicit CRC calculation, the low nibble of the high byte is transmitted
    /// along with the data (i.e. it is explicitly included), the high nibble of the high byte is not used. This is
    /// applicable for the IDs up to 12 bits
    kLower_12_Bit,
    /// @brief Only low byte is included, high byte is never used. This is applicable if the IDs in a particular system
    /// are 8 bits.
    kLower_8_Bit,
};

/// @brief E2E configuration interface base class
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode
class ProfileInterface
{
public:
    /// @brief Constructor
    ProfileInterface() noexcept = default;
    /// @brief Copy constructor -- deleted
    /// @param
    ProfileInterface(ProfileInterface const&) noexcept = delete;
    /// @brief Move constructor -- deleted
    /// @param
    ProfileInterface(ProfileInterface const&&) noexcept = delete;
    /// @brief Default destructor
    virtual ~ProfileInterface() noexcept = default;
    /// @brief Assignment operator overload -- deleted
    /// @param
    /// @return
    ProfileInterface& operator=(ProfileInterface const&) noexcept = delete;
    /// @brief Move assignment operator overload -- deleted
    /// @param
    /// @return
    ProfileInterface& operator=(ProfileInterface const&&) noexcept = delete;
    /// @brief Returns the offset of the E2E header
    /// @return Offset of the E2E header
    virtual uint32_t GetHeaderOffset() const noexcept = 0;
    /// @brief Returns the length of the E2E header
    /// @return Length of the E2E header
    virtual uint32_t GetHeaderLength() const noexcept = 0;
};

/// @brief Buffer type for protect or check
using BufferType = ara::core::Vector< uint8_t >;
/// @brief SourceidType alias
using SourceidType = uint32_t;
/// @brief MessageType alias
using MessageType = profile::MessageType;
/// @brief MessageResult alias
using ResultType = profile::MessageResult;

/// @brief E2E protection interface base class
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode
class ProtectorInterface : public ProfileInterface
{
public:
    /// @brief Constructor
    ProtectorInterface() noexcept = default;
    /// @brief Copy constructor -- deleted
    /// @param
    ProtectorInterface(ProtectorInterface const&) noexcept = delete;
    /// @brief Move constructor -- deleted
    /// @param
    ProtectorInterface(ProtectorInterface const&&) noexcept = delete;
    /// @brief Default destructor
    ~ProtectorInterface() noexcept override = default;
    /// @brief Assignment operator overload -- deleted
    /// @param
    /// @return
    ProtectorInterface& operator=(ProtectorInterface const&) noexcept = delete;
    /// @brief Move assignment operator overload -- deleted
    /// @param
    /// @return
    ProtectorInterface& operator=(ProtectorInterface const&&) noexcept = delete;
    /// @brief Protection routine embeds the E2E header into the given buffer
    /// @param[inout] buffer Data to be protected
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > Protect(BufferType& buffer) noexcept = 0;
    /// @brief e2e protection wrapper for methods
    /// @param[inout] buffer Data to be protected
    /// @param[in] messageType Message type kMessageTypeRequest or kMessageTypeResponse
    /// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
    /// @param[in] sourceID Source ID
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > Protect(BufferType& buffer,
                                              MessageType const& messageType,
                                              ResultType const& messageResult,
                                              SourceidType const& sourceId)
        = 0;
    virtual ara::core::Result< uint32_t > GetCounter() noexcept = 0;
};

/// @brief Profile-independent status of the reception on one single Data in one cycle
/// @note This enumeration left for compatibility with classic platform
enum class ProfileCheckStatus : uint8_t
{
    kOk,         ///< OK: The checks of the Data in this cycle were successful
    kNoNewData,  ///< Error: The Check function has been invoked but no new Data is not available since the last call,
                 ///< according to communication medium
    kError,  ///< Error: error not related to counters occurred (e.g. wrong CRC, wrong length, wrong options, wrong Data
             ///< ID)
    kRepeated,    ///< Error: the checks of the Data in this cycle were successful, with the exception of the repetition
    kOkSomeLost,  ///< OK: the checks of the Data in this cycle were successful (including counter check, which was
                  ///< incremented within the allowed configured delta)
    kWrongSequence  ///< Error: the checks of the Data in this cycle were successful, with the exception of counter jump,
                    ///< which changed more than the allowed delta
};

/// @brief E2E check interface base class
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode
class CheckerInterface : public ProfileInterface
{
public:
    /// @brief Constructor
    CheckerInterface() noexcept = default;
    /// @brief Copy constructor -- deleted
    /// @param
    CheckerInterface(CheckerInterface const&) noexcept = delete;
    /// @brief Move constructor -- deleted
    /// @param
    CheckerInterface(CheckerInterface const&&) noexcept = delete;
    /// @brief Default destructor
    ~CheckerInterface() noexcept override = default;
    /// @brief Assignment operator overload -- deleted
    /// @param
    /// @return
    CheckerInterface& operator=(CheckerInterface const&) noexcept = delete;
    /// @brief Move assignment operator overload -- deleted
    /// @param
    /// @return
    CheckerInterface& operator=(CheckerInterface const&&) noexcept = delete;
    /// @brief Check routine performs validation of E2E header that is supplied within other data in buffer parameter
    /// @return
    /// @param[in] buffer buffer with payload and E2E header
    /// @param[in] status Result of check routine
    virtual ara::core::Result< void > Check(BufferType const& buffer, e2e::ProfileCheckStatus& status) noexcept = 0;
    /// @brief e2e check wrapper for methods (proxy side)
    /// @param[in] buffer Data to be checked
    /// @param[in] status Check result
    /// @param[in] messageType Message type kMessageTypeResponse
    /// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
    /// @param[in] sourceID Source ID
    /// @return Result object -- check result or error
    virtual ara::core::Result< void > Check(BufferType const& buffer,
                                            e2e::ProfileCheckStatus& status,
                                            MessageType const& messageType,
                                            ResultType const& messageResult,
                                            SourceidType const& sourceId)
        = 0;
    /// @brief e2e check wrapper for methods (skeleton side)
    /// @param[in] buffer Data to be checked
    /// @param[in] status Check result
    /// @param[in] messageType Message type kMessageTypeResponse
    /// @param[in] messageResult Result type in response message, for request messages this field is kMessageResultOk.
    /// @param[inout] sourceID Source ID
    /// @return Result object -- check result or error
    virtual ara::core::Result< void > Check(BufferType const& buffer,
                                            e2e::ProfileCheckStatus& status,
                                            MessageType const& messageType,
                                            ResultType const& messageResult,
                                            SourceidType* sourceId)
        = 0;
    /// @brief Parse counter from the data checked this time
    /// @return counter value
    virtual uint32_t GetCounter() const noexcept = 0;
};
/// @brief Protector template
/// @tparam T Profile type
/// @tparam M Method or event protection type
template < typename T, typename M >
class Protector
{
};

/// @brief Protector for events
/// @tparam T Profile type
template < typename T >
class Protector< T, PEvent > final : public ProtectorInterface
{
    /// @brief ConfigType type alias
    using Config = typename T::ConfigType;
    /// @brief ProtectStateType type alias
    using State = typename T::ProtectStateType;
    /// @brief Profile configuration
    Config config_;
    /// @brief Profile check status
    State state_;
    /// @brief DataID
    typename T::IdType dataId_;
    /// @brief DataID List
    typename T::IdType dataIdList_[16];  // NOLINT -- TODO[magic-numbers]
    /// @brief Number of dataIDs, 1 or 16
    uint8_t dataIdCount_;

public:
    /// @brief Constructor
    /// @param[in] config Profile configuration
    explicit Protector(Config&& config) noexcept : config_{std::move(config)}, dataId_{}, dataIdList_{}, dataIdCount_{}
    {
        DataIDInit< T >();
        auto retVal = T::kE2EProtectInit(&state_);
        ComLogDebug(GenArg(retVal));
    };
    /// @brief Constructor -- deleted
    Protector() noexcept = delete;
    /// @brief Copy constructor -- deleted
    /// @param[in] other Reference to the object to be copied
    Protector(Protector const& other) noexcept = delete;
    /// @brief Assignment operator overload -- deleted
    /// @param[in] other Reference to the object to be copied
    /// @return Protector&
    Protector& operator=(Protector const& other) noexcept = delete;
    /// @brief Move constructor -- deleted
    /// @param[in] other Reference to the object to be moved
    Protector(Protector&& other) noexcept = delete;
    /// @brief Move assignment operator overload -- deleted
    /// @param[in] other Reference to the object to be moved
    /// @return Protector&
    Protector& operator=(Protector&& other) noexcept = delete;
    /// @brief Destructor
    ~Protector() noexcept final = default;

    /// @brief Protection logic implementation override
    /// @param[inout] buffer Protected content, protects the e2e protection header
    ara::core::Result< void > Protect(BufferType& buffer) noexcept final
    {
        auto retVal{T::kE2EProtect(&config_, &state_, buffer.data(), buffer.size())};
        if (dataIdCount_ == 1) {
            ComLogDebug(T::Name(), GenArg(retVal), GenK2V("dataId", dataId_), GenK2V("Counter", state_.Counter - 1),
                        GenK2V("length", static_cast< typename T::LengthType >(buffer.size())));
        } else {
            ComLogDebug(T::Name(), GenArg(retVal), GenK2V("dataId", dataIdList_), GenK2V("Counter", state_.Counter - 1),
                        GenK2V("length", static_cast< typename T::LengthType >(buffer.size())));
        }
        if (retVal == E2E_E_OK) {
            return ara::core::Result< void >::FromValue();
        }
        return ara::core::Result< void >::FromError(ara::com::e2e::E2EErrc::kError);
    }

    /// @brief Protection logic implementation override -- Event protection does not support this method
    /// @param[inout] buffer Data to be protected
    /// @param[in] messageType Message type kMessageTypeRequest or kMessageTypeResponse
    /// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
    /// @param[in] sourceID Source ID
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Protect(BufferType& buffer,
                                      MessageType const& messageType,
                                      ResultType const& messageResult,
                                      SourceidType const& sourceId) noexcept final
    {
        std::ignore = buffer;
        std::ignore = sourceId;
        std::ignore = messageType;
        std::ignore = messageResult;
        return ara::core::Result< void >::FromError(ara::com::e2e::E2EErrc::kError);
    }
    /// @brief
    /// @return
    ara::core::Result< uint32_t > GetCounter() noexcept final
    {
        return ara::core::Result< uint32_t >::FromError(e2e::E2EErrc::kError);
    };
    /// @brief Get e2e protection header length
    /// @return e2e protection header length
    uint32_t GetHeaderLength() const noexcept final { return T::kHeaderLength; }
    /// @brief Get e2e protection header offset
    /// @return Header offset
    uint32_t GetHeaderOffset() const noexcept final { return GetHeaderOffset< T, Config >(config_); }

public:
    /// @brief Get e2e protection header offset
    /// @tparam P Profile type
    /// @tparam C Profile configuration
    /// @param[in] c Configuration parameters
    /// @return Header offset
    template < typename P, typename C >
    std::enable_if_t< HasOffset< P >::value, uint32_t > GetHeaderOffset(C const& c) const noexcept
    {
        return c.Offset / 8;  // NOLINT -- TODO[magic-numbers]
    }
    /// @brief Get e2e protection header offset
    /// @tparam P Profile type
    /// @tparam C Profile configuration
    /// @param[in] c Configuration parameters
    /// @return Header offset
    template < typename P, typename C >
    std::enable_if_t< !HasOffset< P >::value, uint32_t > GetHeaderOffset(C const& c) const noexcept
    {
        std::ignore = c;
        return 0;
    }
    /// @brief Initialize DataID
    /// @tparam P Profile type
    /// @return void
    template < typename P >
    std::enable_if_t< HasDataID< P >::value, void > DataIDInit() noexcept
    {
        dataId_      = config_.DataID;
        dataIdCount_ = 1;
    }
    /// @brief Initialize DataID
    /// @tparam P Profile type
    /// @return void
    template < typename P >
    std::enable_if_t< !HasDataID< P >::value, void > DataIDInit() noexcept
    {
        nai_memcpy(dataIdList_, config_.DataIDList, 16);  // NOLINT -- TODO[magic-numbers]
        config_.DataIDList = dataIdList_;
        dataIdCount_       = 16;  // NOLINT -- TODO[magic-numbers]
    }
};
/// @brief Protector for method protection
/// @tparam T Profile type
template < typename T >
class Protector< T, PMethod > final : public ProtectorInterface
{
    /// @brief ConfigType type alias
    using Config = typename T::ConfigType;
    /// @brief ProtectStateType type alias
    using State = typename T::ProtectStateType;
    /// @brief Profile configuration
    Config config_;
    /// @brief Profile check status
    State state_;

public:
    /// @brief Constructor
    /// @param[in] config Profile configuration
    explicit Protector(Config&& config) noexcept : config_{std::move(config)}
    {
        auto retVal = T::kE2EProtectInit(&state_);
        ComLogDebug(GenArg(retVal));
    };
    /// @brief Constructor -- deleted
    Protector() noexcept = delete;
    /// @brief Copy constructor -- deleted
    /// @param[in] other Reference to the object to be copied
    Protector(Protector const& other) noexcept = delete;
    /// @brief Assignment operator overload -- deleted
    /// @param[in] other Reference to the object to be copied
    /// @return Protector&
    Protector& operator=(Protector const& other) noexcept = delete;
    /// @brief Move constructor -- deleted
    /// @param[in] other Reference to the object to be moved
    Protector(Protector&& other) noexcept = delete;
    /// @brief Move assignment operator overload -- deleted
    /// @param[in] other Reference to the object to be moved
    /// @return Protector&
    Protector& operator=(Protector&& other) noexcept = delete;
    /// @brief Destructor
    ~Protector() noexcept final = default;

    /// @brief Protection logic implementation override -- Method protection does not support this method
    /// @param[inout] buffer Protected content, protects the e2e protection header
    ara::core::Result< void > Protect(BufferType& buffer) noexcept final
    {
        std::ignore = buffer;
        return ara::core::Result< void >::FromError(ara::com::e2e::E2EErrc::kError);
    }

    /// @brief Protection logic implementation override
    /// @param[inout] buffer Data to be protected
    /// @param[in] messageType Message type kMessageTypeRequest or kMessageTypeResponse
    /// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
    /// @param[in] sourceID Source ID
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Protect(BufferType& buffer,
                                      MessageType const& messageType,
                                      ResultType const& messageResult,
                                      SourceidType const& sourceId) noexcept final
    {
        auto retVal{T::kE2EProtect(&config_, &state_, sourceId, static_cast< uint8_t >(messageType),
                                   static_cast< uint8_t >(messageResult), buffer.data(), buffer.size())};
        ComLogDebug(T::Name(), GenArg(retVal), GenK2V("dataId", config_.DataID), GenK2V("Counter", state_.Counter - 1),
                    GenK2V("messageType", messageType), GenK2V("messageResult", messageResult),
                    GenK2V("sourceId", sourceId),
                    GenK2V("length", static_cast< typename T::LengthType >(buffer.size())));
        if (retVal == E2E_E_OK) {
            return ara::core::Result< void >::FromValue();
        }
        return ara::core::Result< void >::FromError(ara::com::e2e::E2EErrc::kError);
    }

    /// @brief
    /// @return
    ara::core::Result< uint32_t > GetCounter() noexcept final
    {
        return ara::core::Result< uint32_t >::FromValue(state_.Counter);
    };
    /// @brief Get e2e protection header length
    /// @return e2e protection header length
    uint32_t GetHeaderLength() const noexcept final { return T::kHeaderLength; }
    /// @brief Get e2e protection header offset
    /// @return Header offset
    uint32_t GetHeaderOffset() const noexcept final { return 0; }
};

/// @brief Checker template
/// @tparam T Profile type
/// @tparam M Protection type
template < typename T, typename M >
class Checker
{
};

/// @brief Checker for event protection
/// @tparam T Profile type
template < typename T >
class Checker< T, PEvent > final : public CheckerInterface
{
    /// @brief ConfigType type alias
    using Config = typename T::ConfigType;
    /// @brief ProtectStateType type alias
    using State = typename T::CheckStateType;
    /// @brief Profile configuration
    Config config_;
    /// @brief Profile check status
    State state_;
    /// @brief DataID
    typename T::IdType dataId_;
    /// @brief DataID List
    typename T::IdType dataIdList_[16];  // NOLINT -- TODO[magic-numbers]
    /// @brief Number of dataIDs, 1 or 16
    uint8_t dataIdCount_;

public:
    /// @brief Creates Checker instance using given profile configuration
    /// @param[in] config Profile configuration
    /// @ref [SWS_CM_90433] -- E2E protection/check functions shall comply with corresponding requirements and protocol specifications
    explicit Checker(Config&& config) noexcept : config_{std::move(config)}, dataId_{}, dataIdList_{}, dataIdCount_{}
    {
        DataIDInit< T >();
        auto retVal = T::kE2ECheckInit(&state_);
        ComLogDebug(GenArg(retVal));
    }

    /// @brief Constructor -- deleted
    Checker() noexcept = delete;
    /// @brief Copy constructor -- deleted
    /// @param[in] other Reference to the object to be copied
    Checker(Checker const& other) noexcept = delete;
    /// @brief Assignment operator overload -- deleted
    /// @param[in] other Reference to the object to be copied
    /// @return Checker&
    Checker& operator=(Checker const& other) noexcept = delete;
    /// @brief Move constructor -- deleted
    /// @param[in] other Reference to the object to be moved
    Checker(Checker&& other) noexcept = delete;
    /// @brief Move assignment operator overload -- deleted
    /// @param[in] other Reference to the object to be moved
    /// @return Protector&
    Checker& operator=(Checker&& other) noexcept = delete;
    /// @brief Destructor
    ~Checker() noexcept final = default;

    /// @brief Check logic implementation override
    /// @param[in] buffer Protected content
    /// @param[in] status Check result
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Check(BufferType const& buffer, e2e::ProfileCheckStatus& status) noexcept final
    {
        auto retVal{T::kE2ECheck(&config_, &state_, buffer.data(), buffer.size())};
        auto r = T::kE2EMapStatusToSM(retVal, state_.Status);
        if (dataIdCount_ == 1) {
            ComLogDebug(T::Name(), GenArg(retVal), GenK2V("dataId", dataId_), GenK2V("receivedCounter", state_.Counter),
                        GenK2V("E2EPxxCheckStatusType", state_.Status), GenK2V("e2e::ProfileCheckStatus", r),
                        GenK2V("length", static_cast< typename T::LengthType >(buffer.size())));
        } else {
            ComLogDebug(T::Name(), GenArg(retVal), GenK2V("dataId", dataIdList_),
                        GenK2V("receivedCounter", state_.Counter), GenK2V("E2EPxxCheckStatusType", state_.Status),
                        GenK2V("e2e::ProfileCheckStatus", r),
                        GenK2V("length", static_cast< typename T::LengthType >(buffer.size())));
        }
        if (r >= E2E_P_OK && r <= E2E_P_NONEWDATA) {
            status = static_cast< e2e::ProfileCheckStatus >(r);
        } else {
            status = e2e::ProfileCheckStatus::kError;
        }
        if (retVal == E2E_E_OK) {
            return ara::core::Result< void >::FromValue();
        }
        return ara::core::Result< void >::FromError(ara::com::e2e::E2EErrc::kNotAvailable);
    }
    /// @brief Initialize DataID
    /// @tparam P Profile type
    /// @return void
    template < typename P >
    std::enable_if_t< HasDataID< P >::value, void > DataIDInit() noexcept
    {
        dataId_      = config_.DataID;
        dataIdCount_ = 1;
    }
    /// @brief Initialize DataID
    /// @tparam P Profile type
    /// @return void
    template < typename P >
    std::enable_if_t< !HasDataID< P >::value, void > DataIDInit() noexcept
    {
        nai_memcpy(dataIdList_, config_.DataIDList, 16);  // NOLINT -- TODO[magic-numbers]
        config_.DataIDList = dataIdList_;
        dataIdCount_       = 16;  // NOLINT -- TODO[magic-numbers]
    }
    /// @brief Check logic implementation override -- Event protection does not support this method
    /// @param[in] buffer Data to be protected
    /// @param[in] status Check result
    /// @param[in] messageType Message type kMessageTypeRequest or kMessageTypeResponse
    /// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
    /// @param[in] sourceID Source ID
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Check(BufferType const& buffer,
                                    e2e::ProfileCheckStatus& status,
                                    MessageType const& messageType,
                                    ResultType const& messageResult,
                                    SourceidType const& sourceId) noexcept final
    {
        std::ignore = buffer;
        std::ignore = status;
        std::ignore = messageType;
        std::ignore = messageResult;
        std::ignore = sourceId;
        return ara::core::Result< void >::FromError(ara::com::e2e::E2EErrc::kError);
    }
    /// @brief Check logic implementation override -- Event protection does not support this method
    /// @param[in] buffer Data to be protected
    /// @param[in] status Check result
    /// @param[in] messageType Message type kMessageTypeRequest or kMessageTypeResponse
    /// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
    /// @param[in] sourceID Source ID
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Check(BufferType const& buffer,
                                    e2e::ProfileCheckStatus& status,
                                    MessageType const& messageType,
                                    ResultType const& messageResult,
                                    SourceidType* sourceId) noexcept final
    {
        std::ignore = buffer;
        std::ignore = status;
        std::ignore = messageType;
        std::ignore = messageResult;
        std::ignore = sourceId;
        return ara::core::Result< void >::FromError(ara::com::e2e::E2EErrc::kError);
    }
    /// @brief Get e2e protection header length
    /// @return e2e protection header length
    uint32_t GetHeaderLength() const noexcept final { return T::kHeaderLength; }
    /// @brief Get e2e protection header offset
    /// @return Header offset
    uint32_t GetHeaderOffset() const noexcept final { return GetHeaderOffset< T, Config >(config_); }
    /// @brief Parse counter from the data checked this time
    /// @return counter value
    uint32_t GetCounter() const noexcept final { return 0; };

private:
    /// @brief Get e2e protection header offset
    /// @tparam P Profile type
    /// @tparam C Profile configuration
    /// @param[in] c Configuration parameters
    /// @return Header offset
    template < typename P, typename C >
    std::enable_if_t< HasOffset< P >::value, uint32_t > GetHeaderOffset(C const& c) const noexcept
    {
        return c.Offset / 8;  // NOLINT -- TODO[magic-numbers]
    }
    /// @brief Get e2e protection header offset
    /// @tparam P Profile type
    /// @tparam C Profile configuration
    /// @param[in] c Configuration parameters
    /// @return Header offset
    template < typename P, typename C >
    std::enable_if_t< !HasOffset< P >::value, uint32_t > GetHeaderOffset(C const& c) const noexcept
    {
        std::ignore = c;
        return 0;
    }
};

/// @brief Checker for method protection
/// @tparam T Profile type
template < typename T >
class Checker< T, PMethod > final : public CheckerInterface
{
    /// @brief ConfigType type alias
    using Config = typename T::ConfigType;
    /// @brief ProtectStateType type alias
    using State = typename T::CheckStateType;
    /// @brief Profile configuration
    Config config_;
    /// @brief Profile check status
    State state_;

public:
    /// @brief Creates Checker instance using given profile configuration
    /// @param[in] config Profile configuration
    /// @ref [SWS_CM_90433] -- E2E protection/check functions shall comply with corresponding requirements and protocol specifications
    explicit Checker(Config&& config) noexcept : config_{std::move(config)}, receivedCounter_{}
    {
        auto retVal = T::kE2ECheckInit(&state_);
        std::ignore = retVal;
    }
    /// @brief Constructor -- deleted
    Checker() noexcept = delete;
    /// @brief Copy constructor -- deleted
    /// @param[in] other Reference to the object to be copied
    Checker(Checker const& other) noexcept = delete;
    /// @brief Assignment operator overload -- deleted
    /// @param[in] other Reference to the object to be copied
    /// @return Checker&
    Checker& operator=(Checker const& other) noexcept = delete;
    /// @brief Move constructor -- deleted
    /// @param[in] other Reference to the object to be moved
    Checker(Checker&& other) noexcept = delete;
    /// @brief Move assignment operator overload -- deleted
    /// @param[in] other Reference to the object to be moved
    /// @return Checker&
    Checker& operator=(Checker&& other) noexcept = delete;
    /// @brief Destructor
    ~Checker() noexcept final = default;

    /// @brief Check logic implementation override -- Method protection does not support this method
    /// @param[in] buffer Protected content, protects the e2e protection header
    /// @param[in] status Check result
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Check(BufferType const& buffer, e2e::ProfileCheckStatus& status) noexcept final
    {
        std::ignore = buffer;
        std::ignore = status;
        return ara::core::Result< void >::FromError(ara::com::e2e::E2EErrc::kError);
    }
    /// @brief Check logic implementation override
    /// @param[in] buffer Protected content, protects the e2e protection header
    /// @param[in] status Check result
    /// @param[in] messageType Message type kMessageTypeRequest or kMessageTypeResponse
    /// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
    /// @param[in] sourceID Source ID
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Check(BufferType const& buffer,
                                    e2e::ProfileCheckStatus& status,
                                    MessageType const& messageType,
                                    ResultType const& messageResult,
                                    SourceidType const& sourceId) noexcept final
    {
        auto retVal{T::kE2ESourceCheck(&config_, &state_, sourceId, static_cast< uint8_t >(messageType),
                                       static_cast< uint8_t >(messageResult), buffer.data(), buffer.size())};
        auto r = T::kE2EMapStatusToSM(retVal, state_.Status);
        ComLogDebug(T::Name(), GenArg(retVal), GenK2V("dataId", config_.DataID),
                    GenK2V("receivedCounter", state_.Counter), GenK2V("messageType", messageType),
                    GenK2V("messageResult", messageResult), GenK2V("sourceId", sourceId),
                    GenK2V("E2EPxxCheckStatusType", state_.Status), GenK2V("e2e::ProfileCheckStatus", r),
                    GenK2V("length", static_cast< typename T::LengthType >(buffer.size())));

        if (r >= E2E_P_OK && r <= E2E_P_NONEWDATA) {
            status = static_cast< e2e::ProfileCheckStatus >(r);
        } else {
            status = e2e::ProfileCheckStatus::kError;
        }
        if (retVal == E2E_E_OK) {
            receivedCounter_ = state_.Counter;
            return ara::core::Result< void >::FromValue();
        }
        return ara::core::Result< void >::FromError(ara::com::e2e::E2EErrc::kError);
    }
    /// @brief Check logic implementation override
    /// @param[in] buffer Protected content, protects the e2e protection header
    /// @param[in] status Check result
    /// @param[in] messageType Message type kMessageTypeRequest or kMessageTypeResponse
    /// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
    /// @param[inout] sourceID Source ID
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Check(BufferType const& buffer,
                                    e2e::ProfileCheckStatus& status,
                                    MessageType const& messageType,
                                    ResultType const& messageResult,
                                    SourceidType* sourceId) noexcept final
    {
        std::ignore = messageType;
        std::ignore = messageResult;
        std::ignore = sourceId;
        auto retVal{T::kE2ESinkCheck(&config_, &state_, reinterpret_cast< uint32* >(sourceId),
                                     static_cast< uint8_t >(messageType), static_cast< uint8_t >(messageResult),
                                     buffer.data(), buffer.size())};
        auto r = T::kE2EMapStatusToSM(retVal, state_.Status);
        ComLogDebug(T::Name(), GenArg(retVal), GenK2V("dataId", config_.DataID),
                    GenK2V("receivedCounter", state_.Counter), GenK2V("messageType", messageType),
                    GenK2V("messageResult", messageResult), GenK2V("sourceId", sourceId),
                    GenK2V("E2EPxxCheckStatusType", state_.Status), GenK2V("e2e::ProfileCheckStatus", r),
                    GenK2V("length", static_cast< typename T::LengthType >(buffer.size())));
        if (r >= E2E_P_OK && r <= E2E_P_NONEWDATA) {
            status = static_cast< e2e::ProfileCheckStatus >(r);
        } else {
            status = e2e::ProfileCheckStatus::kError;
        }
        if (retVal == E2E_E_OK) {
            if (buffer.size() >= GetHeaderOffset() + GetHeaderLength()) {
                typename T::CounterType receivedCounter{T::ReceivedCounter(buffer.data(), GetHeaderOffset())};
                receivedCounter_ = static_cast< uint32_t >(receivedCounter > 0 ? receivedCounter : 0);
            }
            return ara::core::Result< void >::FromValue();
        }
        return ara::core::Result< void >::FromError(ara::com::e2e::E2EErrc::kError);
    }
    /// @brief Get e2e protection header length
    /// @return e2e protection header length
    uint32_t GetHeaderLength() const noexcept final { return T::kHeaderLength; }
    /// @brief Get e2e protection header offset
    /// @return Header offset
    uint32_t GetHeaderOffset() const noexcept final { return config_.Offset / 8; }  // NOLINT -- TODO[magic-numbers]
    /// @brief Parse counter from the data checked this time
    /// @return counter value
    uint32_t GetCounter() const noexcept final { return receivedCounter_; };

private:
    /// @brief Counter value from this check
    typename T::CounterType receivedCounter_;
};

}  // namespace profile
}  // namespace com
}  // namespace ara
#endif
