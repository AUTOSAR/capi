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
# @file       args.py
# @brief      Command line argument parsing
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import sys
from argparse import ArgumentParser
from abc import ABC, abstractmethod
from pathlib import Path
from typing import List, Optional, Dict, Any
from dataclasses import dataclass, field
from enum import Enum

__SDK_UTILS_SRC_DIR = str(Path(__file__).resolve().parent.parent)
if __SDK_UTILS_SRC_DIR not in sys.path:
    sys.path.insert(0, __SDK_UTILS_SRC_DIR)

from core import global_fsh

__all__ = [
    "CmdLineParser",
    "BuildType",
    "BuildArguments",
    "PackArguments",
    "InfoArguments",
    "UpdateArguments",
    "AreArguments",
    "AreExportType",
    "AreStripLevel"
]


class BuildType(Enum):
    """Define build type enumeration values."""
    Debug = "Debug"                     # Debug mode, convenient for development and debugging: includes full debugging information (-g), no or minimal optimization (-O0)
    Release = "Release"                 # Release mode, optimizes performance and speed: no debugging information, defines NDEBUG macro, high-level optimization (-O3)
    RelWithDebInfo = "RelWithDebInfo"   # Release mode with debugging information, balances performance and debuggability: includes some debugging information (-g), defines NDEBUG macro, moderate optimization (-O2)
    MinSizeRel = "MinSizeRel"           # Release mode minimizing size, minimizes output file size: no debugging information, defines NDEBUG macro, optimizes for size (-Os)


@dataclass(frozen=True)
class BuildArguments:
    """Define parameters for the 'build' subcommand."""
    TOOLCHAIN:str = None                       # Toolchain name
    BUILD_TYPE:BuildType = BuildType.Debug     # Build type
    BUILD_VERSION:str = None                   # Build version number
    PACKAGE_LIST:List[str] = None              # List of software packages to build
    PACKAGE_CONFIG:Path = None                 # Path to the software package configuration file; if specified, PACKAGE_LIST is ignored.
    SPACE_LIMIT:bool = False                   # Space-limited mode; in this mode, disk usage will be minimized
    SOURCE:Path = None                         # Source directory or archive path; if None, automatically download
    OUTPUT_DIR:Path = None                     # Output folder, i.e., the location where sysroot is stored


@dataclass(frozen=True)
class PackArguments:
    """Define parameters for the 'pack' subcommand."""
    INPUT_DIR:Path = None          # Pack input directory, i.e., the path where sysroot/toolchain is located
    OUTPUT_DIR:Path = None         # Output directory, where the generated SDK will be stored
    EXTRACT:bool = False       # Whether to extract debug information into separate files


@dataclass(frozen=True)
class InfoArguments:
    """Define parameters for the 'info' subcommand."""
    ALL:bool = False            # Whether to display all configuration information
    PROJECT:str = None          # Project name; None means no option set; "" means option set without argument; str means option set with argument
    TOOLCHAIN:str = None        # Toolchain name, same as above
    SOFTWARE:str = None         # Software package, same as above


@dataclass(frozen=True)
class UpdateArguments:
    """Define parameters for the 'update' subcommand."""
    SDK_FILE:Path = None            # Path to the SDK source file
    UPDATE_DIR:Path = None          # Path to the update directory
    OUTPUT_DIR:Path = None           # Path to the output directory, where the updated SDK will be stored


class AreExportType(Enum):
    """Define enumeration values for ARE export types."""
    ARE = "are"                     # Export ARE
    TAR = "tar"                     # Export tar package


class AreStripLevel(Enum):
    """Define enumeration values for ARE export types."""
    NONE = "0"            # No stripping
    FILE = "1"            # File stripping only, delete unnecessary dependent libraries
    BIN = "2"             # Binary stripping only, strip all ELF files
    ALL = "3"             # Full stripping, combination of FILE and BIN


@dataclass(frozen=True)
class AreArguments:
    """Define parameters for the 'are' subcommand."""
    INPUT:Path = None                       # Pack input directory, i.e., the path where sysroot is located
    OUTPUT:Path = None                      # Output directory, where the generated SDK will be stored
    TYPE:AreExportType = AreExportType.ARE  # Type of ARE export, default to ARE package
    STRIP:AreStripLevel = AreStripLevel.NONE  # Strip level, default no strip.
    CONFIG:Path = None                      # Export behavior control file, determines how to export based on configuration.


