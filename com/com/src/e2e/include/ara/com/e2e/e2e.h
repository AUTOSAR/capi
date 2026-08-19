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
/// @file       e2e.h
/// @brief      E2E header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_E2E_E2E_H
#define ARA_COM_E2E_E2E_H

#include "e2e_error_domain.h"
#include "profiles_01.h"
#include "profiles_02.h"
#include "profiles_04.h"
#include "profiles_04m.h"
#include "profiles_05.h"
#include "profiles_06.h"
#include "profiles_07.h"
#include "profiles_07m.h"
#include "profiles_08.h"
#include "profiles_11.h"
#include "profiles_22.h"
#include "profiles_44.h"
#include "state_machine.h"

// e2exf
#include <tuple>

#include "ara/com/internal/log/log.h"
#include "ara/core/map.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "e2e_config.h"
#include "isoft/core/set.h"

namespace ara {
namespace com {

namespace e2e {
using E2EErrorCode   = E2EErrc;
using DataID         = uint32_t;
using MessageCounter = uint32_t;
/// @brief e2e profile check status transition error threshold
/// @param[in] status e2e profile check status result
/// @return error value
inline e2e::E2EErrc ProfileCheckStatusToErrorCode(e2e::ProfileCheckStatus status) noexcept
{
    e2e::E2EErrc e2eErrorCode;
    switch (status) {
        case ara::com::e2e::ProfileCheckStatus::kRepeated:
            e2eErrorCode = ara::com::e2e::E2EErrc::kRepeated;
            break;
        case ara::com::e2e::ProfileCheckStatus::kWrongSequence:
            e2eErrorCode = ara::com::e2e::E2EErrc::kWrongSequence;
            break;
        case ara::com::e2e::ProfileCheckStatus::kError:
            e2eErrorCode = ara::com::e2e::E2EErrc::kError;
            break;
        case ara::com::e2e::ProfileCheckStatus::kNotAvailable:
            e2eErrorCode = ara::com::e2e::E2EErrc::kNotAvailable;
            break;
        case ara::com::e2e::ProfileCheckStatus::kNoNewData:
            e2eErrorCode = ara::com::e2e::E2EErrc::kNoNewData;
            break;
        default:
            e2eErrorCode = ara::com::e2e::E2EErrc::kError;
            break;
    }
    return e2eErrorCode;
}

}  // namespace e2e

namespace e2exf {
/// @brief E2E profile id type
using IdType = uint32_t;
/// @brief E2E profile buffer type
using BufferType = profile::BufferType;
/// @brief E2E profile id type
using IdlistType = ara::core::Vector< IdType >;
/// @brief E2E profile sourceid type
using SourceidType = profile::SourceidType;
/// @brief E2E profile counter type
using CounterType = uint32_t;
/// @brief E2E profile MessageType type
using MessageType = profile::MessageType;
/// @brief E2E profile MessageResult type
using ResultType = profile::MessageResult;
/// @brief E2E Config format
enum class Format : uint8_t
{
    kJson,
    kXml
};

/// @brief SOME/IP default offset
static constexpr uint16_t kApSomeipHeaderoffset{0x0040};

/// @brief Configuration loading template
/// @param[in] configFile Configuration file path
/// @param[in] configFormat Configuration file format
/// @param[in] getConfig Method to load configuration file
/// @return Loaded configuration structure
template < typename T >
T LoadConfiguration(ara::core::String const& configFile,
                    Format const configFormat,
                    std::function< T(ara::core::String const&, Format) > getConfig) noexcept
{
    return getConfig(configFile, configFormat);
}

/// @brief E2E check result
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode
class Result
{
    /// @brief State machine check status
    e2e::SMState state_;
    /// @brief Profile check status
    e2e::ProfileCheckStatus status_;
    CounterType counter_;

public:
    /// @brief Constructor
    /// @param[in] state State machine check status
    /// @param[in] status Profile check status
    Result(e2e::SMState state, e2e::ProfileCheckStatus status, CounterType counter) noexcept;
    /// @brief Constructor
    Result() noexcept;
    /// @brief Copy constructor
    /// @param[in] Reference to the object to be copied
    Result(Result const&) noexcept = default;
    /// @brief Assignment operator overload
    /// @return Result&
    Result& operator=(Result const&) noexcept = default;
    /// @brief Move constructor
    /// @param[in] Reference to the object to be moved
    Result(Result&&) noexcept = default;
    /// @brief Move assignment operator overload
    /// @return Result&
    Result& operator=(Result&&) noexcept = default;
    /// @brief Destructor
    ~Result() noexcept = default;
    /// @brief Get the state machine check result from Result
    /// @return State machine check result
    e2e::SMState GetSMState() const noexcept;
    /// @brief Get the Profile check result from Result
    /// @return Profile check result
    e2e::ProfileCheckStatus GetProfileCheckStatus() const noexcept;

    inline CounterType GetCounter() const noexcept { return counter_; }
    /// @brief Check if the result is valid
    /// @return bool true valid false invalid
    bool IsOk() const noexcept;
};

/// @brief Type alias -- ara::core::Result<Result> alias
using E2EResult = ara::core::Result< Result >;

/// @brief E2E configuration
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode
struct Config
{
    /// @brief Protector list
    ara::core::Map< IdlistType, std::shared_ptr< profile::ProtectorInterface > > eventProtectors;
    ara::core::Map< std::tuple< IdlistType, SourceidType >, std::shared_ptr< profile::ProtectorInterface > >
        methodProtectors;
    /// @brief Checker list
    ara::core::Map< IdlistType, std::shared_ptr< profile::CheckerInterface > > eventCheckers;
    ara::core::Map< std::tuple< IdlistType, SourceidType >, std::shared_ptr< profile::CheckerInterface > >
        methodCheckers;
    /// @brief State machine list
    // ara::core::Map<IdlistType, std::shared_ptr<e2e::StateMachine>> stateMachines;
    /// @brief Time-based state machine list
    ara::core::Map< IdlistType, std::shared_ptr< e2e::StateMachine > > eventSM;
    /// @brief Method state machine list
    ara::core::Map< std::tuple< IdlistType, SourceidType >, std::shared_ptr< e2e::StateMachine > > methodSM;
    // /// @brief source id list
    // ara::core::Map<IdlistType, SourceidType> sourceIds;
    /// @brief Configuration loading result isoft::kSuccess success isoft::kFailure failure or not loaded
    int32_t ret{};
};

/// @brief E2E check handle
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode
class StatusHandler
{
public:
    /// @brief Load configuration file
    /// @param[in] bindingFile e2e mapping configuration file path
    /// @param[in] bindingFormat bindingFile configuration file format
    /// @param[in] e2exfFile e2e profile configuration file path
    /// @param[in] e2exfFormat e2exfFile configuration file format
    /// @return true success false failure
    static bool Configure(ara::core::String const& bindingFile,
                          Format const bindingFormat,
                          ara::core::String const& e2exfFile,
                          Format const e2exfFormat) noexcept;

    /// @brief Get dataId
    /// @param[in] serviceId Service ID
    /// @param[in] instanceId Instance ID
    /// @param[in] eventId Event/Method ID
    /// @return data ID
    static IdlistType GetDataId(uint16_t const serviceId, uint16_t const instanceId, uint16_t const eventId) noexcept;

    static uint32_t GetSourceId(uint16_t const serviceId, uint16_t const instanceId, uint16_t const methodId) noexcept;

    static End2EndEventProtectionProps* GetEPropByDataId(IdlistType const& ids) noexcept;

    static End2EndMethodProtectionProps* GetMPropByDataId(IdlistType const& ids) noexcept;

    static uint32_t GetHeaderLength(IdlistType const& ids) noexcept;

    /// @brief Check if the service is configured with e2e protection
    /// @param[in] serviceid Service ID
    /// @return bool true configured, false not configured
    static bool HasE2E(uint16_t const serviceid) noexcept;

private:
    /// @brief
    class Impl
    {
    private:
        /// @brief Configuration loaded flag true loaded
        bool configured_{false};
        /// @brief DataID mapping list
        ara::core::Map< std::tuple< uint16_t, uint16_t, uint16_t >, IdlistType > ids_;
        /// @brief SourceId mapping list
        ara::core::Map< std::tuple< uint16_t, uint16_t, uint16_t >, SourceidType > sids_;

        ara::core::Map< IdlistType, std::unique_ptr< End2EndEventProtectionProps > > eventProps_;
        ara::core::Map< IdlistType, std::unique_ptr< End2EndMethodProtectionProps > > methodProps_;
        /// @brief Header length cache corresponding to DataId
        ara::core::Map< IdlistType, uint32_t > dataIdHeaderLengths_;
        /// @brief List of services protected by e2e
        isoft::core::Set< uint16_t > services_;

    public:
        /// @brief Load e2e mapping configuration file
        /// @param[in] bindingFile e2e mapping configuration file path
        /// @param[in] bindingFormat e2e mapping configuration file format Json, Xml
        /// @return Configuration loading result
        bool GetDataIdMapping(ara::core::String const& bindingFile, Format const bindingFormat) noexcept;

        /// @brief Configuration loading
        /// @param[in] file Configuration file path
        /// @param[in] format Configuration file format json xml
        /// @return Configuration loading result
        bool GetE2EXfConfiguration(ara::core::String const& file, Format const format) noexcept;

        uint32_t GetProfileHeaderLength(profile::ProfileName const profileName) noexcept;

    public:
        /// @brief Get Impl instance
        /// @return Impl instance
        static Impl& Instance() noexcept;

        /// @brief Load configuration file
        /// @param[in] bindingFile e2e mapping configuration file path
        /// @param[in] bindingFormat bindingFile configuration file format
        /// @param[in] e2exfFile e2e profile configuration file path
        /// @param[in] e2exfFormat e2exfFile configuration file format
        /// @return true success false failure
        bool Configure(ara::core::String const& bindingFile,
                       Format const bindingFormat,
                       ara::core::String const& e2exfFile,
                       Format const e2exfFormat) noexcept;

        /// @brief Get dataId
        /// @param[in] serviceId Service ID
        /// @param[in] instanceId Instance ID
        /// @param[in] eventId Event/Method ID
        /// @return data ID
        IdlistType GetDataId(uint16_t const serviceId, uint16_t const instanceId, uint16_t const eventId) noexcept;

        /// @brief Get sourceId
        /// @param serviceId
        /// @param instanceId
        /// @param methodId
        /// @return
        uint32_t GetSourceId(uint16_t const serviceId, uint16_t const instanceId, uint16_t const methodId) noexcept;

        End2EndEventProtectionProps* GetEPropByDataId(IdlistType const& ids) noexcept;

        End2EndMethodProtectionProps* GetMPropByDataId(IdlistType const& ids) noexcept;

        uint32_t GetHeaderLength(IdlistType const& ids) noexcept;

        // /// @brief Get the corresponding list of dataId and sourceId
        // /// @return Returns the corresponding list of dataId and sourceId
        // ara::core::Map<IdlistType, ara::core::Vector<SourceidType>>& GetIdSidList() noexcept;

        /// @brief Check if the service is configured with e2e protection
        /// @param[in] serviceid Service ID
        /// @return bool true configured, false not configured
        inline bool HasE2E(uint16_t const serviceid) noexcept
        {
            if (services_.find(serviceid) == services_.end()) {
                return false;
            }
            return true;
        }
    };
};

/// @brief E2E protection and check class
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode
class Transformer
{
public:
    /// @brief Configuration loading
    /// @param[in] file Configuration file path
    /// @param[in] format Configuration file format json xml
    /// @return true loading success false loading failure
    static bool Configure(ara::core::String const& file, Format const format = Format::kJson) noexcept;

