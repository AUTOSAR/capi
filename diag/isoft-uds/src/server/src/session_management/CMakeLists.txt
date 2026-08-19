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
# @date       2022-08-31
# @author     gaohuiming
# @version    1.2.0
#
# ================================================================

find_package(ara-core-types REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(isoft-manifestreader REQUIRED)

set(TARGET_NAME ara_diag_dcm)

add_library(${TARGET_NAME} STATIC
    access_permission.cpp
    conversation_security_level.cpp
    conversation_session.cpp
    conversation.cpp
    conversation_manager.cpp
    conversation_manager_skeleton.cpp
    data_element.cpp
    data_identifier.cpp
    data_identifier_manager.cpp
    data_provider.cpp
    environmental_condition.cpp
    environmental_condition_manager.cpp
    request_context.cpp
    routine.cpp
    server.cpp
    server_manager.cpp
    service_base.cpp
    service_custom.cpp
    service_x10.cpp
    service_x11.cpp
    service_x14.cpp
    service_x19.cpp
    service_x22.cpp
    service_x27.cpp
    service_x28.cpp
    service_x2a.cpp
    service_x2c.cpp
    service_x2e.cpp
    service_x31.cpp
    service_x34.cpp
    service_x35.cpp
    service_x36.cpp
    service_x37.cpp
    service_x38.cpp
    service_x3e.cpp
    service_x85.cpp
    service_x86.cpp
)

target_link_libraries(${TARGET_NAME}
    PUBLIC
        isoft::naicpp
        isoft::manifestreader
        ara::core-types
        ara_diag_api_wrapper
)
