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
/// @file       msg_type.h
/// @brief      Define messages used for communication between Client and Server
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Utils
/// @unit_name=FGStateMsg
/// @interface_level=uint
/// @unit_description=Define messages used for communication between Client and Server
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @endcode
///
/// ================================================================

#ifndef ARA_SM_INTERNAL_MSG_TYPE_H_
#define ARA_SM_INTERNAL_MSG_TYPE_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <ara/log/logger.h>

#include <cassert>
#include <cstdint>
#include <cstring>

#include "common.h"
#include "fg_state_internal_type.h"

namespace ara {
namespace sm {
namespace fg_state_ipc {

/// @brief Define alias to resolve qac2428 item: Direct use of character type.
/// @param Char8_t alias of char
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using Char8_t = char;

/// @brief Message enumeration definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08097
/// @trace_id_dd=DD_SM_08356
/// @needwork = ad
/// @endcode
enum class MsgType : uint32_t
{
    kRequestAllFGState      = 0,  ///< request all function group state message
    kRequestAllFGStateResp  = 1,  ///< response of request all function group state message
    kSubscribeFGState       = 2,  ///< subscribe function group state message
    kSubscribeFGStateResp   = 3,  ///< response of subscribe function group state message
    kFGStateNotify          = 4,  ///< function group state change notify message
    kUnsubscribeFGState     = 5,  ///< unsubscribe function group state message
    kUnsubscribeFGStateResp = 6,  ///< response of unsubscribe function group state message
    kUnkown                 = 7   ///< unkown message
};

/// @brief Group state message
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08098
/// @trace_id_dd=DD_SM_08357
/// @needwork = ad
/// @endcode
class FGStateMsg
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00372
    /// @trace_id_dd=DD_SM_00388
    /// @needwork = ad
    /// @endcode
    FGStateMsg() noexcept : FGStateMsg{MsgType::kUnkown} {}

    /// @brief Constructor function
    /// @param type the type
    /// @exception on overflow
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00373
    /// @trace_id_dd=DD_SM_00389
    /// @needwork = ad
    /// @endcode
    explicit FGStateMsg(MsgType const &type) noexcept : type_{type}, len_{0U} { _AddFixedLengthType2Len(len_, type_); }

    /// @brief Constructor function
    /// @param type the type
    /// @param len Length
    /// @exception on overflow
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00374
    /// @trace_id_dd=DD_SM_00390
    /// @needwork = ad
    /// @endcode
    FGStateMsg(MsgType const &type, std::size_t const len) noexcept : type_{type}, len_{len}
    {
        _AddFixedLengthType2Len(len_, type_);
    }

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00375
    /// @trace_id_dd=DD_SM_00391
    /// @needwork = ad
    /// @endcode
    virtual ~FGStateMsg() = default;

