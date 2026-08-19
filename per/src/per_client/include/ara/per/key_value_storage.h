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
/// @file       key_value_storage.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Key-Value storage
/// @date       2021-06-09
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-06-09  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// <tr><td>2021-08-30  <td>1.0.0    <td>hanjingjing      <td>Integrate PHKV implementation into AutoSar interface
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KeyValueStorage/KV Storage Interface
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=KV storage library interface class
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_KEY_VALUE_STORAGE_H_
#define ARA_PER_KEY_VALUE_STORAGE_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/span.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/serialize/serialize.h>

#include <memory>

#include "ara/per/internal/common/isoft_auto_buff.h"
#include "ara/per/internal/common/isoft_per_common_api.h"
#include "ara/per/internal/initialize.h"
#include "ara/per/internal/isoftkv/kv_data_type.h"
#include "ara/per/shared_handle.h"

namespace ara {
namespace per {
namespace isoftkv {
/// @brief Predefinition of KV engine
class PKvSystem;
/// @brief Predefinition of KV configuration
class PConfigMuster_Kv;
/// @brief
struct PPerVersion;
}  // namespace isoftkv
//********************************/
/// @brief Kv library operation class: Key-value store contains a set of keys with associated values.
/// @code{.isoft}
/// @unit_name=KeyValueStorage
/// @traceid {SWS_PER_00339}{SWS_PER_00331}
/// @endcode
/// @brief The Key-Value Storage contains a set of keys with associated values.
class KeyValueStorage
{
public:
    // Can use friend function or public static member
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00459}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Default constructor for KeyValueStorage.
    KeyValueStorage() = delete;

protected:
    /// @brief Constructor initializing KvStorage using configuration
    /// @param[in] kvConfig
    explicit KeyValueStorage(isoftkv::PConfigMuster_Kv const &kvConfig) noexcept;
    /// @brief Constructor initializing KvStorage using Kv storage engine object
    /// @param[in] pKvSystem
    explicit KeyValueStorage(std::unique_ptr< isoftkv::PKvSystem > pKvSystem) noexcept;

private:
    /// @brief
    std::unique_ptr< isoftkv::PKvSystem > pKvSystem_;

public:
    /// @brief Move constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00322}
    /// @threadsafety {re-entran}
    /// @endcode
    /// @brief Move constructor for KeyValueStorage.
    /// @param[in] kvs The KeyValueStorage object to be moved.
    KeyValueStorage(KeyValueStorage &&kvs) noexcept;
    /// @brief Copy constructor: Disabled
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00324}
    /// @endcode
    /// @brief The copy constructor for KeyValueStorage shall not be used.
    /// @param[in] a
    KeyValueStorage(KeyValueStorage const &a) = delete;
    /// @brief Move assignment function
    /// @code{.isoft}
    /// export_level=/Per
    /// @threadsafety {re-entran}
    /// @traceid {SWS_PER_00323}
    /// @endcode
    /// @brief Move assignment operator for KeyValueStorage.
    /// @param[in] kvs The KeyValueStorage object to be moved.
    /// @returns The moved KeyValueStorage object.
    KeyValueStorage &operator=(KeyValueStorage &&kvs) &noexcept;
    /// @brief Copy assignment function: Disabled
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00325}
    /// @endcode
    /// @brief The copy assignment operator for KeyValueStorage shall not be used.
    /// @param[in] a
    /// @return
    KeyValueStorage &operator=(KeyValueStorage const &a) = delete;
    /// @brief Destructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00050}
    /// @endcode
    /// @brief Destructor for KeyValueStorage.
    virtual ~KeyValueStorage() noexcept;

