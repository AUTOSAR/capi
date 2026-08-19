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
# @file       elf.py
# @brief      ELF file related functionalities
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import sys, os
from pathlib import Path, PurePath
import subprocess
import re
import collections
from elftools.elf.elffile import ELFFile, ELFError
from elftools.elf.enums import ENUM_E_MACHINE
from elftools.elf.constants import SH_FLAGS

import multiprocessing
from enum import Enum
from typing import List, Set, Tuple, Dict

__SDK_UTILS_SRC_DIR = str(Path(__file__).resolve().parent.parent)
if __SDK_UTILS_SRC_DIR not in sys.path:
    sys.path.insert(0, __SDK_UTILS_SRC_DIR)

from core import Logger, global_logger
from utils import misc

__all__ = ["ELFManager", "SysrootManager"]



class ELFManager:
    """ELF processor, providing functions to get architecture type, get BUILD ID, strip, etc."""

    def __init__(self, fpath:Path):
        self._path = None
        if fpath is not None and os.path.isfile(fpath):
            self._path = Path(fpath).resolve()
        
        # Loaded information
        self._info = None


    def get_path(self) -> Path:
        """Get file path"""
        return self._path


    def is_elf(self) -> bool:
        """Determine if it is an ELF file.

        Returns:
            bool: True or False
        """
        try:
            with open(self._path, 'rb') as f:
                # Check ELF magic number
                magic = f.read(4)
                return magic == b'\x7fELF'
        except Exception as e:
            return False



    def is_excutable(self):
        """Whether it is an executable file (traditional executable or PIE)"""
        self.load_info()
        if self._info is None:
            return False
        else:
            return self._info['is_executable'] or self._info["is_pie"]


        
    def get_arch(self) -> str:
        """Get the machine architecture string

        Returns:
            str: arm/aarch64/x86/x86_64
        """
        try:
            with open(self._path, 'rb') as f:
                elffile = ELFFile(f)
                arch_name = elffile.header['e_machine']
                # machine_code = ENUM_E_MACHINE.get(arch_name, 'EM_UNKNOWN')
                if arch_name == 'EM_ARM':
                    return 'arm'
                elif arch_name == 'EM_AARCH64':
                    return 'aarch64'
                elif arch_name == 'EM_386':
                    return 'x86'
                elif arch_name == 'EM_X86_64':
                    return 'x86_64'
                else:
                    return 'unknown'
        except Exception as e:
            return None



    def get_needed_libs(self) -> List[str]:
        try:
            self.load_info()
            if self._info is not None:
                return self._info['needed_libs']
            else:
                return None
        except Exception as e:
            global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None


        
    def get_build_id(self):
        """Get Build ID using the readelf command"""

        try:
            cmd = self._get_toolchain_prefix() + "readelf"
            result = subprocess.run(
                [cmd, "-n", str(self._path)],
                capture_output=True, text=True, check=True
            )
            for line in result.stdout.split('\n'):
                if 'Build ID:' in line:
                    # Extract the Build ID value
                    parts = line.split('Build ID:')
                    if len(parts) > 1:
                        return parts[1].strip()
            return None
        except subprocess.CalledProcessError as e:
            global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e.stderr}")
            return False
        except Exception as e:
            global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None
    

    def extract_debug_info(self, debug_file):
        """Extract debug information using objcopy"""

        try:
            cmd = self._get_toolchain_prefix() + "objcopy"
            subprocess.run(
                [cmd, "--only-keep-debug", str(self._path), str(debug_file)],
                capture_output=True, text=True, check=True
            )
            return True
        except subprocess.CalledProcessError as e:
            global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e.stderr}")
            return False
        except Exception as e:
            global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

    
    def strip(self, all=False):
        """Strip the ELF file (remove debug information)"""

        try:
            exe = self._get_toolchain_prefix() + "strip"
            if all:
                cmd = f"{exe} {self._path}"
            else:
                cmd = f"{exe} --strip-debug {self._path}"
            result = subprocess.run(cmd, check=True, capture_output=True, text=True, shell=True)
            if 0 != result.returncode:
                print(result.stdout)
                return False

            return True
        
        except subprocess.CalledProcessError as e:
            global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e.stderr}")
            return False

        except Exception as e:
            global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

    
    
    def _get_toolchain_prefix(self) -> str:
        """
        Determine the architecture type to decide the toolchain prefix.
        
        Currently, only supports x86_64, aarch64, and arm architectures.
        """

        try:
            arch = self.get_arch()
            if "aarch64" == arch:
                return "aarch64-linux-gnu-"
            elif "x86_64" == arch:
                return ""
            elif "arm" == arch:
                return "gcc-arm-linux-gnueabi-"
            else:
                return None
        except Exception as e:
            print(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None



    def __load_info(self) -> bool:
        """
        Efficiently analyze the type of an ELF file
        """
        if self._path is None:
            return None

        try:
            with open(self._path, 'rb') as fp:
                elffile = ELFFile(fp)
                header = elffile.header

                # Information structure
                self._info = {
                    'file_path': self._path,
                    'elf_type': header['e_type'],
                    'entry_point': header['e_entry'],
                    'machine': header['e_machine'],
                    'osabi': header['e_ident']['EI_OSABI'],
                    'has_dynamic': False,
                    'has_interp': False,
                    'has_exec_permission': os.access(self._path, os.X_OK),
                    'needed_libs': None,
                    'is_pie': False,
                    'is_executable': False,
                    'is_shared_library': False
                }

                # Quick check: ET_EXEC is definitely an executable file
                if header['e_type'] == 'ET_EXEC':
                    self._info['is_executable'] = True
                    self._info['is_pie'] = False
                    self._info['is_shared_library'] = False

                    # Check the interpreter (optional, for completeness)
                    """ 
                    for segment in elffile.iter_segments():
                        if segment.header['p_type'] == 'PT_INTERP':
                            self._info['has_interp'] = True
                            break
                    """

                # ET_DYN needs to distinguish between PIE and shared library
                elif header['e_type'] == 'ET_DYN':
                    # Key check: does it have a program interpreter?
                    for segment in elffile.iter_segments():
                        if segment.header['p_type'] == 'PT_INTERP':
                            self._info['has_interp'] = True
                            self._info['is_pie'] = True
                            self._info['is_executable'] = True
                            self._info['is_shared_library'] = False
                            break
                    else:
                        # No PT_INTERP found, it is a shared library
                        self._info['is_shared_library'] = True

                    # Additional check: an entry point of 0 also indicates PIE
                    if self._info['entry_point'] == 0 and not self._info['has_interp']:
                        self._info['is_pie'] = True
                        self._info['is_executable'] = True
                        self._info['is_shared_library'] = False

                # Quick check of the dynamic segment
                dynamic_section = elffile.get_section_by_name('.dynamic')
                if dynamic_section:
                    self._info['has_dynamic'] = True
                    # Get DT_NEEDED entries (dependent libraries)
                    needed_libs = []
                    for tag in dynamic_section.iter_tags():
                        if tag.entry.d_tag == 'DT_NEEDED':
                            needed_libs.append(tag.needed)
                    self._info['needed_libs'] = needed_libs

                return True

        except Exception as e:
            global_logger.error(f"analyze {self._path} failed: {e}")
            return None

            
            
    def load_info(self) -> bool:
        """
        Load ELF information
        """
        if self._path is None:
            return False

        # If _info is not empty, it means it has already been loaded; return directly
        if self._info is not None:
            return True

        try:
            fp = open(self._path, 'rb')

            elffile = ELFFile(fp)
            header = elffile.header

            # 1. Initialize the information structure
            self._info = {
                'file_path': self._path,
                'elf_type': header['e_type'],
                'entry_point': header['e_entry'],
                'machine': header['e_machine'],
                'osabi': header['e_ident']['EI_OSABI'],
                'has_dynamic': False,
                'has_interp': False,
                'has_exec_permission': os.access(self._path, os.X_OK),
                'needed_libs': [],
                'is_pie': False,
                'is_executable': False,
                'is_shared_library': False
            }

            # 2. Get all program headers at once to avoid multiple traversals later
            segments = list(elffile.iter_segments())
            dynamic_segment = None
            interp_segment = None

            for segment in segments:
                p_type = segment.header['p_type']
                if p_type == 'PT_DYNAMIC':
                    dynamic_segment = segment
                elif p_type == 'PT_INTERP':
                    interp_segment = segment
                    self._info['has_interp'] = True  # Found interpreter segment

            # 3. Make judgment based on ELF type and key segments
            if header['e_type'] == 'ET_EXEC':
                self._info['is_executable'] = True
                self._info['is_shared_library'] = False
                self._info['is_pie'] = False  # Traditional non-PIE executable

            elif header['e_type'] == 'ET_DYN':
                # Key optimization: existence of PT_INTERP is a strong indicator of a PIE executable
                if interp_segment is not None:
                    self._info['is_pie'] = True
                    self._info['is_executable'] = True
                    self._info['is_shared_library'] = False
                else:
                    # No interpreter, basically can be determined as a shared library
                    self._info['is_shared_library'] = True
                    self._info['is_executable'] = False
                    # Note: Shared libraries can also be PIE, but from the perspective of "executability", we are more concerned about whether it needs a loader.
                    # Typically, we do not mark shared libraries as is_pie=True unless your analysis has special requirements.
                    self._info['is_pie'] = False

            # 4. Process the dynamic segment and dependent libraries (more efficient)
            if dynamic_segment is not None:
                self._info['has_dynamic'] = True
                try:
                    # Get dependencies directly from the PT_DYNAMIC segment
                    needed_libs = []
                    for tag in dynamic_segment.iter_tags():
                        if tag.entry.d_tag == 'DT_NEEDED':
                            needed_libs.append(tag.needed)
                    self._info['needed_libs'] = needed_libs
                except Exception as e:
                    global_logger.warning(f"Failed to parse dynamic segment for {self._path}: {e}")
                    self._info['needed_libs'] = []  # Ensure it is an empty list instead of None

            # 5. (Optional) More precise fallback check for PIE
            # If previous logic could not determine it is PIE, but the file is ET_DYN and entry point is 0, this is often a characteristic of PIE[3](@ref)
            if not self._info['is_pie'] and header['e_type'] == 'ET_DYN' and header['e_entry'] == 0:
                self._info['is_pie'] = True
                # Note: At this point, is_executable=True may not hold, because shared libraries can also have an entry point of 0.

            return True

        except ELFError as e:
            # More specifically catch ELF parsing errors
            global_logger.error(f"ELF parsing error for {self._path}: {e}")
            self._info = None
            return False
        except Exception as e:
            # Catch other exceptions (e.g., file I/O)
            global_logger.error(f"Unexpected error analyzing {self._path}: {e}")
            self._info = None
            return False
        finally:
            if fp is not None:
                fp.close()
            fp = None



    def debug(self):
        self.load_info()
        """Print analysis results"""
        print(f"File: {self._info['file_path']}")
        print(f"ELF Type: {self._info['elf_type']}")
        print(f"Is Executable: {self._info['is_executable']}")
        print(f"Is PIE: {self._info['is_pie']}")
        print(f"Is Shared Library: {self._info['is_shared_library']}")
        print(f"Entry Point: 0x{self._info['entry_point']:x}")
        print(f"Has Interpreter Segment: {self._info['has_interp']}")
        print(f"Has Execution Permission: {self._info['has_exec_permission']}")
        print(f"Dependent Libraries: {self._info['needed_libs']}")

        if self._info['is_pie']:
            print("🎯 Conclusion: This is a Position Independent Executable (PIE)")
        elif self._info['is_executable']:
            print("🎯 Conclusion: This is a traditional executable file")
        elif self._info['is_shared_library']:
            print("🎯 Conclusion: This is a shared library")
        else:
            print("🎯 Conclusion: Cannot determine file type")
        

    
def _create_build_id_link(debug_dir, build_id, debug_file):
    """Create a Build ID symbolic link"""

    if not build_id or len(build_id) < 4:
        return False
    try:
        # Build ID format: first 2 characters as directory name, remaining as file name
        build_id_dir = debug_dir / ".build-id" / build_id[:2]
        build_id_dir.mkdir(parents=True, exist_ok=True)
        link_path = build_id_dir / f"{build_id[2:]}.debug"
        # Create a relative path symbolic link
        if link_path.exists():
            link_path.unlink()
        # Calculate the relative path
        relative_path = os.path.relpath(debug_file, build_id_dir)
        link_path.symlink_to(relative_path)
        return True
    except Exception as e:
        global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
        return False



def _process_elf_file_in_sysroot(sysroot:Path, debug_dir:Path, elf_obj:ELFManager, is_strip=True) -> bool:
    """
    Extract debuginfo from an ELF file.

    Args:
        sysroot: sysroot path
        debug_dir: Path where debug information is stored
        elf_obj: ELFManager object
        is_strip: Whether to strip the file
    Returns:
        True on success;
        False on failure;
        None if ignored;
    """
    try:
        # Get Build ID
        build_id = elf_obj.get_build_id()
        if not build_id:
            global_logger.verbose(f"skip {elf_obj.get_path().relative_to(sysroot)}: no Build ID")
            return None
        # Compute the path for the debug file (preserving original directory structure)
        relative_path = elf_obj.get_path().relative_to(sysroot)
        debug_file = debug_dir / (str(relative_path) + ".debug")
        debug_file.parent.mkdir(parents=True, exist_ok=True)
        # Extract debug information
        if not elf_obj.extract_debug_info(debug_file):
            global_logger.verbose(f"can not extract debug info from {relative_path}")
            return False
        # Create Build ID index
        if not _create_build_id_link(debug_dir, build_id, debug_file):
            global_logger.verbose(f"failure on create build id link for {relative_path}")
            return False
        # Strip the original ELF file
        if is_strip:
            if not elf_obj.strip():
                global_logger.verbose(f"failure on strip {relative_path}")
                return False
        return True
    except Exception as e:
        global_logger.verbose(f"error on process {elf_obj.get_path()}: {e}")
        return False



class SysrootManager:
    """sysroot manager, providing operations such as stripping, information retrieval, debug info extraction, etc.
    """

    class StripLevel(Enum):
        """Define enumeration values for stripping sysroot."""
        FILE = "1"            # File stripping only, delete unnecessary dependent libraries
        BIN = "2"             # Binary stripping only, strip all ELF files
        ALL = "3"             # Full stripping, combination of FILE and BIN



    def __init__(self, sysroot_path:Path):
        """Initialize

        Args:
            sysroot_dir: sysroot path
        """
        self._path = Path(sysroot_path).resolve()
        # Paths of all ELF files found in the current sysroot and all their dependent library paths
        # Used for deep stripping, only keep executable ELF files and all their dependent libraries, delete everything else
        # Note: This set is reset with each strip operation, so the same SysrootManager object cannot perform parallel stripping at the same time.
        self._remained_elf_paths: Set[Path] = None
        # Stores information for all ELF files in the current sysroot, each entry is a Path:ElfManager object key-value pair.
        self._all_elf_infos:Dict[str, ELFManager] = None



    def get_arch(self):
        """Get the system architecture"""

        try:
            if not self._path.is_dir():
                return None
            for rootpath, dirs, files in os.walk(self._path):
                for fname in files:
                    if PurePath(fname).match("lib*.so"):
                        elf = ELFManager(os.path.join(rootpath, fname))
                        return elf.get_arch()
            return None
        except Exception as e:
            global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None 

            
    
    def strip(self, level:StripLevel=None, rm_dirs:List[str]=None, rm_files:List[str]=None) -> bool:
        """Strip sysroot to reduce space usage. Includes two aspects of stripping:
            1. ELF file stripping, including three levels:
                1. Binary stripping: only strip all ELF files.
                2. File count stripping: only keep all executable programs and their necessary dependent libraries, delete all other ELFs.
                3. Full stripping: combination of 1 and 2.
            2. Delete specified files and directories, supporting wildcards.

        Args:
            level (StripLevel, optional): Strip level
            files (List[str], optional): List of files to delete (including symlinks), supports wildcards
            dirs (List[str], optional): List of directories to delete, supports wildcards

        Returns:
            bool: _description_
        """
        try:
            global_logger.info("removing files ...")
            misc.remove_files(self._path, files=rm_files, dirs=rm_dirs)
            global_logger.info("removing files ... Done")

            if level is None:
                return True

            global_logger.info("Loading ELF Files ...")
            if not self._load_all_elf_info():
                global_logger.error("self.load_all_elf_info()")
                return False
            global_logger.info("Loading ELF Files ... Done")

            if level in (SysrootManager.StripLevel.ALL, SysrootManager.StripLevel.FILE):
                global_logger.info("Removing unused ELF Files ...")
                self._remove_unused_elf_files()
                global_logger.info("Removing unused ELF Files ... Done")

            if level in (SysrootManager.StripLevel.ALL, SysrootManager.StripLevel.BIN):
                global_logger.info("Striping ELF Files ...")
                for elf_name, elf_obj in self._all_elf_infos.items():
                    if elf_obj.get_path().exists():
                        elf_obj.strip(all=True)
                        global_logger.verbose(f"{elf_obj.get_path().relative_to(self._path)} stripped")
                global_logger.info("Striping ELF Files ... Done")

            return True

        except Exception as e:
            global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False



    def _load_all_elf_info(self) -> bool:
        """Load information for all ELF files

        Returns:
            bool: True of False
        """

        # If not empty, it means already loaded
        if self._all_elf_infos is not None:
            return True

        self._all_elf_infos = {}

        try:
            # Supported file extension patterns
            elf_patterns = [
                "**/bin/*",         # Executable files
                "**/sbin/*",        # Executable files
                "**/lib/*.so",      # Dynamic link libraries
                "**/lib/*.so.*",    # Dynamic link libraries
            ]
            for pattern in elf_patterns:
                for elf_file in self._path.rglob(pattern):
                    # Filter out symbolic links and non-regular files
                    if elf_file.is_symlink() or not elf_file.is_file():
                        continue

                    elf_obj = ELFManager(elf_file)
                    # Filter out non-ELF files
                    if not elf_obj.is_elf():
                        continue
                    if elf_obj.load_info():
                        self._all_elf_infos[elf_file.name] = elf_obj

            return True

        except Exception as e:
            global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False



    def _remove_unused_elf_files(self) -> bool:
        """Search for all executable files, retain their dependent libraries and directory structure, delete all other ELF files.

        Returns:
            bool: Returns True on success, False on failure
        """
        try:
            # Reset this set with each strip operation
            self._remained_elf_paths = set()

            # ========== 1. Find all ELF files and executable files ===============
            for elf_name, elf_obj in self._all_elf_infos.items():
                if elf_obj.is_excutable():
                    self._remained_elf_paths.add(elf_obj.get_path())

            global_logger.info(f"Found {len(self._remained_elf_paths)} executable files")
            global_logger.info(f"parsing executable file dependencies ...")

            # ============ 2. Find all dependent libraries of executable files ================
            executable_list = list(self._remained_elf_paths)
            executable_count = len(executable_list)
            index = 0
            for exec_file in executable_list:
                index += 1
                global_logger.verbose(f"Processing executable [{index}/{executable_count}]: {exec_file.relative_to(self._path)}")
                # Get the dependent libraries of the executable file
                dep_list = self._get_elf_dependencies(exec_file)
                self._remained_elf_paths.update(dep_list)
                # Debug, print all dependent libraries
                global_logger.verbose("Dependencies:")
                for dep in dep_list:
                    global_logger.verbose(dep.relative_to(self._path))

            global_logger.info(f"Total files to keep: {len(self._remained_elf_paths)}")

            # ============= 3. Delete unnecessary ELF files ==============
            deleted_count = 0
            error_count = 0

            # Directly calculate the files to delete: all ELF files not in the retention list
            for elf_name, elf_obj in self._all_elf_infos.items():
                elf_path = elf_obj.get_path()

                # If the file is in the retention list, skip it
                if elf_path in self._remained_elf_paths:
                    continue

                # Delete unnecessary ELF files
                try:
                    relative_path = elf_path.relative_to(self._path)
                    # Check if the file exists
                    if elf_path.exists():
                        elf_path.unlink()  # Directly delete the file
                        deleted_count += 1
                        global_logger.verbose(f"Deleted unused ELF file: {relative_path}")

                except Exception as e:
                    error_count += 1
                    global_logger.error(f"Failed to delete {elf_path}: {e}")

            global_logger.info(f"Deleted {deleted_count} unused ELF files, {error_count} errors")

            # ============= 4. Delete empty directories and broken symbolic link files ==============
            removed_dirs_count = 0
            removed_links_count = 0
            # Traverse directories from the bottom up
            for root, dirs, files in os.walk(self._path, topdown=False):
                current_dir = Path(root)
                # Skip the root directory
                if current_dir == self._path:
                    continue

                # Delete broken symbolic link files
                for file in files:
                    fpath = current_dir / file
                    if fpath.is_symlink() and not fpath.exists():
                        fpath.unlink()
                        removed_links_count += 1
                        global_logger.verbose(f"Deleted broken symlink: {fpath.relative_to(self._path)}")

                # Delete empty directories
                try:
                    if not any(current_dir.iterdir()):  # Directory is empty
                        current_dir.rmdir()
                        removed_dirs_count += 1
                        global_logger.verbose(f"Removed empty directory: {current_dir.relative_to(self._path)}")
                except OSError:
                    # Directory is not empty or there are permission issues, skip
                    pass

            global_logger.info(f"Removed {removed_dirs_count} empty directories")
            global_logger.info(f"Removed {removed_links_count} broken symlink files")

            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}: {e}")
            return False



    def _get_elf_dependencies(self, elf_path: Path) -> Set[Path]:
        """Get all dependent libraries of an ELF file (recursive resolution)

        Args:
            elf_path: Path to the ELF file

        Returns:
            Set[Path]: Set of all dependent library paths
        """
        try:
            if not elf_path.exists():
                return set()

            # Store all dependent library paths
            all_deps = set()
            # Track already processed libraries (to avoid circular dependencies)
            processed = set()
            # Pending processing queue
            queue = collections.deque([elf_path])

            while queue:
                current_elf = queue.popleft()
                # TODO: Should use absolute() here, otherwise linked files might be ignored.
                current_real_path = current_elf.resolve()

                # Avoid duplicate processing
                if current_real_path in processed:
                    continue
                processed.add(current_real_path)

                # Get the dependent libraries of the current ELF
                elf_obj = self._all_elf_infos.get(current_real_path.name)
                if not elf_obj:
                    continue

                needed_libs = elf_obj.get_needed_libs()
                if not needed_libs:
                    continue
                
                # Process each dependent library
                for lib_name in needed_libs:
                    # Find the library file
                    lib_files = self._find_library_associated_files(lib_name)
                    if not lib_files:
                        continue

                    # Choose the first found library file (by priority)
                    lib_file = next(iter(lib_files))
                    lib_real_path = lib_file.resolve()

                    # Add to the result set and pending queue
                    if lib_real_path.exists() and lib_real_path not in processed:
                        all_deps.add(lib_real_path)
                        queue.append(lib_real_path)

            return all_deps
        
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}: {e}")
            return all_deps



    def _get_link_name(self, library_name) -> str:
        """
        Get the link_name based on the provided dynamic library file name
    
        Parameters:
            library_name: Dynamic library file name, which can be realname, so_name, or link_name

        Returns:
            link_name: Development link name (e.g., libname.so)
        """
    
        # Dynamic library naming pattern matching
        # realname pattern: libname.so.x.y.z
        realname_pattern = r'^(.*?)\.so\.\d+(?:\.\d+)*$'
        # so_name pattern: libname.so.x
        soname_pattern = r'^(.*?)\.so\.\d+$'
        # link_name pattern: libname.so
        linkname_pattern = r'^(.*?)\.so$'
    
        # Try to match realname (libname.so.x.y.z)
        realname_match = re.match(realname_pattern, library_name)
        if realname_match:
            base_name = realname_match.group(1)
            return f"{base_name}.so"
    
        # Try to match so_name (libname.so.x)
        soname_match = re.match(soname_pattern, library_name)
        if soname_match:
            base_name = soname_match.group(1)
            return f"{base_name}.so"
    
        # Try to match link_name (libname.so)
        linkname_match = re.match(linkname_pattern, library_name)
        if linkname_match:
            # Already in link_name format, return directly
            return library_name
    
        # If none match, check if it contains .so but the format is incorrect
        if '.so' in library_name:
            # Extract the part before .so as the base name
            base_part = library_name.split('.so')[0]
            return f"{base_part}.so"
        else:
            # If .so is not included, it might be an abbreviated form; directly add .so
            return f"{library_name}.so"



    def _find_library_associated_files(self, lib_name:str) -> Set[Path]:
        """Get all file paths related to libname, typically various symbolic link files

        Args:
            lib_name (str): Library file name

        Returns:
            List[Path]: List of all file paths related to the library file
        """
        try:
            # Calculate link_name to match all related lib names, because link_name is the shortest
            link_name = self._get_link_name(lib_name)
            lib_set = set()
            elf_objs:List[ELFManager] = [elf_obj for elf_name, elf_obj in self._all_elf_infos.items() if elf_name.startswith(link_name)]
            if not elf_objs:
                return None
            elf_dir = elf_objs[0].get_path().parent
            matched_files = list(elf_dir.rglob(f"{link_name}*"))
            for lib_path in matched_files:
                if lib_path.is_file() or lib_path.is_symlink():
                    lib_set.add(lib_path)
            
            lib_list = list(lib_set)
            for lib_path in lib_list:
                # The purpose is to match cases where the base names of so_name, real_name, and link_name are completely unrelated. For example, for a library like ld.so, where so_name and its symlink name are completely different, we need to add the actual file to the list as well.
                lib_path:Path
                lib_real_path = lib_path.resolve()
                if lib_real_path and lib_real_path.exists():
                    lib_set.add(lib_real_path)
            return lib_set

        except Exception as e:
            global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}: {e}")
            return None



    def extract_debug_info(self, output_dir: Path, is_strip=True) -> Tuple[int, int, int]:
        """
        Extract debugging information from all ELF files in the sysroot and store it in the specified directory.

        Provides both traditional file directory form and modern Build ID index form.
        Uses multi-process parallel processing to fully utilize multi-core CPU performance.
        Args:
            output_dir: Output directory for debug information files
            is_strip: Whether to strip the file after extraction, defaults to True
        Returns:
            On success, returns a tuple (number of successful files, number of failed files, number of ignored files);
            On failure, returns None
        """
        try:
            # Create the debug directory
            debug_dir = Path(output_dir)
            debug_dir.mkdir(parents=True, exist_ok=True)
            (debug_dir / ".build-id").mkdir(parents=True, exist_ok=True)

            # Get all ELF files
            if not self._load_all_elf_info():
                return False

            total_files = len(self._all_elf_infos)

            if total_files == 0:
                return (0, 0, 0)

            success_files = 0
            failure_files = 0
            skip_files = 0

            # Dynamically calculate the number of processes
            cpu_count = os.cpu_count()
            max_workers = min(cpu_count, total_files)
            global_logger.verbose(f"using {max_workers} handle {total_files} ELF file")

            # Use multiprocessing.Pool instead of ProcessPoolExecutor
            # This method is more stable in Python 3.8; the latter always reports an exception (Error in atexit._run_exitfuncs:OSError: handle is closed) when the main process exits.
            with multiprocessing.Pool(processes=max_workers) as pool:
                # Prepare arguments
                args_list = [(self._path, debug_dir, elf_obj, is_strip) for elf_name, elf_obj in self._all_elf_infos.items()]
                # Use starmap for functions with multiple arguments
                results = pool.starmap(_process_elf_file_in_sysroot, args_list)
                # Count results
                for ret in results:
                    if ret is None:
                        skip_files += 1
                    elif ret is True:
                        success_files += 1
                    else:
                        failure_files += 1

            return (success_files, failure_files, skip_files)

        except Exception as e:
            global_logger.verbose(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}: {e}")
            return None



def _test_SysrootManager():
    """
    import argparse
    parser = argparse.ArgumentParser(
        description="Scan ELF files in the sysroot directory and generate debug files with Build ID indexing"
    )
    parser.add_argument("sysroot", help="Path to the sysroot directory")
    parser.add_argument("debug_dir", help="Output directory for debug files")
    args = parser.parse_args()
    
    # Create a processor
    global_logger.init()
    e = SysrootManager(args.sysroot)
    (p, f, s) = e.extract_debug_info(args.debug_dir, is_strip=False)
    print(f"\n🎯 Processing completed!")
    print(f"✅ Successfully processed: {p}")
    print(f"⚠️ Skipped files: {s}")
    print(f"❌ Error files: {f}")
    """
    e = SysrootManager(sys.argv[1])
    e._load_all_elf_info()
    for elf in e._all_elf_infos:
        elf_obj = e._all_elf_infos[elf]
        print(elf_obj._path)
        #elf_obj.debug()
    print(len(e._all_elf_infos))



def _test_ELFManager():
    ep = ELFManager(sys.argv[1])
    ep.debug()
    print(ep.get_arch())
    print(ep.is_excutable())

if __name__ == "__main__":
     #_test_ELFManager()
    _test_SysrootManager()