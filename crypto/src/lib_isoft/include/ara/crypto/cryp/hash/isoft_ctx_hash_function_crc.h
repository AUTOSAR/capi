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
/// @file       isoft_ctx_hash_function_crc.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Hash function interface: CRC.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Hash Function
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01020
/// @unit_name=PCtxHashFunctionCrc
/// @unit_description=Hash Context Based on CRC
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_HASH_FUNCTION_CRC_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_HASH_FUNCTION_CRC_H_

#include "ara/crypto/cryp/hash/isoft_ctx_hash_function.h"

namespace isoft {
namespace crc {
/// @brief Type pre-definition within isoft::crc
class BufferView;
}  // namespace crc
}  // namespace isoft

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Puhua hash interface: CRC.
///         Base class for all Puhua Hash derived classes. Direct use of this class is not allowed; derived classes must be used.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00145
/// @trace_id_dd=DD_CRYPTO_00788
/// @needwork = ad
/// @endcode
class PCtxHashFunctionCrc : public PCtxHashFunction
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06245
    /// @needwork = dda
    /// @endcode
    using PCtxHashFunction::PCtxHashFunction;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00789
    /// @needwork = dda
    /// @endcode
    ~PCtxHashFunctionCrc() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00790
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc(PCtxHashFunctionCrc &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00791
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc &operator=(PCtxHashFunctionCrc &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00792
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc &operator=(PCtxHashFunctionCrc const &other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00793
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc(PCtxHashFunctionCrc const &other) = delete;

public:
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitiveId::Uptr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00794
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;

public:
    /// @brief Gets the DigestService instance.
    /// @brief Get DigestService instance.
    /// @name   GetDigestService
    /// @returns DigestService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00796
    /// @needwork = dda
    /// @endcode
    DigestService::Uptr GetDigestService() const noexcept override;

protected:
    /// @brief Checks if the specific hash function supports IV; CRC series do not require IV.
    /// @name   SupportIv
    /// @returns true if support iv false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00797
    /// @needwork = dda
    /// @endcode
    bool SupportIv() const noexcept override;

public:  // Added interface of PCtxHashFunction: Provided for DigestService calls
    /// @brief Returns the length of the Hash result
    /// @name   GetHashLength
    /// @returns Length of the Hash result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00798
    /// @needwork = dda
    /// @endcode
    uint32_t GetHashLength() const noexcept override = 0;
    /// @brief Gets the maximum IV length corresponding to the specific algorithm; CRC series do not require IV, so length is set to -1.
    /// @name   GetIvMaxLength
    /// @returns Corresponding maximum IV length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00799
    /// @needwork = dda
    /// @endcode
    uint32_t GetIvMaxLength() const noexcept override;

protected:  // Added interface of PCtxHashFunction
    /// @brief Get HASH result
    /// @name   GetHashResult
    /// @returns Pointer to hash result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00800
    /// @needwork = dda
    /// @endcode
    uint8_t const *GetHashResult() const noexcept override = 0;
    /// @brief Execute initialization logic via initialization vector
    /// @name   DoInitByIV
    /// @param piv Pointer to initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00801
    /// @needwork = dda
    /// @endcode
    void DoInitByIV(uint8_t const *piv) noexcept override;
    /// @brief Execute initialization logic
    /// @name   DoInit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00802
    /// @needwork = dda
    /// @endcode
    void DoInit() noexcept override = 0;
    /// @brief Execute update logic
    /// @name   DoUpdate
    /// @param pVoidData Starting address of data
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00803
    /// @needwork = dda
    /// @endcode
    void DoUpdate(void const *pVoidData, uint32_t nDataLen) noexcept override = 0;
    /// @brief Execute finalization work
    /// @name   DoFinish
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00804
    /// @needwork = dda
    /// @endcode
    void DoFinish() noexcept override{};
};
//********************************/
/// @brief Puhua hash interface: CRC8.
///         Base class for all Puhua Hash.Crc derived classes. Direct use of this class is not allowed; derived classes must be used.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00146
/// @trace_id_dd=DD_CRYPTO_00805
/// @needwork = ad
/// @endcode
template < typename T_Data >
/// @brief CRC hash function context template class
/// @interface_level=unit
class T_PCtxHashFunctionCrc : public PCtxHashFunctionCrc
{
private:
    /// @brief CRC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00806
    /// @needwork = dda
    /// @endcode
    T_Data nCrcResult_{0U};

public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06246
    /// @needwork = dda
    /// @endcode
    using PCtxHashFunctionCrc::PCtxHashFunctionCrc;
    /// @brief Define destructor
    /// @name   ~T_PCtxHashFunctionCrc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00807
    /// @needwork = dda
    /// @endcode
    ~T_PCtxHashFunctionCrc() override = default;
    /// @brief Define copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00808
    /// @needwork = dda
    /// @endcode
    T_PCtxHashFunctionCrc &operator=(T_PCtxHashFunctionCrc const &other) const noexcept = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00809
    /// @needwork = dda
    /// @endcode
    T_PCtxHashFunctionCrc(T_PCtxHashFunctionCrc const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00810
    /// @needwork = dda
    /// @endcode
    T_PCtxHashFunctionCrc(T_PCtxHashFunctionCrc &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00811
    /// @needwork = dda
    /// @endcode
    T_PCtxHashFunctionCrc &operator=(T_PCtxHashFunctionCrc &&other) = delete;

public:  // Added interface of PCtxHashFunctionCrc: Provided for Service calls
    /// @brief Returns the length of the Hash result
    /// @name   GetHashLength
    /// @returns Length of the Hash result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00812
    /// @needwork = dda
    /// @endcode
    uint32_t GetHashLength() const noexcept override;

protected:  // Interface of PCtxHashFunctionCrc
    /// @brief Get hash result
    /// @name   GetHashResult
    /// @returns Pointer to hash result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00813
    /// @needwork = dda
    /// @endcode
    uint8_t const *GetHashResult() const noexcept override;
    /// @brief Execute initialization logic
    /// @name   DoInit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00814
    /// @needwork = dda
    /// @endcode
    void DoInit() noexcept override;
    /// @brief Execute update logic
    /// @name   DoUpdate
    /// @param pVoidData Starting address of data
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00815
    /// @needwork = dda
    /// @endcode
    void DoUpdate(void const *pVoidData, uint32_t nDataLen) noexcept override;

protected:
    /// @brief Execute CRC calculation logic
    /// @name   _DoCalculateCRC
    /// @param bufferView Data to be calculated
    /// @returns Calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00816
    /// @needwork = dda
    /// @endcode
    T_Data _DoCalculateCRC(isoft::crc::BufferView const &bufferView) noexcept;
};
//********************************/
/// @brief Puhua hash interface: CRC8.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00147
/// @trace_id_dd=DD_CRYPTO_00817
/// @needwork = ad
/// @endcode
class PCtxHashFunctionCrc8 : public T_PCtxHashFunctionCrc< uint8_t >
{
public:
    /// @brief Use base class copy construction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06247
    /// @needwork = dda
    /// @endcode
    using T_PCtxHashFunctionCrc< uint8_t >::T_PCtxHashFunctionCrc;
    /// @brief Define destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00818
    /// @needwork = dda
    /// @endcode
    ~PCtxHashFunctionCrc8() override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00819
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc8(PCtxHashFunctionCrc8 const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00820
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc8(PCtxHashFunctionCrc8 &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00821
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc8 &operator=(PCtxHashFunctionCrc8 &&other) = delete;
    /// @brief Define copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00822
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc8 &operator=(PCtxHashFunctionCrc8 const &other) const noexcept = delete;
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitiveId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00823
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
};
//***************/
/// @brief Puhua hash interface: CRC16.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00148
/// @trace_id_dd=DD_CRYPTO_00824
/// @needwork = ad
/// @endcode
class PCtxHashFunctionCrc16 : public T_PCtxHashFunctionCrc< uint16_t >
{
public:
    /// @brief Use base class copy construction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06248
    /// @needwork = dda
    /// @endcode
    using T_PCtxHashFunctionCrc< uint16_t >::T_PCtxHashFunctionCrc;
    /// @brief Define destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00825
    /// @needwork = dda
    /// @endcode
    ~PCtxHashFunctionCrc16() override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00826
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc16(PCtxHashFunctionCrc16 const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00827
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc16(PCtxHashFunctionCrc16 &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00828
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc16 &operator=(PCtxHashFunctionCrc16 &&other) = delete;
    /// @brief Define copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00829
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc16 &operator=(PCtxHashFunctionCrc16 const &other) const noexcept = delete;
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitiveId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00830
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
};
//***************/
/// @brief Puhua hash interface: CRC32.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00149
/// @trace_id_dd=DD_CRYPTO_00831
/// @needwork = ad
/// @endcode
class PCtxHashFunctionCrc32 : public T_PCtxHashFunctionCrc< uint32_t >
{
public:
    /// @brief Use base class copy construction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06249
    /// @needwork = dda
    /// @endcode
    using T_PCtxHashFunctionCrc< uint32_t >::T_PCtxHashFunctionCrc;
    /// @brief Define destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00832
    /// @needwork = dda
    /// @endcode
    ~PCtxHashFunctionCrc32() override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00833
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc32(PCtxHashFunctionCrc32 const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00834
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc32(PCtxHashFunctionCrc32 &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00835
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc32 &operator=(PCtxHashFunctionCrc32 &&other) = delete;
    /// @brief Define copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00836
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc32 &operator=(PCtxHashFunctionCrc32 const &other) const noexcept = delete;
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitiveId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00837
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
};
//***************/
/// @brief Puhua hash interface: CRC64.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00150
/// @trace_id_dd=DD_CRYPTO_00838
/// @needwork = ad
/// @endcode
class PCtxHashFunctionCrc64 : public T_PCtxHashFunctionCrc< uint64_t >
{
public:
    /// @brief Use base class copy construction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06250
    /// @needwork = dda
    /// @endcode
    using T_PCtxHashFunctionCrc< uint64_t >::T_PCtxHashFunctionCrc;
    /// @brief Define destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00839
    /// @needwork = dda
    /// @endcode
    ~PCtxHashFunctionCrc64() override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00840
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc64(PCtxHashFunctionCrc64 const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00841
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc64(PCtxHashFunctionCrc64 &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00842
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc64 &operator=(PCtxHashFunctionCrc64 &&other) = delete;
    /// @brief Define copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00843
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc64 &operator=(PCtxHashFunctionCrc64 const &other) const noexcept = delete;
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitiveId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00844
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_HASH_FUNCTION_CRC_H_
