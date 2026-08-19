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
# @date       2026-03-23
# @author     jinbiao.yang
# @version    1.2.0
#
# ================================================================

set(EMLIB "")

if(ARA_WITH_EXEC)
    message("-----------------------Enable EM----------------")
    set(ARA_GEN_ARA_LIBS ara::exec::execution_client)
    find_package(ara-exec-execution-client REQUIRED)

    # Remove include paths pointing to non-existent directories in sysroot, use internal ara/log
    get_target_property(exec_include_dirs ara::exec::execution_client INTERFACE_INCLUDE_DIRECTORIES)

    if(exec_include_dirs)
        list(FILTER exec_include_dirs EXCLUDE REGEX "ara/framework.*include/ara/log")
        set_target_properties(ara::exec::execution_client PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${exec_include_dirs}")
    endif()

    add_definitions(-DARA_WITH_EXEC)
    set(EMLIB isoft::ipc_em)
endif()

message(STATUS "DltBackend: ARA_WITH_TSYNC = ${ARA_WITH_TSYNC}")

# set(TsyncLibName "")
# if(ARA_WITH_TSYNC)
# message("logd have tsync ")
# find_package(ara-tsync REQUIRED)
# add_definitions(-DARA_WITH_TSYNC)
# message("Include time synchronization -----------")
# set(TsyncLibName "ara::tsync")
# endif()
message(STATUS "ARA_WITH_EXEC is ${ARA_WITH_EXEC}")

# ### ---------------Integrated variables related to modeling configuration------------------------
set(DAEMON_NAME ${ARA_GEN_EXECUTABLE})
message(STATUS "======== Begin : ${PROJECT_NAME}.${DAEMON_NAME} ========")
message(STATUS "  **  CMAKE_CURRENT_LIST_DIR    = ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "  **  CMAKE_CURRENT_SOURCE_DIR  = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS " ***  ARA_LOG_ROOT_DIR          = ${ARA_LOG_ROOT_DIR} ")

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(BUILD_SHARED_LIBS ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS YES)
else()
    set(CMAKE_VERBOSE_MAKEFILE OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS NO)
endif()

file(GLOB_RECURSE DAEMON_SRCS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/logd/*.cpp")

set(LOGD_SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
list(APPEND LOGD_SRC_LIST ${DAEMON_SRCS})

# Output the list of source files involved in compilation
message(STATUS "<<<<< SRC_DIR = ${CMAKE_CURRENT_SOURCE_DIR}, SRC_FILE_LIST: ")

foreach(src_file IN LISTS LOGD_SRC_LIST)
    message(STATUS "  **  srcFile = ${src_file}")
endforeach()

message(STATUS ">>>>>")

# Keep the original executable building
add_executable(${DAEMON_NAME} ${LOGD_SRC_LIST})
target_sources(${DAEMON_NAME} PRIVATE
    $<TARGET_OBJECTS:ara::log-utils>
)
target_include_directories(${DAEMON_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/logd/
    ${ARA_LOG_ROOT_DIR}/
    ${ARA_LOG_ROOT_DIR}/log_lib/include
)

set_target_properties(${DAEMON_NAME}
    PROPERTIES
    OUTPUT_NAME ${ARA_GEN_EXECUTABLE}
)

target_link_libraries(${DAEMON_NAME}
    PUBLIC
    ara::core-types
    nai
    isoft::ara_fsh
    ${LIBTHREAD}
    ${LIBRT}
    ${LIBSOCKET}
    ${LIBANDROIDLOG}
    z
    isoft::naicpp
    ${EMLIB}
    isoft::ipcbase
    isoft::ipccpp
    RapidJSON
    ${ARA_GEN_ARA_LIBS}
    ${ARA_GEN_BUILD_HELPER_LIBRARY}
)

if(ARA_ENABLE_TESTS)
    add_dependencies(${DAEMON_NAME} ${ARA_GEN_BUILD_HELPER_LIBRARY})
    set(buildName tcp-server-lib)

    # Add static library target for testing
    add_library(tcp-server-lib STATIC ${DAEMON_SRCS})
    target_sources(tcp-server-lib PRIVATE
        $<TARGET_OBJECTS:ara::log-utils>
    )
    target_compile_definitions(${buildName}
        PRIVATE
        TEST_BUILD_STATIC=1)

    target_include_directories(${buildName}
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src/
        ${CMAKE_CURRENT_LIST_DIR}/../
    )

    target_link_libraries(${buildName}
        PUBLIC

        ${EMLIB}
        isoft::ipcbase
        isoft::ipccpp
        RapidJSON
        isoft::naicpp
        isoft::ara_fsh
        ara::core-types
        nai
        ${LIBTHREAD}
        ${LIBRT}
        ${LIBSOCKET}
        ${LIBANDROIDLOG}
        ${ARA_GEN_BUILD_HELPER_LIBRARY}
        z
    )

    add_dependencies(${buildName} ${ARA_GEN_BUILD_HELPER_LIBRARY})
endif()

# isoft-ap unified handling of target
aragen_helper_register_target(TARGET ${DAEMON_NAME} TYPE EXE)

message(STATUS "======== End   : ${PROJECT_NAME}.${DAEMON_NAME} ========")