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

cmake_minimum_required(VERSION 3.2)

project(ara-gen
        LANGUAGES NONE
        VERSION 0.0.1
)

include("GNUInstallDirs")

set(CONFIG_DEFAULT_ARAGEN_SCRIPT "${CMAKE_INSTALL_BINDIR}}/aragen")

# Create *-config.cmake file in a relocatable-friendly way.
include(CMakePackageConfigHelpers)

set(INSTALL_DEST "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION ${INSTALL_DEST}
    PATH_VARS CONFIG_DEFAULT_ARAGEN_SCRIPT
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
	DESTINATION ${INSTALL_DEST}
)

install(FILES "./cmake/${PROJECT_NAME}.cmake"
	DESTINATION ${INSTALL_DEST}
)
install(FILES "./cmake/ara-gen-helper-base.cmake"
	DESTINATION ${INSTALL_DEST}
)


# install (aragen)pyinstall
install(CODE "execute_process(COMMAND pyinstaller  ${PROJECT_SOURCE_DIR}/aragen.spec --distpath ${CMAKE_CURRENT_BINARY_DIR})")
install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/aragen"
        DESTINATION "${CMAKE_INSTALL_BINDIR}")
