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
/// @file       idsm_sender.h
/// @brief      Remote transmission related classes
/// @details
/// @date       2023-01-13
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/qualified security event storage
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0011
/// @unit_description=Remote transmission related classes
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_SENDER_H
#define ARA_IDSM_SENDER_H
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <functional>
#include <memory>

#include "ara/idsm/internal/event.h"
#include "event/idsm_event_memory.h"
namespace ara {
namespace idsm {
/// @brief Data structure for data transmission
/// @code{.isoft}
/// @unit_name=TransData
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00258
/// @trace_id_dd=DD_IDSM_00753
/// @needwork = ad
/// @endcode
class TransData
{
public:
    /// @brief Constructor
    /// @param dataBytes Serialized byte stream of qualified security event
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00754
    /// @needwork = dda
    /// @endcode
    explicit TransData(BytesVec dataBytes) : transData_{std::move(dataBytes)} {}
    /// @brief Constructor
    /// @param dataBytes Serialized byte stream of qualified security event
    /// @param len Length of context data of qualified security event
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00755
    /// @needwork = dda
    /// @endcode
    TransData(BytesVec dataBytes, uint64_t const len) : transData_{std::move(dataBytes)}, ctxDataSize_{len} {}
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00756
    /// @needwork = dda
    /// @endcode
    virtual ~TransData() noexcept { _destroy(); }
    /// @brief Less-than operator overloading
    /// @param lhs Right operand of the less-than operator
    /// @param rhs Left operand of the less-than operator
    /// @return Returns true if less, otherwise returns false
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00757
    /// @needwork = dda
    /// @endcode
    friend bool operator<(TransData const& lhs, TransData const& rhs) noexcept
    {
        return lhs.transData_ < rhs.transData_;
    }
    /// @brief Get pointer to serialized byte stream data
    /// @return Pointer to serialized byte stream data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00758
    /// @needwork = dda
    /// @endcode
    std::uint8_t const* Data() const noexcept { return transData_.data(); }
    /// @brief Return length of serialized byte stream data
    /// @return Length of serialized byte stream data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00759
    /// @needwork = dda
    /// @endcode
    size_t Size() const noexcept { return transData_.size(); }

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00760
    /// @needwork = dda
    /// @endcode
    TransData() = default;
    /// @brief Copy constructor
    /// @param transData Object to be copied
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00761
    /// @needwork = dda
    /// @endcode
    TransData(TransData const& other) = default;
    /// @brief Move constructor
    /// @param transData Object to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00762
    /// @needwork = dda
    /// @endcode
    TransData(TransData&& other) = default;
    /// @brief Copy assignment operator
    /// @param transData Object to be copied
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00763
    /// @needwork = dda
    /// @endcode
    TransData& operator=(TransData const& other) = default;
    /// @brief Move assignment operator
    /// @param transData Object to be moved
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00764
    /// @needwork = dda
    /// @endcode
    TransData& operator=(TransData&& other) = default;

private:
    /// @brief Data transmission completed, release memory occupied by the security event
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00765
    /// @needwork = dda
    /// @endcode
    void _destroy() const { EventMemPool::GetInstance()->ReleaseMem(EVENT_MEM_SIZE, ctxDataSize_); }

private:
    /// @brief Security event byte stream data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00766
    /// @needwork = dda
    /// @endcode
    BytesVec transData_{};
    /// @brief Size of security event context data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00767
    /// @needwork = dda
    /// @endcode
    uint64_t ctxDataSize_{0};
};

/// @brief Type redefinition: define the smart pointer type corresponding to the data structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00259
/// @trace_id_dd=DD_IDSM_00768
/// @needwork = ad
/// @endcode
using TransDataPtr = std::shared_ptr< TransData >;
/// @brief Type redefinition: callback function type for IDSM asynchronous sending of qualified security events
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00260
/// @trace_id_dd=DD_IDSM_00769
/// @needwork = ad
/// @endcode
using SendCallBack = std::function< void(TransDataPtr const, bool const) >;
/// @brief Classes required to implement data transmission are as follows
/// @code{.isoft}
/// @unit_name=TransData
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00261
/// @trace_id_dd=DD_IDSM_00770
/// @needwork = ad
/// @endcode
class Message final
{
public:
    /// @brief Get specific data for transmission
    /// @return Specific data for transmission
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00771
    /// @needwork = dda
    /// @endcode
    TransDataPtr GetData() const noexcept;
    /// @brief Custom Message object comparison function
    /// @param l Left operand of the less-than operator
    /// @param r Right operand of the less-than operator
    /// @return Comparison result
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00772
    /// @needwork = dda
    /// @endcode
    friend bool operator<(Message const& l, Message const& r) noexcept { return l.transData < r.transData; }

public:
    /// @name transData
    /// @brief Data content to be transmitted
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00773
    /// @needwork = dda
    /// @endcode
    TransDataPtr transData;
    /// @name userData
    /// @brief User-defined data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00774
    /// @needwork = dda
    /// @endcode
    void* userData;
};
/// @brief Default retry count when IDSR transmission fails
// static uint32_t const retryTimesConst{3U};
/// @brief Data sender base class
/// @code{.isoft}
/// @unit_name=IdsmSender
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00262
/// @trace_id_dd=DD_IDSM_00775
/// @needwork = ad
/// @endcode
class IdsmSender
{
protected:
    /// @brief Parameterless constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00776
    /// @needwork = dda
    /// @endcode
    IdsmSender() : IdsmSender{ara::core::String{""}, 0U} {}
    /// @brief Parameterized constructor
    /// @param ip IP address of the IDSR service
    /// @param p Port number of the IDSR service
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00777
    /// @needwork = dda
    /// @endcode
    IdsmSender(ara::core::String ip, uint16_t const p) : ipAddr_{std::move(ip)}, port_{p} {}
    /// @brief Copy constructor
    /// @param sender Object to be copied
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00778
    /// @needwork = dda
    /// @endcode
    IdsmSender(IdsmSender const& sender) = default;
    /// @brief Move constructor
    /// @param sender Object to be moved
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00779
    /// @needwork = dda
    /// @endcode
    IdsmSender(IdsmSender&& sender) = default;
    /// @brief Copy assignment operator
    /// @param sender Object to be copied
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00780
    /// @needwork = dda
    /// @endcode
    IdsmSender& operator=(IdsmSender const& sender) = default;
    /// @brief Move assignment operator
    /// @param sender Object to be moved
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00781
    /// @needwork = dda
    /// @endcode
    IdsmSender& operator=(IdsmSender&& sender) = default;

public:
    /// @brief Send data to peer
    /// @param data Data to transmit
    /// @param highPriority Callback function called upon transmission completion
    /// @return Number of bytes sent, returns -1 on error
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00782
    /// @needwork = dda
    /// @endcode
    virtual int64_t Send(Message const& data, bool highPriority = false) = 0;
    /// @brief Get IP address of the IDSR service
    /// @return IP address of the IDSR service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00783
    /// @needwork = dda
    /// @endcode
    ara::core::String GetIpAddr() const noexcept { return ipAddr_; }
    /// @brief Get port number of the IDSR service
    /// @return Port number of the IDSR service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00784
    /// @needwork = dda
    /// @endcode
    uint16_t GetPort() const noexcept { return port_; }

public:
    /// @brief Destructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00785
    /// @needwork = dda
    /// @endcode
    virtual ~IdsmSender() = default;

private:
    /// @name ipAddr
    /// @brief IP address of the IDSR service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00786
    /// @needwork = dda
    /// @endcode
    ara::core::String ipAddr_;
    /// @name port
    /// @brief Port of the IDSR service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00787
    /// @needwork = dda
    /// @endcode
    uint16_t port_;
};

}  // namespace idsm

}  // namespace ara
#endif
