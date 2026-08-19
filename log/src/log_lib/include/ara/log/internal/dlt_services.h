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
/// @file       dlt_services.h
/// @brief      DLT service IDs for internal encoding module
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_DLT_SERVICES__
#define __LOG_INTERNAL_DLT_SERVICES__

#include <cstdint>

namespace ara {
namespace log {
namespace internal {

/// @brief DLT service IDs
enum class DltServices : std::uint32_t
{
    kDlt_Service_Id                                 = 0x00U,
    kDlt_Service_Id_Set_Log_Level                   = 0x01U,
    kDlt_Service_Id_Set_Trace_Status                = 0x02U,
    kDlt_Service_Id_Get_Log_Info                    = 0x03U,
    kDlt_Service_Id_Get_Default_Log_Level           = 0x04U,
    kDlt_Service_Id_Store_Config                    = 0x05U,
    kDlt_Service_Id_Reset_To_Factory_Default        = 0x06U,
    kDlt_Service_Id_Set_Com_Interface_Status        = 0x07U,
    kDlt_Service_Id_Set_Com_Interface_Max_Bandwidth = 0x08U,
    kDlt_Service_Id_Set_Verbose_Mode                = 0x09U,
    kDlt_Service_Id_Set_Message_Filtering           = 0x0AU,
    kDlt_Service_Id_Set_Timing_Packets              = 0x0BU,
    kDlt_Service_Id_Get_Local_Time                  = 0x0CU,
    kDlt_Service_Id_Use_Ecu_Id                      = 0x0DU,
    kDlt_Service_Id_Use_Session_Id                  = 0x0EU,
    kDlt_Service_Id_Use_Timestamp                   = 0x0FU,
    kDlt_Service_Id_Use_Extended_Header             = 0x10U,
    kDlt_Service_Id_Set_Default_Log_Level           = 0x11U,
    kDlt_Service_Id_Set_Default_Trace_Status        = 0x12U,
    kDlt_Service_Id_Get_Software_Version            = 0x13U,
    kDlt_Service_Id_Message_Buffer_Overflow         = 0x14U,
    kDlt_Service_Id_Last_Entry                      = 0xffU,

    // Custom service IDs
    kDlt_Custom_Service_Id_Start   = 0x00000FFFU,
    kDlt_Custom_Service_Id_Jsoncmd = 0x00001FFFU,
    kDlt_Custom_Service_Id_End     = 0xFFFFFFFFU,
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // __LOG_INTERNAL_DLT_SERVICES__
