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
/// @file       ids_proto_encode.h
/// @brief      IDS protocol serialization
/// @details
/// @date       2023-01-13
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/IDS protocol
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0011
/// @unit_name=IdsmSerialize
/// @unit_description=IDS protocol serialization
/// @endcode
///
/// ================================================================

#ifndef ARA_IDS_PROTO_ENCODE_H_
#define ARA_IDS_PROTO_ENCODE_H_

#include <iostream>
#include <memory>

#include "ara/core/string.h"
#include "ara/idsm/internal/event.h"
namespace ara {
namespace idsm {
/// @brief  Forward declaration of signature class
class IdsmSignEncode;
/// @brief Event serialization class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00114
/// @trace_id_dd=DD_IDSM_00234
/// @needwork = ad
/// @endcode
class IdsmSerialize
{
public:
    /// @brief Parameterless constructor
    /// @throws Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00115
    /// @trace_id_dd=DD_IDSM_00235
    /// @needwork = ad
    /// @endcode
    IdsmSerialize() : IdsmSerialize{1U, 0U, 0U, "", ""} {}
    /// @brief Parameterized constructor
    /// @param id  IDSM instance ID
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00116
    /// @trace_id_dd=DD_IDSM_00236
    /// @needwork = ad
    /// @endcode
    explicit IdsmSerialize(uint16_t const id) : IdsmSerialize{1U, 0U, id, "", ""} {}
    /// @brief Parameterized constructor
    /// @param version Protocol version number
    /// @param id IDSM instance ID
    /// @throws Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00117
    /// @trace_id_dd=DD_IDSM_00237
    /// @needwork = ad
    /// @endcode
    IdsmSerialize(uint8_t const version, uint16_t const id) : IdsmSerialize{version, 0U, id, "", ""} {}
    /// @brief Constructor
    /// @param id IDSM instance ID
    /// @param slotName Name of the signature algorithm
    /// @param algName Name of the key slot used for signing
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00118
    /// @trace_id_dd=DD_IDSM_00238
    /// @needwork = ad
    /// @endcode
    IdsmSerialize(uint16_t const id, ara::core::String const& slotName, ara::core::String const& algName)
        : IdsmSerialize{1U, 0U, id, slotName, algName}
    {
    }
    /// @brief Copy constructor
    /// @param serialize Object to be copied
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00239
    /// @needwork = dda
    /// @endcode
    IdsmSerialize(IdsmSerialize const& serialize) noexcept = delete;
    /// @brief Move constructor
    /// @param serialize Object to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00240
    /// @needwork = dda
    /// @endcode
    IdsmSerialize(IdsmSerialize&& serialize) noexcept = default;
    /// @brief Copy assignment operator
    /// @param serialize Object to be copied
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00241
    /// @needwork = dda
    /// @endcode
    IdsmSerialize& operator=(IdsmSerialize const& serialize) noexcept = delete;
    /// @brief Move assignment operator
    /// @param serialize Object to be moved
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00242
    /// @needwork = dda
    /// @endcode
    IdsmSerialize& operator=(IdsmSerialize&& serialize) noexcept = default;
    /// @brief Destructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00243
    /// @needwork = dda
    /// @endcode
    virtual ~IdsmSerialize() = default;

private:
    /// @brief Parameterized constructor
    /// @param version Version number
    /// @param header Protocol header
    /// @param id IDSM instance ID
    /// @param rvd Reserved field
    /// @param slot Key slot name
    /// @param alg Encryption algorithm name
    /// @throws Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00244
    /// @needwork = dda
    /// @endcode
    IdsmSerialize(uint8_t const version,
                  uint8_t const header,
                  uint16_t const id,
                  ara::core::String const& slot,
                  ara::core::String const& alg);

public:
    /// @brief Set IDS protocol version number
    /// @param version IDS protocol version number
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00119
    /// @trace_id_dd=DD_IDSM_00245
    /// @needwork = ad
    /// @endcode
    void SetProtoVersion(uint8_t const version) noexcept { this->protoVersion_ = version; }

    /// @brief Set IDSM instance ID
    /// @param id IDSM instance ID
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00120
    /// @trace_id_dd=DD_IDSM_00246
    /// @needwork = ad
    /// @endcode
    void SetIdsmInstanceId(uint16_t const id) noexcept { this->idsmId_ = id; }
    /// @brief Serialize qualified events using the IDS protocol
    /// @param event Event to be serialized
    /// @return Byte stream after event serialization
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00123
    /// @trace_id_dd=DD_IDSM_00249
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< BytesVec > Encode(EventPtr const& event);

private:
    /// @brief The information of a qualified event can be divided into fixed parts and optional parts (timestamp, context data, signature). Serialize the fixed parts.
    /// @param event Qualified event
    /// @param message Byte stream after adding serialized fixed parts of the event
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00250
    /// @needwork = dda
    /// @endcode
    void _encodeEventFrame(EventPtr const& event, std::shared_ptr< BytesVec >& message) const;
    /// @brief Serialize timestamp
    /// @param event Qualified event
    /// @param message Byte stream after adding serialized timestamp
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00251
    /// @needwork = dda
    /// @endcode
    void _encodeTimeStamp(EventPtr const& event, std::shared_ptr< BytesVec >& message) const;
    /// @brief Serialize context data
    /// @param ctxData Qualified event
    /// @param message Byte stream after adding serialized context data
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00252
    /// @needwork = dda
    /// @endcode
    void _encodeContextData(ContextDataType const& ctxData, std::shared_ptr< BytesVec >& message) const;
    /// @brief Serialize signature
    /// @param event Qualified event
    /// @param message Byte stream after adding serialized optional parts
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00253
    /// @needwork = dda
    /// @endcode
    void _encodeSignature(std::shared_ptr< BytesVec >& message);

private:
    /// @name protoVersion
    /// IDS protocol version number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00254
    /// @needwork = dda
    /// @endcode
    uint8_t protoVersion_;
    /// @name protoHeader
    /// IDS protocol header
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00255
    /// @needwork = dda
    /// @endcode
    uint8_t protoHeader_;
    /// @name idsmId
    /// IDSM instance ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00256
    /// @needwork = dda
    /// @endcode
    uint16_t idsmId_;
    /// @code{.isoft}
    /// @reserved
    /// IDS protocol reserved field
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00257
    /// @needwork = dda
    /// @endcode
    //uint8_t reserved;
    /// @brief Signature class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00258
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< IdsmSignEncode > idsmSignEncoder_;
};
}  // namespace idsm
}  // namespace ara
#endif