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
# @brief      cmake configuration file
# @details
# @date       2023-10-12
# @author     junwei.he
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.16)
project(ara-log-lib VERSION 0.1.0 LANGUAGES CXX)

# If ARA_LOG_ROOT_DIR is not defined, set it to the parent directory of this file
if(NOT DEFINED ARA_LOG_ROOT_DIR)
    get_filename_component(ARA_LOG_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../" ABSOLUTE)
    message(STATUS "ARA_LOG_ROOT_DIR not defined, setting to: ${ARA_LOG_ROOT_DIR}")
endif()

include(FindPkgConfig)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(CheckCXXSourceCompiles)

# Try to compile a small piece of code to test whether the macro exists
check_cxx_source_compiles("
    int main() {
    #ifndef __linux__
        #error Not Linux
    #endif
        return 0;
    }"
    COMPILER_HAS_LINUX_MACRO
)

# First check if it is a Linux system
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    if(ANDROID)
        set(IS_ANDROID 1)
    else()
        set(IS_LINUX 1)
    endif()
endif()

# Whether these compilation switches are defined in ap-all is currently undetermined
option(ARA_LOG_HAS_REMOTE "ara log module has remote log feature" ON)
option(ARA_LOG_HAS_DEP "ara log module has determExcution feature" ON)
option(ARA_LOG_INTERNAL_IPC_DEBUG "ara log module has internal ipc debug feature" OFF)
option(ARA_LOG_HAS_COMPRESS "ara log module has compress feature" ON)
option(ARA_LOG_HAS_PLAINTEXT "ara log module has plaintext feature" ON)
option(ARA_LOG_HAS_FILE_CHECKER "ara log module has file checker feature" OFF)
option(ARA_ENABLE_DOXYGEN "Enables the documentation target" OFF)
option(BUILD_SHARED_LIBS "Enables the BUILD_SHARED_LIBS target" ON)
option(ARA_WITH_LOG_BUILD_SHARED_LIBS "Build ara log module as shared library" ON)

if(ARA_ENABLE_TESTS)
    add_subdirectory(test)
endif()

message("-------------------" ${CMAKE_SOURCE_DIR})
message("-------------------" ${PROJECT_SOURCE_DIR})
set(rootDir ${PROJECT_SOURCE_DIR}/../../)
message("rootdir " ${rootDir})

find_package(isoft-cmake-common REQUIRED)
find_package(isoft-common REQUIRED)
find_package(nai REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(isoft-ipc REQUIRED)
find_package(isoft-ara-fsh REQUIRED)
find_package(isoft-manifestreader REQUIRED)
find_package(ara-core-types REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(concurrentqueue REQUIRED)
find_package(ZLIB REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_definitions(-DARA_LOG_DEBUG)
else()
    add_definitions(-DARA_LOG_RELEASE)
endif()

include_directories(${ARA_LOG_ROOT_DIR})

# When compiling independently, need to add Utils subdirectory
if(NOT Utils_add)
    message(STATUS "Standalone build: Adding Utils subdirectory")
    add_subdirectory(${ARA_LOG_ROOT_DIR}/Utils ${CMAKE_CURRENT_BINARY_DIR}/Utils)
    set(Utils_add TRUE)
endif()

add_subdirectory(src)