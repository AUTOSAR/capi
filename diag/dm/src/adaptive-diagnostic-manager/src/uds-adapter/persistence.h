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
/// @file       persistence.h
/// @brief      This file provides the Data Storage class
/// @details
/// @date       2024-11-27
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================
///
/// #ifndef _ARA_DIAG_DM_PERSISTENCE_H_
/// #define _ARA_DIAG_DM_PERSISTENCE_H_
/// #include <ara/per/key_value_storage.h>
/// #include <isoft/uds/persistence_interface.h>
/// namespace ara {
/// namespace diag {
/// namespace dmd {
/// using isoft::uds::server::PersistenceInterface;
/// class Persistence : public PersistenceInterface
/// {
/// public:
/// Persistence(Persistence const&) = default;
/// Persistence(Persistence&&)      = default;
/// Persistence& operator=(Persistence const&) = default;
/// Persistence& operator=(Persistence&&) = default;
/// Persistence(ara::per::SharedHandle< ara::per::KeyValueStorage > const& storage) : kvStorage_{storage} {}
/// ~Persistence() = default;
/// // Save data
/// bool SaveData(const std::string& key, const std::vector< std::uint8_t >& value);
/// // Load data
/// std::vector< std::uint8_t > LoadData(const std::string& key) const;
/// // Delete data
/// bool RemoveData(const std::string& key);
/// /// @param key Key value
/// /// @return true: Corresponding key exists
/// bool KeyExists(std::string const& key);
/// /// @return true: Success
/// bool SyncData();
/// // List all keys
/// std::vector< std::string > ListKeys() const;
/// private:
/// ara::per::SharedHandle< ara::per::KeyValueStorage > kvStorage_;
/// };
/// }  // namespace dmd
/// }  // namespace diag
/// }  // namespace ara
/// #endif
///
/// ================================================================
