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
# @file        Utils.cmake
# @brief      
# @details
# @date        2026-01-01
# @author      jian.feng
# @version     0.1
# @description Implement common utility functions.
# ================================================================
include_guard(GLOBAL)

# =============================================================================
# get_cpu_core_count
# -----------------------------------------------------------------------------
#
# Get the number of CPU cores on the current machine.
#
# Parameter description:
#   RESULT_VAR - (required) Output variable name to store the number of CPU cores.
#   [TYPE]     - (optional) Specify the type of core to retrieve; can be one of the following:
#                 LOGICAL  - Number of logical cores (default, i.e., CPU threads)
#                 PHYSICAL - Number of physical cores (i.e., CPU physical core count)
#
# Return value:
#   Sets the number of CPU cores into the variable RESULT_VAR in the parent scope.
#   If the number of cores cannot be determined, the variable will be set to 0.
#
# Example usage:
#   get_cpu_core_count(CORE_COUNT)           # Get logical core count
#   get_cpu_core_count(PHYSICAL_CORES TYPE PHYSICAL)  # Get physical core count
#
# Important notes:
#   This function requires CMake 3.10 or higher for best compatibility.
#   In container environments, returns the number of CPU cores visible to the container.
#
# =============================================================================
function(utils_get_cpu_core_count RESULT_VAR)
    # Parse arguments
    set(options)
    set(oneValueArgs TYPE)
    set(multiValueArgs)

    cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Set default type to LOGICAL
    if(NOT DEFINED ARG_TYPE)
        set(ARG_TYPE "LOGICAL")
    endif()

    # Convert to uppercase for comparison
    string(TOUPPER "${ARG_TYPE}" CORE_TYPE)

    # Get core count based on requested type
    if(CORE_TYPE STREQUAL "PHYSICAL")
        cmake_host_system_information(RESULT core_count QUERY NUMBER_OF_PHYSICAL_CORES)
    else()
        if(NOT CORE_TYPE STREQUAL "LOGICAL")
            message(WARNING "Unknown core type '${ARG_TYPE}', using default 'LOGICAL'")
        endif()
        cmake_host_system_information(RESULT core_count QUERY NUMBER_OF_LOGICAL_CORES)
    endif()

    # Validate result
    if(NOT DEFINED core_count)
        message(WARNING "Unable to determine CPU core count, setting to 0")
        set(core_count 0)
    endif()

    # Output debug information
    message(STATUS "Detected CPU ${CORE_TYPE} core count: ${core_count}")

    # Set return value
    set(${RESULT_VAR} ${core_count} PARENT_SCOPE)

endfunction(utils_get_cpu_core_count)



# Note:
#       The old configuration distinguishes prefixes, where ARA_WITH_xx indicates enabling a module, and ARA_ENABLE_xx indicates enabling specific features of a module.
#       The new configuration no longer distinguishes between modules and features in the prefix; all are ARA_ENABLE_xx.
#       To maintain compatibility with the old configuration, we copy all ARA_ENABLE_xx and replace them with ARA_WITH_xxx, ensuring that the old configuration does not miss anything. Redundancy does not affect functionality.
function(_utils_compat_develop2 var_list output_var)
    # First, save all variables
    set(result_list ${var_list})
    foreach(var IN LISTS var_list)
        # Replace all ARA_ENABLE_xx with ARA_WITH_xx and add to the list
        string(REPLACE "ARA_ENABLE_" "ARA_WITH_" transformed_var "${var}")
        #message("${var} -> ${transformed_var}")
        list(APPEND result_list "${transformed_var}")
    endforeach()
    # Return result
    set(${output_var} "${result_list}" PARENT_SCOPE)
    #message(STATUS "Conversion result: ${result_list}")
endfunction(_utils_compat_develop2)



