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
# @file       are.py
# @brief      Export ARE installation package
# @details
# @date       by: james.feng
# @author     james
# @version    1.2.0
#
# ================================================================

import sys, os, shutil
import copy
import re
import json5, json
from pathlib import Path
from typing import Dict
import subprocess
from jsonpath import jsonpath
import uuid

__SDK_UTILS_SRC_DIR = str(Path(__file__).resolve().parent.parent)
if __SDK_UTILS_SRC_DIR not in sys.path:
    sys.path.insert(0, __SDK_UTILS_SRC_DIR)

from core import Fsh, global_fsh
from core  import Logger, global_logger
from config import AreArguments, AreExportType, AreStripLevel
import utils

__all__ = ["AREPackager"]


class AreFSH:
    """ara file directory structure
    """
    __DIR_ARA = "ara"
    __DIR_FRAMEWORK = "framework"
    __DIR_CORE = "core"
    __DIR_VAR = "var"
    __DIR_SWCLS = "swcls"
    __KEY_BASE = "base"
    __KEY_UCM = "ucm"
    __KEY_FRAMEWORK = "platform_framework"
    __KEY_CORE = "platform_core"
    __KEY_VAR = "platform_var"
    __KEY_SWCLS = "application_swcls"
    __KEY_FRAMEWORK_VERSION = "framework_version"
    __KEY_CORE_VERSION = "core_version"

    def __init__(self, sysroot):
        # ara-sysroot path
        self._sysroot_path = Path(sysroot)
        # ara-config path
        self._config_path = self._find_ara_config()
        # ara-config content
        self._config_data = None
        self.load()

    """
    Get the path of ara-sysroot.
    ara-sysroot is the root path of the ara runtime environment.
    Usually, ara-sysroot is separated from sdk-sysroot, so that different runtime environments can be created from the same sdk-sysroot.
    """
    def get_ara_sysroot(self):
        return self._sysroot_path

    """
    Get the path of sdk-sysroot.
    Usually, ara-sysroot is separated from sdk-sysroot, so that different runtime environments can be easily created from the same sdk-sysroot.
    """
    def get_sdk_sysroot(self):
        path = self.framework_path() / "../.."
        return path.resolve()


    """
    Get the currently active configuration file path
    """
    def get_config_path(self):
        return self._config_path

    """
    Reset to default configuration
    """
    def reset(self):
        self._config_data.pop(self.__KEY_BASE, None)

    
    def get_path(self, component:str) -> Path:
        """
        Get the path of the specified software set
        """
        if component == self.__DIR_FRAMEWORK:
            return self.framework_path()
        elif component == self.__DIR_CORE:
            return self.core_path()
        elif component == self.__DIR_SWCLS:
            return self.swcls_path()
        else:
            return None

    """
    Get or set the framework directory path
    """
    def framework_path(self, path=None):
        key_path = self.__KEY_BASE + "." + self.__KEY_FRAMEWORK
        # get option
        if None == path:
            value = self._get_value(key_path)
            if None == value:
                return self.get_ara_sysroot() / self.__DIR_ARA / self.__DIR_FRAMEWORK
            else:
                return Path(value) / self.__DIR_FRAMEWORK
        # get option
        else:
            return self._set_value(key_path, path)
            
    """
    Get or set the core directory path
    """
    def core_path(self, path=None):
        key_path = self.__KEY_BASE + "." + self.__KEY_CORE
        # get option
        if None == path:
            value = self._get_value(key_path)
            if None == value:
                return self.get_ara_sysroot() / self.__DIR_ARA / self.__DIR_CORE
            else:
                return Path(value) / self.__DIR_CORE
        # get option
        else:
            return self._set_value(key_path, path)
            
    """
    Get or set the var directory path
    """
    def var_path(self, path=None):
        key_path = self.__KEY_BASE + "." + self.__KEY_VAR
        # get option
        if None == path:
            value = self._get_value(key_path)
            if None == value:
                return self.get_ara_sysroot() / self.__DIR_ARA / self.__DIR_VAR
            else:
                return Path(value) / self.__DIR_VAR
        # get option
        else:
            return self._set_value(key_path, path)
            
    """
    Get or set the swcls directory path
    """
    def swcls_path(self, path=None):
        key_path = self.__KEY_BASE + "." + self.__KEY_SWCLS
        # get option
        if None == path:
            value = self._get_value(key_path)
            if None == value:
                return self.get_ara_sysroot() / self.__DIR_ARA / self.__DIR_SWCLS
            else:
                return Path(value) / self.__DIR_SWCLS
        # get option
        else:
            return self._set_value(key_path, path)
            
    """
    Get or set framework_version
    """
    def framework_version(self, version=None):
        key_path = self.__KEY_UCM + "." + self.__KEY_FRAMEWORK_VERSION
        # get option
        if None == version:
            return self._get_value(key_path)
        # get option
        else:
            return self._set_value(key_path, version)
            
    """
    Get or set core_version
    """
    def core_version(self, version=None):
        key_path = self.__KEY_UCM + "." + self.__KEY_CORE_VERSION
        # get option
        if None == version:
            return self._get_value(key_path)
        # get option
        else:
            return self._set_value(key_path, version)
        
    """
    Get the value based on the key path
    """
    def _get_value(self, key_path):
        try:
            ret = jsonpath(self._config_data, key_path)
            if False != ret:
                return ret[0]
            else:
                return None 
        except Exception as e:
            return None

    """
    Set the value based on the key path
    """
    def _set_value(self, key_path, value):
        try:
            # TODO: Support multi-level paths
            if None == self._config_data:
                self._config_data = {}
            key_list = key_path.split(".")
            if key_list[0] not in self._config_data:
                self._config_data[key_list[0]] = {}
            self._config_data[key_list[0]][key_list[1]] = value
            return True
        except Exception as e:
            # global_logger.error("ERROR on {fun}: {error}".format(fun=sys._getframe().f_code.co_name, error=e))
            return False


    """
    Load configuration
    """
    def load(self):
        try:
            with open(self._config_path, 'r') as f:
                self._config_data = json.load(f)
            return True
        except Exception as e:
            return False

    # Store configuration
    def store(self):
        try:
            with open(self._config_path, 'w') as f:
                # Note, strictly use json, not json5, because its lenient output mode may cause other modules to be unable to read it.
                json.dump(self._config_data, f, indent=4, sort_keys=True, ensure_ascii=False)
            return True
        except Exception as e:
            #print("ERROR on {fun}: {error}".format(fun=sys._getframe().f_code.co_name, error=e))
            return False

    """
    Find config configuration files.
    If multiple copies exist, return the configuration file with the largest version.
    """
    def _find_ara_config(self):
        try:
            file_match_list = []
            ara_path = self.get_ara_sysroot() / "ara"
            for f in os.listdir(ara_path):
                if re.match("ara_ver\\d+\\.json", f):
                    file_match_list.append(f)

            if 0 == len(file_match_list):
                raise Exception("no config file")

            file_match_list.sort(reverse=True)
            return ara_path / file_match_list[0]
        
        except Exception as e:
            #global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None
    
    def debug(self):
        print("+++ {}::Debug +++".format(__class__))
        print(self._config_data)
        print("AraSysroot: ", self.get_ara_sysroot())
        print("AraConfig:  ", self.get_config_path())
        print("Framework:  ", self.framework_path())
        print("Core:       ", self.core_path())
        print("Var:        ", self.var_path())
        print("Swcls:      ", self.swcls_path())
        print("FrameworkVersion: ", self.framework_version())
        print("CoreVersion:      ", self.core_version())
        print("--- {}::Debug ---".format(__class__))



