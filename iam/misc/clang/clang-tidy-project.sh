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
# @file       clang-tidy-project.sh
# @brief
# @details
# @date       2025-04-15
# @author     hanjingjing
# @version    1.2.0
#
# ================================================================
#
# !/bin/bash
# 批量执行clang-tidy：目标是给定目录下的所有C++源码文件
# 检查是否提供了目标目录作为参数
#
# ================================================================

if [ -z "$1" ]; then
  echo "Usage: $0 请输入目标目录"
  exit 1
fi

TARGET_DIR="$1"

CLANG_TIDY_EXECUTABLE=clang-tidy-12
# 检查是否安装了clang-format
if command -v ${CLANG_TIDY_EXECUTABLE} &> /dev/null; then
    echo "========${CLANG_TIDY_EXECUTABLE} is installed.========[PuHua AutoSar-AP]"
else
    echo "${CLANG_TIDY_EXECUTABLE} is not installed. Please install, Command: sudo apt install clang-format"
    exit 1
fi

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
echo "Script directory: $SCRIPT_DIR"

pathCmd=$(pwd)
printf "工作目录[%s]\n" ${pathCmd}

# 设置编译数据库路径（如果有）
BUILD_DIR="${SCRIPT_DIR}/../../build"
export COMPILATION_DATABASE_PATH="${BUILD_DIR}/compile_commands.json"
echo "Clang-Tidy BuildDir = ${BUILD_DIR}"
# 初始化计数器
nFileIndex=0
nSkipCount=0
nFileTotal=0
# 使用 find 命令递归遍历目标目录下的所有C和C++源码文件，并统计数量 
# 使用 while 循环和 process substitution 来避免子shell问题
while read -r file; do
    ((nFileTotal++))
done < <(find "$TARGET_DIR" -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.cxx" -o -name "*.h" -o -name "*.hh" -o -name "*.hpp" -o -name "*.hxx" \) )
while read -r file; do
    ((nFileIndex++))
    # 跳过目录(所有项目适用)：files、build、CMake、doc、qac、examples; 跳过文件名中包含"test"字符的文件
    if [[ "$file" == *"/files/"* || "$file" == *"/build/"* || "$FILE_PATH" == "CMake"* || "$file" == *"/doc/"* || "$file" == *"/qac/"* || "$file" == *"test"* || "$file" == *"/examples/"* ]]; then
        echo "====>>>>[${nFileIndex}/${nFileTotal}] Skiping clang-tidy File : $file"
        # 计数器加1
        ((nSkipCount++))
    else
        # 功能模块定制的跳过目录和文件
        if [[ "$file" == *"/libhse/"* || "$file" == *"/libpkcs/"* || "$file" == *"/hsmbst/"* || "$file" == *"/pkcs/"* ]]; then  #|| "$file" == *"/hsm/"*
            echo "====>>>>[${nFileIndex}/${nFileTotal}] Skiping clang-tidy File : $file"
            # 计数器加1
            ((nSkipCount++))
        else
            file_name=$(basename "$file")
            # 打印文件的完整路径，格式化每一个找到的文件
            echo "====<<<<[${nFileIndex}/${nFileTotal}] Running clang-tidy Begin: $file"
            file_name_without_ext="${file_name%.*}"
            # ${CLANG_TIDY_EXECUTABLE} "$file" --export-fixes=- --p="${BUILD_DIR}/compile_commands.json" --config-file="${BUILD_DIR}/../.clang-tidy"
            ${CLANG_TIDY_EXECUTABLE} "$file" --export-fixes=- --p="${BUILD_DIR}/compile_commands.json" | grep ${file_name_without_ext}
            echo "====<<<<[${nFileIndex}/${nFileTotal}] Running clang-tidy End  : $file"
        fi
    fi
done < <(find "$TARGET_DIR" -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.cxx" -o -name "*.h" -o -name "*.hh" -o -name "*.hpp" -o -name "*.hxx" \) )

echo "Total number of C and C++ source files processed: Running[${nFileIndex}], Skiping[${nSkipCount}]"
exit 0
