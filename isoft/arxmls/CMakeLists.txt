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
# @date       2021-07-21
# @author     jiawei
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.4)

project(ara-arxmls VERSION 1.1.0 LANGUAGES NONE) # project version,not sdk version

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(ENABLE_COMPONENT_CHECK "enable component check " ON)
find_package(ara-gen)
if(ara-gen_FOUND)
  add_custom_target(check_platform_components ALL
      COMMAND ${ARAGEN_PATH} models/common models/platform_components --list-swc
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      SOURCES models
      COMMENT "check platform_components design depend without development_machines"
      VERBATIM
  )

  add_custom_target(check_machine1 ALL
      COMMAND ${ARAGEN_PATH} models/common models/platform_components models/development_machines/diagnostic_extract models/development_machines/system models/development_machines/machine1  --list-swc
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      SOURCES models
      COMMENT "check machine1"
      VERBATIM
  )  

  add_custom_target(check_machine2 ALL
      COMMAND ${ARAGEN_PATH} models/common models/platform_components models/development_machines/diagnostic_extract models/development_machines/system models/development_machines/machine2  --list-swc
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      SOURCES models
      COMMENT "check machine2"
      VERBATIM
  )  

  add_custom_target(check_all ALL
    COMMAND ${ARAGEN_PATH} models --list-machines --check-uuid
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    SOURCES models 
    COMMENT "check sky ref and uuid"
    VERBATIM
  )


  
#############################################
# check each component(不缺失依赖项)
#find_program(ARA_TOOLS_COMPONENT_UTIL_SCRIPT NAMES componentUtil PATHS ${ARA_SYSROOT}/ara-tools)
if(ENABLE_COMPONENT_CHECK)
  #set(DEBUG_ARA_GEN ON)
  set(ARA_TOOLS_COMPONENT_UTIL_SCRIPT ${ARA_SYSROOT}/ara-tools/componentUtil)
  if(NOT EXISTS ${ARA_TOOLS_COMPONENT_UTIL_SCRIPT})
    set(ARA_TOOLS_COMPONENT_UTIL_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/../ara-tools/componentUtil)
  endif()

  execute_process(
    COMMAND ${ARA_TOOLS_COMPONENT_UTIL_SCRIPT} models/components.json
    ERROR_VARIABLE out_errors
    OUTPUT_VARIABLE out_outputs
    RESULT_VARIABLE out_return
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  # string(REPLACE "\n" ";" out_outputs ${out_outputs})

  foreach(it  ${out_outputs})
    execute_process(
      COMMAND ${ARA_TOOLS_COMPONENT_UTIL_SCRIPT} models/components.json ${it}
      ERROR_VARIABLE out_errors
      OUTPUT_VARIABLE out_outputs2
      RESULT_VARIABLE out_return
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    if("${out_outputs2}" STREQUAL "")
      continue()
    endif()
    string(REPLACE "\n" ";" out_outputs2 ${out_outputs2})
    add_custom_target(check_component_${it} ALL
      COMMAND ${ARAGEN_PATH}  ${out_outputs2} --list-swc
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/models/"
      COMMENT "check_component_${it}"
      VERBATIM
    )
  endforeach()
endif()
#############################################

endif()


set(ARA_ARXMLS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}")

# Create *-config.cmake file
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  PATH_VARS ARA_ARXMLS_DIR
)

# Install *-config.cmake file
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)


# Install the arxml files
install(DIRECTORY models/
    DESTINATION "${ARA_ARXMLS_DIR}"
    COMPONENT dev
)
# remove ara process 
set(_MACHINE1_SWCL_ARXML ${CMAKE_INSTALL_PREFIX}/${ARA_ARXMLS_DIR}/development_machines/machine1/distribution/swcls/core/core_swcl.arxml)
set(_MACHINE2_SWCL_ARXML ${CMAKE_INSTALL_PREFIX}/${ARA_ARXMLS_DIR}/development_machines/machine2/distribution/swcls/core/core_swcl2.arxml)
set(_MACHINE1_SWCL_FRAMEWORK_ARXML ${CMAKE_INSTALL_PREFIX}/${ARA_ARXMLS_DIR}/development_machines/machine1/distribution/swcls/framework/framework_swcl.arxml)
set(_MACHINE2_SWCL_FRAMEWORK_ARXML ${CMAKE_INSTALL_PREFIX}/${ARA_ARXMLS_DIR}/development_machines/machine2/distribution/swcls/framework/framework_swcl2.arxml)
set(_MACHINE1_PROCESSES_HSMD_PROCESS_ARXML ${CMAKE_INSTALL_PREFIX}/${ARA_ARXMLS_DIR}/development_machines/machine1/processes/cryptohsmd/integration/process.arxml)

