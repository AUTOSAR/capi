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
/// @file       idsm_diag_did_server.h
/// @brief      Security event reporting mode access class, providing AA-side users with related functions for accessing security event reporting mode
/// @details
/// @date       2023-01-16
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Diagnostic data identifier service
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0004
/// @unit_name=DiagDidServer
/// @unit_description=Security event reporting mode access class, providing AA-side users with related functions for accessing security event reporting mode
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_DIAG_DID_SERVER_H_
#define ARA_IDSM_DIAG_DID_SERVER_H_
#include <ara/core/map.h>
#include <ara/core/promise.h>
#ifdef ARA_WITH_DIAG
    #include <ara/diag/generic_data_identifier.h>

namespace ara {
namespace idsm {
/// @brief Type redefinition: diagnostic module related types
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00247
/// @trace_id_dd=DD_IDSM_00706
/// @needwork = ad
/// @endcode
using OperationOutput = ara::diag::GenericDataIdentifier::OperationOutput;
/// @brief Security event reporting mode read/write class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00248
/// @trace_id_dd=DD_IDSM_00707
/// @needwork = ad
/// @endcode
class DiagDidServer final : public ara::diag::GenericDataIdentifier
{
public:
    /// @brief Initialize mapping between diagnostic data identifier and event id
    /// @param m mapping from data identifier to event id
    /// @return 0 success, -1 failure
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00249
    /// @trace_id_dd=DD_IDSM_00708
    /// @needwork = ad
    /// @endcode
    int32_t Init(ara::core::Map< uint16_t, uint16_t > const& m);
    /// @brief Read reporting mode of security event
    /// @param dataIdentifier data identifier mapped to event id
    /// @param metaInfo diagnostic related parameters
    /// @param cancellationHandler diagnostic related parameters
    /// @return reporting mode of security event
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00250
    /// @trace_id_dd=DD_IDSM_00709
    /// @needwork = ad
    /// @endcode
    ara::core::Future< OperationOutput > Read(std::uint16_t dataIdentifier,
                                              ara::diag::MetaInfo& metaInfo,
                                              ara::diag::CancellationHandler cancellationHandler) final;
    /// @brief Set reporting mode of security event
    /// @param dataIdentifier data identifier mapped to event id
    /// @param requestData diagnostic related parameters
    /// @param metaInfo diagnostic related parameters
    /// @param cancellationHandler diagnostic related parameters
    /// @return whether data write succeeded
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00251
    /// @trace_id_dd=DD_IDSM_00710
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > Write(std::uint16_t dataIdentifier,
                                    ara::core::Span< std::uint8_t > requestData,
                                    ara::diag::MetaInfo& metaInfo,
                                    ara::diag::CancellationHandler cancellationHandler) final;

public:
    /// @brief default constructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00711
    /// @needwork = dda
    /// @endcode
    DiagDidServer() = delete;
    /// @brief constructor
    /// @param specifier descriptor
    /// @param reentrancyType diagnostic related parameters
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00252
    /// @trace_id_dd=DD_IDSM_00712
    /// @needwork = ad
    /// @endcode
    DiagDidServer(ara::core::InstanceSpecifier const& specifier,
                  ara::diag::DataIdentifierReentrancyType const reentrancyType);
    /// @brief destructor
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00713
    /// @needwork = dda
    /// @endcode
    ~DiagDidServer() final = default;

public:
    /// @exception stack overflow exception
    /// @brief copy constructor, deleted
    /// @param server object to copy
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00714
    /// @needwork = dda
    /// @endcode
    DiagDidServer(DiagDidServer const& server) = delete;
    /// @brief move constructor, deleted
    /// @param server object to move
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00715
    /// @needwork = dda
    /// @endcode
    DiagDidServer(DiagDidServer&& server) = delete;
    /// @brief copy assignment operator, deleted
    /// @param server right operand of assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00716
    /// @needwork = dda
    /// @endcode
    DiagDidServer& operator=(DiagDidServer const& server) = delete;
    /// @brief move assignment operator, deleted
    /// @param server right operand of assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00717
    /// @needwork = dda
    /// @endcode
    DiagDidServer& operator=(DiagDidServer&& server) = delete;

private:
    /// @name didIdToEventIdMap
    /// @brief mapping between event id and data identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00718
    /// @needwork = dda
    /// @endcode
    ara::core::Map< uint16_t, uint16_t > didIdToEventIdMap_{};
};

}  // namespace idsm
}  // namespace ara
#endif

#endif  // ARA_IDSM_DIAG_DID_SERVER_H_