# =============================================================================
# utils_get_cmake_args_from_cache_vars
# -----------------------------------------------------------------------------
# Read all cached variable definitions in a CMake project and convert them into a -D command-line argument string.
#
# Parameter description:
#   OUTPUT_VAR    - (required) Variable name to store the generated argument string
#   [FILTER_PREFIX] - (optional) Only process option names that start with the specified prefix
#
# Return value:
#   Sets the generated argument string into the OUTPUT_VAR variable, format: "-DOPT1=ON -DVAR1=helloworld"
#
# Example usage:
#   utils_get_cmake_args_from_cache_vars(CMAKE_ARG_STR)
#   utils_get_cmake_args_from_cache_vars(CMAKE_ARG_STR FILTER_PREFIX ARA_)
#   message(STATUS "Generated option arguments: ${CMAKE_ARG_STR}")
# =============================================================================
function(utils_get_cmake_args_from_cache_vars OUTPUT_VAR)
    # Parse optional arguments
    set(option_args)
    set(oneValueArgs FILTER_PREFIX FILTER_TYPE EXCLUDE_PREFIX)
    set(multiValueArgs)
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Get all cached variables
    get_cmake_property(CACHE_VARS CACHE_VARIABLES)

    set(MATCHED_ARGS_LIST)

    # Iterate over all cached variables, searching for variables with specified prefix and type
    foreach(VAR_NAME IN LISTS CACHE_VARS)
        # Exclude filter
        if(DEFINED ARG_EXCLUDE_PREFIX)
            if("${VAR_NAME}" MATCHES "^${ARG_EXCLUDE_PREFIX}")
                continue()
            endif()
        endif()

        # Prefix filter
        if(DEFINED ARG_FILTER_PREFIX)
            if(NOT "${VAR_NAME}" MATCHES "^${ARG_FILTER_PREFIX}")
                continue()
            endif()
        endif()

        # Type filter
        get_property(VAR_TYPE CACHE ${VAR_NAME} PROPERTY TYPE)
        if(DEFINED ARG_FILTER_TYPE)
            if(NOT "${VAR_TYPE}" STREQUAL "${ARG_FILTER_TYPE}")
                continue()
            endif()
        endif()

        # Exclude filter
        if(DEFINED ARG_EXCLUDE_PREFIX)
            if("${VAR_NAME}" MATCHES "^${ARG_EXCLUDE_PREFIX}")
                continue()
            endif()
        endif()

        # Get variable value
        get_property(VAR_VALUE CACHE ${VAR_NAME} PROPERTY VALUE)

        # Generate -D parameter format
        list(APPEND MATCHED_ARGS_LIST "-D${VAR_NAME}=${VAR_VALUE}")
    endforeach()

    # To maintain compatibility with existing develop2.2, need to replace some ARA_ENABLE_XX with ARA_WITH_XX
    _utils_compat_develop2("${MATCHED_ARGS_LIST}" COMPAT_ARGS_LIST)

    # If certain variables need to be deleted globally by default, the following operation can be used
    # list(FILTER MODULE_CMAKE_ARGS EXCLUDE REGEX ".*-DCMAKE_HOME_DIRECTORY=.*")

    # Set output variable
    set(${OUTPUT_VAR} "${COMPAT_ARGS_LIST}" PARENT_SCOPE)

endfunction(utils_get_cmake_args_from_cache_vars)


# =============================================================================
# Extract -D defined variables from CMake command line arguments
# -----------------------------------------------------------------------------
# This function parses CMake command line arguments passed to CMake, extracts all variable definitions starting with -D,
# and stores the result in the specified output variable.
# Note: Only useful in script mode (cmake -P), not useful in configuration mode (cmake).
#
# Parameters:
#   OUTPUT_VAR - Output variable name to store the extracted variable definition list
#
# Usage:
#   utils_get_cmake_args_from_command_line(EXTRACTED_ARGS)
#   message(STATUS "Extracted arguments: ${EXTRACTED_ARGS}")
#
# Example:
#   If the command line is: cmake -DA=1 -DB=2 -DC ..
#   The function will return: "A=1;B=2;DC="
# =============================================================================
function(utils_get_cmake_args_from_command_line OUTPUT_VAR)
    set(result)

    # Iterate over all command line arguments
    # CMAKE_ARGC contains the total number of arguments, counting from 0
    foreach(i RANGE 0 ${CMAKE_ARGC})
        if(DEFINED CMAKE_ARGV${i})
            set(arg "${CMAKE_ARGV${i}}")

            # Check if it is a definition argument starting with -D
            if(NOT arg MATCHES "^-D(.+)")
                continue()
            endif()
            set(definition "${CMAKE_MATCH_1}")

            # Separate variable name and value
            if(definition MATCHES "^([^=]+)=(.*)$")
                set(var_name "${CMAKE_MATCH_1}")
                set(var_value "${CMAKE_MATCH_2}")

                message(STATUS "Command line definition: ${var_name} = ${var_value}")
                list(APPEND result "${var_name}=${var_value}")
            else()
                # No equals sign, treat as definition without value
                message(STATUS "Command line definition (no value): ${definition}")
                list(APPEND result "${definition}=")
            endif()
        endif()
    endforeach()

    # Set the result into a variable in the parent scope
    if(result)
        set(${OUTPUT_VAR} ${result} PARENT_SCOPE)
    endif()
