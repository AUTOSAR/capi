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
/// @file       isoft_crypto_primitive_id.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Crypto Primitive
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PAlgId_Base
/// @unit_description=Crypto Primitive ID Encapsulation
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"

#include <cstring>

#include "ara/crypto/common/isoft_data_type.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Initialize "Initialization Vector"
/// @param pInitData Initialization data
/// @param nDataLen Data length
void InitVector(uint8_t* const& pInitData, int32_t const nDataLen) noexcept
{
    for (int32_t i{0}; i < (nDataLen / kInt_8); ++i) {
        std::ignore = std::memcpy(pInitData + i * kInt_8, "isoftssl", kInt_8U);
    }
}
//********************************/
/// @brief Parameterized constructor
/// @param nInitLen Initial storage BUFF length
PAlgId_Base::PAlgId_Base(uint32_t const nInitLen) noexcept
    : CryptoPrimitiveId{}  // NOLINT
    , stAlgName_{}         // NOLINT
{
    stAlgName_.reserve(static_cast< std::size_t >(nInitLen));
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Base::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kUnDefine);
}
/// @brief Check if ID is the same
/// @param nAlgId Crypto primitive ID
/// @return true is suitable false otherwise
bool PAlgId_Base::IsMinePrimitiveId(AlgId nAlgId) const noexcept { return nAlgId == GetPrimitiveId(); }
/// @brief Add to crypto primitive: key
void PAlgId_Base::AddKey() noexcept
{
    UpdateAlgName("k");
    UpdateAlgName("e");
    UpdateAlgName("y");
}
/// @brief Add to crypto primitive: des
void PAlgId_Base::AddDes() noexcept
{
    UpdateAlgName("d");
    UpdateAlgName("e");
    UpdateAlgName("s");
}
/// @brief Add to crypto primitive: aes
void PAlgId_Base::AddAes() noexcept
{
    UpdateAlgName("a");
    UpdateAlgName("e");
    UpdateAlgName("s");
}
/// @brief Add to crypto primitive: ecb
void PAlgId_Base::AddEcb() noexcept
{
    UpdateAlgName("e");
    UpdateAlgName("c");
    UpdateAlgName("b");
}
/// @brief Add to crypto primitive: cbc
void PAlgId_Base::AddCbc() noexcept
{
    UpdateAlgName("c");
    UpdateAlgName("b");
    UpdateAlgName("c");
}
/// @brief Add to crypto primitive: ctr
void PAlgId_Base::AddCtr() noexcept
{
    UpdateAlgName("c");
    UpdateAlgName("t");
    UpdateAlgName("r");
}
/// @brief Add to crypto primitive: gcm
void PAlgId_Base::AddGcm() noexcept
{
    UpdateAlgName("g");
    UpdateAlgName("c");
    UpdateAlgName("m");
}
/// @brief Add to crypto primitive: hmac
void PAlgId_Base::AddHmac() noexcept
{
    UpdateAlgName("h");
    UpdateAlgName("m");
    UpdateAlgName("a");
    UpdateAlgName("c");
}
/// @brief Add to crypto primitive: cfb
void PAlgId_Base::AddCfb() noexcept
{
    UpdateAlgName("c");
    UpdateAlgName("f");
    UpdateAlgName("b");
}
/// @brief Add to crypto primitive: ofb
void PAlgId_Base::AddOfb() noexcept
{
    UpdateAlgName("o");
    UpdateAlgName("f");
    UpdateAlgName("b");
}
/// @brief Add to crypto primitive: rsa
void PAlgId_Base::AddRsa() noexcept
{
    UpdateAlgName("r");
    UpdateAlgName("s");
    UpdateAlgName("a");
}
/// @brief Add to crypto primitive: pss
void PAlgId_Base::AddPss() noexcept
{
    UpdateAlgName("p");
    UpdateAlgName("s");
    UpdateAlgName("s");
}
/// @brief Add to crypto primitive: crc
void PAlgId_Base::AddCrc() noexcept
{
    UpdateAlgName("c");
    UpdateAlgName("r");
    UpdateAlgName("c");
}
/// @brief Add to crypto primitive: mac
void PAlgId_Base::AddMac() noexcept
{
    UpdateAlgName("m");
    UpdateAlgName("a");
    UpdateAlgName("c");
}
/// @brief Add to crypto primitive: hash
void PAlgId_Base::AddHash() noexcept
{
    UpdateAlgName("h");
    UpdateAlgName("a");
    UpdateAlgName("s");
    UpdateAlgName("h");
}
/// @brief Add to crypto primitive: sha
void PAlgId_Base::AddSha() noexcept
{
    UpdateAlgName("s");
    UpdateAlgName("h");
    UpdateAlgName("a");
}
/// @brief Add to crypto primitive: kdf
void PAlgId_Base::AddKdf() noexcept
{
    UpdateAlgName("k");
    UpdateAlgName("d");
    UpdateAlgName("f");
}
/// @brief Add to crypto primitive: md5
void PAlgId_Base::AddMd5() noexcept
{
    UpdateAlgName("m");
    UpdateAlgName("d");
    UpdateAlgName("5");
}
/// @brief Add to crypto primitive: pad
void PAlgId_Base::AddPad() noexcept
{
    UpdateAlgName("p");
    UpdateAlgName("a");
    UpdateAlgName("d");
}
/// @brief Add to crypto primitive: ipc
void PAlgId_Base::AddIpc() noexcept
{
    UpdateAlgName("i");
    UpdateAlgName("p");
    UpdateAlgName("c");
}
/// @brief Add to crypto primitive: ran
void PAlgId_Base::AddRan() noexcept
{
    UpdateAlgName("r");
    UpdateAlgName("a");
    UpdateAlgName("n");
}
/// @brief Add to crypto primitive: dom
void PAlgId_Base::AddDom() noexcept
{
    UpdateAlgName("d");
    UpdateAlgName("o");
    UpdateAlgName("m");
}
/// @brief Add to crypto primitive: ede
void PAlgId_Base::AddEde() noexcept
{
    UpdateAlgName("e");
    UpdateAlgName("d");
    UpdateAlgName("e");
}
/// @brief Add to crypto primitive: 64
void PAlgId_Base::Add64() noexcept
{
    UpdateAlgName("6");
    UpdateAlgName("4");
}
/// @brief Add to crypto primitive: 128
void PAlgId_Base::Add128() noexcept
{
    UpdateAlgName("1");
    UpdateAlgName("2");
    UpdateAlgName("8");
}
/// @brief Add to crypto primitive: 192
void PAlgId_Base::Add192() noexcept
{
    UpdateAlgName("1");
    UpdateAlgName("9");
    UpdateAlgName("2");
}
/// @brief Add to crypto primitive: 224
void PAlgId_Base::Add224() noexcept
{
    UpdateAlgName("2");
    UpdateAlgName("2");
    UpdateAlgName("4");
}
/// @brief Add to crypto primitive: 256
void PAlgId_Base::Add256() noexcept
{
    UpdateAlgName("2");
    UpdateAlgName("5");
    UpdateAlgName("6");
}
/// @brief Add to crypto primitive: 384
void PAlgId_Base::Add384() noexcept
{
    UpdateAlgName("3");
    UpdateAlgName("8");
    UpdateAlgName("4");
}
/// @brief Add to crypto primitive: 512
void PAlgId_Base::Add512() noexcept
{
    UpdateAlgName("5");
    UpdateAlgName("1");
    UpdateAlgName("2");
}
/// @brief Add to crypto primitive: 1024
void PAlgId_Base::Add1024() noexcept
{
    UpdateAlgName("1");
    UpdateAlgName("0");
    UpdateAlgName("2");
    UpdateAlgName("4");
}
/// @brief Add to crypto primitive: 2048
void PAlgId_Base::Add2048() noexcept
{
    UpdateAlgName("2");
    UpdateAlgName("0");
    UpdateAlgName("4");
    UpdateAlgName("8");
}
/// @brief Add to crypto primitive: 4096
void PAlgId_Base::Add4096() noexcept
{
    UpdateAlgName("4");
    UpdateAlgName("0");
    UpdateAlgName("9");
    UpdateAlgName("6");
}
/// @brief Add to crypto primitive: 8192
void PAlgId_Base::Add8192() noexcept
{
    UpdateAlgName("8");
    UpdateAlgName("1");
    UpdateAlgName("9");
    UpdateAlgName("2");
}
//********************************/
/// @brief Constructor
PAlgId_CertSlot::PAlgId_CertSlot() noexcept : PAlgId_Base{kInt_20U}
{
    UpdateAlgName("c");
    UpdateAlgName("e");
    UpdateAlgName("r");
    UpdateAlgName("t");
    UpdateAlgName("s");
    UpdateAlgName("l");
    UpdateAlgName("o");
    UpdateAlgName("t");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_CertSlot::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kCertSlot);
}
//********************************/ //CRC
/// @brief Constructor
PAlgId_CrcBase::PAlgId_CrcBase() noexcept : PAlgId_Base{kInt_20U} { AddCrc(); }
//***************/
/// @brief Constructor
PAlgId_Crc8::PAlgId_Crc8() noexcept : PAlgId_CrcBase{} { UpdateAlgName("8"); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Crc8::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashCrc8);
}
//***************/
/// @brief Constructor
PAlgId_Crc16::PAlgId_Crc16() noexcept : PAlgId_CrcBase{}
{
    UpdateAlgName("1");
    UpdateAlgName("6");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Crc16::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashCrc16);
}
//***************/
/// @brief Constructor
PAlgId_Crc32::PAlgId_Crc32() noexcept : PAlgId_CrcBase{}
{
    UpdateAlgName("3");
    UpdateAlgName("2");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Crc32::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashCrc32);
}
//***************/
/// @brief Constructor
PAlgId_Crc64::PAlgId_Crc64() noexcept : PAlgId_CrcBase{}
{
    UpdateAlgName("6");
    UpdateAlgName("4");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Crc64::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashCrc64);
}
//********************************/ //Md5
/// @brief Constructor
PAlgId_Md5::PAlgId_Md5() noexcept : PAlgId_Base{kInt_20U} { AddMd5(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Md5::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashMd5);
}
//********************************/ //SHA
/// @brief Constructor
PAlgId_ShaBase::PAlgId_ShaBase() noexcept : PAlgId_Base{kInt_8U} { AddSha(); }
//***************/
/// @brief Constructor
PAlgId_Sha1::PAlgId_Sha1() noexcept : PAlgId_ShaBase{} { UpdateAlgName("1"); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Sha1::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha1);
}
//***************/
/// @brief Constructor
PAlgId_Sha2::PAlgId_Sha2() noexcept : PAlgId_ShaBase{}
{
    UpdateAlgName("2");
    UpdateAlgName("-");
}
/// @brief Constructor
PAlgId_Sha2_224::PAlgId_Sha2_224() noexcept : PAlgId_Sha2{} { Add224(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Sha2_224::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_224);
}
//***************/
/// @brief Constructor
PAlgId_Sha2_256::PAlgId_Sha2_256() noexcept : PAlgId_Sha2{} { Add256(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Sha2_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_256);
}
//***************/
/// @brief Constructor
PAlgId_Sha2_384::PAlgId_Sha2_384() noexcept : PAlgId_Sha2{} { Add384(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Sha2_384::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_384);
}
//***************/
/// @brief Constructor
PAlgId_Sha2_512::PAlgId_Sha2_512() noexcept : PAlgId_Sha2{} { Add512(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Sha2_512::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_512);
}
//***************/
/// @brief Constructor
PAlgId_Sha1_File::PAlgId_Sha1_File() noexcept : PAlgId_Sha1{}
{
    UpdateAlgName("-");
    UpdateAlgName("f");
    UpdateAlgName("i");
    UpdateAlgName("l");
    UpdateAlgName("e");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Sha1_File::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha1_File);
}
//***************/
/// @brief Constructor
PAlgId_Sha2_224_File::PAlgId_Sha2_224_File() noexcept : PAlgId_Sha2_224{}
{
    UpdateAlgName("-");
    UpdateAlgName("f");
    UpdateAlgName("i");
    UpdateAlgName("l");
    UpdateAlgName("e");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Sha2_224_File::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_224_File);
}
//***************/
/// @brief Constructor
PAlgId_Sha2_256_File::PAlgId_Sha2_256_File() noexcept : PAlgId_Sha2_256{}
{
    UpdateAlgName("-");
    UpdateAlgName("f");
    UpdateAlgName("i");
    UpdateAlgName("l");
    UpdateAlgName("e");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Sha2_256_File::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_256_File);
}
//********************************/ //Symmetric Encryption: DES
/// @brief Constructor
PAlgId_Symmetric_DesBase::PAlgId_Symmetric_DesBase() noexcept : PAlgId_Base{kInt_8U} { AddDes(); }
//***************/
/// @brief Constructor
PAlgId_Symmetric_DesKey::PAlgId_Symmetric_DesKey() noexcept : PAlgId_Symmetric_DesBase{} { AddKey(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_DesKey::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricDesKey);
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_3DesKey::PAlgId_Symmetric_3DesKey() noexcept : PAlgId_Symmetric_DesKey{} { SetAlgNamePre("3"); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_3DesKey::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetric3DesKey);
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_2DesKey::PAlgId_Symmetric_2DesKey() noexcept : PAlgId_Symmetric_DesKey{} { SetAlgNamePre("2"); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_2DesKey::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetric2DesKey);
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_Des_Ecb::PAlgId_Symmetric_Des_Ecb() noexcept : PAlgId_Symmetric_DesBase{} { AddEcb(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Des_Ecb::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricDesEcb);
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_Des_Cbc::PAlgId_Symmetric_Des_Cbc() noexcept : PAlgId_Symmetric_DesBase{} { AddCbc(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Des_Cbc::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricDesCbc);
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_Des_Cfb::PAlgId_Symmetric_Des_Cfb() noexcept : PAlgId_Symmetric_DesBase{} { AddCfb(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Des_Cfb::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricDesCfb);
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_Des_Cfb64::PAlgId_Symmetric_Des_Cfb64() noexcept : PAlgId_Symmetric_DesBase{}
{
    AddCfb();
    UpdateAlgName("-");
    Add64();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Des_Cfb64::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricDesCfb64);
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_Des_Ofb::PAlgId_Symmetric_Des_Ofb() noexcept : PAlgId_Symmetric_DesBase{} { AddOfb(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Des_Ofb::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricDesOfb);
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_Des_Ofb64::PAlgId_Symmetric_Des_Ofb64() noexcept : PAlgId_Symmetric_DesBase{}
{
    AddOfb();
    UpdateAlgName("-");
    Add64();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Des_Ofb64::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricDesOfb64);
}
//********************************/ //3DES
/// @brief Constructor
PAlgId_Symmetric_3DesBase::PAlgId_Symmetric_3DesBase() noexcept : PAlgId_Base{kInt_8U}
{
    UpdateAlgName("3");
    AddDes();
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_3Des_Ecb::PAlgId_Symmetric_3Des_Ecb() noexcept : PAlgId_Symmetric_3DesBase{} { AddEcb(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_3Des_Ecb::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetric3DesEcb);
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_3Des_Cbc::PAlgId_Symmetric_3Des_Cbc() noexcept : PAlgId_Symmetric_3DesBase{} { AddCbc(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_3Des_Cbc::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetric3DesCbc);
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_3Des_Cfb1::PAlgId_Symmetric_3Des_Cfb1() noexcept : PAlgId_Symmetric_3DesBase{}
{
    AddCfb();
    UpdateAlgName("-");
    UpdateAlgName("1");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_3Des_Cfb1::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetric3DesCfb1);
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_3Des_Cfb64::PAlgId_Symmetric_3Des_Cfb64() noexcept : PAlgId_Symmetric_3DesBase{}
{
    AddCfb();
    UpdateAlgName("-");
    UpdateAlgName("6");
    UpdateAlgName("4");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_3Des_Cfb64::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetric3DesCfb64);
}
//***************/
/// @brief Constructor
PAlgId_Symmetric_3Des_Ofb64::PAlgId_Symmetric_3Des_Ofb64() noexcept : PAlgId_Symmetric_3DesBase{}
{
    AddOfb();
    UpdateAlgName("-");
    UpdateAlgName("6");
    UpdateAlgName("4");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_3Des_Ofb64::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetric3DesOfb64);
}
//********************************/ //Symmetric Encryption: AES Key
/// @brief Constructor
PAlgId_Symmetric_AesBase::PAlgId_Symmetric_AesBase() noexcept : PAlgId_Base{kInt_20U} { AddAes(); }
//***************/
/// @brief Constructor
PAlgId_Symmetric_AesKey::PAlgId_Symmetric_AesKey() noexcept : PAlgId_Symmetric_AesBase{} { AddKey(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_AesKey::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesKey);
}
/// @brief Check if ID is the same
/// @param nAlgId Crypto primitive ID
/// @return
bool PAlgId_Symmetric_AesKey::IsMinePrimitiveId(AlgId nAlgId) const noexcept
{
    return (nAlgId == static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey))
           || (nAlgId == static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey128))
           || (nAlgId == static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey192))
           || (nAlgId == static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey256));
}
//***************/
/// @brief Constructor    //AES KEY for HMAC
PAlgId_Symmetric_AesKeyHMAC::PAlgId_Symmetric_AesKeyHMAC() noexcept : PAlgId_Symmetric_AesKey{} { AddHmac(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_AesKeyHMAC::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesKeyHmac);
}
/// @brief Check if ID is the same
/// @param nAlgId Crypto primitive ID
/// @return
bool PAlgId_Symmetric_AesKeyHMAC::IsMinePrimitiveId(AlgId nAlgId) const noexcept
{
    return nAlgId == static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKeyHmac);
}
/// @brief Constructor
PAlgId_Symmetric_AesKey128::PAlgId_Symmetric_AesKey128() noexcept : PAlgId_Symmetric_AesKey{}
{
    UpdateAlgName("-");
    Add128();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_AesKey128::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesKey128);
}
/// @brief Check if ID is the same
/// @name   IsMinePrimitiveId
/// @param nAlgId Crypto primitive ID
/// @returns true is suitable false otherwise
bool PAlgId_Symmetric_AesKey128::IsMinePrimitiveId(AlgId nAlgId) const noexcept
{
    return ((nAlgId == static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey128))
            || (nAlgId == static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey)));
}
/// @brief Constructor
PAlgId_Symmetric_AesKey192::PAlgId_Symmetric_AesKey192() noexcept : PAlgId_Symmetric_AesKey{}
{
    UpdateAlgName("-");
    Add192();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_AesKey192::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesKey192);
}
/// @brief Check if ID is the same
/// @name   IsMinePrimitiveId
/// @param nAlgId Crypto primitive ID
/// @returns true is suitable false otherwise
bool PAlgId_Symmetric_AesKey192::IsMinePrimitiveId(AlgId nAlgId) const noexcept
{
    return nAlgId == static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey192);
}
/// @brief Constructor
PAlgId_Symmetric_AesKey256::PAlgId_Symmetric_AesKey256() noexcept : PAlgId_Symmetric_AesKey{}
{
    UpdateAlgName("-");
    Add256();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_AesKey256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesKey256);
}
/// @brief Check if ID is the same
/// @name   IsMinePrimitiveId
/// @param nAlgId Crypto primitive ID
/// @returns true is suitable false otherwise
bool PAlgId_Symmetric_AesKey256::IsMinePrimitiveId(AlgId nAlgId) const noexcept
{
    return nAlgId == static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey256);
}
//***************/ //AES.Ecb
/// @brief Constructor
PAlgId_Symmetric_Aes_Ecb::PAlgId_Symmetric_Aes_Ecb() noexcept : PAlgId_Symmetric_AesBase{} { AddEcb(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Ecb::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesEcb);
}
// AES.Ecb-128
/// @brief Constructor
PAlgId_Symmetric_Aes_Ecb_128::PAlgId_Symmetric_Aes_Ecb_128() noexcept : PAlgId_Symmetric_Aes_Ecb{}
{
    UpdateAlgName("-");
    Add128();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Ecb_128::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesEcb128);
}
// AES.Ecb-192
/// @brief Constructor
PAlgId_Symmetric_Aes_Ecb_192::PAlgId_Symmetric_Aes_Ecb_192() noexcept : PAlgId_Symmetric_Aes_Ecb{}
{
    UpdateAlgName("-");
    Add192();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Ecb_192::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesEcb192);
}
// AES.Ecb-256
/// @brief Constructor
PAlgId_Symmetric_Aes_Ecb_256::PAlgId_Symmetric_Aes_Ecb_256() noexcept : PAlgId_Symmetric_Aes_Ecb{}
{
    UpdateAlgName("-");
    Add256();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Ecb_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesEcb256);
}
//***************/  //AES.Cbc
/// @brief Constructor
PAlgId_Symmetric_Aes_Cbc::PAlgId_Symmetric_Aes_Cbc() noexcept : PAlgId_Symmetric_AesBase{} { AddCbc(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Cbc::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesCbc);
}
// AES.Cbc-128
/// @brief Constructor
PAlgId_Symmetric_Aes_Cbc_128::PAlgId_Symmetric_Aes_Cbc_128() noexcept : PAlgId_Symmetric_Aes_Cbc{}
{
    UpdateAlgName("-");
    Add128();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Cbc_128::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesCbc128);
}
// AES.Cbc-192
/// @brief Constructor
PAlgId_Symmetric_Aes_Cbc_192::PAlgId_Symmetric_Aes_Cbc_192() noexcept : PAlgId_Symmetric_Aes_Cbc{}
{
    UpdateAlgName("-");
    Add192();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Cbc_192::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesCbc192);
}
// AES.Cbc-256
/// @brief Constructor
PAlgId_Symmetric_Aes_Cbc_256::PAlgId_Symmetric_Aes_Cbc_256() noexcept : PAlgId_Symmetric_Aes_Cbc{}
{
    UpdateAlgName("-");
    Add256();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Cbc_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesCbc256);
}
//***************/  //AES.Ctr
/// @brief Constructor
PAlgId_Symmetric_Aes_Ctr::PAlgId_Symmetric_Aes_Ctr() noexcept : PAlgId_Symmetric_AesBase{} { AddCtr(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Ctr::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesCtr);
}
//***************/  //AES.Gcm
/// @brief Constructor
PAlgId_Symmetric_Aes_Gcm::PAlgId_Symmetric_Aes_Gcm() noexcept : PAlgId_Symmetric_AesBase{} { AddGcm(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Gcm::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesGcm);
}
//***************/  //AES.Cfb
/// @brief Constructor
PAlgId_Symmetric_Aes_Cfb::PAlgId_Symmetric_Aes_Cfb() noexcept : PAlgId_Symmetric_AesBase{} { AddCfb(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Cfb::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesCfb);
}
// AES.Cfb-1
/// @brief Constructor
PAlgId_Symmetric_Aes_Cfb1::PAlgId_Symmetric_Aes_Cfb1() noexcept : PAlgId_Symmetric_Aes_Cfb{}
{
    UpdateAlgName("-");
    UpdateAlgName("1");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Cfb1::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesCfb1);
}
// AES.Cfb-8
/// @brief Constructor

