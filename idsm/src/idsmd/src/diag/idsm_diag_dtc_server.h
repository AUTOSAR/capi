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
/// @file       idsm_diag_dtc_server.h
/// @brief      Security event storage class, providing diagnostic functions for retrieving security event information
/// @details
/// @date       2023-01-17
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Qualified security event storage
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0012
/// @unit_name=DiagDtcServer
/// @unit_description=Security event storage class, providing diagnostic functions for retrieving security event information
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_DIAG_DTC_SERVER_H_
#define ARA_IDSM_DIAG_DTC_SERVER_H_
#include <ara/core/promise.h>
#include <ara/diag/monitor.h>

#include <chrono>
#include <deque>
#include <memory>
#include <mutex>

#include "ara/idsm/common.h"
#ifdef ARA_WITH_DIAG
    #include <ara/diag/generic_data_identifier.h>
namespace ara {
namespace idsm {
/// @brief Type redefinition
using OperationOutput = ara::diag::GenericDataIdentifier::OperationOutput;
/// @brief Diagnostic class for reading security event data
/// @code{.isoft}
/// @unit_name=DiagDtcServer
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00253
/// @trace_id_dd=DD_IDSM_00719
/// @needwork = ad
/// @endcode
class DiagDtcServer final : public ara::diag::GenericDataIdentifier
{
private:
    /// @brief Record the time of data buffer
    /// @code{.isoft}
    /// @unit_name=DataCache
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00254
    /// @trace_id_dd=DD_IDSM_00720
    /// @needwork = ad
    /// @endcode
    class DataCache final
    {
    public:
        /// @brief default constructor
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00721
        /// @needwork = dda
        /// @endcode
        DataCache() noexcept = default;
        /// @brief copy constructor
        /// @param d object to copy
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00722
        /// @needwork = dda
        /// @endcode
        DataCache(DataCache const& d) noexcept = default;
        /// @brief move constructor
        /// @param d object to move
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00723
        /// @needwork = dda
        /// @endcode
        DataCache(DataCache&& d) noexcept = default;
        /// @brief copy assignment operator
        /// @param d object to copy in assignment operator
        /// @return left operand of assignment operator
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00724
        /// @needwork = dda
        /// @endcode
        DataCache& operator=(DataCache const& d) noexcept = default;
        /// @brief move assignment operator
        /// @param d object to move in assignment operator
        /// @return left operand of assignment operator
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00725
        /// @needwork = dda
        /// @endcode
        DataCache& operator=(DataCache&& d) noexcept = default;
        /// @brief destructor
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00726
        /// @needwork = dda
        /// @endcode
        ~DataCache() = default;
        /// @brief parameterized constructor
        /// @param t timestamp
        /// @param d context data
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00727
        /// @needwork = dda
        /// @endcode
        DataCache(std::time_t const t, ContextDataType d) noexcept : timestamp_{t}, data_{std::move(d)} {}
        /// @brief get timestamp
        /// @return timestamp
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00728
        /// @needwork = dda
        /// @endcode
        std::time_t GetTimeStamp() const noexcept { return timestamp_; }
        /// @brief get context data
        /// @param context context data
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00729
        /// @needwork = dda
        /// @endcode
        void GetContextData(ContextDataType& context) noexcept { context.swap(data_); }
        /// @brief set timestamp
        /// @param ct timestamp
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00730
        /// @needwork = dda
        /// @endcode
        void SetTimeStamp(std::time_t const ct) noexcept { timestamp_ = ct; }
        /// @brief set context
        /// @param context context
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00731
        /// @needwork = dda
        /// @endcode
        void SetContextData(ContextDataType& context) noexcept { data_.swap(context); }

