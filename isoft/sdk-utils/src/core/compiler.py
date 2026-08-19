#!/usr/bin/env python3

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
# @file       compiler.py
# @brief      Compiler abstract class
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import sys, os, stat
import shutil
from abc import ABC, abstractmethod
from pathlib import Path
from typing import List, Optional, Dict, Any
from dataclasses import dataclass, field, asdict
import json
import subprocess

__SDK_UTILS_SRC_DIR = str(Path(__file__).resolve().parent.parent)
if __SDK_UTILS_SRC_DIR not in sys.path:
    sys.path.insert(0, __SDK_UTILS_SRC_DIR)

from core import Fsh, global_fsh
from core import Logger, global_logger
import utils

__all__ = ["CompilerInterface", "CompilerProfile"]


@dataclass()
class CompilerProfile:
    """
    Define compiler standard attributes
    """
    NAME:str = None                # Compiler name
    VERSION:str = None             # Compiler version
    DESCRIPTION:str = None         # Compiler description
    PROJECT:str = None             # Project source
    IS_CROSS_COMPILER:bool = False # Whether it is a cross-compilation toolchain
    BIN_DIR:str = None             # Directory where compiler executables are located, relative path starting from the toolchain top-level directory
    TARGET:str = None              # Compiler target architecture name, e.g., "x86_64-linux-gnu"
    HOST:str = None                # Compiler host architecture name, e.g., "x86_64-linux-gnu"
    BUILD:str = None               # Compiler build architecture name, e.g., "x86_64-linux-gnu"
    TARGET_ARCH:str = None         # CPU architecture name of the target program, e.g., x86_64, aarch64, etc.
    INSTALL_PREFIX:str = "/usr"    # Installation path after building the project source code
    INSTALL_LIB:str = "lib"        # Name of the lib directory for installation after building the project source code (lib/lib64)
    PREFIX:str = ""                # Compiler name prefix, e.g., aarch64-linux-gnu-
    CC:str = "gcc"
    CXX:str = "g++"
    LD:str = "ld"
    AS:str = "as"
    AR:str = "ar"
    STRIP:str = "strip"
    CFLAGS:str = ""
    CXXFLAGS:str = ""
    LDFLAGS:str = ""
    CMAKE_ARGS:str = ""                         # CMake arguments configured for the compiler
    CMAKE_SYSTEM_NAME:str = "Linux"             # Default is Linux
    CMAKE_SYSTEM_VERSION:str = None             # NDK version required for Android
    CMAKE_FILE_CUSTOM_LINES:List[str] = None    # Compiler-specific special CMake configurations to be written into toolchain.cmake
    ENVIRON:Dict = None                         # Environment variables required for compiler execution
    DOWNLOAD_URL:str = None                     # Compiler download URL, used for install operation
    INSTALL_SCRIPT:str = None                   # Installation script, used to perform specific installation operations for the bundled toolchain when installing the SDK
    TARGET_SYSROOT_DIR:str = None               # Directory name of the bundled target system sysroot (relative path based on the toolchain installation directory)


    def to_dict(self):
        """Convert to dict"""
        try:
            #return json.dumps(asdict(self))
            return asdict(self)
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None

    
    def to_json(self):
        """Convert to Json"""
        try:
            return json.dumps(asdict(self), indent=4, ensure_ascii=False, sort_keys=False)
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None


