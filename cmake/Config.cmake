#  Disclaimer
#
#  This work (specification and/or software implementation) and the material
#  contained in it, as released by AUTOSAR, is for the purpose of information
#  only. AUTOSAR and the companies that have contributed to it shall not be
#  liable for any use of the work.
#
#  The material contained in this work is protected by copyright and other
#  types of intellectual property rights. The commercial exploitation of the
#  material contained in this work requires a license to such intellectual
#  property rights.
#
#  This work may be utilized or reproduced without any modification, in any
#  form or by any means, for informational purposes only. For any other
#  purpose, no part of the work may be utilized or reproduced, in any form
#  or by any means, without permission in writing from the publisher.
#
#  The work has been developed for automotive applications only. It has
#  neither been developed, nor tested for non-automotive applications.
#
#  The word AUTOSAR and the AUTOSAR logo are registered trademarks.
#  --------------------------------------------------------------------------

# ================================================================
#
# File description:
# ----------------
# @file        Config.cmake
# @brief      
# @details
# @date        2026-01-01
# @author      jian.feng
# @version     0.1
# @description Global AP source code configuration. Configure global standard
#              macros according to project requirements, enabling or disabling
#              AP modules or features.
# ================================================================

include_guard(GLOBAL)

# =============================================================================
# Conditional debugging macro: control debug output via ARA_CMAKE_VERBOSE
# =============================================================================
option(ARA_CMAKE_VERBOSE "Enable verbose CMake debugging output" OFF)

# Prefix description:
# Purpose of prefix definition:
#   1. Unify naming style to improve readability, acting as a namespace;
#   2. When passing parameters to ExternalProject, can filter parameters by prefix, simplifying parameter handling;
# ARA_: All custom configurations that affect the SDK start with ARA_.

# ARA_ENABLE_xx: Control xx compilation feature
# ARA_ENABLE_xx: Whether to enable xx software module (including ara and isoft). If set to OFF, the entire xx module will not be compiled, and source code pruning will completely delete the module.
# ARA_ENABLE_xx_yy: Whether to enable yy software feature of xx module. Its impact is evaluated by each module. If set to OFF, source code pruning may delete the yy related directory of the xx module (depending on the pruning implementation of each module).

#TODO: Starting from cmake 3.19, string(JSOFF) is supported to read jsOFF configuration, and CMakePreset.jsOFF is also supported.
#       Later, consider putting all these complex configurations into jsOFF to further simplify project customization configuration.

# Each time AP_ALL is merged, if the SDK is intended to be released externally (to the testing department or users), this version number should be updated
set(SDK_VERSION 1.0.0)

# Global configuration option definitions
set(ARA_SYSROOT "$ENV{ARA_SYSROOT}" CACHE PATH "AP runtime environment system root path")
set(ARA_DEST_MACHINE "/ISOFT/Development/Machine1" CACHE PATH "Target machine path")
set(ARA_GEN_EXECUTABLE_OUTPUT "$ENV{HOME}/.isoft/tmp/ara_binout" CACHE PATH "Generated executable output path")

set(ARA_MAJOR_VERSION 1)
# Temporary, currently v3 hasn't been branched separately; this variable is to distinguish from v2
option(ARA_ENABLE_V3 "build with v3 content" OFF)

# Compilation feature configuration
option(ARA_BUILD_WITH_ASAN "Build with address sanitizer" OFF)
option(ARA_BUILD_WITH_LINUX_AIO "Enable Linux AIO" ON)
option(ARA_BUILD_WITH_SSL "Enable SSL support" ON)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Compiler configuration
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Temporary fix for Clang" FORCE)
endif()
# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Parallel build configuration, the maximum number of tasks allowed to be used in parallel during compilation. If not configured, the default is the current compiler's maximum logical core count minus 2.
# Note:
#   1. Strongly discourage writing this configuration item into the cmake file; if control is needed, strongly recommend passing parameters via command line.
#   2. In lower versions of cmake, this configuration is an environment variable, not a variable (so-called cmake macro), but in this project we actively recognize this variable.
#set(CMAKE_BUILD_PARALLEL_LEVEL 10 CACHE STRING "Parallel build level" FORCE)

# ============ ara tools and manual configuration ============
option(ARA_ENABLE_TOOLS "Enable the ara tools" ON)
option(ARA_ENABLE_MANUAL "Enable the ara manual" OFF)
# Build option classification configuration
option(ARA_ENABLE_APENV "Enable AP envirOFFment" ON)
option(ARA_ENABLE_TESTS "Enable unit tests" OFF)
option(ARA_ENABLE_SAMPLES "Enable sample code" OFF)
option(ARA_ENABLE_DOCS "Enable docs" OFF)
option(ARA_ENABLE_NML "build ara with isoft nml" OFF)

