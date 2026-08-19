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
/// @file       isoft_data_type.h
/// @brief      AutoSar-AP
/// @details    Some common data
/// @date       2023-08-15
/// @author     CheJinzhao
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author          <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=MAraCoreInit
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_COMMON_PUHUA_DATA_TYPE_H_
#define ARA_CRYPTO_COMMON_PUHUA_DATA_TYPE_H_

#include <cstddef>
#include <cstdint>

namespace ara {
namespace crypto {

//********************************/
// unsigned int8
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00330
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_0U{0U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00331
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_5U{5U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_08772
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_6U{6U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_08773
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_7U{7U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00332
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_8U{8U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00333
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_10U{8U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00334
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_16U{16U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00335
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_20U{20U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00336
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_28U{28U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00337
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_32U{32U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00338
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_48U{48U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00339
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_64U{64U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00340
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_0x80U{0x80U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00341
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_0x87U{0x87U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00342
/// @needwork = dd
/// @endcode
constexpr uint8_t kInt8_0x1bU{0x1bU};

//********************************/
/// @brief QAC2428: Cannot use char directly
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_06241
/// @needwork = dd
/// @endcode
using char8_t = char;  // NOLINT
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00343
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_0{0};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00344
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_1{1};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00345
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_2{2};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00346
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_3{3};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00347
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_4{4};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00348
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_5{5};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00349
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_7{7};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00350
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_8{8};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00351
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_9{9};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00352
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_10{10};
constexpr int32_t kInt_11{11};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00353
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_16{16};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00354
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_24{24};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00355
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_32{32};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00356
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_60{60};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00357
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_64{64};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00358
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_100{100};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00359
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_128{128};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00360
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_192{192};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00361
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_256{256};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00362
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_255{255};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00363
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_512{512};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00364
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_1024{1024};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00365
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_2048{2048};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00366
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_4096{4096};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00367
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_8192{8192};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00368
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_0x10{0x10};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00369
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_0x100{0x100};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00370
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_0x10000{0x10000};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00371
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_2E5{static_cast< int32_t >(1U << 5U)};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00372
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_2E11{static_cast< int32_t >(1U << 11U)};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00373
/// @needwork = dd
/// @endcode
constexpr int32_t kInt_FF{0xFF};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00374
/// @needwork = dd
/// @endcode
constexpr uint16_t kInt_0xFFFFU{0xFFFFU};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00375
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_0U{0U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00376
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_1U{1U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00377
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_2U{2U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00378
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_3U{3U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00379
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_4U{4U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00380
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_5U{5U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00381
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_6U{6U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00382
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_7U{7U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00383
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_8U{8U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00384
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_9U{9U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00385
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_10U{10U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00386
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_11U{11U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00387
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_12U{12U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00388
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_14U{14U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00389
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_16U{16U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00390
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_20U{20U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00391
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_24U{24U};
constexpr uint32_t kInt_25U{25U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00392
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_28U{28U};
constexpr uint32_t kInt_30U{30U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00393
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_32U{32U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00394
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_36U{36U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00395
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_40U{40U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00396
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_47U{47U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00397
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_48U{48U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00398
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_52U{52U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00399
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_56U{56U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00400
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_60U{60U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00401
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_64U{64U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00402
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_70U{70U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00403
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_72U{72U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00404
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_74U{74U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00405
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_88U{88U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00406
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_94U{94U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00407
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_118U{118U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00408
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_126U{126U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00409
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_128U{128U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00410
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_130U{130U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00411
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_140U{140U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00412
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_150U{150U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00413
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_158U{158U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00414
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_160U{160U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00415
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_162U{162U};
constexpr uint32_t kInt_172U{172U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00416
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_174U{174U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00417
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_178U{178U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00418
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_190U{190U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00419
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_192U{192U};
constexpr uint32_t kInt_205U{205U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00420
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_208U{208U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00421
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_223U{223U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00422
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_224U{224U};
constexpr uint32_t kInt_237U{237U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00423
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_240U{240U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00424
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_251U{251U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00425
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_252U{252U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00426
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_255U{255U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00427
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_43U{43U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00428
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_100U{100U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00429
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_256U{256U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00430
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_270U{270U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00431
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_317U{317U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00432
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_320U{320U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00433
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_384U{384U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00434
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_426U{426U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00435
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_493U{493U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00436
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_512U{512U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00437
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_521U{521U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00438
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_526U{526U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00439
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_607U{607U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00440
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_666U{666U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00441
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_775U{775U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00442
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_887U{887U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00443
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_1000U{1000U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00444
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_1024U{1024U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00445
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_1038U{1038U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00446
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_1190U{1190U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00447
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_1466U{1466U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00448
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_1675U{1675U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00449
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_1900U{1900U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00450
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_2048U{2048U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00451
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_2347U{2347U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00452
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_3243U{3243U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00453
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_4096U{4096U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00454
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_4651U{4651U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00455
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_6363U{6363U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00456
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_8192U{8192U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00457
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_0x07U{static_cast< uint32_t >(0x07)};
constexpr uint32_t kInt_0x01U{static_cast< uint32_t >(0x01)};
constexpr uint32_t kInt_0x10U{static_cast< uint32_t >(kInt_0x10)};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00458
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_0x100U{static_cast< uint32_t >(kInt_0x100)};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00459
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_0xFFU{0xFFU};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00460
/// @needwork = dd
/// @endcode
constexpr uint32_t kInt_0xFFFFFFFFU{0xFFFFFFFFU};

// 64
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00461
/// @needwork = dd
/// @endcode
constexpr uint64_t kInt_0x100000000U{0x100000000U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00462
/// @needwork = dd
/// @endcode
constexpr uint64_t kInt_0x00FF000000000000U{0x00FF000000000000U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00463
/// @needwork = dd
/// @endcode
constexpr uint64_t kInt_0xFF00000000000000U{0xFF00000000000000U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00464
/// @needwork = dd
/// @endcode
constexpr uint64_t kInt_0x0000FF0000000000U{0x0000FF0000000000U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00465
/// @needwork = dd
/// @endcode
constexpr uint64_t kInt_0x000000FFFFFFFFFFU{0x000000FFFFFFFFFFU};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00466
/// @needwork = dd
/// @endcode
constexpr char8_t kChar_a{'a'};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00467
/// @needwork = dd
/// @endcode
constexpr char8_t kChar_z{'z'};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00468
/// @needwork = dd
/// @endcode
constexpr char8_t kChar_A{'A'};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00469
/// @needwork = dd
/// @endcode
constexpr char8_t kChar_Z{'Z'};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00470
/// @needwork = dd
/// @endcode
constexpr char8_t kChar_0{'0'};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00471
/// @needwork = dd
/// @endcode
constexpr char8_t kChar_9{'9'};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00472
/// @needwork = dd
/// @endcode
constexpr char8_t kChar_Enter{'\n'};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00473
/// @needwork = dd
/// @endcode
constexpr char8_t kChar_Dollar{'$'};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00474
/// @needwork = dd
/// @endcode
constexpr char8_t kChar_Comma{','};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00475
/// @needwork = dd
/// @endcode
constexpr char8_t kChar_BraceEnd{'}'};

// ----------- HSM -----------
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00476
/// @needwork = dd
/// @endcode
uint64_t const kUlInt0UL{0U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00477
/// @needwork = dd
/// @endcode
uint64_t const kUlInt8UL{8U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00478
/// @needwork = dd
/// @endcode
uint64_t const kUlInt20UL{20U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00479
/// @needwork = dd
/// @endcode
uint64_t const kUlInt28UL{28U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00480
/// @needwork = dd
/// @endcode
uint64_t const kUlInt32UL{32U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00481
/// @needwork = dd
/// @endcode
uint64_t const kUlInt48UL{48U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00482
/// @needwork = dd
/// @endcode
uint64_t const kUlInt64UL{64U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00483
/// @needwork = dd
/// @endcode
uint64_t const kUlInt128UL{128U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_00484
/// @needwork = dd
/// @endcode
uint64_t const kUlInt4096UL{4096U};

//********************************/
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_COMMON_PUHUA_DATA_TYPE_H_