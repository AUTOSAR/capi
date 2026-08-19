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
# @file       log.py
# @brief      Log-related operations
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import datetime
import sys
import os
from typing import TextIO
from pathlib import Path

__all__ = ["Logger", "global_logger"]


class Logger:
    """Logging module for log-related operations"""
    CHANNEL_CONSOLE: TextIO = sys.stdout    # Console output channel
    CHANNEL_FILE: TextIO = None             # File output channel


    def __init__(self):
        return

    
    def __del__(self):
        try:
            if self.CHANNEL_FILE is not None:
                self.CHANNEL_FILE.close()
        except:
            return

    
    def init(self, log_file:Path = None):
        """
            Initialize logging
        Args:
            log_file: Log storage file
        """
        try:
            if log_file is None:
                return True

            if self.CHANNEL_FILE is not None:
                self.error("have been inited.")
                return False

            self.CHANNEL_FILE = open(log_file, 'wt')

        except Exception as e:
            self.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def error(self, msg:str):
        """
            Print error log; log will be written to file and printed to terminal
        Args:
            msg: Log content
        """
        self._format(self.CHANNEL_CONSOLE, "ERROR", msg)
        self._format(self.CHANNEL_FILE,    "ERROR", msg)

    
    def warning(self, msg:str):
        """
            Print warning log; log will be written to file and printed to terminal
        Args:
            msg: Log content
        """
        self._format(self.CHANNEL_CONSOLE, "WARN ", msg)
        self._format(self.CHANNEL_FILE,    "WARN ", msg)


    def info(self, msg):
        """
            Print important log; log will be written to file and printed to terminal
        Args:
            msg: Log content
        """
        self._format(self.CHANNEL_CONSOLE, "INFO ", msg)
        self._format(self.CHANNEL_FILE,    "INFO ", msg)


    def verbose(self, msg):
        """
            Print detailed log; log will be written to file
        Args:
            msg: Log content
        """
        self._format(self.CHANNEL_FILE, "DEBUG", msg)


    def _format(self, channel:TextIO, type:str, msg:str):
        """
            Format log string
        Args:
            channel: Log channel
            type:   Log type
            msg:    Log content
        """
        try:
            if channel is None:
                return
            text_line = f"== sdk-utils == [{type}] {self._get_time_stamp()} # {msg}\n"
            channel.writelines([text_line])
            channel.flush()
        except Exception as e:
            print(type, __class__.__name__, "->", sys._getframe().f_code.co_name, ":", e)


    def _get_time_stamp(self):
        """
            Get timestamp
        Returns:
            Formatted timestamp string
        """
        now = datetime.datetime.now()
        return datetime.datetime.strftime(now, "%Y-%m-%d %H:%M:%S")


"""
Global log object
"""
global_logger = Logger()


def _tests():
    #global_logger.init()
    global_logger.info("helloworld")


if "__main__" == __name__:
    _tests()