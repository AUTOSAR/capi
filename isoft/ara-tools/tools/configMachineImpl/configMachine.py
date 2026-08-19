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

"""
Entry point of configMachine.
"""

import glob
import logging
import os
import signal
# from distutils.command.config import config
import sys
# Import class time from time module
from time import time

from tools.configMachineImpl.configMachine_args import configMachineArgs
from tools.configMachineImpl.configurator.configurator import Configurator
from tools.configMachineImpl.exception_handling import handle_exceptions


def handle_sigint(signum, frame):
    """
    Handle SIGINT (i.e. Ctrl-c) properly.
    This entails triggering the system's default SIGINT handler
    after all our own processing has been done, so that a calling
    shell knows that we have terminated because of SIGINT.
    """
    # pylint: disable=unused-argument
    print('Interrupted by user', file=sys.stderr)
    signal.signal(signum, signal.SIG_DFL)
    os.kill(os.getpid(), signum)

@handle_exceptions(error_string_template="ERROR: {exception_text}")
def main(argv=None):
    """
    Main entry point to aragen.
    """
    signal.signal(signal.SIGINT, handle_sigint)
    
    # return value
    ret_code = 0

    if argv is None:
        argv = sys.argv
    args = configMachineArgs(argv)

    # Configure root logger, from which all other logger inherit their settings.
    log_level = logging.WARNING - 10*args.verbose
    # The simple calculation above does not cope with the fact that our own TRACE level
    # does not fully adher to the level number sequence rule.
    if args.verbose >= 3:
        log_level = logging.TRACE
    # log_format = '{asctime} - [{levelname:<8}] {name} : {message}'
    log_format = '{asctime} - [{levelname:<8}] [ARATOOLS] [ConfigMachine] {name}: {message}'

    # Make sure that there is always at least one handler in the list,
    # otherwise logging.basicConfig() might add in some default handlers.
    log_handlers = [logging.NullHandler()]
    if not args.quiet:
        # This option only concerns itself with suppressing console output.
        stream_handler = logging.StreamHandler()
        stream_handler.setLevel(log_level)
        log_handlers.append(stream_handler)
    
    fullLogPath = ""
    if args.log:
        file_handler = logging.FileHandler(args.log)
        file_handler.setLevel(log_level)
        log_handlers.append(file_handler)
        
        # get full log file path
        args.log = os.path.realpath(args.log)
        logPath = os.path.normpath(args.log)
        logDir = os.path.dirname(logPath)
        logName = os.path.basename(logPath)
        logNameParts = logName.rsplit(".", 1)
        fullLogName = ""
        for idx in range(len(logNameParts)):
            if(idx == 1):
                fullLogName += "."
            fullLogName += logNameParts[idx]
            if(idx == 0):
                fullLogName += "-full"
        fullLogPath = os.path.join(
            logDir, fullLogName)
    
    # adapt relative path
    args.ara_sysroot = os.path.realpath(args.ara_sysroot)
    
    # if log parameter not specified, full log is stored under ara_sysroot/tools-log
    if 0 == len(fullLogPath):
        logDir = os.path.join(args.ara_sysroot, "tools-log")
        fullLogPath = os.path.join(logDir, "updateMachine{0}-full.log".format(int(round(time()))))
    
    # clean logs
    log_file_path = os.path.join(logDir, "updateMachine*-full.log")
    log_files = sorted(glob.iglob(log_file_path), key=os.path.getctime, reverse=True)
    for i in range(10, len(log_files)):
        os.remove(log_files[i])
    
    # create log directory
    if not os.path.exists(logDir): 
        os.makedirs(logDir, 0o777)

    full_log_file_handler = logging.FileHandler(fullLogPath)
    full_log_file_handler.setLevel(level=logging.NOTSET)
    log_handlers.append(full_log_file_handler)
    
    logging.basicConfig(level=logging.NOTSET, format=log_format, style='{', handlers=log_handlers)

    log = logging.getLogger(__name__)
    log.info(
        "Starting up: files=%s fullLogPath=%s ara_sysroot=%s",
        args.files,
        fullLogPath,
        args.ara_sysroot,
    )

    if args.withSDK:
        args.withSDK = os.path.realpath(args.withSDK)
        log.info('main, args.withSDK:%s', args.withSDK)

    if args.qos_path:
        args.qos_path = os.path.realpath(args.qos_path)
        log.info('main, args.qos_path:%s', args.qos_path)
    
    for idx in range(len(args.executable_build_dirs)):
        args.executable_build_dirs[idx] = os.path.realpath(args.executable_build_dirs[idx])

    args.workdir = os.path.realpath(args.workdir)
    log.info('main, args.workdir:%s', args.workdir)

    # Step 5: Generation.
    con = Configurator(args)
    ret_code = con.configure()

    log.info('Finished with ret_code:%d', ret_code)
    return ret_code
