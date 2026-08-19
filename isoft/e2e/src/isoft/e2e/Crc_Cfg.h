
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

/*******************************************************************************
**                                                                            **
**  FILENAME    : Crc_Cfg.h                                                   **
**                                                                            **
**  Created on  :                                                             **
**  Author      : YB                                                          **
**  Vendor      :                                                             **
**  DESCRIPTION :                                                             **
**                                                                            **
**  SPECIFICATION(S) :   AUTOSAR classic Platform R19-11                      **
**                                                                            **
*******************************************************************************/

#ifndef CRC_CFG_H_
#define CRC_CFG_H_

#include "Std_Types.h"

/* req<sws_crc_00040></sws_crc_00040> */
/*=======[M A C R O S]========================================================*/

/* calculation method of Crc mode */
#define CRC_HARDWARE 0
#define CRC_RUNTIME  1
#define CRC_TABLE    2

/* CRC8 (SAE J1850) algorithm calculation switch */
#define CRC8_ALGORITHM STD_ON

/* CRC8 (2Fh polynomial) algorithm calculation switch */
#define CRC8H2F_ALGORITHM STD_ON

/* CRC16 (CCITT) algorithm calculation switch */
#define CRC16_ALGORITHM STD_ON

/* CRC32 (Ethernet) algorithm calculation switch */
#define CRC32_ALGORITHM STD_ON

/* CRC32 (0xF4ACFB13 polynomial) algorithm calculation switch */
#define CRC32P4_ALGORITHM STD_ON

/* CRC64 algorithm calculation switch */
#define CRC64_ALGORITHM STD_ON

/* Switch to select one of the available CRC 8-bit (SAE J1850) calculation methods
 * req<ecuc_crc_00030></ecuc_crc_00030> */
#define CRC8_MODE CRC_RUNTIME

/* Switch to select one of the available CRC 8-bit (2Fh polynomial) calculation methods
 * req<ecuc_crc_00031>*/
#define CRC8H2F_MODE CRC_RUNTIME

/* Switch to select one of the available CRC 16-bit (CCITT) calculation methods
 * req<ecuc_crc_00025>*/
#define CRC16_MODE CRC_RUNTIME

/* Switch to select one of the available CRC 32-bit (IEEE-802.3 CRC32 Ethernet Standard) calculation methods
 * req<ecuc_crc_00026>*/
#define CRC32_MODE CRC_RUNTIME

/* Switch to select one of the available CRC 32-bit E2E Profile 4 calculation methods
 * req<ecuc_crc_00032>*/
#define CRC32P4_MODE CRC_RUNTIME

/* Switch to select one of the available CRC 64 calculation methods
 * req<ecuc_crc_00034>*/
#define CRC64_MODE CRC_RUNTIME
#endif
