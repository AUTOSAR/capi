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
# @file        Modules.cmake
# @brief      
# @details
# @date        2026-01-01
# @author      jian.feng
# @version     0.1
# @description Configure cmake projects for AP modules. Decide which AP modules 
#              or proprietary dependency libraries to add to external project based
#              n configuration.
# ================================================================

include(ExternalProject)
include(Utils)
include(Config)

# Prevent multiple inclusions
include_guard(GLOBAL)

# Third-party dependency configuration
function(module_check_dependency)
    # Mandatory dependencies
    # find_package(RapidJSON REQUIRED)
    # Optional dependencies
    # if(ARA_ENABLE_SAMPLES)
    #     find_package(Boost 1.60.0 REQUIRED COMPONENTS system filesystem thread program_options)
    #     find_package(Poco COMPONENTS Foundation Zip REQUIRED)
    # endif()

    # if(ARA_ENABLE_EXEC)
    #     # Find system packages
    #     find_package(PkgConfig REQUIRED)
    #     pkg_search_module(PKG_CGROUP libcgroup)

    #     if(NOT PKG_CGROUP_FOUND)
    #         message(WARNING "libcgroup package not found, EXEC module may have limited functionality")
    #     endif()

    #     # Check libseccomp
    #     pkg_search_module(PKG_SECCOMP libseccomp)

    #     if(NOT PKG_SECCOMP_FOUND)
    #         message(WARNING "libseccomp not found, seccomp features will be disabled")
    #     endif()
    # endif()

endfunction(module_check_dependency)



# =============================================================================
# _module_config_projects
# -----------------------------------------------------------------------------
#
# Function description:
# Used to dynamically set the project (project) names to be included based on CMake configuration options (ARA_ENABLE_xxx, etc.).
# Each PRJ_xxx variable corresponds to a specific module project; these variables will be used externally to add to the CMake project list.
#
# Working logic:
# 1. First, set basic libraries (such as isoft-fsh, etc.) as default dependencies for all ARA modules
# 2. Set corresponding project variables according to the switch conditions of each module (if statements)
# 3. All PRJ_xxx variables will be passed to the caller scope after the function execution
# 4. If a module is not defined, the corresponding PRJ_xxx variable will not be set, and the related project will not be included
#
# Variable naming convention:
# - PRJ_ prefix: indicates this is a project name variable
# - Variable value: corresponds to the specific CMake project target name
#
# Legacy module description:
# - Modules marked "legacy, removes in 3.0" will be removed in version 3.0
# - Modules marked "for guo-qi-zhi-kong only" are custom modules for specific customers
#
# Usage example:
# Later, can determine whether to add the corresponding project by checking if(PRJ_xxx)
#
# Note:
# 1. All ARA_ENABLE_xxx variables should be defined before calling this macro function
#

