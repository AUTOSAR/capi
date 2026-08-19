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
/// @file       ids_proto_decode.h
/// @brief      IDS protocol deserialization implementation
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
/// @unit_name=IdsmDeserialize
/// @unit_description=IDS protocol deserialization implementation
/// @endcode
///
/// ================================================================

#ifndef ARA_IDS_PROTO_DECODE_H_
#define ARA_IDS_PROTO_DECODE_H_
#include <iostream>
#include <memory>

#include "ara/core/string.h"
#include "ara/idsm/internal/event.h"

namespace ara {
namespace idsm {
/// @brief IDS protocol header length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00126
/// @trace_id_dd=DD_IDSM_00259
/// @needwork = dd
/// @endcode
uint32_t const kIdsProtoHeadSize{8U};
/// @brief User-defined digital digest function
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00124
/// @trace_id_dd=DD_IDSM_00260
/// @needwork = ad
/// @endcode
using DigestFunc = std::function< BytesVec(uint8_t const*, size_t) >;
/// @brief User-defined decryption function
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00125
/// @trace_id_dd=DD_IDSM_00261
/// @needwork = ad
/// @endcode
using DecryptFunc = std::function< BytesVec(uint8_t const*, size_t) >;
/// @brief Forward declaration
class IdsmSignVerify;
/// @brief Event deserialization class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00126
/// @trace_id_dd=DD_IDSM_00262
/// @needwork = ad
/// @endcode
class IdsmDeserialize
{
public:
    /// @brief Define char_8 type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00263
    /// @needwork = dda
    /// @endcode
    using char_8 = char;  // NOLINT
    /// @brief Parameterless constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00127
    /// @trace_id_dd=DD_IDSM_00264
    /// @needwork = ad
    /// @endcode
    IdsmDeserialize() : IdsmDeserialize{0U, 0U, 0U, 0U, 0U, 0U, std::vector< uint8_t >{}, "", ""} {};
    /// @brief Constructor
    /// @param slotName Name of the key slot used for signing
    /// @param algName Name of the signature algorithm
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00128
    /// @trace_id_dd=DD_IDSM_00265
    /// @needwork = ad
    /// @endcode
    IdsmDeserialize(ara::core::String const& slotName, ara::core::String const& algName)
        : IdsmDeserialize{0U, 0U, 0U, 0U, 0U, 0U, std::vector< uint8_t >{}, slotName, algName}
    {
    }

public:
    /// @brief Deserialize byte stream into a structured event
    /// @param data Byte stream data
    /// @param[in/out] dataSize Byte stream data size
    /// @return Structured event
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00129
    /// @trace_id_dd=DD_IDSM_00266
    /// @needwork = ad
    /// @endcode
    std::vector< EventPtr > Decode(uint8_t const* data, uint32_t dataSize);
    /// @brief Register user-defined digest function and decryption function
    /// @param digest User-defined digest function, the current digest algorithm must be MD5
    /// @param decrypt User-defined decryption function
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00132
    /// @trace_id_dd=DD_IDSM_00269
    /// @needwork = ad
    /// @endcode
    void Register(DigestFunc const& digest, DecryptFunc const& decrypt);

private:
    /// @brief Parameterized constructor
    /// @param version Version number
    /// @param header Protocol header
    /// @param id IDSM instance ID
    /// @param rvd Reserved field
    /// @param size Protocol header length
    /// @param len Protocol data length
    /// @param v Version number
    /// @param slot Key slot name
    /// @param alg Decryption algorithm
    /// @param digest User-defined digest algorithm
    /// @param decrypt User-defined decryption algorithm
    /// @throws Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00270
    /// @needwork = dda
    /// @endcode
    IdsmDeserialize(uint8_t const version,
                    uint8_t const header,
                    uint16_t const id,
                    uint8_t const rvd,
                    uint16_t const size,
                    uint32_t const len,
                    std::vector< uint8_t > v,
                    ara::core::String const& slot,
                    ara::core::String const& alg,
                    DigestFunc digest   = nullptr,
                    DecryptFunc decrypt = nullptr);

private:
    /// @brief Process the protocol header
    /// @param data Buffer storing protocol header data
    /// @param size Buffer length
    /// @return 0 if protocol header processing fails, non-zero for protocol header length
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00271
    /// @needwork = dda
    /// @endcode
    uint32_t _processProtoHead(uint8_t const* const data, uint32_t const size);
    /// @brief Deserialize Event Frame from byte stream
    /// @param pos Event frame deserialization data starts from pos. When the function ends, pos moves to the next byte after the end of the event frame serialization data
    /// @param event Structured event
    /// @exception Stack overflow exception
    /// @param event Structured event
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00272
    /// @needwork = dda
    /// @endcode
    void _decodeEventFrame(size_t& pos, EventPtr& event) noexcept;
    /// @brief Deserialize timestamp from byte stream
    /// @param pos Timestamp serialization data starts from pos. When the function ends, pos moves to the next byte after the end of the timestamp serialization data
    /// @param event Structured event
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00273
    /// @needwork = dda
    /// @endcode
    void _decodeTimestamp(size_t& pos, EventPtr& event);
    /// @brief Deserialize context data from byte stream
    /// @param pos Context data serialization data starts from pos. When the function ends, pos moves to the next byte after the end of the context data serialization data
    /// @param event Structured event
    /// @param event Structured event
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00274
    /// @needwork = dda
    /// @endcode
    void _decodeContextData(size_t& pos, EventPtr& event);
    /// @brief Deserialize signature from byte stream
    /// @param pos Signature serialization data starts from pos. When the function ends, pos moves to the next byte after the end of the signature serialization data
    /// @param event Structured event
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00275
    /// @needwork = dda
    /// @endcode
    void _decodeSignature(size_t& pos, EventPtr& event);
    /// @brief Signature verification
    /// @param pos Data length
    /// @param event The signature ciphertext to be verified is stored in the event structure
    /// @return Returns true if signature verification succeeds, otherwise returns false
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00276
    /// @needwork = dda
    /// @endcode
    bool _signVerify(size_t const& pos, EventPtr const& event);

private:
    /// @name protoVersion
    /// IDS protocol version number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00277
    /// @needwork = dda
    /// @endcode
    uint8_t protoVersion_;
    /// @name protoHeader
    /// IDS protocol header
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00278
    /// @needwork = dda
    /// @endcode
    uint8_t protoHeader_;
    /// @name idsmId
    /// IDSM instance ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00279
    /// @needwork = dda
    /// @endcode
    uint16_t idsmId_;
    /// @name reserved
    /// IDS protocol reserved field
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00280
    /// @needwork = dda
    /// @endcode
    uint8_t reserved_;
    /// @name bufSize
    /// Protocol header length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00281
    /// @needwork = dda
    /// @endcode
    uint32_t bufSize_;
    /// @name headBuf
    /// Protocol header buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00282
    /// @needwork = dda
    /// @endcode
    char_8 headBuf_[kIdsProtoHeadSize];
    /// @name msgSize
    /// Protocol data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00283
    /// @needwork = dda
    /// @endcode
    uint32_t msgSize_;
    /// @name message
    /// Protocol data buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00284
    /// @needwork = dda
    /// @endcode
    std::vector< uint8_t > message_;
    /// @brief Signature verification
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00285
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< IdsmSignVerify > signVerifier_;
    /// @brief User-defined digest function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00286
    /// @needwork = dda
    /// @endcode
    DigestFunc digest_;
    /// @brief User-defined decryption function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00287
    /// @needwork = dda
    /// @endcode
    DecryptFunc decrypt_;
};
}  // namespace idsm
}  // namespace ara
#endif