PAlgId_Symmetric_Aes_Cfb8::PAlgId_Symmetric_Aes_Cfb8() noexcept : PAlgId_Symmetric_Aes_Cfb{}
{
    UpdateAlgName("-");
    UpdateAlgName("8");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Cfb8::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesCfb8);
}
// AES.Cfb-128
/// @brief Constructor
PAlgId_Symmetric_Aes_Cfb128::PAlgId_Symmetric_Aes_Cfb128() noexcept : PAlgId_Symmetric_Aes_Cfb{}
{
    UpdateAlgName("-");
    Add128();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Cfb128::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesCfb128);
}
// AES.Ofb
/// @brief Constructor
PAlgId_Symmetric_Aes_Ofb::PAlgId_Symmetric_Aes_Ofb() noexcept : PAlgId_Symmetric_AesBase{} { AddOfb(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Ofb::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesOfb);
}
// AES.Ofb-128
/// @brief Constructor
PAlgId_Symmetric_Aes_Ofb128::PAlgId_Symmetric_Aes_Ofb128() noexcept : PAlgId_Symmetric_Aes_Ofb{}
{
    UpdateAlgName("-");
    Add128();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Symmetric_Aes_Ofb128::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSymmetricAesOfb128);
}
//********************************/
/// @brief Constructor
PAlgId_Asymmetric_Rsa::PAlgId_Asymmetric_Rsa() noexcept : PAlgId_Base{kInt_8U} { AddRsa(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Asymmetric_Rsa::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsa);
}
//********************************/
/// @brief Constructor
PAlgId_Asymmetric_Rsa_Sha1::PAlgId_Asymmetric_Rsa_Sha1() noexcept : PAlgId_Asymmetric_Rsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("-");
    UpdateAlgName("1");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Asymmetric_Rsa_Sha1::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha1);
}
//********************************/
/// @brief Constructor
PAlgId_Asymmetric_Rsa_Sha2_256::PAlgId_Asymmetric_Rsa_Sha2_256() noexcept : PAlgId_Asymmetric_Rsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("256");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Asymmetric_Rsa_Sha2_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_256);
}
//********************************/
/// @brief Constructor
PAlgId_Asymmetric_Rsa_Sha2_384::PAlgId_Asymmetric_Rsa_Sha2_384() noexcept : PAlgId_Asymmetric_Rsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("384");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Asymmetric_Rsa_Sha2_384::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_384);
}
//********************************/
/// @brief Constructor
PAlgId_Asymmetric_Rsa_Sha2_512::PAlgId_Asymmetric_Rsa_Sha2_512() noexcept : PAlgId_Asymmetric_Rsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("512");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Asymmetric_Rsa_Sha2_512::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_512);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Pss::PAlgId_Dsv_SignerPrivateRsa_Pss() noexcept : PAlgId_Dsv_SignerPrivateRsa{}  // NOLINT
{
    UpdateAlgName("-");
    UpdateAlgName("pss");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Pss::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_Pss);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Pss_Md5::PAlgId_Dsv_SignerPrivateRsa_Pss_Md5() noexcept : PAlgId_Dsv_SignerPrivateRsa_Pss{}
{
    UpdateAlgName("-");
    AddMd5();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Pss_Md5::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssMd5);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Pss_Sha1::PAlgId_Dsv_SignerPrivateRsa_Pss_Sha1() noexcept
    : PAlgId_Dsv_SignerPrivateRsa_Pss{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("1");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Pss_Sha1::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha1);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_224::PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_224() noexcept
    : PAlgId_Dsv_SignerPrivateRsa_Pss{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("224");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_224::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_224);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_256::PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_256() noexcept
    : PAlgId_Dsv_SignerPrivateRsa_Pss{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("256");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_256);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_384::PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_384() noexcept
    : PAlgId_Dsv_SignerPrivateRsa_Pss{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("384");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_384::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_384);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_512::PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_512() noexcept
    : PAlgId_Dsv_SignerPrivateRsa_Pss{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("512");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_512::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_512);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateEcdsa::PAlgId_Dsv_SignerPrivateEcdsa() noexcept : PAlgId_Base{kInt_20U}
{
    UpdateAlgName("s");
    UpdateAlgName("i");
    UpdateAlgName("g");
    UpdateAlgName("n");
    UpdateAlgName("ecdsa");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateEcdsa::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsa);
}
/// @brief Check if ID is the same
/// @param nAlgId Crypto primitive ID
/// @return true is suitable false otherwise
bool PAlgId_Dsv_SignerPrivateEcdsa::IsMinePrimitiveId(AlgId nAlgId) const noexcept
{
    if ((nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsa))
        || (nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsaSha1))
        || (nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsaSha2_224))
        || (nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsaSha2_256))
        || (nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsaSha2_384))
        || (nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsaSha2_512))) {
        return true;
    }
    return false;
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateEcdsa_Sha1::PAlgId_Dsv_SignerPrivateEcdsa_Sha1() noexcept : PAlgId_Dsv_SignerPrivateEcdsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("1");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateEcdsa_Sha1::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsaSha1);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateEcdsa_Sha2_224::PAlgId_Dsv_SignerPrivateEcdsa_Sha2_224() noexcept
    : PAlgId_Dsv_SignerPrivateEcdsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("224");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateEcdsa_Sha2_224::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsaSha2_224);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateEcdsa_Sha2_256::PAlgId_Dsv_SignerPrivateEcdsa_Sha2_256() noexcept
    : PAlgId_Dsv_SignerPrivateEcdsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("256");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateEcdsa_Sha2_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsaSha2_256);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateEcdsa_Sha2_384::PAlgId_Dsv_SignerPrivateEcdsa_Sha2_384() noexcept
    : PAlgId_Dsv_SignerPrivateEcdsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("384");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateEcdsa_Sha2_384::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsaSha2_384);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateEcdsa_Sha2_512::PAlgId_Dsv_SignerPrivateEcdsa_Sha2_512() noexcept
    : PAlgId_Dsv_SignerPrivateEcdsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("512");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateEcdsa_Sha2_512::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsaSha2_512);
}
//********************************/
/// @brief Constructor
PAlgId_Dsv_SignerPrivateEcdsa_R5_Sha2_256::PAlgId_Dsv_SignerPrivateEcdsa_R5_Sha2_256() noexcept
    : PAlgId_Dsv_SignerPrivateEcdsa{}
{
    UpdateAlgName("-");
    UpdateAlgName("r");
    UpdateAlgName("5");
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add256();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateEcdsa_R5_Sha2_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsaR5Sha2_256);
}
//***************/
/// @brief Constructor
PAlgId_Asymmetric_RsaKey::PAlgId_Asymmetric_RsaKey() noexcept : PAlgId_Asymmetric_Rsa{} { AddKey(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Asymmetric_RsaKey::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaKey);
}
/// @brief Check if ID is the same
///         Currently, kAsymmetricRsa is not used; kAsymmetricRsaKey is only used for crypto provider encryption context generation
/// @name   IsMinePrimitiveId
/// @param nAlgId Crypto primitive ID
/// @returns true is suitable false otherwise
bool PAlgId_Asymmetric_RsaKey::IsMinePrimitiveId(AlgId nAlgId) const noexcept
{
    if ((nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsa))
        || (nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaKey))
        || (nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaKey512))
        || (nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaKey1024))
        || (nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaKey2048))
        || (nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaKey4096))
        || (nAlgId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaKey8192))) {
        return true;
    }
    return false;
}
//***************/
/// @brief Constructor
PAlgId_Asymmetric_RsaKey512::PAlgId_Asymmetric_RsaKey512() noexcept : PAlgId_Asymmetric_RsaKey{}
{
    UpdateAlgName("-");
    Add512();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
PAlgId_Asymmetric_RsaKey512::AlgId PAlgId_Asymmetric_RsaKey512::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaKey512);
}
//***************/
/// @brief Constructor
PAlgId_Asymmetric_RsaKey1024::PAlgId_Asymmetric_RsaKey1024() noexcept : PAlgId_Asymmetric_RsaKey{}
{
    UpdateAlgName("-");
    Add1024();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
PAlgId_Asymmetric_RsaKey1024::AlgId PAlgId_Asymmetric_RsaKey1024::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaKey1024);
}
//***************/
/// @brief Constructor
PAlgId_Asymmetric_RsaKey2048::PAlgId_Asymmetric_RsaKey2048() noexcept : PAlgId_Asymmetric_RsaKey{}
{
    UpdateAlgName("-");
    Add2048();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
PAlgId_Asymmetric_RsaKey2048::AlgId PAlgId_Asymmetric_RsaKey2048::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaKey2048);
}
//***************/
/// @brief Constructor
PAlgId_Asymmetric_RsaKey4096::PAlgId_Asymmetric_RsaKey4096() noexcept : PAlgId_Asymmetric_RsaKey{}
{
    UpdateAlgName("-");
    Add4096();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
PAlgId_Asymmetric_RsaKey4096::AlgId PAlgId_Asymmetric_RsaKey4096::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaKey4096);
}
//***************/
/// @brief Constructor
PAlgId_Asymmetric_RsaKey8192::PAlgId_Asymmetric_RsaKey8192() noexcept : PAlgId_Asymmetric_RsaKey{}
{
    UpdateAlgName("-");
    Add8192();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
PAlgId_Asymmetric_RsaKey8192::AlgId PAlgId_Asymmetric_RsaKey8192::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaKey8192);
}
//***************/
/// @brief Constructor
PAlgId_Asymmetric_Rsa_Oaep::PAlgId_Asymmetric_Rsa_Oaep() noexcept : PAlgId_Asymmetric_Rsa{}
{
    UpdateAlgName("-");
    UpdateAlgName("o");
    UpdateAlgName("a");
    UpdateAlgName("e");
    UpdateAlgName("p");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
PAlgId_Asymmetric_Rsa_Oaep::AlgId PAlgId_Asymmetric_Rsa_Oaep::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricRsaOaep);
}
//********************************/
/// @brief Constructor
PAlgId_Asymmetric_Ecc::PAlgId_Asymmetric_Ecc() noexcept : PAlgId_Base{kInt_20U}
{
    UpdateAlgName("e");
    UpdateAlgName("c");
    UpdateAlgName("c");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Asymmetric_Ecc::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricEcc);
}
/// @brief Constructor
PAlgId_Asymmetric_EccKey::PAlgId_Asymmetric_EccKey() noexcept : PAlgId_Asymmetric_Ecc{} { AddKey(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Asymmetric_EccKey::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricEccKey);
}
/// @brief Check if ID is the same
/// @param nAlgId Crypto primitive ID
/// @return true is suitable false otherwise
bool PAlgId_Asymmetric_EccKey::IsMinePrimitiveId(AlgId nAlgId) const noexcept
{
    return (nAlgId == static_cast< AlgId >(EPhCtxTypeID::kAsymmetricEccKey))
           || (nAlgId == static_cast< AlgId >(EPhCtxTypeID::kAsymmetricEccKey64))
           || (nAlgId == static_cast< AlgId >(EPhCtxTypeID::kAsymmetricEccKey128))
           || (nAlgId == static_cast< AlgId >(EPhCtxTypeID::kAsymmetricEccKey192))
           || (nAlgId == static_cast< AlgId >(EPhCtxTypeID::kAsymmetricEccKey256));
}
/// @brief Constructor
PAlgId_Asymmetric_EccKey64::PAlgId_Asymmetric_EccKey64() noexcept : PAlgId_Asymmetric_EccKey{}
{
    UpdateAlgName("-");
    Add64();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Asymmetric_EccKey64::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricEccKey64);
}
/// @brief Constructor
PAlgId_Asymmetric_EccKey128::PAlgId_Asymmetric_EccKey128() noexcept : PAlgId_Asymmetric_EccKey{}
{
    UpdateAlgName("-");
    Add128();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Asymmetric_EccKey128::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricEccKey128);
}
/// @brief Constructor
PAlgId_Asymmetric_EccKey192::PAlgId_Asymmetric_EccKey192() noexcept : PAlgId_Asymmetric_EccKey{}
{
    UpdateAlgName("-");
    Add192();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Asymmetric_EccKey192::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricEccKey192);
}
/// @brief Constructor
PAlgId_Asymmetric_EccKey256::PAlgId_Asymmetric_EccKey256() noexcept : PAlgId_Asymmetric_EccKey{}
{
    UpdateAlgName("-");
    Add256();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Asymmetric_EccKey256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAsymmetricEccKey256);
}
//********************************/
/// @brief Constructor
PAlgId_RandomGenerator::PAlgId_RandomGenerator() noexcept : PAlgId_Base{kInt_8U}
{
    AddRan();
    AddDom();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_RandomGenerator::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kRandomGenerator);
}
//********************************/
/// @brief Constructor
PAlgId_RandomGenerator_Global::PAlgId_RandomGenerator_Global() noexcept : PAlgId_Base{kInt_8U}
{
    AddRan();
    AddDom();
    UpdateAlgName("-");
    UpdateAlgName("g");
    UpdateAlgName("l");
    UpdateAlgName("o");
    UpdateAlgName("b");
    UpdateAlgName("a");
    UpdateAlgName("l");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_RandomGenerator_Global::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kRandomGeneratorglobal);
}
//********************************/
/// @brief Parameterized constructor
/// @param nInitLen Initial storage BUFF length
PAlgId_Kdf_Base::PAlgId_Kdf_Base(uint32_t const nInitLen) noexcept : PAlgId_Base{nInitLen} { AddKdf(); }
//***************/
/// @brief Constructor
PAlgId_Kdf_Des::PAlgId_Kdf_Des() noexcept : PAlgId_Kdf_Base{kInt_8U} { AddDes(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Kdf_Des::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Des);
}
//***************/
/// @brief Constructor
PAlgId_Kdf_3Des::PAlgId_Kdf_3Des() noexcept : PAlgId_Kdf_Base{kInt_8U}
{
    UpdateAlgName("3");
    AddDes();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Kdf_3Des::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_3Des);
}
//***************/
/// @brief Constructor
PAlgId_Kdf_Aes::PAlgId_Kdf_Aes() noexcept : PAlgId_Kdf_Base{kInt_8U} { AddAes(); }
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Kdf_Aes::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes);
}
/// @brief Constructor
PAlgId_Kdf_Aes128::PAlgId_Kdf_Aes128() noexcept : PAlgId_Kdf_Aes{}
{
    UpdateAlgName("-");
    Add128();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Kdf_Aes128::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes128);
}
/// @brief Constructor
PAlgId_Kdf_Aes192::PAlgId_Kdf_Aes192() noexcept : PAlgId_Kdf_Aes{}
{
    UpdateAlgName("-");
    Add192();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Kdf_Aes192::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes192);
}
/// @brief Constructor
PAlgId_Kdf_Aes256::PAlgId_Kdf_Aes256() noexcept : PAlgId_Kdf_Aes{}
{
    UpdateAlgName("-");
    Add256();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Kdf_Aes256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes256);
}
//********************************/
/// @brief Constructor
PAlgId_SignatureObj::PAlgId_SignatureObj() noexcept : PAlgId_Base{kInt_16U}
{
    UpdateAlgName("s");
    UpdateAlgName("i");
    UpdateAlgName("g");
    UpdateAlgName("n");
    UpdateAlgName("a");
    UpdateAlgName("t");
    UpdateAlgName("u");
    UpdateAlgName("r");
    UpdateAlgName("e");
    UpdateAlgName("o");
    UpdateAlgName("b");
    UpdateAlgName("j");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_SignatureObj::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignatureObj);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_SigEncodePrivateRsa::PAlgId_Dsv_SigEncodePrivateRsa() noexcept : PAlgId_Base{kInt_16U}
{
    UpdateAlgName("s");
    UpdateAlgName("i");
    UpdateAlgName("g");
    UpdateAlgName("n");
    UpdateAlgName("e");
    UpdateAlgName("n");
    UpdateAlgName("c");
    UpdateAlgName("o");
    UpdateAlgName("d");
    UpdateAlgName("e");
    AddRsa();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SigEncodePrivateRsa::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SigEncodeRsa);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_MsgRecoveryPublicRsa::PAlgId_Dsv_MsgRecoveryPublicRsa() noexcept : PAlgId_Base{kInt_16U}
{
    UpdateAlgName("m");
    UpdateAlgName("s");
    UpdateAlgName("g");
    UpdateAlgName("r");
    UpdateAlgName("e");
    UpdateAlgName("c");
    UpdateAlgName("o");
    UpdateAlgName("v");
    UpdateAlgName("e");
    UpdateAlgName("r");
    UpdateAlgName("y");
    AddRsa();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_MsgRecoveryPublicRsa::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_MsgRecoveryRsa);
}
//***************
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa::PAlgId_Dsv_SignerPrivateRsa() noexcept : PAlgId_Base{kInt_20U}
{
    UpdateAlgName("s");
    UpdateAlgName("i");
    UpdateAlgName("g");
    UpdateAlgName("n");
    AddRsa();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa);
}

