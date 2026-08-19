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
Setup an argument parser for aragen based on the information present in
doc/README.md
"""


__version__ = "1.0.0"

import argparse
import json
import logging

EPILOG = r'''
Examples
--------
    aragen --output output /path/to/arxmls/

This command will generate the code in a folder 'output' under the
current directory.
Everything what is possible to generate will be generated,
I.e. Service Interface headers, including proxies and skeletons,
process-related manifests for Execution Management
and machine-related configs for nsomeip daemon, networking and EM.  
SOMEIP network bindings via nsomeip library will be generated
if they are present in the system.  
'''

POSSIBLE_FOR_PROCESS = ["ALL", "CODE", "PROJECT", "PROJECT_UPDATE"]
DEFAULT_FOR_PROCESS = "ALL"

POSSIBLE_SOMEIP_LIBS = ["nsomeip2", "none"]
DEFAULT_SOMEIP_LIB = "nsomeip2"
POSSIBLE_DDS_LIBS = ["fastdds","none"]
DEFAULT_DDS_LIB = "fastdds"
POSSIBLE_IPC_LIBS =  ["npc", "none"]
DEFAULT_IPC_LIB = "none"
POSSIBLE_ICC_LIBS =  ["icc", "none"]
DEFAULT_ICC_LIB = "none"


def config_load(config):
    """
    When loading a configuration we need to know whether it is a file or inline
    JSON string.
    :param config: path to JSON configuration or inline JSON string
    :return: dictionary representation of the config
    """
    if ".json" in config:
        with open(config, "r") as config_file:
            return json.load(config_file)
    else:
        return json.loads(config)


class CsvChoices(object):
    """
    Allow a csv list to be passed and cross referenced with a limited set of
    choices.
    """
    def __init__(self, name, choices, none_value=None):
        """
        :param choices: list of possible choices
        """
        self.name = name
        self.choices = choices
        self.none_value = none_value

    def __call__(self, csv):
        """
        Splits the CSV list in to a regular python list. Note that csv must
        always contain at least one element. In the ArgumentParser there is
        always a default value associated with this type, for example.
        :param csv: comma separated list of choices
        :return: list of choices made
        """
        split = [item for item in csv.split(',')]
        invalid_choices = set(split) - set(self.choices)
        if invalid_choices:
            raise argparse.ArgumentError(
                argument=None,
                message="In {name}: {items} {isare} not valid".format(
                    name=self.name,
                    items=','.join(invalid_choices),
                    isare="is" if len(invalid_choices) == 1 else "are"))

        result = list(set(split))
        if self.none_value in result:
            # and "none" is the only item in list
            if len(result) == 1:
                result = []
            else:
                raise argparse.ArgumentError(
                    argument=None,
                    message="In {name}: either of comma separatet list or '{nv}', not both".format(
                        name=self.name,
                        nv=self.none_value))

        return result


class AragenArgs(object):
    """
    Allow to store the Aragen Args, do checks and run the corresponding
    flags.
    """
    def __init__(self, argv):
        """
        Build the argument parser and return the parsed command line
        """
        self._log = logging.getLogger(__name__)
        self.zeroList = []
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
            "--list-files",
            action="store_true",
            help="Print (to stdout) the file paths (relative to the argument of -o)\n"
                 "that would be generated, but do not generate anything")

        arg_parser.add_argument(
            "--list-machines",
            action="store_true",
            help="List the names of all machines defined in the input files and exit")

        arg_parser.add_argument(
            "--list-swc",
            action="store_true",
            help="List the names of all software components defined in input files and exit")

        arg_parser.add_argument(
            "--list-swp",
            action="store_true",
            help="List the names of all software package defined in input files and exit")

        arg_parser.add_argument(
            "--list-processes",
            action="store_true",
            help="List the names of all processes defined in input files and exit")

        arg_parser.add_argument(
            "--list-swcl-info",
            action="store_true",
            help="List the full info  of all swcl defined in input files and exit")

        arg_parser.add_argument(
            "--check-uuid",
            action="store_true",
            default=False,
            help="Check for duplicate uuid")

        arg_parser.add_argument(
            "-n",
            "--dry-run",
            action="store_true",
            default=False,
            help="Runs the generator in dry-run mode, i.e. run all the way\n"
                 "through without actually generating anything")

        arg_parser.add_argument(
            "--version",
            action="version",
            version="%(prog)s {}".format(__version__),
            help="List versions of the generator components and exit")

        arg_parser.add_argument(
            "-g",
            "--generate",
            default=DEFAULT_FOR_PROCESS,
            type=CsvChoices(name="generate", choices=POSSIBLE_FOR_PROCESS),
            help="What to generate for process. Options are {generators}; default is {default}".format(generators=",".
                                                                                           join(POSSIBLE_FOR_PROCESS),
                                                                                           default=DEFAULT_FOR_PROCESS))

        arg_parser.add_argument(
            "-l",
            "--log",
            help="Write all log data to the specified file")

        arg_parser.add_argument(
            "-m",
            "--machines",
            default=None,
            type=lambda csv: [machine for machine in csv.split(',')],
            help="Generate output only for machines whose FQNs\n"
                 "match the comma separated list; default is all machines")

        arg_parser.add_argument(
            "-s",
            "--software-components",
            default=None,
            type=lambda csv: [application for application in csv.split(',')],
            help="Generate output only for the software components whose FQNs\n"
                 "match the comma separated list; default is all software components")

        arg_parser.add_argument(
            "-p",
            "--processes",
            default=None,
            type=lambda csv: [process for process in csv.split(',')],
            help="Generate output only for the processes whose FQNs\n"
                 "match the comma separated list; default is all processes")
        arg_parser.add_argument(
            "-e",
            "--executables",
            default=None,
            type=lambda csv: [executable for executable in csv.split(',')],
            help="Generate output only for the executables whose FQNs\n"
                 "match the comma separated list")

        arg_parser.add_argument(
            "-P",
            "--softwarePackageFqn",
            default=None,
            type=lambda csv: [softwarePackage for softwarePackage in csv.split(',')],
            help="software-package-fqn")

        arg_parser.add_argument(
            "--software-cluster",
            default=None,
            type=lambda csv: [softwareCluster for softwareCluster in csv.split(',')],
            help="software-cluster")

        arg_parser.add_argument(
            "-V",
            "--vehiclePackageFqn",
            default=None,
            type=lambda csv: [vehiclePackage for vehiclePackage in csv.split(',')],
            help="vehicle-package-fqn")

        arg_parser.add_argument(
            "--someip-libs",
            type=CsvChoices(name="someip-libs", choices=POSSIBLE_SOMEIP_LIBS, none_value="none"),
            default=DEFAULT_SOMEIP_LIB,
            help="Generate SOMEIP network binding output for libraries in comma separated list.\n"
                 "Use --someip-libs=\"none\" when binding is not needed")

        arg_parser.add_argument(
            "--ipc-libs",
            type=CsvChoices(name="ipc-libs", choices=POSSIBLE_IPC_LIBS, none_value="none"),
            default=DEFAULT_IPC_LIB,
            help="Generate user defined network binding output for libraries in comma separated list.\n"
                 "Use --user-defined-libs=\"none\" when binding is not needed")

        arg_parser.add_argument(
            "--icc-libs",
            type=CsvChoices(name="icc-libs", choices=POSSIBLE_ICC_LIBS, none_value="none"),
            default=DEFAULT_ICC_LIB)

        arg_parser.add_argument(
            "--dds-libs",
            type=CsvChoices(name="dds-libs", choices=POSSIBLE_DDS_LIBS, none_value="none"),
            default=DEFAULT_DDS_LIB,
            help="Generate DDS network binding output for libraries in comma separated list.\n"
                 "Use --dds-libs=\"none\" when binding is not needed")

        arg_parser.add_argument(
            "-o",
            "--output",
            help="Output directory for the generator")

        arg_parser.add_argument(
            "files",
            nargs="+",
            help="A list of files that will be input for the generator,\n"
                 "can be either globs, individual files, or directories.\n"
                 "In the case that there is a path to a directory, all\n"
                 ".arxml files in the directory will be used as input.")

        arg_parser.add_argument(
            '--force-pkg-path-fallback',
            action='store_true',
            required=False,
            default=False,
            help='Force the fallback of using package name hierarchy'
                 'when namespaces (symbol-props) are not available')

        arg_parser.add_argument(
            "--no-reference-checking",
            action='store_true',
            required=False,
            default=False,
            help="Skip preliminary checks for references validity")

        arg_parser.add_argument(
            '--single-process',
            action='store_true',
            required=False,
            default=False,
            help="Single process for debug")

        arg_parser.add_argument(
            '--soa-element-cpp-def-style',
            required=False,
            default="")

        arg_parser.add_argument(
            '--qos-path',
            required=False,
            default=None,
            help="Path to the qos template")

        arguments = argv[1:]
        self._args = arg_parser.parse_args(arguments)

    def __getattr__(self, item):
        # pass through getters to underlying args object
        return getattr(self._args, item)

    # aliases for more clean naming
    @property
    def output_dir(self):
        """
        Return Output Directory of the Generator.
        :return: Output Directory Path
        """
        return self._args.output

    @property
    def software_package_fqn(self):
        return self._args.softwarePackageFqn

    @property
    def software_cluster_fqn(self):
        return self._args.software_cluster

    @property
    def vehicle_package_fqn(self):
        return self._args.vehiclePackageFqn


def getstate(self):
    return self._args


def setstate(self, state):
    self._args = state


AragenArgs.__getstate__ = getstate
AragenArgs.__setstate__ = setstate
