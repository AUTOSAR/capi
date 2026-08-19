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
/// @file       diagdtcserver.h
/// @brief      Reporting diagnostic events related
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/nmm
/// @interface_level=unit
/// @trace_id_sr=SRS_NM_00017
/// @unit_name=DiagDtcServer
/// @unit_description=Reporting diagnostic events related
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_DIAGDTCSERVER_H_
#define _ARA_NM_DIAGDTCSERVER_H_
#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <ara/diag/generic_data_identifier.h>
#include <ara/diag/monitor.h>
#include <ara/diag/operation_cycle.h>

#include <chrono>
#include <deque>
#include <memory>
#include <mutex>

namespace ara {
namespace nm {
namespace internal {

/// @brief Type redefinition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100000
/// @trace_id_dd=DD_NM_00867
/// @needwork = ad
/// @endcode
using OperationOutput = ara::diag::GenericDataIdentifier::OperationOutput;
/// @brief Diagnostic fault event data reading class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100057
/// @trace_id_dd=DD_NM_00797
/// @needwork = ad
/// @endcode
class DiagDtcServer final : public ara::diag::GenericDataIdentifier
{
private:
    /// @brief Record data buffer time
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100000
    /// @trace_id_dd=DD_NM_00908
    /// @needwork = ad
    /// @endcode
    struct DataCache final
    {
    public:
        /// @brief timestamp
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_NM_00000
        /// @trace_id_dd=DD_NM_00799
        /// @needwork = dda
        /// @endcode
        std::time_t timestamp{};

        /// @brief data
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_NM_00000
        /// @trace_id_dd=DD_NM_00800
        /// @needwork = dda
        /// @endcode
        ara::core::String data{};
    };

public:
    /// @brief Copy constructor
    /// @exception Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00381
    /// @needwork = dda
    /// @endcode
    DiagDtcServer() = delete;

    /// @brief Constructor
    /// @param specifier
    /// @param reentrancyType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00382
    /// @needwork = dda
    /// @endcode
    DiagDtcServer(ara::core::InstanceSpecifier const &specifier,
                  ara::diag::DataIdentifierReentrancyType const reentrancyType) noexcept;

    /// @brief Constructor
    /// @param specifier
    /// @param reentrancyType
    /// @param operCycleInstance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00382
    /// @needwork = dda
    /// @endcode
    DiagDtcServer(ara::core::InstanceSpecifier const &specifier,
                  ara::diag::DataIdentifierReentrancyType const reentrancyType,
                  ara::core::InstanceSpecifier const &operCycleInstance) noexcept;

    /// @brief Constructor
    /// @brief Copy constructor
    /// @param server
    /// @exception Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00383
    /// @needwork = dda
    /// @endcode
    DiagDtcServer(DiagDtcServer const &server) = delete;

    /// @brief Move constructor
    /// @param server
    /// @exception Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00384
    /// @needwork = dda
    /// @endcode
    DiagDtcServer(DiagDtcServer &&server) = delete;

    /// @brief Copy assignment operator
    /// @param server
    /// @return
    /// @exception Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00385
    /// @needwork = dda
    /// @endcode
    DiagDtcServer &operator=(DiagDtcServer const &server) = delete;

    /// @brief Move assignment operator
    /// @param server
    /// @return
    /// @exception Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00386
    /// @needwork = dda
    /// @endcode
    DiagDtcServer &operator=(DiagDtcServer &&server) = delete;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00387
    /// @needwork = dda
    /// @endcode
    ~DiagDtcServer() final
    {
        if (nullptr != operationCycle_) {
            operationCycle_->SetOperationCycle(ara::diag::OperationCycleType::kOperationCycleEnd);
        }
    };

    /// @brief Initialize the event id mapped by the diagnostic data identifier
    /// @param monitor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00388
    /// @needwork = dda
    /// @endcode
    void Init(ara::core::InstanceSpecifier const &monitor) noexcept;

    /// @brief Read fault event data
    /// @param dataIdentifier Data identifier mapped to event id
    /// @param metaInfo Inherited interface parameter, currently not used
    /// @param cancellationHandler Inherited interface parameter, currently not used
    /// @throws Throws exception
    /// @return Fault event information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00389
    /// @needwork = dda
    /// @endcode
    ara::core::Future< OperationOutput > Read(std::uint16_t const dataIdentifier,
                                              ara::diag::MetaInfo &metaInfo,
                                              ara::diag::CancellationHandler cancellationHandler) noexcept final;
    /// @brief Will not be called
    /// @param dataIdentifier
    /// @param requestData
    /// @param metaInfo
    /// @param cancellationHandler
    /// @throws Throws exception
    /// @return Whether data write succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00390
    /// @needwork = dda
    /// @endcode
    ara::core::Future< void > Write(std::uint16_t dataIdentifier,
                                    ara::core::Span< std::uint8_t > requestData,
                                    ara::diag::MetaInfo &metaInfo,
                                    ara::diag::CancellationHandler cancellationHandler) noexcept final;

    /// @brief Append fault event data
    /// @param data Fault event data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00391
    /// @needwork = dda
    /// @endcode
    void PushData(ara::core::String const &data) noexcept;

    /// @brief Notify process end
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00392
    /// @needwork = dda
    /// @endcode
    void NotifyStop() noexcept { bStop_ = true; };

private:
    /// @brief bufferLock_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00393
    /// @needwork = dda
    /// @endcode
    std::mutex bufferLock_;

    /// @brief dataQueue_ Data storage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00394
    /// @needwork = dda
    /// @endcode
    std::deque< std::shared_ptr< DataCache > > dataQueue_;

    /// @brief monitorInstance_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00395
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ara::core::InstanceSpecifier > monitorInstance_;

    /// @brief monitor_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00396
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ara::diag::Monitor > monitor_;

    /// @brief operationCycle_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00398
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ara::diag::OperationCycle > operationCycle_;

    /// @brief bStop_ Whether the process has ended
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00397
    /// @needwork = dda
    /// @endcode
    bool bStop_;
};

/// @brief Type redefinition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100001
/// @trace_id_dd=DD_NM_00869
/// @needwork = ad
/// @endcode
using DiagDtcServerPtr = std::shared_ptr< DiagDtcServer >;

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  // _ARA_NM_DIAGDTCSERVER_H_