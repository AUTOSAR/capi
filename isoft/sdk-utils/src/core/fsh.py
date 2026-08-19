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
# @file       fsh.py
# @brief      Build project file directory structure
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

__all__ = ["Fsh", "global_fsh"]

import os, copy
from pathlib import Path
#from typing import List, Optional, Dict, Any
from dataclasses import dataclass, field
import json
    
"""
WORK_DIR directory structure:
var
├── j5
│   ├── build
│   ├── sysroot
│   └── toolchain
└── s32g
    ├── build
    ├── sysroot
    └── toolchain
"""


@dataclass(init=False)
class Fsh:
    """
    Define standard attribute interface for file directory structure
    """
    SDK_UTILS_ROOT_DIR: Path = None             # Software top-level directory
    ETC_DIR: Path = None                        # Software configuration directory
    WORK_DIR: Path = None                       # Software working directory
    OUTPUT_DIR: Path = None                     # SDK artifact output directory, where the generated SDK is stored
    PLATFORM_DIR: str = None                    # Platform directory, i.e., output directory for build artifacts, determines the location of BUILD_DIR/TOOLCHAIN_DIR/SYSROOT_DIR. Generally shares the same name as the build toolchain.
    TOOLCHAIN_DIR: Path = None                  # Compiler installation directory
    SYSROOT_DIR: Path = None                    # Build system root directory
    BUILD_DIR: Path = None                      # Build temporary file directory
    SOURCE_DIR: Path = None                     # Source code file directory
    PACKAGE_DIR: Path = None                    # Software package configuration directory
    PATCH_DIR: Path = None                      # Software package patch directory
    TEMPLATE_DIR: Path = None                   # Template directory used internally by the software
    RELEASE_FILE_NAME:str = ".release.json"     # File recording SDK release information
    DEBUG: bool = False                         # Whether to enable debug mode

    def __init__(self):
        try:
            # Initialize default values
            # The following are fixed configurations that cannot be modified
            self.SDK_UTILS_ROOT_DIR = Path(__file__).resolve().parent.parent.parent
            self.ETC_DIR = self.SDK_UTILS_ROOT_DIR / "etc"
            self.PACKAGE_DIR = self.ETC_DIR / "package"
            self.PATCH_DIR = self.PACKAGE_DIR / "patch"
            self.TEMPLATE_DIR = self.SDK_UTILS_ROOT_DIR / "src" / "template"
            self.WORK_DIR = self.SDK_UTILS_ROOT_DIR / "var"         # Working directory, stores various generated artifacts
            self.SOURCE_DIR = self.WORK_DIR / ".source"             # Downloaded source code cache directory
            self.OUTPUT_DIR = self.WORK_DIR / "output"              # SDK output directory

            # The following are dynamically modifiable variables
            self.PLATFORM_DIR = self.WORK_DIR / "native"            # Platform directory, stores artifacts related to specific toolchain builds
            self.TOOLCHAIN_DIR = self.PLATFORM_DIR / "toolchain"    # Toolchain directory, stores specific toolchains
            self.SYSROOT_DIR = self.PLATFORM_DIR / "ara-sysroot"    # Sysroot directory, installation path for software packages
            self.BUILD_DIR = self.PLATFORM_DIR / ".build"           # Build temporary directory, stores temporary files

        except Exception as e:
            return
        

    def init(self, platform_dir:Path=None, output_dir:Path=None):
        """Initialize

        Args:
            paltform_dir (str): Platform directory
        """
        try:
            if not self._load_config():
                return False

            if platform_dir is not None:
                platform_str = str(platform_dir).strip()
                # If it is not a path, place it under WORK_DIR
                if '.' != platform_str[0] and '/' != platform_str[0]:
                    self.PLATFORM_DIR = self.WORK_DIR / platform_str
                # If it is a path, assign directly
                else:
                    self.PLATFORM_DIR = Path(platform_str)

                self.TOOLCHAIN_DIR = self.PLATFORM_DIR / "toolchain"    # Toolchain directory, stores specific toolchains
                self.SYSROOT_DIR = self.PLATFORM_DIR / "ara-sysroot"    # Sysroot directory, installation path for software packages
                self.BUILD_DIR = self.PLATFORM_DIR / ".build"           # Build temporary directory, stores temporary files
            
            if output_dir is not None:
                self.OUTPUT_DIR = Path(output_dir)

            os.makedirs(self.SOURCE_DIR, mode=0o755, exist_ok=True)
            os.makedirs(self.OUTPUT_DIR, mode=0o755, exist_ok=True)
            os.makedirs(self.PLATFORM_DIR, mode=0o755, exist_ok=True)
            os.makedirs(self.BUILD_DIR, mode=0o755, exist_ok=True)
            os.makedirs(self.SYSROOT_DIR, mode=0o755, exist_ok=True)
        except Exception as e:
            return False


    def copy(self):
        return copy.deepcopy(self)

    def _load_config(self):
        try:
            config_file = self.ETC_DIR / "config.json"
            config_data = {}
            if config_file.exists():
                with open(config_file, 'r') as f:
                    config_data = json.load(f)
            if "work_path" in config_data and config_data["work_path"] is not None:
                self.WORK_DIR = Path(config_data["work_path"]).expanduser()
            if "source_path" in config_data and config_data["source_path"] is not None:
                self.SOURCE_DIR = Path(config_data["source_path"]).expanduser()
            else:
                self.SOURCE_DIR = self.WORK_DIR / ".source"             # Downloaded source code cache directory
            
            self.OUTPUT_DIR = self.WORK_DIR / "output"              # SDK output directory
            self.PLATFORM_DIR = self.WORK_DIR / "native"            # Platform directory, stores artifacts related to specific toolchain builds

            return True
        except Exception as e:
            return False


    def _debug(self):
        print(f"+++ {__class__} debug +++")
        print(f"SDK_UTILS_ROOT_DIR: {self.SDK_UTILS_ROOT_DIR}")
        print(f"ETC_DIR: {self.ETC_DIR}")
        print(f"PKG_DIR: {self.PACKAGE_DIR}")
        print(f"WORK_DIR: {self.WORK_DIR}")
        print(f"OUTPUT_DIR: {self.OUTPUT_DIR}")
        print(f"SOURCE_DIR: {self.SOURCE_DIR}")
        print(f"PLATFORM_DIR: {self.PLATFORM_DIR}")
        print(f"TOOLCHAIN_DIR: {self.TOOLCHAIN_DIR}")
        print(f"BUILD_DIR: {self.BUILD_DIR}")
        print(f"SYSROOT_DIR: {self.SYSROOT_DIR}")
        print(f"--- {__class__} debug ---")


"""
Global static object
"""
global_fsh = Fsh()


if __name__ == "__main__":
    # test code
    fsh = global_fsh.copy()
    fsh.init(platform_dir="/home/fj/j5")
    global_fsh._debug() 
    fsh._debug()