    /// @brief e2e check wrapper for events
    /// @param[in] dataId dataID
    /// @param[in] buffer Data to be checked
    /// @return Check result
    static E2EResult E2E_check(IdlistType const& dataId, BufferType& buffer) noexcept;

    /// @brief e2e check wrapper for events
    /// @param[in] dataId dataID
    /// @param[in] buffer Data to be checked
    /// @return Check result
    static E2EResult E2E_check(IdlistType const& dataId, BufferType* buffer) noexcept;

    /// @brief e2e protection wrapper for events
    /// @param[in] dataId dataID
    /// @param[inout] buffer Data to be protected
    /// @return Result object -- empty/value or error
    static ara::core::Result< void > E2E_protect(IdlistType const& dataId, BufferType& buffer) noexcept;

    /// @brief e2e check wrapper for methods (proxy side)
    /// @param[in] dataId dataID
    /// @param[in] messageType Message type kMessageTypeResponse
    /// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
    /// @param[in] sourceID Source ID
    /// @param[in] buffer Data to be checked
    /// @return Result object -- check result or error
    static E2EResult E2E_check(IdlistType const& dataId,
                               MessageType const& messageType,
                               ResultType const& messageResult,
                               SourceidType const& sourceID,
                               BufferType& buffer) noexcept;

    /// @brief e2e check wrapper for methods (skeleton side)
    /// @param[in] dataId dataID
    /// @param[in] messageType Message type kMessageTypeRequest
    /// @param[in] messageResult Result type in response message
    /// @param[out] sourceID Source ID
    /// @param[in] buffer Data to be checked
    /// @return Result object -- check result or error
    static E2EResult E2E_check(IdlistType const& dataId,
                               uint16_t const client,
                               uint16_t const methodId,
                               MessageType const& messageType,
                               ResultType const& messageResult,
                               SourceidType* sourceID,
                               BufferType& buffer) noexcept;

    /// @brief e2e protection wrapper for methods
    /// @param[in] dataId dataID
    /// @param[in] messageType Message type kMessageTypeRequest or kMessageTypeResponse
    /// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
    /// @param[in] sourceID Source ID
    /// @param[inout] buffer Data to be protected
    /// @return Result object -- empty/value or error
    static ara::core::Result< void > E2E_protect(IdlistType const& dataId,
                                                 MessageType const& messageType,
                                                 ResultType const& messageResult,
                                                 SourceidType const& sourceID,
                                                 BufferType& buffer) noexcept;

    /// @brief Get e2e protection header length
    /// @param[in] dataId DataID
    /// @return e2e protection header length
    static uint32_t E2E_GetHeaderLength(IdlistType const& dataId) noexcept;

    /// @brief Check if dataid is configured with e2e protection
    /// @param[in] dataId dataid
    /// @return bool true configured with protection false not configured
    static bool E2E_IsProtected(IdlistType const& dataId) noexcept;

    /// @brief Insert clientId to sourceId mapping into cache
    /// @param[in] clientId Client ID
    /// @param[in] sourceId Source ID
    static void E2E_InsertClientSourceIdMap(uint16_t const clientId,
                                            uint16_t const methodId,
                                            SourceidType const& sourceId) noexcept;

    /// @brief Get sourceid corresponding to clientId
    /// @param[in] clientId Client ID
    /// @return Result object -- SourceidType/value or error
    static ara::core::Result< SourceidType > E2E_ClientIdToSourceId(uint16_t const clientId,
                                                                    uint16_t const methodId) noexcept;

