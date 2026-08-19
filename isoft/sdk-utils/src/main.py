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
# @file       sdk-utils.py
# @brief      Program entry point
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import sys, os
from pathlib import Path
import json

__SDK_UTILS_SRC_DIR = str(Path(__file__).resolve().parent)
if __SDK_UTILS_SRC_DIR not in sys.path:
    sys.path.insert(0, __SDK_UTILS_SRC_DIR)

from core import Fsh, global_fsh, Logger, global_logger
from config.args import *
from config import ConfigInfo
from build.builder import Builder
from package.sdk import SDKPackager, SDKUpdater
from package.are import AREPackager


class Main():
    def __init__(self):
        self._cmdline_args = None
        self._is_initialized = False


    def run(self):
        try:
            self._cmdline_args = CmdLineParser()
            self._init_global_env()
            if self._cmdline_args.info_args is not None:
                return self._do_info()
            elif self._cmdline_args.is_publish:
                return self._do_publish()
            elif self._cmdline_args.build_args is not None:
                return self._do_build()
            elif self._cmdline_args.pack_args is not None:
                return self._do_pack()
            elif self._cmdline_args.update_args is not None:
                return self._do_update()
            elif self._cmdline_args.are_args is not None:
                return self._do_are()
            else:
                raise Exception("invalid arguments")

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
    
    
    def _do_info(self):
        try:
            info_args = self._cmdline_args.info_args
            ci = ConfigInfo()
            if info_args.ALL:
                all_info = ci.get_all()
                print(json.dumps(all_info, indent=4, ensure_ascii=False, sort_keys=False))

            elif None is not info_args.TOOLCHAIN:
                tc_name = info_args.TOOLCHAIN
                if 0 == len(tc_name):
                    tc_name = None
                tc_info = ci.get_toolchain(tc_name)
                print(json.dumps(tc_info, indent=4, ensure_ascii=False, sort_keys=False))

            elif None is not info_args.SOFTWARE:
                sw_name = info_args.SOFTWARE
                if 0 == len(sw_name):
                    sw_name = None
                sw_info = ci.get_software(sw_name)
                print(json.dumps(sw_info, indent=4, ensure_ascii=False, sort_keys=False))

            elif None is not info_args.PROJECT:
                prj_name = info_args.PROJECT
                if 0 == len(prj_name):
                    prj_name = None
                prj_info = ci.get_project(prj_name)
                print(prj_info)
            else:
                print("1.0.0")
            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

    

    def _do_pack(self):
        try:
            pack_args = self._cmdline_args.pack_args
            output_dir = global_fsh.OUTPUT_DIR.absolute()
            input_dir = None if pack_args.INPUT_DIR is None else Path(pack_args.INPUT_DIR).absolute()
            
            sp = SDKPackager(input_dir)
            return sp.generate(output_dir, pack_args.EXTRACT)

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    
    def _do_build(self):
        try:
            build_args = self._cmdline_args.build_args
            b = Builder(build_args)
            return b.run()

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False



    def _do_publish(self):
        return self._do_build() and self._do_pack()



    def _do_update(self):
        try:
            update_args = self._cmdline_args.update_args
            if None is not update_args.OUTPUT_DIR:
                output = update_args.OUTPUT_DIR
            else:
                output = Path(update_args.SDK_FILE).parent
            b = SDKUpdater(update_args.SDK_FILE)
            return b.update(update_args.UPDATE_DIR, output)

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
    
    

    def _do_are(self):
        try:
            are_args = self._cmdline_args.are_args
            a = AREPackager(are_args)
            return a.generate()

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False



    def _init_global_env(self):
        """Unified global environment initialization"""
        if self._is_initialized:
            return
            
        # SDK file output directory
        output_dir = None
        # Build intermediate artifacts output directory
        platform_dir = None
        # Log file
        log_file = None
        
        # When the subcommand is pub or build, parse build_args to get work_dir and platform_dir
        if self._cmdline_args.is_publish:
            platform_dir = self._cmdline_args.build_args.TOOLCHAIN
            # In pub mode, both build's OUTPUT and pack's INPUT are None, using the default platform directory connection
            output_dir = self._cmdline_args.pack_args.OUTPUT_DIR
            global_fsh.init(output_dir=output_dir, platform_dir=platform_dir)
            log_file = global_fsh.OUTPUT_DIR / "sdk-utils.log"

        elif self._cmdline_args.build_args is not None:
            platform_dir = self._cmdline_args.build_args.TOOLCHAIN
            if self._cmdline_args.build_args.OUTPUT_DIR is not None:
                platform_dir = self._cmdline_args.build_args.OUTPUT_DIR
            global_fsh.init(output_dir=output_dir, platform_dir=platform_dir)
            log_file = global_fsh.PLATFORM_DIR / "sdk-utils.log"
        
        elif self._cmdline_args.pack_args is not None:
            output_dir = self._cmdline_args.pack_args.OUTPUT_DIR
            global_fsh.init(output_dir=output_dir, platform_dir=platform_dir)
            log_file = global_fsh.OUTPUT_DIR / "sdk-utils.log"

        else:
            global_fsh.init(output_dir=output_dir, platform_dir=platform_dir)
            log_file = global_fsh.OUTPUT_DIR / "sdk-utils.log"

        # global_fsh._debug()
        global_logger.init(log_file=log_file)
        
        self._is_initialized = True

    
    
if __name__ == "__main__":
    m = Main()
    if m.run():
        sys.exit(0)
    else:
        sys.exit(1)