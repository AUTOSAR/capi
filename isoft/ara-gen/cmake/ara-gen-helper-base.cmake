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

# AP  App   cmake project 辅助 cmake 函数

#  集成所有对ara的依赖为单一的 library target (自动添加依赖的 libs,includes，以及 代码生成target, 配置生成target) 
include(CMakeParseArguments)
function(_aragen_helper_add_target)
	set(oneValueArgs DESTINATION EXECUTABLE TARGET)
	set(multiValueArgs ARXMLS)
	cmake_parse_arguments(helper_ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

     set(_GeneratedTargent)

    FILE(WRITE ${helper_ARGS_DESTINATION}/helper_libary.cpp "")
    set(_GeneratedSrc ${helper_ARGS_DESTINATION}/helper_libary.cpp)

    if(HAS_FASTDDS_BINDING)

        add_fastdds(
            DESTINATION ${helper_ARGS_DESTINATION}
            TARGET ${helper_ARGS_TARGET}_CodeGenerated
            EXECUTABLE ${helper_ARGS_EXECUTABLE}
            ARXMLS ${helper_ARGS_ARXMLS}
        )

        set(_GeneratedSrc ${${helper_ARGS_TARGET}_CodeGenerated_SOURCES})
        list(APPEND _GeneratedSrc $<TARGET_OBJECTS:${helper_ARGS_TARGET}_CodeGeneratedLib>)
        if( FASTDDS_ROS2)
            set(ARA_GEN_ARA_CXX_FLAGS -DFASTDDS_ROS2)
        endif()
        set(ARA_GEN_ARA_CXX_FLAGS ${ARA_GEN_ARA_CXX_FLAGS} -Wno-switch-unreachable -Wno-float-equal)
        set(helper_ARGS_INCLUDE ${helper_ARGS_INCLUDE} ${helper_ARGS_DESTINATION}/net-bindings/fastdds_idl)

    else() #TODO 无生成代码时不用调用
        add_aragen(
            DESTINATION ${helper_ARGS_DESTINATION}
            TARGET ${helper_ARGS_TARGET}_CodeGenerated
            EXECUTABLE ${helper_ARGS_EXECUTABLE}
            ARXMLS ${helper_ARGS_ARXMLS}
        )

        list(APPEND _GeneratedSrc ${${helper_ARGS_TARGET}_CodeGenerated_SOURCES})
    endif(HAS_FASTDDS_BINDING)
    list(APPEND _GeneratedTargent ${helper_ARGS_TARGET}_CodeGenerated)
   
    # 构建 helper 库
    add_library(${helper_ARGS_TARGET}  OBJECT  ${_GeneratedSrc} )
    target_include_directories(${helper_ARGS_TARGET}
        PUBLIC
            ${helper_ARGS_DESTINATION}/includes
            ${helper_ARGS_DESTINATION}/net-bindings
            ${helper_ARGS_INCLUDE}
    )

    target_compile_options(${helper_ARGS_TARGET}
        PUBLIC
            ${ARA_GEN_ARA_CXX_FLAGS})

    target_link_libraries(${helper_ARGS_TARGET}
        PUBLIC
            ${ARA_GEN_ARA_LIBS}
            ${ARA_GEN_ARA_LIBS_LOOP_REFERENCE}
    )

    add_dependencies(${helper_ARGS_TARGET} ${_GeneratedTargent}) 

endfunction(_aragen_helper_add_target)

macro(_aragen_helper_print_info)
    message(STATUS "---------------------------------------------------")
    message(STATUS "CMAKE_INSTALL_PREFIX:${CMAKE_INSTALL_PREFIX}")
    message(STATUS "ARA_GEN_SDK_NAME:${ARA_GEN_SDK_NAME}")
    message(STATUS "ARA_GEN_EXECUTABLE_FQN:${ARA_GEN_EXECUTABLE_FQN}")
    message(STATUS "ARA_GEN_ARA_LIBS:${ARA_GEN_ARA_LIBS}")
    message(STATUS "ARA_GEN_ARA_LIBS_LOOP_REFERENCE:${ARA_GEN_ARA_LIBS_LOOP_REFERENCE}")
    message(STATUS "ARA_GEN_ARA_CXX_FLAGS:${ARA_GEN_ARA_CXX_FLAGS}")    
    message(STATUS "ARA_GEN_EXECUTABLE_OUTPUT:${ARA_GEN_EXECUTABLE_OUTPUT}")        
    foreach( ARA_GEN_ARXML ${ARA_GEN_ARXMLS})
        message(STATUS "ARA_GEN_ARXMLS:${ARA_GEN_ARXML}")
    endforeach()
    message(STATUS "---------------------------------------------------")
endmacro(_aragen_helper_print_info)

macro(_aragen_helper_init)
    set(ARA_GEN_SDK_NAME generic_x84_x64)
    if(EXISTS "${CMAKE_INSTALL_PREFIX}/../../../.release.json")
        file(READ "${CMAKE_INSTALL_PREFIX}/../../../.release.json" _CONFIG_JSON)
        string(REGEX MATCH "\"SDK_NAME\"[ ]*:[ ]*\"([^\"]+)\"" _MATCH_SDK_NAME "${_CONFIG_JSON}")
        if(_MATCH_SDK_NAME)
            string(REGEX REPLACE " " "" ARA_GEN_SDK_NAME ${CMAKE_MATCH_1})
        endif()
        set(skip_old_release ON)
    endif()
    if(NOT skip_old_release AND EXISTS "${CMAKE_INSTALL_PREFIX}/../../../release")
        file(STRINGS ${CMAKE_INSTALL_PREFIX}/../../../release _ConfigContents)
        foreach(_NameAndValue ${_ConfigContents})
            # Strip leading spaces
            string(REGEX REPLACE "^[ ]+" "" _NameAndValue ${_NameAndValue})
            string(REGEX REPLACE "\"" "" _NameAndValue ${_NameAndValue})
            # Find variable name & value
            string(REGEX MATCH "^[^=]+" _Name ${_NameAndValue})
            string(REPLACE "${_Name}=" "" _Value ${_NameAndValue})
            string(REGEX REPLACE " " "" _Name ${_Name})
            if(${_Name} STREQUAL SDK_NAME)
                string(REGEX REPLACE " " "" ARA_GEN_SDK_NAME ${_Value})
                break()
            endif()
        endforeach()
    endif()
    if(NOT DEFINED ARA_GEN_EXECUTABLE_OUTPUT)
        set(ARA_GEN_EXECUTABLE_OUTPUT "$ENV{HOME}/.isoft/tmp/ara_binout")
    endif()
endmacro(_aragen_helper_init)
_aragen_helper_init()


# HOOK用户工程,做适配性处理
function(aragen_helper_register_target)
    set(oneValueArgs TARGET TYPE)
    cmake_parse_arguments(helper_ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # copy package datas to ARA_GEN_EXECUTABLE_OUTPUT
    if(helper_ARGS_TYPE STREQUAL EXE)
        set(_PACKAGE_DATA_PATH ${ARA_GEN_EXECUTABLE_OUTPUT}/${ARA_GEN_EXECUTABLE_FQN}/${ARA_GEN_SDK_NAME}/)
        # config ${_PACKAGE_DATA_PATH}/info.json.in
        # copy arxmls from ARA_GEN_ARXMLS to {_PACKAGE_DATA_PATH}
        INSTALL(TARGETS ${helper_ARGS_TARGET}
            RUNTIME DESTINATION ${_PACKAGE_DATA_PATH}
            #LIBRARY DESTINATION ${ARA_GEN_EXECUTABLE_OUTPUT}
            #ARCHIVE DESTINATION ${ARA_GEN_EXECUTABLE_OUTPUT}
        )
    endif()
    
endfunction(aragen_helper_register_target)

macro(aragen_helper_add_build_helper_library)

    _aragen_helper_print_info()

    set(ARA_GEN_BUILD_HELPER_LIBRARY ${ARA_GEN_EXECUTABLE}-build-helper-library)

    _aragen_helper_add_target(
        DESTINATION ${PROJECT_BINARY_DIR}/gen/${ARA_GEN_EXECUTABLE}
        TARGET ${ARA_GEN_BUILD_HELPER_LIBRARY}
        EXECUTABLE ${ARA_GEN_EXECUTABLE}
        ARXMLS ${ARA_GEN_ARXMLS}
    )
endmacro()


function(aragen_helper_add_custom_command_copy_generated_configs)
    message(WARNING "!!!aragen_helper_add_custom_command_copy_generated_configs is removed!!! you can delete this function call or recreate project from IDE")
endfunction(aragen_helper_add_custom_command_copy_generated_configs)