    /// @brief
    /// @param dataId
    /// @return
    static ara::core::Result< uint32_t > E2E_GetProtecterCounter(IdlistType const& dataId,
                                                                 SourceidType const& sourceID) noexcept;

private:
    /// @brief Transformer instance pointer
    static std::shared_ptr< Transformer > s_Instance_;
    /// @brief e2e configuration
    Config config_;
    /// @brief clientid methodId sourceid mapping map
    ara::core::Map< std::tuple< uint16_t, uint16_t >, SourceidType > clientSourceIdMap_;

    /// @brief Configuration loading
    /// @param[in] file Configuration file path
    /// @param[in] format Configuration file format json xml
    /// @return Configuration loading result
    static Config GetE2EXfConfiguration(ara::core::String const& file, Format const format) noexcept;

public:
    /// @brief Constructor
    /// @param[in] file Configuration file path
    /// @param[in] format Configuration file format
    explicit Transformer(ara::core::String const& file, Format const format = Format::kJson) noexcept;
    /// @brief Constructor
    /// @param[in] config Configuration structure
    explicit Transformer(Config&& config) noexcept;
    /// @brief Constructor
    Transformer() noexcept;
    /// @brief Constructor
    ~Transformer() noexcept = default;
    /// @brief Copy constructor
    Transformer(Transformer const&) noexcept = default;
    /// @brief Assignment operator overload
    /// @return Transformer&
    Transformer& operator=(Transformer const&) noexcept = default;
    /// @brief Move constructor
    Transformer(Transformer&&) noexcept = default;
    /// @brief Move assignment operator overload
    /// @return Transformer&
    Transformer& operator=(Transformer&&) noexcept = default;

    /// @brief Check if dataid is configured with e2e protection
    /// @param[in] id dataid
    /// @return bool true configured with protection false not configured
    bool IsProtected(IdType const id) const noexcept;

    /// @brief Check if dataid is configured with e2e protection
    /// @param[in] id dataid
    /// @return bool true configured with protection false not configured
    bool IsProtected(IdlistType const& id) const noexcept;

    /// @brief e2e protection wrapper for events
    /// @param[in] dataID dataID
    /// @param[inout] buffer Data to be protected
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Protect(IdlistType const& dataID, BufferType& buffer) noexcept;

    /// @brief e2e protection wrapper for methods
    /// @param[in] id dataID
    /// @param[inout] buffer Data to be protected
    /// @param[in] messageType Message type kMessageTypeRequest or kMessageTypeResponse
    /// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
    /// @param[in] sourceId Source ID
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Protect(IdlistType const& id,
                                      BufferType& buffer,
                                      MessageType const& messageType,
                                      ResultType const& messageResult,
                                      SourceidType const& sourceId) noexcept;

    /// @brief e2e protection wrapper for methods
    /// @param[in] id dataID list
    /// @param[in] buffer Data to be protected
    /// @return Result object -- empty/value or error
    E2EResult Check(IdlistType const& id, BufferType const& buffer) noexcept;

    /// @brief e2e protection wrapper for events
    /// @param[in] id dataID list
    /// @return Result object -- empty/value or error
    E2EResult Check(IdlistType const& id) noexcept;

    /// @brief e2e check wrapper for methods (proxy side)
    /// @param[in] id dataID list
    /// @param[in] buffer Data to be checked
    /// @param[in] messageType Message type kMessageTypeResponse
    /// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
    /// @param[in] sourceId Source ID
    /// @return Result object -- check result or error
    E2EResult Check(IdlistType const& id,
                    BufferType const& buffer,
                    MessageType const& messageType,
                    ResultType const& messageResult,
                    SourceidType const& sourceId) noexcept;

