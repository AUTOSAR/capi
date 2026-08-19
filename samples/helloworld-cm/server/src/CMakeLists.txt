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


# 本文件由ara-gen(1.1.0)根据arxml生成

set(EXE_src
    main.cpp
)
set(EXE_target ${ARA_GEN_EXECUTABLE})
add_executable(${EXE_target} ${EXE_src})

set_target_properties(${EXE_target}
    PROPERTIES
        OUTPUT_NAME ${ARA_GEN_EXECUTABLE}
    )

target_link_libraries(${EXE_target}
    ${ARA_GEN_BUILD_HELPER_LIBRARY}
)
add_dependencies(${EXE_target} ${ARA_GEN_BUILD_HELPER_LIBRARY})

# isoft-ap 对 target 的统一处理
aragen_helper_register_target(TARGET ${EXE_target} TYPE EXE)