    /// @brief deleted copy constructor function
    /// @param other The FGStateMsg instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00376
    /// @trace_id_dd=DD_SM_00392
    /// @needwork = ad
    /// @endcode
    FGStateMsg(FGStateMsg const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The FGStateMsg instance to be copyed
    /// @return the assigned FGStateMsg instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00377
    /// @trace_id_dd=DD_SM_00393
    /// @needwork = ad
    /// @endcode
    FGStateMsg &operator=(FGStateMsg const &other) = delete;

    /// @brief deleted move constructor function
    /// @param other The FGStateMsg instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00378
    /// @trace_id_dd=DD_SM_00394
    /// @needwork = ad
    /// @endcode
    FGStateMsg(FGStateMsg &&other) = delete;

    /// @brief deleted move assignment function
    /// @param other The FGStateMsg instance to be moved
    /// @return the assigned FGStateMsg instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00379
    /// @trace_id_dd=DD_SM_00395
    /// @needwork = ad
    /// @endcode
    FGStateMsg &operator=(FGStateMsg &&other) = delete;

    /// @brief Get the message size
    /// @return Number of message bytes
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00380
    /// @trace_id_dd=DD_SM_00396
    /// @needwork = ad
    /// @endcode
    inline std::size_t GetSize() const noexcept { return len_; }

    /// @brief  qac requires member variables to be private only
    /// @param  len Parameter len
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00381
    /// @trace_id_dd=DD_SM_00397
    /// @needwork = ad
    /// @endcode
    void SetSize(std::size_t const len) noexcept { len_ = len; }

    /// @brief  qac requires member variables to be private only
    /// @return type_ Member variable type_
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00382
    /// @trace_id_dd=DD_SM_00398
    /// @needwork = ad
    /// @endcode
    inline MsgType GetType() const noexcept { return type_; }

    /// @brief  qac requires member variables to be private only
    /// @param  type Parameter type
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00383
    /// @trace_id_dd=DD_SM_00399
    /// @needwork = ad
    /// @endcode
    void SetType(MsgType const type) noexcept { type_ = type; }

    /// @brief Serialize
    /// @param p Serialization target address
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00384
    /// @trace_id_dd=DD_SM_00400
    /// @needwork = ad
    /// @endcode
    virtual void Serialize(uint8_t *p) noexcept
    {
        // Message type
        _AppendFixedLengthType(p, type_);
    }

    /// @brief Deserialize
    /// @param data Deserialization source address
    /// @param len Length
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00385
    /// @trace_id_dd=DD_SM_00401
    /// @needwork = ad
    /// @endcode
    virtual void Deserialize(uint8_t *data, std::size_t const &len) noexcept
    {
        _RecoverFixedLengthType(data, type_);
        std::ignore = len;
    }

protected:
    /// @brief Template function, appends the length of fixTypeObj to len
    /// @param len Length
    /// @param fixTypeObj Fixed-size variable
    /// @code{.isoft}
    /// @tparam T Template
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08358
    /// @needwork = dda
    /// @endcode
    template < typename T >
    void _AddFixedLengthType2Len(std::size_t &len, T const &fixTypeObj) const noexcept
    {
        // for qac: This non static member function does not access any member data.qacpp-5.3.0(4212)
        std::ignore = len_;

        len += sizeof(fixTypeObj);  // Append fixed type length
    }

    /// @brief Template function, appends the content of fixTypeObj to the target address
    /// @param p Target address to be appended
    /// @param fixTypeObj Fixed-length variable
    /// @code{.isoft}
    /// @tparam T Template
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08359
    /// @needwork = dda
    /// @endcode
    template < typename T >
    void _AppendFixedLengthType(uint8_t *&p, T const &fixTypeObj) const noexcept
    {
        // for qac: This non static member function does not access any member data.qacpp-5.3.0(4212)
        std::ignore = len_;

        std::ignore = memcpy(p, &fixTypeObj, sizeof(fixTypeObj));
        p += sizeof(fixTypeObj);
    }

    /// @brief Restore fixed length
    /// @param p Source address for restoring fixed-length variables
    /// @param fixTypeObj Fixed-length variable
    /// @code{.isoft}
    /// @tparam T Template
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08360
    /// @needwork = dda
    /// @endcode
    template < typename T >
    void _RecoverFixedLengthType(uint8_t *&p, T &fixTypeObj) const noexcept
    {
        // for qac: This non static member function does not access any member data.qacpp-5.3.0(4212)
        std::ignore = len_;

        std::ignore = std::memcpy(&fixTypeObj, p, sizeof(fixTypeObj));
        p += sizeof(fixTypeObj);
    }

    /// @brief Append string length
    /// @param len Length
    /// @param str String
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08361
    /// @needwork = dda
    /// @endcode
    void _AddString2Len(std::size_t &len, core::String const &str) const noexcept
    {
        // for qac: This non static member function does not access any member data.qacpp-5.3.0(4212)
        std::ignore = len_;

        len += sizeof(core::String::size_type);  // Append string length size
        len += str.length();                     // Append string length
    }

    /// @brief Append string
    /// @param p Target address to be appended
    /// @param str String
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08362
    /// @needwork = dda
    /// @endcode
    void _AppendString(uint8_t *&p, core::String const &str) const noexcept
    {
        // for qac: This non static member function does not access any member data.qacpp-5.3.0(4212)
        std::ignore = len_;

        core::String::size_type const len{str.length()};
        std::ignore = memcpy(p, &len, sizeof(len));
        p += sizeof(len);

        std::ignore = memcpy(p, str.data(), len);
        p += len;
    }

    /// @brief Restore string
    /// @param p Source address for restoring the string
    /// @param str String
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08363
    /// @needwork = dda
    /// @endcode
    void _RecoverString(uint8_t *&p, core::String &str) const noexcept
    {
        // for qac: This non static member function does not access any member data.qacpp-5.3.0(4212)
        std::ignore = len_;

        core::Vector< FGStateInternalType >::size_type sz{};
        std::ignore = std::memcpy(&sz, p, sizeof(sz));

        core::String::size_type len{};
        std::ignore = std::memcpy(&len, p, sizeof(len));  // String length
        p += sizeof(len);

        core::String strData;

        // Copy data
        for (size_t i{0U}; i < len; ++i) {
            std::ignore = strData.append(1UL, static_cast< Char8_t >(p[i]));
        }

        std::ignore = str.append(strData);
        p += len;
    }

private:
    /// @brief Message type
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08364
    /// @needwork = dda
    /// @endcode
    MsgType type_;

    /// @brief Length
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08365
    /// @needwork = dda
    /// @endcode
    std::size_t len_;
};

/// @brief Structure for subscribing to function group state
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_02002
/// @trace_id_ad=AD_SM_08099
/// @trace_id_dd=DD_SM_08366
/// @needwork = ad
/// @endcode
class SubscribeFGStateMsg : public FGStateMsg
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00386
    /// @trace_id_dd=DD_SM_00402
    /// @needwork = ad
    /// @endcode
    SubscribeFGStateMsg() noexcept : FGStateMsg{MsgType::kUnkown}, subscriberId_{} {}