# ============ COM module configuration ============
option(ARA_ENABLE_COM "Enable the AP com module" ON)
if(ARA_ENABLE_COM)
    option(ARA_ENABLE_COM_NPC "Enable ara com npc" ON)
    option(ARA_ENABLE_COM_E2E "Enable ara com E2E" ON)
    option(ARA_ENABLE_COM_NSOMEIP "Enable ara nsomeip bind" ON)
    option(ARA_ENABLE_COM_ICC "Enable ara com icc" OFF)
    option(ARA_ENABLE_COM_FASTDDS "Enable ara fastdds bind" OFF)
    option(ARA_ENABLE_COM_CG "Enable ara CG" ON)
    option(ARA_ENABLE_COM_CG_DAEMON "build ara with com cg daemon " ON)
    option(ARA_ENABLE_COM_RAW "Enable ara RAW" ON)
    option(ARA_ENABLE_COM_NET_FILTER "Enable ara COM nsi net filter" ON)
    option(ARA_ENABLE_COM_MONITOR "Enable ara COM monitor log trace" ON)
    option(ARA_ENABLE_COM_TIMER "Enable ara COM timer" ON)
    option(ARA_ENABLE_COM_SOMEIP_DAEMON "build ara with com someip daemon " ON)
    option(ARA_ENABLE_COM_FSOMEIP "Enable ara com fsomeip binding" OFF)
    option(ARA_ENABLE_COM_EVENT_SEND_TRIGGER "Enable ara com event send trigger" OFF)
    option(ARA_ENABLE_COM_EVENT_LOST_CHECKER "Enable ara com event lost checker" OFF)
    option(ARA_ENABLE_COM_SECOC "Enable ara com secoc" OFF)
endif(ARA_ENABLE_COM)

# ================ ara-diag     ================
option(ARA_ENABLE_DIAG "Enable the AP diag module" ON)
option(ARA_ENABLE_DIAG_DIAGNOSTIC_GATEWAY "Enable the AP diagnostic gateway module" OFF)
option(ARA_ENABLE_UDS_SERVER "Enable the AP uds server module" ${ARA_ENABLE_DIAG})
option(ARA_ENABLE_DIAG_DAEMON "build ara with diag daemon " ${ARA_ENABLE_DIAG})
option(ARA_ENABLE_DIAG_DOIP_CLIENT "Enable the AP doip client" OFF)
option(ARA_ENABLE_DIAG_DOIP_SERVER "build ara with doip server" ${ARA_ENABLE_DIAG})
option(ARA_ENABLE_DIAG_UDS_SERVER "build ara with uds server" ${ARA_ENABLE_DIAG})
option(ARA_ENABLE_DIAG_DOIP_DAEMON "build ara with doip daemon " OFF) # legacy, removes in 3.0
option(ARA_ENABLE_DIAG_UDSGW_DAEMON "build ara with uds-gateway deamon" OFF) # for guo-qi-zhi-kong only, removes in 3.0
option(ARA_ENABLE_DIAG_DOIP_BUILD_SHARED_LIBS "build ara doip as shared libs" ${ARA_BUILD_SHARED_LIBS})
option(ARA_ENABLE_DIAG_UDS_BUILD_SHARED_LIBS "build ara uds as shared libs" ${ARA_BUILD_SHARED_LIBS})

# =============== ara-core     ================
option(ARA_ENABLE_CORE "Enable the AP core module" ON)

# =============== ara-em       ================
option(ARA_ENABLE_EXEC "Enable the AP em module" ON)
if(ARA_ENABLE_EXEC)
    option(ARA_ENABLE_DEP "Enable Deterministic execution feature" OFF)
    option(ARA_ENABLE_DETERMINISTIC_EXECUTION "enable deterministic execution" OFF)
endif(ARA_ENABLE_EXEC)

# ================ ara-sm       ================
option(ARA_ENABLE_SM "build ara with sm" ON)
option(ARA_ENABLE_SM_DAEMON "build ara with sm daemon" ${ARA_ENABLE_SM})
option(ARA_ENABLE_SM_BUILD_FG_SHARED_LIBS "build ara with sm shared libs" ${ARA_BUILD_SHARED_LIBS})
option(ARA_ENABLE_SM_BUILD_UTILS_SHARED_LIBS "build ara with sm shared libs" ${ARA_BUILD_SHARED_LIBS})

# ================ ara-phm      ================
option(ARA_ENABLE_PHM "build ara with phm" ON)
option(ARA_ENABLE_PHM_DAEMON "build ara with phm daemon" ${ARA_ENABLE_PHM})

# =============== ara-log       ================
option(ARA_ENABLE_LOG "Enable the AP log module" ON)
if(ARA_ENABLE_LOG)
    option(ARA_ENABLE_LOG_DAEMON "Enable the AP log daemOFF module" ON)
    option(ARA_ENABLE_LOG_LIB "Enable the log moudle feature - XX" ON)
