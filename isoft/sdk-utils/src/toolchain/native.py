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
# @file       native.py
# @brief      Default local compiler implementation
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import os, sys, shutil
from pathlib import Path

__SDK_UTILS_SRC_DIR = str(Path(__file__).resolve().parent.parent)
if __SDK_UTILS_SRC_DIR not in sys.path:
    sys.path.insert(0, __SDK_UTILS_SRC_DIR)

from core import CompilerInterface


class Compiler(CompilerInterface):
    """
    Local compiler implementation
    """
    def __init__(self):
        super().__init__()
        self.profile.NAME = Path(__file__).stem     # Same as file name
        self.profile.VERSION = "9.4.0"
        self.profile.DESCRIPTION = "Current host machine's gcc compiler"
        self.profile.PROJECT = "iSOFT"
        self.profile.BIN_DIR = os.path.dirname(shutil.which("gcc"))
        self.profile.HOST = "x86_64-linux-gnu"
        self.profile.BUILD = self.profile.HOST
        self.profile.TARGET = self.profile.HOST
        self.profile.TARGET_ARCH = "x86_64"
        self.profile.IS_CROSS_COMPILER = False
        # self._fsh.set_platform_dir(self.profile.NAME)
        self.profile.CXXFLAGS = f"-I{self._fsh.SYSROOT_DIR}/usr/include"
        self.profile.LDFLAGS = f"-L{self._fsh.SYSROOT_DIR}/usr/lib"


if __name__ == "__main__":
    c = Compiler()
    c.install()
    print(c.profile.to_dict())
    print(c.profile.to_json())