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
# @file       software_package.py
# @brief      Software package management
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import sys, os, json
from pathlib import Path
from typing import List, Dict, Union
from dataclasses import dataclass, asdict

__SDK_UTILS_SRC_DIR = str(Path(__file__).resolve().parent.parent)
if __SDK_UTILS_SRC_DIR not in sys.path:
    sys.path.insert(0, __SDK_UTILS_SRC_DIR)

from core import global_fsh
from core import global_logger
#from core.decorator import handle_exception, handle_func_exception

__all__ = ["PackageManager", "Package", "split_package_descriptor"]


def split_package_descriptor(name_descriptor:str):
    """
    
        Parse software package descriptor
        Format of software package descriptor passed via command line: name:version#url
    Args:
        name_descriptor: Software package descriptor
    """
    try:
        name = None
        version = None
        url = None
        # Format of software package name passed via command line: name:version#url
        if name_descriptor.count("#") > 1 or name_descriptor.count(":") > 1:
            raise Exception(f"invalid package name: {name_descriptor}")

        if '#' in name_descriptor:
            name_version, url = name_descriptor.split('#')
        else:
            name_version = name_descriptor
        if ':' in name_version:
            name, version = name_version.split(":")
        else:
            name = name_version
        return (name, version, url)

    except Exception as e:
        global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
        return (None, None, None)


@dataclass(frozen=False)
class Package:
    """
    Software package information class
    """
    NAME: str = None              # Software package name
    VERSION: str = None           # Software package version
    URL: str = None               # Download URL
    PATCH: str = None             # Patch file name
    DEPEND: List[str] = None      # List of software package dependencies
    PREPROC_CMD: List[str] = None # List of preprocessing commands
    CONFIG_CMD: List[str] = None  # List of configuration commands
    BUILD_CMD: List[str] = None   # List of build commands
    INSTALL_CMD: List[str] = None # List of installation commands


    def __init__(self, config_path:Path):
        """
        Load information from configuration file
        """
        with open(config_path, 'r') as f:
            self._data = json.load(f)
        # print(self._data)
        self.NAME = self._data["name"]
        if "version" in self._data:
            self.VERSION = self._data["version"]
        if "url" in self._data:
            self.URL = self._data["url"]
        # preprocess
        if "preproc" in self._data:
            self.PREPROC_CMD = self._data["preproc"]
        # configure
        if "configure" in self._data:
            self.CONFIG_CMD = self._data["configure"]
        # build
        if "build" in self._data:
            self.BUILD_CMD = self._data["build"]
        # install
        if "install" in self._data:
            self.INSTALL_CMD = self._data["install"]
        # depend
        if "depend" in self._data:
            depend = self._data["depend"]
            self.DEPEND = depend if isinstance(depend, list) else [depend]
        # patch
        if "patch" in self._data:
            self.PATCH = self._data["patch"]

            
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

        
        

class PackageManager:
    """
    Package container, used to manage all software packages
    """
    def __init__(self):
        # List of software packages, will be implemented with treelib later
        self._package_list: List[Package] = None
        

    def load(self):
        """
        Load all software package information
        """
        try:
            self._package_list = []
            for root, dirs, files in os.walk(global_fsh.PACKAGE_DIR):
                for fname in files:
                    if "json" != fname.split(".")[-1]:
                        continue
                    pkg = Package(os.path.join(root, fname))
                    self._package_list.append(pkg)
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

    
    def add(self, config_path):
        """Add a software package; if a package with the same version already exists, update its content"""
        try:
            new_pkg = Package(config_path)
            old_pkg = self.find(f"{new_pkg.NAME}:{new_pkg.VERSION}")
            if None is old_pkg:
                self._package_list.append(new_pkg)
            else:
                # If a package with the same version is found, update the content
                if None is not new_pkg.DEPEND:
                    old_pkg.DEPEND = new_pkg.DEPEND
                if None is not new_pkg.BUILD_CMD:
                    old_pkg.BUILD_CMD = new_pkg.BUILD_CMD
                if None is not new_pkg.PREPROC_CMD:
                    old_pkg.PREPROC_CMD = new_pkg.PREPROC_CMD
                if None is not new_pkg.CONFIG_CMD:
                    old_pkg.CONFIG_CMD = new_pkg.CONFIG_CMD
                if None is not new_pkg.INSTALL_CMD:
                    old_pkg.INSTALL_CMD = new_pkg.INSTALL_CMD
                if None is not new_pkg.URL:
                    old_pkg.URL = new_pkg.URL
                if None is not new_pkg.PATCH:
                    old_pkg.PATCH = new_pkg.PATCH
            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def export(self, package_name:str=None) -> Union[Dict, List[Dict]]:
        try:
            if None is self._package_list:
                if not self.load():
                    return None

            # Export individual software package information
            if None is not package_name:
                pack = self.find(package_name)
                if None is pack:
                    return None
                return pack.to_dict()

            # Export information for all software packages
            all = []
            for pack in self._package_list:
                all.append(pack.to_dict())
            return all

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None


    def traversal(self):
        if None is self._package_list:
            return False
        for pkg in self._package_list:
            print(pkg)
    

    def find(self, name_descriptor: str) -> Package:
        """
        
            Find the software package object by name and version
            If the configuration file has an explicit software version, find the package according to the version specified by the command line parameter.
            If the configuration file does not have an explicit software version, find the software by name only, and set the version number specified by the command line as the software package's version. This feature is suitable for git sources, allowing selection of the branch to build via the command line.
        Args:
            name_descriptor: String consisting of package name and version, format 'name:version#url'
        Returns:
            Pacakge: Found software package reference
        """
        try:
            if None is self._package_list:
                return None

            name, version, url = split_package_descriptor(name_descriptor)
            # First, put all packages found by name into a list
            found_pkg_list = []
            default_version_pkg = None
            for pkg in self._package_list:
                #pkg.debug()
                if name == pkg.NAME:
                    found_pkg_list.append(pkg)
            # Return None if list is empty
            if 0 == len(found_pkg_list):
                return None

            # Match version among related software packages
            for pkg in found_pkg_list:
                if version == pkg.VERSION:
                    return pkg
                if None is pkg.VERSION:
                    default_version_pkg = pkg

            # If there is a default software package configuration, return that default configuration
            if None is not default_version_pkg:
                return default_version_pkg
            
            # If no specific version is specified for search, return the first configuration
            if version in [None, "None", ""]:
                return found_pkg_list[0]
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None
        
        

def _tests():
    p = Package(global_fsh.PACKAGE_DIR / "zlib.json")
    print(p)
    c = PackageManager()
    c.load()
    all = c.export()
    print(all)


if __name__ == "__main__":
    _tests()