macro(_module_config_projects)
    # Common basic dependency libraries for all ara modules, defined as a list to avoid redundant information when configuring project dependencies
    set(PRJ_ARA_BASE_LIBRARIES
        isoft-common
        isoft-cmake-common
        isoft-nai
        isoft-naicpp
        isoft-ipc
        isoft-ipcbase
        isoft-fsh
        isoft-manifestreader
        ara-core-types
    )

    if(ARA_ENABLE_NML)
        set(PRJ_ISOFT_NML "isoft-nml")
    endif()

    # AP module configuration
    if(ARA_ENABLE_CORE)
        set(PRJ_ARA_CORE "ara-core")
    endif()

    if(ARA_ENABLE_EXEC)
        set(PRJ_ARA_EXEC "ara-exec")
    endif()

    # ================ ara-sm       ================
    if(ARA_ENABLE_SM)
        set(PRJ_ARA_SM "ara-sm")
        set(PRJ_ARA_SM_FG_STATE_NOTIFY_CLIENT "ara-sm-fg-state-notify-client")
        set(PRJ_ARA_SM_UTILS "ara-sm-utils")
        set(PRJ_ARA_SM_RESET_MACHINE "ara-sm-reset-machine")
        set(PRJ_ARA_SM_SHUTDOWN_MACHINE "ara-sm-shutdown-machine")
        set(PRJ_ARA_SM_SHELL_COMMAND_CLIENT "ara-sm-shell-command-client")
    endif()

    # ================ ara-phm      ================
    if(ARA_ENABLE_PHM)
        set(PRJ_ARA_PHM_CLIENT "ara-phm-client")
        set(PRJ_ARA_PHM_RECOVERY_ACTION "ara-phm-recovery-action")
    endif()
    if(ARA_ENABLE_PHM_DAEMON)
        set(PRJ_ARA_PHM_DAEMON "ara-phm-daemon")
    endif()
    if(ARA_ENABLE_TSYNC)
        set(PRJ_ARA_TSYNC "ara-tsync")
    endif()
    if(ARA_ENABLE_TSYNC_DAEMON)
        set(PRJ_ARA_TSYNC_DAEMON "ara-tsync-daemon")
    endif()

    if(ARA_ENABLE_LOG_LIB)
        set(PRJ_ARA_LOG_LIB "ara-log")
    endif()
    if(ARA_ENABLE_LOG_DAEMON)
        set(PRJ_ARA_LOG_DAEMON "ara-log-daemon")
    endif()

    if(ARA_ENABLE_NM)
        set(PRJ_ARA_NM "ara-nm")
        set(PRJ_ARA_NM_ETHOPER "ara-nm-ethoper")
    endif()

    if(ARA_ENABLE_PER)
        set(PRJ_ARA_PER "ara-per")
    endif()

    # ================ ara-ucm      ================
    if(ARA_ENABLE_UCM)
        set(PRJ_ARA_UCM_LIBS "ara-ucm-libs")
        set(PRJ_ARA_UCM_DAEMON "ara-ucm-daemon")
    endif()
    if(ARA_ENABLE_VUCM)
        set(PRJ_ARA_VUCM_DAEMON "ara-vucm-daemon")
    endif()

    if(ARA_ENABLE_IAM)
        set(PRJ_ARA_IAM "ara-iam")
    endif()

    # ================ ara-crypto   ================
    if(ARA_ENABLE_CRYPTO)
        set(PRJ_ARA_CRYPTO "ara-crypto")
    endif()
    if(ARA_ENABLE_CRYPTO_KEY_DAEMON)
        set(PRJ_ARA_CRYPTO_DAEMON_KEYS "ara-crypto-daemon-keys")
    endif()
    if(ARA_ENABLE_CRYPTO_X509_DAEMON)
        set(PRJ_ARA_CRYPTO_DAEMON_X509 "ara-crypto-daemon-x509")
    endif()
    if(ARA_ENABLE_CRYPTO_HSM_NXP_DAEMON)
        set(PRJ_ARA_CRYPTO_HSM_NXP "ara-crypto-daemon-hsm-nxp")
    endif()
    if(ARA_ENABLE_CRYPTO_HSM_BST_DAEMON)
        set(PRJ_ARA_CRYPTO_HSM_BST "ara-crypto-daemon-hsm-bst")
    endif()

    # ================ ara-idsm     ================
    if(ARA_ENABLE_IDSM)
        set(PRJ_ARA_IDSM_CLIENT "ara-idsm-client")
        set(PRJ_ARA_IDS_PROTO "ara-ids-proto")
    endif()
    if(ARA_ENABLE_IDSM_DAEMON)
        set(PRJ_ARA_IDSM_DAEMON "ara-idsm-daemon")
    endif()

    # ================ ara-diag     ================
    if(ARA_ENABLE_DIAG)
        set(PRJ_ARA_DIAG "ara-diag")
        set(PRJ_ARA_DIAG_COMMON "diag-common")
    endif()
    if(ARA_ENABLE_DIAG_DAEMON)
        set(PRJ_ARA_DIAG_DAEMON "ara-diag-daemon")
    endif()
    if(ARA_ENABLE_DIAG_DOIP_SERVER OR ARA_ENABLE_DIAG_DOIP_CLIENT)
        set(PRJ_ARA_DIAG_DOIP "ara-diag-doip")
    endif()
    if(ARA_ENABLE_DIAG_UDS_SERVER)
        set(PRJ_ARA_DIAG_UDS_SERVER "ara-diag-uds-server")
    endif()

    if(ARA_ENABLE_DIAG_UDSGW_DAEMON)
        set(PRJ_ISOFT_UDSGW_DAEMON "isoft-udsgw-daemon")
    endif()

    if(ARA_ENABLE_COM)
        set(PRJ_ARA_COM "ara-com")
        if(ARA_ENABLE_COM_SOMEIP_DAEMON)
            set(PRJ_ARA_COM_NSOMEIP_NPC_DAEMON "ara-com-nsomeip-npc-daemon")
        endif()

        if(ARA_ENABLE_COM_CG_DAEMON)
            set(PRJ_ARA_COM_CG_DAEMON "ara-com-cg-daemon")
        endif()
    endif()

    if(ARA_ENABLE_DEP)
        set(PRJ_ISOFT_DEP "isoft-dep")
        set(PRJ_ISOFT_DEP_GEN "dep-gen")
    endif()

    if(ARA_ENABLE_FW)
        set(PRJ_ARA_FW_CLIENT "ara-fw-client")
        set(PRJ_ARA_FW_ENGINE "ara-fw-engine")
    endif()
    if(ARA_ENABLE_FW_DAEMON)
        set(PRJ_ARA_FW_DAEMON "ara-fw-daemon")
    endif()

    # Basic libraries
    if(ARA_ENABLE_COM OR ARA_ENABLE_PER)
        set(PRJ_ISOFT_CRC "isoft-crc")
        set(PRJ_ISOFT_SERIALIZE "isoft-serialize")
    endif()

    if(ARA_ENABLE_COM)
        set(PRJ_ISOFT_CRC "isoft-crc")
        set(PRJ_ISOFT_E2E "isoft-e2e")
        if(ARA_ENABLE_COM_NSOMEIP)
            set(PRJ_ISOFT_NSOMEIP "isoft-nsomeip")
            if(ARA_ENABLE_COM_NPC)
                set(PRJ_ISOFT_NPC "isoft-npc")
            endif()
        endif()
    endif(ARA_ENABLE_COM)

    set(PRJ_ISOFT_ARA_GEN isoft-ara-gen)

    if(ARA_ENABLE_V3)
        set(PRJ_ISOFT_ARA_GEN_V3 "isoft-ara-gen-v3")
        set(PRJ_ISOFT_ARA_TOOLS_V3 "isoft-ara-tools-v3")
        set(PRJ_ISOFT_SYS_MONITOR_V3 "isoft-sys-monitor-v3")
        set(PRJ_ISOFT_MODEL_DEFINITION_V3 "isoft-model-definition-v3")
        set(PRJ_ISOFT_MODEL_UPGRADE_V3 "isoft-model-upgrade-v3")
        set(PRJ_ISOFT_ARA_MODEL "isoft-ara-model-v3")
        if(ARA_ENABLE_MANUAL)
            set(PRJ_ISOFT_USER_MANUAL_V3 "isoft-user-manual-v3")
        endif()
    else()
        if(ARA_ENABLE_TOOLS)
            set(PRJ_ISOFT_ARA_TOOLS isoft-ara-tools)
        endif()
    endif()