//***************
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Md5::PAlgId_Dsv_SignerPrivateRsa_Md5() noexcept : PAlgId_Dsv_SignerPrivateRsa{}
{
    UpdateAlgName("-");
    AddMd5();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Md5::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaMd5);
}
//***************
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Sha1::PAlgId_Dsv_SignerPrivateRsa_Sha1() noexcept : PAlgId_Dsv_SignerPrivateRsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("1");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Sha1::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha1);
}
//***************
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Sha2_224::PAlgId_Dsv_SignerPrivateRsa_Sha2_224() noexcept : PAlgId_Dsv_SignerPrivateRsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("224");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Sha2_224::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_224);
}
//***************
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Sha2_256::PAlgId_Dsv_SignerPrivateRsa_Sha2_256() noexcept : PAlgId_Dsv_SignerPrivateRsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("256");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Sha2_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_256);
}
//***************
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Sha2_384::PAlgId_Dsv_SignerPrivateRsa_Sha2_384() noexcept : PAlgId_Dsv_SignerPrivateRsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("384");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Sha2_384::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_384);
}
//***************
/// @brief Constructor
PAlgId_Dsv_SignerPrivateRsa_Sha2_512::PAlgId_Dsv_SignerPrivateRsa_Sha2_512() noexcept : PAlgId_Dsv_SignerPrivateRsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    UpdateAlgName("512");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_SignerPrivateRsa_Sha2_512::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_512);
}
/********************************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa::PAlgId_Dsv_VerifierPublicRsa() noexcept : PAlgId_Base{kInt_20U}
{
    UpdateAlgName("v");
    UpdateAlgName("e");
    UpdateAlgName("r");
    UpdateAlgName("i");
    UpdateAlgName("f");
    UpdateAlgName("y");
    AddRsa();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsa);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Md5::PAlgId_Dsv_VerifierPublicRsa_Md5() noexcept : PAlgId_Dsv_VerifierPublicRsa{}
{
    UpdateAlgName("-");
    AddMd5();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Md5::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsaMd5);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Sha1::PAlgId_Dsv_VerifierPublicRsa_Sha1() noexcept : PAlgId_Dsv_VerifierPublicRsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("1");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Sha1::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsaSha1);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Sha2_224::PAlgId_Dsv_VerifierPublicRsa_Sha2_224() noexcept : PAlgId_Dsv_VerifierPublicRsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add224();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Sha2_224::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsaSha2_224);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Sha2_256::PAlgId_Dsv_VerifierPublicRsa_Sha2_256() noexcept : PAlgId_Dsv_VerifierPublicRsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add256();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Sha2_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsaSha2_256);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Sha2_384::PAlgId_Dsv_VerifierPublicRsa_Sha2_384() noexcept : PAlgId_Dsv_VerifierPublicRsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add384();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Sha2_384::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsaSha2_384);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Sha2_512::PAlgId_Dsv_VerifierPublicRsa_Sha2_512() noexcept : PAlgId_Dsv_VerifierPublicRsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add512();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Sha2_512::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsaSha2_512);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Pss::PAlgId_Dsv_VerifierPublicRsa_Pss() noexcept : PAlgId_Dsv_VerifierPublicRsa{}
{
    UpdateAlgName("-");
    AddPss();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Pss::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsa_Pss);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Pss_Md5::PAlgId_Dsv_VerifierPublicRsa_Pss_Md5() noexcept
    : PAlgId_Dsv_VerifierPublicRsa_Pss{}
{
    UpdateAlgName("-");
    AddMd5();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Pss_Md5::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsa_PssMd5);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Pss_Sha1::PAlgId_Dsv_VerifierPublicRsa_Pss_Sha1() noexcept
    : PAlgId_Dsv_VerifierPublicRsa_Pss{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("1");
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Pss_Sha1::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsa_PssSha1);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_224::PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_224() noexcept
    : PAlgId_Dsv_VerifierPublicRsa_Pss{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add224();
}
/// @brief Get vendor-specific crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_224::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsa_PssSha2_224);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_256::PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_256() noexcept
    : PAlgId_Dsv_VerifierPublicRsa_Pss{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add256();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsa_PssSha2_256);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_384::PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_384() noexcept
    : PAlgId_Dsv_VerifierPublicRsa_Pss{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add384();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_384::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsa_PssSha2_384);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_512::PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_512() noexcept
    : PAlgId_Dsv_VerifierPublicRsa_Pss{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add512();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_512::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicRsa_PssSha2_512);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicEcdsa::PAlgId_Dsv_VerifierPublicEcdsa() noexcept : PAlgId_Base{kInt_20U}
{
    UpdateAlgName("v");
    UpdateAlgName("e");
    UpdateAlgName("r");
    UpdateAlgName("i");
    UpdateAlgName("f");
    UpdateAlgName("y");
    UpdateAlgName("ecdsa");
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicEcdsa::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicEcdsa);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicEcdsa_Sha1::PAlgId_Dsv_VerifierPublicEcdsa_Sha1() noexcept : PAlgId_Dsv_VerifierPublicEcdsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("1");
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicEcdsa_Sha1::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicEcdsaSha1);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicEcdsa_Sha2_224::PAlgId_Dsv_VerifierPublicEcdsa_Sha2_224() noexcept
    : PAlgId_Dsv_VerifierPublicEcdsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add224();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicEcdsa_Sha2_224::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicEcdsaSha2_224);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicEcdsa_Sha2_256::PAlgId_Dsv_VerifierPublicEcdsa_Sha2_256() noexcept
    : PAlgId_Dsv_VerifierPublicEcdsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add256();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicEcdsa_Sha2_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicEcdsaSha2_256);
}  //***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicEcdsa_Sha2_384::PAlgId_Dsv_VerifierPublicEcdsa_Sha2_384() noexcept
    : PAlgId_Dsv_VerifierPublicEcdsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add384();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicEcdsa_Sha2_384::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicEcdsaSha2_384);
}
//***************/
/// @brief Constructor
PAlgId_Dsv_VerifierPublicEcdsa_Sha2_512::PAlgId_Dsv_VerifierPublicEcdsa_Sha2_512() noexcept
    : PAlgId_Dsv_VerifierPublicEcdsa{}
{
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add512();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicEcdsa_Sha2_512::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicEcdsaSha2_512);
}
//***************/
/// @brief Constructor
/// @throws
PAlgId_Dsv_VerifierPublicEcdsa_R5_Sha2_256::PAlgId_Dsv_VerifierPublicEcdsa_R5_Sha2_256() noexcept
    : PAlgId_Dsv_VerifierPublicEcdsa{}
{
    UpdateAlgName("-");
    UpdateAlgName("r");
    UpdateAlgName("5");
    UpdateAlgName("-");
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add256();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return
CryptoPrimitiveId::AlgId PAlgId_Dsv_VerifierPublicEcdsa_R5_Sha2_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_VerifierPublicEcdsaR5Sha2_256);
}
//********************************/ //CMAC aes
/// @brief Constructor
PAlgId_MacAesBase::PAlgId_MacAesBase() noexcept : PAlgId_Base{kInt_20U}
{
    AddMac();
    AddAes();
}
//***************/
/// @brief Constructor
PAlgId_MacAesCbc_128::PAlgId_MacAesCbc_128() noexcept : PAlgId_MacAesBase{}
{
    AddCbc();
    UpdateAlgName("-");
    Add128();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_MacAesCbc_128::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kMac_Aes_Cbc_128);
}
//***************/
/// @brief Constructor
PAlgId_MacAesCbc_192::PAlgId_MacAesCbc_192() noexcept : PAlgId_MacAesBase{}
{
    AddCbc();
    UpdateAlgName("-");
    Add192();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_MacAesCbc_192::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kMac_Aes_Cbc_192);
}
//***************/
/// @brief Constructor
PAlgId_MacAesCbc_256::PAlgId_MacAesCbc_256() noexcept : PAlgId_MacAesBase{}
{
    AddCbc();
    UpdateAlgName("-");
    Add256();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_MacAesCbc_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kMac_Aes_Cbc_256);
}
//********************************/ //CMAC des
/// @brief Constructor
PAlgId_MacDesBase::PAlgId_MacDesBase() noexcept : PAlgId_Base{kInt_20U}
{
    AddMac();
    AddDes();
}
//***************/
/// @brief Constructor
PAlgId_MacDesCbc::PAlgId_MacDesCbc() noexcept : PAlgId_MacDesBase{} { AddCbc(); }
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_MacDesCbc::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kMac_Des_Cbc);
}
//***************/
/// @brief Constructor
PAlgId_MacDesCbc_ede2::PAlgId_MacDesCbc_ede2() noexcept : PAlgId_MacDesBase{}
{
    AddCbc();
    UpdateAlgName("-");
    AddEde();
    UpdateAlgName("2");
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_MacDesCbc_ede2::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kMac_Des_Cbc_ede2);
}
//***************/
/// @brief Constructor
PAlgId_MacDesCbc_ede3::PAlgId_MacDesCbc_ede3() noexcept : PAlgId_MacDesBase{}
{
    AddCbc();
    UpdateAlgName("-");
    AddEde();
    UpdateAlgName("3");
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_MacDesCbc_ede3::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kMac_Des_Cbc_ede3);
}
//********************************/ //mac hash
/// @brief Constructor
PAlgId_Machash::PAlgId_Machash() noexcept : PAlgId_Base{kInt_20U}
{
    AddMac();
    AddHash();
}
//***************/
/// @brief Constructor
PAlgId_MacHashMd5::PAlgId_MacHashMd5() noexcept : PAlgId_Machash{} { AddMd5(); }
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_MacHashMd5::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kMac_Hash_Md5);
}
//***************/
/// @brief Constructor
PAlgId_MacHashSha1::PAlgId_MacHashSha1() noexcept : PAlgId_Machash{}
{
    AddSha();
    UpdateAlgName("1");
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_MacHashSha1::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kMac_Hash_Sha1);
}
//***************/
/// @brief Constructor
PAlgId_MacHashSha2_224::PAlgId_MacHashSha2_224() noexcept : PAlgId_Machash{}
{
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add224();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_MacHashSha2_224::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kMac_Hash_Sha2_224);
}
//***************/
/// @brief Constructor
PAlgId_MacHashSha2_256::PAlgId_MacHashSha2_256() noexcept : PAlgId_Machash{}
{
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add256();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_MacHashSha2_256::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kMac_Hash_Sha2_256);
}
//***************/
/// @brief Constructor
PAlgId_MacHashSha2_384::PAlgId_MacHashSha2_384() noexcept : PAlgId_Machash{}
{
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add384();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_MacHashSha2_384::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kMac_Hash_Sha2_384);
}
//***************/
/// @brief Constructor
PAlgId_MacHashSha2_512::PAlgId_MacHashSha2_512() noexcept : PAlgId_Machash{}
{
    AddSha();
    UpdateAlgName("2");
    UpdateAlgName("-");
    Add512();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_MacHashSha2_512::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kMac_Hash_Sha2_512);
}
//********************************/ //wrap
/// @brief Constructor
PAlgId_WrapAes::PAlgId_WrapAes() noexcept : PAlgId_Base{kInt_24U}
{
    UpdateAlgName("w");
    UpdateAlgName("r");
    UpdateAlgName("a");
    UpdateAlgName("p");
    AddAes();
}
//********************************/ //WrapAesPad
/// @brief Constructor
PAlgId_WrapAesPad::PAlgId_WrapAesPad() noexcept : PAlgId_WrapAes{} { AddPad(); }
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_WrapAesPad::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kWrap_Aes_pad);
}
//********************************/ //PAlgId_WrapAesUnPad
/// @brief Constructor
PAlgId_WrapAesUnPad::PAlgId_WrapAesUnPad() noexcept : PAlgId_WrapAes{}
{
    UpdateAlgName("u");
    UpdateAlgName("n");
    AddPad();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_WrapAesUnPad::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kWrap_Aes_unpad);
}
//********************************/ //PAlgId_WrapAesUnPad_256
/// @brief Constructor
PAlgId_WrapDes::PAlgId_WrapDes() noexcept : PAlgId_Base{kInt_20U}
{
    UpdateAlgName("w");
    UpdateAlgName("r");
    UpdateAlgName("a");
    UpdateAlgName("p");
    AddDes();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_WrapDes::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kWrap_Des);
}
//********************************/ //PAlgId_Kem_private
/// @brief Constructor
PAlgId_Kem_private::PAlgId_Kem_private() noexcept : PAlgId_Base{kInt_24U}
{
    UpdateAlgName("k");
    UpdateAlgName("e");
    UpdateAlgName("m");
    UpdateAlgName("p");
    UpdateAlgName("r");
    UpdateAlgName("i");
    UpdateAlgName("v");
    UpdateAlgName("a");
    UpdateAlgName("t");
    UpdateAlgName("e");
}
//********************************/ //PAlgId_Kem_private_rsa
/// @brief Constructor
PAlgId_Kem_private_rsa::PAlgId_Kem_private_rsa() noexcept : PAlgId_Kem_private{}
{
    UpdateAlgName("-");
    AddRsa();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Kem_private_rsa::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKem_decapsulator_rsa);
}
//********************************/ //PAlgId_Kem_public
/// @brief Constructor
PAlgId_Kem_public::PAlgId_Kem_public() noexcept : PAlgId_Base{kInt_24U}
{
    UpdateAlgName("k");
    UpdateAlgName("e");
    UpdateAlgName("m");
    UpdateAlgName("p");
    UpdateAlgName("u");
    UpdateAlgName("b");
    UpdateAlgName("l");
    UpdateAlgName("i");
    UpdateAlgName("c");
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Kem_public::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKem_encapsulator);
}
//********************************/ //PAlgId_Kem_public_rsa
/// @brief Constructor
PAlgId_Kem_public_rsa::PAlgId_Kem_public_rsa() noexcept : PAlgId_Kem_public{}
{
    UpdateAlgName("-");
    AddRsa();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Kem_public_rsa::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKem_encapsulator_rsa);
}
//********************************/ //PAlgId_Key_Agreement_Private
/// @brief Constructor
PAlgId_Key_Agreement_Private::PAlgId_Key_Agreement_Private() noexcept : PAlgId_Base{kInt_24U}
{
    AddKey();
    UpdateAlgName("-");
    UpdateAlgName("a");
    UpdateAlgName("g");
    UpdateAlgName("r");
    UpdateAlgName("e");
    UpdateAlgName("e");
    UpdateAlgName("m");
    UpdateAlgName("e");
    UpdateAlgName("n");
    UpdateAlgName("t");
    UpdateAlgName("-");
    UpdateAlgName("p");
    UpdateAlgName("r");
    UpdateAlgName("i");
    UpdateAlgName("v");
    UpdateAlgName("a");
    UpdateAlgName("t");
    UpdateAlgName("e");
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Key_Agreement_Private::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKey_agreement_private);
}
//********************************/ //PAlgId_Ae_Stream
/// @brief Constructor
PAlgId_Ae_Stream::PAlgId_Ae_Stream() noexcept : PAlgId_Base{kInt_20U}
{
    UpdateAlgName("a");
    UpdateAlgName("e");
    UpdateAlgName("-");
    UpdateAlgName("s");
    UpdateAlgName("t");
    UpdateAlgName("r");
    UpdateAlgName("e");
    UpdateAlgName("a");
    UpdateAlgName("m");
}
//********************************/ //PAlgId_Ae_Stream_Aes_Cbc
/// @brief Constructor
PAlgId_Ae_Stream_Aes_Cbc::PAlgId_Ae_Stream_Aes_Cbc() noexcept : PAlgId_Ae_Stream{}
{
    UpdateAlgName("-");
    AddAes();
    UpdateAlgName("-");
    AddCbc();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Ae_Stream_Aes_Cbc::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAe_Aes);
}
//********************************/ //PAlgId_Ae_Stream_Des_Cbc
/// @brief Constructor
PAlgId_Ae_Stream_Des_Cbc::PAlgId_Ae_Stream_Des_Cbc() noexcept : PAlgId_Ae_Stream{}
{
    UpdateAlgName("-");
    AddDes();
    UpdateAlgName("-");
    AddCbc();
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_Ae_Stream_Des_Cbc::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kAe_Des);
}
//********************************/ //PAlgId_SecretseedObj
/// @brief Constructor
PAlgId_SecretseedObj::PAlgId_SecretseedObj() noexcept : PAlgId_Base{kInt_20U}
{
    UpdateAlgName("s");
    UpdateAlgName("e");
    UpdateAlgName("c");
    UpdateAlgName("r");
    UpdateAlgName("e");
    UpdateAlgName("t");
    UpdateAlgName("s");
    UpdateAlgName("e");
    UpdateAlgName("e");
    UpdateAlgName("d");
}
/// @brief Get the vendor-specific cryptographic primitive ID
/// @return Cryptographic primitive ID
CryptoPrimitiveId::AlgId PAlgId_SecretseedObj::GetPrimitiveId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kSecret_seedObj);
}

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
