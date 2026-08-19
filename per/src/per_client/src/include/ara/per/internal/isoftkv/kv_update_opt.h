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
/// @file       kv_update_opt.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    KV storage self-update operation class
/// @date       2021-08-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KeyValueStorage/Data Version Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=KV storage self-update operation
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-08-18 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#ifndef ARA_PER_PHKV_KV_UPDATE_OPT_H_
#define ARA_PER_PHKV_KV_UPDATE_OPT_H_

#include "ara/per/internal/common/isoft_auto_buff.h"
#include "ara/per/internal/common/isoft_file_opt.h"
#include "ara/per/internal/isoftkv/kv_error_code.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
/// @code{.isoft}
/// @unit_name=PWordEncode
/// @endcode
class PWordEncode final
{
private:
    /// @brief
    uint8_t nXorCode_;

public:
    /// @brief
    PWordEncode() = default;
    /// @brief
    /// @param nXorCode
    explicit PWordEncode(uint32_t const nXorCode) noexcept;
    /// @brief
    /// @param nXorCode
    inline void SetCode(uint32_t const nXorCode) noexcept
    {
        nXorCode_ = static_cast< uint8_t >(T_Mod(nXorCode, kInt_0x100));
    }
    /// @brief
    /// @param chInput
    /// @param nRandom
    /// @return
    ara::core::String Encode(uint8_t chInput, int32_t const nRandom) const noexcept;
    /// @brief
    /// @param chA
    /// @param chB
    /// @param nRandom
    /// @return
    uint8_t Decode(uint8_t const chA, uint8_t const chB, int32_t const nRandom) const noexcept;

protected:
};
//********************************/
/// @brief Four different attributes for each line of data in the *.update file
/// @code{.isoft}
/// @unit_name=EUpdateKeyWord
/// @endcode
enum class EUpdateKeyWord : char8_t
{
    kAction = 'A',  // UpdateAction
    kKey    = 'K',  // Key
    kValue  = 'V',  // Value
    kCrc    = 'C',  // CRC
};
/// @brief
/// @code{.isoft}
/// @unit_name=PKvUpdateSave_Base
/// @interface_level=unit
/// @endcode
class PKvUpdateSave_Base
{
private:
    /// @brief
    PFileOpt const &fileOpt_;
    /// @brief
    int32_t nLineIndex_;
    /// @brief
    PWordEncode *pWordEncode_;
    /// @brief Whether started
    bool bHaveBegin_;

protected:
    /// @brief
    /// @return
    inline PFileOpt const &_GetFileOpt() const noexcept { return fileOpt_; }

public:
    /// @brief
    /// @param fileOpt
    /// @param nLineIndex
    /// @param pWordEncode
    explicit PKvUpdateSave_Base(PFileOpt const &fileOpt,
                                int32_t const nLineIndex,
                                PWordEncode *const pWordEncode) noexcept;
    /// @brief
    virtual ~PKvUpdateSave_Base() = default;
    /// @brief
    PKvUpdateSave_Base() = delete;
    /// @brief
    /// @param a
    PKvUpdateSave_Base(PKvUpdateSave_Base const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateSave_Base &operator=(PKvUpdateSave_Base const &a) = delete;
    /// @brief
    /// @param a
    PKvUpdateSave_Base(PKvUpdateSave_Base &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateSave_Base &operator=(PKvUpdateSave_Base &&a) = delete;
    /// @brief
    /// @param eKeyWord
    /// @param stData
    /// @param bEndLine
    void SaveString(EUpdateKeyWord const eKeyWord, ara::core::String const &stData, bool const bEndLine) noexcept;
    /// @brief
    /// @param eKeyWord
    /// @param nDataType
    /// @param nDataLen
    /// @return
    virtual int32_t SaveBegin(EUpdateKeyWord const eKeyWord, int32_t const nDataType, uint32_t const nDataLen) noexcept;
    /// @brief
    /// @param bEndLine
    /// @return
    virtual int32_t SaveEnd(bool const bEndLine) noexcept;
    /// @brief
    /// @param pVoidData
    /// @param nDataLen
    /// @return
    virtual int32_t SaveData(const void *const pVoidData, uint32_t const nDataLen) noexcept;

protected:
    /// @brief
    /// @return
    bool _SaveStart() noexcept;
    /// @brief
    /// @param bEndLine
    /// @param stWord
    /// @return
    int32_t _SaveEnd(bool const bEndLine, ara::core::StringView const &stWord) noexcept;
};
//***************/
/// @brief Save format: "K=A,9,{data},"
/// @code{.isoft}
/// @unit_name=PKvUpdateSave_Value
/// @endcode
class PKvUpdateSave_Value final : public PKvUpdateSave_Base
{
public:
    /// @brief
    /// @param fileOpt
    /// @param nLineIndex
    /// @param pWordEncode
    explicit PKvUpdateSave_Value(PFileOpt const &fileOpt,
                                 int32_t const nLineIndex,
                                 PWordEncode *const pWordEncode = nullptr) noexcept;
    /// @brief
    ~PKvUpdateSave_Value() noexcept final = default;
    /// @brief
    /// @param a
    PKvUpdateSave_Value(PKvUpdateSave_Value const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateSave_Value &operator=(PKvUpdateSave_Value const &a) = delete;
    /// @brief
    /// @param a
    PKvUpdateSave_Value(PKvUpdateSave_Value &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateSave_Value &operator=(PKvUpdateSave_Value &&a) = delete;

public:
    /// @brief
    /// @param eKeyWord
    /// @param nDataType
    /// @param nDataLen
    /// @return
    int32_t SaveBegin(EUpdateKeyWord const eKeyWord, int32_t const nDataType, uint32_t const nDataLen) noexcept final;
    /// @brief
    /// @param bEndLine
    /// @return
    int32_t SaveEnd(bool const bEndLine) noexcept final;
};
//***************/
/// @brief Save format: "K={data},"
/// @code{.isoft}
/// @unit_name=PKvUpdateSave_String
/// @endcode
class PKvUpdateSave_String final : public PKvUpdateSave_Base
{
public:
    /// @brief
    /// @param fileOpt
    /// @param nLineIndex
    /// @param pWordEncode
    explicit PKvUpdateSave_String(PFileOpt const &fileOpt,
                                  int32_t const nLineIndex,
                                  PWordEncode *const pWordEncode = nullptr) noexcept;
    /// @brief
    ~PKvUpdateSave_String() noexcept final = default;
    /// @brief
    /// @param a
    PKvUpdateSave_String(PKvUpdateSave_String const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateSave_String &operator=(PKvUpdateSave_String const &a) = delete;
    /// @brief
    /// @param a
    PKvUpdateSave_String(PKvUpdateSave_String &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateSave_String &operator=(PKvUpdateSave_String &&a) = delete;

public:
    /// @brief
    /// @param eKeyWord
    /// @param nDataType
    /// @param nDataLen
    /// @return
    int32_t SaveBegin(EUpdateKeyWord const eKeyWord, int32_t const nDataType, uint32_t const nDataLen) noexcept final;
    /// @brief
    /// @param bEndLine
    /// @return
    int32_t SaveEnd(bool const bEndLine) noexcept final;
};
//***************/
/// @brief Save format: "[data]:"
/// @code{.isoft}
/// @unit_name=PKvUpdateSave_Action
/// @interface_level=unit
/// @endcode
class PKvUpdateSave_Action final : public PKvUpdateSave_Base
{
public:
    /// @brief
    /// @param fileOpt
    /// @param nLineIndex
    /// @param pWordEncode
    explicit PKvUpdateSave_Action(PFileOpt const &fileOpt,
                                  int32_t const nLineIndex,
                                  PWordEncode *const pWordEncode = nullptr) noexcept;
    /// @brief
    ~PKvUpdateSave_Action() noexcept final = default;
    /// @brief
    /// @param a
    PKvUpdateSave_Action(PKvUpdateSave_Action const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateSave_Action &operator=(PKvUpdateSave_Action const &a) = delete;
    /// @brief
    /// @param a
    PKvUpdateSave_Action(PKvUpdateSave_Action &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateSave_Action &operator=(PKvUpdateSave_Action &&a) = delete;

public:
    /// @brief
    /// @param eKeyWord
    /// @param nDataType
    /// @param nDataLen
    /// @return
    int32_t SaveBegin(EUpdateKeyWord const eKeyWord, int32_t const nDataType, uint32_t const nDataLen) noexcept final;
    /// @brief
    /// @param bEndLine
    /// @return
    int32_t SaveEnd(bool const bEndLine) noexcept final;
};
//********************************/
/// @brief
/// @code{.isoft}
/// @unit_name=PKvUpdateRead_Base
/// @endcode
class PKvUpdateRead_Base
{
private:
    /// @brief
    int32_t nLineIndex_;
    /// @brief Update content identifier characters: Strategy/Key/Value/CRC
    EUpdateKeyWord eUpdateWord_;
    /// @brief
    PWordEncode *pWordEncode_;
    /// @brief Need to save an extra copy because the read raw data needs transcoding before use // One line of data may span multiple pages, so nDataLen_ <=
    /// nTotalLen_
    PAutoBuff readBuff_;
    /// @brief Remaining characters from the previous page when encountering odd page crossing in two-byte Base16 encoding
    uint8_t nExtChar_;
    /// @brief Offset relative to the original Line
    int32_t nDataStartPos_;

public:
    /// @brief
    PKvUpdateRead_Base() = delete;
    /// @brief
    /// @param nLineIndex
    /// @param eUpdateWord
    /// @param pWordEncode
    explicit PKvUpdateRead_Base(int32_t const nLineIndex,
                                EUpdateKeyWord const eUpdateWord,
                                PWordEncode *const pWordEncode) noexcept;
    /// @brief
    virtual ~PKvUpdateRead_Base() = default;
    /// @brief
    /// @param a
    PKvUpdateRead_Base(PKvUpdateRead_Base const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateRead_Base &operator=(PKvUpdateRead_Base const &a) = delete;
    /// @brief
    /// @param a
    PKvUpdateRead_Base(PKvUpdateRead_Base &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateRead_Base &operator=(PKvUpdateRead_Base &&a) = delete;

public:
    /// @brief
    /// @return
    inline EUpdateKeyWord GetUpdateWord() const noexcept { return eUpdateWord_; }
    /// @brief
    /// @return
    uint8_t const *GetReadData() const noexcept;
    /// @brief
    /// @return
    uint32_t GetReadLen() const noexcept;
    /// @brief
    /// @return
    inline int32_t GetDataStartPos() const noexcept { return nDataStartPos_; }
    /// @brief
    /// @return
    virtual int32_t GetReadDataType() const noexcept;
    /// @brief
    /// @return
    virtual uint32_t GetReadTotalLen() const noexcept;
    /// @brief Interpret the read data
    /// @param pDataBase
    /// @param nDataTotal
    /// @param nOffset
    /// @return
    virtual int32_t ParseReadData(uint8_t const *pDataBase, int32_t nDataTotal, int32_t const nOffset) noexcept;
    /// @brief Interpret the read cross-page data
    /// @param pDataBase
    /// @param nDataTotal
    /// @param nOffset
    /// @return
    virtual int32_t ParseReadDataAdd(uint8_t const *pDataBase, int32_t nDataTotal, int32_t const nOffset) noexcept;

protected:
    /// @brief Interpret the read data
    /// @param pBReadData
    /// @param nReadLen
    /// @return
    int32_t _DecodeData(uint8_t const *const pBReadData, int32_t const nReadLen) noexcept;

protected:
    /// @brief Reset Read result data
    virtual void V_ResetRead() noexcept;
    /// @brief
    /// @param stReadData
    /// @param nDataStart
    /// @param nDataEnd
    /// @return
    virtual int32_t V_ParseReadData(ara::core::StringView const &stReadData,
                                    ara::core::StringView::size_type &nDataStart,
                                    ara::core::StringView::size_type &nDataEnd) noexcept = 0;
};
//***************/
/// @brief Interpret format: "K=A,9,{data}"
/// @code{.isoft}
/// @unit_name=PKvUpdateRead_Value
/// @interface_level=unit
/// @endcode
class PKvUpdateRead_Value final : public PKvUpdateRead_Base
{
private:
    /// @brief Data type
    int32_t nDataType_{0};
    /// @brief Total length read
    uint32_t nTotalLen_{0U};

public:
    /// @brief
    /// @param nLineIndex
    /// @param eUpdateWord
    /// @param pWordEncode
    explicit PKvUpdateRead_Value(int32_t const nLineIndex,
                                 EUpdateKeyWord const eUpdateWord,
                                 PWordEncode *const pWordEncode = nullptr) noexcept;
    /// @brief
    ~PKvUpdateRead_Value() noexcept final = default;
    /// @brief
    /// @param a
    PKvUpdateRead_Value(PKvUpdateRead_Value const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateRead_Value &operator=(PKvUpdateRead_Value const &a) = delete;
    /// @brief
    /// @param a
    PKvUpdateRead_Value(PKvUpdateRead_Value &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateRead_Value &operator=(PKvUpdateRead_Value &&a) = delete;

public:
    /// @brief
    /// @return
    int32_t GetReadDataType() const noexcept final;
    /// @brief
    /// @return
    uint32_t GetReadTotalLen() const noexcept final;
    /// @brief
    void ClearReadLen() noexcept;

protected:
    /// @brief Reset Read result data
    void V_ResetRead() noexcept final;
    /// @brief
    /// @param stReadData
    /// @param nDataStart
    /// @param nDataEnd
    /// @return
    int32_t V_ParseReadData(ara::core::StringView const &stReadData,
                            ara::core::StringView::size_type &nDataStart,
                            ara::core::StringView::size_type &nDataEnd) noexcept final;
};
//***************/
/// @brief Interpret format: "K={data},"
/// @code{.isoft}
/// @unit_name=PKvUpdateRead_String
/// @interface_level=unit
/// @endcode
class PKvUpdateRead_String final : public PKvUpdateRead_Base
{
private:
    /// @brief
    int32_t nClearQac_{0};

public:
    /// @brief
    inline void ClearQac() noexcept { nClearQac_ = 1; }
    /// @brief
    /// @param nLineIndex
    /// @param eUpdateWord
    /// @param pWordEncode
    explicit PKvUpdateRead_String(int32_t const nLineIndex,
                                  EUpdateKeyWord const eUpdateWord,
                                  PWordEncode *const pWordEncode = nullptr) noexcept;
    /// @brief
    ~PKvUpdateRead_String() noexcept final = default;
    /// @brief
    /// @param a
    PKvUpdateRead_String(PKvUpdateRead_String const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateRead_String &operator=(PKvUpdateRead_String const &a) = delete;
    /// @brief
    /// @param a
    PKvUpdateRead_String(PKvUpdateRead_String &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateRead_String &operator=(PKvUpdateRead_String &&a) = delete;
    /// @brief Return the read string
    /// @return
    ara::core::String GetReadString() const;

protected:
    /// @brief
    /// @param stReadData
    /// @param nDataStart
    /// @param nDataEnd
    /// @return
    int32_t V_ParseReadData(ara::core::StringView const &stReadData,
                            ara::core::StringView::size_type &nDataStart,
                            ara::core::StringView::size_type &nDataEnd) noexcept final;
};
//***************/
/// @brief Interpret format: "[data]:"
/// @code{.isoft}
/// @unit_name=PKvUpdateRead_Action
/// @interface_level=unit
/// @endcode
class PKvUpdateRead_Action final : public PKvUpdateRead_Base
{
private:
    /// @brief
    char8_t chUpdateAction_{static_cast< char8_t >(0)};

public:
    /// @brief
    /// @param nLineIndex
    /// @param eUpdateWord
    /// @param pWordEncode
    explicit PKvUpdateRead_Action(int32_t const nLineIndex,
                                  EUpdateKeyWord const eUpdateWord,
                                  PWordEncode *const pWordEncode = nullptr) noexcept;
    /// @brief
    ~PKvUpdateRead_Action() noexcept final = default;
    /// @brief
    /// @param a
    PKvUpdateRead_Action(PKvUpdateRead_Action const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateRead_Action &operator=(PKvUpdateRead_Action const &a) = delete;
    /// @brief
    /// @param a
    PKvUpdateRead_Action(PKvUpdateRead_Action &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateRead_Action &operator=(PKvUpdateRead_Action &&a) = delete;
    /// @brief
    /// @return
    inline char8_t GetUpdateAction() const noexcept { return chUpdateAction_; }

protected:
    /// @brief
    /// @param stReadData
    /// @param nDataStart
    /// @param nDataEnd
    /// @return
    int32_t V_ParseReadData(ara::core::StringView const &stReadData,
                            ara::core::StringView::size_type &nDataStart,
                            ara::core::StringView::size_type &nDataEnd) noexcept final;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
#endif