public:
    /// @brief Return snapshot of all Keys at the time of calling
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00042}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::String > >
    /// @brief Returns a list of all currently available keys of this Key-Value Storage.
    /// The list of keys is only accurate if no key is added or deleted at the same time.
    /// @returns A Result containing a list of available keys.
    /// In caseof an error, it contains any of the errors definedbelow, or a vendor specific error.
    GetAllKeys() const noexcept;
    /// @brief Check if Key exists
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00043}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< bool >
    /// @brief Checks if a key exists in this Key-Value Storage.
    /// The result is only accurate if no key is added or deleted at the same time.
    /// E.g. when a key isremoved in another thread directly after this function returned "true",
    /// the result is not validanymore
    /// @param[in] stKey The key that shall be checked.
    /// @returns A Result containing true if the key could be located or false if it couldn’t.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    KeyExists(ara::core::StringView const stKey) const noexcept;
    /// @brief Return size of value assigned to this key (in bytes).
    // GetCurrentValueSize may be delayed due to another thread performing RemoveAll
    // Keys or DiscardPendingChanges calls, or deferred due to SetValue, RemoveKey, RecoverKey, or ResetKey calls on the same key-value pair.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00554}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< std::uint64_t >
    /// @brief Returns the size (in bytes) of the value assigned to a key of this Key-Value Storage.
    // GetCurrentValueSize may be delayed by an ongoing call from another thread to RemoveAll Keys or
    // DiscardPendingChanges, or to SetValue, RemoveKey, RecoverKey, or ResetKey for the same key-value pair.
    /// @param key The key to look up.
    /// @return A Result containing the size of the value in bytes. In case of an error, it contains any of the errors
    /// defined below, or a vendor specific error.
    /// PerErrc::kKeyNotFound Returned if the provided key does not exist in the Key-Value Storage.
    /// PerErrc::kPhysicalStorageFailure Returned if access to the physical storage fails.
    /// PerErrc::kIntegrityCorrupted Returned if stored data cannot be read because the structural integrity is
    /// corrupted. PerErrc::kValidationFailed Returned if the validity of stored data cannot be ensured.
    /// PerErrc::kEncryptionFailed Returned if the decryption of stored data fails.
    /// PerErrc::kAuthenticationFailed Returned if checking the MAC of stored data fails.
    GetCurrentValueSize(ara::core::StringView key) const noexcept;
    /// @brief Get Value corresponding to Key
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00332}
    /// @threadsafety {re-entrant}
    /// @endcode
    template < class T >
    ara::core::Result< T >
    /// @brief Returns the value assigned to a key of this Key-Value Storage.
    /// GetValue may be delayed by an ongoing call from another thread to RemoveAllKeys or DiscardPendingChanges,
    /// or to SetValue, RemoveKey, RecoverKey, or ResetKey for the same key.
    /// @param[in] stKey The key to look up.
    /// @returns A Result containing the retr ieved value. In case of an error,
    /// it contains any of the errors defined below, or a vendor specific error.
    GetValue(ara::core::StringView const stKey) const noexcept
    {
        if (false == isoftkv::IsPerInitialize()) {
            return std::move(ara::core::Result< T >::FromError(PerErrc::kNotInitialized));
        }
        return std::move(_GetValueImpl< T >(stKey));
    }
    /// @brief Return value assigned to this KeyValueStorage. This method should only be used for repeated access to very large values.
    /// GetValue may be delayed due to another thread performing RemoveAllKeys or DiscardPendingChanges, or SetValue, RemoveKey, RecoverKey, or ResetKey calls on the same key-value pair.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00044}
    /// @threadsafety {re-entrant}
    /// @endcode
    template < class T >
    ara::core::Result< void >
    /// @brief Returns the value assigned to a key of this KeyValueStorage.
    /// This method should only be used to access very large values repeatedly.
    /// GetValue may be delayed by an ongoing call from another thread to RemoveAllKeys or DiscardPendingChanges, or to
    /// SetValue, RemoveKey, RecoverKey, or ResetKey for the same key-value pair.
    /// @param[in] stKey The key to look up.
    /// @param[out] value The retrieved value.
    /// @returns A Result of void. In case of an error, it contains any of the errors defined below, or a vendor
    ///     specific error.
    /// PerErrc::kKeyNotFound Returned if the provided key does not exist in the Key-Value Storage.
    /// PerErrc::kPhysicalStorageFailure Returned if access to the physical storage fails.
    /// PerErrc::kIntegrityCorrupted Returned if stored data cannot be read because the structural integrity is
    ///     corrupted.
    /// PerErrc::kValidationFailed Returned if the validity of stored data cannot be ensured.
    /// PerErrc::kEncryptionFailed Returned if the decryption of stored data fails.
    /// PerErrc::kDataTypeMismatch Returned if the data type of stored value does not match the templated type.
    /// PerErrc::kAuthenticationFailed Returned if checking the MAC of stored data fails.
    GetValue(ara::core::StringView const stKey, T &value) const noexcept
    {
        ara::core::Result< T > const result{GetValue< T >(stKey)};
        if (false == result.HasValue()) {
            return std::move(isoftkv::T_ErrorResult< T, void >(result));
        }
        value = std::move(std::move(result).Value());
        return ara::core::Result< void >::FromValue();
    }
    /// @brief Set Value for Key
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00046}
    /// @threadsafety {re-entrant}
    /// @endcode
    template < class T >
    ara::core::Result< void >
    /// @brief Stores a key in this Key-Value Storage.
    /// If a value already exists and has the same data type as the new value, it is overwritten.
    /// If the new value has a different data type than the stored value, kDataTypeMismatch is returned.
    /// SetValue may be delayed by an ongoing call from another thread
    /// to RemoveAllKeys, SyncToStorage, or DiscardPendingChanges,
    /// or to SetValue, GetValue, RemoveKey, RecoverKey, orResetKey for the same key.
    /// @param[in] stKey  The key to assign the value to.
    /// @param[in] value  The value to store.
    /// @returns A Result of void.
    /// In case of an error, it contains anyof the errors defined below, or a vendor specific error.
    SetValue(ara::core::StringView const stKey, T const &value) noexcept
    {
        if (false == isoftkv::IsPerInitialize()) {
            return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
        }
        return std::move(_SetValueImpl< T >(stKey, value));
    }
    /// @brief Delete Key and Value
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00047}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< void >
    /// @brief Removes a key and the associated value from this Key-Value Storage.
    /// RemoveKey may be delayed by an ongoing call from another thread
    /// to RemoveAllKeys, SyncToStorage, or DiscardPendingChanges,
    /// or to SetValue, GetValue, RemoveKey, RecoverKey, orResetKey for the same key.
    /// @param[in] stKey  The key to be removed.
    /// @returns A Result of void. In case of an error, it contains anyof the errors defined below,
    /// or a vendor specific error.
    RemoveKey(ara::core::StringView const stKey) noexcept;
    /// @brief Recover Value corresponding to Key
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00427}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< void >
    /// @brief Recovers a single key of this Key Value Storage.
    /// This method allows to recover a single key when the redundancy checks fail.
    /// This method does a best-effort recovery of the key.
    /// After recovery, the key might contain outdated or initial content, or might be lost.
    /// RecoverKey may be delayed by an ongoing call from another thread
    /// to RemoveAllKeys, SyncToStorage, or DiscardPendingChanges,
    /// or to SetValue, GetValue, RemoveKey, RecoverKey, or ResetKey for the same key.
    /// @param[in] stKey  The key to be recovered.
    /// @returns A Result of void.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    RecoverKey(ara::core::StringView const stKey) noexcept;
    /// @brief Reset Value corresponding to Key
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00426}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< void >
    /// @brief Resets a key of this Key-Value Storage to its initial value.
    /// This method allows to reset a single key to its initial value.
    /// If the key is currently not available in the Key-Value Storage, it is re-created.
    /// ResetKey will fail with kInitValueNotAvailable when design
    /// and deployment do not define aninitial value for the key.
    /// ResetKey may be delayed by an ongoing call from another thread
    /// to RemoveAllKeys, SyncToStorage, or DiscardPendingChanges,
    /// or to SetValue, GetValue, RemoveKey, RecoverKey, or ResetKey for the same key.
    /// @param[in] stKey  The key to be reset.
    /// @returns A Result of void.
    /// In case of an error, it contains anyof the errors defined below, or a vendor specific error.
    ResetKey(ara::core::StringView const stKey) noexcept;
    /// @brief Delete all Keys in this center
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00048}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< void >
    /// @brief Removes all keys and associated values from this Key-Value Storage.
    /// RemoveAllKeys may be delayed by an ongoing call from another thread
    /// to RemoveAllKeys, SyncToStorage, DiscardPendingChanges, SetValue, GetValue, RemoveKey, RecoverKey,
    /// or ResetKey.
    /// @returns A Result of void.
    /// In case of an error, it contains anyof the errors defined below, or a vendor specific error.
    RemoveAllKeys() noexcept;
    /// @brief Save memory data to persistent storage device
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00049}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< void >
    /// @returns A Result of void.
    /// In case of an error, it contains anyof the errors defined below, or a vendor specific error.
    /// @brief
    /// Triggers flushing of changed key-value pairs of the Key-Value Storage to the physical storage.
    /// SyncToStorage may be delayed by an ongoing call from another thread
    /// to RemoveAllKeys, DiscardPendingChanges, SetValue, RemoveKey, RecoverKey, or ResetKey.
    SyncToStorage() noexcept;
    /// @brief Discard all modifications in current memory
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00365}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< void >
    /// @brief Removes all pending changes to this Key-Value Storage since the last
    /// call to SyncToStorage() or since this Key-Value Storage was opened using OpenKeyValueStorage().
    /// DiscardPendingChanges may be delayed by an ongoing call from another thread
    /// to RemoveAllKeys, SyncToStorage, DiscardPendingChanges, SetValue, GetValue, RemoveKey, RecoverKey,
    /// or ResetKey.
    /// @returns A Result of void.
    /// In case of an error, it contains anyof the errors defined below, or a vendor specific error.
    DiscardPendingChanges() noexcept;