class CmdLineParser:
    """Command line argument parser."""
    def __init__(self):
        self._parser = None
        self.build_args = None
        self.pack_args = None
        self.info_args = None
        self.update_args = None
        self.are_args = None
        self.is_publish = False
        self._init_parser()
    

    def _init_parser(self):
        self._parser = ArgumentParser(usage="Adaptive Platform SDK tools", allow_abbrev=True)
        self._parser.add_argument("-d", "--debug", action="store_true", default=False, help="debug mode")
        subparsers = self._parser.add_subparsers(dest="command", help="available subcommands")
        # Add build subcommand
        self._add_build_subcommand(subparsers)
        self._add_pack_subcommand(subparsers)
        self._add_info_subcommand(subparsers)
        self._add_pub_subcommand(subparsers)
        self._add_update_subcommand(subparsers)
        self._add_are_subcommand(subparsers)
        # Call uniformly after parsing
        args = self._parser.parse_args()
        if args.debug:
            global_fsh.DEBUG = True
            
        if hasattr(args, 'func'):
            # Directly call the bound function
            args.func(args)  
        else:
            self._parser.print_help()
        return True

    
    def _add_pub_subcommand(self, parser:ArgumentParser) -> None:
        """
            Add pub subcommand
        Args:
            parser: Parser
        Returns:
            None
        """

        parser_pub = parser.add_parser("pub", help="publish SDK, build all the source and export SDK")
        parser_pub.set_defaults(func=self._handle_pub)
        group = parser_pub.add_mutually_exclusive_group(required=True)
        # Add positional argument; nargs='?' means the positional argument is optional
        group.add_argument("branch", nargs='?', help="the version and url string of apall source.")
        group.add_argument("-f", "--config-file", metavar="CONFIG_FILE", dest="config_file", default=None, nargs=1, help="the configuration file of the software package to be built.")
        parser_pub.add_argument("-t", "--toolchain", metavar="TOOLCHAIN", dest="toolchain", default=["native"], nargs=1, help="toolchain name, default is 'native'.")
        parser_pub.add_argument("-v", "--build-version", metavar="BUILD_VERSION", dest="build_version", default=None, nargs=1, help="build version. if not specified, it will default to the current date.")
        parser_pub.add_argument("-s", "--space-limit", dest="space_limit", action="store_true", default=False, help="space-limited mode, if enabled, it will minimize the disk space usage during compilation. the default value is 'False'.")
        parser_pub.add_argument("-b", "--build-type", metavar="BUILD_TYPE", dest="build_type", nargs=1,
                                  choices=['Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel'],
                                  default=["Debug"], help="build type, default is %(default)s.")
        parser_pub.add_argument("-e", "--extract-debug-info", dest="extract", action="store_true", default=False, help="extract the debuginfo to a separate file and strip the SDK, the default value is 'False'.")
        parser_pub.add_argument("-o", "--output-dir", metavar="OUTPUT_DIR", dest="output_dir", nargs=1,
                                  help="the folder where the generated SDK is stored. if no specified, it will be defaulted to var/output.")


    def _handle_pub(self, args):
        """
        Process parameters for the pack subcommand.
        
        The pub subcommand is essentially a combination of the build and pack subcommands, so no new structure is defined for storing parameters; existing build and pack parameter structures are used.
        """

        self.is_publish = True

        # Parse build-related parameters
        toolchain = args.toolchain[0]

        build_type = BuildType.RelWithDebInfo
        if args.build_type is not None:
            build_type = BuildType(args.build_type[0])

        build_version = None if args.build_version is None else args.build_version[0]
        pkg_list = None
        config_path = None
        if args.config_file is not None:
            config_path = Path(args.config_file[0])
        else:
            pkg_list = [f"apall:{args.branch}"]
        
        output_dir = None if args.output_dir is None else Path(args.output_dir[0]).absolute()

        # Save results
        self.build_args = BuildArguments(
            TOOLCHAIN = toolchain,
            BUILD_TYPE = build_type,
            BUILD_VERSION = build_version,
            PACKAGE_LIST = pkg_list,
            PACKAGE_CONFIG = config_path,
            SPACE_LIMIT = args.space_limit,
            OUTPUT_DIR = None, # Output directory not specified, use default location
        )

        self.pack_args = PackArguments(
            INPUT_DIR = None, # pub command does not need to specify input for PackArguments; use default. Because the Builder process has already called global_fsh.set_platform_dir().
            OUTPUT_DIR = output_dir,
            EXTRACT = args.extract
        )

        
        
    def _add_build_subcommand(self, parser:ArgumentParser) -> None:
        """
        Add build subcommand.
        
        Args:
            parser: Parser
        Returns:
            None
        """

        parser_build = parser.add_parser("build", help="build software package")
        parser_build.set_defaults(func=self._handle_build)
        parser_build.add_argument("package_list", nargs="*", help="list of software packages that need to be built. format is 'NAME[:VERSION][#URL]'")
        parser_build.add_argument("-f", "--config-file", metavar="CONFIG_FILE", dest="config_file", default=None, nargs=1, help="the configuration file of the software package to be built.")
        parser_build.add_argument("-t", "--toolchain", metavar="TOOLCHAIN", dest="toolchain", default=["native"], nargs=1, help="toolchain name, default is 'native'.")
        parser_build.add_argument("-v", "--build-version", metavar="BUILD_VERSION", dest="build_version", default=None, nargs=1, help="build version. if not specified, it will default to the current date.")
        parser_build.add_argument("-s", "--space-limit", dest="space_limit", action="store_true", default=False, help="space-limited mode, if enabled, it will minimize the disk space usage during compilation. the default value is 'False'.")
        parser_build.add_argument("-o", "--output-dir", metavar="OUTPUT_DIR", dest="output_dir", nargs=1,
                                  help="the folder where the generated sysroot is stored. if no specified, it will be defaulted to var/PLATFORM/sysroot.")
        parser_build.add_argument("-b", "--build-type", metavar="BUILD_TYPE", dest="build_type", nargs=1,
                                  choices=['Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel'],
                                  default=["Debug"],
                                  help="build type, default is %(default)s.")
    
    
    def _handle_build(self, args):
        """Process parameters for the build subcommand."""
        # Parse build type
        bt_str = args.build_type[0]
        build_type = BuildType[bt_str]
        build_version = None if args.build_version is None else args.build_version[0]
        # Manually implement mutually exclusive group
        if args.config_file and args.package_list:
            self._parser.error("arguments -f/--config-file and 'package_list' cannot be used together.")
        if not (args.config_file or args.package_list):
            self._parser.error("you must specify either -f/--config-file or at least one package name.")
        config_file = None if args.config_file is None else Path(args.config_file[0]).absolute()
        output_dir = None if args.output_dir is None else Path(args.output_dir[0]).absolute()
        # Save results
        self.build_args = BuildArguments(
            TOOLCHAIN = args.toolchain[0],
            BUILD_TYPE = build_type,
            BUILD_VERSION = build_version,
            PACKAGE_LIST = args.package_list,
            PACKAGE_CONFIG = config_file,
            SPACE_LIMIT = args.space_limit,
            OUTPUT_DIR = output_dir,
        )

    
    def _add_pack_subcommand(self, parser:ArgumentParser) -> None:
        """
        Add pack subcommand.
        Args:
            parser: Parser
        Returns:
            None
        """

        parser_pack = parser.add_parser('pack', help='package SDK')
        parser_pack.set_defaults(func=self._handle_pack)
        parser_pack.add_argument("input_dir", nargs=1, help="the input dir, contains source sysroot and toolchain, used for generating the SDK.")
        parser_pack.add_argument("-e", "--extract-debug-info", dest="extract", action="store_true", default=False, help="extract the debuginfo to a separate file and strip the SDK, the default value is 'False'.")
        parser_pack.add_argument("-o", "--output-dir", metavar="OUTPUT_DIR", dest="output_dir", nargs=1,
                                  help="the folder where the generated SDK is stored. if no specified, it will be defaulted to var/output.")


    def _handle_pack(self, args):
        """Process parameters for the pack subcommand."""
        # Parse output path
        output_dir = None if args.output_dir is None else Path(args.output_dir[0]).absolute()
        # Save results
        self.pack_args = PackArguments(
            INPUT_DIR = args.input_dir[0],
            OUTPUT_DIR = output_dir,
            EXTRACT = args.extract
        )
    

    def _add_info_subcommand(self, parser:ArgumentParser) -> None:
        """
        Add info subcommand.

        Args:
            parser: Parser
        Returns:
            None
        """

        parser_info = parser.add_parser('info', help='display the configuration information')
        parser_info.set_defaults(func=self._handle_info)
        parser_info.add_argument("-a", "--all", dest="all", action="store_true", default=False, help="display all the configuration information")
        parser_info.add_argument("-p", "--project", metavar="PROJECT", dest="project", nargs='?', default=None, const="",
                                 help="display the project configuration. if no project name is specified, display all the project configurations.")
        parser_info.add_argument("-s", "--software", metavar="SOFTWARE", dest="software", nargs='?', default=None, const="",
                                 help="display the software configuration. if no toolchain name is specified, display all the software configurations.")
        parser_info.add_argument("-t", "--toolchain", metavar="TOOLCHAIN", dest="toolchain", nargs='?', default=None, const="",
                                 help="display the toolchain configuration. if no toolchain name is specified, display all the toolchain configurations.")
    
    
    def _handle_info(self, args):
        """Process parameters for the update subcommand."""
        self.info_args = InfoArguments(
            ALL = args.all,
            PROJECT = args.project,
            TOOLCHAIN = args.toolchain,
            SOFTWARE = args.software
        )

    
    def _add_update_subcommand(self, parser:ArgumentParser) -> None:
        """
        Add update subcommand.

        Args:
            parser: Parser
        Returns:
            None
        """

        parser_update = parser.add_parser('update', help='update the content of the SDK file')
        parser_update.set_defaults(func=self._handle_update)
        parser_update.add_argument("sdk_file", metavar="SDK_FILE", nargs=1, help="specify the origin SDK file.")
        parser_update.add_argument("update_dir", metavar="UPDATE_DIR", nargs=1, help="specify the update directory.")
        parser_update.add_argument("-o", "--output-dir", metavar="OUTPUT_DIR",dest="output_dir", nargs=1, help="specify the output directory for the new SDK. If not specified, it will overwrite the original SDK files.")
    
    
    def _handle_update(self, args):
        """Process parameters for the update subcommand."""
        # Parse output path
        output = None if args.output_dir is None else Path(args.output_dir[0]).absolute()
        self.update_args = UpdateArguments(
            SDK = Path(args.sdk_file[0]).absolute(),
            UPDATE = Path(args.update_dir[0]).absolute(),
            OUTPUT = output
        )
    
    
    def _add_are_subcommand(self, parser:ArgumentParser) -> None:
        """
        Add are subcommand.

        Args:
            parser: Parser
        Returns:
            None
        """

        parser_are = parser.add_parser('are', help='export the ARE(AP Runtime Environment) file')
        parser_are.set_defaults(func=self._handle_are)
        parser_are.add_argument("sysroot", metavar="SYSROOT_DIR", nargs=1, help="specify the sysroot dir.")
        parser_are.add_argument("-o", "--output-dir", metavar="OUTPUT_DIR",dest="output_dir", nargs=1, help="specify the output directory for the new SDK. If not specified, it will overwrite the original SDK files.")
        parser_are.add_argument("-s", "--strip", metavar="STRIP", dest="strip", nargs='?',
                                  choices=['1', '2', '3'], const=True, default=None, help="strip level, default is %(default)s.")
        parser_are.add_argument("-t", "--export-type", metavar="EXPORT_TYPE", dest="export_type", nargs=1,
                                  choices=['tar', 'are'],
                                  default=["are"], help="export type, default is %(default)s.")
        parser_are.add_argument("-f", "--config-file", metavar="CONFIG_FILE", dest="config_file", default=None, nargs=1, help="the configuration file to control export content.")
    
    
    def _handle_are(self, args):
        """Process parameters for the are subcommand."""
        # Parse output path
        output = None
        if args.output_dir is not None:
            output = args.output_dir[0]
        
        export_type = AreExportType.ARE
        if args.export_type is not None:
            export_type = AreExportType(args.export_type[0])

        strip_type = AreStripLevel.NONE
        if args.strip is not None:
            if True == args.strip:
                strip_type = AreStripLevel.ALL
            else:
                strip_type = AreStripLevel(args.strip[0])
        
        config_file = None
        if args.config_file is not None:
            config_file = args.config_file[0]

        self.are_args = AreArguments(args.sysroot[0], output, export_type, strip_type, config_file)
    

def _tests():
    """
    Test program
    """
    c = CmdLineParser()
    print(c.is_publish)
    print(c.build_args)
    print(c.pack_args)
    print(c.info_args)
    print(c.update_args)
    print(c.are_args)
    

if __name__ == "__main__":
    _tests()