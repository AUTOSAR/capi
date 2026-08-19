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
/// @file       cg_instance.h
/// @brief
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_COMMUNICATON_GROUP_H
#define ARA_COM_COMMUNICATON_GROUP_H

#include "ara/com/internal/runtime.h"
#include "ara/com/types.h"
#include "ara/core/future.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/vector.h"

namespace ara {
namespace com {
namespace internal {
namespace cg {
/// @brief
template < typename T >
struct CgTypeInfo
{
};

/// @brief
class CommunicationGroupObj
{
public:
    /// @brief
    CommunicationGroupObj() noexcept = default;
    /// @brief
    virtual ~CommunicationGroupObj() noexcept = default;
    /// @brief Copy constructor
    /// @param other
    CommunicationGroupObj(CommunicationGroupObj const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    CommunicationGroupObj(CommunicationGroupObj&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return CommunicationGroupObj
    CommunicationGroupObj& operator=(CommunicationGroupObj const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return CommunicationGroupObj
    CommunicationGroupObj& operator=(CommunicationGroupObj&& other) noexcept = default;

public:
    /// @brief
    /// @param[in] instanceSpec
    /// @return bool
    virtual bool OfferService(ara::core::InstanceSpecifier const& instanceSpec) noexcept = 0;
    /// @brief
    /// @return
    virtual void StopOfferService() noexcept = 0;
    /// @brief
    /// @return
    virtual ara::core::Future< bool > ProcessNextMethodCall() noexcept = 0;
    /// @brief
    /// @return
    virtual void StartFindService(/*ara::core::StringView instanceId, int32_t clientId*/) noexcept = 0;
    /// @brief
    /// @return
    virtual void StopFindService() noexcept = 0;
};

/// @brief
template < class T >
class CommunicationGroup final : public CommunicationGroupObj
{
    /// @brief CgTypeInfo<T>::Skeleton
    using Skeleton = typename CgTypeInfo< T >::Skeleton;

public:
    /// @brief
    CommunicationGroup() noexcept = default;
    /// @brief
    ~CommunicationGroup() noexcept final { StopOfferService(); };
    /// @brief Copy constructor
    /// @param other
    CommunicationGroup(CommunicationGroup const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    CommunicationGroup(CommunicationGroup&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return CommunicationGroup
    CommunicationGroup& operator=(CommunicationGroup const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return CommunicationGroup
    CommunicationGroup& operator=(CommunicationGroup&& other) noexcept = default;

public:
    /// @brief
    /// @param[in] instanceSpec
    /// @return bool
    bool OfferService(ara::core::InstanceSpecifier const& instanceSpec) noexcept final
    {
        auto instanceIdentifiersRes{ara::com::runtime::ResolveInstanceIDs(instanceSpec)};
        if (!instanceIdentifiersRes) {
            return false;
        }
        auto instanceIdentifiers{std::move(instanceIdentifiersRes).Value()};
        if (instanceIdentifiers.empty()) {
            return false;
        }
        ComLogDebug("typename:", CgTypeInfo< T >::type_name());
        skeleton_ = CgTypeInfo< T >::Create(instanceSpec);

        if (!skeleton_) {
            return false;
        }
        auto voidRes{skeleton_->OfferService()};
        if (!voidRes) {
            return false;
        }
        return true;
    }

    /// @brief
    void StopOfferService() noexcept final{};
    /// @brief Process method and event requests received by the skeleton
    /// @return ara::core::Future<bool>
    ara::core::Future< bool > ProcessNextMethodCall() noexcept final { return skeleton_->ProcessNextMethodCall(); };

    /// @brief
    void StartFindService() noexcept final
    {
        auto b{skeleton_->InitClient()};
        ComLogDebug("InitClient:", b);
    }

    /// @brief
    void StopFindService() noexcept final { skeleton_->StopFindService(); }

private:
    /// @brief skeleton instance pointer
    std::unique_ptr< Skeleton > skeleton_{};
};
}  // namespace cg
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
