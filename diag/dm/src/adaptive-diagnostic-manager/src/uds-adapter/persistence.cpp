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
/// @file       persistence.cpp
/// @brief      This file provides the implementation of the Data Storage class
/// @details
/// @date       2024-11-27
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================
///
/// #include "persistence.h"
/// #include "log/log.h"
/// namespace ara {
/// namespace diag {
/// namespace dmd {
/// using ara::diag::common::LogError;
/// using ara::diag::common::LogWarn;
/// bool Persistence::SaveData(const std::string& key, const std::vector< std::uint8_t >& value)
/// {
/// if (!kvStorage_.operator bool()) {
/// LogError() << "Persistence::SaveData|kvStorage is nullptr key =" << key.c_str();
/// return false;
/// }
/// ara::core::Result< void > save_res{
/// std::move(kvStorage_->SetValue(std::move(ara::core::StringView(key.c_str())), value))};
/// if (!save_res.HasValue()) {
/// LogError() << "Persistence::SaveData|save fail err"
/// << " key =" << key.c_str() << "error =" << save_res.Error().Message();
/// return false;
/// }
/// return true;
/// }
/// std::vector< std::uint8_t > Persistence::LoadData(const std::string& key) const
/// {
/// std::vector< std::uint8_t > tem;
/// if (!kvStorage_.operator bool()) {
/// LogError() << "Persistence::LoadData|kvStorage is nullptr key =" << key.c_str();
/// return tem;
/// }
/// ara::core::Result< ara::core::Vector< uint8_t > > load_res{
/// std::move(kvStorage_->GetValue< ara::core::Vector< uint8_t > >(std::move(ara::core::StringView(key.c_str()))))};
/// if (!load_res.HasValue()) {
/// LogWarn() << "Persistence::LoadData|load failed err"
/// << ", key=" << key.c_str() << "error =" << load_res.Error().Message();
/// return tem;
/// }
/// tem.insert(tem.end(), load_res.Value().begin(), load_res.Value().end());
/// return tem;
/// }
/// bool Persistence::RemoveData(const std::string& key)
/// {
/// if (!kvStorage_.operator bool()) {
/// LogError() << "Persistence::RemoveData|kvStorage is nullptr key =" << key.c_str();
/// return false;
/// }
/// ara::core::Result< void > remove_res{
/// std::move(kvStorage_->RemoveKey(std::move(ara::core::StringView(key.c_str()))))};
/// if (!remove_res.HasValue()) {
/// LogError() << "Persistence::RemoveData|remove fail err"
/// << " key =" << key.c_str() << "error =" << remove_res.Error().Message();
/// return false;
/// }
/// return true;
/// }
/// bool Persistence::KeyExists(std::string const& key)
/// {
/// if (!kvStorage_.operator bool()) {
/// LogError() << "Persistence::KeyExists|kvStorage is nullptr key =" << key.c_str();
/// return false;
/// }
/// ara::core::Result< bool > exists_res{
/// std::move(kvStorage_->KeyExists(std::move(ara::core::StringView(key.c_str()))))};
/// if (!exists_res.HasValue()) {
/// LogWarn() << "Persistence::KeyExists|KeyExists failed err"
/// << ", key=" << key.c_str() << "error =" << exists_res.Error().Message();
/// return false;
/// }
/// return exists_res.Value();
/// }
/// bool Persistence::SyncData()
/// {
/// if (!kvStorage_.operator bool()) {
/// LogError() << "Persistence::SyncData|kvStorage is nullptr";
/// return false;
/// }
/// ara::core::Result< void > sync_res{std::move(kvStorage_->SyncToStorage())};
/// if (!sync_res.HasValue()) {
/// LogError() << "Persistence::SyncData|sync fail error =" << sync_res.Error().Message();
/// return false;
/// }
/// return true;
/// }
/// std::vector< std::string > Persistence::ListKeys() const
/// {
/// std::vector< std::string > tem;
/// if (!kvStorage_.operator bool()) {
/// LogError() << "Persistence::ListKeys|kvStorage is nullptr";
/// return tem;
/// }
/// ara::core::Result< ara::core::Vector< ara::core::String > > load_all__res{std::move(kvStorage_->GetAllKeys())};
/// if (!load_all__res.HasValue()) {
/// LogWarn() << "Persistence::ListKeys|no key storaged";
/// return tem;
/// }
/// for (auto& key : load_all__res.Value()) {
/// tem.emplace_back(std::move(std::string(key.c_str())));
/// }
/// return tem;
/// }
/// }  // namespace dmd
/// }  // namespace diag
/// }  // namespace ara
///
/// ================================================================