public:
    /// @brief Initialize FileStorage using configuration in Manifest
    /// @param[in] portIns The shortName path of a PortPrototype typed by aPersistencyFileStorageInterface.
    /// @param[in] appVersion
    /// @param[in] perVersion
    /// @returns A Result of void.
    ara::core::Result< void > InitStorage(ara::core::InstanceSpecifier const &portIns, bool bNewBuild) const noexcept;
    /// @brief Get internal Kv engine
    /// @returns
    isoftkv::PKvSystem *GetKvSystem() const noexcept;
    /// @brief Backup current library
    /// @returns
    ara::core::Result< bool > BackupMain() const noexcept;
    /// @brief Delete main library
    /// @returns
    ara::core::Result< bool > RemoveMain() const noexcept;
    /// @brief Recover current library using backup library
    /// @returns
    ara::core::Result< bool > RecoverFromBackup() const noexcept;
    /// @brief Delete backup library
    /// @returns
    ara::core::Result< bool > RemoveBackup() const noexcept;
    /// @brief Get total space occupied by all files in bytes
    /// @threadsafety {re-entrant}
    ara::core::Result< uint64_t >
    /// @brief May correspond to the Persistency File.fileName of a configured file.
    /// @returns A Result containing the occupied space in bytes.
    GetStorageSpace() const noexcept;

