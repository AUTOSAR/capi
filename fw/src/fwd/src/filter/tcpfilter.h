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
/// @file       tcpfilter.h
/// @brief      tcpfilter filter
/// @details    tcpfilter filter
/// @date       2025-10-15
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Filter Mode
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0004
/// @unit_name=Filter_Tcp
/// @unit_description=Firewall Tcp filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_FILTER_TCPFILTER_H_
#define ARA_FW_FILTER_TCPFILTER_H_

// Add header file includes
#include <ara/core/vector.h>

#include "ara/fw/filterengine/engine_tcp.h"
#include "ara/fw/internal/manifest_data.h"
#include "filterbase.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief tcp filter.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00470
/// @trace_id_dd=DD_FW_00723
/// @needwork = ad
/// @endcode
class TcpFilter : public FilterBase
{
public:
    /// @brief TcpFilter constructor
    /// @param vecTcpRules tcp rule validation set
    /// @param action action
    /// @param inOutFlag hook point in/out
    /// @param pRate Processing method
    /// @param qLength Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00471
    /// @trace_id_dd=DD_FW_00724
    /// @needwork = ad
    /// @endcode
    TcpFilter(ara::core::Vector< PRuleParse > vecTcpRules,
              int32_t const &action,
              int32_t const &inOutFlag,
              int32_t pRate   = -1,
              int32_t qLength = -1) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00472
    /// @trace_id_dd=DD_FW_00725
    /// @needwork = ad
    /// @endcode
    ~TcpFilter() override = default;

    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00473
    /// @trace_id_dd=DD_FW_00726
    /// @needwork = ad
    /// @endcode
    TcpFilter(TcpFilter const &other) = delete;

    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00474
    /// @trace_id_dd=DD_FW_00727
    /// @needwork = ad
    /// @endcode
    TcpFilter(TcpFilter const &&other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return TcpFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00475
    /// @trace_id_dd=DD_FW_00728
    /// @needwork = ad
    /// @endcode
    TcpFilter &operator=(TcpFilter const &other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return TcpFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00476
    /// @trace_id_dd=DD_FW_00729
    /// @needwork = ad
    /// @endcode
    TcpFilter &operator=(TcpFilter const &&other) = delete;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00477
    /// @trace_id_dd=DD_FW_00730
    /// @needwork = ad
    /// @endcode
    void RulesFilter() noexcept override;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00478
    /// @trace_id_dd=DD_FW_00731
    /// @needwork = ad
    /// @endcode
    void GetRules() noexcept override;

private:
    /// @brief tcp filter rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00732
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecTcpRules_{};
    /// @brief Checksum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00733
    /// @needwork = dda
    /// @endcode
    bool checksumVerification_{true};
    /// @brief Maximum port value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00734
    /// @needwork = dda
    /// @endcode
    int32_t maxDestPortNum_{-1};
    /// @brief Minimum port value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00735
    /// @needwork = dda
    /// @endcode
    int32_t minDestPortNum_{-1};
    /// @brief Source address maximum port value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00736
    /// @needwork = dda
    /// @endcode
    int32_t maxSrcPortNum_{-1};
    /// @brief Source address minimum port value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00737
    /// @needwork = dda
    /// @endcode
    int32_t minSrcPortNum_{-1};
    /// @brief tcp max session value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00738
    /// @needwork = dda
    /// @endcode
    int32_t numberOfParallelTcpSessions_{-1};
    /// @brief tcp state transition flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00739
    /// @needwork = dda
    /// @endcode
    bool stateManagementBasedOnTcpFlags_{false};
    /// @brief tcp timeout setting
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00740
    /// @needwork = dda
    /// @endcode
    int32_t timeoutCheck_{-1};

    /// @brief udp engine udp engine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00741
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< TcpEngine > unpTcpEngine_{};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif