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
# @file       dependencies.cmake
# @brief
# @details
# @date       2025-12-25
# @author     chejinzhao
# @version    1.2.0
#
# ================================================================

find_package(isoft-cmake-common REQUIRED) 
# Define a function: Enable ARA Crypto debug mode for the specified target
function(add_precompiled_macros TARGET_NAME)
    # When not using log modeling messages, this macro must be set; otherwise, log compilation will report an error.
    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            ARA_LOG_WITHOUT_MODEL_MESSAGE=1
    )
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_LOG_WITHOUT_MODEL_MESSAGE=1")

    # Set compilation flags
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fno-strict-aliasing -Wall -Werror")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fno-strict-aliasing -Wall -Wextra -Wsign-promo -Wpedantic -Wnon-virtual-dtor -Wctor-dtor-privacy -Woverloaded-virtual -Wold-style-cast -Wpointer-arith -Wshadow -Wunused -Wuninitialized -Winit-self -Wdeprecated -Wfloat-equal")

    if(ARA_CRYPTO_DEBUG)
        # Add debug macro definitions
        target_compile_definitions(${TARGET_NAME}
            PRIVATE
                ARA_DEBUG=1
                ARA_CRYPTO_DEBUG=1
        )
        message(STATUS "${PROJECT_NAME}.${TARGET_NAME} Building in Debug Mode")
    endif()
endfunction()

function(setup_find_packages)
    cmake_minimum_required(VERSION 3.16)
    # "I recognize two flag options: IN_DEAMON and IN_TEST"
    # If IN_DEAMON is passed, set PACKAGE_IN_DEAMON=TRUE
    # If IN_TEST is passed, set PACKAGE_IN_TEST=TRUE
    set(options IN_ARA_CRYPTO IN_DEAMON IN_TEST IN_PLUGIN WITH_IAM)  # Flags
    cmake_parse_arguments(PACKAGE # prefix, use "_" if none
        "${options}"  # flag options
        ""            # no single-value options
        ""            # no multi-value options
        ${ARGN}       # passed arguments
    )
    # Configure compilation flags
    # set(CMAKE_CXX_STANDARD 14)
    # set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    # Include header file search directories
    # include(FindPkgConfig)
    # include(CMakePackageConfigHelpers)
    # Precompiled macros

    # First configure common libraries
    find_package(isoft-ipc REQUIRED)
    find_package(isoft-naicpp REQUIRED)

    if(PACKAGE_IN_DEAMON)
        # Some daemon executables require libara_crypto_ph.so
        find_package(ara-crypto REQUIRED)
        find_package(ara-core REQUIRED)
        find_package(ara-exec-find-process-client REQUIRED)
        find_package(ara-per REQUIRED)
    endif()

    if(PACKAGE_WITH_IAM)
        # Decide whether to decouple IAM based on the top-level CMakeLists
        if(ARA_WITH_IAM)
            find_package(ara-iam REQUIRED)
        endif()
    endif()

    if(PACKAGE_IN_TEST) 
        find_package(ara-core-types REQUIRED)
        find_package(isoft-crc REQUIRED)
        find_package(nai REQUIRED)
        find_package(GTest REQUIRED CONFIG) # Depend on GTest library
        find_library(GMOCK gmock)           # Both GTest libraries are required
    endif()

    if(PACKAGE_IN_PLUGIN)
        find_package(ara-core REQUIRED)
        find_package(OpenSSL 3.5.1 REQUIRED)
        # 3. Check whether OpenSSL 3.5.1 was found successfully
        if(OPENSSL_FOUND)
            message(STATUS "Using OpenSSL ${OPENSSL_VERSION} (${OPENSSL_INCLUDE_DIR}, ${OPENSSL_LIBRARIES})")
        else()
            message(FATAL_ERROR "OpenSSL 3.5.1 not found!")
        endif()
        find_package(isoft-crc REQUIRED)
    endif()

    if(PACKAGE_IN_DEAMON OR PACKAGE_IN_TEST OR PACKAGE_IN_PLUGIN)
        find_package(ara-exec-execution-client REQUIRED)
    endif()

    if(PACKAGE_IN_ARA_CRYPTO)
        find_package(nai REQUIRED)
        find_package(ara-log REQUIRED)
        find_package(ara-core-types REQUIRED)
        find_package(isoft-manifestreader REQUIRED)
        find_package(isoft-crc REQUIRED)
        find_package(isoft-ara-fsh REQUIRED)
    endif()

    # Depend on OpenSSL library
    if (${CMAKE_SYSTEM_NAME} MATCHES "QNX")
    else()
        find_package(OpenSSL REQUIRED)
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        if(ANDROID)
            set(__android__ 1)
            add_compile_definitions(-D__android__)
        else()
            set(__linux__ 1)
        endif()
    endif()
    
    if (${CMAKE_SYSTEM_NAME} MATCHES "QNX")
        set(__QNX__ 1)
    endif()
    message(STATUS "  **  ===xxx=== ${PROJECT_NAME}.CMAKE_CXX_STANDARD  = ${CMAKE_CXX_STANDARD}")
endfunction()

# Dependent libraries
set(ARA_NEED_LIBS
	ara::core-types
    ara::log
    isoft::crc
    isoft::manifestreader
    isoft::ara_fsh
	isoft::ipc
	isoft::naicpp
    nai
)

set(SSL_NEED_LIBS
    ssl
    crypto
)

set(PLUGIN_DEAMON_NEED_LIBS
    isoft::ipccpp  
    isoft::naicpp
    ara::ara_crypto
    ara::core
    ara::exec::execution_client 
)

set(BST_NEED_LIBS
	cybersecurity
	teec
)

SET(PER_NEED_LIBS
    ara::per
)

SET(EXEC_NEED_LIBS
    ara::exec::find_process_client
)

set(TEST_NEED_LIBS
    ara::core-types
    ara::ara_crypto
    nai
    GTest::gtest_main
    ara::exec::execution_client
)

function(set_link_libraries TARGET_NAME)
    set(options 
        IN_ARA_CRYPTO   # crypto library
        IN_DEAMON       # demo program
        IN_TEST_PUHUA   # test program
        IN_TEST_X509
        IN_TEST_HSMNXP
        IN_PLUGIN    # plugin library
        WITH_BST     # requires BST library
        WITH_IAM     
    )  
    
    set(multiValueArgs EXTRA_LIBS)  # multi-value parameter: receives extra library list, EXTRA_LIBS value is ${ARA_GEN_BUILD_HELPER_LIBRARY}
    
    cmake_parse_arguments(LINK # prefix, use "_" if none
        "${options}"  # flag options
        ""            # no single-value options
        "${multiValueArgs}"            # multi-value options
        ${ARGN}       # passed arguments
    )

    # 1. Start from scratch
    set(final_libs "")

    # If it is the crypto library case
    if(LINK_IN_ARA_CRYPTO) 
        list(APPEND final_libs ${ARA_NEED_LIBS})
        list(APPEND final_libs ${SSL_NEED_LIBS})
    endif()

    # If it is the daemon program case
    if(LINK_IN_DEAMON) 
        list(APPEND final_libs ${PLUGIN_DEAMON_NEED_LIBS})
        list(APPEND final_libs ${PER_NEED_LIBS})
        list(APPEND final_libs ${EXEC_NEED_LIBS})
        list(APPEND final_libs ${SSL_NEED_LIBS})
        list(APPEND final_libs ${LINK_EXTRA_LIBS})
    endif()

    # Decouple IAM
    if(ARA_WITH_IAM)
        target_compile_definitions(${TARGET_NAME}
            PRIVATE
                ARA_WITH_IAM=1
        )
    endif()
    # Daemon has decoupling requirement, WITH_IAM = TRUE
    if(LINK_WITH_IAM) 
        # Decide whether to decouple IAM based on the top-level CMakeLists
        if(ARA_WITH_IAM)
                list(APPEND final_libs ara::iam)
        endif()
    endif()
    
    # If it is the plugin library case
    if(LINK_IN_PLUGIN) 
        list(APPEND final_libs ${PLUGIN_DEAMON_NEED_LIBS})
        list(APPEND final_libs ${SSL_NEED_LIBS})
    endif()

    # If it is the gtest program case
    if(LINK_IN_TEST_PUHUA)
        list(APPEND final_libs ${TEST_NEED_LIBS})
        list(APPEND final_libs lib-isoft)
    endif()

    # If it is the gtest program case
    if(LINK_IN_TEST_X509)
        list(APPEND final_libs ${TEST_NEED_LIBS})
        list(APPEND final_libs lib-x509)
    endif()

    # If it is the gtest program case
    if(LINK_IN_TEST_HSMNXP)
        list(APPEND final_libs ${TEST_NEED_LIBS})
        list(APPEND final_libs lib-hsmnxp)
    endif()

    # If it requires BST
    if(LINK_WITH_BST) 
        list(APPEND final_libs ${BST_NEED_LIBS})
    endif()

    # Perform linking. IN_ARA_CRYPTO requires PUBLIC/PRIVATE etc., others are exe and do not need.
    if(LINK_IN_ARA_CRYPTO OR LINK_IN_PLUGIN)
        target_link_libraries(${TARGET_NAME}
                ${final_libs}
        )
        message(STATUS "${PROJECT_NAME}.${TARGET_NAME} link_libraries: ${final_libs}")
    else()
        target_link_libraries(${TARGET_NAME}
                ${final_libs}
        )
        message(STATUS "${PROJECT_NAME}.${TARGET_NAME} link_libraries: ${final_libs}")
    endif()
endfunction()

# Function: print_source_list(<source_list> <log_label>)
#
# Parameter description:
#   source_list : list of source files (passed directly, not a variable name)
#   log_label   : label used in logs, e.g., "srcFileName"
function(print_source_list source_list log_label)
    message(STATUS "<<<<< SRC_DIR = ${CMAKE_CURRENT_SOURCE_DIR}, SRC_FILE_LIST: ")
    foreach(src_file IN LISTS source_list)
        message(STATUS "  **  ${log_label} = ${src_file}")
    endforeach()
    message(STATUS ">>>>>")
endfunction()