    /// @brief Constructor function
    /// @param subscriberID Subscriber ID
    /// @param msgType
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00387
    /// @trace_id_dd=DD_SM_00403
    /// @needwork = ad
    /// @endcode
    explicit SubscribeFGStateMsg(core::String subscriberID,
                                 MsgType const &msgType = MsgType::kSubscribeFGState) noexcept
        : FGStateMsg{msgType}, subscriberId_{std::move(subscriberID)}
    {
        std::size_t len{GetSize()};
        MsgType const type{GetType()};

        /// @brief Message type length
        _AddFixedLengthType2Len(len, type);

        /// @brief Subscriber ID length
        _AddString2Len(len, subscriberId_);

        /// Set len
        SetSize(len);
    }

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00388
    /// @trace_id_dd=DD_SM_00404
    /// @needwork = ad
    /// @endcode
    ~SubscribeFGStateMsg() final = default;

    /// @brief deleted copy constructor function
    /// @param other The SubscribeFGStateMsg instance to be copyed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00389
    /// @trace_id_dd=DD_SM_00405
    /// @needwork = ad
    /// @endcode
    SubscribeFGStateMsg(SubscribeFGStateMsg const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The SubscribeFGStateMsg instance to be copyed
    /// @return the assigned SubscribeFGStateMsg instance itself
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00390
    /// @trace_id_dd=DD_SM_00406
    /// @needwork = ad
    /// @endcode
    SubscribeFGStateMsg &operator=(SubscribeFGStateMsg const &other) = delete;

    /// @brief deleted move constructor function
    /// @param other The SubscribeFGStateMsg instance to be moved
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00391
    /// @trace_id_dd=DD_SM_00407
    /// @needwork = ad
    /// @endcode
    SubscribeFGStateMsg(SubscribeFGStateMsg &&other) = delete;

    /// @brief deleted move assignment function
    /// @param other The SubscribeFGStateMsg instance to be moved
    /// @return the assigned SubscribeFGStateMsg instance itself
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00392
    /// @trace_id_dd=DD_SM_00408
    /// @needwork = ad
    /// @endcode
    SubscribeFGStateMsg &operator=(SubscribeFGStateMsg &&other) = delete;

    /// @brief Serialize
    /// @param p Serialization target address
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00393
    /// @trace_id_dd=DD_SM_00409
    /// @needwork = ad
    /// @endcode
    void Serialize(uint8_t *p) noexcept final
    {
        MsgType const type{GetType()};
        // Message type
        _AppendFixedLengthType(p, type);

        // Subscriber ID
        _AppendString(p, subscriberId_);
    }

    /// @brief Deserialize
    /// @param data Deserialization source address
    /// @param len Length
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00394
    /// @trace_id_dd=DD_SM_00410
    /// @needwork = ad
    /// @endcode
    void Deserialize(uint8_t *const data, std::size_t const &len) noexcept final
    {
        uint8_t *p{data};
        MsgType type{GetType()};

        std::ignore = len;
        assert(len > 0U);

        // Message type
        _RecoverFixedLengthType(p, type);
        SetType(type);

        // Subscriber ID
        _RecoverString(p, subscriberId_);
    }

    /// @brief  qac requires member variables to be private only
    /// @return subscriberId_ Member variable subscriberId_
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00395
    /// @trace_id_dd=DD_SM_00411
    /// @needwork = ad
    /// @endcode
    core::String GetSubscriberId() const noexcept { return subscriberId_; }

    /// @brief  qac requires member variables to be private only
    /// @param  subscriberId Parameter subscriberId
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00396
    /// @trace_id_dd=DD_SM_00412
    /// @needwork = ad
    /// @endcode
    void SetSubscriberId(core::String const &subscriberId) noexcept { subscriberId_ = subscriberId; }

private:
    /// @brief   Subscriber ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08367
    /// @needwork = dda
    /// @endcode
    core::String subscriberId_;
};

/// @brief Reply message
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_02002
/// @trace_id_ad=AD_SM_08100
/// @trace_id_dd=DD_SM_08368
/// @needwork = ad
/// @endcode
class SubscribeFGStateRespMsg : public FGStateMsg
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00397
    /// @trace_id_dd=DD_SM_00413
    /// @needwork = ad
    /// @endcode
    SubscribeFGStateRespMsg() noexcept : FGStateMsg{}, errorStr_{ara::sm::fg_state_ipc::Get_ERROR_INFO_SUCCESS()} {}

