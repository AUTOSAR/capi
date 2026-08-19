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
# @file       builder.py
# @brief
# @details
# @date       2025-10-31
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import sys, os, shutil, json
import psutil
from pathlib import Path
import importlib
import subprocess
import datetime

__SDK_UTILS_SRC_DIR = str(Path(__file__).resolve().parent.parent)
if __SDK_UTILS_SRC_DIR not in sys.path:
    sys.path.insert(0, __SDK_UTILS_SRC_DIR)

from core import Fsh, global_fsh
from core import CompilerInterface, CompilerProfile
from config import BuildArguments, BuildType, CmdLineParser
from config import ApallInfo
from core import Logger, global_logger
import utils
from config import Package, PackageManager, split_package_descriptor

__all__ = ["Builder"]


class Stack:
    """List-based stack implementation"""
    def __init__(self, len):
        self._stack = []
        self._limit = len
        return
    

    def push(self, data):
        if len(self._stack) >= self._limit:
            return False
        self._stack.append(data)
        return True


    def pop(self):
        if self.empty():
            return None
        ret = self._stack[-1]
        del self._stack[-1]
        return ret


    def empty(self):
        if len(self._stack) <= 0:
            return True
        else:
            return False


    def top(self):
        if self.empty():
            return None
        else:
            return self._stack[-1]


    def bottom(self):
        if self.empty():
            return None
        else:
            return self._stack[0]


"""
Used to mark the build status of each software package.
Statuses are divided into: download, preprocess, configure, build, install.
Used to reduce redundant steps, similar to how make projects reduce build workload by checking timestamps.
Instead of establishing a unified global status identifier for all software packages, a separate status file is created for each software package within its own build directory. This allows recompilation by simply deleting the package's build directory.
"""
class BuildState:
    """Build stage flag file. A flag is written to the file upon completion of each stage."""

    _BUILD_STATE_FILE = ".build_state.json"
    _STATE_DLD = "download"
    _STATE_PRE = "preproc"
    _STATE_CMP = "compile"
    _STATE_CFG = "config"
    _STATE_IST = "install"

    def __init__(self, pkg:Package):
        self._flag_file_path:Path = global_fsh.BUILD_DIR / pkg.NAME / self._BUILD_STATE_FILE
        self.name = pkg.NAME

    
    def compiled(self, value=None):
        return self._get_or_set(self._STATE_CMP, value)

    
    def downloaded(self, value=None):
        return self._get_or_set(self._STATE_DLD, value)


    def preprocessed(self, value=None):
        return self._get_or_set(self._STATE_PRE, value)

        
    def configed(self, value=None):
        return self._get_or_set(self._STATE_CFG, value)

    
    def installed(self, value=None):
        return self._get_or_set(self._STATE_IST, value)


    def _set_flag(self, flag, value):
        try:
            flags_data = {}
            if self._flag_file_path.exists():
                with open(self._flag_file_path, 'rt') as f:
                    flags_data = json.load(f)
            flags_data[flag] = value
            with open(self._flag_file_path, "wt") as f:
                json.dump(flags_data, f, indent=4)
            return True
        except Exception as e:
            return False


    def _get_flag(self, flag):
        try:
            # exceptional handling for apall - this is always handled as if its build states are false
            if self.name == "apall":
                return False
            flags_data = None
            with open(self._flag_file_path, 'rt') as f:
                flags_data = json.load(f)
            ret = flags_data.get(flag)
            if None is ret:
                return False
            return ret
        except Exception as e:
            return False


    def _get_or_set(self, flag, value=None):
        if None is not value:
            # With argument, set the flag value
            return self._set_flag(flag, value)
        else:
            # Without argument, get the flag value
            return self._get_flag(flag)


