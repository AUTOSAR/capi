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
# @date       2022-06-02
# @author     junwei.he
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.10)


# project(ara-log VERSION 0.1.0 LANGUAGES CXX)
project(none)
include(../gen/aragen-helper.cmake)

message(STATUS "ARA_WITH_EXEC=${ARA_WITH_EXEC}")
if (${ARA_WITH_EXEC} STREQUAL ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_NM_WITH_EXEC=1")
endif()

message(STATUS "ARA_WITH_COM=${ARA_WITH_COM}")
message(STATUS "ARA_WITH_DIAG=${ARA_WITH_DIAG}")
if (${ARA_WITH_DIAG} STREQUAL ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_NM_WITH_DIAG=1")
endif()

message(STATUS "ARA_WITH_COM=${ARA_WITH_COM}")
if (${ARA_WITH_COM} STREQUAL ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_NM_WITH_COM=1")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_NM_WITHOUT_COM=1")
endif()

# Set the list of arxml files used by the current project: ARA_GEN_ARXMLS
# Can be passed by cmake parameters (e.g: cmake -DARA_GEN_ARXMLS=/var/xx1/;/var/xx2/)
# Can also be added, deleted, or modified here
if((${ARA_WITH_COM} STREQUAL ON) AND (${ARA_WITH_DIAG} STREQUAL ON))
message(STATUS "ARA_GEN_ARXMLS ARA_WITH_COM=${ARA_WITH_COM} ARA_WITH_DIAG=${ARA_WITH_DIAG}")
set(ARA_GEN_ARXMLS ${ARA_GEN_ARXMLS} 
        ${ARA_ARXMLS_DIR}/common
        ${ARA_ARXMLS_DIR}/development_machines/system
        ${ARA_ARXMLS_DIR}/development_machines/diagnostic_extract
        ${ARA_ARXMLS_DIR}/development_machines/machine1/common
        ${ARA_ARXMLS_DIR}/development_machines/machine2/common


        ${ARA_ARXMLS_DIR}/platform_components/nm
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/nmd 
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/nmd 

        #only for depends process staus and debug(to gen manifest)
        ${ARA_ARXMLS_DIR}/platform_components/com/daemon
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/nsomeipd 
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/nsomeipd 

        # dm
        ${ARA_ARXMLS_DIR}/platform_components/diag/daemon
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/dmd
) # arxmls
elseif((${ARA_WITH_COM} STREQUAL ON) AND (${ARA_WITH_DIAG} STREQUAL OFF))
message(STATUS "ARA_GEN_ARXMLS ARA_WITH_COM=${ARA_WITH_COM} ARA_WITH_DIAG=${ARA_WITH_DIAG}")
set(ARA_GEN_ARXMLS ${ARA_GEN_ARXMLS} 
        ${ARA_ARXMLS_DIR}/common
        ${ARA_ARXMLS_DIR}/development_machines/system
        ${ARA_ARXMLS_DIR}/development_machines/machine1/common
        ${ARA_ARXMLS_DIR}/development_machines/machine2/common


        ${ARA_ARXMLS_DIR}/platform_components/nm
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/nmd 
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/nmd 

        #only for depends process staus and debug(to gen manifest)
        ${ARA_ARXMLS_DIR}/platform_components/com/daemon
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/nsomeipd 
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/nsomeipd 

        # dm
        ${ARA_ARXMLS_DIR}/platform_components/diag/daemon
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/dmd
) # arxmls
elseif((${ARA_WITH_COM} STREQUAL OFF) AND (${ARA_WITH_DIAG} STREQUAL ON))
message(STATUS "ARA_GEN_ARXMLS ARA_WITH_COM=${ARA_WITH_COM} ARA_WITH_DIAG=${ARA_WITH_DIAG}")
set(ARA_GEN_ARXMLS ${ARA_GEN_ARXMLS} 
        ${ARA_ARXMLS_DIR}/common
        ${ARA_ARXMLS_DIR}/development_machines/system
        ${ARA_ARXMLS_DIR}/development_machines/diagnostic_extract
        ${ARA_ARXMLS_DIR}/development_machines/machine1/common
        ${ARA_ARXMLS_DIR}/development_machines/machine2/common


        ${ARA_ARXMLS_DIR}/platform_components/nm/daemon/application_design.arxml
        ${ARA_ARXMLS_DIR}/platform_components/nm/daemon/diag_interface.arxml
        ${ARA_ARXMLS_DIR}/platform_components/nm/daemon/nm_diag_port.arxml
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/nmd/integration/process.arxml 
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/nmd/integration/process.arxml
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/nmd/integration/mapping.arxml 
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/nmd/integration/mapping.arxml 

        #only for depends process staus and debug(to gen manifest)
        ${ARA_ARXMLS_DIR}/platform_components/com/daemon
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/nsomeipd 
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/nsomeipd 

        # dm
        ${ARA_ARXMLS_DIR}/platform_components/diag/daemon
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/dmd
) # arxmls
else()
message(STATUS "ARA_GEN_ARXMLS ARA_WITH_COM=${ARA_WITH_COM} ARA_WITH_DIAG=${ARA_WITH_DIAG}")
set(ARA_GEN_ARXMLS ${ARA_GEN_ARXMLS} 
        ${ARA_ARXMLS_DIR}/common
        ${ARA_ARXMLS_DIR}/development_machines/system
        ${ARA_ARXMLS_DIR}/development_machines/machine1/common
        ${ARA_ARXMLS_DIR}/development_machines/machine2/common


        ${ARA_ARXMLS_DIR}/platform_components/nm/daemon/application_design.arxml
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/nmd/integration/process.arxml 
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/nmd/integration/process.arxml
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/nmd/integration/mapping.arxml 
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/nmd/integration/mapping.arxml

        #only for depends process staus and debug(to gen manifest)
        ${ARA_ARXMLS_DIR}/platform_components/com/daemon
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/nsomeipd 
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/nsomeipd 

        # dm
        ${ARA_ARXMLS_DIR}/platform_components/diag/daemon
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/dmd
) # arxmls
endif()

# Add ARA_GEN_BUILD_HELPER_LIBRARY target
aragen_helper_add_build_helper_library() 

# This file is generated by ara-gen(1.0.0) from arxml
if (${ARA_WITH_DIAG} STREQUAL ON)
    if (${ARA_WITH_COM} STREQUAL ON)
        set(EXE_src
            config/configure.cpp
            protcl/ethersocket.cpp
            protcl/nmmsg.cpp
            nmd/main.cpp
            nmd/daemon.cpp
            nmm/udpnm.cpp
            nmm/logicnetworkoper.cpp
            nmm/partialnetworkoper.cpp
            nmm/diagdtcserver.cpp
            nmm/nmetherstatemachine.cpp
            utils/netcard.cpp
            smi/sminterface.cpp
            smi/nmipcproc.cpp
        )
    else()
        set(EXE_src
            config/configure.cpp
            protcl/ethersocket.cpp
            protcl/nmmsg.cpp
            nmd/main.cpp
            nmd/daemon.cpp
            nmm/udpnm.cpp
            nmm/logicnetworkoper.cpp
            nmm/partialnetworkoper.cpp
            nmm/diagdtcserver.cpp
            nmm/nmetherstatemachine.cpp
            utils/netcard.cpp
            smi/nmipcproc.cpp
        )
    endif()
else()
    if (${ARA_WITH_COM} STREQUAL ON)
        set(EXE_src
            config/configure.cpp
            protcl/ethersocket.cpp
            protcl/nmmsg.cpp
            nmd/main.cpp
            nmd/daemon.cpp
            nmm/udpnm.cpp
            nmm/logicnetworkoper.cpp
            nmm/partialnetworkoper.cpp
            nmm/nmetherstatemachine.cpp
            utils/netcard.cpp
            smi/sminterface.cpp
            smi/nmipcproc.cpp
        )
    else()
        set(EXE_src
            config/configure.cpp
            protcl/ethersocket.cpp
            protcl/nmmsg.cpp
            nmd/main.cpp
            nmd/daemon.cpp
            nmm/udpnm.cpp
            nmm/logicnetworkoper.cpp
            nmm/partialnetworkoper.cpp
            nmm/nmetherstatemachine.cpp
            utils/netcard.cpp
            smi/nmipcproc.cpp
        )
    endif()
endif()

set(EXE_target ${ARA_GEN_EXECUTABLE})
add_executable(${EXE_target} ${EXE_src})

include_directories(${EXE_target}
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/../../ethoper/src
)

set_target_properties(${EXE_target}
    PROPERTIES
        OUTPUT_NAME ${ARA_GEN_EXECUTABLE}
    )

target_link_libraries(${EXE_target}
    pthread
    ara::core
    ara::log
    nai
    isoft::naicpp
    ara::nmethoper
    ${ARA_GEN_BUILD_HELPER_LIBRARY}
)
add_dependencies(${EXE_target} ${ARA_GEN_BUILD_HELPER_LIBRARY})

# isoft-ap unified handling of target
aragen_helper_register_target(TARGET ${EXE_target} TYPE EXE)