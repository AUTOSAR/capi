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
Setup an argument parser for configMachine
"""


__version__ = "0.1.0"

import argparse
import logging




EPILOG = r'''
Examples
--------
    ./configMachine --ara_sysroot /home/hanzhibo/ara-sysroot --machine /ISOFT/Development/Machine1 --executable-build-dirs /home/hanzhibo/.isoft/tmp/ara_binout -w ./tmp /home/hanzhibo/ara-sysroot/ara/framework/x.x.x/share/ara-arxmls -v -v

This command will integrate all the software clusters for the machine whose fqn is /ISOFT/Development/Machine1 in the /home/hanzhibo/ara-sysroot as ara_sysroot using /tmp/machine as work directory.
While integrating a software cluster, try to find the contained executable in the executable-build-dirs:/home/hanzhibo/.isoft/tmp/ara_binout 
'''

class configMachineArgs(object):
    """
    Allow to store the configMachine Args, do checks and run the corresponding
    flags.
    """
    def __init__(self, argv):
        """
        Build the argument parser and return the parsed command line
        """
        self._log = logging.getLogger(__name__)

        arg_parser = argparse.ArgumentParser(
            description="",
            epilog=EPILOG,
            formatter_class=argparse.RawTextHelpFormatter)

        # Flags

        arg_parser.add_argument(
            "-v",
            "--verbose",
            action="count",
            default=0,
            help="Increase verbosity of logging (can give multiple times)")

        arg_parser.add_argument(
            "-q",
            "--quiet",
            action="store_true",
            help="Suppress all logging output to console (overrides --verbose)")

        arg_parser.add_argument(
            "-d",
            "--debug",
            action="store_true",
            default=False,
            help="Specify detailed logging configuration")

        arg_parser.add_argument(
            "-l",
            "--log",
            help="Write all log data to the specified file")

        arg_parser.add_argument(
            "-m",
            "--machine",
            required=True,
            help="The fqn of the machine for which we generate machine manifest.")

        arg_parser.add_argument(
            "-r",
            "--ara_sysroot",
            required=True,
            help="directory for ARA_SYSROOT")

        arg_parser.add_argument(
            "-w",
            "--workdir",
            required=True,
            help="Work directory for ara_gen.")
        
        arg_parser.add_argument(
            "-e",
            "--executable-build-dirs",
            required=True,
            default=None,
            type=lambda csv: [buildDir for buildDir in csv.split(',')],
            help="Search all executables in the comma separated build dirs list.\n")

        arg_parser.add_argument(
            '--withSDK',
            required=False,
            help="The directory of the existed SDK;\n")
        
        arg_parser.add_argument(
            "--qos-path",
            help="The qos file path that will be passed to aragen.")

        arg_parser.add_argument(
            '--canUseExistingExe',
            action='store_true',
            required=False,
            default=False,
            help="When canUseExistingExe is true, it is ok to use the existing executable of the SWCL in ara_sysroot;\n")
        
        arg_parser.add_argument(
            '--keep-ara-gen-output',
            action='store_true',
            required=False,
            default=False,
            help="When keeping ara-gen's output, we will not clean the output from ara-gen;\n"
                 "Otherwise, we will clean them.\n")
        
        arg_parser.add_argument(
            "files",
            nargs="+",
            help="A list of files that will be input for the configurator,\n"
                 "can be either globs, individual files, or directories.\n"
                 "In the case that there is a path to a directory, all\n"
                 ".arxml files in the directory will be used as input.")

        arguments = argv[1:]
        self._args = arg_parser.parse_args(arguments)


    def __getattr__(self, item):
        # pass through getters to underlying args object
        return getattr(self._args, item)


    # aliases for more clean naming
    @property
    def ara_sysroot_dir(self):
        """
        Return Directory of the ARA_SYSROOT.
        :return: ARA_SYSROOT Directory Path
        """
        return self._args.ara_sysroot
    