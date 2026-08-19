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
/// @file       synchronized_storage.h
/// @brief      Definition of the synchronized storage class
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/Utils
/// @module_path=/UCM/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=SynchronizedStorage
/// @unit_description=Definition of the synchronized storage class
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_SYNCHRONIZED_STORAGE_H_
#define ARA_UCM_PKGMGR_SYNCHRONIZED_STORAGE_H_

#include "ara/com/types.h"
#include "common/alias.h"
#include "common/const.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief This class is a synchronized map
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00075, 9850f64690a31d70dc9d3efef9f15f7b6fa67469}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10324
/// @trace_id_dd=DD_UCM_10796
/// @needwork = ad
/// @endcode
template < typename Key, typename ValueType >
class SynchronizedStorage
{
public:
    /// @brief Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Value = std::shared_ptr< ValueType >;

    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10797
    /// @needwork = dda
    /// @endcode
    SynchronizedStorage() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10798
    /// @needwork = dda
    /// @endcode
    virtual ~SynchronizedStorage() noexcept = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10799
    /// @needwork = dda
    /// @endcode
    SynchronizedStorage(SynchronizedStorage const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10800
    /// @needwork = dda
    /// @endcode
    SynchronizedStorage& operator=(SynchronizedStorage const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10801
    /// @needwork = dda
    /// @endcode
    SynchronizedStorage(SynchronizedStorage&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10802
    /// @needwork = dda
    /// @endcode
    SynchronizedStorage& operator=(SynchronizedStorage&& other) = delete;

    /// @brief add item to the container
    ///
    /// @param id Key where to insert a new value
    /// @param data Value to insert
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10803
    /// @needwork = dda
    /// @endcode
    void AddItem(Key const& id, Value data);

    /// @brief Get item from the container assotiated with the key
    ///
    /// @param id identifier of an item to get
    ///
    /// @return Value or nullptr in case of an error
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10804
    /// @needwork = dda
    /// @endcode
    inline virtual Value GetItem(Key const& id) const;

    /// @brief Create and store new item
    ///
    /// @param factory Item factory to use
    /// @param generator
    ///
    /// @return Newly inserted pair or error
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10805
    /// @needwork = dda
    /// @endcode
    template < typename Factory, typename Generator >
    inline AraOptional< std::pair< Key, Value > > CreateAndStoreItem(Factory& factory, Generator& generator);

    /// @brief delete item from the container
    ///
    /// @param id Key of a value to delete
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10806
    /// @needwork = dda
    /// @endcode
    inline void DeleteItem(Key const& id);

    /// @brief Apply function to every item in caontainer
    ///
    /// @param processor Function to apply to key-value pairs
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10807
    /// @needwork = dda
    /// @endcode
    inline void IterateItems(std::function< void(std::pair< Key, Value const& > const) > const processor) const;

    /// @brief Create and store new item
    ///
    /// @param factory Item factory to use
    /// @param id Key of a value to Create
    ///
    /// @return Newly inserted pair or error
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10808
    /// @needwork = dda
    /// @endcode
    template < typename Factory >
    inline AraOptional< std::pair< Key, Value > > CreateAndStoreItemWithKey(Factory& factory, Key const& id);

private:
    /// @brief _nonBlockingAddItem
    /// @param id
    /// @param data
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10809
    /// @needwork = dda
    /// @endcode
    inline void _nonBlockingAddItem(Key const& id, Value const data);

private:
    /// @brief access_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10810
    /// @needwork = dda
    /// @endcode
    mutable std::mutex access_;  // instead of booost::shared_mutex//////
    /// @brief data_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10811
    /// @needwork = dda
    /// @endcode
    AraMap< Key, Value > data_{};
};

/// @brief TValue
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
template < typename Key, typename ValueType >
using TValue = typename SynchronizedStorage< Key, ValueType >::Value;

/// @brief add item to the container
///
/// @param id Key where to insert a new value
/// @param data Value to insert
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00000
/// @trace_id_dd=DD_UCM_10812
/// @needwork = dda
/// @endcode
template < typename Key, typename ValueType >
void SynchronizedStorage< Key, ValueType >::AddItem(Key const& id, SynchronizedStorage< Key, ValueType >::Value data)
{
    std::unique_lock< std::mutex > const lock{access_};  // instead of booost::unique_lock<booost::shared_mutex>//////
    _nonBlockingAddItem(id, data);
}

/// @brief Get item from the container assotiated with the key
///
/// @param id identifier of an item to get
///
/// @return Value or nullptr in case of an error
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00000
/// @trace_id_dd=DD_UCM_10813
/// @needwork = dda
/// @endcode
template < typename Key, typename ValueType >
inline typename SynchronizedStorage< Key, ValueType >::Value SynchronizedStorage< Key, ValueType >::GetItem(
    Key const& id) const
{
    std::unique_lock< std::mutex > const lock{access_};  // instead of booost::shared_lock<booost::shared_mutex>//////
    SynchronizedStorage< Key, ValueType >::Value dataSPtr{nullptr};

    typename AraMap< Key, TValue< Key, ValueType > >::const_iterator const it{std::move(data_.find(id))};
    if (it != data_.end()) {  // find data
        dataSPtr = it->second;
    }

    return dataSPtr;
}

/// @brief Create and store new item
///
/// @param factory Item factory to use
/// @param generator
///
/// @return Newly inserted pair or error
/// @throws no
template < typename Key, typename ValueType >
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00000
/// @trace_id_dd=DD_UCM_10814
/// @needwork = dda
/// @endcode
template < typename Factory, typename Generator >
inline AraOptional< std::pair< Key, TValue< Key, ValueType > > >
SynchronizedStorage< Key, ValueType >::CreateAndStoreItem(Factory& factory, Generator& generator)
{
    static_assert(std::is_same< typename Generator::ResultType, Key >::value,
                  "Generated Result Type shall be equal to Key data type");

    std::unique_lock< std::mutex > const lock{access_};

    AraOptional< Key > id{std::move(generator(data_))};
    if (!id.has_value()) {
        return {};
    }

    TValue< Key, ValueType > item{//using Value = std::shared_ptr<ValueType>;.
                                  factory.Create(*id, std::move(AraStringView(kKV_MODEL_IDENTIFIER)))};
    _nonBlockingAddItem(*id, item);

    return {std::make_pair(item->GetID(), TValue< Key, ValueType >{item})};
}

/// @brief Create and store new item
///
/// @param factory Item factory to use
/// @param id Key of a value to Create
///
/// @return Newly inserted pair or error
/// @throws no
template < typename Key, typename ValueType >
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00000
/// @trace_id_dd=DD_UCM_10815
/// @needwork = dda
/// @endcode
template < typename Factory >
inline AraOptional< std::pair< Key, TValue< Key, ValueType > > >
SynchronizedStorage< Key, ValueType >::CreateAndStoreItemWithKey(Factory& factory, Key const& id)
{
    std::unique_lock< std::mutex > const lock{access_};

    TValue< Key, ValueType > item{factory.Create(id, std::move(AraStringView(kKV_MODEL_IDENTIFIER)))};
    _nonBlockingAddItem(id, item);

    return {std::make_pair(item->GetID(), TValue< Key, ValueType >{item})};
}

/// @brief delete item from the container
///
/// @param id Key of a value to delete
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00000
/// @trace_id_dd=DD_UCM_10816
/// @needwork = dda
/// @endcode
template < typename Key, typename ValueType >
inline void SynchronizedStorage< Key, ValueType >::DeleteItem(Key const& id)
{
    std::unique_lock< std::mutex > const lock{access_};
    std::ignore = data_.erase(id);
}

/// @brief Apply function to every item in caontainer
///
/// @param processor Function to apply to key-value pairs
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00000
/// @trace_id_dd=DD_UCM_10817
/// @needwork = dda
/// @endcode
template < typename Key, typename ValueType >
inline void SynchronizedStorage< Key, ValueType >::IterateItems(
    std::function< void(std::pair< Key, SynchronizedStorage< Key, ValueType >::Value const& > const) > const processor)
    const
{
    std::unique_lock< std::mutex > const lock{access_};
    for (std::pair< Key, TValue< Key, ValueType > const& > const& it : data_) {
        processor(it);
    }
}

/// @brief _nonBlockingAddItem
/// @param id
/// @param data
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00000
/// @trace_id_dd=DD_UCM_10818
/// @needwork = dda
/// @endcode
template < typename Key, typename ValueType >
inline void SynchronizedStorage< Key, ValueType >::_nonBlockingAddItem(
    Key const& id, SynchronizedStorage< Key, ValueType >::Value const data)
{
    data_[id] = data;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_SYNCHRONIZED_STORAGE_H_