protected:
    /// @brief Automatically repair errors using M/N redundancy
    /// @param[in] stKey Key involved in recovery operation
    /// @return
    ara::core::Result< void > _AutoRecover(ara::core::StringView const &stKey) const noexcept;
    /// @brief Encapsulated read: Binary
    /// @param[in] stKey
    /// @param[in] pBuff
    /// @param[in] nBufLen
    /// @return Actual length of data read
    ara::core::Result< int32_t > _ReadValueBinary(ara::core::StringView const &stKey,
                                                  uint8_t *const pBuff,
                                                  uint32_t const nBufLen) const noexcept;
    /// @brief Encapsulated read: String
    /// @param[in] stKey
    /// @return
    ara::core::Result< ara::core::String > _ReadValueString(ara::core::StringView const &stKey) const noexcept;
    /// @brief Encapsulated read: Value length
    /// @param[in] stKey
    /// @return
    ara::core::Result< int32_t > _ReadValueLength(ara::core::StringView const &stKey) const noexcept;
    /// @brief Encapsulated write: Binary
    /// @param[in] stKey
    /// @param[in] pBValue
    /// @param[in] nValueLen
    /// @return
    ara::core::Result< void > _WriteValueBinary(ara::core::StringView const &stKey,
                                                uint8_t const *const pBValue,
                                                uint32_t const nValueLen) const noexcept;
    /// @brief Encapsulated read: Supports basic types like int, uint32_t, int64_t, uint64_t, float, double
    /// @tparam T
    /// @param[in] stKey
    /// @return
    /// @throws
    template < typename T >
    ara::core::Result< T > _ReadValue(ara::core::StringView const &stKey) const noexcept;
    /// @brief Encapsulated write: Supports basic types like int, uint32_t, int64_t, uint64_t, float, double, and ara::core::String type
    /// @tparam T
    /// @param[in] stKey
    /// @param[in] value
    /// @return
    /// @throws
    template < typename T >
    ara::core::Result< void > _WriteValue(ara::core::StringView const &stKey, T const &value) const noexcept;

    // Basic data types
