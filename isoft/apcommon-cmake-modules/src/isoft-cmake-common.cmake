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
# @file       isoft-cmake-common.cmake
# @brief
# @details
# @date       2021-04-27
# @author     jiawei
# @version    1.2.0
#
# ================================================================

include(CMakeDependentOption)
# TODO AUTOSAR_FO_EXP_SecurityOverview Security-related compilation options
#########################################################
# part 1 Compilation options
# Workaround for incremental build issue: Force GNUInstallDirs to re-evaluate relative paths on incremental builds instead of hardcoding absolute build paths.
# Alternative: add --fresh to the configure step, not supported by the currently used cmake version 
unset(CMAKE_INSTALL_LIBDIR CACHE)
include(GNUInstallDirs)
macro(apcommon_defaults)
 # Compilation type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
            "Choose the type of build, one of: Debug, Release, RelWithDebInfo, MinSizeRel"
            FORCE)
    endif()

 # Compilation option configuration We always want these flags enabled.
 # The project can use the same name -Wno-xxx to disable
    list(APPEND _apcommon_common_warning_flags
        -Wall
        -Wdeprecated
        -Wextra
        -Wfloat-equal           # floating-point values are used in equality comparisons
        -Wpedantic
        -Wshadow                # a local variable or type declaration shadows another variable, parameter, type, class member, ...
        -Wuninitialized         # an automatic variable is used without first being initialized
        -Wunused                # an unused parameter, variable, function, label, local typedef, value is encountered            
        )

    list(APPEND _apcommon_c_only_warning_flags
        -Wcast-align            # a pointer is cast such that the required alignment of the target is increased
        -Wcast-qual             # a pointer is cast so as to remove a type qualifier
        -Wconversion            # implicit conversions that may alter a value
        -Winit-self             # an uninitialized variable is initialized with itselves
        -Wmissing-include-dirs  # a user-supplied include directory does not exist
        -Wredundant-decls       # anything is declared more than once in the same scope
        -Wwrite-strings         # give string constants the type "const char[length]", and warn about loss of constness when copying
        -Wsign-conversion       # implicit conversions that may change the sign of an integer value
        -Wstrict-prototypes     # a function is declared or defined without specifying the argument types
        )

    list(APPEND _apcommon_cxx_only_warning_flags
        -Wctor-dtor-privacy     # a class seems unusable because all the constructors or destructors in that class are private
        -Wnon-virtual-dtor      # class has virtual functions and an accessible non-virtual destructor
        -Wold-style-cast        # an old-style (C-style) cast to a non-void type is used
        -Woverloaded-virtual    # a function declaration hides virtual functions from a base class
        -Wsign-promo            # overload resolution chooses a promotion from unsigned type to a signed type
        )

    set(c_and_gnu "$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>")
    set(cxx_and_gnu "$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>")
    set(c_or_cxx_and_gnu "$<OR:${c_and_gnu},${cxx_and_gnu}>")

    set(c_and_clang "$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:Clang>>")
    set(cxx_and_clang "$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:Clang>>")
    set(c_or_cxx_and_clang "$<OR:${c_and_clang},${cxx_and_clang}>")
    # Position-Independent Code for  shared lib .so
    add_compile_options("$<${c_or_cxx_and_clang}:-fPIC>")
    
    set(_cxx_like_gnu "$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>")
    set(cxx_and_like_gnu "$<AND:$<COMPILE_LANGUAGE:CXX>,${_cxx_like_gnu}>")

    set(_c_like_gnu "$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>")
    set(c_and_like_gnu "$<AND:$<COMPILE_LANGUAGE:C>,${_c_like_gnu}>")

    set(c_or_cxx_and_like_gnu "$<OR:${c_and_like_gnu},${cxx_and_like_gnu}>")

    add_compile_options("$<${c_or_cxx_and_like_gnu}:${_apcommon_common_warning_flags}>")
    add_compile_options("$<${c_and_like_gnu}:${_apcommon_c_only_warning_flags}>")
    add_compile_options("$<${cxx_and_like_gnu}:${_apcommon_cxx_only_warning_flags}>")

    unset(_apcommon_cxx_only_warning_flags)
    unset(_apcommon_c_only_warning_flags)
    unset(_apcommon_common_warning_flags)

 # Compatibility standard: static destructors
    add_compile_options("$<${cxx_and_like_gnu}:-fuse-cxa-atexit>")
    # stack protector
    add_compile_options("$<${c_or_cxx_and_gnu}:-fstack-protector-strong>")
    # Position-Independent Code for  shared lib .so
    add_compile_options("$<${c_or_cxx_and_gnu}:-fPIC>")

    # disable rtti
    # add_compile_options("$<${c_or_cxx_and_gnu}:-fno-rtti>")

 # Enable full RELRO (read only relocation)
 # This compilation option will cause runtime loading to be too slow, leading to some process timeouts. Normally it is disabled, and enabled only on a project-driven basis.
    # link_libraries("$<BUILD_INTERFACE:$<$<OR:$<C_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:GNU>>:-Wl,-z,relro,-z,now>>")

 # Set C++14 && C99 && disable exceptions
    set(CMAKE_CXX_EXTENSIONS OFF)
    option(BUILD_WITH_CXX17 "Use C++17 standard" OFF)

    if(BUILD_WITH_CXX17)
        set(CMAKE_CXX_STANDARD 17)
    else()
        set(CMAKE_CXX_STANDARD 14)
    endif()

    set(CMAKE_C_EXTENSIONS OFF)
    set(CMAKE_C_STANDARD 99)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(/DDEBUG)
    endif()
    
    option(ARA_ENABLE_TESTS "Enable unit testing" OFF)
    option(ARA_RUN_TESTS "Enable running unit testing" OFF)
    option(ARA_ENABLE_DOXYGEN "Enables the documentation target" OFF)
    
    set(PLATFORM_FRAMEWORK_VERSION 1.0.0) 
endmacro()

#########################################################
function(isoftap_install_app_executable SWC_NAME SWC_VERSION  TARGET)
    message(WARNING "!!!isoftap_install_app_executable function is removed!!! you can delete this function call or recreate project from IDE")
endfunction()
function(isoftap_install_app_file SWC_NAME SWC_VERSION PROCESS_NAME DIR_NAME FILE_NAME)
    message(WARNING "!!!isoftap_install_app_file function is removed!!! you can delete this function call or recreate project from IDE")
endfunction()
function(isoftap_install_ara_file DIR_NAME FILE_NAME)
    message(WARNING "!!!isoftap_install_ara_file function is removed!!! you can delete this function call or recreate project from IDE")
endfunction()
function(isoftap_install_app_file_rename SWC_NAME SWC_VERSION PROCESS_NAME DIR_NAME FILE_NAME RENAME_NAME)
    message(WARNING "!!!isoftap_instalisoftap_install_app_file_renamel_ara_file function is removed!!! you can delete this function call or recreate project from IDE")
endfunction()

# Apply default configuration
apcommon_defaults()
