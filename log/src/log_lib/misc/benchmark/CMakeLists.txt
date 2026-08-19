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
# @file       CMakeLists.txt
# @brief
# @details
# @date       2025-09-03
# @author     jinbiao.yang
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.16)
project(ara-log-benchmark VERSION 0.1.0 LANGUAGES CXX)

find_package(ara-log REQUIRED)
find_package(ara-core-types REQUIRED)
find_package(ara-core REQUIRED)
find_package(ara-exec-execution-client REQUIRED)

# Remove include paths pointing to non-existent directories in sysroot, use internal ara/log
get_target_property(exec_include_dirs ara::exec::execution_client INTERFACE_INCLUDE_DIRECTORIES)

if(exec_include_dirs)
    list(FILTER exec_include_dirs EXCLUDE REGEX "ara/framework.*include/ara/log")
    set_target_properties(ara::exec::execution_client PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${exec_include_dirs}")
endif()

find_package(isoft-cmake-common REQUIRED)

set(TARGET_NAME ${PROJECT_NAME})

if(NOT DEFINED ARA_WITH_LOG_DAEMON)
    set(ARA_WITH_LOG_DAEMON OFF)
endif()

if(ARA_WITH_LOG_DAEMON)
    message("starting logd")
endif()

set(EXE_src
    src/main.cpp
    src/eventloop.cpp
    src/base_tester.cpp
    src/ecu_logger_common.cpp
    src/api_benchmark.cpp
    src/main_logic.cpp
    src/signal_handler.cpp
    src/sys_benchmark.cpp
    src/api_single.cpp
    src/ini_parser.cpp
)

add_executable(${TARGET_NAME} ${EXE_src})

target_include_directories(${TARGET_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${TARGET_NAME}
    PRIVATE
    ara::core-types
    ara::log
    ara::core
    nai
    isoft::naicpp
    isoft::ipc_em
    ara::exec::execution_client
)

# Installation rules
install(TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)