protected:
    /// @brief GetValue implementation, basic data types
    /// @tparam T
    /// @param[in] stKey
    /// @param[in] p
    /// @return
    template < typename T >
    ara::core::Result< T > _GetValueImpl(ara::core::StringView const stKey,
                                         std::enable_if_t< std::is_scalar< T >::value > *const p
                                         = nullptr) const noexcept
    {
        std::ignore = p;
        return _ReadValue< T >(stKey);
    }
    /// @brief SetValue implementation, basic data types and ara::core::String type
    /// @tparam T
    /// @param[in] stKey
    /// @param[in] value
    /// @param[in] p
    /// @return
    template < typename T >
    ara::core::Result< void > _SetValueImpl(
        ara::core::StringView const &stKey,
        T const &value,
        std::enable_if_t< (std::is_scalar< T >::value) || (std::is_same< T, ara::core::String >::value) > *const p
        = nullptr) const noexcept
    {
        std::ignore = p;
        return _WriteValue< T >(stKey, value);
    }

protected:
    /// @brief GetValue implementation, ara::core::String type
    /// @tparam T
    /// @param[in] stKey
    /// @param[in] p
    /// @return
    template < typename T >
    ara::core::Result< T > _GetValueImpl(ara::core::StringView const stKey,
                                         std::enable_if_t< std::is_same< T, ara::core::String >::value > *p
                                         = nullptr) const noexcept
    {
        std::ignore = p;
        return _ReadValueString(stKey);
    }

    // Support reading/writing ara::core::Array
protected:
    /// @brief Check if type is ara::core::Array
    /// @tparam T
    template < class T >
    struct IsArray final : std::false_type
    {
    };
    /// @brief Check if type is ara::core::Array
    /// @tparam T
    /// @tparam nSize
    template < class T, std::size_t nSize >
    struct IsArray< ara::core::Array< T, nSize > > final : std::true_type
    {
    };
    /// @brief GetValue implementation, ara::core::Array<T, int32_t> type array
    /// @tparam T
    /// @param[in] stKey
    /// @param[in] p
    /// @return
    template < class T >
    ara::core::Result< T > _GetValueImpl(ara::core::StringView const stKey,
                                         std::enable_if_t< IsArray< T >::value > *p = nullptr) const noexcept
    {
        std::ignore = p;
        T arrayData;
        uint32_t const nArraySize{static_cast< uint32_t >(arrayData.size())};
        uint32_t const nArrayLenInByte{static_cast< uint32_t >(nArraySize * sizeof(typename T::value_type))};
        // TODO(hangjingjing) If objects in array are non-simple types, not supported; need to read elements here with for loop
        ara::core::Result< int32_t > const resultRead{_ReadValueBinary(
            stKey, isoftkv::T_TransBytes< typename T::value_type >(arrayData.data()), nArrayLenInByte)};
        if (false == resultRead.HasValue()) {
            return std::move(isoftkv::T_ErrorResult< int32_t, T >(resultRead));
        }
        return std::move(ara::core::Result< T >::FromValue(arrayData));
    }
    /// @brief SetValue implementation, ara::core::Array<T, int32_t> type array
    /// @tparam T
    /// @param[in] stKey
    /// @param[in] value
    /// @param[in] p
    /// @return
    template < class T >
    ara::core::Result< void > _SetValueImpl(ara::core::StringView const &stKey,
                                            T const &value,
                                            std::enable_if_t< IsArray< T >::value > *p = nullptr) const noexcept
    {
        std::ignore = p;
        // TODO(hanjingjing) If objects in array are non-simple types, not supported; need to read elements here with for loop
        return _WriteValueBinary(stKey, isoftkv::T_TransBytes(value.data()),
                                 static_cast< uint32_t >(value.size() * sizeof(typename T::value_type)));
    }
    // Support reading/writing ara::core::Vector
