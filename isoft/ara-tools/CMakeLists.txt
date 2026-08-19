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

cmake_minimum_required(VERSION 3.8)
project(isoft-ara-tools VERSION 1.1.0 LANGUAGES NONE) # project version,not sdk version
include(GNUInstallDirs)

set(PYINSTALL_EXECUTABLE pyinstaller)

# python main 打包
function(add_python_command COMMAND_NAME)
    set(CMD_NAME ${COMMAND_NAME})
    set(SRC_PATH ${PROJECT_SOURCE_DIR})
    configure_file(${PROJECT_SOURCE_DIR}/files/pyinstaller_cmd.spec.in ${CMAKE_CURRENT_BINARY_DIR}/${COMMAND_NAME}.spec @ONLY)

    add_custom_target(${COMMAND_NAME} ALL
        COMMAND ${PYINSTALL_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${COMMAND_NAME}.spec  --distpath ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM
    )
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${COMMAND_NAME}
        DESTINATION "${ARA_SYSROOT}/ara-tools"
    )
endfunction()

add_python_command(configMachine)