class AREPackager:
    def __init__(self, args:AreArguments):
        self._args = args
        if self._args.OUTPUT is not None:
            self._output_dir = self._args.OUTPUT
        else:
            self._output_dir = global_fsh.OUTPUT_DIR
        self._output_dir = Path(self._output_dir)
        # ARE file name format
        self._are_file_name_format:str = "ARE-{sdk_version}.{build_version}-{target_arch}-{host_osv}-{toolchain}-{build_type}.{subfix}"
        self._ara_sysroot = Path(self._args.INPUT)
        self._are_import_fsh = AreFSH(self._ara_sysroot)
        self._sdk_sysroot = Path(self._are_import_fsh.get_sdk_sysroot())
        self._tmp_sysroot = self._output_dir / f"tmp-are-sysroot-{uuid.uuid4().hex}"
        self._are_cmd = self._ara_sysroot / "run.sh"
        self._are_install_size = 0
        self._are_release_info = None
        self._sdk_release_info = None
        # Temporary sysroot's FSH configuration needs to be used after copying the sysroot
        self._are_export_fsh = None
        self._are_export_config = None
        
        sdk_release_file = self._ara_sysroot / ".release.json"
        if sdk_release_file.exists():
            # If the release.json file exists, get information directly from it
            with open(sdk_release_file, "rt") as f:
                self._sdk_release_info = json5.load(f)


    def generate(self):
        """
        Generate ARE.

        Args:
        Returns:
            bool: Success or Failure
        """
        try:
            if not self._load_export_config():
                return False
            if AreExportType.ARE == self._args.TYPE:
                return self._generate_are()
            elif AreExportType.TAR == self._args.TYPE:
                return self._generate_tar()
            else:
                return False
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False




    def _is_export_component(self, component_name:str) -> bool:
        """Determine whether to export the component based on configuration

        Args:
            component_name (str): Component name

        Returns:
            bool
        """
        try:
            # If no export configuration is specified, export all by default
            if self._are_export_config is None:
                return True

            is_export = False
            if "components" in self._are_export_config \
                and component_name in self._are_export_config["components"] \
                    and "export" in self._are_export_config["components"][component_name]:
                        is_export = self._are_export_config["components"][component_name]["export"]
            return is_export

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    
    def _copy_ara_component(self, component:str) -> bool:
        """Copy software components, limited to core/framework/swcls

        Args:
            component (str): Software component. Optional names are: core/framework/swcls

        Returns:
            bool: true of false
        """
        try:
            if not self._is_export_component(component):
                global_logger.info(f"ignore export {component}")
                return True
            
            global_logger.info(f"Exporting {component} ...")
            comp_src_path = self._are_import_fsh.get_path(component)
            comp_dest_path = self._are_export_fsh.get_ara_sysroot() / f"ara/{component}"
            if not os.access(comp_src_path, os.F_OK):
                global_logger.warning(f"{comp_src_path} not exists !")
                return False
            # Copy files
            shutil.copytree(comp_src_path, comp_dest_path, dirs_exist_ok=True, symlinks=True)
            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
    

    
    def _copy_syslibs_to_framework(self) -> bool:
        """Copy all system libraries to the framework software set

        Returns:
            bool: True of False
        """
        try:
            lib_list = []
            # ARA_SYSROOT/usr/lib
            platform_version = self._are_import_fsh.framework_version()
            lib_list.append(self._are_import_fsh.get_sdk_sysroot() / "usr/lib")
            lib_list.append(self._are_import_fsh.get_sdk_sysroot() / "lib")
            # lib_list.append(self._are_import_fsh.framework_path() / f"{platform_version}/lib")
            # Copy all system libraries to the target path
            ara_libdir = self._tmp_sysroot / f"ara/framework/{platform_version}/lib"
            for libdir in lib_list:
                if libdir.is_dir():
                    shutil.copytree(libdir, ara_libdir, dirs_exist_ok=True, symlinks=True)
            
            # Fix broken symbolic links in the library, for the issue where symbolic links become invalid after copying the lib directory
            #make symlinks
            for dirpath, dirnames, filenames in os.walk(ara_libdir):
                for libfile in filenames:
                    libpath = os.path.join(dirpath, libfile)
                    if not os.path.islink(libpath):
                        continue
                    link = os.readlink(libpath)
                    # If the link path is not the current directory, delete the symbolic link and re-create it
                    if "" != os.path.dirname(link):
                        os.remove(libpath)
                        refName = os.path.basename(link)
                        # Only create the link if the target file is in the current directory
                        if os.path.exists(os.path.join(ara_libdir, refName)):
                            os.symlink(refName, os.path.join(ara_libdir, libfile))

            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False



    def _load_export_config(self) -> bool:
        """Load export configuration

        Returns:
            bool: true of false
        """
        try:
            config_file = self._args.CONFIG

            if config_file is None:
                return True

            if not os.path.exists(config_file):
                global_logger.error(f"no config file: {config_file}")
                return False

            with open(config_file, 'r') as f:
                self._are_export_config = json5.load(f)

            if not self._are_export_config.__contains__("ara-sysroot"):
                raise Exception("lack ara-sysroot")
            if not self._are_export_config.__contains__("components"):
                raise Exception("lack components")
            for component in ["framework", "core", "swcls", "var"]:
                if not self._are_export_config["components"].__contains__(component):
                    raise Exception("lack {}".format(component))
                if not self._are_export_config["components"][component].__contains__("export"):
                    raise Exception("lack {}.export".format(component))
            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False



    def _update_ara_config(self):
        try:
            # If no export configuration is specified, use the default configuration; no need to set
            if self._are_export_config is None:
                return True
            ###############  framework #################
            export_framework = self._are_export_config["components"]["framework"]
            export_path = None
            if export_framework.__contains__("path") and len(export_framework["path"]) > 0:
                export_path = export_framework["path"]
                global_logger.info("Set Framework Dir to: ", export_path)
            else:
                global_logger.info("Set Framework Dir to Default location")
            # Update export path
            self._are_export_fsh.framework_path(export_path)

            ###############  core #################
            export_core = self._are_export_config["components"]["core"]
            export_path = None
            if export_core.__contains__("path") and len(export_core["path"]) > 0:
                export_path = export_core["path"]
                global_logger.info("Set Core Dir to: ", export_path)
            else:
                global_logger.info("Set Core Dir to Default location")
            # Update export path
            self._are_export_fsh.core_path(export_path)

            ###############  swcls #################
            export_swcls = self._are_export_config["components"]["swcls"]
            export_path = None
            if export_swcls.__contains__("path") and len(export_swcls["path"]) > 0:
                export_path = export_swcls["path"]
                global_logger.info("Set Swcls Dir to: ", export_path)
            else:
                global_logger.info("Set Swcls Dir to Default location")
            # Update export path
            self._are_export_fsh.swcls_path(export_path)

            ###############  var #################
            export_var = self._are_export_config["components"]["var"]
            export_path = None
            if export_var.__contains__("path") and len(export_var["path"]) > 0:
                export_path = export_var["path"]
                global_logger.info("Set Var Dir to: ", export_path)
            else:
                global_logger.info("Set Var Dir to Default location")
            self._are_export_fsh.var_path(export_path)

            # Store modifications
            self._are_export_fsh.store()
            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def _gen_release_file(self):
        try:
            self._are_release_info = {}
            self._are_release_info["build_info"] = self._sdk_release_info["build_args"]
            self._are_release_info["sdk_version"] = self._sdk_release_info["apall_info"]["version"]
            self._are_release_info["source_hash"] = self._sdk_release_info["apall_info"]["commit_hash"]
            self._are_release_info["install_size"] = f"{self._are_install_size}MB"
            with open(self._tmp_sysroot / "release.json", "wt") as f:
                json.dump(self._are_release_info, f, indent=4, ensure_ascii=False, sort_keys=True)
            return True
        except Exception as e:
            #global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def _get_file_size(self, path):
        try:
            cmd = ["du", "-sm", path]
            proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
            return proc.stdout.readline().decode().strip().split()[0]
        except Exception as e:
            return 0

            
    def _strip_sysroot(self) -> bool:
        """Strip sysroot, delete unnecessary files, libraries, and strip ELF files.

        Returns:
            bool: True of False
        """

        try:
            # Delete unnecessary files in ARE
            rm_dirs = ["include", "share", "cmake", "pkgconfig", "man", "info", "doc", "ara-arxmls", "bintest"]
            rm_files = ["lib*.a", "*.la", "**/bin/validation", "**/bin/aragen"]
            # 
            if self._args.STRIP == AreStripLevel.ALL:
                strip_level = utils.SysrootManager.StripLevel.ALL
            elif self._args.STRIP == AreStripLevel.BIN:
                strip_level = utils.SysrootManager.StripLevel.BIN
            elif self._args.STRIP == AreStripLevel.FILE:
                strip_level = utils.SysrootManager.StripLevel.FILE
            else:
                strip_level = None
                
            sysman = utils.SysrootManager(self._tmp_sysroot)
            return sysman.strip(level=strip_level, rm_dirs=rm_dirs, rm_files=rm_files)
            
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
    

    def _copy_sysroot(self):
        try:
            # First copy the ara directory, excluding core/framework/swcls/var etc.
            ara_src_path = self._ara_sysroot / "ara"
            ara_dest_path = self._tmp_sysroot / "ara"
            # Callback function for copytree
            def _copyAraIgnoreCallback(dir, files):
                return ["core", "swcls", "var", "framework"]
            shutil.copytree(ara_src_path, ara_dest_path, symlinks=True, dirs_exist_ok=True, ignore=_copyAraIgnoreCallback)
            self._are_export_fsh = AreFSH(os.path.realpath(self._tmp_sysroot))
            self._are_export_fsh.reset()
            self._are_export_fsh.core_version(self._are_import_fsh.core_version())
            self._are_export_fsh.framework_version(self._are_import_fsh.framework_version())

            #self._are_import_fsh.debug()
            #self._are_export_fsh.debug()

            self._copy_ara_component("core")
            self._copy_ara_component("framework")
            self._copy_ara_component("swcls")
            # TODO: Determine if this is truly necessary. It seems it should not be needed.
            self._copy_syslibs_to_framework()

            # Update ara_ver*.json
            self._update_ara_config()
            return True
            
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
        

    def _gen_are_file_name(self, subfix):
        try:
            sdk_ver = self._sdk_release_info["apall_info"]["version"]
            build_ver = self._sdk_release_info["build_args"]["build_version"]
            arch = utils.SysrootManager(self._sdk_sysroot).get_arch()
            tc_name = self._sdk_release_info["build_args"]["toolchain"]
            if "native" == tc_name:
                os_ver = utils.get_os_version()
            else:
                os_ver = "linux"
            build_type = self._sdk_release_info["build_args"]["build_type"]

            # ARE-{sdk_version}.{build_version}-{target_arch}-{host_osv}-{toolchain}-{build_type}.{subfix}
            pack_name = self._are_file_name_format.format(\
                sdk_version = sdk_ver,\
                build_version = build_ver,\
                target_arch = arch,\
                host_osv = os_ver,\
                toolchain = tc_name,\
                build_type = build_type,\
                subfix=subfix)
            return pack_name

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None



    def _generate_tar(self) -> bool:
        """Generate tar format ARE package
        """
        try:
            if True != os.access(self._ara_sysroot, os.F_OK):
                global_logger.error(f"the SysRoot '{self._ara_sysroot}' is not exists !")
                return False

            #copy to tmp
            global_logger.info("Copying sysroot ...")
            self._copy_sysroot()
            
            # Strip sysroot
            self._strip_sysroot()

            #copy are-cmd
            shutil.copy(self._are_cmd, self._tmp_sysroot)
            
            # Get ARE installation size
            self._are_install_size = self._get_file_size(self._tmp_sysroot)

            # Generate release file
            self._gen_release_file()

            #create tarball
            global_logger.info("Make Archive ...")
            are_tarball_path = self._output_dir / self._gen_are_file_name("tar.gz")
            os.makedirs(self._output_dir, exist_ok=True)
            utils.compress_file(self._tmp_sysroot, are_tarball_path, strip_topdir=True)
            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

        finally:
            # Delete temporary working directory
            if True == os.access(self._tmp_sysroot, os.F_OK) and not global_fsh.DEBUG:
                if global_fsh.DEBUG:
                    global_logger.verbose(f"rmdir: {self._tmp_sysroot}[ignore for debug]")
                else:
                    global_logger.verbose(f"rmdir: {self._tmp_sysroot}")
                    shutil.rmtree(self._tmp_sysroot)


    def _generate_are(self) -> bool:
        """Generate standard self-extracting format ARE

        Returns:
            bool: _description_
        """
        try:
            are_file_path = self._output_dir / self._gen_are_file_name("run")
            are_tarball_path = self._output_dir / self._gen_are_file_name("tar.gz")
            # First generate the tar package
            self._generate_tar()

            # Copy the file header
            shutil.copy(global_fsh.TEMPLATE_DIR / "are-header.template", are_file_path)
            
            # Render the header file
            sysroot_dir = ""
            if self._are_export_config is not None:
                sysroot_dir = self._are_export_config["ara-sysroot"]["path"]
            are_size = os.path.getsize(are_tarball_path)
            release_info = json.dumps(self._are_release_info, indent=4, ensure_ascii=False, sort_keys=False)
            value_mapping = [
                ("ARA_SYSROOT", sysroot_dir),
                ("PACK_SIZE", str(are_size)),
                ("RELEASE_INFO", release_info)
            ]
            utils.render_template_file(are_file_path, value_mapping)
            
            # Append the ARE archive
            with open(are_file_path, "ab") as fd:
                with open(are_tarball_path, "rb") as fs:
                    fd.write(fs.read())
            os.chmod(are_file_path, 0o755)
            # Delete the temporarily generated ARE archive
            os.remove(are_tarball_path)
            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

            
def _tests():
    af = AreFSH("/home/fj/test/ara-sysroot/")
    af.framework_path("/tmp/test/ara")
    af.store()
    
if __name__ == "__main__":
    _tests()