protected:
    /// @brief Check if type is ara::core::Vector
    /// @tparam T
    template < typename T >
    struct IsVector final : std::false_type
    {
    };
    /// @brief Check if type is ara::core::Vector
    /// @tparam T
    /// @tparam T_Allocator
    template < typename T, typename T_Allocator >
    struct IsVector< ara::core::Vector< T, T_Allocator > > final : std::true_type
    {
    };
    /// @brief GetValue implementation, ara::core::Vector<T, Allocator> type array
    /// @tparam T
    /// @param[in] stKey
    /// @param[in] p
    /// @return
    template < typename T >
    ara::core::Result< T > _GetValueImpl(ara::core::StringView const stKey,
                                         std::enable_if_t< IsVector< T >::value > *p = nullptr) const noexcept
    {
        std::ignore = p;
        ara::core::Result< int32_t > const resultValueLen{_ReadValueLength(stKey)};
        if (false == resultValueLen.HasValue()) {
            return std::move(isoftkv::T_ErrorResult< int32_t, T >(resultValueLen));
        }
        T vecData;
        int32_t const nValueLen{resultValueLen.Value()};
        if (0 == nValueLen) {
            return std::move(ara::core::Result< T >::FromValue(std::move(vecData)));
        }
        isoftkv::PAutoBuff const autoBuff{static_cast< uint32_t >(nValueLen)};
        ara::core::Result< int32_t > const resultRead{
            _ReadValueBinary(stKey, autoBuff.data(), static_cast< uint32_t >(nValueLen))};
        if (false == resultRead.HasValue()) {
            return std::move(isoftkv::T_ErrorResult< int32_t, T >(resultRead));
        }
        int32_t nReadTotal{0};
        vecData.reserve(static_cast< typename T::size_type >(
            ara::per::isoftkv::TSerialize::GetVectorLength< typename T::value_type >(nValueLen)));
        while (nReadTotal < nValueLen) {
            typename T::value_type data;
            int32_t const nReadLen{
                static_cast< int32_t >(ara::per::isoftkv::TSerialize::ReadData< typename T::value_type >(
                    data, autoBuff.data(static_cast< uint32_t >(nReadTotal))))};
            if (nReadLen <= 0) {
                break;
            }
            nReadTotal += nReadLen;
            vecData.push_back(data);
        }
        return std::move(ara::core::Result< T >::FromValue(std::move(vecData)));
    }
    /// @brief SetValue implementation, ara::core::Vector<T, Allocator> type array
    /// @tparam T
    /// @param[in] stKey
    /// @param[in] value
    /// @param[in] p
    /// @return
    template < typename T >
    ara::core::Result< void > _SetValueImpl(ara::core::StringView const &stKey,
                                            T const &value,
                                            std::enable_if_t< IsVector< T >::value > *p = nullptr) const noexcept
    {
        std::ignore = p;
        uint32_t nTotalLen{0U};
        for (typename T::value_type const &itData : value) {
            nTotalLen += ara::per::isoftkv::TSerialize::WriteData(itData, nullptr);
        }
        isoftkv::PAutoBuff const autoBuff{nTotalLen};
        uint32_t nWriteLen{0U};
        for (typename T::value_type const &itData : value) {
            nWriteLen += ara::per::isoftkv::TSerialize::WriteData(itData, autoBuff.data(nWriteLen));
        }
        return _WriteValueBinary(stKey, autoBuff.data(), nWriteLen);
    }
    // Support writing ara::core::Span, reading using ara::core::Vector
protected:
    /// @brief Check if type is ara::core::Span
    /// @tparam T
    template < typename T >
    struct IsSpan final : std::false_type
    {
    };
    /// @brief Check if type is ara::core::Span
    /// @tparam T
    /// @tparam nExtent
    template < typename T, std::size_t nExtent >
    struct IsSpan< ara::core::Span< T, nExtent > > final : std::true_type
    {
    };
    /// @brief SetValue implementation, ara::core::ara::core::Span<T, nExtent> memory
    /// @tparam T
    /// @param[in] stKey
    /// @param[in] value
    /// @param[in] p
    /// @return
    template < typename T >
    ara::core::Result< void > _SetValueImpl(ara::core::StringView const &stKey,
                                            T const &value,
                                            std::enable_if_t< IsSpan< T >::value > *p = nullptr) const noexcept
    {
        std::ignore = p;
        uint32_t nTotalLen{0U};
        for (typename T::value_type const &itData : value) {
            nTotalLen += ara::per::isoftkv::TSerialize::WriteData(itData, nullptr);
        }
        isoftkv::PAutoBuff const autoBuff{nTotalLen};
        uint32_t nWriteLen{0U};
        for (typename T::value_type const &itData : value) {
            nWriteLen += ara::per::isoftkv::TSerialize::WriteData(itData, autoBuff.data(nWriteLen));
        }
        return _WriteValueBinary(stKey, autoBuff.data(), nWriteLen);
    }

    // TODO(hanjingjing) Use common serialization (conversion between arbitrary types and bytes e.g. struct)