class CompilerInterface(ABC):
    """
    Compiler abstract interface class, defining the basic operations a compiler should have
    """
    @abstractmethod
    def __init__(self) -> None:
        """
        
            Initialize the compiler
        """
        # Compiler attributes
        self.profile = CompilerProfile()
        self._fsh = global_fsh


    def get_bin_path(self) -> Path:
        # If the configured bin directory is an absolute path, return it directly
        if "/" == str(self.profile.BIN_DIR)[0]:
            return Path(self.profile.BIN_DIR)
        # If the configured bin directory is a relative path, assume the toolchain is placed in the toolchain directory of this project
        else:
            return self._fsh.TOOLCHAIN_DIR / self.profile.BIN_DIR


    def get_environs(self) -> Dict:
        """
        
            Get the environment variables required for compiler execution
        Returns:
            Environment variable dictionary
        """
        env = {}
        # Common environment variables
        env["PATH"] = f"{self.get_bin_path()}:{os.environ['PATH']}"
        env["CC"] = self.profile.CC
        env["CXX"] = self.profile.CXX
        if None is not self.profile.CFLAGS:
            env["CFLAGS"] = self.profile.CFLAGS
        if None is not self.profile.CXXFLAGS:
            env["CXXFLAGS"] = self.profile.CXXFLAGS
        if None is not self.profile.LDFLAGS:
            env["LDFLAGS"] = self.profile.LDFLAGS
        # Configure pkgconfig environment variables
        env["PKG_CONFIG_PATH"] = f'{self._fsh.SYSROOT_DIR}{self.profile.INSTALL_PREFIX}/{self.profile.INSTALL_LIB}/"pkgconfig"'
        env["PKG_CONFIG_SYSROOT_DIR"] = str(self._fsh.SYSROOT_DIR)
        env["PKG_CONFIG_ALLOW_SYSTEM_LIBS"] = "true"
        env["LIBRARY_PATH"] = self._fsh.SYSROOT_DIR / "usr/lib"
        # Toolchain-specific environment variables
        if None is not self.profile.ENVIRON:
            env.update(self.profile.ENVIRON)
        return env


    def get_cmake_args(self):
        """
        
            Get CMake arguments
        """
        cmake_args = f"-DCMAKE_TOOLCHAIN_FILE={self._fsh.TOOLCHAIN_DIR}/toolchain.cmake " \
            + self.profile.CMAKE_ARGS
        # cmake_args might be a string containing spaces, need to be enclosed in double quotes to prevent word splitting by the shell command line
        return cmake_args

    
    def install(self, toolchain_install_dir:Path = None) -> bool:
        """
        
            Install the compiler
        Args:
            install_path: Installation path; if not specified, install to default path
        Returns:
            bool: Whether installation succeeded
        """
        try:
            if None is toolchain_install_dir:
                toolchain_install_dir = self._fsh.TOOLCHAIN_DIR

            # No need to reinstall if already present locally
            # TODO: Write a common detection method
            if os.access(self._fsh.TOOLCHAIN_DIR/ "toolchain.cmake", os.F_OK):
                return True
            
            # Generate cmake file
            if not self._gen_cmake_file():
                raise Exception("gen_cmake_file()")

            if None is self.profile.DOWNLOAD_URL:
                global_logger.info(f"toolchain {self.profile.NAME} has not configured the DOWNLOAD_URL, install() is ignored.")
                return True

            # Download
            os.makedirs(self._fsh.SOURCE_DIR, exist_ok=True)
            cross_tool_fname = self.profile.DOWNLOAD_URL.split("/")[-1]
            cross_tool_fpath = self._fsh.SOURCE_DIR / cross_tool_fname
            # If there is a local download cache, do not re-download
            # TODO: Check MD5 sum; if incorrect, re-download
            if not os.access(cross_tool_fpath, os.F_OK):
                global_logger.info(f"Downloading {self.profile.DOWNLOAD_URL}")
                cmd = f"wget -cnv -t 3 {self.profile.DOWNLOAD_URL} -O {cross_tool_fpath}"
                if 0 != subprocess.call(cmd, shell=True, stderr=global_logger.CHANNEL_FILE, stdout=global_logger.CHANNEL_FILE):
                    raise Exception(cmd)

            # If already installed, do not reinstall
            # TODO: Write a robust method to detect if the compiler is already installed
            gcc_path = self.get_bin_path()  / f"{self.profile.CC}"
            if not os.access(gcc_path, os.F_OK):
                global_logger.info(f"Unpacking {cross_tool_fpath}")
                return self._unpack(cross_tool_fpath, toolchain_install_dir)
            return True 

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def _unpack(self, toolchain_fpath:Path, toolchain_install_dir:Path):
        """
        
            Extract and install the toolchain file
        Args:
            toolchain_fpath: Toolchain file (archive)
            toolchain_install_path: Installation path
        Returns:
            bool: Whether installation succeeded
        """
        try:
            # Detect toolchain file type
            if utils.is_tarfile(toolchain_fpath):
                # tar or tar.gz
                os.makedirs(toolchain_install_dir, exist_ok=True)
                toolchain_topdir = utils.get_tarfile_top_dir(toolchain_fpath)
                if None is not toolchain_topdir:
                    # If it contains a top-level directory, strip it when extracting
                    utils.uncompress_file(toolchain_fpath, self._fsh.TOOLCHAIN_DIR, strip_topdir=True)
                else:
                    # If it does not contain a top-level directory, extract directly to toolchain
                    utils.uncompress_file(toolchain_fpath, self._fsh.TOOLCHAIN_DIR, strip_topdir=False)
            elif utils.is_shell_script(toolchain_fpath):
                # yocto
                # Set executable permissions
                os.chmod(toolchain_fpath, stat.S_IRWXU)
                # Install and extract
                cmd = f"bash {toolchain_fpath} -y -d {toolchain_install_dir}"
                if 0 != subprocess.call(cmd, shell=True, stderr=global_logger.CHANNEL_FILE, stdout=global_logger.CHANNEL_FILE):
                    raise Exception(str(cmd)) 
            else:
                # Other forms of build toolchains are not supported for now
                raise Exception("unkown toolchain")
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
    

    def init(self):
        return self.copy_sysroot()

    
    def copy_sysroot(self) -> bool:
        """
        
            Generate sysroot, copy the internal sysroot of the compiler to the specified directory. This is done to avoid polluting the compiler's own sysroot.
        Returns:
            bool: Whether generation succeeded
        """
        try:
            if None is self.profile.TARGET_SYSROOT_DIR:
                global_logger.info(f"toolchain {self.profile.NAME} has not configured the TARGET_SYSROOT_DIR, copy_sysroot() is ignored.")
                return True
            # TODO: Write a generic detection function
            if os.access(self._fsh.SYSROOT_DIR / "lib/libc.so.6", os.F_OK):
                return True
            # TODO: The toolchain might be installed to a path other than the default location
            toolchain_inner_sysroot = self._fsh.TOOLCHAIN_DIR / self.profile.TARGET_SYSROOT_DIR
            sc = SysrootCreator(toolchain_inner_sysroot, self._fsh.SYSROOT_DIR)
            return sc.export_minimal()

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

    
    def package(self, output_dir: Path, input_dir: Path = None) -> bool:
        """
        
            Package the current compiler to the specified directory for installation and deployment elsewhere.
        Args:
            output_dir: Output directory
            input_dir: Original toolchain directory; if not specified, use the default location
        Returns:
            bool: Whether export succeeded
        """
        try:
            if None is output_dir:
                raise Exception("None is output_dir")

            toolchain_dir = self._fsh.TOOLCHAIN_DIR
            if input_dir is not None:
                toolchain_dir = Path(input_dir).absolute()
            
            # Copy cmake file
            shutil.copy(toolchain_dir / "toolchain.cmake", output_dir)
            
            # Toolchains without a configured download URL do not provide automatic packaging functionality
            if None is self.profile.DOWNLOAD_URL:
                global_logger.info(f"toolchain {self.profile.NAME} has not configured the DOWNLOAD_URL, package() is ignored.")
                return True

            output_dir = Path(output_dir)
            toolchain_fname = self.profile.DOWNLOAD_URL.split("/")[-1]
            toolchain_fpath = self._fsh.SOURCE_DIR / toolchain_fname
                
            # Detect toolchain file type
            if utils.is_tarfile(toolchain_fpath):
                # If it is tar or tar.gz, directly copy the installed toolchain
                shutil.copytree(src=toolchain_dir, dst=output_dir, symlinks=True, dirs_exist_ok=True)
            elif utils.is_shell_script(toolchain_fpath):
                # If it is a self-extracting toolchain generated by yocto, copy the original file
                shutil.copy(toolchain_fpath, output_dir)
                # Write the installation script; ABET will use this script for toolchain installation operations
                with open(output_dir / "install.sh", "w") as f:
                    f.write(self.profile.INSTALL_SCRIPT)
                os.chmod(output_dir / "install.sh", 0o755)
            else:
                raise Exception("unkown toolchain")
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def _replace_path_to_expression(self, src_str:str) -> str:
        ret = str(src_str)
        ret = ret.replace(f"{self._fsh.SYSROOT_DIR}", "${SYSROOT}")
        ret = ret.replace(f"{self._fsh.TOOLCHAIN_DIR}", "${TOOLCHAIN_DIR}")
        return ret


    def _gen_cmake_file(self, install_path: Path = None) -> bool:
        """
        
            Generate toolchain.cmake configuration
        Args:
            install_path: Installation directory; if not specified, install to default directory
        Returns:
            bool: Whether succeeded
        """
        try:
            if None is install_path:
                install_path = self._fsh.TOOLCHAIN_DIR
            os.makedirs(install_path, mode=0o755, exist_ok=True)
            cmake_template_file = self._fsh.TEMPLATE_DIR / "toolchain.cmake.template"
            cmake_file = install_path / "toolchain.cmake"

            # Copy the toolchain.cmake template
            shutil.copy(dst=cmake_file, src=cmake_template_file)

            # Render the toolchain.cmake file
            # Generate environment variables
            env_lines = "############### Setting environment variables #################\n"
            bin_path = self._replace_path_to_expression(self.get_bin_path())
            env_lines += (f"set(ENV{{PATH}} \"{bin_path}:$ENV{{PATH}}\")\n")
            # Configure pkgconfig environment variables
            env_lines += f"set(ENV{{PKG_CONFIG_PATH}} \"${{SYSROOT}}/{self.profile.INSTALL_PREFIX}/{self.profile.INSTALL_LIB}/pkgconfig\")\n"
            env_lines += "set(ENV{PKG_CONFIG_SYSROOT_DIR} \"${SYSROOT}\")\n"
            env_lines += "set(ENV{PKG_CONFIG_ALLOW_SYSTEM_LIBS} \"true\")\n"
            if None is not self.profile.ENVIRON:
                for env_key in self.profile.ENVIRON:
                    # Environment variable setting method in cmake: set(ENV{LD_LIBRARY_PATH} "/opt/toolchain/lib:$ENV{LD_LIBRARY_PATH}")
                    # Replace all sysroot paths with variables
                    env_value = self._replace_path_to_expression(self.profile.ENVIRON[env_key])
                    env_str = f"set(ENV{{{env_key}}} \"{env_value}:$ENV{{{env_key}}}\")\n"
                    env_lines += env_str

            cmake_system_name = self.profile.CMAKE_SYSTEM_NAME
            cmake_system_version = self.profile.CMAKE_SYSTEM_VERSION
            if None is cmake_system_name:
                cmake_system_name = ""
            if None is cmake_system_version:
                cmake_system_version = ""
            is_cross_compiler = "OFF"
            if self.profile.IS_CROSS_COMPILER:
                is_cross_compiler = "ON"
            value_mapping = [
                ("ENVIRONS", env_lines),
                ("TOOLCHAIN_NAME", self.profile.NAME),
                ("CC", self.profile.CC),
                ("CXX", self.profile.CXX),
                ("CXXFLAGS", self._replace_path_to_expression(self.profile.CXXFLAGS)),
                ("CFLAGS", self._replace_path_to_expression(self.profile.CFLAGS)),
                ("LDFLAGS", self._replace_path_to_expression(self.profile.LDFLAGS)),
                ("CMAKE_SYSTEM_NAME", cmake_system_name),
                ("CMAKE_SYSTEM_VERSION", cmake_system_version),
                ("CMAKE_SYSTEM_PROCESSOR", self.profile.TARGET_ARCH),
                ("IS_CROSS_COMPILER", is_cross_compiler)
            ]
            utils.render_template_file(cmake_file, value_mapping)

            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

