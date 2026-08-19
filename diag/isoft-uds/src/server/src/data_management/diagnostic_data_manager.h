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
/// @file       diagnostic_data_manager.h
/// @brief      This file provides diagnostic_data_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/data_management
/// @interface_level=software
/// @trace_id_sr=52c4bd77-d69d-4f55-b41b-508f4e1ef4c6
/// @unit_name=DiagnosticDataManager
/// @unit_description=This file provides diagnostic_data_manager functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_DIAGNOSTICDATAMANAGER_H_
#define ISOFT_UDS_SERVER_DIAGNOSTICDATAMANAGER_H_
#include <list>
#include <memory>
#include <vector>

#include "common/persistence_service.h"
#include "isoft/uds/data_management/diagnostic_data_model_manager.h"
#include "isoft/uds/result.h"
#include "security_access_management/security_level_manager.h"
#include "session_management/session_manager.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Buffer type
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
using Buffer = std::vector< std::uint8_t >;

/// @brief Data source type
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
struct SourceData
{
    /// @brief Data identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint16_t id{0U};
    /// @brief Data location
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t nPos{0U};
    /// @brief Data size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t nSize{0U};
};

/// @brief Dynamic data model type
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
struct DiagnosticDynamicDataModel
{
    /// @brief Data identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint16_t id{0U};
    /// @brief Data record
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::list< SourceData > record{0U};
};

/// @brief Dynamic data manager type
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
class DiagnosticDataManager
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00758
    /// @needwork = dda
    /// @endcode
    DiagnosticDataManager(DiagnosticDataManager const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00759
    /// @needwork = dda
    /// @endcode
    DiagnosticDataManager& operator=(DiagnosticDataManager const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00760
    /// @needwork = dda
    /// @endcode
    DiagnosticDataManager(DiagnosticDataManager&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00761
    /// @needwork = dda
    /// @endcode
    DiagnosticDataManager& operator=(DiagnosticDataManager&&) = delete;

public:
    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    explicit DiagnosticDataManager(std::shared_ptr< PersistenceService >& persistenceService) noexcept;

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    ~DiagnosticDataManager() noexcept = default;

    /// @brief Initialization function
    /// @param modelManager Data model manager
    /// @param sessionManagerPtr Session manager pointer
    /// @param securityLevelManagerPtr Security level manager pointer
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(DiagnosticDataModelManager& modelManager,
                            std::shared_ptr< SessionManager >& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr) noexcept;

    /// @brief Get diagnostic data element model function
    /// @param id Identifier
    /// @return Diagnostic data element model
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Result< DiagnosticDataElementModel > GetDiagnosticDataElementModel(std::uint32_t id) noexcept;

    /// @brief Dynamic data identifier determination
    /// @param id Identifier
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool IsDynamic(std::uint16_t id) noexcept;

    /// @brief Check whether diagnostic data identifier is configured
    /// @param id Identifier
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool HasDataIdentifier(std::uint16_t id) noexcept;

    /// @brief Read diagnostic data function
    /// @param dataIdentifierTable Data identifier table
    /// @param meataInfo Metadata mapping
    /// @param cancellationState Cancel state pointer
    /// @return Diagnostic data list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Result< std::list< DiagnosticData > > Read(std::vector< std::uint16_t >& dataIdentifierTable,
                                               MetaInfoMap const& meataInfo,
                                               std::shared_ptr< CancellationState > const& cancellationState) noexcept;

    /// @brief Read diagnostic data element function
    /// @param idTable Identifier table
    /// @param meataInfo Metadata mapping
    /// @param cancellationState Cancel state pointer
    /// @return Diagnostic data element list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Result< std::list< DiagnosticDataElement > > ReadDataElement(
        std::vector< std::uint32_t > const& idTable,
        MetaInfoMap const& meataInfo,
        std::shared_ptr< CancellationState > const& cancellationState) noexcept;

    /// @brief Read diagnostic data element function
    /// @param id Identifier
    /// @param meataInfo Metadata mapping
    /// @param cancellationState Cancel state pointer
    /// @return Diagnostic data element
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Result< DiagnosticDataElement > ReadDataElement(
        std::uint32_t id,
        MetaInfoMap const& meataInfo,
        std::shared_ptr< CancellationState > const& cancellationState) noexcept;

    /// @brief Read diagnostic data element function
    /// @param idTable Identifier table
    /// @param meataInfo Metadata mapping
    /// @param cancellationState Cancel state pointer
    /// @return Diagnostic data element list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Result< std::list< DiagnosticDataElement > > ReadEdrElement(
        std::vector< std::uint32_t > const& idTable,
        MetaInfoMap const& meataInfo,
        std::shared_ptr< CancellationState > const& cancellationState) noexcept;

    /// @brief Write diagnostic data function
    /// @param data Diagnostic data
    /// @param meataInfo Metadata mapping
    /// @param cancellationState Cancel state pointer
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Result< void > Write(DiagnosticData const& data,
                         MetaInfoMap const& meataInfo,
                         std::shared_ptr< CancellationState > const& cancellationState) noexcept;

    /// @brief Defined function
    /// @param data Diagnostic dynamic data model
    /// @param store Parameter
    /// @return Unsigned integer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t Defined(DiagnosticDynamicDataModel& data, bool store) noexcept;

    /// @brief Get how many static data identifier elements are already contained in the dynamic data identifier
    /// @param did Parameter
    /// @return Size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::size_t GetCountOfDiagnosticDynamicDataModelSource(std::uint16_t did) noexcept;

    /// @brief Clear dynamic data identifier
    /// @param did Parameter
    /// @return Unsigned integer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t ClearDefined(std::uint16_t did) noexcept;

    /// @brief Clear all dynamic data identifiers
    /// @return Unsigned integer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t ClearAllDefined() noexcept;

    /// @brief Get size
    /// @param did Parameter
    /// @return Size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::size_t GetDataSize(std::uint16_t did) noexcept;

    /// @brief Get all dynamic data models
    /// @return All dynamic data models
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::vector< std::reference_wrapper< DiagnosticDynamicDataModel > > GetAllDynamicDataModel() noexcept;