protected:
    /// @brief Check if type is CppDataType exported by ara::arxml
    template < typename T >
    using IsCppDataType
        = std::conditional_t< !((isoft::serialize::IsBase< T >::value) || (isoft::serialize::IsString< T >::value)
                                || (IsArray< T >::value) || (IsVector< T >::value) || (IsSpan< T >::value)),
                              std::true_type,
                              std::false_type >;
    /// @brief GetValue implementation, CppDataType type
    /// @tparam T
    /// @param[in] stKey
    /// @param[in] p
    /// @return
    template < typename T >
    ara::core::Result< T > _GetValueImpl(ara::core::StringView const stKey,
                                         std::enable_if_t< IsCppDataType< T >::value > *p = nullptr) const noexcept
    {
        std::ignore = p;
        ara::core::Result< int32_t > const resultValueLen{_ReadValueLength(stKey)};
        if (false == resultValueLen.HasValue()) {
            return std::move(isoftkv::T_ErrorResult< int32_t, T >(resultValueLen));
        }
        T cppData;
        int32_t const nValueLen{resultValueLen.Value()};
        if (0 == nValueLen) {
            return std::move(ara::core::Result< T >::FromValue(std::move(cppData)));
        }
        isoftkv::PAutoBuff autoBuff{static_cast< uint32_t >(nValueLen)};
        ara::core::Result< int32_t > const resultRead{
            _ReadValueBinary(stKey, autoBuff.data(), static_cast< uint32_t >(autoBuff.GetBuffLen()))};
        if (false == resultRead.HasValue()) {
            return std::move(isoftkv::T_ErrorResult< int32_t, T >(resultRead));
        }
        autoBuff.SetDataLen(resultRead.Value());
        isoft::serialize::StringBorrow payLoad{isoftkv::T_TransChar(autoBuff.data()),
                                               static_cast< std::size_t >(autoBuff.size())};
        isoft::serialize::Buffer< isoft::serialize::StringBorrow > buffer{payLoad};
        isoft::serialize::Buffer< isoft::serialize::StringBorrow >::result_t resultDeserialize{
            isoft::serialize::Deserialize(buffer, cppData)};
        if (resultDeserialize <= 0) {
            return std::move(ara::core::Result< T >::FromError(PerErrc::kDataTypeMismatch));
        }
        return std::move(ara::core::Result< T >::FromValue(std::move(cppData)));
    }
    /// @brief SetValue implementation, CPP data type generated by toolchain
    /// @tparam T
    /// @param[in] stKey
    /// @param[in] value
    /// @param[in] p
    /// @return
    template < typename T >
    ara::core::Result< void > _SetValueImpl(ara::core::StringView const &stKey,
                                            T const &value,
                                            std::enable_if_t< IsCppDataType< T >::value > *p = nullptr) const noexcept
    {
        std::ignore = p;
        ara::core::Vector< uint8_t > payLoad;
        isoft::serialize::Buffer< ara::core::Vector< uint8_t > > buffer{payLoad};
        isoft::serialize::Buffer< ara::core::Vector< uint8_t > >::result_t resultSerialize{
            isoft::serialize::Serialize(buffer, value)};
        if (resultSerialize <= 0) {
            return ara::core::Result< void >::FromError(PerErrc::kDataTypeMismatch);
        }
        return _WriteValueBinary(stKey, payLoad.data(), payLoad.size());
    }
};
//********************************/
/// @brief Open/create a Key-Value data center
/// @code{.isoft}
/// @unit_name=OpenKeyValueStorage
/// @traceid {SWS_PER_00052}
/// @threadsafety {re-entrant}
/// @endcode
ara::core::Result< SharedHandle< KeyValueStorage > >
/// @brief Opens a Key-Value Storage.OpenKeyValueStorage will fail with kResourceBusy
/// when the Key-Value Storage is currently being modified by a call from another thread
/// to UpdatePersistency, ResetPersistency, Recover KeyValueStorage, or ResetKeyValueStorage.
/// @param[in] kvs The shortName path of a PortPrototype typed by a PersistencyKeyValueStorageInterface.
/// @returns A Result containing a SharedHandle for the KeyValueStorage.
/// In case of an error, it contains any of the errors defined below, or a vendor specific error.
OpenKeyValueStorage(ara::core::InstanceSpecifier const &kvs) noexcept;
/// @brief Recover a Key-Value data center
/// @code{.isoft}
/// @unit_name=RecoverKeyValueStorage
/// @traceid {SWS_PER_00333}
/// @threadsafety {re-entrant}
/// @endcode
/// @brief Recovers a Key-Value Storage.
/// RecoverKeyValueStorage allows to recover a key-value storage when the redundancy checks fail.
/// It will fail with kResourceBusy when the Key-Value Storage is currently open, or when it is modified by a call from
/// another thread to UpdatePersistency, ResetPersistency, RecoverKeyValueStorage, or ResetKeyValueStorage. This method
/// does a best-effort recovery of all keys. After recovery, keys might show outdated or initial value, or might be
/// lost.
/// @param[in] kvs The shortName path of a PortPrototype typed by a PersistencyKeyValueStorageInterface.
/// @returns A Result of void.
/// In case of an error, it contains anyof the errors defined below, or a vendor specific error.
ara::core::Result< void > RecoverKeyValueStorage(ara::core::InstanceSpecifier const &kvs) noexcept;
/// @brief Reset a Key-Value data center
/// @code{.isoft}
/// @unit_name=ResetKeyValueStorage
/// @traceid {SWS_PER_00334}
/// @threadsafety {re-entrant}
/// @endcode
/// @brief Resets a Key-Value Storage to the initial state.
/// ResetKeyValueStorage allows to reset a Key-Value Storage to the initial state, containing only keys which were
/// deployed from the manifest, with their initial values. It will fail with kResourceBusy when the Key-Value Storage is
/// currently open, or when it ismodified by a call from another thread to UpdatePersistency, ResetPersistency,
/// RecoverKeyValueStorage, or ResetKeyValueStorage.
/// @param[in] kvs The shortName path of a PortPrototype typed by a PersistencyKeyValueStorageInterface.
/// @returns
/// A Result of void. In case of an error, it contains any of the errors defined below,
/// or a vendor specific error.
ara::core::Result< void > ResetKeyValueStorage(ara::core::InstanceSpecifier const &kvs) noexcept;
/// @brief Return storage space occupied by current Key-Value data center in bytes (including redundancy and backup size)
/// @code{.isoft}
/// @unit_name=GetCurrentKeyValueStorageSize
/// @traceid {SWS_PER_00405}
/// @threadsafety {re-entrant}
/// @endcode
/// @brief Returns the space in bytes currently occupied by a Key-Value Storage.
/// The returned size includes all meta data and the space used for redundancy and backups.
/// The returned size is only accurate if no other operation
/// on the Key-Value Storage takes place atthe same time.
/// @param[in] kvs The shortName path of a PortPrototype typed by a PersistencyKeyValueStorageInterface
/// @returns A Result containing the occupied space in bytes.
/// Incase of an error, it contains any of the errors definedbelow, or a vendor specific error.
ara::core::Result< uint64_t > GetCurrentKeyValueStorageSize(ara::core::InstanceSpecifier const &kvs) noexcept;
/// @brief Delete a Key-Value data center: Internal interface 2023-08-11
/// @code{.isoft}
/// @unit_name=RemoveKeyValueStorage
/// @endcode
/// @param[in] kvs The shortName path of a PortPrototype typed by a PersistencyKeyValueStorageInterface.
/// @returns
/// A Result of void. In case of an error, it contains any of the errors defined below,
/// or a vendor specific error.
ara::core::Result< void > RemoveKeyValueStorage(ara::core::InstanceSpecifier const &kvs) noexcept;
//********************************/
}  // namespace per
}  // namespace ara

#endif
