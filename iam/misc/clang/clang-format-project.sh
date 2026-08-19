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
# @file       clang-format-project.sh
# @brief
# @details
# @date       2025-02-18
# @author     hanjingjing
# @version    1.2.0
#
# ================================================================
#
# !/bin/bash
# 初始化计数器
#
# ================================================================

file_count=0

# 检查是否提供了目标目录作为参数
if [ -z "$1" ]; then
  echo "Usage: $0 /path/to/directory"
  exit 1
fi

TARGET_DIR="$1"

# 2025-02-27 韩玉信
CLANG_FORMAT_EXECUTABLE=clang-format
# 请根据自己选用的[clang-format_style]类型，打开如下的格式化风格
CLANG_FORMAT_STYLE=file
# CLANG_FORMAT_STYLE="{ BasedOnStyle: Google, AccessModifierOffset: -4, AlignConsecutiveMacros: true, AlignConsecutiveAssignments: true, AlignEscapedNewlines: Right, AllowShortIfStatementsOnASingleLine: Never, AllowShortLoopsOnASingleLine: false, BinPackParameters: false, BreakBeforeBinaryOperators: All, BreakBeforeBraces: Custom, BraceWrapping: {AfterClass: true, AfterControlStatement: Never, AfterEnum: true, AfterFunction: true, AfterStruct: true, AfterUnion: true, AfterExternBlock: true}, BreakInheritanceList: BeforeComma, BreakConstructorInitializers: BeforeComma, ColumnLimit: 120, IndentPPDirectives: BeforeHash, IndentWidth: 4, ReflowComments: false, SpacesInAngles: true, TabWidth: 4}"

# 检查是否安装了clang-format
if command -v ${CLANG_FORMAT_EXECUTABLE} &> /dev/null; then
    echo "========${CLANG_FORMAT_EXECUTABLE} is installed.========[PuHua AutoSar-AP]"
else
    echo "${CLANG_FORMAT_EXECUTABLE} is not installed. Please install, Command: sudo apt install clang-format"
    exit 1
fi
# 使用 find 命令递归遍历目标目录下的所有C和C++源码文件，并统计数量 # 统计不到
# find "$TARGET_DIR" -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.cxx" -o -name "*.h" -o -name "*.hh" -o -name "*.hpp" -o -name "*.hxx" \) | while read -r file; do
#     # 打印文件的完整路径
#     echo "Processing file: $file"
#     # 计数器加1
#     ((file_count++))
# done
# 使用 while 循环和 process substitution 来避免子shell问题
while read -r file; do
    # 打印文件的完整路径
    echo "Formatting File: $file"
    # 格式化每一个找到的文件
    ${CLANG_FORMAT_EXECUTABLE} -style="${CLANG_FORMAT_STYLE}" -i $file
    # 计数器加1
    ((file_count++))
done < <(find "$TARGET_DIR" -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.cxx" -o -name "*.h" -o -name "*.hh" -o -name "*.hpp" -o -name "*.hxx" \))

echo "Total number of C and C++ source files processed: ${file_count}"