class Downloader:
    """
    Source code downloader.
    """
    def __init__(self, pkg:Package, build_dir:Path, source_dir:Path):
        # TODO: Add MD5 checksum to software package configuration, verify after source download completes
        self._pkg = pkg
        # Target directory for extracted source code (build directory), the parent directory of this source package's top-level directory
        self._build_dir = build_dir
        # Directory where downloaded source packages are stored
        self._source_dir = source_dir
        return


    def run(self):
        try:
            os.makedirs(self._source_dir, exist_ok=True)
            url_type = self._parse_url_type(self._pkg.URL)
            if None is url_type:
                return False
            if "file" == url_type:
                return self._fetch_from_local()
            elif "git" == url_type:
                return self._fetch_from_git()
            elif "net" == url_type:
                return self._fetch_from_net()
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    """Identify URL type"""
    def _parse_url_type(self, url:str):
        try:
            if 0 == len(url.strip()):
                return None
            # XX.git
            if "git" == url.split('.')[-1]:
                return "git"
            # http:XX or https:XX or ftp:XX
            elif "http" == url.split(':')[0] or \
                "https" == url.split(':')[0] or \
                    "ftp" == url.split(':')[0]:
                return "net"
            else:
                return "file"
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None

    
    def _fetch_from_git(self):
        """Clone repository via git"""

        try:
            cache_path = self._source_dir / self._pkg.NAME
            dest_path = self._build_dir / self._pkg.NAME
            # If a cloned repository already exists in the local cache, do not clone again
            if not cache_path.is_dir():
                cmd = f"git clone {self._pkg.URL} {cache_path}"
                if 0 != subprocess.call(cmd, shell=True, stdout=global_logger.CHANNEL_FILE, stderr=global_logger.CHANNEL_FILE):
                    raise Exception(cmd)
            # Switch to the specified branch
            cmd = f"git -C {cache_path} checkout {self._pkg.VERSION}"
            if 0 != subprocess.call(cmd, shell=True, stdout=global_logger.CHANNEL_FILE, stderr=global_logger.CHANNEL_FILE):
                raise Exception(cmd)
            # Pull the latest commits
            cmd = f"git -C {cache_path} pull"
            if 0 != subprocess.call(cmd, shell=True, stdout=global_logger.CHANNEL_FILE, stderr=global_logger.CHANNEL_FILE):
                raise Exception(cmd)
            # Copy to the target directory
            shutil.copytree(cache_path, dest_path)
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def _fetch_from_net(self):
        """Download software package via network"""

        try:
            fname: str = self._pkg.URL.split("/")[-1]
            if self._pkg.NAME and self._pkg.VERSION:
                fname = f"{self._pkg.NAME}-{self._pkg.VERSION}.{utils.get_pkg_ext(fname)}"

            cache_path = self._source_dir / fname
            dest_path = self._build_dir / self._pkg.NAME
            # If a downloaded package already exists in the local cache, do not download again
            if not os.access(cache_path, os.F_OK):
                # Download
                cmd = f"wget -cnv -t 3 {self._pkg.URL} -O {cache_path}"
                if 0 != subprocess.call(cmd, shell=True, stderr=global_logger.CHANNEL_FILE, stdout=global_logger.CHANNEL_FILE):
                    raise Exception(cmd)
            # Extract
            os.makedirs(dest_path, exist_ok=True)
            return utils.uncompress_file(cache_path, dest_path, strip_topdir=True)
        except Exception as e:
            os.remove(cache_path)
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def _fetch_from_local(self):
        """Copy from local"""

        try:
            dest_path = self._build_dir / self._pkg.NAME
            p = Path(self._pkg.URL).resolve()
            if not p.exists():
                raise Exception(f"No such file : {p}")
            if p.is_dir():
                # copy the src but make sure that no self-copy occurs

                global_logger.info(f"copying local dir {p} to {dest_path}")

                # use rsync to copy only what is needed, allowing caching
                # exclude files that shall not be copied from source (e.g. build)
                # also exclude files that shall not be deleted in the target just because they do not exist in the source
                # this is the bulk of the excludes. e.g. /isoft/nsomeip/pdb There the build creates new files
                # removing them each time triggers unnecessary re-builds
                # note: add "--itemize-changes", to see what changes in detail
                subprocess.run(["rsync", "-a", "--omit-dir-times", "--delete", "--exclude=__pycache__", "--exclude=/.build", "--exclude=**.build", "--exclude=/build", "--exclude=/isoft/nai/thirdparty", "--exclude=/isoft/nai/pdb", "--exclude=/isoft/nai/lib", "--exclude=/isoft/nai/bin", "--exclude=/isoft/nai/include/nai/config", "--exclude=/isoft/npc/thirdparty", "--exclude=/isoft/npc/pdb", "--exclude=/isoft/npc/lib", "--exclude=/isoft/npc/bin", "--exclude=/isoft/npc/include/npc/config", "--exclude=/isoft/nsomeip/thirdparty", "--exclude=/isoft/nsomeip/pdb", "--exclude=/isoft/nsomeip/lib", "--exclude=/isoft/nsomeip/bin", "--exclude=/isoft/nsomeip/include/nsomeip/config",  "--exclude=/.build_state.json", f"{p}/", dest_path], timeout=10, check=True)

                return True
            elif p.is_file():
                if not utils.is_tarfile(p):
                    raise Exception("invalid file format, need tar or tar.gz")
                # If the archive contains a top-level directory, strip it when extracting
                strip = False
                if None is not utils.get_tarfile_top_dir(p):
                    strip = True
                os.makedirs(dest_path, exist_ok=True)
                return utils.uncompress_file(p, dest_path, strip_topdir=strip)
            else:
                raise Exception("invalid file type")
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