# 多一个DEAMON判断是为了兼容AP2.1，该版本没有对模块做解构，只能单独控制daemon是否部署，无法完全移除模块
message(STATUS " ARXMLS ARA_WITH_CRYPTO = ${ARA_WITH_CRYPTO} ")
message(STATUS " ARXMLS ARA_WITH_CRYPTO_KEY_DAEMON = ${ARA_WITH_CRYPTO_KEY_DAEMON} ")
message(STATUS " ARXMLS ARA_WITH_CRYPTO_X509_DAEMON = ${ARA_WITH_CRYPTO_X509_DAEMON} ")
message(STATUS " ARXMLS ARA_WITH_CRYPTO_HSM_NXP_DAEMON = ${ARA_WITH_CRYPTO_HSM_NXP_DAEMON} ")
if((NOT ARA_WITH_CRYPTO) OR (NOT ARA_WITH_CRYPTO_KEY_DAEMON)OR (NOT ARA_WITH_CRYPTO_X509_DAEMON))
  # remove process from core
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/cryptod_proc<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/cryptox509d_proc<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  # remove executable from framework
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/cryptox509d<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/cryptod<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/cryptod<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE2_SWCL_FRAMEWORK_ARXML})")
  message(STATUS " ARXMLS execute_process = cryptod_proc ")
endif()
# 2024-12-10 韩玉信修改CRYPTO：可以通过一个宏单独控制HSM_NXP版本的密钥管理器的编译
if(NOT ARA_WITH_CRYPTO_HSM_NXP_DAEMON)
  # remove process from core
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/cryptohsmd_proc<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  # remove executable from framework
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/cryptohsmd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  # delete <FUNCTION-GROUP-STATE-IREFS> in cryptohsmd/integration/process.arxml 
  install(CODE "execute_process(COMMAND sed '/<FUNCTION-GROUP-STATE-IREFS>/,/<\/FUNCTION-GROUP-STATE-IREFS>/ { /<FUNCTION-GROUP-STATE-IREF>/,/<\/FUNCTION-GROUP-STATE-IREF>/d }' ${_MACHINE1_PROCESSES_HSMD_PROCESS_ARXML})")
endif()


message(STATUS " ARXMLS ARA_WITH_DIAG = ${ARA_WITH_DIAG} ")
message(STATUS " ARXMLS ARA_WITH_DIAG_DAEMON = ${ARA_WITH_DIAG_DAEMON} ")
if((NOT ARA_WITH_DIAG) OR (NOT ARA_WITH_DIAG_DAEMON))
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/dmd<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  #install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/dmd2<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE2_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/dmd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/dmd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE2_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/doipd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/doipd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE2_SWCL_FRAMEWORK_ARXML})")
  message(STATUS " ARXMLS execute_process = dmd ")
endif()

if((NOT ARA_WITH_NM) OR (NOT ARA_WITH_NM_DAEMON))
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/nm_daemon_proc1<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/nm_daemon_proc2<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE2_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/nmd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/nmd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE2_SWCL_FRAMEWORK_ARXML})")
endif()

if((NOT ARA_WITH_PHM) OR (NOT ARA_WITH_PHM_DAEMON))
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/phmd_process1<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/phmd_process2<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE2_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/phmd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/phmd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE2_SWCL_FRAMEWORK_ARXML})")
endif()

if((NOT ARA_WITH_TSYNC) OR (NOT ARA_WITH_TSYNC_DAEMON))
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/tsync_daemon_proc1<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/tsync_daemon_proc2<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE2_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/tsyncd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/tsyncd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE2_SWCL_FRAMEWORK_ARXML})")
endif()

if(NOT ARA_WITH_UCM)
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/package_manager<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/package_manager<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE2_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/ucmd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/ucmd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE2_SWCL_FRAMEWORK_ARXML})")
endif()

if(NOT ARA_WITH_VUCM)
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/ucmm<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/ucmm2<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE2_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/ucmmd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/ucmmd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE2_SWCL_FRAMEWORK_ARXML})")
endif()

if((NOT ARA_WITH_IDSM) OR (NOT ARA_WITH_IDSM_DAEMON))
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/idsm_process1<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/idsmd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
endif()

if((NOT ARA_WITH_LOG) OR (NOT ARA_WITH_LOG_DAEMON))
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/dlt_daemon_proc<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/daemon_proc2<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE2_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/logd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/logd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE2_SWCL_FRAMEWORK_ARXML})")
endif()

if((NOT ARA_WITH_COM) OR (NOT ARA_WITH_COM_SOMEIP_DAEMON))
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/nsomeip_routing_proc<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/nsomeip_routing_proc2<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE2_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/nsomeipd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/nsomeipd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE2_SWCL_FRAMEWORK_ARXML})")
endif()
if((NOT ARA_WITH_COM) OR (NOT ARA_WITH_COM_CG_DAEMON))
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/cgd_proc<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/cgd_proc2<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE2_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/cgd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/cgd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE2_SWCL_FRAMEWORK_ARXML})")
endif()
if((NOT ARA_WITH_FW) OR (NOT ARA_WITH_FW_DAEMON))
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/fwdaemon_proc<\\\\/CONTAINED-PROCESS-REF\>/d\" ${_MACHINE1_SWCL_ARXML})")
  install(CODE "execute_process(COMMAND sed  -i \"/.*\\\\/fwd<\\\\/CONTAINED-AR-ELEMENT-REF\>/d\" ${_MACHINE1_SWCL_FRAMEWORK_ARXML})")
endif()