endfunction(utils_get_cmake_args_from_command_line)



# =============================================================================
# Function: utils_remove_files
# -----------------------------------------------------------------------------
# 
# Description: Advanced deletion function, supports files, directories, and wildcard patterns
# 
# Parameters:
#   [PATTERNS] - Multi-value parameter: List of file/directory patterns to delete
#   [BASE_DIR] - Single-value parameter: Base directory for searching (default current directory)
#   [RECURSIVE] - Option parameter: Whether to search subdirectories recursively (only effective for wildcards)
#   [DRY_RUN] - Option parameter: Simulate execution, only show without actually deleting
#   [VERBOSE] - Option parameter: Show detailed operation information
#
# Usage:
#   utils_remove_files(PATTERNS "*.tmp" "temp_*" "build")
#   utils_remove_files(PATTERNS "*.log" "cache/" BASE_DIR "${PROJECT_BINARY_DIR}" RECURSIVE VERBOSE)
#   utils_remove_files(PATTERNS "**/*.bak" BASE_DIR "src" RECURSIVE DRY_RUN)
#
# Notes:
#   - Supports absolute and relative paths
#   - Wildcard * matches any characters, ? matches a single character
#   - The **/ pattern can recursively match subdirectories (requires RECURSIVE option)
#   - Automatically identifies files and directories, using appropriate deletion methods
# =============================================================================
function(utils_remove_files)
    # Parse arguments
    set(option_args RECURSIVE DRY_RUN VERBOSE)
    set(oneValueArgs BASE_DIR)
    set(multiValueArgs PATTERNS)
    cmake_parse_arguments(ARGS "${option_args}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Parameter validation
    if(NOT ARGS_PATTERNS)
        message(WARNING "utils_remove_files: No patterns specified")
        return()
    endif()

    # Set base directory
    if(ARGS_BASE_DIR)
        set(BASE_DIR "${ARGS_BASE_DIR}")
    else()
        set(BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")
    endif()

    # Verify base directory exists
    if(NOT IS_DIRECTORY "${BASE_DIR}")
        message(WARNING "utils_remove_files: Base directory does not exist: ${BASE_DIR}")
        return()
    endif()

    if(ARGS_VERBOSE)
        message(STATUS "utils_remove_files: Base directory: ${BASE_DIR}")
        message(STATUS "utils_remove_files: Patterns: ${ARGS_PATTERNS}")
        if(ARGS_DRY_RUN)
            message(STATUS "utils_remove_files: DRY RUN MODE - No files will be deleted")
        endif()
    endif()

    # Process each pattern
    foreach(pattern ${ARGS_PATTERNS})
        _utils_process_remove_pattern("${pattern}" "${BASE_DIR}" ${ARGS_RECURSIVE} ${ARGS_DRY_RUN} ${ARGS_VERBOSE})
    endforeach()
endfunction()


# =============================================================================
# Internal Function: _utils_process_remove_pattern
# 
# Description: Process a single deletion pattern
# =============================================================================
function(_utils_process_remove_pattern pattern base_dir recursive dry_run verbose)
    # Determine if it is a wildcard pattern
    if(pattern MATCHES "[*?]")
        # Wildcard pattern - use file(GLOB) or file(GLOB_RECURSE)
        if(recursive)
            if(verbose)
                message(STATUS "utils_remove_files: Recursive glob pattern: ${pattern}")
            endif()
            file(GLOB_RECURSE matched_items "${base_dir}/${pattern}")
        else()
            if(verbose)
                message(STATUS "utils_remove_files: Simple glob pattern: ${pattern}")
            endif()
            file(GLOB matched_items "${base_dir}/${pattern}")
        endif()
    else()
        # Exact path pattern
        set(matched_items "${base_dir}/${pattern}")
        if(verbose)
            message(STATUS "utils_remove_files: Exact path: ${matched_items}")
        endif()
    endif()

    # Process matched items
    foreach(item ${matched_items})
        if(NOT EXISTS "${item}")
            if(verbose)
                message(STATUS "utils_remove_files: Not found, skipping: ${item}")
            endif()
            continue()
        endif()

        # Delete item
        _utils_remove_single_item("${item}" ${dry_run} ${verbose})
    endforeach()

    # If no matches and not a wildcard pattern, check the exact path
    if(NOT matched_items AND NOT pattern MATCHES "[*?]")
        set(exact_path "${base_dir}/${pattern}")
        if(EXISTS "${exact_path}")
            _utils_remove_single_item("${exact_path}" ${dry_run} ${verbose})
        elseif(verbose)
            message(STATUS "utils_remove_files: Path does not exist: ${exact_path}")
        endif()
    endif()
endfunction()


# =============================================================================
# Internal Function: _utils_remove_single_item
# 
# Description: Delete a single file or directory
# =============================================================================
function(_utils_remove_single_item item_path dry_run verbose)
    # Determine item type and perform deletion
    if(IS_DIRECTORY "${item_path}")
        if(dry_run)
            message(STATUS "utils_remove_files: [DRY RUN] Would remove directory: ${item_path}")
        else()
            if(verbose)
                message(STATUS "utils_remove_files: Removing directory: ${item_path}")
            endif()
            file(REMOVE_RECURSE "${item_path}")
            # Verify deletion
            if(EXISTS "${item_path}")
                message(WARNING "utils_remove_files: Failed to remove directory: ${item_path}")
            elseif(verbose)
                message(STATUS "utils_remove_files: ✓ Directory removed successfully")
            endif()
        endif()
    else()
        if(dry_run)
            message(STATUS "utils_remove_files: [DRY RUN] Would remove file: ${item_path}")
        else()
            if(verbose)
                message(STATUS "utils_remove_files: Removing file: ${item_path}")
            endif()
            file(REMOVE "${item_path}")
            # Verify deletion
            if(EXISTS "${item_path}")
                message(WARNING "utils_remove_files: Failed to remove file: ${item_path}")
            elseif(verbose)
                message(STATUS "utils_remove_files: ✓ File removed successfully")
            endif()
        endif()
    endif()
endfunction()


#=======================================================================
# Function name: utils_remove_dirs
# Function description: Recursively/non-recursively search the specified directory for folders whose names match a wildcard pattern, and delete them.
# Matching rule: Supports * ? wildcards (consistent with find -name behavior)
# Core features: Strictly match directory names | Arbitrary depth recursion | Preview without deletion | Detailed log output
#=======================================================================
#
# ============================== Parameter description ==============================
#
# [Required parameters]
#
# BASE_DIR
#   Function: Specify the root directory for searching, starting from this directory to find folders
#   Value: Absolute path or CMake path variable (${CMAKE_SOURCE_DIR} / ${PROJECT_SOURCE_DIR})
#   Example: BASE_DIR ${CMAKE_SOURCE_DIR}
#
# PATTERN
#   Function: Folder name matching pattern, supports wildcards
#   Wildcard rules:
#       *       Matches any characters (any length)
#       ?       Matches a single character
#   Example:
#       PATTERN "test*"    → matches test, tests, test123, test_dir
#       PATTERN "*test"    → matches unittest, tmp_test
#       PATTERN "*test*"   → matches any directory containing test
#
# -----------------------------------------------------------------------
# [Optional parameters (just write parameter name)]
#
# RECURSIVE
#   Function: Enable recursive search, traversing all subdirectories (any depth)
#   Default: Not enabled → only searches the first-level directories under BASE_DIR
#
# DRY_RUN
#   Function: Preview mode, only prints directories that will be deleted, does not actually delete
#   Purpose: Used to safely confirm the deletion scope
#
# VERBOSE
#   Function: Output detailed execution logs
#   Output content: Search directory, match pattern, total matches, all matching directory paths
#
# EXCLUSIONS
#   Function: Specify exclusion keywords (regular expressions). Directories whose names contain these keywords will be skipped (not deleted, not recursed into)
#   Format: Space-separated list of strings/regex patterns
#   Example: 
#       EXCLUSIONS "temp" "backup"  → skips folders named my_temp_data, backup_2024, etc.
#
# ============================== Usage examples ==============================
#
# Example 1: Delete all folders starting with test in the project root directory (recursive + preview + detailed log)
# utils_remove_dirs(
#     BASE_DIR ${CMAKE_SOURCE_DIR}
#     PATTERN "test*"
#     RECURSIVE
#     DRY_RUN
#     VERBOSE
# )
#
# Example 2: Delete only the folder named build in the current directory (non-recursive, direct deletion)
# utils_remove_dirs(
#     BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
#     PATTERN "build"
# )
#
# Example 3: Delete all directories with names containing tmp in the project (recursive deletion)
# utils_remove_dirs(
#     BASE_DIR ${PROJECT_SOURCE_DIR}
#     PATTERN "*tmp*"
#     RECURSIVE
#     VERBOSE
# )
#
# Example 4: Delete folders starting with 'cache', but exclude those containing 'keep' or 'system' in their names
# utils_remove_dirs(
#     BASE_DIR ${CMAKE_SOURCE_DIR}
#     PATTERN "cache*"
#     RECURSIVE
#     EXCLUSIONS "keep" "system"
#     VERBOSE
# )
#
#=======================================================================
function(utils_remove_dirs)
    set(options DRY_RUN RECURSIVE VERBOSE)
    set(oneValueArgs BASE_DIR PATTERN)
    set(multiValueArgs EXCLUSIONS)

    # Parse arguments
    cmake_parse_arguments(ARG
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    # Parameter validation
    if(NOT ARG_BASE_DIR)
        message(FATAL_ERROR "utils_remove_dirs: Missing required parameter BASE_DIR")
    endif()
    if(NOT ARG_PATTERN)
        message(FATAL_ERROR "utils_remove_dirs: Missing required parameter PATTERN")
    endif()
    if(NOT IS_DIRECTORY "${ARG_BASE_DIR}")
        message(FATAL_ERROR "utils_remove_dirs: Directory does not exist ${ARG_BASE_DIR}")
    endif()

    set(RESULT_DIRS "")

    # Macro for recursive directory traversal
    macro(_traverse _cur_dir)
        file(GLOB _children "${_cur_dir}/*")
        foreach(_child ${_children})
            if(IS_DIRECTORY "${_child}")
                get_filename_component(_dir_name "${_child}" NAME)

                set(_should_exclude FALSE)
                foreach(_exclusion IN LISTS ARG_EXCLUSIONS)
                    if(_dir_name MATCHES "${_exclusion}")
                        set(_should_exclude TRUE)
                        break()
                    endif()
                endforeach()

                if(_should_exclude)
                    continue()
                endif()

                # Wildcard matching (convert * ? to CMake regex)
                string(REPLACE "." "\\." ARG_PATTERN_ESC ${ARG_PATTERN})
                string(REPLACE "*" ".*" ARG_PATTERN_REGEX ${ARG_PATTERN_ESC})
                string(REPLACE "?" "." ARG_PATTERN_REGEX ${ARG_PATTERN_REGEX})

                set(_matched FALSE)
                if(_dir_name MATCHES "^${ARG_PATTERN_REGEX}$")
                    set(_matched TRUE)
                    list(APPEND RESULT_DIRS "${_child}")
                endif()

                # Recurse into subdirectories
                if(ARG_RECURSIVE AND NOT _matched)
                    _traverse("${_child}")
                endif()
            endif()
        endforeach()
    endmacro()

    # Start traversal
    _traverse("${ARG_BASE_DIR}")

    # Output detailed logs
    if(ARG_VERBOSE)
        list(LENGTH RESULT_DIRS _cnt)
        message(STATUS "")
        message(STATUS "========================================")
        message(STATUS "Search directory: ${ARG_BASE_DIR}")
        message(STATUS "Match pattern: ${ARG_PATTERN}")
        message(STATUS "Recursive mode: ${ARG_RECURSIVE}")
        message(STATUS "Exclusions: ${ARG_EXCLUSIONS}")
        message(STATUS "Total matches: ${_cnt}")
        message(STATUS "========================================")
    endif()

    # Execute deletion / preview
    foreach(_dir ${RESULT_DIRS})
        if(ARG_DRY_RUN)
            message(STATUS "[DRY_RUN] To be deleted: ${_dir}")
        else()
            file(REMOVE_RECURSE "${_dir}")
            if(ARG_VERBOSE)
                message(STATUS "Deleted: ${_dir}")
            endif()
        endif()
    endforeach()

    if(ARG_VERBOSE AND NOT ARG_DRY_RUN)
        message(STATUS "✅ All matching directories deleted!")
    endif()
endfunction()

function(_tests_utils)
    utils_get_cmake_args_from_cache_vars(RET_VAR FILTER_PREFIX ARA_ FILTER_TYPE BOOL)
    message("cmake_args_from_cmd: ${RET_VAR}")

endfunction(_tests_utils)
