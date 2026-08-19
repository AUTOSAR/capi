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
# @file       clang-format-project.sh
# @brief
# @details
# @date       2024-06-01
# @author     yanglong.wang
# @version    1.2.0
#
# ================================================================
#
# Initialize counter
#
# ================================================================

file_count=0

# Check if target directory is provided as a parameter
if [ -z "$1" ]; then
  echo "Usage: $0 /path/to/directory"
  exit 1
fi

TARGET_DIR="$1"

CLANG_FORMAT_EXECUTABLE=clang-format
CLANG_FORMAT_STYLE=file
# Check if clang-format is installed
if command -v ${CLANG_FORMAT_EXECUTABLE} &> /dev/null; then
    echo "========${CLANG_FORMAT_EXECUTABLE} is installed.========[PuHua AutoSar-AP]"
else
    echo "${CLANG_FORMAT_EXECUTABLE} is not installed. Please install, Command: sudo apt install clang-format"
    exit 1
fi
# Use find command to recursively traverse all C and C++ source files in the target directory and count the number # cannot count
# find "$TARGET_DIR" -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.cxx" -o -name "*.h" -o -name "*.hh" -o -name "*.hpp" -o -name "*.hxx" \) | while read -r file; do
#     # Print the full path of the file
#     echo "Processing file: $file"
#     # Increment counter by 1
#     ((file_count++))
# done
# Use while loop and process substitution to avoid subshell issues
while read -r file; do
    # Print the full path of the file
    echo "Formatting File: $file"
    # Format each found file
    ${CLANG_FORMAT_EXECUTABLE} -style=${CLANG_FORMAT_STYLE} -i $file
    # Increment counter by 1
    ((file_count++))
done < <(find "$TARGET_DIR" -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.cxx" -o -name "*.h" -o -name "*.hh" -o -name "*.hpp" -o -name "*.hxx" \))

echo "Total number of C and C++ source files processed: ${file_count}"