    private:
        /// @name timestamp
        /// @brief timestamp
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00732
        /// @needwork = dda
        /// @endcode
        std::time_t timestamp_{};
        /// @name data
        /// @brief context data
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_IDSM_00000
        /// @trace_id_dd=DD_IDSM_00733
        /// @needwork = dda
        /// @endcode
        ContextDataType data_{};
    };
    /// @brief Type redefinition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00734
    /// @needwork = dda
    /// @endcode
    using DataCachePtr = std::shared_ptr< DataCache >;

public:
    /// @brief Initialize event id mapped by diagnostic data identifier
    /// @param id Id of the security event
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// export_level=/idsm/Qualified security event storage
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00255
    /// @trace_id_dd=DD_IDSM_00735
    /// @needwork = ad
    /// @endcode
    void Init(uint16_t const id);
    /// @brief Read data of security event
    /// @param dataIdentifier data identifier mapped to event id
    /// @param metaInfo diagnostic related parameters
    /// @param cancellationHandler diagnostic related parameters
    /// @return security event information
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// export_level=/idsm/Qualified security event storage
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00256
    /// @trace_id_dd=DD_IDSM_00736
    /// @needwork = ad
    /// @endcode
    ara::core::Future< OperationOutput > Read(std::uint16_t dataIdentifier,
                                              ara::diag::MetaInfo& metaInfo,
                                              ara::diag::CancellationHandler cancellationHandler) final;
    /// @brief will not be called
    /// @param dataIdentifier data identifier mapped to event id
    /// @param requestData diagnostic related parameters
    /// @param metaInfo diagnostic related parameters
    /// @param cancellationHandler diagnostic related parameters
    /// @return whether data write succeeded
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// export_level=/idsm/Qualified security event storage
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00257
    /// @trace_id_dd=DD_IDSM_00737
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > Write(std::uint16_t dataIdentifier,
                                    ara::core::Span< std::uint8_t > requestData,
                                    ara::diag::MetaInfo& metaInfo,
                                    ara::diag::CancellationHandler cancellationHandler) final;
    /// @brief Append data of security event
    /// @param data data of the security event
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00738
    /// @needwork = dda
    /// @endcode
    void PushData(ContextDataType const& data) noexcept;
    /// @brief Discard data of security event
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00739
    /// @needwork = dda
    /// @endcode
    void PopData() noexcept;

public:
    /// @brief default constructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00740
    /// @needwork = dda
    /// @endcode
    DiagDtcServer() = delete;
    /// @brief constructor
    /// @param specifier descriptor
    /// @param reentrancyType diagnostic related parameters
    /// @param monitor monitor instance descriptor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00741
    /// @needwork = dda
    /// @endcode
    DiagDtcServer(ara::core::InstanceSpecifier const& specifier,
                  ara::diag::DataIdentifierReentrancyType const reentrancyType,
                  ara::core::InstanceSpecifier monitor);
    /// @brief destructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00742
    /// @needwork = dda
    /// @endcode
    ~DiagDtcServer() final = default;

public:
    /// @brief copy constructor
    /// @param server object to copy
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00743
    /// @needwork = dda
    /// @endcode
    DiagDtcServer(DiagDtcServer const& server) = delete;
    /// @brief move constructor
    /// @param server object to move
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00744
    /// @needwork = dda
    /// @endcode
    DiagDtcServer(DiagDtcServer&& server) = delete;
    /// @brief copy assignment operator
    /// @param server object to copy in assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00745
    /// @needwork = dda
    /// @endcode
    DiagDtcServer& operator=(DiagDtcServer const& server) = delete;
    /// @brief move assignment operator
    /// @param server object to move in assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00746
    /// @needwork = dda
    /// @endcode
    DiagDtcServer& operator=(DiagDtcServer&& server) = delete;

private:
    /// @brief Check if data of security event has timed out
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00747
    /// @needwork = dda
    /// @endcode
    void _checkOldData();

private:
    /// @brief mutex
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00748
    /// @needwork = dda
    /// @endcode
    std::mutex bufferLock_{};
    /// @brief data to be sent
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00749
    /// @needwork = dda
    /// @endcode
    std::deque< DataCachePtr > dataQueue_{};
    /// @brief monitor instance descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00750
    /// @needwork = dda
    /// @endcode
    ara::core::InstanceSpecifier monitorInstance_;
    /// @brief security event Id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00751
    /// @needwork = dda
    /// @endcode
    uint16_t eventId_{0U};
    /// @brief monitor instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00752
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ara::diag::Monitor > monitor_{nullptr};
};
/// @brief Type redefinition
using DiagDtcServerPtr = std::shared_ptr< DiagDtcServer >;

}  // namespace idsm
}  // namespace ara
#endif

#endif  // ARA_IDSM_DIAG_DTC_SERVER_H_