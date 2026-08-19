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
# @author     jiawei
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.4)

project(ara-ucm-libs LANGUAGES CXX C VERSION 0.1.0)

# modified by hanzhibo
# # Initialize compilation options, and define common CMake Functions & variables (AP platform)

include(FindPkgConfig)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
find_package(isoft-cmake-common REQUIRED) 

# Import ARA
message(STATUS "Importing Dependencies")
include(GNUInstallDirs)

# Threads using pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(THREADS_PTHREAD_ARG "2" CACHE STRING "Forcibly set to circumvent cross-compiler bug in cmake <=3.10" FORCE)
find_package(Threads)

# find_package(Boost 1.60.0 REQUIRED COMPONENTS system filesystem thread)
# find_package(Poco COMPONENTS Foundation Zip REQUIRED)

find_package(ara-crypto REQUIRED)

find_package(ara-log REQUIRED)
find_package(ara-per REQUIRED)
find_package(ara-core-types REQUIRED) #added by hanzhibo
find_package(isoft-manifestreader REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(isoft-ipc REQUIRED)


add_subdirectory(src)

if(ARA_ENABLE_TESTS)
	enable_testing()
	find_package(GTest REQUIRED CONFIG)
	add_subdirectory(test)
endif(ARA_ENABLE_TESTS)

install(
	EXPORT ${PROJECT_NAME}-targets
	NAMESPACE ara::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
	DIRECTORY include/ara
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	COMPONENT dev
)
