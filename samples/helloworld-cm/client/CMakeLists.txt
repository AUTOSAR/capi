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


# 本文件由ara-gen(1.1.0) 生成
cmake_minimum_required(VERSION 3.4)
project(None) # 仅占位，会在下面的aragen-helper.cmake中被更新为ARA_GEN_EXECUTABLE及其版本
# include aragen-helper.cmake --执行如下事项：
#   a> 将arxml定义的Executable信息设置到ARA_GEN_XXX为前缀的cmake的变量中
#   b> 根据arxml的定义Executable使用ara的情况 自动引入所依赖的库（如 ara::per ara::com ara::phm ara::diag....)
#   c> 将b步骤的多个被依赖库合并成单一的、名为ARA_GEN_BUILD_HELPER_LIBRARY的 library target 供应用直接引用(参见src/CMakeLists.txt)
#   d> library target ARA_GEN_BUILD_HELPER_LIBRARY 会自动根据arxml的定义的信息生成符合AP规范的 ara API代码，应用可直接在当前工程使用（include & library path 已自动添加)
include(files/aragen-helper.cmake)

# 设置当前工程所用到的arxml列表:ARA_GEN_ARXMLS
# 可由cmake 参数传入(e.g: cmake -DARA_GEN_ARXMLS=/var/xx1/;/var/xx2/)
# 也可在此处增删改
set(ARXML_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../model) # arxml model path prefix
set(ARA_GEN_ARXMLS ${ARA_GEN_ARXMLS}
	# ARA_ARXMLS_DIR 是isot-ap平台arxml所在路径
	# ${ARA_ARXMLS_DIR}/common/autosar/AUTOSAR_MOD_StandardTypes.arxml
	# ${ARA_ARXMLS_DIR}/common/isoft/DeclarationGroups.arxml
	# ${ARA_ARXMLS_DIR}/common/isoft/SharedTypes.arxml
	${ARA_ARXMLS_DIR}/common

	# design & common deployment
	${ARXML_PREFIX}/library_common
	${ARXML_PREFIX}/library_common/../library_common
	${ARXML_PREFIX}/library_common
	# ${ARXML_PREFIX}/app/client_design.arxml
	${ARXML_PREFIX}/app

	# deployment & integration
	# ${ARXML_PREFIX}/library_for_machine1_integration/client_deployment.arxml
	# ${ARXML_PREFIX}/library_for_machine1_integration/machine_extension.arxml
	${ARXML_PREFIX}/library_for_machine1_integration
	${ARA_ARXMLS_DIR}/development_machines/system
	${ARA_ARXMLS_DIR}/development_machines/machine1/common
) # arxmls

# 添加 ARA_GEN_BUILD_HELPER_LIBRARY target
aragen_helper_add_build_helper_library()

# 添加 用户 target(exe) 和 tests
add_subdirectory(src)

if(ARA_ENABLE_TESTS)
	enable_testing()
	find_package(GTest REQUIRED CONFIG)
	add_subdirectory(test)
endif(ARA_ENABLE_TESTS)