    /// @brief Constructor function
    /// @param errorCode Error code
    /// @param errorStr Error string
    /// @param msgType Message type
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00398
    /// @trace_id_dd=DD_SM_00414
    /// @needwork = ad
    /// @endcode
    explicit SubscribeFGStateRespMsg(uint32_t const &errorCode,
                                     core::String errorStr,
                                     MsgType const &msgType = MsgType::kSubscribeFGStateResp) noexcept
        : FGStateMsg{msgType}, errorCode_{errorCode}, errorStr_{std::move(errorStr)}
    {
        // First calculate the length
        SetSize(0U);
        // Get len and type
        std::size_t len{GetSize()};
        MsgType const type{GetType()};
        // Message type length
        _AddFixedLengthType2Len(len, type);

        // Error code
        _AddFixedLengthType2Len(len, errorCode_);

        // Error information
        _AddString2Len(len, errorStr_);
        /// Set len
        SetSize(len);
    }

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00399
    /// @trace_id_dd=DD_SM_00415
    /// @needwork = ad
    /// @endcode
    ~SubscribeFGStateRespMsg() override = default;

    /// @brief deleted copy constructor function
    /// @param other The SubscribeFGStateRespMsg instance to be copyed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00400
    /// @trace_id_dd=DD_SM_00416
    /// @needwork = ad
    /// @endcode
    SubscribeFGStateRespMsg(SubscribeFGStateRespMsg const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The SubscribeFGStateRespMsg instance to be copyed
    /// @return the assigned SubscribeFGStateRespMsg instance itself
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00401
    /// @trace_id_dd=DD_SM_00417
    /// @needwork = ad
    /// @endcode
    SubscribeFGStateRespMsg &operator=(SubscribeFGStateRespMsg const &other) = delete;

    /// @brief deleted move constructor function
    /// @param other The SubscribeFGStateRespMsg instance to be moved
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00402
    /// @trace_id_dd=DD_SM_00418
    /// @needwork = ad
    /// @endcode
    SubscribeFGStateRespMsg(SubscribeFGStateRespMsg &&other) = delete;

    /// @brief deleted move assignment function
    /// @param other The SubscribeFGStateRespMsg instance to be moved
    /// @return the assigned SubscribeFGStateRespMsg instance itself
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00403
    /// @trace_id_dd=DD_SM_00419
    /// @needwork = ad
    /// @endcode
    SubscribeFGStateRespMsg &operator=(SubscribeFGStateRespMsg &&other) = delete;

    /// @brief Serialize
    /// @param p Serialization target address
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00404
    /// @trace_id_dd=DD_SM_00420
    /// @needwork = ad
    /// @endcode
    void Serialize(uint8_t *p) noexcept final
    {
        // Get type
        MsgType const type{GetType()};
        // Message type
        _AppendFixedLengthType(p, type);

        // Error code
        _AppendFixedLengthType(p, errorCode_);

        // Error information
        _AppendString(p, errorStr_);
    }

    /// @brief Deserialize
    /// @param data Deserialization source address
    /// @param len Length
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00405
    /// @trace_id_dd=DD_SM_00421
    /// @needwork = ad
    /// @endcode
    void Deserialize(uint8_t *const data, std::size_t const &len) noexcept final
    {
        uint8_t *p{data};

        std::ignore = len;

        assert(len > 0U);

        // Get type
        MsgType type{GetType()};
        // Message type
        _RecoverFixedLengthType(p, type);
        // Set type
        SetType(type);

        // Error code
        _RecoverFixedLengthType(p, errorCode_);

        // Error information
        errorStr_ = "";
        _RecoverString(p, errorStr_);
    }

    /// @brief  qac requires member variables to be private only
    /// @return errorCode_ Member variable errorCode_
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00406
    /// @trace_id_dd=DD_SM_00422
    /// @needwork = ad
    /// @endcode
    uint32_t const &GetErrorCode() const noexcept { return errorCode_; }

    /// @brief  qac requires member variables to be private only
    /// @param  errorCode Parameter errorCode
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00407
    /// @trace_id_dd=DD_SM_00423
    /// @needwork = ad
    /// @endcode
    void SetErrorCode(uint32_t const errorCode) noexcept { errorCode_ = errorCode; }

    /// @brief  qac requires member variables to be private only
    /// @return errorStr_ Member variable errorStr_
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00408
    /// @trace_id_dd=DD_SM_00424
    /// @needwork = ad
    /// @endcode
    core::String GetErrorStr() const noexcept { return errorStr_; }

    /// @brief  qac requires member variables to be private only
    /// @param  errorStr Parameter errorStr
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00409
    /// @trace_id_dd=DD_SM_00425
    /// @needwork = ad
    /// @endcode
    void SetErrorStr(core::String const &errorStr) noexcept { errorStr_ = errorStr; }

private:
    /// @brief Error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08369
    /// @needwork = dda
    /// @endcode
    uint32_t errorCode_{0U};

    /// @brief Error information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08370
    /// @needwork = dda
    /// @endcode
    core::String errorStr_;
};

/// @brief Request all messages
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_02001
/// @trace_id_ad=AD_SM_08101
/// @trace_id_dd=DD_SM_08371
/// @needwork = ad
/// @endcode
class RequestAllFGStateRespMsg : public FGStateMsg
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00410
    /// @trace_id_dd=DD_SM_00426
    /// @needwork = ad
    /// @endcode
    RequestAllFGStateRespMsg() = default;

