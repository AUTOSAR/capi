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
# @file       clang-tidy-project.sh
# @brief
# @details
# @date       2024-06-01
# @author     yanglong.wang
# @version    1.2.0
#
# ================================================================
#
# Batch execute clang-tidy: target is all C++ source files in the given directory
# Check if target directory is provided as a parameter
#
# ================================================================

if [ -z "$1" ]; then
  echo "Usage: $0 Please enter the target directory"
  exit 1
fi

TARGET_DIR="$1"

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
BUILD_DIR="${SCRIPT_DIR}/../../build"
export COMPILATION_DATABASE_PATH="${BUILD_DIR}/compile_commands.json"
echo "Clang-Tidy BuildDir = ${BUILD_DIR}"
# Initialize counter
nFileIndex=0
nSkipCount=0
nFileTotal=0
# Use find command to recursively traverse all C and C++ source files in the target directory and count the number 
# Use while loop and process substitution to avoid subshell issues
while read -r file; do
    ((nFileTotal++))
done < <(find "$TARGET_DIR" -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.cxx" -o -name "*.h" -o -name "*.hh" -o -name "*.hpp" -o -name "*.hxx" \) )
while read -r file; do
    ((nFileIndex++))
    # Skip directories (applicable to all projects): files, build, CMake, doc, qac, examples; skip files containing "test" in the filename
     if [[ "$file" == *"/files/"* || "$file" == *"/build/"* || "$FILE_PATH" == "CMake"* || "$file" == *"/doc/"* || "$file" == *"/qac/"* || "$file" == *"test"* || "$file" == *"/examples/"* ]]; then
        echo "====>>>>[${nFileIndex}/${nFileTotal}] Skiping clang-tidy File : $file"
        # Increment counter by 1
        ((nSkipCount++))
    else
        # Skip directories and files customized by functional modules
        if [[ "$file" == *"/libhse/"* || "$file" == *"/libpkcs/"* || "$file" == *"/hsmbst/"* || "$file" == *"/pkcs/"* ]]; then  #|| "$file" == *"/hsm/"*
            echo "====>>>>[${nFileIndex}/${nFileTotal}] Skiping clang-tidy File : $file"
            # Increment counter by 1
            ((nSkipCount++))
        else
            file_name=$(basename "$file")
            # Print the full path of the file, format each found file
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