private:
    /// @brief Verify whether data identifier is valid
    /// @param id Identifier
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool _validateDataIdentifier(std::uint16_t id) noexcept;

    /// @brief Verify whether data element is internal element
    /// @param id Identifier
    /// @param offset Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool _isInternalElement(std::uint16_t id, std::uint32_t offset) noexcept;

    /// @brief Read diagnostic data function
    /// @param dataIdentifierTable Data identifier table
    /// @param meataInfo Metadata mapping
    /// @param cancellationState Cancel state pointer
    /// @return Diagnostic data list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Result< std::list< DiagnosticData > > _readDynamic(
        std::vector< std::uint16_t >& dataIdentifierTable,
        MetaInfoMap const& meataInfo,
        std::shared_ptr< CancellationState > const& cancellationState) noexcept;


    /// @brief Read diagnostic data element function
    /// @param dataIdentifierTable Data identifier table
    /// @param meataInfo Metadata mapping
    /// @param cancellationHandler Cancel handler
    /// @return Diagnostic data element list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Result< std::list< DiagnosticData > > _readByElement(std::vector< std::uint16_t >& dataIdentifierTable,
                                                         MetaInfoMap const& meataInfo,
                                                         CancellationHandler cancellationHandler) noexcept;

    /// @brief Load stored dynamic data model function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _loadStoreDynamicModel();

    /// @brief Save stored dynamic data model function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _saveStoreDynamicModel();

    /// @brief Clear stored dynamic data model function
    /// @param did Parameter
    /// @return Unsigned integer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _clearStoreDynamicModel(std::uint16_t did);

private:
    /// @brief Diagnostic data model table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::map< std::uint16_t, DiagnosticDataModel > diagnosticDataModelTable_;

    /// @brief Diagnostic dynamic data model table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::map< std::uint16_t, DiagnosticDynamicDataModel > diagnosticDynamicDataModelTable_;

    /// @brief Diagnostic data element model table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::map< std::uint32_t, DiagnosticDataParamterModel > edrElementTable_;

    /// @brief Dynamic data identifier set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::set< std::uint16_t > dynamicAvailableIdentifier_;

    /// @brief Persistence service pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< PersistenceService >& persistenceService_;

    /// @brief Generic data identifier interface pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< GenericDataIdentifierInterface > dataInterfacePtr_;

    /// @brief Generic data element interface pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< GenericDataElementInterface > dataElementInterfacePtr_;

    /// @brief Store dynamic data model flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool storeDynamicDataModel_;

    /// @brief Current session ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    SessionId currentSessionId_;

    /// @brief Current security level ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    SecurityLevelId currentSecurityLevelId_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_DIAGNOSTICDATAMANAGER_H_