    /// @brief Copy constructor
    /// @param fGStateVec
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00411
    /// @trace_id_dd=DD_SM_00427
    /// @needwork = ad
    /// @endcode
    explicit RequestAllFGStateRespMsg(core::Vector< FGStateInternalType > fGStateVec) noexcept
        : FGStateMsg{MsgType::kRequestAllFGStateResp}, fGStateVec_{std::move(fGStateVec)}
    {
        // First calculate the length
        SetSize(0U);
        // Get len and type
        std::size_t len{GetSize()};
        MsgType const type{GetType()};

        // Message type length
        _AddFixedLengthType2Len(len, type);

        len += sizeof(core::Vector< FGStateInternalType >::size_type);  // Length of information array
        for (auto &info : fGStateVec_) {
            _AddString2Len(len, info.fgName);   // Function group state name
            _AddString2Len(len, info.fgState);  // Function group state
        }
        // Set len
        SetSize(len);
    }

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00412
    /// @trace_id_dd=DD_SM_00428
    /// @needwork = ad
    /// @endcode
    ~RequestAllFGStateRespMsg() override = default;

    /// @brief deleted copy constructor function
    /// @param other The RequestAllFGStateRespMsg instance to be copyed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00413
    /// @trace_id_dd=DD_SM_00429
    /// @needwork = ad
    /// @endcode
    RequestAllFGStateRespMsg(RequestAllFGStateRespMsg const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The RequestAllFGStateRespMsg instance to be copyed
    /// @return the assigned RequestAllFGStateRespMsg instance itself
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00414
    /// @trace_id_dd=DD_SM_00430
    /// @needwork = ad
    /// @endcode
    RequestAllFGStateRespMsg &operator=(RequestAllFGStateRespMsg const &other) = delete;

    /// @brief deleted move constructor function
    /// @param other The RequestAllFGStateRespMsg instance to be moved
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00415
    /// @trace_id_dd=DD_SM_00431
    /// @needwork = ad
    /// @endcode
    RequestAllFGStateRespMsg(RequestAllFGStateRespMsg &&other) = delete;

    /// @brief deleted move assignment function
    /// @param other The RequestAllFGStateRespMsg instance to be moved
    /// @return the assigned RequestAllFGStateRespMsg instance itself
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00416
    /// @trace_id_dd=DD_SM_00432
    /// @needwork = ad
    /// @endcode
    RequestAllFGStateRespMsg &operator=(RequestAllFGStateRespMsg &&other) = delete;

    /// @brief Serialize
    /// @param p Serialization target address
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00417
    /// @trace_id_dd=DD_SM_00433
    /// @needwork = ad
    /// @endcode
    void Serialize(uint8_t *p) noexcept final
    {
        // Get type
        MsgType const type{GetType()};
        // Message type
        _AppendFixedLengthType(p, type);

        // Information array
        core::Vector< FGStateInternalType >::size_type const sz{fGStateVec_.size()};  // Length
        std::ignore = memcpy(p, &sz, sizeof(sz));
        p += sizeof(sz);
        for (auto &info : fGStateVec_) {
            _AppendString(p, info.fgName);   // Function group state name
            _AppendString(p, info.fgState);  // Function group state
        }
    }

    /// @brief Deserialize
    /// @param data Deserialization source address
    /// @param len Length
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00418
    /// @trace_id_dd=DD_SM_00434
    /// @needwork = ad
    /// @endcode
    void Deserialize(uint8_t *const data, std::size_t const &len) noexcept final
    {
        uint8_t *p{data};

        std::ignore = len;
        assert(len > 0U);

        // Get type
        MsgType type{GetType()};
        // Message type
        _RecoverFixedLengthType(p, type);
        // Set type
        SetType(type);

        // Length of information array
        core::Vector< FGStateInternalType >::size_type sz{};
        std::ignore = std::memcpy(&sz, p, sizeof(sz));
        p += sizeof(sz);

        fGStateVec_.clear();
        for (core::Vector< FGStateInternalType >::size_type i{0U}; i < sz; i++) {
            FGStateInternalType info{};
            _RecoverString(p, info.fgName);   // Function group state name
            _RecoverString(p, info.fgState);  // Function group state
            fGStateVec_.emplace_back(info);
        }
    }

    /// @brief  qac requires member variables to be private only
    /// @return fGStateVec_ Member variable FGStateVec_
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00419
    /// @trace_id_dd=DD_SM_00435
    /// @needwork = ad
    /// @endcode
    core::Vector< FGStateInternalType > GetFGStateVec() const noexcept { return fGStateVec_; }

    /// @brief  qac requires member variables to be private only
    /// @param  fGStateVec Parameter fGStateVec
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00420
    /// @trace_id_dd=DD_SM_00436
    /// @needwork = ad
    /// @endcode
    void SetFGStateVec(core::Vector< FGStateInternalType > const &fGStateVec) noexcept { fGStateVec_ = fGStateVec; }

private:
    /// @brief State list
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08372
    /// @needwork = dda
    /// @endcode
    core::Vector< FGStateInternalType > fGStateVec_;
};

/// @brief Message notification class
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_02002
/// @trace_id_ad=AD_SM_08102
/// @trace_id_dd=DD_SM_08373
/// @needwork = ad
/// @endcode
class FGStateNotifyMsg : public FGStateMsg
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00421
    /// @trace_id_dd=DD_SM_00437
    /// @needwork = ad
    /// @endcode
    FGStateNotifyMsg() = default;

