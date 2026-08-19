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
# @file       misc.py
# @brief      General utilities
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import sys, os
import shutil
import tarfile
from pathlib import Path, PurePath
import subprocess
from typing import Dict, List, Set

__SDK_UTILS_SRC_DIR = str(Path(__file__).resolve().parent.parent)
if __SDK_UTILS_SRC_DIR not in sys.path:
    sys.path.insert(0, __SDK_UTILS_SRC_DIR)

# from core.decorator import handle_exception, handle_func_exception
from core.log import global_logger



__all__ = [
    "render_template_file", 
    "compress_file",
    "uncompress_file",
    "remove_files",
    "get_os_version",
    "is_tarfile",
    "get_tarfile_top_dir",
    "is_shell_script",
    "get_disk_usage",
    "get_pkg_ext"
]


    
def get_os_version():
    """Get OS name and version number"""
    try:
        osv_fpath = "/etc/os-release"
        if not os.access(osv_fpath, os.F_OK):
            return "Unkown"
        lines = []
        with open(osv_fpath, "r") as f:
            lines = f.readlines()
        os_name = ""
        os_version = ""
        for line in lines:
            kv = line.strip().split("=")
            if 2 > len(kv):
                continue
            if "ID" == kv[0]:
                os_name = kv[1].strip().replace('"','')
            elif "VERSION_ID" == kv[0]:
                os_version = kv[1].strip().replace('"','')
        return (os_name + os_version)
    except Exception as e:
        global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
        return None



def is_shell_script(file_path) ->bool:
    """
    Comprehensive determination of whether a file is a shell script.
    Returns: True or False
    """
    # 1. Basic check: does the file exist?
    if not os.path.isfile(file_path):
        return False
    """
    # 2. Check file extension (quick initial screening)
    filename = os.path.basename(file_path)
    # Common shell script extensions, can be supplemented as needed
    shell_extensions = ('.sh', '.bash', '.zsh')
    if not filename.endswith(shell_extensions):
        return False
    """
    # 3. Check file content - Shebang (the most critical check)
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            first_line = f.readline().strip()
    except (IOError, UnicodeDecodeError):
        return False

    # Determine if the Shebang line points to a shell interpreter
    shell_indicators = ['#!/bin/bash', '#!/bin/sh', '#!/usr/bin/env bash', '#!/usr/bin/env sh']
    # More general check: starts with #! and the line contains 'sh' or 'bash'
    if first_line.startswith('#!'):
        if any(indicator in first_line for indicator in shell_indicators):
            return True
    return False



def is_tarfile(file_path) -> bool:
    """
    Determine if a file is a tar package
    """
    try:
        with tarfile.open(file_path, 'r:*') as tar:
            return True
    except Exception:
        return False



def get_tarfile_top_dir(tar_file_path) -> str:
    """
    Check if a tar package contains a single top-level directory.
    
        Get the name of the top-level directory in a tar or tar.gz file, can be used to determine if the tar package contains a top-level directory.
    Args:
        tar_file_path: Path to the tar file
    Returns:
        If it contains a top-level directory, returns the directory name;
        If it does not contain a top-level directory, returns None
    """
    try:
        with tarfile.open(tar_file_path, 'r:*') as tar:
            members = tar.getmembers()
            if not members:
                return None
            
            top_level_names = set()
            for member in members:
                name = member.name.lstrip('/')
                parts = name.split('/')
                if parts and parts[0]:  # Ensure the first part is non-empty
                    top_level_names.add(parts[0])
            
            if len(top_level_names) == 1:
                top_dir = top_level_names.pop()
                # Additional check: if this top-level directory name itself corresponds to a directory entry (not just a path prefix), it's more certain
                if any(member.isdir() and member.name.rstrip('/') == top_dir for member in members):
                    return top_dir
                else:
                    return None
            else:
                return None
                
    except Exception  as e:
        global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
        return None

        

def remove_files(top_dir:Path, files:List[str], dirs:List[str]) -> bool:
    """Delete specified directories and files from the given directory.

    Args:
        top_dir (Path): The directory to process
        files (List[str]): List of files to delete (including symlinks), supports wildcards
        dirs (List[str]): List of directories to delete, supports wildcards

    Returns:
        bool: True or False
    """
    try:
        # Protect against the possibility of a string being passed in
        top_dir = Path(top_dir)
        # Check if the directory exists
        if not top_dir.exists():
            global_logger.verbose(f"error: dir {top_dir} no exists")
            return False
        if not top_dir.is_dir():
            global_logger.verbose(f"error：{top_dir} is not dir")
            return False

        # Collect all paths to delete
        paths_to_delete:Set[Path] = set()

        # Process file pattern matching
        for file_pattern in files:
            try:
                # Recursively match all files meeting the criteria (including symbolic links)
                matched_files = list(top_dir.rglob(file_pattern))
                for file_path in matched_files:
                    if file_path.is_file() or file_path.is_symlink():
                        paths_to_delete.add(file_path)
            except Exception as e:
                global_logger.verbose(f"file matched error '{file_pattern}': {e}")
                continue

        # Process directory pattern matching
        for dir_pattern in dirs:
            try:
                # Recursively match all directories meeting the criteria
                matched_dirs = list(top_dir.rglob(dir_pattern))
                for dir_path in matched_dirs:
                    if dir_path.is_dir():
                        paths_to_delete.add(dir_path)
            except Exception as e:
                global_logger.verbose(f"dir matched error '{dir_pattern}': {e}")
                continue

        if not paths_to_delete:
            global_logger.verbose("none file or dir matched.")
            return True


        # Perform deletion
        success_count = 0
        total_count = len(paths_to_delete)
        global_logger.verbose(f"removing files from {top_dir} ... ")

        for path in paths_to_delete:
            if path.is_symlink() or path.is_file():
                # Delete file or symbolic link
                path.unlink()
                global_logger.verbose(f"rmfile: {path.relative_to(top_dir)}")
                success_count += 1
                
            elif path.is_dir():
                # Delete directory
                shutil.rmtree(path, ignore_errors=True)
                global_logger.verbose(f"rmdir : {path.relative_to(top_dir)}")
                success_count += 1
                
        return success_count == total_count

    except Exception  as e:
        global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
        return False