endif(ARA_ENABLE_LOG)

# =============== ara-crypto     ================
option(ARA_ENABLE_CRYPTO "build ara with crypto" ON)
if(ARA_ENABLE_CRYPTO)
    option(ARA_ENABLE_CRYPTO_BUILD_SHARED_LIBS "build ara-crypto as shared libs" ${ARA_BUILD_SHARED_LIBS})
    option(ARA_ENABLE_CRYPTO_KEY_DAEMON "build ara with crypto-key daemon" ON)
    option(ARA_ENABLE_CRYPTO_X509_DAEMON "build ara with crypto-x509 daemon" ON)
    option(ARA_ENABLE_CRYPTO_PLUGIN "Enable build with crypto_plugin" OFF)
    option(ARA_ENABLE_CRYPTO_HSM_NXP "build ara with crypto-hsm-nxp" OFF)
    option(ARA_ENABLE_CRYPTO_HSM_NXP_DAEMON "build ara with crypto-hsm-nxp daemon" OFF)
    option(ARA_ENABLE_CRYPTO_HSM_BST "build ara with crypto-hsm-bst" OFF)
    option(ARA_ENABLE_CRYPTO_HSM_BST_DAEMON "build ara with crypto-hsm-bst daemon" OFF)
    option(ARA_ENABLE_CRYPTO_PLUGIN "Enable build with crypto_plugin" OFF)
endif(ARA_ENABLE_CRYPTO)

# ================ ara-fw       ================
option(ARA_ENABLE_FW "build ara with firewall" ON)
option(ARA_ENABLE_FW_DAEMON "build ara with firewall daemon" ${ARA_ENABLE_FW})
option(ARA_ENABLE_FW_ENGINE_BUILD_SHARED_LIBS "build ara-fw-engine as shared libs" ${ARA_BUILD_SHARED_LIBS})
option(ARA_ENABLE_FW_CLIENT_BUILD_SHARED_LIBS "build ara-fw-client as shared libs" ${ARA_BUILD_SHARED_LIBS})

# ================ ara-iam      ================
option(ARA_ENABLE_IAM "build ara with iam" ON)
option(ARA_ENABLE_IAM_BUILD_SHARED_LIBS "build ara-iam as shared libs" ${ARA_BUILD_SHARED_LIBS})

# ================ ara-idsm     ================
option(ARA_ENABLE_IDSM "build ara with idsm" ON)
option(ARA_ENABLE_IDSM_DAEMON "build ara with idsm daemon" ${ARA_ENABLE_IDSM})
option(ARA_ENABLE_IDSM_PROTO_BUILD_SHARED_LIBS "build ara idsm proto as shared libs" ${ARA_BUILD_SHARED_LIBS})
option(ARA_ENABLE_IDSM_CLIENT_BUILD_SHARED_LIBS "build ara idsm client as shared libs" ${ARA_BUILD_SHARED_LIBS})

# ================ ara-per      ================
option(ARA_ENABLE_PER "build ara with per" ON)
option(ARA_ENABLE_PER_DAEMON "build ara with idsm daemon" OFF)
option(ARA_ENABLE_PER_BUILD_SHARED_LIBS "build ara per client as shared libs" ${ARA_BUILD_SHARED_LIBS})

# ================ ara-tsync ===================
option(ARA_ENABLE_TSYNC "build ara with tsync" ON)
option(ARA_ENABLE_TSYNC_DAEMON "build ara with tsync daemon" ${ARA_ENABLE_TSYNC})
if(ARA_ENABLE_TSYNC AND ARA_ENABLE_TSYNC_DAEMON)
    if(NOT ARA_ENABLE_PER)
        message(WARNING "tsyncd storage module requires PER, build may fail!")
    endif()
endif()

# =============== ara-nm      ==================
option(ARA_ENABLE_NM "Enable the AP nm module" ON)

# ================ ara-ucm      ================
option(ARA_ENABLE_UCM "build ara with ucm" ON)
option(ARA_ENABLE_VUCM "build ara with ucm master" ${ARA_ENABLE_UCM})

# ================ ara-isoft    ================
option(ARA_ENABLE_ISOFT_NPC "Enable isoft npc" ${ARA_ENABLE_COM_NPC})
option(ARA_ENABLE_ISOFT_SERIALIZE "Enable isoft serialize" ON)

# Configuration validation
function(config_validation)
    if(ARA_ENABLE_DIAG_DAEMON AND NOT ARA_ENABLE_DIAG)
        message(WARNING "DIAG_DAEMON requires DIAG module, disabling daemon")
        set(ARA_ENABLE_DIAG_DAEMON OFF CACHE BOOL "" FORCE)
    endif()
    # TODO: Implement more configuration validation logic...
endfunction(config_validation)
