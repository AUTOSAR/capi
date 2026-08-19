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
# @file       info.py
# @brief      Parse ara information
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import sys, json
from pathlib import Path
from typing import Dict, List, Union
from dataclasses import dataclass, field
import subprocess
import importlib

__SDK_UTILS_SRC_DIR = str(Path(__file__).resolve().parent.parent)
if __SDK_UTILS_SRC_DIR not in sys.path:
    sys.path.insert(0, __SDK_UTILS_SRC_DIR)

#from core import Fsh, global_fsh
from core import Logger, global_logger
from core import CompilerInterface
from config.software_package import PackageManager, Package
import toolchain

__all__ = ["ConfigInfo"]


class ConfigInfo:
    """
    
        Get configuration information
    """
    def __init__(self):
        pass


    def get_toolchain(self, toolchain_name:str=None) -> Union[List[str], Dict]:
        try:
            toolchain_list = toolchain.__all__
            if None is toolchain_name:
                return toolchain_list
            else:
                tc = importlib.import_module("toolchain." + toolchain_name)
                tc_obj:CompilerInterface = eval("tc.Compiler")()
                return tc_obj.profile.to_dict()
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None
    
    
    def get_project(self) -> Dict:
        try:
            # TODO: Implement project information
            return []
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None
    

    def get_software(self, software_name:str=None) -> bool:
        try:
            pm = PackageManager()
            return pm.export(software_name)

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None

    
    def get_all(self) -> bool:
        try:
            all = {}
            all["project"] = self.get_project()
            all["toolchain"] = self.get_toolchain()
            all["software"] = self.get_software()
            return all
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None



def _tests():
    ci = ConfigInfo()
    ci.load()


if __name__ == "__main__":
    _tests()