    /// @brief Constructor function
    /// @param state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00422
    /// @trace_id_dd=DD_SM_00438
    /// @needwork = ad
    /// @endcode
    explicit FGStateNotifyMsg(FGStateInternalType state) noexcept
        : FGStateMsg{MsgType::kFGStateNotify}, fgState_{std::move(state)}
    {
        // First calculate the length
        SetSize(0U);
        // Get len and type
        std::size_t len{GetSize()};
        MsgType const type{GetType()};
        // Message type length
        _AddFixedLengthType2Len(len, type);

        _AddString2Len(len, fgState_.fgName);   // Function group
        _AddString2Len(len, fgState_.fgState);  // Function group state
        // Set len
        SetSize(len);
    }

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00423
    /// @trace_id_dd=DD_SM_00439
    /// @needwork = ad
    /// @endcode
    ~FGStateNotifyMsg() override = default;

    /// @brief deleted copy constructor function
    /// @param other The FGStateNotifyMsg instance to be copyed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00424
    /// @trace_id_dd=DD_SM_00440
    /// @needwork = ad
    /// @endcode
    FGStateNotifyMsg(FGStateNotifyMsg const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The FGStateNotifyMsg instance to be copyed
    /// @return the assigned FGStateNotifyMsg instance itself
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00425
    /// @trace_id_dd=DD_SM_00441
    /// @needwork = ad
    /// @endcode
    FGStateNotifyMsg &operator=(FGStateNotifyMsg const &other) = delete;

    /// @brief deleted move constructor function
    /// @param other The FGStateNotifyMsg instance to be moved
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00426
    /// @trace_id_dd=DD_SM_00442
    /// @needwork = ad
    /// @endcode
    FGStateNotifyMsg(FGStateNotifyMsg &&other) = delete;

    /// @brief deleted move assignment function
    /// @param other The FGStateNotifyMsg instance to be moved
    /// @return the assigned FGStateNotifyMsg instance itself
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00427
    /// @trace_id_dd=DD_SM_00443
    /// @needwork = ad
    /// @endcode
    FGStateNotifyMsg &operator=(FGStateNotifyMsg &&other) = delete;

    /// @brief Serialize
    /// @param p Serialization target address
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00428
    /// @trace_id_dd=DD_SM_00444
    /// @needwork = ad
    /// @endcode
    void Serialize(uint8_t *p) noexcept final
    {
        // Get type
        MsgType const type{GetType()};
        // Message type
        _AppendFixedLengthType(p, type);

        _AppendString(p, fgState_.fgName);   // Function group
        _AppendString(p, fgState_.fgState);  // Function group state
    }

    /// @brief Deserialize
    /// @param data Deserialization source address
    /// @param len Length
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00429
    /// @trace_id_dd=DD_SM_00445
    /// @needwork = ad
    /// @endcode
    void Deserialize(uint8_t *const data, std::size_t const &len) noexcept final
    {
        uint8_t *p{data};

        std::ignore = len;
        assert(len > 0U);

        // Get type
        MsgType type{GetType()};
        // Message type
        _RecoverFixedLengthType(p, type);
        // Set type
        SetType(type);

        _RecoverString(p, fgState_.fgName);   // Function group
        _RecoverString(p, fgState_.fgState);  // Function group state
    }

    /// @brief  qac requires member variables to be private only
    /// @return fgState_ Member variable fgState_
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00430
    /// @trace_id_dd=DD_SM_00446
    /// @needwork = ad
    /// @endcode
    FGStateInternalType const &GetFgState() const noexcept { return fgState_; }

    /// @brief  qac requires member variables to be private only
    /// @param  fgState Parameter fgState
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00431
    /// @trace_id_dd=DD_SM_00447
    /// @needwork = ad
    /// @endcode
    void SetFgState(FGStateInternalType const &fgState) noexcept { fgState_ = fgState; }

private:
    /// @brief Group state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08374
    /// @needwork = dda
    /// @endcode
    FGStateInternalType fgState_;
};

}  // namespace fg_state_ipc
}  // namespace sm
}  // namespace ara

#endif  // ARA_SM_INTERNAL_MSG_TYPE_H_
