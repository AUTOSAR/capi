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

# 本文件由ara-gen(1.1.0)根据arxml生成，随着 arxml的更新，该文件应重新生成（即不应直接修改该文件）
#################### ara-gen 生成点begin ####################
macro(aragen_helper_add_project)
    # Project信息：Design
    if(NOT DEFINED ARA_GEN_EXECUTABLE_FQN)
        set(ARA_GEN_EXECUTABLE_FQN /ClientApp/exe/clientd)
    endif()
    get_filename_component(ARA_GEN_EXECUTABLE ${ARA_GEN_EXECUTABLE_FQN} NAME)

    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug")
    endif()

    if(NOT DEFINED ARA_GEN_EXECUTABLE_VERSION)
        set(ARA_GEN_EXECUTABLE_VERSION 1.1.0) # 来自EXECUTABE version字段
    endif()

    # Define cmake project
    project(${ARA_GEN_EXECUTABLE} VERSION ${ARA_GEN_EXECUTABLE_VERSION} LANGUAGES CXX)

    # Import base cmake modules
    find_package(isoft-cmake-common REQUIRED)
    find_package(ara-arxmls REQUIRED)

    # Set arxml models path
    set(ARA_GEN_ARXMLS ${ARA_GEN_ARXMLS}) # arxmls

    # Setup build defines
    #  依赖的lib
    set(ARA_GEN_ARA_LIBS ara::core ara::exec::execution_client)
    # 循环依赖的lib(目前仅 com/cm-bind/cm-gen-code 有该项)
    set(ARA_GEN_ARA_LIBS_LOOP_REFERENCE ara_com ara_com_nsomeip)

    #set(ENABLE_SINGLE_PRCESS_MODE ON)

    # 如果service interface 有设置 someip部署，开启如下设置
    set(USE_NSOMEIP2 ON)
    set(HAS_SOMEIP_BINDING ON)


    # Import depends ara libs
    find_package(ara-gen REQUIRED)
    find_package(ara-core REQUIRED)
    find_package(ara-exec-execution-client REQUIRED)

    find_package(ara-com REQUIRED)
    find_package(ara-com-nsomeip REQUIRED)
endmacro()
#################### ara-gen 生成点end ####################
aragen_helper_add_project()
