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
# @date       2021-07-19
# @author     jiangzhiyuan
# @version    1.2.0
#
#
# ================================================================

set(LIBRARY_NAME ara-iam)       # Library file name and package name
set(LIBRARY_FILE_NAME ara_iam)  # Base name of the library file
set(TARGET_NAME iam)            # Target name changed to iam
set(EXPORT_NAME iam)            # Export set name changed to iam
set(ALIA_NAME ara::iam)         # Alias remains unchanged
#iam debug mode.
set(ARA_IAM_DEBUG FALSE)

include(CMakePackageConfigHelpers)

# Dependencies to other ara parts
find_package(ara-log REQUIRED)
find_package(ara-core-types REQUIRED)
find_package(isoft-manifestreader REQUIRED)
find_package(isoft-ara-fsh REQUIRED)

# iam is deployed independently, does not require exec library.
if(ARA_ENABLE_EXEC OR ARA_WITH_EXEC)
    find_package(ara-exec-find-process-client REQUIRED)
    set(PACKAGE_ARA_EXEC_DEPENDENCY "find_dependency(ara-exec-find-process-client)")
    message(STATUS "IAM find Process client!")
    add_definitions(-DARA_WITH_EXEC)
endif()


file(GLOB_RECURSE SRC_PH_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp" )
    
message(STATUS "<<<<< SRC_PH_LIST = ${CMAKE_CURRENT_SOURCE_DIR}")

# List of files to compile: alternative way of writing
# 4. Unified build shared library (includes all conditionally appended target files)
if(ARA_WITH_IAM_BUILD_SHARED_LIBS)
	add_library(${TARGET_NAME} SHARED ${SRC_PH_LIST})
else()
    add_library(${TARGET_NAME} STATIC ${SRC_PH_LIST})
endif()

add_library(${ALIA_NAME} ALIAS ${TARGET_NAME})

message(STATUS "${PROJECT_NAME}.${TARGET_NAME}: CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(${TARGET_NAME}
	PUBLIC
        ara::core-types
    PRIVATE
        ${ADAPTIVEPLATFORM_LIBRARIES}
        isoft::manifestreader
        isoft::ara_fsh
        ara::log
)

#Add iam compatibility mode.
if (ARA_WITH_EXEC OR ARA_IAM_DEBUG) 
target_link_libraries(${TARGET_NAME}
    PRIVATE
    ara::exec::find_process_client
)
endif()


message(STATUS "${PROJECT_NAME}.${TARGET_NAME}: PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")

#debug debugging (add ARA_WITH_EXEC compatibility)
if(ARA_IAM_DEBUG)
    target_compile_definitions(${TARGET_NAME} 
        PRIVATE 
            ARA_IAM_DEBUG=1 
            ARA_IAM_DEBUG_WITHOUT_EM=1
            ARA_WITH_EXEC
    )
    target_compile_options(${TARGET_NAME} PRIVATE -g)
endif()



##================== Installation configuration related: if it is independent deployment, this link needs to be cleared.
if(NOT ARA_WITH_EXEC)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                   COMMAND   sed -i 's/find_dependency\(ara-exec-find-process-client\)//g' ${PROJECT_SOURCE_DIR}/misc/${LIBRARY_NAME}-config.cmake.in )
endif()

set_target_properties(${TARGET_NAME}
    PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
	OUTPUT_NAME ara_${LIBRARY_NAME}
)

## Create *.pc file
if(PKG_CONFIG_FOUND)
    configure_file(${PROJECT_SOURCE_DIR}/misc/ara-iam.pc.in ${LIBRARY_NAME}.pc @ONLY)
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()


# Create *-config.cmake file
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/misc/${LIBRARY_NAME}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME} #
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
    COMPATIBILITY ExactVersion
)

# Install *-config.cmake and *-version.cmake files
install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)



# Install header files
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${LIBRARY_FILE_NAME}
    EXPORT_NAME ${EXPORT_NAME}
)


# Install lib
install(
    TARGETS ${TARGET_NAME}
    EXPORT ${EXPORT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ara/iam
)

# Install targets.cmake
install(
   EXPORT ${EXPORT_NAME}
   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
   NAMESPACE ara::
   FILE ara-iam-targets.cmake
)