endmacro(_module_config_projects)



# Record all configured project names. last_project will depend on them.
set(MODULE_ALL_PROJECTS)


# =============================================================================
# _module_add_project
# -----------------------------------------------------------------------------
#
# Define and build an ARA (AUTOSAR Runtime for Adaptive Applications) module.
# This function checks if the module directory exists, registers the module to a global property, and uses ExternalProject_Add
# to manage the module's build process, supporting parallel compilation.
#
# Parameter description:
#   [OPTIONAL]     - Option parameter: if set, the module directory not existing is treated as a warning rather than an error.
#   NAME           - Single-value parameter: (required) Unique name of the module, also used as the target name for ExternalProject.
#   DIRECTORY      - Single-value parameter: (required) Path of the module source code relative to the current source directory.
#   DEPENDS        - Multi-value parameter: (optional) List of other CMake targets (modules managed by apall) that this module depends on.
#
# Return value:
#   No direct return value. However, the module name (ARA_MODULES) will be recorded in a GLOBAL property.
#   If the module builds successfully, an ExternalProject target named ${ARGS_NAME} will be created.
#
# Example usage:
#   _module_add_project(
#       NAME vehicle_controller
#       DIRECTORY modules/vehicle
#       DEPENDS common_lib diagnostics
#       CMAKE_ARGS -DBUILD_TESTING=ON -DLOG_LEVEL=INFO
#   )
#
#   Add an optional module (no error if directory does not exist):
#   _module_add_project(
#       NAME experimental_feature
#       DIRECTORY modules/experimental
#       OPTIONAL
#   )
#
# Important notes:
#   1. The function internally depends on get_cpu_core_count to set the number of parallel compilation jobs (cores - 2)
#   2. Uses ExternalProject_Add to manage builds; ensure the ExternalProject module is correctly included
#   3. The module directory must exist under CMAKE_CURRENT_SOURCE_DIR
#   4. The INSTALL_COMMAND uses degraded handling (make install || true), may need adjustment based on platform
#
# =============================================================================
function(_module_add_project)
    set(option_args)
    set(oneValueArgs NAME DIRECTORY)
    set(multiValueArgs DEPENDS TARGETS)

    cmake_parse_arguments(ARGS "${option_args}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Configuration check; if no project name is configured, exit normally without terminating cmake execution
    if(NOT ARGS_NAME)
        return()
    endif()

    # Existence check; if the directory does not exist, report an error and terminate the entire cmake execution
    if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${ARGS_DIRECTORY}")
        # Report error and stop further configuration
        message(FATAL_ERROR "Module directory ${ARGS_DIRECTORY} not found for ${ARGS_NAME}")
    endif()

    if(TARGETS)
        set(MODULE_BUILD_TARGETS "--target ${ARGS_TARGETS}")
        set(MODULE_INSTALL_COMPONENTS "--component ${ARGS_TARGETS}")
    endif()

    if(DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
        set(MODULE_BUILD_JOBS ${CMAKE_BUILD_PARALLEL_LEVEL})
        message("JOBS from CMAKE_BUILD_PARALLEL_LEVEL: ${MODULE_BUILD_JOBS}")
    else()
        utils_get_cpu_core_count(CORE_COUNT)
        math(EXPR MODULE_BUILD_JOBS "${CORE_COUNT} - 2")
        message("JOBS from CPU_CORE: ${MODULE_BUILD_JOBS}")
    endif()

    # Get all currently cached variables and pass them to the external project.
    # Only pass configuration variables starting with ARA_, avoiding internal CMake variables and pkg-config results
    utils_get_cmake_args_from_cache_vars(MODULE_CMAKE_ARGS EXCLUDE_PREFIX "CMAKE_HOME_DIRECTORY")

    if(ARA_ENABLE_V3)
        list(APPEND MODULE_CMAKE_ARGS
            "-DORIENTAIS_TOOL_INSTALLATION_PATH=${ARA_SYSROOT}/ara-tools"
            "-DAP_ALL_PATH=${PROJECT_SOURCE_DIR}"
        )
        set(ARA_MAJOR_VERSION 3)
    endif()

    ExternalProject_Add(${ARGS_NAME}
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${ARGS_DIRECTORY}"
        CMAKE_ARGS ${MODULE_CMAKE_ARGS}
        DEPENDS ${ARGS_DEPENDS}
        # No need to customize the configuration command; if customization is necessary, be sure to manually pass the ${MODULE_CMAKE_ARGS} parameters
        # CONFIGURE_COMMAND ${CMAKE_COMMAND} <SOURCE_DIR> -B <BINARY_DIR> ${MODULE_CMAKE_ARGS}
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> -j ${MODULE_BUILD_JOBS} ${MODULE_BUILD_TARGETS}
        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> ${MODULE_INSTALL_COMPONENTS} || true #TODO: Some modules report errors during installation but do not affect usage; temporarily ignore, need to fix errors later
    )

    # Add the project to the list for establishing dependencies for deployment_machine
    list(APPEND MODULE_ALL_PROJECTS ${ARGS_NAME})
    set(MODULE_ALL_PROJECTS ${MODULE_ALL_PROJECTS} PARENT_SCOPE)

    message(STATUS "Registered module: ${ARGS_NAME}")

endfunction(_module_add_project)


# TODO: Remove external projects, replace with executing commands?
function(_module_deployment_machine)
    set(DEPLOYMENT_PROJECT_DIR "${CMAKE_BINARY_DIR}/deployment-project")
    file(WRITE ${DEPLOYMENT_PROJECT_DIR}/CMakeLists.txt "project(deployment_project)")
    set(MODULE_BUILD_CMD
        ${ARA_SYSROOT}/ara-tools/configMachine
        -r ${ARA_SYSROOT}
        -m ${ARA_DEST_MACHINE}
        -e ${ARA_GEN_EXECUTABLE_OUTPUT}
        -w ${DEPLOYMENT_PROJECT_DIR}
        -v
        ${CMAKE_INSTALL_PREFIX}/share/ara-arxmls
    )

    # Create configuration project
    ExternalProject_Add("isoft-deployment-project"
        SOURCE_DIR ${DEPLOYMENT_PROJECT_DIR}
        CONFIGURE_COMMAND echo "deployment machine command: ${MODULE_BUILD_CMD}"
        BUILD_COMMAND "${MODULE_BUILD_CMD}"
        INSTALL_COMMAND echo "Development environment configured"
        DEPENDS ${MODULE_ALL_PROJECTS}
    )
endfunction(_module_deployment_machine)



# =============================================================================
# module_add_all_projects
# -----------------------------------------------------------------------------
# Configure and build all enabled ara and isoft software modules.
#
# Function description:
# This function, in order of dependencies between modules, configures and builds all modules defined (enabled) by PRJ_XXX. It automatically handles inter-module dependencies, ensuring dependent modules are built before the target module.
#
# Module build order:
# 1. Basic tools and common libraries
# 2. ARA toolchain and code generators
# 3. Core basic libraries (IPC, serialization, communication, etc.)
# 4. AP module libraries (in dependency order)
# 5. Daemon and service modules
# 6. V3 specific modules (if applicable)
#
# Dependency management:
# - Use the DEPENDS parameter to explicitly declare inter-module dependencies
# - Basic libraries (PRJ_ARA_BASE_LIBRARY) are automatically included in relevant module dependencies
# - Ensure dependent modules are built first to avoid linking errors
#
# Special handling description:
# - Daemon modules are built separately from their corresponding library modules to avoid initialization conflicts
# - V3 modules are built only when ARA_ENABLE_V3 is enabled
# - Legacy modules are marked to be removed in version 3.0
#
# Example output:
#   [STATUS] Configuring ARA modules...
#   [STATUS] Registered module: isoft-cmake-common
#   [STATUS] Registered module: isoft-ini-file
#   ... (more module registration information)
#
# Important notes:
# 1. This function must be called after PRJ_XXX variables have been processed
# 2. All module paths are relative to CMAKE_CURRENT_SOURCE_DIR
# 3. Module dependencies must be declared correctly, otherwise build will fail
# 4. Daemon modules need to be compiled separately to avoid ara::core::Initialize conflicts
#
# Related functions:
#   _module_config_projects() - Configure module switches and project variables
#   _module_add_project()       - Low-level implementation for adding a single module
#
# =============================================================================
function(module_add_all_projects)
    message(STATUS "Configuring ARA modules...")

    # Define projects based on module configuration
    _module_config_projects()

    # ara tools and models
    _module_add_project(NAME isoft-cmake-common DIRECTORY isoft/apcommon-cmake-modules)
    _module_add_project(NAME ${PRJ_ISOFT_ARA_TOOLS} DIRECTORY isoft/ara-tools)
    _module_add_project(NAME ${PRJ_ISOFT_ARA_BUILDER} DIRECTORY isoft/ara-builder)
    _module_add_project(NAME ${PRJ_ISOFT_ARA_GEN} DIRECTORY isoft/ara-gen DEPENDS isoft-cmake-common)
    _module_add_project(NAME isoft-arxmls DIRECTORY isoft/arxmls DEPENDS isoft-cmake-common ${PRJ_ISOFT_ARA_GEN})

    # Basic dependency libraries
    _module_add_project(NAME ara-core-types DIRECTORY core/src/core-types DEPENDS isoft-cmake-common)
    _module_add_project(NAME isoft-common DIRECTORY isoft/common DEPENDS isoft-cmake-common ara-core-types ${PRJ_ISOFT_ARA_GEN} isoft-arxmls)
    _module_add_project(NAME isoft-manifestreader DIRECTORY isoft/manifestreader DEPENDS ara-core-types isoft-common)
    _module_add_project(NAME isoft-nai DIRECTORY isoft/nai DEPENDS isoft-common)
    _module_add_project(NAME ${PRJ_ISOFT_NML} DIRECTORY isoft/nml DEPENDS isoft-common)
    _module_add_project(NAME isoft-fsh DIRECTORY isoft/ara-fsh DEPENDS isoft-common isoft-nai)
    _module_add_project(NAME isoft-naicpp DIRECTORY isoft/naicpp DEPENDS isoft-common isoft-nai ara-core-types)
    _module_add_project(NAME isoft-ipcbase DIRECTORY isoft/ipcbase DEPENDS isoft-nai isoft-common isoft-fsh)
    _module_add_project(NAME isoft-ipc DIRECTORY isoft/ipc DEPENDS isoft-common isoft-ipcbase isoft-naicpp)
    _module_add_project(NAME ${PRJ_ISOFT_CRC} DIRECTORY isoft/crc DEPENDS isoft-common)
    _module_add_project(NAME ${PRJ_ISOFT_E2E} DIRECTORY isoft/e2e DEPENDS isoft-common)
    _module_add_project(NAME ${PRJ_ISOFT_SERIALIZE} DIRECTORY isoft/serialize DEPENDS isoft-common ara-core-types)
    _module_add_project(NAME ${PRJ_ISOFT_NPC} DIRECTORY isoft/npc DEPENDS isoft-common isoft-nai isoft-fsh)
    _module_add_project(NAME ${PRJ_ISOFT_NSOMEIP} DIRECTORY isoft/nsomeip DEPENDS isoft-common isoft-nai ${PRJ_ISOFT_NPC})


    # AP module Libs 
    _module_add_project(NAME ${PRJ_ARA_TSYNC} DIRECTORY tsync/src/tsync_client REPO tsync DEPENDS ${PRJ_ARA_BASE_LIBRARIES})
    _module_add_project(NAME ${PRJ_ARA_LOG_LIB} DIRECTORY log/src/log_lib DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_TSYNC})
    _module_add_project(NAME ${PRJ_ARA_CORE} DIRECTORY core/src/core DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB})
    _module_add_project(NAME ${PRJ_ARA_EXEC} DIRECTORY exec DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB})
    _module_add_project(NAME ${PRJ_ARA_IAM} DIRECTORY iam DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_EXEC})
    _module_add_project(NAME ${PRJ_ARA_CRYPTO} DIRECTORY crypto/src DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_IAM})
    _module_add_project(NAME ${PRJ_ARA_PER} DIRECTORY per DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ISOFT_CRC} ${PRJ_ARA_CRYPTO} ${PRJ_ISOFT_SERIALIZE})
    _module_add_project(NAME ${PRJ_ARA_IDSM_CLIENT} DIRECTORY idsm/src/idsm_client DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB})
    _module_add_project(NAME ${PRJ_ARA_IDS_PROTO} DIRECTORY idsm/src/ids_proto DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_CRYPTO})

    _module_add_project(NAME ${PRJ_ARA_PHM_CLIENT} DIRECTORY phm/src/phm_client DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_IAM} ${PRJ_ARA_EXEC})

    _module_add_project(NAME ${PRJ_ARA_PHM_RECOVERY_ACTION} DIRECTORY phm/src/recovery_action DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_IAM} ${PRJ_ARA_EXEC})

    _module_add_project(NAME ${PRJ_ARA_UCM_LIBS} DIRECTORY ucm/ucmlibs DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_PER})
    _module_add_project(NAME ${PRJ_ARA_FW_ENGINE} DIRECTORY fw/src/libfwengine DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB})
    _module_add_project(NAME ${PRJ_ARA_FW_CLIENT} DIRECTORY fw/src/libfwclient DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_IDSM})
    _module_add_project(NAME ${PRJ_ARA_COM} DIRECTORY com
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ISOFT_E2E} ${PRJ_ISOFT_NSOMEIP} ${PRJ_ISOFT_NPC} ${PRJ_ARA_LOG_LIB} ${PRJ_ISOFT_CRC} ${PRJ_ARA_IAM} ${PRJ_ARA_CRYPTO} ${PRJ_ARA_EXEC})


    _module_add_project(NAME ${PRJ_ARA_DIAG_COMMON} DIRECTORY diag/common DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ISOFT_NSOMEIP})
    _module_add_project(NAME ${PRJ_ARA_DIAG} DIRECTORY diag/dm/src/adaptive-diagnostic-library DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_PER} ${PRJ_ISOFT_NSOMEIP} ${PRJ_ARA_DIAG_COMMON} ${PRJ_ISOFT_NML})

    _module_add_project(NAME ${PRJ_ARA_DIAG_DOIP} DIRECTORY diag/isoft-doip DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB})

    _module_add_project(NAME ${PRJ_ARA_DIAG_UDS_SERVER} DIRECTORY diag/isoft-uds DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_DIAG_COMMON})

    _module_add_project(NAME ${PRJ_ARA_NM_ETHOPER} DIRECTORY nm/src/ethoper DEPENDS ${PRJ_ARA_CORE} ${PRJ_ARA_LOG_LIB})
    _module_add_project(NAME ${PRJ_ARA_NM} DIRECTORY nm
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_CORE} ${PRJ_ARA_EXEC} ${PRJ_ARA_COM} ${PRJ_ARA_DIAG} ${PRJ_ARA_NM_ETHOPER} ${PRJ_ARA_LOG_LIB})


    _module_add_project(NAME ${PRJ_ARA_SM_UTILS} DIRECTORY sm/sm/src/utils DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_EXEC} ${PRJ_ARA_LOG} ${PRJ_ISOFT_E2E})

    _module_add_project(NAME ${PRJ_ARA_SM_FG_STATE_NOTIFY_CLIENT} DIRECTORY sm/sm/src/fg_state_notify_client DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_SM_UTILS})

    # AP module Daemons
    # Regarding separate compilation of daemon
    #     Because ara::core::Initialize initializes ARA (each module LIB), all modules that need to be initialized in ara::core::Initialize should compile daemon and lib separately (refer to log && log/daemon)
    _module_add_project(NAME ${PRJ_ARA_SM} DIRECTORY sm/sm/src/smd
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_EXEC} ${PRJ_ARA_PER} ${PRJ_ARA_COM} ${PRJ_ISOFT_FSH} ${PRJ_ARA_PHM_RECOVERY_ACTION} ${PRJ_ARA_PHM_CLIENT} ${PRJ_ARA_DIAG} ${PRJ_ARA_NM} ${PRJ_ARA_SM_UTILS})

    _module_add_project(NAME ${PRJ_ARA_SM_RESET_MACHINE} DIRECTORY sm/app/src/reset_machine
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_EXEC})

    _module_add_project(NAME ${PRJ_ARA_SM_SHUTDOWN_MACHINE} DIRECTORY sm/app/src/shutdown_machine
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_EXEC})

    _module_add_project(NAME ${PRJ_ARA_TSYNC_DAEMON} DIRECTORY tsync DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_EXEC} ${PRJ_ARA_PER})

    _module_add_project(NAME ${PRJ_ARA_COM_NSOMEIP_NPC_DAEMON} DIRECTORY com/routing DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_COM} ${PRJ_ARA_CORE})

    _module_add_project(NAME ${PRJ_ARA_COM_CG_DAEMON} DIRECTORY com/cg DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_COM})

    _module_add_project(NAME ${PRJ_ARA_DIAG_DAEMON} DIRECTORY diag/dm/src/adaptive-diagnostic-manager
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_EXEC} ${PRJ_ARA_DIAG} ${PRJ_ARA_DIAG_DOIP_SERVER} ${PRJ_ARA_DIAG_UDS_SERVER} ${PRJ_ISOFT_NML})

    _module_add_project(NAME ${PRJ_ISOFT_UDSGW_DAEMON} DIRECTORY diag/uds-gateway
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_EXEC} ${PRJ_ARA_DIAG_DOIP_CLIENT})

    _module_add_project(NAME ${PRJ_ARA_FW_DAEMON} DIRECTORY fw/src/fwd DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_EXEC} ${PRJ_ARA_IDSM_CLIENT} ${PRJ_ARA_FW_ENGINE})

    _module_add_project(NAME ${PRJ_ARA_IDSM_DAEMON} DIRECTORY idsm/src/idsmd
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_EXEC} ${PRJ_ARA_DIAG} ${PRJ_ARA_COM} ${PRJ_ARA_IAM} ${PRJ_ARA_IDSM_CLIENT} ${PRJ_ARA_IDS_PROTO} ${PRJ_ARA_SM_FG_STATE_NOTIFY_CLIENT} ${PRJ_ARA_TSYNC})

    _module_add_project(NAME ${PRJ_ARA_SM_SHELL_COMMAND_CLIENT} DIRECTORY sm/app/src/shell_command_client
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_COM} ${PRJ_ARA_LOG_LIB})

    _module_add_project(NAME ${PRJ_ARA_CRYPTO_DAEMON_KEYS} DIRECTORY crypto/src/cryptod
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_EXEC} ${PRJ_ARA_CRYPTO} ${PRJ_ARA_PER} ${PRJ_ARA_LOG_LIB})

    _module_add_project(NAME ${PRJ_ARA_CRYPTO_DAEMON_X509} DIRECTORY crypto/src/cryptox509d
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_EXEC} ${PRJ_ARA_CRYPTO} ${PRJ_ARA_PER} ${PRJ_ARA_LOG_LIB})

    _module_add_project(NAME ${PRJ_ARA_CRYPTO_HSM_NXP} DIRECTORY crypto/src/hsm_nxp/cryptohsmd
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_EXEC} ${PRJ_ARA_CRYPTO} ${PRJ_ARA_PER} ${PRJ_ARA_LOG_LIB})

    _module_add_project(NAME ${PRJ_ARA_CRYPTO_HSM_BST} DIRECTORY crypto/src/hsm_bst/cryptohsmbstd
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_EXEC} ${PRJ_ARA_CRYPTO} ${PRJ_ARA_PER} ${PRJ_ARA_LOG_LIB})

    _module_add_project(NAME ${PRJ_ARA_PHM_DAEMON} DIRECTORY phm/src/phmd
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_EXEC} ${PRJ_ARA_PHM} ${PRJ_ARA_SM_FG_STATE_NOTIFY_CLIENT} ${PRJ_ARA_IAM} ${PRJ_ARA_LOG_LIB})

    _module_add_project(NAME ${PRJ_ARA_LOG_DAEMON} DIRECTORY log/src/log_daemon DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_CORE} ${PRJ_ARA_EXEC} ${PRJ_ARA_LOG_LIB})

    _module_add_project(NAME ${PRJ_ARA_UCM_DAEMON} DIRECTORY ucm/pkgmgr
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_UCM_LIBS} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_EXEC} ${PRJ_ARA_CORE} ${PRJ_ARA_COM} ${PRJ_ARA_CRYPTO})

    _module_add_project(NAME ${PRJ_ARA_VUCM_DAEMON} DIRECTORY ucm/vpkgmgr
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_UCM_LIBS} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_EXEC} ${PRJ_ARA_CORE} ${PRJ_ARA_COM} ${PRJ_ARA_CRYPTO})


    _module_add_project(NAME ${PRJ_ISOFT_DEP} DIRECTORY isoft/dep/libs
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_LOG_LIB} ${PRJ_ARA_COM})

    _module_add_project(NAME ${PRJ_ISOFT_DEP_GEN} DIRECTORY isoft/dep/depGen
        DEPENDS ${PRJ_ARA_BASE_LIBRARIES})


    # V3 module specific
    if(ARA_ENABLE_V3)
        _module_add_project(NAME ${PRJ_ISOFT_ARA_GEN_V3} DIRECTORY isoft/ara-genv3)
        _module_add_project(NAME ${PRJ_ISOFT_ARA_TOOLS_V3} DIRECTORY isoft/ara-toolsv3)

        _module_add_project(NAME ${PRJ_ISOFT_SYS_MONITOR_V3} DIRECTORY isoft/ara-toolsv3/tools/areCmdSysMonitor DEPENDS ${PRJ_ARA_BASE_LIBRARIES} ${PRJ_ARA_COM})
        _module_add_project(NAME ${PRJ_ISOFT_MODEL_DEFINITION_V3} DIRECTORY isoft/n-model-definition/utility/model-convert)
        _module_add_project(NAME ${PRJ_ISOFT_MODEL_UPGRADE_V3} DIRECTORY isoft/n-model-definition/utility/model-upgrade)
        _module_add_project(NAME ${PRJ_ISOFT_ARA_MODEL} DIRECTORY isoft/ara-model)
        if(ARA_ENABLE_MANUAL)
            _module_add_project(NAME ${PRJ_ISOFT_USER_MANUAL_V3} DIRECTORY isoft/user-manual)
        endif()
    endif()
    # Machine configuration and installation deployment
    if(ARA_ENABLE_APENV)
        _module_deployment_machine()
    endif()

endfunction(module_add_all_projects)