def uncompress_file(src_file:Path, dest_dir:Path, strip_topdir=False) -> bool:
    """
    
        Universal decompression, automatically determine the archive type and extract to the specified directory
    Args:
        src_file: Path to the source archive file
        dest_dir: Path where the extraction will be stored
        strip_topdir: If strip_topdir is True, extraction will not include the top-level directory; only effective when src contains a directory.
    Returns:
        True on success;
        False on failure
    """
    try:
        if is_tarfile(src_file):
            strip_args = ""
            if strip_topdir:
                strip_args = "--strip-components=1"
            cmd = f"tar xf {src_file} -C {dest_dir} {strip_args}"
        else:
            #TODO: Implement more archive types
            cmd = ""
        if 0 != subprocess.call(cmd, shell=True, stderr=subprocess.DEVNULL, stdout=subprocess.DEVNULL):
            raise Exception(cmd)
        return True
    except Exception as e:
        global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
        return False


    
def compress_file(src:Path, dst:Path, strip_topdir=False) -> int:
    """
    
        Compress a file or directory
    Args:
        src: Path to the source file
        dst: Path to the target file
        strip_topdir: If strip_topdir is True, packaging will not include the top-level directory; only effective when src is a directory.
    Returns:
        int: Number of bytes of the compressed file; returns -1 on error.
    """
    try:
        src_path = Path(src)
        dst_path = Path(dst)
        if src_path.is_dir():
            # If the source is a directory, use tar to package the directory into a single file
            tarball_path = src_path.with_suffix(src_path.suffix + ".tar")
            if strip_topdir:
                # Remove the top-level directory
                cmd = ["tar", "cf", tarball_path, "-C", src_path, "."]
            else:
                # Include the top-level directory
                cmd = ["tar", "cf", tarball_path, src_path]
            if 0 != subprocess.call(cmd):
                raise Exception(str(cmd))
            # Compress
            cmd = ["pigz", tarball_path]
            if 0 != subprocess.call(cmd):
                raise Exception(str(cmd))
            # pigz modifies the file name, generating a new name
            gzip_path = tarball_path.with_suffix(tarball_path.suffix + ".gz")
            # Move the generated archive to the new path
            shutil.move(gzip_path, dst_path)
        elif src_path.is_file():
            # Compress directly, preserving the source file
            cmd = ["pigz", "-k", src_path]
            if 0 != subprocess.call(cmd):
                raise Exception(str(cmd))
            # pigz modifies the file name, generating a new name
            gzip_path = src.with_suffix(src_path.suffix + ".gz")
            # Move the generated archive to the new path
            shutil.move(gzip_path, dst_path)
        else:
            raise Exception("invalid file type")
        # Calculate file size
        return os.path.getsize(dst_path)
        
    except Exception as e:
        global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
        return None


    
def render_template_file(file_path:Path, value_mapping:Dict) -> bool:
    """
    
        Render a template file, replacing all placeholders in the template with actual values
    Args:
        file_path: Path to the file
        value_mapping: Key-value pairs
    Returns:
        bool: Success or failure
    """
    try:
        # Read the file content
        with open(file_path, "rt") as src_file:
            src_lines = src_file.readlines()
        # Replace placeholders with actual values
        dst_lines = []
        for line in src_lines:
            # TODO: Locate @TEMPLATE_PLACEHOLDERS_BEGIN@ and start replacement only after that
            # End marker; no more placeholders to replace after this
            if "@TEMPLATE_PLACEHOLDERS_END@" in line:
                break
            # Replace placeholders
            if "@" in line:
                for vp in value_mapping:
                    if vp[0] in line:
                        line = line.replace(f"@{vp[0]}@", vp[1])
            dst_lines.append(line)
        dst_lines += src_lines[len(dst_lines):]

        # Write the modified content back to the original file
        with open(file_path, "wt") as dest_file:
            # Modified template configuration
            dest_file.writelines(dst_lines)
        return True
    except Exception as e:
        global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
        return False


def get_disk_usage(path):
    """Get the disk space occupied by the data at the specified path (in MB)"""
    try:
        cmd = ["du", "-sm", path]
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
        return proc.stdout.readline().decode().strip().split()[0]
    except Exception as e:
        global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
        return 0

def get_pkg_ext(filename: str) -> str:
    """Get package extension"""
    exts = ["tar.gz", "tar.bz2", "tar.xz", "zip", "7z", "gz", "bz2", "xz"]
    for ext in sorted(exts, key=len, reverse=True): # 优先匹配长后缀
        if filename.endswith(ext):
            return ext
    return ""

def _tests():
    #compress_file(sys.argv[1], sys.argv[2])
    #topdir = get_tarfile_top_dir(sys.argv[1])
    #is_tarfile(sys.argv[1])
    print(is_shell_script(sys.argv[1]))


if __name__ == "__main__":
    _tests()