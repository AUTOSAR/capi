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
Configurator
"""
import configparser
import filecmp
import glob
import hashlib
import json
import logging
import os
import re
import shutil
import sys

from tools.araGenerator.araGenerator import AraGenerator
from tools.configMachineImpl.configurator.configurator_settings import (
    ConfiguratorSettings,
)
from tools.configMachineImpl.exception_handling import handle_method_exceptions

ARA_DIR_NAME = "ara"
CORE_SWCL_NAME = "core"
SWCLS_DIR_NAME = "swcls"
ETC_DIR_NAME = "etc"
BIN_DIR_NAME = "bin"

ARA_VER_ONE_CFG_FILE_NAME = "ara_ver1.json"
ARA_VER_ONE_MD5_FILE_NAME = "ara_ver1.md5"

PROCESSES_DIR_NAME_IN_WORKDIR = "processes"
MACHINES_DIR_NAME_IN_WORKDIR = "machines"
SOFTWARE_CLUSTERS_DIR_NAME_IN_WORKDIR = "software-clusters"
SWCL_MANIFEST_FILE_NAME = "swcl_manifest.json"
SWCL_STATUS_FILE_NAME = "swcl_status.json"

APPLICATION_SWCL_LIST_SECTION_NAME = "run_time_application_swcl_list"
APPLICATION_SWCL_LIST_VERSION_ONE_FILE_NAME = "run_time_application_swcl_list_ver1.json"
FRAMEWORK_CORE_LIST_FILE_NAME = "run_time_framework_core_list.json"
APPLICATION_SWCL_LIST_CFG_FILE_NAME = "run_time_application_swcl_list.json"
APPLICATION_SWCL_LIST_MD5_FILE_NAME = "run_time_application_swcl_list.md5"

MACHINES_ROOT_DIR_NAME = "machines"
MACHINE_MANIFEST_FILE_NAME = "machine_manifest.json"
PLATFORM_LEVEL_EXECUTABLE_CATEGORY = "PLATFORM_LEVEL"
PROCESS_NAME_IN_DICT = "Process"
ROOTSWC_NAME_IN_DICT = "RootSWC"

SM_ROOTSWC_NAME = "sm"  # name of RootSWC for state management
ROUTING_ROOTSWC_NAME = "nsomeip_routing"  # name of RootSWC for routing
NSOMEIP_ETC_FILE_NAME = "nsomeip.json"  # nsomeip configuration information
NPC_ETC_FILE_NAME = "npc.json"  # npc configuration information
LOG_ROOTSWC_NAME = "dlt_daemon"  # name of RootSWC for log


ARA_VER_CFG_BASE_SECTION_NAME = "base"
ARA_VER_CFG_UCM_SECTION_NAME = "ucm"
PLATFORM_FRAMEWORK_NAME = "platform_framework"
PLATFORM_CORE_NAME = "platform_core"
APPLICATION_SWCLS_NAME = "application_swcls"
PLATFORM_VAR_NAME = "platform_var"

FRAMEWORK_VERSION_NAME = "framework_version"
CORE_VERSION_NAME = "core_version"

OS_SWCL_NAME = "os"
FRAMEWORK_SWCL_NAME = "framework"
CORE_SWCL_NAME = "core"
APPLICATION_LAYER_SWCL_CATEGORY_NAME = "APPLICATION_LAYER"

ARA_LOADER_NAME = "ara_loader"
EXE_VERSION_NAME = "VERSION"
SDK_NAME_KEY_NAME = "SDK_NAME"


# FUNCTION_GROUP_SETS_FILE_NAME = "functiongroupsets.json"
FUNCTION_GROUP_SETS_FILE_NAME = "function_groups.json"
MODE_DECLARATION_GROUPS_FILE_NAME = "modedeclarationgroups.json"
SWCL_MANIFEST_FILE_NAME = "swcl_manifest.json"

SWCL_MANIFEST_CONTAINED_PROCESSES_KEY_NAME = "containedProcesses"
SWCL_MANIFEST_CONTAINED_EXECUTABLES_KEY_NAME = "containedExecutables"

ISOFT_FRAMEWORK_SWCL_CATEGORY_NAME = "ISOFT_FRAMEWORK"
FRAMEWORK_SWCL_FQN = "/ISOFT/Development/SoftwareClusters/framework"

SMD_EXECUTABLE_NAME = "smd"
SHELL_EXECUTABLE_NAME = "shell_request_client"
ARAGEN_EXECUTABLE_NAME = "aragen"
VALIDATION_EXECUTABLE_NAME = "validation"
DIAG_GW_NAME = "diagGW"
RESETMACHINE_EXECUTABLE_NAME = "ResetMachine"
SHUTDOWN_EXECUTABLE_NAME = "shutdown"
OS_SWCL_CATEGORY_NAME = "ISOFT_CORE_OS"
PLATFORM_CORE_SWCL_CATEGORY_NAME = "PLATFORM_CORE"
PLATFORM_SWCL_CATEGORY_NAME = "PLATFORM"


ARA_RUN_FILE_NAME = "are-cmd"

def md5(fname):
    hash_md5 = hashlib.md5()
    with open(fname, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

class Configurator:
    """
    Configurator for MachineManifest.
    """

    def __init__(self, args):
        self._settings = ConfiguratorSettings(args)
        self._log = logging.getLogger(__name__)

    @handle_method_exceptions(error_string_template="clear dir ERROR: {exception_text}")
    def _clear_dir(self, dir):
        self._log.debug("_clear_dir, begin with dir:%s", dir)
        if os.path.exists(dir):
            try:
                shutil.rmtree(dir)
                self._log.info("_clear_dir, dir:%s is removed successfully", dir)
            except FileNotFoundError:
                self._log.debug("_clear_dir, can't find dir:%s.", dir)
            except Exception as exception:
                self._log.error(
                    "_clear_dir, there is a exception:%s when trying to remove dir:%s",
                    exception.strerror,
                    dir,
                )
                sys.exit(
                    "error: failed to remove dir:{0} with exception:{1} when _clear_dir.\n".format(
                        dir, exception.strerror
                    )
                )

    @handle_method_exceptions(error_string_template="configure ERROR: {exception_text}")
    def configure(self):
        """
        Configures machine info files.
        """
        # return value
        ret_code = 0
        
        workdir = self._settings.workdir
        self._machineInfoDir = os.path.join(self._settings.workdir, "MachineInfo")
        self._log.debug(
            "configure, workdir:%s ara_sysroot:%s machine:%s files:%s self._machineInfoDir:%s",
            workdir,
            self._settings.ara_sysroot,
            self._settings.machine,
            self._settings.files,
            self._machineInfoDir
        )
        
        try:
            # ara directory
            self._AraDir = os.path.join(
                self._settings.ara_sysroot, ARA_DIR_NAME
            )
            self._log.info(
                "configure, self._AraDir:%s", self._AraDir
            )

            # 1 Determine framework software cluster, core software cluster, and all application-level software clusters swcls and var folder locations
            self.readAraDirConf()

            # Build mapping from ExecutableFQN to its path based on executable_build_dirs
            self.buildExecutableFQN2PathDict()
            
            # get aragen path
            self.getAraGenPath()        
            
            # 2 Get all software cluster information from arxml files
            self.getSWCLInfo()
            
            # Check whether os software cluster information in swclToIntegrateDict is correct
            if (OS_SWCL_NAME in self._swclToIntegrateDict) and self._swclToIntegrateDict[OS_SWCL_NAME][1] != OS_SWCL_CATEGORY_NAME:
                self._log.error("configure, the category of %s is not %s in self._swclToIntegrateDict", OS_SWCL_NAME, OS_SWCL_CATEGORY_NAME)
                sys.exit("error: the category of {0} is not {1} in self._swclToIntegrateDict when configure.\n".format(OS_SWCL_NAME, OS_SWCL_CATEGORY_NAME))
            
            # 3 Check whether framework software cluster information exists in swclToIntegrateDict; if not, report error; determine framework_version
            if FRAMEWORK_SWCL_NAME not in self._swclToIntegrateDict:
                self._log.error("configure, can't find:%s in self._swclToIntegrateDict", FRAMEWORK_SWCL_NAME)
                sys.exit("error: failed to find:{0} in self._swclToIntegrateDict when configure.\n".format(FRAMEWORK_SWCL_NAME))
            elif self._swclToIntegrateDict[FRAMEWORK_SWCL_NAME][1] != ISOFT_FRAMEWORK_SWCL_CATEGORY_NAME:
                self._log.error("configure, the category of %s is not %s in self._swclToIntegrateDict", FRAMEWORK_SWCL_NAME, ISOFT_FRAMEWORK_SWCL_CATEGORY_NAME)
                sys.exit("error: the category of {0} is not {1} in self._swclToIntegrateDict when configure.\n".format(FRAMEWORK_SWCL_NAME, ISOFT_FRAMEWORK_SWCL_CATEGORY_NAME))
            else:
                self._framework_version = self._swclToIntegrateDict[FRAMEWORK_SWCL_NAME][2]
                self._log.info("configure, got self._framework_version:%s", self._framework_version)
            
            # 4 Check whether core software cluster information exists in swclToIntegrateDict; if not, report error; determine core_version
            if CORE_SWCL_NAME not in self._swclToIntegrateDict:
                self._log.error("configure, can't find:%s in self._swclToIntegrateDict", CORE_SWCL_NAME)
                sys.exit("error: failed to find:{0} in self._swclToIntegrateDict when configure.\n".format(CORE_SWCL_NAME))
            elif self._swclToIntegrateDict[CORE_SWCL_NAME][1] != PLATFORM_CORE_SWCL_CATEGORY_NAME:
                self._log.error("configure, the category of %s is not %s in self._swclToIntegrateDict", CORE_SWCL_NAME, PLATFORM_CORE_SWCL_CATEGORY_NAME)
                sys.exit("error: the category of {0} is not {1} in self._swclToIntegrateDict when configure.\n".format(CORE_SWCL_NAME, PLATFORM_CORE_SWCL_CATEGORY_NAME))
            else:
                self._core_version = self._swclToIntegrateDict[CORE_SWCL_NAME][2]
                self._log.info("configure, got self._core_version:%s", self._core_version)

            # 5 Generate deployment information (json files) for all software cluster FQNs, all process FQNs, and target machine FQN involved in swclToIntegrateDict, e.g., software cluster description, function groups, process execution manifests, service instance manifests, machine manifests, to corresponding directories under self._manifestDir
            self.generateManifests()

            # 6 Configure all application-level software clusters (non-core & non-framework) and generate application-level software cluster list
            self.configApplicationLayerSWCLs()

            # 7 Configure framework & core software clusters and generate framework & core software cluster list run_time_framework_core_list.json
            self.configFrameworkCore()
            
            # 8 Check whether the exe corresponding to each process in each software cluster in swclToIntegrateDict is available (process may not have exe)
            self.checkAllExeAvailable()
            
            # 9 Configure boot guidance information
            self.configBootInfo()
            
            # 10 Copy files from sdk: are-cmd, build-log.json to self._settings.ara_sysroot
            if (self._settings.withSDK):
                self.copyAraRunAndBuildLog()
        except:      
            if (self._settings.withSDK):   
                # delete boot guidance information
                self.deleteAraBootInfo()
            
            # Raise the exception again.
            raise
        finally:    
            # 10 Based on keep-ara-gen-output, clear generated machine configuration information under self._machineInfoDir
            if not self._settings.keep_ara_gen_output:
                self.clearGeneratedMachineInfo()

        # return value
        return ret_code

    def clearGeneratedMachineInfo(self):
        """
        clear generated info.
        """
        # clear temporary directory
        self._log.debug("clearGeneratedMachineInfo, self._machineInfoDir:%s", self._machineInfoDir)
        if os.path.exists(self._machineInfoDir):
            self._clear_dir(self._machineInfoDir)

    # 1 Determine framework software cluster, core software cluster, and all application-level software clusters swcls and var folder locations
    def readAraDirConf(self):
        """
        read the dir config for the ara in ara_sysroot.
        """
        # According to the content of the original ara_verX.json (if any, take the largest X), get the configuration of framework software cluster, core software cluster, swcls software cluster, and var folder location
        list_of_ara_ver_cfg_files = sorted( filter( os.path.isfile, glob.glob(os.path.join(self._AraDir,'ara_ver*.json'))) )
        self._log.info("readAraDirConf, list_of_ara_ver_cfg_files:%s", list_of_ara_ver_cfg_files)
        
        # Read platform_framework, platform_core, application_swcls, platform_var
        self._platform_framework = ""
        self._platform_core = ""
        self._application_swcls = ""
        self._platform_var = ""
        if(len(list_of_ara_ver_cfg_files) > 0):
            # Take ara_verX.json with the largest X
            max_version_ara_ver_cfg_file = list_of_ara_ver_cfg_files[-1]
            self._log.debug("readAraDirConf, max_version_ara_ver_cfg_file:%s", max_version_ara_ver_cfg_file)
            
            # Parse ara_verX.json
            with open(max_version_ara_ver_cfg_file) as user_file:
                file_contents = user_file.read()
            config = json.loads(file_contents)
            self._log.debug("readAraDirConf, get config:%s", config)
            
            if ARA_VER_CFG_BASE_SECTION_NAME in config:
                # Read platform_framework
                if PLATFORM_FRAMEWORK_NAME in config[ARA_VER_CFG_BASE_SECTION_NAME]:
                    self._platform_framework = config[ARA_VER_CFG_BASE_SECTION_NAME][PLATFORM_FRAMEWORK_NAME]
                
                # Read platform_core
                if PLATFORM_CORE_NAME in config[ARA_VER_CFG_BASE_SECTION_NAME]:
                    self._platform_core = config[ARA_VER_CFG_BASE_SECTION_NAME][PLATFORM_CORE_NAME]
                
                # Read application_swcls
                if APPLICATION_SWCLS_NAME in config[ARA_VER_CFG_BASE_SECTION_NAME]:
                    self._application_swcls = config[ARA_VER_CFG_BASE_SECTION_NAME][APPLICATION_SWCLS_NAME]
                
                # Read platform_var
                if PLATFORM_VAR_NAME in config[ARA_VER_CFG_BASE_SECTION_NAME]:
                    self._platform_var = config[ARA_VER_CFG_BASE_SECTION_NAME][PLATFORM_VAR_NAME]

        self._log.debug(
            "readAraDirConf, read _platform_framework:%s _platform_core:%s _application_swcls:%s _platform_var:%s",
            self._platform_framework,
            self._platform_core,
            self._application_swcls,
            self._platform_var,
        )
        
        # If the directory of an existing framework software cluster is specified
        if self._settings.withSDK:    
            # Ignore the platform_framework configuration in ara_sysroot/ara/ara_verX.json;
            self._platform_framework = os.path.join(self._settings.withSDK, ARA_DIR_NAME)
            self._framework_dir = self._platform_framework
        else:
            if (len(self._platform_framework) == 0):
                self._framework_dir = self._AraDir
            else:
                self._framework_dir = self._platform_framework
        
        if (len(self._platform_core) == 0):
            self._core_dir = self._AraDir
        else:
            self._core_dir = self._platform_core

        if (len(self._application_swcls) == 0):
            self._swcls_dir = os.path.join(self._AraDir, "swcls")
        else:
            self._swcls_dir = os.path.join(self._application_swcls, "swcls")

        if (len(self._platform_var) == 0):
            self._var_dir = os.path.join(self._AraDir, "var")
        else:
            self._var_dir = os.path.join(self._platform_var, "var")

        self._log.debug(
            "readAraDirConf, got _platform_framework:%s _framework_dir:%s _core_dir:%s _swcls_dir:%s _var_dir:%s",
            self._platform_framework,
            self._framework_dir,
            self._core_dir,
            self._swcls_dir,
            self._var_dir,
        )
    

    # Build mapping from ExecutableFQN to its path based on executable_build_dirs
    def buildExecutableFQN2PathDict(self):
        """
        build the dict in which the key is ExecutableFQN and the value is its corresponding EXE 
        """
        sdk_full_name = "generic_x84_x64"

        # Path to sdk release file
        sdk_release_file_path = os.path.join(self._settings.ara_sysroot, ".release.json")
        if self._settings.withSDK:  # get release file from withSDK
            sdk_release_file_path = os.path.join(self._settings.withSDK, ".release.json")
            self._log.debug("buildExecutableFQN2PathDict, get sdk_release_file_path:%s from self._settings.withSDK:%s", sdk_release_file_path, self._settings.withSDK)
        if os.path.isfile(sdk_release_file_path):
            self._log.debug("buildExecutableFQN2PathDict, get sdk_release_file_path:%s", sdk_release_file_path)
            with open(sdk_release_file_path, "r", encoding='utf-8') as f:
                releaseOb = json.load(f)
                if SDK_NAME_KEY_NAME in releaseOb:
                    sdk_full_name = releaseOb[SDK_NAME_KEY_NAME]
        else:
            sdk_release_file_path = os.path.join(self._settings.ara_sysroot, "release")
            if self._settings.withSDK:  # get release file from withSDK
                sdk_release_file_path = os.path.join(self._settings.withSDK, "release")
                self._log.debug("buildExecutableFQN2PathDict, get sdk_release_file_path:%s from self._settings.withSDK:%s", sdk_release_file_path, self._settings.withSDK)
            if os.path.isfile(sdk_release_file_path):
                self._log.debug("buildExecutableFQN2PathDict, get sdk_release_file_path:%s", sdk_release_file_path)
                config = configparser.ConfigParser()
                config.read(sdk_release_file_path)
                if SDK_NAME_KEY_NAME in config["ara"]:
                    sdk_full_name = config["ara"][SDK_NAME_KEY_NAME]
                    sdk_full_name = sdk_full_name.strip("\"")

        self._log.info("buildExecutableFQN2PathDict, get sdk_full_name:%s", sdk_full_name)

        # Check validity of sdk_full_name
        if 0 == len(sdk_full_name):
           self._log.error("buildExecutableFQN2PathDict, sdk_full_name is empty.")
           sys.exit("error: sdk_full_name is empty when buildExecutableFQN2PathDict.\n")

        self._executableFQN2PathDict = {}
        # Iterate over all executable_build_dirs
        executable_build_dir_set = set(self._settings.executable_build_dirs)
        self._log.debug('buildExecutableFQN2PathDict, get executable_build_dir_set:%s', executable_build_dir_set)
        for executable_build_dir in executable_build_dir_set:
            self._log.debug('buildExecutableFQN2PathDict, get a executable_build_dir:%s', executable_build_dir)
            for root, directories, files in os.walk(executable_build_dir):
                for exeName in files:
                    # join the two strings in order to form the full filepath.
                    exePath = os.path.join(root, exeName)
                    self._log.debug("buildExecutableFQN2PathDict, get a exeName:%s exePath:%s", exeName, exePath)

                    # folder name
                    head_tail = os.path.split(root)
                    dir_name = head_tail[-1]
                    self._log.debug("buildExecutableFQN2PathDict, get a dir_name:%s", dir_name)

                    # Find an exe that meets the requirements
                    if dir_name == sdk_full_name:
                        self._log.debug("buildExecutableFQN2PathDict, get a legal exePath:%s", exePath)

                        # Parse out the parts that form ExecutableFQN
                        ExecutableFQN = root[len(executable_build_dir):-(len(dir_name)+1)]
                        self._log.debug("buildExecutableFQN2PathDict, get a ExecutableFQN:%s", ExecutableFQN)

                        # Whether it already exists
                        if ExecutableFQN in self._executableFQN2PathDict:
                            self._log.error("buildExecutableFQN2PathDict, ExecutableFQN:%s exists at both %s and %s.", ExecutableFQN,  self._executableFQN2PathDict[ExecutableFQN], exePath)
                            sys.exit("error: ExecutableFQN:{0} exists at both {1} and {2} when buildExecutableFQN2PathDict.\n".format(ExecutableFQN,  self._executableFQN2PathDict[ExecutableFQN], exePath))
                        else: # save path
                            self._executableFQN2PathDict[ExecutableFQN] = exePath
                            self._log.debug("buildExecutableFQN2PathDict, save ExecutableFQN:%s and exePath:%s", ExecutableFQN, exePath)
            
        self._log.debug('buildExecutableFQN2PathDict, end with self._executableFQN2PathDict:%s', self._executableFQN2PathDict)

    def getSWCLDir(self, swclName, swclCategory):
        """
        get the directory for the software cluster of the specified category
        """      
        self._log.debug("getSWCLDir, begin with swclName:%s, swclCategory:%s", swclName, swclCategory)
        swclDir = ""
        if (swclName == FRAMEWORK_SWCL_NAME):
            swclDir = os.path.join(self._framework_dir, swclName)
        elif (swclName == CORE_SWCL_NAME):
            swclDir = os.path.join(self._core_dir, swclName)
        else:
            swclDir = os.path.join(self._swcls_dir, swclName)  
        
        self._log.debug("getSWCLDir, end with swclDir:%s.", swclDir)
        return swclDir 

    # get aragen path
    def getAraGenPath(self):
        """
        get the path of the aragen
        """
        
        self._aragen_path = ""
        framework_swcl_dir = self.getSWCLDir(FRAMEWORK_SWCL_NAME, "ISOFT_FRAMEWORK");
        self._log.info("getAraGenPath, framework_swcl_dir:%s", framework_swcl_dir)
        
        try:
            # All version directories under core
            versions = [f.name for f in os.scandir(framework_swcl_dir) if f.is_dir()]
        except FileNotFoundError:
            self._log.error("getAraGenPath, can't find framework_swcl_dir:%s.", framework_swcl_dir)
            sys.exit(
                "error: failed to find framework_swcl_dir:{0} when getAraGenPath.\n".format(
                    framework_swcl_dir
                )
            )
        
        # Find the largest version
        # sort
        versions.sort(
            key=lambda s: list(map(int, (re.split(r"[-|+]+", s)[0]).split(".")))
        )

        # maximum version
        maxVersion = versions[-1]
        self._log.info("getAraGenPath we get maxVersion:%s", maxVersion)

        self._aragen_path = os.path.join(framework_swcl_dir, maxVersion, "bin", ARAGEN_EXECUTABLE_NAME)
        self._log.info("getAraGenPath, self._aragen_path:%s", self._aragen_path)
        
        # Determine whether ara_gen_path is valid
        if not os.path.exists(self._aragen_path):
            self._log.error("getAraGenPath, Can't find self._aragen_path:%s, so we will exit.", self._aragen_path)
            sys.exit("error: Can't find self._aragen_path:{}, so we will exit..\n".format(self._aragen_path))

    # 2 Get all software cluster information from arxml files
    #     /home/hanzhibo/ap-all/isoft/ara-gen/aragen -o /home/hanzhibo/ap-all/tmp --list-swcl-info  /home/hanzhibo/ap-all/isoft/arxmls/models
    #     Return changed to:
    #       swcl FQN: swcl category: swcl version: swcl dependsOn (including recursive): process FQN: executable FQN corresponding to process: RootSWC FQN of executable: machine FQN
    #       swcl FQN: swcl category: swcl version: swcl dependsOn (including recursive)::::     --- even if software cluster has no process, still return a separate line
    #         Used for: determine software cluster on target machine via process; get software cluster FQN and process FQN for later batch generation of all related files;
    #              Through RootSWC special handling of machine-level configuration files; used to check whether exe corresponding to each process in swclToIntegrateDict is available
    def getSWCLInfo(self):
        """
        get swcl info for this machine.
        """
        workdir = os.path.join(self._settings.workdir, "getSWCLInfo")
        self._log.debug(
            "getSWCLInfo, workdir:%s ara_sysroot:%s machine:%s files:%s",
            workdir,
            self._settings.ara_sysroot,
            self._settings.machine,
            self._settings.files,
        )
        # First clear temporary directory
        self._clear_dir(workdir)
        
        legalSWCLCategorySet = (OS_SWCL_CATEGORY_NAME, ISOFT_FRAMEWORK_SWCL_CATEGORY_NAME, PLATFORM_CORE_SWCL_CATEGORY_NAME, APPLICATION_LAYER_SWCL_CATEGORY_NAME)
        self._log.debug("getSWCLInfo, legalSWCLCategorySet:%s", legalSWCLCategorySet)

        # Find all software clusters
        gen = AraGenerator(
            self._settings._args,
            ["-o", workdir, "--list-swcl-info", *(self._settings.files)],
        )
        out = gen.generate(self._aragen_path)

        # return result
        lines = out.split("\n")

        # clear temporary directory
        if not self._settings.keep_ara_gen_output:
            self._clear_dir(workdir)
        
        # Get software cluster information on target machine (including software clusters not belonging to any machine, e.g., framework) (may have multiple versions):
        # swclDict mapping software cluster name to its information (may have multiple versions); software cluster information: {software cluster version, (software cluster FQN, software cluster category, software cluster DependsOn, [(process FQN, executable FQN corresponding to process, RootSWC name of executable)])}
        swclOnTargetMachineDict = {}
        
        # List of software clusters without specified target machine (e.g., framework): unspecifiedMachineSWCLList:
        #   [(software cluster FQN, software cluster category, software cluster version, software cluster DependsOn)]
        unspecifiedMachineSWCLList = []
        
        # Dict mapping software cluster to machine (empty string means no target machine), used for [constr_1536]: Definition of SoftwareCluster applies for a single Machine
        #   dWithin the scope of a SoftwareCluster, each Process referenced in the
        #   role containedProcess shall be mapped (e.g. by means of the existence of a ProcessToMachineMapping) to the same Machine.c()
        swcl2MachinesDict = {}
        
        # Also build depentSWCL2MachineDict: {SWCLFQN, set(machine FQN)} for dependent software clusters
        depentSWCL2MachineDict = {}
        for line in lines:
            self._log.debug("getSWCLInfo, get a line:%s", line)
            if len(line) != 0:
                fields = line.split(":")
                if len(fields) != 8:
                    self._log.error("getSWCLInfo, the line:%s is illegal", line)
                    sys.exit(
                        "error: get a illegal line:{0} when getSWCLInfo.\n".format(
                            line
                        )
                    )

                # Parse: swcl FQN: swcl category: swcl version: swcl dependsOn (including recursive): process FQN: executable FQN corresponding to process: RootSWC FQN of executable: machine FQN
                SWCLFQN = fields[0]
                SWCLCategory = fields[1]
                SWCLVersion = fields[2]
                if(0 == len(fields[3])):
                    SWCLDependsOn = []
                else:
                    SWCLDependsOn = fields[3].split(',')
                ProcessFQN = fields[4]
                ExecutableFQN = fields[5]
                ExecutableRootSWCFQN = fields[6]
                machineFQN = fields[7]

                self._log.debug(
                    "getSWCLInfo, get a SWCLFQN:%s, SWCLCategory:%s, SWCLVersion:%s, SWCLDependsOn:%s, ProcessFQN:%s, ExecutableFQN:%s, ExecutableRootSWCFQN:%s, machineFQN:%s",
                    SWCLFQN,
                    SWCLCategory,
                    SWCLVersion,
                    SWCLDependsOn,
                    ProcessFQN,
                    ExecutableFQN,
                    ExecutableRootSWCFQN,
                    machineFQN,
                )
                
                # If it is a process, check whether the software cluster to which it belongs contains multiple machines
                if 0 != len(ProcessFQN):
                    # If machineFQN is empty, report error
                    if 0 == len(machineFQN):
                        self._log.error("getSWCLInfo, within the scope of the SoftwareCluster:%s, each Process referenced in the role containedProcess shall be mapped to the same Machine, but now machineFQN:%s for ProcessFQN:%s.", SWCLFQN, machineFQN, ProcessFQN)
                        sys.exit(
                            "error: within the scope of the SoftwareCluster:{0}, each Process referenced in the role containedProcess shall be mapped to the same Machine, but now machineFQN:{1} for ProcessFQN:{2} when getSWCLInfo.\n".format(
                                SWCLFQN, machineFQN, ProcessFQN
                            )
                        )
                    if SWCLFQN not in swcl2MachinesDict:
                        swcl2MachinesDict[SWCLFQN] = set()
                    swcl2MachinesDict[SWCLFQN].add(machineFQN)
                    if len(swcl2MachinesDict[SWCLFQN]) > 1:
                        self._log.error("getSWCLInfo, within the scope of the SoftwareCluster:%s, each Process referenced in the role containedProcess shall be mapped to the same Machine, but now swcl2MachinesDict[SWCLFQN]:%s.", SWCLFQN, swcl2MachinesDict[SWCLFQN])
                        sys.exit(
                            "error: within the scope of the SoftwareCluster:{0}, each Process referenced in the role containedProcess shall be mapped to the same Machine, but now swcl2MachinesDict[SWCLFQN]:{1} when getSWCLInfo.\n".format(
                                SWCLFQN, swcl2MachinesDict[SWCLFQN]
                            )
                        )
                
                # Determine validity of software cluster category
                if SWCLCategory not in legalSWCLCategorySet:
                    self._log.error("getSWCLInfo, the SWCLCategory:%s is illegal for SWCLFQN:%s", SWCLCategory, SWCLFQN)
                    sys.exit(
                        "error: get a illegal SWCLCategory:{0} for for SWCLFQN{1}: when getSWCLInfo.\n".format(
                            SWCLCategory, SWCLFQN
                        )
                    )

                # Whether it belongs to the target machine or no machine (e.g., framework software cluster)
                if (machineFQN == self._settings.machine):
                    # software cluster name
                    SWCLName = re.split(r"[/]+", SWCLFQN)[-1]

                    # ExecutableRootSWC name
                    ExecutableRootSWCName = re.split(r"[/]+", ExecutableRootSWCFQN)[-1]

                    # Whether software cluster name already exists
                    if SWCLName not in swclOnTargetMachineDict:
                        swclOnTargetMachineDict[SWCLName] = {}

                    # Information corresponding to software cluster (may have multiple versions)
                    swclVersionDict = swclOnTargetMachineDict[SWCLName]

                    # Whether the specified version of software cluster already exists
                    if SWCLVersion not in swclVersionDict:
                        swclVersionDict[SWCLVersion] = (SWCLFQN, SWCLCategory, SWCLDependsOn, [])

                    # Information corresponding to the specified version of software cluster
                    swclInfo = swclVersionDict[SWCLVersion]

                    # Skip if no process information
                    if 0 == len(ProcessFQN):
                        continue
                    
                    # process information
                    processInfo = (
                        ProcessFQN,
                        ExecutableFQN,
                        ExecutableRootSWCName,
                    )

                    # process list
                    swclInfo[3].append(processInfo)
                    
                    self._log.debug(
                        "getSWCLInfo, save a processInfo:%s for SWCLName:%s SWCLVersion:%s",
                        processInfo,
                        SWCLName,
                        SWCLVersion,
                    )
                    
                    # depentSWCL2MachineDict = {}
                    # {SWCLFQN, set(machine FQN)}
                    for depentSWCL in SWCLDependsOn:
                        if depentSWCL not in depentSWCL2MachineDict:
                            depentSWCL2MachineDict[depentSWCL] = set()
                        depentSWCL2MachineDict[depentSWCL].add(machineFQN)
                        self._log.debug(
                            "getSWCLInfo, we save a depentSWCL:%s, machineFQN:%s to depentSWCL2MachineDict",
                            depentSWCL,
                            machineFQN
                        )
                elif (0 == len(machineFQN)): # not belonging to any machine
                    #[(software cluster FQN, software cluster category, software cluster version, software cluster DependsOn)]
                    unspecifiedMachineSWCLList.append((SWCLFQN, SWCLCategory, SWCLVersion, SWCLDependsOn)) 
                    self._log.debug(
                        "getSWCLInfo, we save a SWCLFQN:%s, SWCLCategory:%s, SWCLVersion:%s, SWCLDependsOn:%s to unspecifiedMachineSWCLList",
                        SWCLFQN,
                        SWCLCategory,
                        SWCLVersion,
                        SWCLDependsOn
                    )
        self._log.debug(
            "getSWCLInfo, swclOnTargetMachineDict:%s unspecifiedMachineSWCLList:%s depentSWCL2MachineDict:%s",
            swclOnTargetMachineDict,
            unspecifiedMachineSWCLList,
            depentSWCL2MachineDict,
        )
        
        # Iterate over unspecifiedMachineSWCLList, for each unspecifiedMachineSWCL:
        #   Check whether the software cluster FQN of unspecifiedMachineSWCL has the target machine in the value of depentSWCL2MachineDict;
        #   If yes, add unspecifiedMachineSWCL to swclDict
        for unspecifiedMachineSWCL in unspecifiedMachineSWCLList:
            self._log.debug(
                "getSWCLInfo, get a unspecifiedMachineSWCL:%s", unspecifiedMachineSWCL
            )
            
            unspecifiedMachineSWCLFQN = unspecifiedMachineSWCL[0]
            unspecifiedMachineSWCLCategory = unspecifiedMachineSWCL[1]
            unspecifiedMachineSWCLVersion = unspecifiedMachineSWCL[2]
            unspecifiedMachineSWCLDependsOn = unspecifiedMachineSWCL[3]
            
            # software cluster name
            unspecifiedMachineSWCLName = re.split(r"[/]+", unspecifiedMachineSWCLFQN)[-1]
            self._log.debug(
                "getSWCLInfo, get unspecifiedMachineSWCLName:%s unspecifiedMachineSWCLFQN:%s unspecifiedMachineSWCLCategory:%s unspecifiedMachineSWCLVersion:%s unspecifiedMachineSWCLDependsOn:%s",
                unspecifiedMachineSWCLName, unspecifiedMachineSWCLFQN, unspecifiedMachineSWCLCategory, unspecifiedMachineSWCLVersion, unspecifiedMachineSWCLDependsOn
            )
      
            # Whether software cluster name already exists
            if unspecifiedMachineSWCLName not in swclOnTargetMachineDict:
                swclOnTargetMachineDict[unspecifiedMachineSWCLName] = {}
                
            # Information corresponding to software cluster (may have multiple versions)
            swclVersionDict = swclOnTargetMachineDict[unspecifiedMachineSWCLName]

            # Whether the specified version of software cluster already exists
            if (unspecifiedMachineSWCLVersion not in swclVersionDict) or (unspecifiedMachineSWCLFQN in depentSWCL2MachineDict): # The software cluster without specified machine is a dependent software cluster on the target machine
                swclVersionDict[unspecifiedMachineSWCLVersion] = (unspecifiedMachineSWCLFQN, unspecifiedMachineSWCLCategory, unspecifiedMachineSWCLDependsOn, []) 
            elif unspecifiedMachineSWCLName == FRAMEWORK_SWCL_NAME:
                self._log.debug(
                    "getSWCLInfo, unspecifiedMachineSWCLVersion:%s already exists in swclOnTargetMachineDict:%s for unspecifiedMachineSWCLFQN:%s", unspecifiedMachineSWCLVersion, swclOnTargetMachineDict,unspecifiedMachineSWCLFQN
                ) 
            else:
                self._log.warn(
                    "getSWCLInfo, unspecifiedMachineSWCLVersion:%s already exists in swclOnTargetMachineDict:%s for unspecifiedMachineSWCLFQN:%s", unspecifiedMachineSWCLVersion, swclOnTargetMachineDict,unspecifiedMachineSWCLFQN
                )        

        self._log.debug(
            "getSWCLInfo, get new swclOnTargetMachineDict:%s", swclOnTargetMachineDict
        )

        
        #     Get swclToIntegrateDict of software clusters to be integrated on target machine: {software cluster name, (software cluster FQN, software cluster category, software cluster version, software cluster DependsOn, [(process FQN, executable FQN corresponding to process)])}
        #       If there are multiple versions of a software cluster, the higher version takes effect.
        #     Also get rootSwc2ProcessDictInCore in the core software cluster to be integrated: {RootSWC name of executable corresponding to process, [process name]}
        self._swclToIntegrateDict = {}
        self._rootSwc2ProcessDictInCore = {}
        for swclName, swclVersionDict in swclOnTargetMachineDict.items():
            self._log.debug(
                "getSWCLInfo, get a swclName:%s swclVersionDict:%s",
                swclName,
                swclVersionDict,
            )

            # software cluster version information
            versions = list(swclVersionDict.keys())

            # sort
            versions.sort(
                key=lambda s: list(map(int, (re.split(r"[-|+]+", s)[0]).split(".")))
            )

            # maximum version
            maxVersion = versions[-1]
            self._log.info(
                "getSWCLInfo, we get maxVersion:%s for swclName:%s",
                maxVersion,
                swclName,
            )

            # software cluster version information
            swclMaxVersionInfo = swclVersionDict[maxVersion]
    
            # Whether the specified version of software cluster already exists
            if swclName not in self._swclToIntegrateDict:
                self._swclToIntegrateDict[swclName] = (swclMaxVersionInfo[0], swclMaxVersionInfo[1], maxVersion, swclMaxVersionInfo[2], [])

            # process list
            for processInfo in swclMaxVersionInfo[3]:
                
                # process FQN,
                ProcessFQN = processInfo[0]
                
                # executable FQN corresponding to process,
                ExecutableFQN = processInfo[1]
                
                # RootSWC name of executable
                ExecutableRootSWCName = processInfo[2]
                self._swclToIntegrateDict[swclName][4].append((ProcessFQN, ExecutableFQN))
                
                # Whether it is a core software cluster
                if swclName == CORE_SWCL_NAME:
                    if ExecutableRootSWCName not in self._rootSwc2ProcessDictInCore:
                        self._rootSwc2ProcessDictInCore[ExecutableRootSWCName] = []

                    # process name
                    ProcessName = re.split(r"[/]+", ProcessFQN)[-1]
                    self._rootSwc2ProcessDictInCore[ExecutableRootSWCName].append(ProcessName)

        self._log.debug("getSWCLInfo, get self._swclToIntegrateDict:%s self._rootSwc2ProcessDictInCore:%s", self._swclToIntegrateDict, self._rootSwc2ProcessDictInCore)
        
        # Number of software clusters of categories OS_SWCL_CATEGORY_NAME, ISOFT_FRAMEWORK_SWCL_CATEGORY_NAME, PLATFORM_CORE_SWCL_CATEGORY_NAME
        isoft_os_category_list = []
        isoft_framework_category_list = []
        platform_core_category_list = []
        # Replace the FQN of framework software cluster in swclDependsOn of software clusters
        for swclName, swclVersionInfo in self._swclToIntegrateDict.items():
            swclFQN = swclVersionInfo[0]
            swclCategory = swclVersionInfo[1]
            if swclCategory == OS_SWCL_CATEGORY_NAME:
                isoft_os_category_list.append(swclFQN)
            elif swclCategory == ISOFT_FRAMEWORK_SWCL_CATEGORY_NAME:
                isoft_framework_category_list.append(swclFQN)
            elif swclCategory == PLATFORM_CORE_SWCL_CATEGORY_NAME:
                platform_core_category_list.append(swclFQN)
            swclDependsOn = swclVersionInfo[3]
            for index in range(len(swclDependsOn)):
                depentSWCLFQN = swclDependsOn[index]
                depentSWCLName = re.split(r"[/]+", depentSWCLFQN)[-1]
                self._log.debug("getSWCLInfo, get a depentSWCLName:%s depentSWCLFQN:%s for swclFQN:%s ", depentSWCLName, depentSWCLFQN, swclFQN)
                if (depentSWCLName in self._swclToIntegrateDict):
                    depentSWCLCategory = self._swclToIntegrateDict[depentSWCLName][1]
                    self._log.debug("getSWCLInfo, get a depentSWCLCategory:%s", depentSWCLCategory)
                    if (depentSWCLCategory == ISOFT_FRAMEWORK_SWCL_CATEGORY_NAME):
                        swclDependsOn[index] = FRAMEWORK_SWCL_FQN
                        self._log.debug("getSWCLInfo, we replace with swclDependsOn[index]:%s for a dependsOn of swclFQN:%s in self._swclToIntegrateDict", swclDependsOn[index], swclFQN)

        # Whether the name of the software cluster of category OS_SWCL_CATEGORY_NAME is os
        for os_swcl_FQN in isoft_os_category_list: 
            # os software cluster name
            os_swcl_name = re.split(r"[/]+", os_swcl_FQN)[-1]
            self._log.debug("getSWCLInfo, get a os_swcl_name:%s for os_swcl_FQN:%s ", os_swcl_name, os_swcl_FQN)
            if os_swcl_name != OS_SWCL_NAME:
                self._log.error("getSWCLInfo, the os_swcl_name:%s of os_swcl_FQN:%s which category is %s is not %s", os_swcl_name, os_swcl_FQN, OS_SWCL_CATEGORY_NAME, OS_SWCL_NAME)
                sys.exit("error: the os_swcl_name:{0} of os_swcl_FQN:{1} which category is {2} is not {3} when getSWCLInfo.\n".format(os_swcl_name, os_swcl_FQN, OS_SWCL_CATEGORY_NAME, OS_SWCL_NAME))
                
        if(len(isoft_os_category_list) > 1):
            self._log.error(
                "getSWCLInfo, there should be at most one swcl whose category is %s, but now isoft_os_category_list is %s.",
                OS_SWCL_CATEGORY_NAME, isoft_os_category_list
            )
            sys.exit(
                "error: there should be exactly one swcl whose category is {0}, but now isoft_os_category_list is {1} when getSWCLInfo.\n".format(
                OS_SWCL_CATEGORY_NAME, isoft_os_category_list
                )
            )
           
        if(len(isoft_framework_category_list) != 1):
            self._log.error(
                "getSWCLInfo, there should be exactly one swcl whose category is %s, but now isoft_framework_category_list is %s.",
                ISOFT_FRAMEWORK_SWCL_CATEGORY_NAME, isoft_framework_category_list
            )
            sys.exit(
                "error: there should be exactly one swcl whose category is {0}, but now isoft_framework_category_list is {1} when getSWCLInfo.\n".format(
                ISOFT_FRAMEWORK_SWCL_CATEGORY_NAME, isoft_framework_category_list
                )
            )
        
        if(len(platform_core_category_list) != 1):
            self._log.error(
                "getSWCLInfo, there should be exactly one swcl whose category is %s, but now platform_core_category_list is %s.",
                PLATFORM_CORE_SWCL_CATEGORY_NAME, platform_core_category_list
            )
            sys.exit(
                "error: there should be exactly one swcl whose category is {0}, but now platform_core_category_list is {1} when getSWCLInfo.\n".format(
                PLATFORM_CORE_SWCL_CATEGORY_NAME, platform_core_category_list
                )
            )
        
        self._log.debug("getSWCLInfo, get new self._swclToIntegrateDict:%s", self._swclToIntegrateDict)


    # 5 Generate deployment information (json files) for all software cluster FQNs, all process FQNs, and target machine FQN involved in swclToIntegrateDict, e.g., software cluster description, function groups, process execution manifests, service instance manifests, machine manifests, to corresponding directories under self._manifestDir
    def generateManifests(self):
        """
        generate all manifests.
        """

        self._log.debug("generateManifests, machine:%s", self._settings.machine)

        # Clear machine information directory
        self._clear_dir(self._machineInfoDir)
        
        # Mapping from process to software cluster, used to check if the same process is included multiple times
        process2SWCLDict = {}
        
        # swclToIntegrateDict: {software cluster name, (software cluster FQN, software cluster category, software cluster version, [(process FQN, executable FQN corresponding to process)])}
        # Iterate over _swclToIntegrateDict to get all process information and all software cluster FQN information
        allProcessFQNs = ""
        allSwclFQNs = ""
        for swclName, swclVersionInfo in self._swclToIntegrateDict.items():
            self._log.debug(
                "generateManifests, get a swclName:%s swclVersionInfo:%s",
                swclName,
                swclVersionInfo,
            )

            # software cluster FQN
            swclFQN = swclVersionInfo[0]
            if len(allSwclFQNs) != 0:
                allSwclFQNs += ","
            allSwclFQNs += swclFQN
            self._log.info("generateManifests, we save a swclFQN:%s", swclFQN)

            # process FQN information
            for processInfo in swclVersionInfo[4]:
                ProcessFQN = processInfo[0]
                
                # Whether process already exists
                if ProcessFQN not in process2SWCLDict:
                    process2SWCLDict[ProcessFQN] = swclFQN
                else:
                    self._log.error(
                        "getSWCLInfo, can't include ProcessFQN:%s multiple times in swcls:%s,%s.",
                        ProcessFQN, process2SWCLDict[ProcessFQN], swclFQN
                    )
                    sys.exit(
                        "error: can't include ProcessFQN:{0} multiple times in swcls:{1},{2} when getSWCLInfo.\n".format(
                        ProcessFQN, process2SWCLDict[ProcessFQN], swclFQN
                        )
                    )
                
                if len(allProcessFQNs) != 0:
                    allProcessFQNs += ","
                allProcessFQNs += ProcessFQN
                self._log.info(
                    "generateManifests, we save a ProcessFQN:%s", ProcessFQN
                )

        self._log.debug(
            "generateManifests, get allSwclFQNs:%s allProcessFQNs:%s",
            allSwclFQNs,
            allProcessFQNs,
        )

        # optional parameters
        optionalArgs = []
        if (self._settings.qos_path):
            optionalArgs = ["--qos-path", self._settings.qos_path]
        
        self._log.debug("generateManifests, get optionalArgs:%s", optionalArgs)
        # Generate machine configuration
        gen = AraGenerator(
            self._settings._args,
            optionalArgs + [
                "-m",
                self._settings.machine,
                "-o",
                self._machineInfoDir,
                "-p",
                allProcessFQNs,
                "--software-cluster",
                allSwclFQNs,
                "--someip-libs=nsomeip2",
                "--ipc-libs=npc",
                "--icc-libs=icc",
                *(self._settings.files),
            ],
        )
        gen.generate(self._aragen_path)

    # 6 Configure all application-level software clusters (non-core & non-framework) and generate application-level software cluster list
    def configApplicationLayerSWCLs(self):
        """
        configure all swcls which type is application layer.
        """
        self._log.debug("configApplicationLayerSWCLs, begin with self._swcls_dir:%s and try to cleanDir it", self._swcls_dir)

        # 6.1 Clean up directories under application_swcls that do not exist in swclToIntegrateDict (WARN log)
        self.cleanDir(self._swcls_dir, lambda entryName:(entryName in self._swclToIntegrateDict))

        # 6.2 For each application-level software cluster to be integrated in swclToIntegrateDict: swclName, swclFQN, swclCategory, swclVersion, call IntegrateSWCL and CollectSWCLInfo;
        applicationLayerSwclList = []
        applicationLayerFunctionGroupList = []
        self._swclContextDict = {}
        for swclName, swclVersionInfo in self._swclToIntegrateDict.items():
            swclFQN = swclVersionInfo[0]
            swclCategory = swclVersionInfo[1]
            swclVersion = swclVersionInfo[2]
            swclDependsOn = swclVersionInfo[3]
            self._log.debug(
                "configApplicationLayerSWCLs, get a swclName:%s swclFQN:%s swclCategory:%s swclVersion:%s swclDependsOn:%s",
                swclName,
                swclFQN,
                swclCategory,
                swclVersion,
                swclDependsOn
            )

            # application software cluster
            if swclCategory == APPLICATION_LAYER_SWCL_CATEGORY_NAME or swclCategory == OS_SWCL_CATEGORY_NAME: 
                # integrate software cluster
                self.IntegrateSWCL(swclName, swclFQN, swclCategory, swclVersion)

                # collect software cluster information
                self.CollectSWCLInfo(swclName, swclFQN, swclCategory, swclVersion, swclDependsOn, applicationLayerSwclList, applicationLayerFunctionGroupList, self._swclContextDict)

        #   6.3 Use the swclList and functionGroupList obtained in the previous step to generate application-level software cluster list run_time_application_swcl_list_ver1.json to the specified configuration location;
        self.generateApplicationLayerSwclList(applicationLayerSwclList, applicationLayerFunctionGroupList)
        self._log.debug("configApplicationLayerSWCLs, end.")


    # 7 Configure framework & core software clusters and generate framework & core software cluster list run_time_framework_core_list.json
    def configFrameworkCore(self):
        """
        configure framework and core swcl.
        """
        self._log.debug("configFrameworkCore, begin.")

        framework_core_list = []
        framework_core_functionGroupList = []

        framework_swclFQN = self._swclToIntegrateDict[FRAMEWORK_SWCL_NAME][0]
        framework_swclCategory = self._swclToIntegrateDict[FRAMEWORK_SWCL_NAME][1]
        framework_swclVersion = self._swclToIntegrateDict[FRAMEWORK_SWCL_NAME][2]
        framework_swclDependsOn = self._swclToIntegrateDict[FRAMEWORK_SWCL_NAME][3]
        self._log.debug(
            "configFrameworkCore, get a swclName:%s framework_swclFQN:%s framework_swclCategory:%s framework_swclVersion:%s framework_swclDependsOn:%s",
            FRAMEWORK_SWCL_NAME,
            framework_swclFQN,
            framework_swclCategory,
            framework_swclVersion,
            framework_swclDependsOn
        )

        # Call IntegrateSWCL for framework software cluster
        if not (self._settings.withSDK):
            self.IntegrateSWCL(FRAMEWORK_SWCL_NAME, framework_swclFQN, framework_swclCategory, framework_swclVersion)
        else: # if withSDK
            self._log.debug("configFrameworkCore, get self._settings.ara_sysroot:%s, self._settings.withSDK:%s", self._settings.ara_sysroot, self._settings.withSDK)
            if self._settings.ara_sysroot != self._settings.withSDK:
                # framework software cluster directory under ara
                frameworkInAra = os.path.join(
                    self._AraDir, FRAMEWORK_SWCL_NAME
                )
                
                # Clear frameworkInAra directory
                self._log.debug("configFrameworkCore, try to clear frameworkInAra:%s.", frameworkInAra)
                self._clear_dir(frameworkInAra)

        # Collect software cluster information
        # Call CollectSWCLInfo for framework software cluster // Do not configure framework software cluster, but need to check whether platform_framework directory contains the framework software cluster (version) defined in the model;
        self.CollectSWCLInfo(FRAMEWORK_SWCL_NAME, framework_swclFQN, framework_swclCategory, framework_swclVersion, framework_swclDependsOn, framework_core_list, framework_core_functionGroupList, self._swclContextDict)

        core_swclFQN = self._swclToIntegrateDict[CORE_SWCL_NAME][0]
        core_swclCategory = self._swclToIntegrateDict[CORE_SWCL_NAME][1]
        core_swclVersion = self._swclToIntegrateDict[CORE_SWCL_NAME][2]
        core_swclDependsOn = self._swclToIntegrateDict[CORE_SWCL_NAME][3]
        self._log.debug(
            "configFrameworkCore, get a swclName:%s core_swclFQN:%s core_swclCategory:%s core_swclVersion:%s core_swclDependsOn:%s",
            CORE_SWCL_NAME,
            core_swclFQN,
            core_swclCategory,
            core_swclVersion,
            core_swclDependsOn
        )

        #   Call IntegrateSWCL and CollectSWCLInfo for core software cluster
        self.IntegrateSWCL(CORE_SWCL_NAME, core_swclFQN, core_swclCategory, core_swclVersion)
        self.CollectSWCLInfo(CORE_SWCL_NAME, core_swclFQN, core_swclCategory, core_swclVersion, core_swclDependsOn, framework_core_list, framework_core_functionGroupList, self._swclContextDict)

        #   Use the obtained framework_core_list and functionGroupList to generate framework & core software cluster list run_time_framework_core_list.json under the etc of the core software cluster;
        self.generateFrameworkCoreSwclList(framework_core_list, framework_core_functionGroupList)

        #   Link the run_time_application_swcl_list_ver1.json generated in step 6 to the etc directory of the core software cluster
        self.LinkApplicationLayerSWCLList2Core()
        self._log.debug("configFrameworkCore, end.")    


    def generateFrameworkCoreSwclList(self, framework_core_list, framework_core_functionGroupList):
        """
        generate framework&core swcl list.
        """
        self._log.debug("generateFrameworkCoreSwclList, begin with framework_core_list:%s framework_core_functionGroupList:%s", framework_core_list, framework_core_functionGroupList)

        core_swclFQN = self._swclToIntegrateDict[CORE_SWCL_NAME][0]
        core_swclCategory = self._swclToIntegrateDict[CORE_SWCL_NAME][1]
        core_swclVersion = self._swclToIntegrateDict[CORE_SWCL_NAME][2]
        self._log.debug(
            "generateFrameworkCoreSwclList, get a swclName:%s core_swclFQN:%s core_swclCategory:%s core_swclVersion:%s",
            CORE_SWCL_NAME,
            core_swclFQN,
            core_swclCategory,
            core_swclVersion
        )

        core_swclDir = self.getSWCLDir(CORE_SWCL_NAME, core_swclCategory)
        core_swclVersionDir = os.path.join(core_swclDir, core_swclVersion)
        core_swclETCDir = os.path.join(core_swclVersionDir, ETC_DIR_NAME)
        self._log.debug("generateFrameworkCoreSwclList, got core_swclETCDir:%s", core_swclETCDir)

        # framework & core software cluster list path
        framework_core_listPath = os.path.join(
            core_swclETCDir, FRAMEWORK_CORE_LIST_FILE_NAME
        )
        self._log.debug(
            "generateFrameworkCoreSwclList, framework_core_listPath:%s", framework_core_listPath
        )
        
        # Generate new software cluster list
        self.generateSWCLList(framework_core_list, framework_core_functionGroupList, framework_core_listPath)


    def generateApplicationLayerSwclList(self, applicationLayerSwclList, applicationLayerFunctionGroupList):
        """
        generate application layer swcl list.
        """
        self._log.debug(
            "generateApplicationLayerSwclList, self._swcls_dir:%s",
            self._swcls_dir,
        )
        self._log.debug("generateApplicationLayerSwclList, applicationLayerSwclList:%s applicationLayerFunctionGroupList:%s", applicationLayerSwclList, applicationLayerFunctionGroupList)

        # Create directory if it does not exist
        if not os.path.exists(self._swcls_dir):
            self._log.debug(
                "generateApplicationLayerSwclList, try to makedirs applicationLayerSwclListDir: %s",
                self._swcls_dir,
            )
            os.makedirs(self._swcls_dir, exist_ok=True)

        # application-level software cluster list path
        applicationLayerSwclListPath = os.path.join(
            self._swcls_dir, APPLICATION_SWCL_LIST_VERSION_ONE_FILE_NAME
        )
        self._log.debug(
            "generateApplicationLayerSwclList, applicationLayerSwclListPath:%s", applicationLayerSwclListPath
        )
        
        # # Delete the original application-level software cluster list
        # self.cleanDir(self._swcls_dir, lambda entryName:len(re.findall(r'(run_time_application_swcl_list_ver\d+.json)|(run_time_application_swcl_list_ver\d+.md5)',entryName))==0)

        # Generate new software cluster list
        self.generateSWCLList(applicationLayerSwclList, applicationLayerFunctionGroupList, applicationLayerSwclListPath)


    # Clean up specified directory,
    def cleanDir(self, dir, excludeCondition):
        """
        clean a dir.
        """
        self._log.debug("cleanDir, with dir:%s excludeCondition:%s", dir, excludeCondition)
        if os.path.exists(dir):
            for entry in os.scandir(dir):
                self._log.debug("cleanDir, get a entry:%s", entry)

                entryName = os.path.basename(os.path.normpath(entry))
                self._log.debug("cleanDir, get a entryName:%s", entryName)

                # Whether it can be deleted
                if not excludeCondition(entryName):
                    try:
                        if entry.is_file():
                            os.remove(entry)
                        else:
                            shutil.rmtree(entry)
                        self._log.warning(
                            "cleanDir, entry:%s is removed successfully", entry
                        )
                    except Exception as exception:
                        self._log.error(
                            "cleanDir, there is a exception:%s when trying to remove entry:%s",
                            exception.strerror,
                            entry,
                        )
                        sys.exit(
                            "error: failed to remove dir:{0} with exception:{1} when cleanDir.\n".format(
                                entry, exception.strerror
                            )
                        )

    # Integrate software cluster IntegrateSWCL(swclName, swclFQN, swclCategory, swclVersion)
    def IntegrateSWCL(self, swclName,swclFQN,swclCategory,swclVersion):
        """
        Integrate a software cluster.
        """
        self._log.debug("IntegrateSWCL, begin with swclName:%s swclFQN:%s swclCategory:%s swclVersion:%s self._settings.canUseExistingExe:%s", swclName, swclFQN, swclCategory, swclVersion, self._settings.canUseExistingExe)

        # 1. Clean up the target location of the software cluster
        #     Get the target directory swclDir of the software cluster based on swclName and swclCategory
        swclDir = self.getSWCLDir(swclName, swclCategory)
        swclVersionDir = os.path.join(swclDir, swclVersion)
        swclVersionEtcDir = os.path.join(swclVersionDir, ETC_DIR_NAME)
        swclVersionBinDir = os.path.join(swclVersionDir, BIN_DIR_NAME)
        self._log.debug(
            "IntegrateSWCL, swclDir:%s swclVersionDir:%s swclVersionEtcDir:%s swclVersionBinDir:%s",
            swclDir,
            swclVersionDir,
            swclVersionEtcDir,
            swclVersionBinDir,
        )

        # Clear directories under swclDir that are not swclVersion (be careful not to delete ara_loader and VERSION under framework software cluster) (WARN log)
        self._log.debug("IntegrateSWCL, try to cleanDir swclDir:%s", swclDir)
        excludeNames = [swclVersion]
        if (swclName == FRAMEWORK_SWCL_NAME):
            excludeNames.append(ARA_LOADER_NAME)
            excludeNames.append(EXE_VERSION_NAME)
        self._log.debug("IntegrateSWCL, excludeNames:%s", excludeNames)
        self.cleanDir(swclDir, lambda entryName:(entryName in excludeNames))

        #   Clear swclDir/swclVersion/etc folder
        self._log.debug("IntegrateSWCL, try to _clear_dir swclVersionEtcDir:%s", swclVersionEtcDir)
        self._clear_dir(swclVersionEtcDir)

        # 2. Based on swclFQN, copy swcl_manifest.json and related FunctionGroupSets.json, ModeDeclarationGroups.json from self._manifestDir to swclDir/swclVersion -- using copyFilesWithPrefix
        # Generated software-clusters directory
        generated_swcl_path = os.path.join(
            self._machineInfoDir, SOFTWARE_CLUSTERS_DIR_NAME_IN_WORKDIR
        )
        self._log.debug("IntegrateSWCL, try to copy swcl_manifest.json&FunctionGroupSets.json&ModeDeclarationGroups.json from generated_swcl_path:%s to swclVersionDir:%s", generated_swcl_path, swclVersionDir)
        self.copyFilesWithPrefix(generated_swcl_path, swclVersionDir, swclName)

        # Create etc directory
        if (swclName != OS_SWCL_NAME): 
            os.makedirs(swclVersionEtcDir)
        
        # 3. Move the FunctionGroupSets.json, ModeDeclarationGroups.json copied in step 2 to swclDir/swclVersion/etc
        functionGroupSetsPath = os.path.join(swclVersionDir, FUNCTION_GROUP_SETS_FILE_NAME)
        if os.path.exists(functionGroupSetsPath):
            self._log.debug("IntegrateSWCL, try to move functionGroupSetsPath:%s to swclVersionEtcDir:%s", functionGroupSetsPath, swclVersionEtcDir)
            try:
                shutil.move(functionGroupSetsPath, swclVersionEtcDir)
                self._log.debug(
                    "IntegrateSWCL, functionGroupSetsPath:%s is moved to swclVersionEtcDir:%s successfully",
                    functionGroupSetsPath,
                    swclVersionEtcDir,
                )
            except Exception as exception:
                self._log.error(
                    "IntegrateSWCL, there is a exception:%s when trying to move functionGroupSetsPath:%s to swclVersionEtcDir:%s",
                    exception.strerror,
                    functionGroupSetsPath,
                    swclVersionEtcDir,
                )
                sys.exit(
                    "error: failed to move functionGroupSetsPath:{0} to swclVersionEtcDir:{1} with exception:{2} when IntegrateSWCL.\n".format(
                        functionGroupSetsPath, swclVersionEtcDir, exception.strerror
                    )
                )

        modeDeclarationGroupsPath = os.path.join(swclVersionDir, MODE_DECLARATION_GROUPS_FILE_NAME)
        if os.path.exists(modeDeclarationGroupsPath):
            self._log.debug("IntegrateSWCL, try to move modeDeclarationGroupsPath:%s to swclVersionEtcDir:%s", modeDeclarationGroupsPath, swclVersionEtcDir)
            try:
                shutil.move(modeDeclarationGroupsPath, swclVersionEtcDir)
                self._log.debug(
                    "IntegrateSWCL, modeDeclarationGroupsPath:%s is moved to swclVersionEtcDir:%s successfully",
                    modeDeclarationGroupsPath,
                    swclVersionEtcDir,
                )
            except Exception as exception:
                self._log.error(
                    "IntegrateSWCL, there is a exception:%s when trying to move modeDeclarationGroupsPath:%s to swclVersionEtcDir:%s",
                    exception.strerror,
                    modeDeclarationGroupsPath,
                    swclVersionEtcDir,
                )
                sys.exit(
                    "error: failed to move modeDeclarationGroupsPath:{0} to swclVersionEtcDir:{1} with exception:{2} when IntegrateSWCL.\n".format(
                        modeDeclarationGroupsPath, swclVersionEtcDir, exception.strerror
                    )
                )

        
        # 4. Parse swcl_manifest.json under swclDir/swclVersion to get containedExecutables, containedProcesses
        SWCLManifestPath = os.path.join(swclVersionDir, SWCL_MANIFEST_FILE_NAME)
        self._log.debug("IntegrateSWCL, SWCLManifestPath:%s", SWCLManifestPath)
        with open(SWCLManifestPath) as user_file:
            file_contents = user_file.read()
        SWCLManifestJson = json.loads(file_contents)
        self._log.debug("IntegrateSWCL, get SWCLManifestJson:%s", SWCLManifestJson)
            
        containedProcesses = []
        if SWCL_MANIFEST_CONTAINED_PROCESSES_KEY_NAME in SWCLManifestJson:
            containedProcesses = SWCLManifestJson[SWCL_MANIFEST_CONTAINED_PROCESSES_KEY_NAME]
        self._log.debug("IntegrateSWCL, get containedProcesses:%s", containedProcesses)

        containedExecutables = []
        if SWCL_MANIFEST_CONTAINED_EXECUTABLES_KEY_NAME in SWCLManifestJson:
            containedExecutables = SWCLManifestJson[SWCL_MANIFEST_CONTAINED_EXECUTABLES_KEY_NAME]
        self._log.debug("IntegrateSWCL, get containedExecutables:%s", containedExecutables)

        
        # Clean up files under swclDir/swclVersion/bin that are not in containedExecutables (be careful not to delete aragen under framework software cluster bin) (WARN log)
        exeNames = []
        for containedExecutable in containedExecutables:
            exeName = re.split(r"[/]+", containedExecutable)[-1]
            exeNames.append(exeName)
            # If the current software cluster is core, and the name corresponding to a certain exeFQN in containedExecutables is smd, then
            #     If exeFQN does not exist in _executableFQN2PathDict, then:
            #         _executableFQN2PathDict[exeFQN] = path of smd in SDK core software cluster directory
            if (self._settings.withSDK) and (swclName == CORE_SWCL_NAME) and ((exeName == SMD_EXECUTABLE_NAME) or (exeName == RESETMACHINE_EXECUTABLE_NAME) or (exeName == SHUTDOWN_EXECUTABLE_NAME) or (exeName == SHELL_EXECUTABLE_NAME)):
                self._log.debug("IntegrateSWCL, try to set containedExecutable:%s in self._executableFQN2PathDict for state management/ResetMachine/shutdown.", containedExecutable)
                
                SDKAra = os.path.join(self._settings.withSDK, ARA_DIR_NAME)
                self._log.debug("IntegrateSWCL, get SDKAra:%s", SDKAra)

                SDKAraVerCFGPath = os.path.join(
                    SDKAra,
                    ARA_VER_ONE_CFG_FILE_NAME,
                )
                self._log.info(
                    "IntegrateSWCL, get SDKAraVerCFGPath: %s",
                    SDKAraVerCFGPath
                )
                            
                with open(SDKAraVerCFGPath) as user_file:
                    file_contents = user_file.read()
                config = json.loads(file_contents)
                self._log.debug("IntegrateSWCL, get config:%s", config)
                
                SDKCoreversion = config[ARA_VER_CFG_UCM_SECTION_NAME][CORE_VERSION_NAME]
                self._log.info(
                    "IntegrateSWCL, SDKCoreversion: %s",
                    SDKCoreversion
                )

                exeInSDKCore = os.path.join(
                    SDKAra,
                    CORE_SWCL_NAME, SDKCoreversion, BIN_DIR_NAME, exeName
                )
                
                # exe not yet in _executableFQN2PathDict
                if (containedExecutable not in self._executableFQN2PathDict):
                    self._executableFQN2PathDict[containedExecutable] = exeInSDKCore
                    self._log.info(
                        "IntegrateSWCL, we save containedExecutable: %s and exeInSDKCore:%s in _executableFQN2PathDict.",
                        containedExecutable, exeInSDKCore
                    )
                else: # exe already in _executableFQN2PathDict
                    # Get modification times of both
                    exePath = self._executableFQN2PathDict[containedExecutable]
                    exePathMTime = os.path.getmtime(exePath)
                    self._log.debug("IntegrateSWCL, get modified time:%f for exePath:%s", exePathMTime, exePath)
                    exeInSDKCoreMTime = os.path.getmtime(exeInSDKCore)
                    self._log.debug("IntegrateSWCL, get modified time:%f for exeInSDKCore:%s", exeInSDKCoreMTime, exeInSDKCore)
                    if (exeInSDKCoreMTime > exePathMTime): # Compare modification times
                        self._executableFQN2PathDict[containedExecutable] = exeInSDKCore
                        self._log.info(
                            "IntegrateSWCL, we update containedExecutable: %s and exeInSDKCore:%s in _executableFQN2PathDict.",
                            containedExecutable, exeInSDKCore
                        )
                        
        if (swclName == FRAMEWORK_SWCL_NAME):
            exeNames.append(ARAGEN_EXECUTABLE_NAME)
            exeNames.append(VALIDATION_EXECUTABLE_NAME)
            exeNames.append(DIAG_GW_NAME)
        self._log.debug("IntegrateSWCL, try to cleanDir swclVersionBinDir:%s with exeNames:%s", swclVersionBinDir, exeNames)
        self.cleanDir(swclVersionBinDir, lambda entryName:(entryName in exeNames))
        

        # 5. For each ExecutableFQN in containedExecutables:
        self.IntegrateExecutables(containedExecutables, swclVersionBinDir)

        # 6. For each ProcessFQN in containedProcesses: find all corresponding manifest files from self._manifestDir and copy to swclDir/swclVersion/etc/ProcessName -- using copyFilesWithPrefix
        self.IntegrateProcesses(containedProcesses, swclVersionEtcDir)

        # 7. If it is a core software cluster:
        #     Based on rootSwc2ProcessDictInCore, find the routing process, delete nsomeip.json and npc.json under its etc
        #     Find all machine-level manifest files from self._manifestDir and copy to swclDir/swclVersion/etc -- using copyFilesWithPrefix
        #     Use rootSwc2ProcessDictInCore to move dlt.conf, nsomeip.json, npc.json, etc_network_interfaces under etc to special directories
        if (swclName == CORE_SWCL_NAME):
            self.IntegrateMachine(swclVersionEtcDir)

        # 8. Generate software cluster status information swcl_status.json to swclDir
        self.generateSWCLStatusInfo(
            swclVersion, os.path.join(swclDir, SWCL_STATUS_FILE_NAME)
        )

    # Collect software cluster information CollectSWCLInfo(swclName, swclFQN, swclCategory, swclVersion, swclList, functionGroupList, swclContextDict)  --- swclList, functionGroupList, swclContextDict are output parameters
    def CollectSWCLInfo(self, swclName, swclFQN, swclCategory, swclVersion, swclDependsOn, swclList, functionGroupList, swclContextDict):
        """
        Collect the info of a software cluster.
        """
        self._log.debug("CollectSWCLInfo, begin with swclName:%s swclFQN:%s swclCategory:%s swclVersion:%s swclDependsOn:%s", swclName, swclFQN, swclCategory, swclVersion, swclDependsOn)

        # 1. Get target directory swclDir of software cluster based on swclName and swclCategory
        swclDir = self.getSWCLDir(swclName, swclCategory)
        swclVersionDir = os.path.join(swclDir, swclVersion)
        swclVersionEtcDir = os.path.join(swclVersionDir, ETC_DIR_NAME)

        # 2. Collect software cluster information from swclDir/swclVersion/swcl_manifest.json and add to swclList
        SWCLManifestPath = os.path.join(swclVersionDir, SWCL_MANIFEST_FILE_NAME)
        self._log.debug("CollectSWCLInfo, swclDir:%s swclVersionDir:%s swclVersionEtcDir:%s SWCLManifestPath:%s", swclDir, swclVersionDir, swclVersionEtcDir, SWCLManifestPath)

        # If SWCLManifestPath does not exist, then:
        if not os.path.exists(SWCLManifestPath):
            self._log.error(
                "CollectSWCLInfo, can't find SWCLManifestPath:%s for swclName:%s swclFQN:%s swclVersion:%s",
                SWCLManifestPath,
                swclName, 
                swclFQN,
                swclVersion
            )
            sys.exit(
                "error: can't find SWCLManifestPath:{0} for swclName:{1} swclFQN:{2} swclVersion:{3} when CollectSWCLInfo.\n".format(
                    SWCLManifestPath, 
                    swclName, 
                    swclFQN,
                    swclVersion
                )
            )

        with open(SWCLManifestPath) as user_file:
            file_contents = user_file.read()
        SWCLManifestJson = json.loads(file_contents)
        self._log.debug("CollectSWCLInfo, get SWCLManifestJson:%s", SWCLManifestJson)

        # Data to be written
        # {
        #     "swcl_name": "framework",
        #     "swcl_fqn": "/ISOFT/Development/SoftwareClusters1/framework",
        #     "category": "ISOFT_FRAMEWORK",
        #     "swcl_version": "1.0.0",
        #     "dependsOn": [
        #     ],
        #     "executable_list": [
        #         "/ISOFT/PlatformComponents/crypto/cryptod",
        #         "/ISOFT/PlatformComponents/diag/Executable/dmd",
        #         "/ISOFT/PlatformComponents/log/exe/logd",
        #         "/ISOFT/PlatformComponents/nm/exe/nmd",
        #         "/ISOFT/PlatformComponents/com/exe/nsomeipd",
        #         "/ISOFT/PlatformComponents/phm/exe/phmd",
        #         "/ResetMachineApp/exe/ResetMachine",
        #         "/ISOFT/PlatformComponents/tsync/tsyncd",
        #         "/ISOFT/PlatformComponents/ucm/exe/ucmd",
        #         "/ISOFT/PlatformComponents/ucm_master/exe/ucmmd"
        #     ],
        #     "process_list": [     -- note here is shortname
        #     ]
        # },
        swclInfoDict = {}
        swclInfoDict["swcl_name"] = SWCLManifestJson["shortName"]
        swclInfoDict["swcl_fqn"] = SWCLManifestJson["fullQualifiedName"]
        swclInfoDict["category"] = SWCLManifestJson["category"]
        swclInfoDict["swcl_version"] = SWCLManifestJson["version"]

        # software cluster FQN in software cluster description file
        swclFQNInManifest = SWCLManifestJson["fullQualifiedName"]
        self._log.debug("CollectSWCLInfo, get a swclFQNInManifest:%s for swclFQN:%s", swclFQNInManifest, swclFQN)
        
        swclInfoDict["dependsOn"] = swclDependsOn
        # if "dependsOn" in SWCLManifestJson:
        #     swclInfoDict["dependsOn"] = SWCLManifestJson["dependsOn"]

        swclInfoDict["executable_list"] = []
        if "containedExecutables" in SWCLManifestJson:
            swclInfoDict["executable_list"] = SWCLManifestJson["containedExecutables"]
        
        swclInfoDict["process_list"] = []
        if "containedProcesses" in SWCLManifestJson:
            for processFQN in SWCLManifestJson["containedProcesses"]:
                swclInfoDict["process_list"].append(re.split(r"[/]+", processFQN)[-1])

        swclInfoDict["process_list"].sort()
        
        if "diagnosticAddress" in SWCLManifestJson:
            swclInfoDict["diagnostic_address"] = SWCLManifestJson["diagnosticAddress"]
        
        swclList.append(swclInfoDict)
        self._log.debug("CollectSWCLInfo, save a swclInfoDict:%s", swclInfoDict)

        # Also collect software cluster content information swclContextDict: {swclFQN, ([dependsOnSWCLFQN], [ExecutableFQN])}
        swclContextDict[swclFQNInManifest] = (swclDependsOn, swclInfoDict["executable_list"])

    def IntegrateExecutables(self, executableFQNs, swclVersionBinDir):
        """
        Integrate executables to swclVersionBinDir.
        """
        self._log.debug("IntegrateExecutables, begin with executableFQNs:%s and swclVersionBinDir:%s", executableFQNs, swclVersionBinDir)

        # Create bin directory
        if len(executableFQNs) > 0:
            os.makedirs(swclVersionBinDir, exist_ok=True)
        
        for executableFQN in executableFQNs:
            exeName = re.split(r"[/]+", executableFQN)[-1]
            self._log.debug("IntegrateSWCL, get a executableFQN:%s and exeName:%s", executableFQN, exeName)
            
            # target path
            dest_exe_path = os.path.join(swclVersionBinDir, exeName)
            self._log.debug(
                "IntegrateSWCL, get a dest_exe_path:%s", dest_exe_path
            )

            #     Find corresponding exe from executableBuildDirs
            if executableFQN in self._executableFQN2PathDict: # found
                executablePath = self._executableFQN2PathDict[executableFQN]  
                self._log.debug("IntegrateSWCL, get a executablePath:%s.", executablePath)

                # Compare files (as long as different)
                equal = os.path.exists(dest_exe_path) and filecmp.cmp(executablePath, dest_exe_path)
                self._log.debug("IntegrateSWCL, get a equal:%s for executablePath:%s and dest_exe_path:%s.", equal, executablePath, dest_exe_path)
                if not equal:
                    # copy to swclDir/swclVersion/bin
                    try:
                        shutil.copyfile(executablePath, dest_exe_path)
                        shutil.copystat(executablePath, dest_exe_path)
                        self._log.debug(
                            "IntegrateSWCL, executablePath:%s is copied to dest_exe_path:%s successfully",
                            executablePath,
                            dest_exe_path,
                        )
                    except Exception as exception:
                        self._log.error(
                            "IntegrateSWCL, there is a exception:%s when trying to copy executablePath:%s to dest_exe_path:%s",
                            exception.strerror,
                            executablePath,
                            dest_exe_path,
                        )
                        sys.exit(
                            "error: failed to copy executablePath:{0} to dest_exe_path:{1} with exception:{2} when IntegrateSWCL.\n".format(
                                executablePath, dest_exe_path, exception.strerror
                            )
                        )
            elif (self._settings.canUseExistingExe) and (os.path.exists(dest_exe_path)):  # Check whether there is a corresponding exe under swclDir/swclVersion/bin
                # When configuring exe in software cluster, if the built file cannot be found, use the existing file.
                self._log.debug(
                        "IntegrateSWCL, can't find executablePath for executableFQN:%s, but self._settings.canUseExistingExe:%s and dest_exe_path:%s exists",
                        executableFQN,
                        self._settings.canUseExistingExe,
                        dest_exe_path,
                    )
            else:
                self._log.error(
                        "IntegrateSWCL, can't find executablePath for executableFQN:%s, and self._settings.canUseExistingExe:%s and dest_exe_path:%s exists:%s",
                        executableFQN,
                        self._settings.canUseExistingExe,
                        dest_exe_path,
                        os.path.exists(dest_exe_path)
                    )
                sys.exit(
                    "error: can't find executablePath for executableFQN:{0}, and self._settings.canUseExistingExe:{1} and dest_exe_path:{2} exists:{3} when IntegrateSWCL.\n".format(
                        executableFQN, self._settings.canUseExistingExe,
                        dest_exe_path,
                        os.path.exists(dest_exe_path)
                    )
                )

    # For each ProcessFQN in containedProcesses: find all corresponding manifest files from self._manifestDir and copy to swclDir/swclVersion/etc/ProcessName -- using copyFilesWithPrefix
    def IntegrateProcesses(self, processFQNs, swclVersionEtcDir):
        """
        Integrate processes to swclVersionEtcDir.
        """
        self._log.debug("IntegrateProcesses, begin with processFQNs:%s and swclVersionEtcDir:%s", processFQNs, swclVersionEtcDir)

        for processFQN in processFQNs:
            processName = re.split(r"[/]+", processFQN)[-1]
            processesDir = os.path.join(
                swclVersionEtcDir, processName
            )
            self._log.debug("IntegrateProcesses, get a processFQN:%s and processName:%s and processesDir:%s", processFQN, processName, processesDir)

            # generated processes directory
            generatedProcessesPath = os.path.join(
                self._machineInfoDir, PROCESSES_DIR_NAME_IN_WORKDIR
            )
            self._log.debug(
                "IntegrateProcesses, generatedProcessesPath:%s", generatedProcessesPath
            )

            self.copyFilesWithPrefix(
                generatedProcessesPath, processesDir, processName
            )

    
    def IntegrateMachine(self, swclVersionEtcDir):
        """
        Integrate machine info to swclVersionEtcDir.
        """
        self._log.debug("IntegrateMachine, begin with swclVersionEtcDir:%s", swclVersionEtcDir)

        # Based on rootSwc2ProcessDictInCore, find the routing process, delete nsomeip.json and npc.json under its etc
        if ROUTING_ROOTSWC_NAME in self._rootSwc2ProcessDictInCore:
            routingProcessPath = os.path.join(
                swclVersionEtcDir, self._rootSwc2ProcessDictInCore[ROUTING_ROOTSWC_NAME][0]
            )
            self._log.debug(
                "IntegrateMachine, routingProcessPath:%s", routingProcessPath
            )

            # nsomeip configuration information
            routingProcessNsomeipPath = os.path.join(
                routingProcessPath, NSOMEIP_ETC_FILE_NAME
            )

            # npc configuration information
            routingProcessNPCPath = os.path.join(
                routingProcessPath, NPC_ETC_FILE_NAME
            )

            self._log.debug(
                "IntegrateMachine, routingProcessNsomeipPath:%s routingProcessNPCPath:%s", routingProcessNsomeipPath, routingProcessNPCPath
            )
            try:
                if os.path.exists(routingProcessNsomeipPath):
                    os.remove(routingProcessNsomeipPath)
                if os.path.exists(routingProcessNPCPath):
                    os.remove(routingProcessNPCPath)
                self._log.debug(
                    "IntegrateMachine, routingProcessNsomeipPath:%s or/and routingProcessNPCPath:%s is removed successfully",
                    routingProcessNsomeipPath,
                    routingProcessNPCPath
                )
            except FileNotFoundError:
                self._log.warn(
                    "IntegrateMachine, can't find routingProcessNsomeipPath:%s or/and routingProcessNPCPath:%s.",
                    routingProcessNsomeipPath,
                    routingProcessNPCPath)
        
        # machine name
        machine_standard_name = re.split(r"[/]+", self._settings.machine)[-1]

        # generated machines directory
        generatedMachinesPath = os.path.join(
            self._machineInfoDir, MACHINES_DIR_NAME_IN_WORKDIR
        )
        self._log.debug(
            "IntegrateMachine, machine_standard_name:%s generatedMachinesPath:%s", machine_standard_name, generatedMachinesPath
        )
    
        # Find all machine-level manifest files from self._manifestDir and copy to swclDir/swclVersion/etc -- using copyFilesWithPrefix
        self.copyFilesWithPrefix(
            generatedMachinesPath, swclVersionEtcDir, machine_standard_name
        )

        # Special machine-related configuration file name to its path
        SpecialConfigName2PathInCore = {}
        if LOG_ROOTSWC_NAME in self._rootSwc2ProcessDictInCore:
            SpecialConfigName2PathInCore["dlt.conf"] = os.path.join(
                    swclVersionEtcDir, self._rootSwc2ProcessDictInCore[LOG_ROOTSWC_NAME][0], "dlt.conf"
                )
        
        if ROUTING_ROOTSWC_NAME in self._rootSwc2ProcessDictInCore:
            SpecialConfigName2PathInCore["nsomeip.json"] = os.path.join(
                    swclVersionEtcDir, self._rootSwc2ProcessDictInCore[ROUTING_ROOTSWC_NAME][0], NSOMEIP_ETC_FILE_NAME
                )
            SpecialConfigName2PathInCore["npc.json"] = os.path.join(
                    swclVersionEtcDir, self._rootSwc2ProcessDictInCore[ROUTING_ROOTSWC_NAME][0], NPC_ETC_FILE_NAME
                )

        # Use rootSwc2ProcessDictInCore to move dlt.conf, nsomeip.json, npc.json, etc_network_interfaces under etc to special directories
        for specialConfigName, specialConfigDestPath in SpecialConfigName2PathInCore.items():
            specialConfigSrcPath = os.path.join(
                    swclVersionEtcDir, specialConfigName
                )
            self._log.debug("IntegrateMachine, try to move specialConfigSrcPath:%s to specialConfigDestPath:%s", specialConfigSrcPath, specialConfigDestPath)
            try:
                shutil.move(specialConfigSrcPath, specialConfigDestPath)
                self._log.debug(
                    "IntegrateMachine, specialConfigSrcPath:%s is moved to specialConfigDestPath:%s successfully",
                    specialConfigSrcPath,
                    specialConfigDestPath,
                )
            except Exception as exception:
                self._log.error(
                    "IntegrateMachine, there is a exception:%s when trying to move specialConfigSrcPath:%s to specialConfigDestPath:%s",
                    exception.strerror,
                    specialConfigSrcPath,
                    specialConfigDestPath,
                )
                sys.exit(
                    "error: failed to move specialConfigSrcPath:{0} to specialConfigDestPath:{1} with exception:{2} when IntegrateMachine.\n".format(
                        specialConfigSrcPath, specialConfigDestPath, exception.strerror
                    )
                )


    def copyFilesWithPrefix(self, srcDir, destDir, prefixName):
        """
        copy files which have prefix:prefixName from srcDir to destDir, removing prefixName as new file name
        """
        self._log.debug(
            "copyFilesWithPrefix, srcDir:%s destDir:%s prefixName:%s",
            srcDir,
            destDir,
            prefixName,
        )

        # Find all files under srcDir
        srcFiles = [f.path for f in os.scandir(srcDir) if f.is_file()]

        # Try to create target directory
        if len(srcFiles) > 0:
            os.makedirs(destDir, exist_ok=True)

        for srcFilePath in srcFiles:
            # file name
            srcFileName = os.path.basename(os.path.normpath(srcFilePath))
            self._log.debug("copyFilesWithPrefix, get a srcFilePath:%s srcFileName:%s", srcFilePath, srcFileName)

            # Check whether the prefix matches prefixName
            index = srcFileName.find(prefixName+"_")
            if index == 0:
                destFileName = srcFileName[(index + len(prefixName) + 1) :]
                destFilePath = os.path.join(destDir, destFileName)
                self._log.debug(
                    "copyFilesWithPrefix, get a destFileName:%s destFilePath:%s", destFileName, destFilePath
                )

                try:
                    shutil.copyfile(srcFilePath, destFilePath)
                    self._log.debug(
                        "copyFilesWithPrefix, srcFilePath:%s is copied to destFilePath:%s successfully",
                        srcFilePath,
                        destFilePath,
                    )
                except Exception as exception:
                    self._log.error(
                        "copyFilesWithPrefix, there is a exception:%s when trying to copy srcFilePath:%s to destFilePath:%s",
                        exception.strerror,
                        srcFilePath,
                        destFilePath,
                    )
                    sys.exit(
                        "error: failed to copy srcFilePath:{0} to destFilePath:{1} with exception:{2} when copyFilesWithPrefix.\n".format(
                            srcFilePath, destFilePath, exception.strerror
                        )
                    )


    def generateSWCLStatusInfo(self, swclVersion, outputPath):
        """
        generate swcl status info for a swcl.
        """
        self._log.debug("generateSWCLStatusInfo, swclVersion:%s outputPath:%s", swclVersion, outputPath)

        # Data to be written
        dictionary = {"state": "kPresent", "version": "1.0.0", "new_version": ""}
        dictionary["version"] = swclVersion

        # Serializing json
        json_object = json.dumps(dictionary, indent=4)

        # Writing to sample.json
        with open(outputPath, "w") as outfile:
            outfile.write(json_object)


    def generateSWCLList(self, swclList, functionGroupList, swclListPath):
        """
        generate swcl list.
        """
        self._log.debug(
            "generateSWCLList, swclList:%s functionGroupList:%s swclListPath:%s",
            swclList,
            functionGroupList,
            swclListPath,
        )

        # dictionary = {"swcls": swclList, "functionGroups": functionGroupList}
        # Temporarily remove function group information
        dictionary = {"swcls": swclList}

        # sort software clusters
        def get_swclName(element):
            return element["swcl_name"]

        dictionary["swcls"].sort(key=get_swclName)

        # Serializing json
        json_object = json.dumps(dictionary, indent=4)

        # Writing to processListPath
        with open(swclListPath, "w") as outfile:
            outfile.write(json_object)

        self._log.debug("generateSWCLList, end.")


    #   Link the run_time_application_swcl_list_ver1.json generated in step 6 to the etc directory of the core software cluster
    def LinkApplicationLayerSWCLList2Core(self):
        # application-level software cluster list path
        applicationLayerSwclListPath = os.path.join(
            self._swcls_dir, APPLICATION_SWCL_LIST_VERSION_ONE_FILE_NAME
        )
        self._log.debug(
            "LinkApplicationLayerSWCLList2Core, applicationLayerSwclListPath:%s", applicationLayerSwclListPath
        )

        if not os.path.exists(applicationLayerSwclListPath):
            self._log.error(
                "LinkApplicationLayerSWCLList2Core, can't find applicationLayerSwclListPath: %s.",
                applicationLayerSwclListPath,
            )
            sys.exit(
                "error: can't find applicationLayerSwclListPath:{0} in self._swclInfoDict when LinkApplicationLayerSWCLList2Core.\n".format(
                    applicationLayerSwclListPath
                )
            )
        
        core_swclCategory = self._swclToIntegrateDict[CORE_SWCL_NAME][1]
        core_swclVersion = self._swclToIntegrateDict[CORE_SWCL_NAME][2]
        core_swclDir = self.getSWCLDir(CORE_SWCL_NAME, core_swclCategory)
        core_swclVersionDir = os.path.join(core_swclDir, core_swclVersion)
        core_swclETCDir = os.path.join(core_swclVersionDir, ETC_DIR_NAME)
        self._log.debug("linkApplicationLayerSWCLList2Core, got core_swclETCDir:%s", core_swclETCDir)

        # Generate run_time_application_swcl_list.json and run_time_application_swcl_list.md5 to the etc directory of the platform software cluster maximum version directory
        # Generate md5 of applicationLayerSwclListPath
        applicationLayerSwclListMD5 = md5(applicationLayerSwclListPath)
        ApplicationLayerSwclListCFGPath = os.path.join(
            core_swclETCDir, APPLICATION_SWCL_LIST_CFG_FILE_NAME
        )
        ApplicationLayerSwclListMD5Path = os.path.join(
            core_swclETCDir, APPLICATION_SWCL_LIST_MD5_FILE_NAME
        )
        self._log.info(
            "LinkApplicationLayerSWCLList2Core, get ApplicationLayerSwclListCFGPath:%s, ApplicationLayerSwclListMD5Path:%s",
            ApplicationLayerSwclListCFGPath,
            ApplicationLayerSwclListMD5Path,
        )
        
        # Data to be written
        dictionary = {}
        dictionary[APPLICATION_SWCL_LIST_SECTION_NAME] = {}
        dictionary[APPLICATION_SWCL_LIST_SECTION_NAME]["file_name"] = APPLICATION_SWCL_LIST_VERSION_ONE_FILE_NAME
        dictionary[APPLICATION_SWCL_LIST_SECTION_NAME]["md5"] = applicationLayerSwclListMD5

        # Serializing json
        json_object = json.dumps(dictionary, indent=4)

        # Writing to sample.json
        with open(ApplicationLayerSwclListCFGPath, "w") as outfile:
            outfile.write(json_object)

        try:
            # 3594eceda945a783105828c5fc0f01d2
            MD5file = open(ApplicationLayerSwclListMD5Path, "w+")
            MD5Str = md5(ApplicationLayerSwclListCFGPath)
            MD5file.write(MD5Str)
            MD5file.close()
        except Exception as exception:
            self._log.error(
                "LinkApplicationLayerSWCLList2Core, there is a exception:%s when trying to open ApplicationLayerSwclListMD5Path:%s",
                exception.strerror,
                ApplicationLayerSwclListMD5Path,
            )
            sys.exit(
                "error: failed to open ApplicationLayerSwclListMD5Path:{0} with exception:{1} when LinkApplicationLayerSWCLList2Core.\n".format(
                    ApplicationLayerSwclListMD5Path, exception.strerror
                )
            )

        self._log.info("LinkApplicationLayerSWCLList2Core end.")

    # 8 Check whether the exe corresponding to each process in each software cluster in swclToIntegrateDict is available (process may not have exe)
    # swclContextDict:{swclFQN, ([dependsOnSWCLFQN],[ExecutableFQN])}
    def checkAllExeAvailable(self):
        self._log.debug("checkAllExeAvailable, begin with self._swclContextDict:%s", self._swclContextDict)

        # 1. Get available resource list for each software cluster SWCLAvailableResourceDict: {swclFQN, set(ExecutableFQN)}
        SWCLAvailableResourceDict = {}
        
        # For each swclFQN in swclContextDict:
        for swclFQN, swclContent in self._swclContextDict.items():
            self._log.debug("checkAllExeAvailable, get a swclFQN:%s swclContent:%s", swclFQN, swclContent)

            # Get its visible software cluster list visiableSWCLList
            circleDependCheckSet = {swclFQN} # for circular dependency check
            visiableSWCLList = self.GetVisiableSWCLList(swclFQN, self._swclContextDict, circleDependCheckSet)
            self._log.debug("checkAllExeAvailable, get a visiableSWCLList:%s for swclFQN:%s", visiableSWCLList, swclFQN)

            for visiableSWCLFQN in visiableSWCLList:
                if swclFQN not in SWCLAvailableResourceDict:
                    SWCLAvailableResourceDict[swclFQN] = set()
                SWCLAvailableResourceDict[swclFQN].update(self._swclContextDict[visiableSWCLFQN][1])
    
            self._log.debug("checkAllExeAvailable, get a SWCLAvailableResource:%s for swclFQN:%s", SWCLAvailableResourceDict[swclFQN], swclFQN)

        # swclToIntegrateDict: {software cluster name, (software cluster FQN, software cluster category, software cluster version, software cluster DependsOn, [(process FQN, executable FQN corresponding to process)])}
        # 2. For each swclFQN to be checked in swclToIntegrateDict
        for swclName, swclVersionInfo in self._swclToIntegrateDict.items():
            swclFQN = swclVersionInfo[0]
            swclDependsOn = swclVersionInfo[3]
            self._log.debug(
                "checkAllExeAvailable, get a swclFQN:%s swclDependsOn:%s",
                swclFQN,
                swclDependsOn
            )

            # For each process ProcessFQN to be checked, see whether its corresponding ExecutableFQN exists in SWCLAvailableResourceDict
            # process FQN and EXE FQN information
            for processInfo in swclVersionInfo[4]:
                ProcessFQN = processInfo[0]
                ExeFQN = processInfo[1]
                self._log.debug(
                    "checkAllExeAvailable, get a ProcessFQN:%s ExeFQN:%s",
                    ProcessFQN,
                    ExeFQN
                )
                if((swclFQN not in SWCLAvailableResourceDict) or (ExeFQN not in SWCLAvailableResourceDict[swclFQN])):
                    self._log.error(
                        "checkAllExeAvailable, can't find ExeFQN:%s for ProcessFQN:%s of swclFQN:%s in it and its dependsOn:%s.",
                        ExeFQN,
                        ProcessFQN,
                        swclFQN,
                        swclDependsOn
                    )
                    sys.exit(
                        "error: failed to find ExeFQN:{0} for ProcessFQN:{1} of swclFQN:{2} in it and its dependsOn:{3}.\n".format(
                            ExeFQN, ProcessFQN, swclFQN, swclDependsOn
                        )
                    )


    def GetVisiableSWCLList(self, swclFQN, swclContentDict, circleDependCheckSet):
        # Add itself to visible list
        visiableSWCLList = [swclFQN] 

        # Get dependent software cluster list
        for dependsOnSWCLFQN in swclContentDict[swclFQN][0]: 
            if dependsOnSWCLFQN in circleDependCheckSet: # circular dependency detected, need to exit
                self._log.error(
                    "GetVisiableSWCLList, there is a circle dependency as for dependsOnSWCLFQN:%s",
                    dependsOnSWCLFQN
                )
                sys.exit(
                    "error: there is a circle dependency as for dependsOnSWCLFQN:{0} when GetVisiableSWCLList.\n".format(
                        dependsOnSWCLFQN
                    )
                )
            newCircleDependCheckSet = circleDependCheckSet.copy()
            newCircleDependCheckSet.add(dependsOnSWCLFQN)
            visiableSWCLList.extend(self.GetVisiableSWCLList(dependsOnSWCLFQN, swclContentDict, newCircleDependCheckSet))
        return visiableSWCLList


    # Delete ${ARA_SYSROOT}/ara/ara_ver1.json
    def deleteAraBootInfo(self):
        araVerCFGPath = os.path.join(
            self._AraDir,
            ARA_VER_ONE_CFG_FILE_NAME,
        )
        self._log.debug(
            "deleteAraBootInfo, araVerCFGPath: %s",
            araVerCFGPath
        )

        try:
            os.remove(araVerCFGPath)
            self._log.debug(
                "deleteAraBootInfo, araVerCFGPath:%s is removed successfully",
                araVerCFGPath,
            )
        except FileNotFoundError:
            self._log.debug(
                "deleteAraBootInfo, can't find araVerCFGPath:%s.",
                araVerCFGPath,
            )

        self._log.debug("deleteAraBootInfo end.")

    
    # 9 Configure boot guidance information
    def configBootInfo(self):
        self._log.debug(
            "configBootInfo, begin with _AraDir:%s _platform_framework:%s _platform_core:%s _application_swcls:%s _platform_var:%s _framework_version:%s _core_version:%s",
            self._AraDir,
            self._platform_framework,
            self._platform_core,
            self._application_swcls,
            self._platform_var,
            self._framework_version,
            self._core_version
        )

        # Delete all ara_verX.json & ara_verX.md5 files under ara.
        self.cleanDir(self._AraDir, lambda entryName:len(re.findall(r'(ara_ver\d+.json)|(ara_ver\d+.md5)',entryName))==0)
   
        os.makedirs(self._AraDir, exist_ok=True)
        
        #   Based on platform_framework, platform_core, application_swcls, platform_var and core_version, framework_version, generate ara_ver1.json and corresponding ara_ver1.md5
        araVerCFGPath = os.path.join(
            self._AraDir,
            ARA_VER_ONE_CFG_FILE_NAME,
        )
        araVerMD5Path = os.path.join(
            self._AraDir,
            ARA_VER_ONE_MD5_FILE_NAME,
        )
        self._log.info(
            "configBootInfo, araVerCFGPath: %s, araVerMD5Path: %s",
            araVerCFGPath,
            araVerMD5Path,
        )
        
        # Data to be written
        dictionary = {}
        if((0 !=len(self._platform_framework)) or (0 !=len(self._platform_core)) or (0 !=len(self._application_swcls)) or (0 !=len(self._platform_var))):
            dictionary[ARA_VER_CFG_BASE_SECTION_NAME] = {}
            if (0 !=len(self._platform_framework)):
                dictionary[ARA_VER_CFG_BASE_SECTION_NAME][PLATFORM_FRAMEWORK_NAME] = self._platform_framework
            
            if (0 !=len(self._platform_core)):  
                dictionary[ARA_VER_CFG_BASE_SECTION_NAME][PLATFORM_CORE_NAME] = self._platform_core
            
            if (0 !=len(self._application_swcls)):
                dictionary[ARA_VER_CFG_BASE_SECTION_NAME][APPLICATION_SWCLS_NAME] = self._application_swcls
            
            if (0 !=len(self._platform_var)):
                dictionary[ARA_VER_CFG_BASE_SECTION_NAME][PLATFORM_VAR_NAME] = self._platform_var

        dictionary[ARA_VER_CFG_UCM_SECTION_NAME] = {}
        dictionary[ARA_VER_CFG_UCM_SECTION_NAME][FRAMEWORK_VERSION_NAME] = self._framework_version
        dictionary[ARA_VER_CFG_UCM_SECTION_NAME][CORE_VERSION_NAME] = self._core_version
        
        # Serializing json
        json_object = json.dumps(dictionary, indent=4)

        # Writing to sample.json
        with open(araVerCFGPath, "w") as outfile:
            outfile.write(json_object)
            
        with open(araVerCFGPath, 'r') as configfile:
            file_contents = configfile.read()
            all_of_it = json.loads(file_contents)
            self._log.debug("configBootInfo, get all_of_it:%s", all_of_it)

        # 6b7489e8b875fd0f6dcefedaecd692a5
        MD5file = open(araVerMD5Path, "w+")
        MD5Str = md5(araVerCFGPath)
        MD5file.write(MD5Str)
        MD5file.close()

        self._log.info("configBootInfo, end.")


    # 10 Copy files from sdk: are-cmd, build-log.json to self._settings.ara_sysroot
    def copyAraRunAndBuildLog(self):
        SDKdir = os.path.join(self._settings.withSDK)
        self._log.debug("copyAraRunAndBuildLog, begin SDKdir:%s self._settings.ara_sysroot:%s", SDKdir, self._settings.ara_sysroot)
        
        # files to copy
        filesToCopy = [ARA_RUN_FILE_NAME, "release", ".release.json"]
        for fileToCopy in filesToCopy:
            # source path
            src_path = os.path.join(SDKdir, fileToCopy)
            self._log.debug(
                "copyAraRunAndBuildLog, get a src_path:%s", src_path
            )
            
            # target path
            dest_path = os.path.join(self._settings.ara_sysroot, fileToCopy)
            self._log.debug(
                "copyAraRunAndBuildLog, get a dest_path:%s", dest_path
            )
            
            if os.path.exists(src_path):
                try:
                    shutil.copyfile(src_path, dest_path)
                    shutil.copystat(src_path, dest_path)
                    self._log.debug(
                        "copyAraRunAndBuildLog, src_path:%s is copied to dest_path:%s successfully",
                        src_path,
                        dest_path,
                    )
                except Exception as exception:
                    self._log.error(
                        "IntegrateSWCL, there is a exception:%s when trying to copy src_path:%s to dest_path:%s",
                        exception.strerror,
                        src_path,
                        dest_path,
                    )
                    sys.exit(
                        "error: failed to copy src_path:{0} to dest_path:{1} with exception:{2} when IntegrateSWCL.\n".format(
                            src_path, dest_path, exception.strerror
                        )
                    )


    def _log_invocation_details(self):
        config_message = "configMachine started with:\n\t"
        config_message += "\n\t".join(self._settings.configuration_dump)
        config_message = config_message.rstrip()

        self._log.debug(config_message)