class SysrootCreator:
    """sysroot generator
    Used to copy the original sysroot to the target location, with filtering and stripping
    """
    def __init__(self, src:Path, dst:Path):
        self._src_sysroot = Path(src).resolve()
        self._dest_sysroot = Path(dst).resolve()
        self._include_dirs = [
            "include",
            "usr/include"
        ]
        # TODO: Support regular expressions
        self._libs = [
            # C runtime core libraries
            "libc.so",
            "libc_nonshared.a",
            "libm.so", 
            "libpthread.so",
            "librt.so",
            "libdl.so",
            "libresolv.so",
            "libcrypt.so",
            "libatomic.so",
            # C++ runtime libraries
            "libstdc++.so",
            # Android C++ libraries
            "libc++_shared.so",
            "libc++.so",
            "libgcc",
            # System tool dependencies
            "libutil.so",
            "libnss_files.so",
            "libnss_dns.so",
            "libtest.so",
            # Dynamic linker
            "ld-linux"
        ]

    def export_minimal(self):
        try:
            os.makedirs(self._dest_sysroot, exist_ok=True)
            # Copy include
            for include_dir in self._include_dirs:
                src_dir_path = self._src_sysroot / include_dir
                if not src_dir_path.exists():
                    continue
                dest_dir_path = self._dest_sysroot / include_dir
                os.makedirs(dest_dir_path, exist_ok=True)
                shutil.copytree(src_dir_path, dest_dir_path, dirs_exist_ok=True, symlinks=True, ignore_dangling_symlinks=True)

            # Copy library
            self._copy_libraries()

            return True
        except Exception as e:
            print(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
            
    
    def _copy_libraries(self):
        """
        Search for files with the specified prefix in the source directory, copy them to the target directory while preserving the directory structure
        Args:
            prefix: File prefix
        """
        try:
            # Traverse the source directory tree
            for root, dirs, files in os.walk(self._src_sysroot):
                current_dir = Path(root)
                # Iterate over all files in the current directory
                for file in files:
                    src_fpath = current_dir / file
                    # Copy all .o files, which are needed by the compiler
                    if "o" == file.split(".")[-1]:
                        self._copy_file(src_fpath)
                        continue
                    # Iterate over Lib prefixes
                    for lib_prefix in self._libs:
                        # Check if the file has the specified prefix
                        if file.startswith(lib_prefix):
                            self._copy_file(src_fpath)

            return True
        except Exception as e:
            print(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

        
    def _copy_file(self, src_fpath:Path):
        """
        If it is a regular file, copy it directly.
        If it is a symbolic link, create it recursively based on the link target, e.g.: libm.so -> libm.so.6 -> libm-2.31.so
        """
        try:
            src_dir = src_fpath.parent
            relative_subdir = src_dir.relative_to(self._src_sysroot.resolve())
            dest_dir = self._dest_sysroot / relative_subdir
            dest_fpath = dest_dir / src_fpath.name
            os.makedirs(dest_dir, exist_ok=True)
            if src_fpath.is_symlink():
                link_target_file = os.readlink(src_fpath)
                # Simple check for circular symlinks to prevent infinite recursion
                if "." == link_target_file or src_fpath.name == link_target_file:
                    return True
                # Create the current symbolic link file
                if not dest_fpath.is_symlink() and not dest_fpath.exists():
                    os.symlink(link_target_file, dest_fpath)
                # Get link_target_fpath and create recursively
                link_target_fpath =  src_dir / link_target_file
                return self._copy_file(link_target_fpath)
            elif src_fpath.is_file():
                shutil.copy2(src_fpath, dest_fpath)
            return True

        except Exception as e:
            print(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