    /// @brief e2e check wrapper for methods (skeleton side)
    /// @param[in] id dataID list
    /// @param[in] buffer Data to be checked
    /// @param[in] messageType Message type kMessageTypeRequest
    /// @param[in] messageResult Result type in response message
    /// @param[inout] sourceId Source ID
    /// @return Result object -- check result or error
    E2EResult Check(IdlistType const& id,
                    uint16_t const client,
                    uint16_t const methodId,
                    BufferType const& buffer,
                    MessageType const& messageType,
                    ResultType const& messageResult,
                    SourceidType* sourceId) noexcept;

    /// @brief Get e2e protection header length
    /// @param[in] dataId DataID
    /// @return e2e protection header length
    uint32_t GetHeaderLength(IdlistType const& dataId) noexcept;

    /// @brief Insert clientId to sourceId mapping into cache
    /// @param[in] clientId Client ID
    /// @param[in] sourceId Source ID
    void InsertClientSourceIdMap(uint16_t const clientId,
                                 uint16_t const methodId,
                                 SourceidType const& sourceId) noexcept;

    /// @brief Get sourceid corresponding to clientId
    /// @param[in] clientId Client ID
    /// @return Result object -- SourceidType/value or error
    ara::core::Result< SourceidType > ClientIdToSourceId(uint16_t const clientId, uint16_t const methodId) noexcept;

    /// @brief Reset Protector counter, solving counter reset issue when method requester restarts.
    /// @param[in] id DataID
    /// @param[in] sourceId sourceid
    /// @return Result object -- uint32_t/value or error
    ara::core::Result< uint32_t > GetProtecterCounter(IdlistType const& id, SourceidType const& sourceId) noexcept;
};

#if 0
/// @brief E2E shared pointer
/// @tparam T
/// @note [Todo] unused
template < typename T >
class SharedPtr
{
    std::shared_ptr< T > ptr;
    e2e::ProfileCheckStatus status;

public:
    template < typename... Args, std::enable_if_t< sizeof...(Args) != 1, bool > = true >
    SharedPtr(Args&&... args) noexcept : ptr{{std::forward< Args >(args)...}}, status{}
    {
    }
    template < typename U, std::enable_if_t< !std::is_base_of< SharedPtr, U >::value, bool > = true >
    SharedPtr(U&& u) noexcept : ptr{std::forward< U >(u)}, status{}
    {
    }
    decltype(auto) operator*() const noexcept { return ptr.operator*(); }
    decltype(auto) operator->() const noexcept { return ptr.operator->(); }
    e2e::ProfileCheckStatus GetProfileCheckStatus() const noexcept { return status; }
};
#endif

/// @brief enable E2E
/// @tparam T
template < typename T >
struct EnableE2E : std::true_type
{
};

/// @brief get E2E check result(enable E2E)
/// @return
/// @tparam T
/// @param[in] event
template < typename T >
typename std::enable_if_t< EnableE2E< T >::value, E2EResult > GetE2ECheckResult(T const& event) noexcept
{
    return event.GetE2EResult();
}
/// @brief get E2E check result(disable E2E)
/// @return
/// @param[in] event
/// @tparam T
template < typename T >
typename std::enable_if_t< !EnableE2E< T >::value, E2EResult > GetE2ECheckResult(T const& event) noexcept
{
    std::ignore = event;
    return E2EResult{{}};
}

/// @brief get E2E profile check status(enable E2E)
/// @param[in] ptr
/// @return
/// @tparam T
template < typename T >
typename std::enable_if_t< EnableE2E< T >::value, e2e::ProfileCheckStatus > GetProfileCheckStatus(T const& ptr) noexcept
{
    return ptr.GetProfileCheckStatus();
}
/// @brief get E2E profile check status(disable E2E)
/// @tparam T
/// @param[in] ptr
/// @return
template < typename T >
typename std::enable_if_t< !EnableE2E< T >::value, e2e::ProfileCheckStatus > GetProfileCheckStatus(
    T const& ptr) noexcept
{
    std::ignore = ptr;
    return e2e::ProfileCheckStatus{};
}
}  // namespace e2exf
}  // namespace com
}  // namespace ara
#endif