class Builder:
    """SDK toolset - Software package builder."""

    def __init__(self, args:BuildArguments):
        # Builder configuration, from command line arguments or configuration file
        self._args:BuildArguments = args
        # Build toolchain object
        self._toolchain:CompilerInterface = None
        # Software package manager
        self._package_manager:PackageManager = None
        # Global file directory structure
        self._fsh:Fsh = global_fsh


    def run(self) -> bool:
        """Perform the build process

        Returns:
            bool: True, success; False, failure;
        """
        # Load software package information
        global_logger.info("loading packages ...")
        if not self._load_package():
            global_logger.error("loading package")
            return False
        global_logger.info("loading packages ... Done")
        
        # Initialize the toolchain
        global_logger.info("initializing toolchain ...")
        if not self._init_toolchain():
            global_logger.error("initializing toolchain")
            return False
        global_logger.info("initializing toolchain ... Done")

        # Build the software package
        global_logger.info("building packages ...")
        if not self._build_package():
            global_logger.error("building packages")
            return False
        global_logger.info("building packages ... Done")
        
        # Export build information
        global_logger.info("exporting build info ...")
        if not self._export_release_info():
            global_logger.error("exporting build info")
            return False
        global_logger.info("exporting build info ... Done")

        # Return True if no exception occurred
        return True


    #@handle_exception(False)
    def _init_toolchain(self):
        """Initialize the build toolchain."""

        tc_name = self._args.TOOLCHAIN
        # Create a toolchain instance based on the obtained toolchain name
        if None is tc_name:
            return False
        tool = importlib.import_module("toolchain." + tc_name)
        self._toolchain = eval("tool.Compiler")()
        # Perform installation operation
        if not self._toolchain.install():
            raise Exception("toolchain.install()")

        # Check if the specified compiler matches the current sysroot
        toolchain_target_arch = self._toolchain.profile.TARGET_ARCH
        sysroot_arch = utils.SysrootManager(self._fsh.SYSROOT_DIR).get_arch()
        if None is not sysroot_arch:
            if sysroot_arch.lower() != toolchain_target_arch.lower():
                raise Exception(f"toolchain:{toolchain_target_arch} != sysroot:{sysroot_arch}")
        return self._toolchain.init()


    def _replace_path_to_expression(self, src_str:str) -> str:
        ret = str(src_str)
        ret = ret.replace(f"{self._fsh.SYSROOT_DIR}", "${SYSROOT_DIR}")
        ret = ret.replace(f"{self._fsh.TOOLCHAIN_DIR}", "${TOOLCHAIN_DIR}")
        return ret


    def _export_release_info(self):
        """
        Export release information to prepare for subsequent operations like exporting ARE/SDK/PACK, etc.
        
            Expected information to obtain includes:
                Build parameters:
                TOOLCHAIN_NAME
                BUILD_TYPE
                CMAKE_ARGS
                ENVIRON
                APALL information:
                BRANCH
                COMMIT_ID
                VERSION
                SUBMODULES
        Returns:
            bool: Success or failure
        """
        try:
            release_info = {}
            build_args = {}
            build_args["toolchain"] = self._toolchain.profile.NAME
            build_args["build_type"] = self._args.BUILD_TYPE.value
            build_version = self._args.BUILD_VERSION
            if None is build_version:
                now = datetime.datetime.now()
                build_version = datetime.datetime.strftime(now, "%m%d")
            build_args["build_version"] = build_version
            build_args["cmake_args"] = self._replace_path_to_expression(self._toolchain.get_cmake_args())
            build_args["environ"] = {}
            toolchain_origin_envs = self._toolchain.get_environs()
            for key in toolchain_origin_envs:
                build_args["environ"][key] = self._replace_path_to_expression(toolchain_origin_envs[key])
            # Correct the PATH environment variable to contain only the toolchain's bin
            build_args["environ"]["PATH"] = self._replace_path_to_expression(self._toolchain.get_bin_path())
            # TODO: Temporarily write SDK_IDE_API_VERSION directly here; later change to global configuration
            release_info["build_args"] = build_args
            # Get source information for apall
            apall_info = ApallInfo(self._fsh.BUILD_DIR / "apall").load()
            if None is apall_info:
                raise Exception("loading apall info")
            release_info["apall_info"] = apall_info
            # Write all information to the SDK's release.json file
            json_path = self._fsh.SYSROOT_DIR / self._fsh.RELEASE_FILE_NAME
            with open(json_path, "w", encoding="utf-8") as f:
                json.dump(release_info, f, indent=4, ensure_ascii=False, sort_keys=False)
            # Copy the ARE runtime script
            shutil.copy(self._fsh.TEMPLATE_DIR / "are-run.sh", self._fsh.SYSROOT_DIR / "run.sh")
            os.chmod(self._fsh.SYSROOT_DIR / "run.sh", 0o755)

            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
            
            
    def _load_package(self):
        """
        Load all software package information
        """
        self._package_manager = PackageManager()
        if not self._package_manager.load():
            return False
        # Check if building software packages via a configuration file
        pkg_config_file = self._args.PACKAGE_CONFIG
        if None is not pkg_config_file:
            # Add software packages from the configuration file to the package manager
            return self._package_manager.add(pkg_config_file)

        return True


    def _build_package(self):
        """
        
            Build all user-specified software packages and their dependencies, performing download, preprocess, configure, build, install sequentially.
        Args:
            None
        Returns:
            bool: Whether execution was successful
        """
        try:
            # Loop to build user-specified software packages
            pkg_config_file = self._args.PACKAGE_CONFIG
            pkg_build_list = self._args.PACKAGE_LIST
            if None is not pkg_config_file:
                # Update the build list
                pkg = Package(pkg_config_file)
                pkg_build_list = [f"{pkg.NAME}:{pkg.VERSION}"]

            for pkg in pkg_build_list:
                if not self._build_with_depends(pkg):
                    return False
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

    
    def _is_all_depends_finished(self, package:Package):
        """
        
            Determine if all dependencies have been built
        Args:
            pkg: Software package
        Returns:
            bool: Whether completed
        """
        if None is package.DEPEND:
            return True
        for dep_pkg in package.DEPEND:
            pkg = self._package_manager.find(dep_pkg)
            if None is pkg:
                return False
            if not BuildState(pkg).installed():
                return False
        #end for
        return True

    
    def _multi_cmd_to_one_line(self, cmd):
        """
        
            Combine multiple shell commands into one
        Args:
            cmd: Original shell command string or list
        Returns:
            Single-line shell command string
        """
        try:
            if isinstance(cmd, str):
                return cmd 
            elif isinstance(cmd, list):
                cmd_str = ""
                for c in cmd:
                    cmd_str += c + ";"
                return cmd_str
            else:
                return None
        except Exception as e:
            return None


    def _render_command(self, cmd):
        """
        
            Render the command, replacing placeholders in the build configuration script with actual strings
        Args:
            cmd: Original command line
        Returns:
            cmd: Rendered command line
        """
        try:
            cmd_line = self._multi_cmd_to_one_line(cmd)
            if None is cmd_line:
                return None
            # Determine maximum number of concurrent jobs for the build:
            # One job per CPU core or GiB of memory, whatever is lower
            num_jobs = min(os.cpu_count(), psutil.virtual_memory().total // pow(1024, 3))
            cmake_args = self._toolchain.get_cmake_args() \
                + f" -DCMAKE_BUILD_TYPE={self._args.BUILD_TYPE.value}" \
                    + f" -DCMAKE_BUILD_PARALLEL_LEVEL={num_jobs}"
            value_mapping = [
                ("SYSROOT", str(self._fsh.SYSROOT_DIR)),
                ("TOOLCHAIN_NAME", self._toolchain.profile.NAME),
                ("TARGET", self._toolchain.profile.TARGET),
                ("BUILD", self._toolchain.profile.BUILD),
                ("ARCH", self._toolchain.profile.TARGET_ARCH),
                ("PREFIX", self._toolchain.profile.INSTALL_PREFIX),
                ("LIBDIR", self._toolchain.profile.INSTALL_LIB),
                ("JOBS", str(num_jobs)),
                ("CXX", self._toolchain.profile.CXX),
                ("CC", str(self._toolchain.profile.CC)),
                ("CFLAGS", str(self._toolchain.profile.CFLAGS)),
                ("CXXFLAGS", str(self._toolchain.profile.CXXFLAGS)),
                ("CMAKE_ARGS", cmake_args),
            ]
            for l in value_mapping:
                if l[0] in cmd_line:
                    cmd_line = cmd_line.replace(f"@{l[0]}@", l[1])
            return cmd_line
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None

        
    def _do_download(self, pkg:Package):
        """
        
            Perform the step of downloading the software package source code
        Args:
            pkg: The software package to download
        """
        try:
            # If already downloaded, skip to save time
            build_state = BuildState(pkg)
            if build_state.downloaded():
                return True
            if None is pkg.URL:
                global_logger.verbose("No url configured !")
                return True
            # Download
            dwn = Downloader(pkg, self._fsh.BUILD_DIR, self._fsh.SOURCE_DIR)
            if not dwn.run():
                return False
            # Apply patches
            if None is not pkg.PATCH:
                for patch in pkg.PATCH:
                    pkg_build_dir = self._fsh.BUILD_DIR / pkg.NAME
                    cmd = f"patch -p1 -d {pkg_build_dir} -i {self._fsh.PATCH_DIR / patch}"
                    global_logger.verbose(cmd)
                    if 0 != subprocess.call(cmd, shell=True, stdout=global_logger.CHANNEL_FILE, stderr=global_logger.CHANNEL_FILE):
                        raise Exception(cmd)
            # Mark as downloaded
            build_state.downloaded(True)
            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
    

    def _do_preproc(self, pkg:Package):
        """
            Perform optional preprocessing immediately after downloading the
            software package source code.
        Args:
            pkg: Software package
        Returns:
            bool: Whether execution was successful
        """
        try:
            if None is pkg.PREPROC_CMD:
                global_logger.info(f"{pkg.NAME} NO PREPROC_CMD")
                return True

            # If already preprocessed, skip this step
            build_state = BuildState(pkg)
            if build_state.preprocessed():
                return True

            real_cmd = self._render_command(pkg.PREPROC_CMD)
            global_logger.verbose(real_cmd)
            pkg_build_dir = self._fsh.BUILD_DIR / pkg.NAME
            command_env = os.environ.copy()
            command_env.update(self._toolchain.get_environs())
            if 0 != subprocess.call(real_cmd, shell=True, env=command_env, cwd=pkg_build_dir, stderr=global_logger.CHANNEL_FILE, stdout=global_logger.CHANNEL_FILE):
                raise Exception(str(real_cmd))
            build_state.preprocessed(True)
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def _do_config(self, pkg:Package):
        """
            Perform the step of configuring the software package source code
        Args:
            pkg: Software package
        Returns:
            bool: Whether execution was successful
        """
        try:
            if None is pkg.CONFIG_CMD:
                global_logger.info(f"{pkg.NAME} NO CONFIG_CMD")
                return True
                
            # If already configured, skip this step
            build_state = BuildState(pkg)
            if build_state.configed():
                return True
            real_cmd = self._render_command(pkg.CONFIG_CMD)
            global_logger.verbose(real_cmd)
            pkg_build_dir = self._fsh.BUILD_DIR / pkg.NAME
            if 0 != subprocess.call(real_cmd, shell=True, env=self._toolchain.get_environs(), cwd=pkg_build_dir, stderr=global_logger.CHANNEL_FILE, stdout=global_logger.CHANNEL_FILE):
                raise Exception(str(real_cmd))
            build_state.configed(True)
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def _do_compile(self, pkg:Package):
        """
        
            Perform the step of building the software package
        Args:
            pkg: Software package
        Returns:
            bool: Whether execution was successful
        """
        try:
            if None is pkg.BUILD_CMD:
                global_logger.info(f"{pkg.NAME} NO BUILD_CMD")
                return True

            build_state = BuildState(pkg)
            # If already built, skip to save time
            if build_state.compiled():
                return True
            real_cmd = self._render_command(pkg.BUILD_CMD)
            global_logger.verbose(real_cmd)
            pkg_build_dir = self._fsh.BUILD_DIR / pkg.NAME
            if 0 != subprocess.call(real_cmd, shell=True, env=self._toolchain.get_environs(), cwd=pkg_build_dir, stdout=global_logger.CHANNEL_FILE, stderr=global_logger.CHANNEL_FILE):
                raise Exception(str(real_cmd))
            build_state.compiled(True)
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
    

    def _do_install(self, pkg:Package):
        """
        
            Perform the step of installing the software package
        Args:
            pkg: Software package
        Returns:
            bool: Whether execution was successful
        """
        try:
            if None is pkg.INSTALL_CMD:
                global_logger.info(f"{pkg.NAME} NO INSTALL_CMD")
                return True

            build_state = BuildState(pkg)
            # If already installed, skip to save time
            if build_state.installed():
                return True
            real_cmd = self._render_command(pkg.INSTALL_CMD)
            global_logger.verbose(real_cmd)
            pkg_build_dir = self._fsh.BUILD_DIR / pkg.NAME
            if 0 != subprocess.call(real_cmd, shell=True, env=self._toolchain.get_environs(), cwd=pkg_build_dir, stdout=global_logger.CHANNEL_FILE, stderr=global_logger.CHANNEL_FILE):
                raise Exception(str(real_cmd))
            build_state.installed(True)
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

    
    def _do_clean(self, pkg:Package):
        """
        
            Perform the step of cleaning the software package
        Args:
            pkg: Software package
        Returns:
            bool: Whether execution was successful
        """
        try:
            retain_list = [
                # Keep the build status file; needed to retrieve software build status
                BuildState._BUILD_STATE_FILE,
                # Keep the CMakeLists.txt file; needed to retrieve apall information
                "CMakeLists.txt"
            ]
            build_state = BuildState(pkg)
            # If not installed, skip
            if not build_state.installed():
                return False

            pkg_build_dir = self._fsh.BUILD_DIR / pkg.NAME
            # Get all contents of the directory
            all_items = list(pkg_build_dir.iterdir())
            for item in all_items:
                # If a retained file is encountered, skip
                if item.name in retain_list:
                    continue
                # Delete non-retained files
                if item.is_file() or item.is_symlink():
                    item.unlink()
                elif item.is_dir():
                    shutil.rmtree(item)

            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

        
    def _build_one(self, pkg:Package):
        """
        
            Build a single software package, executing download, preprocess, configure, build, install
        Args:
            pkg: Software package
        Returns:
            bool: Whether execution was successful
        """
        try:
            # Create the build directory
            pkg_build_dir = self._fsh.BUILD_DIR / pkg.NAME

            # Download source code
            #self._log.info("Build {name}:{version} Downloading".format(name=pkg.getName(), version=pkg.getVersion()))
            global_logger.info(f"Build {pkg.NAME}:{pkg.VERSION} Downloading")
            if not self._do_download(pkg):
                raise Exception("__do_download()")

            # Execute the optional preprocessing step
            global_logger.info(f"Build {pkg.NAME}:{pkg.VERSION} Preprocessing")
            if not self._do_preproc(pkg):
                raise Exception("_do_preproc()")
            
            # Execute the configure step
            #self._log.info("Build {name}:{version} Configuring".format(name=pkg.getName(), version=pkg.getVersion()))
            global_logger.info(f"Build {pkg.NAME}:{pkg.VERSION} Configuring")
            if not self._do_config(pkg):
                raise Exception("__do_config()")

            # Execute the build step
            global_logger.info(f"Build {pkg.NAME}:{pkg.VERSION} Compiling")
            if not self._do_compile(pkg):
                raise Exception("__do_compile()")

            # Execute the install step
            #self._log.info("Build {name}:{version} Installing".format(name=pkg.getName(), version=pkg.getVersion()))
            global_logger.info(f"Build {pkg.NAME}:{pkg.VERSION} Installing")
            if not self._do_install(pkg):
                raise Exception("__do_install()")

            # Execute the clean step
            # TODO: Clean functionality is currently not supported. Reasons:
            #       1. Increased cost: Cleaning apall would prevent retrieving apall information. Although apall information could be obtained during the build phase, such a "hole-punching" approach would confuse the software architecture and also require solving the problem of retrieving apall information during secondary builds, increasing costs.
            #       2. Minimal benefit: Build-time cache usage details: sysroot(2G), toolchain(xG), apall build intermediates (8G), apall dependency build intermediates (3G, could be reduced by 2G after removing boost, poco).
            #           Enabling cache cleaning would only reduce the space occupied by apall dependency intermediates, decreasing the peak disk usage by only 3G (apall dependency build intermediates), having a limited overall impact.
            #           Consider implementing this feature later after apall is optimized and its space usage is reduced.
            """
            if self._args.SPACE_LIMIT:
                global_logger.verbose(f"clean {pkg.NAME}")
                if not self._do_clean(pkg):
                    global_logger.verbose(f"failed to clean {pkg.NAME}")
            """

            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

    
    def _build_with_depends(self, pkg_descriptor:str):
        """
        
            Build the specified software package and its dependencies, performing download, preprocess, configure, build, install sequentially.
        Args:
            pkg_descriptor: Software package descriptor
        Returns:
            bool: Whether execution was successful
        """
        try:
            # Initialize stack, maximum stack length 1024
            pkg_stack = Stack(1024)
            pkg:Package = self._package_manager.find(pkg_descriptor)
            if None is pkg:
                global_logger.error(f"NO {pkg_descriptor} found !")
                return False
            # If the package has already been built successfully, return; otherwise, push onto stack
            if BuildState(pkg).installed():
                global_logger.info(f"\033[1;32mpackage {pkg.NAME} built status: Finished\033[0m")
                return True
            # Write the user-specified version into the returned pkg version to retain user information, suitable for switching branches in git
            name,version,url = split_package_descriptor(pkg_descriptor)
            if None is pkg.VERSION and None is not version:
                pkg.VERSION = version
            # If the user specified a URL, save it for the actual download
            if None is not url:
                pkg.URL = url
            pkg_stack.push(pkg)

            # Loop to build packages in the stack
            while False == pkg_stack.empty():
                pkg = pkg_stack.top()
                if self._is_all_depends_finished(pkg):
                    # If all dependencies are built, build the package and pop from stack
                    if BuildState(pkg).installed() or True == self._build_one(pkg):
                        BuildState(pkg).installed(True)
                        global_logger.info(f"\033[1;32mpackage {pkg.NAME} built status: Finished\033[0m")
                        pkg_stack.pop()
                    else:
                        raise Exception(f"build {pkg.NAME})")
                else:
                    # If there are unfinished dependencies, push the unbuilt ones onto the stack
                    for dep_name in pkg.DEPEND:
                        dep_pkg = self._package_manager.find(dep_name)
                        if None is dep_pkg:
                            global_logger.error(f"NO {name}:{version} found !")
                            return False
                        if False == BuildState(dep_pkg).installed():
                            # global_logger.info(f"\033[1;33mpackage {pkg.NAME} built status: Queuing\033[0m")
                            pkg_stack.push(dep_pkg)
                        #else:
                            #global_logger.info(f"\033[1;33mpackage {pkg.NAME} built status: Finished\033[0m")

                    #end for
                #end if
            #end while
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


def _tests():
    args = CmdLineParser().build_args
    global_fsh.set_platform_dir(args.TOOLCHAIN)
    global_logger.init(error_file=f"{global_fsh.BUILD_DIR}/error.log", out_file=f"{global_fsh.BUILD_DIR}/out.log")
    b = Builder(args)
    b.run()


if __name__ == "__main__":
    _tests()
