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
/// @file       isoft_x509_process_base.cpp
/// @brief      Base class for IPC server-side key provider logic processing of KeyProvider provider.
/// @details
/// @date       2023-09-24
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Manager/Certificate Manager
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=PX509Process_Base
/// @unit_description=Base class for server-side certificate provider logic processing
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/isoft_x509_process_base.h"

#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/x509/isoft_x509_manager.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Extended use of isoft's logging system
using ara::crypto::isoft_def::LogInfo;
//********************************/
/// @brief Parameterized constructor
/// @param lpcProcessManager LCP command processing manager
PX509Process_Base::PX509Process_Base(PX509_Manager& lpcProcessManager) noexcept : lpcProcessManager_{lpcProcessManager}
{
}
//***************/
/// @brief Gets the referenced provider
/// @return Crypto provider reference
cryp::CryptoProvider& PX509Process_Base::GetCryptoProvider() const noexcept
{
    return lpcProcessManager_.GetCryptoProvider();
}
//********************************/
/// @brief Handles IPC errors
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param nErrorCode Error code
/// @return 0 sucess fail otherwise
PX509Process_Base::PResultLen PX509Process_Base::ProcessCmd_Error(PIpcPac_Head const* const pReqHead,
                                                                  PIpcAutoPacket& aswMsg,
                                                                  SecurityErrorDomain::Errc const nErrorCode) noexcept
{
    PX509_Manager::ProcessCmd_Error(pReqHead, aswMsg, nErrorCode);
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Handles IPC errors
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param nErrorCode Error code
/// @return 0 sucess fail otherwise
PX509Process_Base::PResultLen PX509Process_Base::ProcessCmd_Error(PIpcPac_Head const* const pReqHead,
                                                                  PIpcAutoPacket& aswMsg,
                                                                  int32_t const nErrorCode) noexcept
{
    PX509_Manager::ProcessCmd_Error(pReqHead, aswMsg, static_cast< SecurityErrorDomain::Errc >(nErrorCode));
    return PResultLen::FromValue(pReqHead->nPacSize);
}

//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
