#!/bin/bash

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
# @file       clang-tidy-file.sh
# @brief      Script to run clang-tidy on a single C++ source file
# @details
# @date       2024-10-11
# @author     longxiao.liang
# @version    1.2.0
#
# ================================================================
#
# Batch execute clang-tidy: target is the given C++ source file
# Check if target source file is provided as a parameter
#
# ================================================================

if [ -z "$1" ]; then
  echo "Usage: $0 Please enter the target file"
  exit 1
fi

TARGET_FILE="$1"

CLANG_TIDY_EXECUTABLE=clang-tidy-12
# Check if clang-format is installed
if command -v ${CLANG_TIDY_EXECUTABLE} &> /dev/null; then
    echo "========${CLANG_TIDY_EXECUTABLE} is installed.========[PuHua AutoSar-AP]"
else
    echo "${CLANG_TIDY_EXECUTABLE} is not installed. Please install, Command: sudo apt install clang-format"
    exit 1
fi

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
echo "Script directory: $SCRIPT_DIR"

pathCmd=$(pwd)
printf "Working directory [%s]\n" ${pathCmd}

# Set compilation database path (if any)
BUILD_DIR="${SCRIPT_DIR}/../build"
export COMPILATION_DATABASE_PATH="${BUILD_DIR}/compile_commands.json"
echo "Clang-Tidy BuildDir = ${BUILD_DIR}"
# Use find command to recursively traverse all C and C++ source files in the target directory and count the number 
# Use while loop and process substitution to avoid subshell issues

FILE_NAME=$(basename ${TARGET_FILE})
echo "====<<<<Running clang-tidy File: ${TARGET_FILE}, Name=${FILE_NAME}"
# --config-file="${BUILD_DIR}/../.clang-tidy"
# >> ${pathCmd}/1.log 
${CLANG_TIDY_EXECUTABLE} "${TARGET_FILE}" --export-fixes=- --header-filter=.* --p="${BUILD_DIR}/compile_commands.json"  | grep ${FILE_NAME} 

exit 0
