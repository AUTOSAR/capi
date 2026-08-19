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
# @file       sdk.py
# @brief      SDK packager
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import sys, os, shutil, json
import copy
from pathlib import Path
from typing import Dict
import importlib
import subprocess

__SDK_UTILS_SRC_DIR = str(Path(__file__).resolve().parent.parent)
if __SDK_UTILS_SRC_DIR not in sys.path:
    sys.path.insert(0, __SDK_UTILS_SRC_DIR)

from core import Fsh, global_fsh
from core  import Logger, global_logger
import utils

__all__ = ["SDKPackager", "SDKUpdater"]


class SDKPackager:
    def __init__(self, input_dir:Path=None):
        try:
            # File directory structure handle
            self._fsh:Fsh = global_fsh
            self._input_dir = input_dir
            # Passed-in sysroot path
            if input_dir is None:
                self._sysroot:Path = global_fsh.SYSROOT_DIR
                # Temporary working directory needed to generate SDK
                self._work_path:Path = self._fsh.BUILD_DIR / "tmp-sdk-packager"
            else:
                self._sysroot:Path = Path(input_dir) / "ara-sysroot"
                self._work_path:Path = Path(input_dir) / "tmp-sdk-packager"
            # SDK release information
            self._release_info:Dict = None
            if not self._load_release_info():
                return
            # SDK file header template path
            self._sdk_header_template_fpath:Path = self._fsh.TEMPLATE_DIR / "sdk-header.template"
            # Directory containing the SDK content
            self._sdk_content_dir:Path = self._work_path / "sdk"
            # Name of the SDK content archive
            self._sdk_content_tarball_file:Path = self._work_path / "sdk.tar"
            # SDK file name format
            self._sdk_file_name_format:str = "SDK-{sdk_version}.{build_version}-{target_arch}-{host_osv}-{toolchain}-{build_type}.run"
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return


    def _load_release_info(self):
        """Load release information from sysroot."""

        try:
            release_fpath = self._sysroot / global_fsh.RELEASE_FILE_NAME
            with open(release_fpath, 'r') as f:
                self._release_info = json.load(f)
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

    def _save_release_info(self, release_fpath:Path):
        """Save release information."""
        try:
            with open(release_fpath, 'w') as f:
                json.dump(self._release_info, f, indent=4)
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
        
    def _gen_sdk_name(self):
        """Generate SDK file name."""

        try:
            sdk_name = self._sdk_file_name_format.format(\
                    sdk_version = self._release_info["apall_info"]["version"], \
                    build_version = self._release_info["build_args"]["build_version"], \
                    target_arch=utils.SysrootManager(self._sysroot).get_arch(),
                    host_osv = utils.get_os_version(), \
                    toolchain = self._release_info["build_args"]["toolchain"], \
                    build_type = self._release_info["build_args"]["build_type"], \
            )
            return sdk_name

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None


    def generate(self, output_dir:Path=None, extract=False):
        """
        Generate SDK.

        Args:
            output_dir: Generated SDK output path
            extract: Whether to extract Debug information into separate files
        Returns:
            bool: Success or Failure
        """
        try:
            if None is output_dir:
                output_dir = self._fsh.OUTPUT_DIR
            else:
                output_dir = Path(output_dir)

            os.makedirs(self._work_path, exist_ok=True)
            os.makedirs(output_dir, exist_ok=True)
            print(self._work_path)

            # Export toolchain
            global_logger.info("exporting toolchain ...")
            if not self._export_toolchain():
                global_logger.error("exporting toolchain")
                return False
            global_logger.info("exporting toolchain ... Done")
           
            # Export sysroot
            global_logger.info("copying sysroot ...")
            shutil.copytree(src=self._sysroot, dst=self._sdk_content_dir/"ara-sysroot", symlinks=True)
            global_logger.info("copying sysroot ... Done")
            
            # Export build environment
            global_logger.info("generating compile script ...")
            self._gen_build_environment()
            global_logger.info("generating compile script ... Done")

            # Export debug information
            if extract:
                global_logger.info("exporting debuginfo ...")
                if not self._export_debug_info(output_dir):
                    global_logger.error("export debuginfo")
                    return False
                global_logger.info("exporting debuginfo ... Done")

            # Package SDK
            global_logger.info("packing SDK ...")
            if not self._gen_sdk_package(output_dir):
                global_logger.error("packing SDK")
                return False
            global_logger.info("packing SDK ... Done")
            global_logger.info(f"SDK file path is: {output_dir.absolute()}")
                
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
        finally:
            if global_fsh.DEBUG:
                global_logger.verbose(f"rmdir: {self._work_path}[ignore for debug]")
            else:
                global_logger.verbose(f"rmdir: {self._work_path}")
                shutil.rmtree(self._work_path, ignore_errors=True)
    

    def _export_toolchain(self):
        """Export the build toolchain."""
        try:
            tc_name = self._release_info["build_args"]["toolchain"]
            # Create a toolchain instance based on the obtained toolchain name
            if None is tc_name:
                return False
            tool = importlib.import_module("toolchain." + tc_name)
            toolchain = eval("tool.Compiler")()

            if not hasattr(toolchain.package, "__call__"):
                raise Exception(f"toolchain {tc_name} has no 'package' method")
            toolchain_export_dir = self._sdk_content_dir / "toolchain"
            os.makedirs(toolchain_export_dir, 0o755, exist_ok=True)
            input_dir = self._input_dir
            if input_dir is not None:
                input_dir = Path(input_dir) / "toolchain"
            return toolchain.package(toolchain_export_dir, input_dir)
            
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def _gen_build_environment(self):
        try:
            # Generate VSCode configuration file for the SDK
            self._gen_vscode_setting()
            self._gen_compile_script()
            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False

    
    def _gen_vscode_setting(self):
        """Generate VSCode configuration, can be used to directly use the installed SDK in VSCode."""

        # Considerations for generating vscode configuration:
        #   environs cannot reference other variables
        #   Using environment variables in cmake_args requires the ${env:KEY} format
        #   cmake_args cannot use CMake lists (multiple variables separated by ;)
        #   cmake_args placed in environment variables will also take effect
        #   The separator in environment variable lists must be ':'
        # Design approach:
        #   Use plain text entirely
        #   Variables containing ; in cmake_args are moved to environment variables, and CMake list separators are replaced with ':'
        #   All references to environment variables in cmake_args are replaced with ${env:KEY}
        return True
        vscode_setting_file = self._work_path / "ara-sysroot/vscode-settings.json"

        environs = copy.deepcopy(self._release_info["build_args"]["envrion"])
        if "PATH" in environs:
            environs["PATH"] = "{bin}:{sys_bin}".format(bin=environs["PATH"], sys_bin="${env:PATH}")
        cmake_args = {}
        
        # Separate, move variables containing semicolons to environs
        origin_cmake_args = self._release_info["build_args"]["cmake_args"]
        for key in origin_cmake_args:
            # Replace all sysroot paths with variables for easy substitution later.
            value = origin_cmake_args[key].replace(str(self._fsh.SYSROOT_DIR), "${ARA_SYSROOT}")
            if ";" in value:
                environs[key] = value.replace(";", ":")
            else:
                cmake_args[key] = value
        
        # Replace, convert the way environment variables are referenced in cmake_args
        for key in cmake_args:
            value = cmake_args[key]
            if "${" in value:
                cmake_args[key] = value.replace("${", "${env:")
            
        # Replace all references to environment variables with plain text, because vscode's environment variables do not support referencing other environment variables.
        for key in environs:
            if "${ARA_SYSROOT}" in environs[key]:
                environs[key] = environs[key].replace("${ARA_SYSROOT}", str(self._fsh.SYSROOT_DIR))

        # For configuration files generated for the abe installation package, replace configurable paths with flag strings for easy replacement
        # Some paths are configured starting with workdir (e.g., LD_LIBRARY_PATH for certain compilers), need to be replaced with relative paths to ara-sysroot.
        workdir = os.path.dirname(self._ara_sysroot)
        for key in environs:
            value = environs[key]
            value = value.replace(self._ara_sysroot, "@SYSROOT@")
            value = value.replace(workdir, "@SYSROOT@/../")
            environs[key] = value

        vscode_obj = {}
        # ap-all needs to define ARA_SYSROOT via parameters
        vscode_obj["cmake.configureSettings"] = cmake_args
        vscode_obj["cmake.environment"] = environs
        vscode_obj["C_Cpp.default.configurationProvider"] = "ms-vscode.makefile-tools"
        vscode_obj["C_Cpp.default.includePath"] = [\
            "${workspaceFolder}/**", \
            "@SYSROOT@/ara/framework/{ver}/include".format(ver=self._platform_version), \
            "@SYSROOT@/usr/include"]
        cmakeDebugConfig = {}
        cmakeDebugConfig["environment"] = [
                {
                    "name": "LD_LIBRARY_PATH",
                    "value": "@SYSROOT@/ara/framework/{ver}/lib:@SYSROOT@/usr/lib".format(ver=self._platform_version)
                }
        ]
        vscode_obj["cmake.debugConfig"] = cmakeDebugConfig

        try:
            with open(fpath, "wt") as f:
                json.dump(vscode_obj, f, indent=4)
        except Exception as e:
            print("write vscode_obj: ", e)


    def _replace_path_to_expression(self, src_str:str) -> str:
        """Replace actual paths with reference expressions for generating templates."""

        ret = str(src_str)
        ret = ret.replace(f"{self._fsh.SYSROOT_DIR}", "${SYSROOT_DIR}")
        ret = ret.replace(f"{self._fsh.TOOLCHAIN_DIR}", "${TOOLCHAIN_DIR}")
        return ret


    def _gen_compile_script(self):
        """Generate build script."""

        try:
            # Copy the SDK build script template
            script_path = self._sdk_content_dir / "ara-sysroot/build.sh"
            shutil.copy(dst=script_path, src=self._fsh.TEMPLATE_DIR / "sdk-build.sh.template")
            os.chmod(script_path, 0o755)
            
            value_mapping = [
                ("CMAKE_ARGS", self._replace_path_to_expression(self._release_info["build_args"]["cmake_args"])),
            ]
            utils.render_template_file(script_path, value_mapping)
            
            script_path = self._sdk_content_dir / "ara-sysroot/config.sh"
            shutil.copy(dst=script_path, src=self._fsh.TEMPLATE_DIR / "ara-config.sh")
            os.chmod(script_path, 0o755)

            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def _gen_sdk_package(self, output_dir:Path):
        """Generate SDK file."""
        try:
            # Update SDK release information
            sdk_name = self._gen_sdk_name()
            self._release_info["SDK_NAME"] = sdk_name.rsplit(".", 1)[0]
            self._release_info["SDK_IDE_API_VERSION"] = "4.0"
            self._release_info["ORIENTAIS_TOOL_INSTALLATION_PATH"] = "${ARA_SYSROOT}/ara-tools"
            self._release_info["SDK_INSTALL_SIZE"] = utils.get_disk_usage(self._sdk_content_dir)
            self._save_release_info(self._sdk_content_dir / "ara-sysroot" / global_fsh.RELEASE_FILE_NAME)
            
            # Compress
            sdk_tarball_size = utils.compress_file(src=self._sdk_content_dir, dst=self._sdk_content_tarball_file, strip_topdir=True)
            if 0 == sdk_tarball_size:
                raise Exception("compress_file")

            # Copy the SDK file header template
            sdk_file_path = output_dir / sdk_name
            shutil.copy(dst=sdk_file_path, src=self._sdk_header_template_fpath)

            value_mapping = [
                ("SDK_BODY_SIZE", str(sdk_tarball_size)),
                ("TOOLCHAIN_BIN_DIR", self._release_info["build_args"]["environ"]["PATH"]),
                ("RELEASE_INFO", json.dumps(self._release_info, indent=4, ensure_ascii=False, sort_keys=False) ),
            ]
            utils.render_template_file(sdk_file_path, value_mapping)

            # Append archive
            with open(sdk_file_path, "ab") as fd:
                with open(self._sdk_content_tarball_file, "rb") as fs:
                    fd.write(fs.read())
            os.chmod(sdk_file_path, 0o755)
            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False


    def _export_debug_info(self, output_dir:Path):
        """
        Export debug information: remove debugging information from all ELF files in the SDK and export it to separate files to reduce SDK size.
        
        Adopts a modern build-id deployment scheme.
        """

        try:
            sysroot = self._sdk_content_dir / "ara-sysroot"
            debug_dir = self._work_path / "debug"
            sdk_file_name_prefix = self._gen_sdk_name().rsplit(".", 1)[0]
            debug_info_fpath = output_dir / (sdk_file_name_prefix + ".debuginfo.tgz")
            extractor = utils.SysrootManager(sysroot)
            ret = extractor.extract_debug_info(debug_dir)
            if None is ret:
                return False
            global_logger.info(f"export debug info: {ret[0]} processed, {ret[1]} failed, {ret[2]} skipped")
            utils.compress_file(src=debug_dir, dst=debug_info_fpath, strip_topdir=True)
            if global_fsh.DEBUG:
                global_logger.verbose(f"rmdir: {debug_dir}[ignore for debug]")
            else:
                global_logger.verbose(f"rmdir: {debug_dir}")
                shutil.rmtree(debug_dir, ignore_errors=True)
            return True

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False



class SDKUpdater:
    """
    SDK updater, used to update SDK file content.
    
    Typical usage scenario is to encrypt/authorize the SDK (e.g., replace the encrypted aragen).
    """

    def __init__(self, sdk_fpath:Path):
        try:
            # SDK source file
            self._sdk_fpath = Path(sdk_fpath)
            # SDK archive size
            self._sdk_body_size = None
            # SDK file header content
            self._sdk_header = None
            # SDK output directory
            self._output_path = None
            # File directory structure handle
            self._fsh:Fsh = global_fsh
            # Temporary working directory
            self._work_path:Path = self._fsh.WORK_DIR / "tmp-sdk-packager"
            # SDK content directory
            self._sdk_content_dir:Path = self._work_path / "sdk"
            # SDK archive name
            self._sdk_content_tarball_file:Path = self._work_path / "sdk.tar"
        except Exception as e: 
            global_logger.error(e)


    def update(self, update_dir:Path, output_dir:Path=None):
        """
        Generate (update) SDK.

        Args:
            output_dir: Output directory; if not specified, defaults to the same directory as the original file, and overwrites the original file.
        Returns:
            bool: True of False
        """

        try:
            if None is output_dir:
                self._output_path = self._sdk_fpath.parent
            else:
                self._output_path = Path(output_dir)

            self._update_path = Path(update_dir).absolute()
            os.makedirs(self._work_path, exist_ok=True)
            os.makedirs(self._update_path, exist_ok=True)

            # Extract SDK
            global_logger.info("unpacking SDK ...")
            if not self._unpack_sdk_file():
                global_logger.error("unpack sdk file")
                return False 
            global_logger.info("unpacking SDK ... done")
            
            # Update content
            global_logger.info("updating SDK ...")
            if not self._do_update():
                global_logger.error("update sdk")
                return False
            global_logger.info("updating SDK ... done")
            
            # Compress and package
            global_logger.info("packing SDK ...")
            if not self._pack_sdk_file():
                global_logger.error("pack sdk file")
                return False
            global_logger.info("packing SDK ... Done")
            global_logger.info(f"SDK file path is: {self._output_path.absolute()}")
                
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
        finally:
            if global_fsh.DEBUG:
                global_logger.verbose(f"rmdir: {self._work_path}[ignore for debug]")
            else:
                global_logger.verbose(f"rmdir: {self._work_path}")
                shutil.rmtree(self._work_path, ignore_errors=True)
    
    
    def _unpack_sdk_file(self):
        """Load original SDK file information."""

        try:
            # Get SDK archive size
            with open(self._sdk_fpath, "rb") as f:
                for l in f:
                    kv = l.decode().split("=")
                    if "SDK_BODY_SIZE" == kv[0]:
                        self._sdk_body_size = int(kv[1].strip())
                        break

            # Save SDK header
            sdk_file_size = os.stat(self._sdk_fpath).st_size
            sdk_header_size = sdk_file_size - self._sdk_body_size
            #print(sdk_head_size)
            with open(self._sdk_fpath, "rb") as f:
                sdk_head_raw = f.read(sdk_header_size)
                self._sdk_header = sdk_head_raw.decode().splitlines(True)
            
            os.makedirs(self._sdk_content_dir, exist_ok=True)
            # Extract SDK content
            cmd = "tail -c {data_size} {sdk_file} | tar xz -C {install_dir}".format(data_size=self._sdk_body_size,
                                                                                    sdk_file=self._sdk_fpath,
                                                                                    install_dir=self._sdk_content_dir)
            os.system(cmd)
            return True
        except Exception as e:
            print(e)
            return False
    
    
    def _do_update(self):
        """Perform update operation."""

        try:
            tmp_update_dir = self._sdk_content_dir / "update"
            tmp_update_script = tmp_update_dir / "update.sh"
            shutil.copytree(src=self._update_path, dst=tmp_update_dir)
            os.chmod(tmp_update_script, 0o775)
            if 0 != subprocess.call(str(tmp_update_script), shell=True, cwd= tmp_update_dir):
                raise Exception(tmp_update_script)
            return True
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return False
        finally:
            if global_fsh.DEBUG:
                global_logger.verbose(f"rmdir: {self.tmp_update_dir}[ignore for debug]")
            else:
                global_logger.verbose(f"rmdir: {self.tmp_update_dir}")
                shutil.rmtree(tmp_update_dir)


    def _pack_sdk_file(self):
        """Package SDK file"""

        try:
            sdk_name = self._sdk_fpath.name
            os.makedirs(self._output_path, exist_ok=True)

            # Create the updated archive
            sdk_tarball_size = utils.compress_file(src=self._sdk_content_dir, dst=self._sdk_content_tarball_file, strip_topdir=True)
            if 0 == sdk_tarball_size:
                raise Exception("compress_file")

            # Update SDK header information
            sdk_head_lines_new = []
            for l in self._sdk_header:
                if "SDK_BODY_SIZE" == l.split("=")[0]:
                    l = "SDK_BODY_SIZE={}\n".format(sdk_tarball_size)
                sdk_head_lines_new.append(l)

            # Generate new SDK, write the new SDK header to the file, append the archive at the end of the file
            sdk_new_path = self._output_path / sdk_name
            with open(sdk_new_path, "w") as f:
                f.writelines(sdk_head_lines_new)

            sdk_new_fp = open(sdk_new_path, "ab")
            with open(self._sdk_content_tarball_file, "rb") as f:
                for l in f:
                    sdk_new_fp.write(l)
            sdk_new_fp.close()

            return True
        except Exception as e:
            global_logger.error(e)
            return False

        finally:
            os.remove(self._sdk_content_tarball_file)
        

def _tests():
    """
    from config  import CmdLineParser
    args = CmdLineParser().pack_args
    if None is args:
        return False
    if None is not args.OUTPUT:
        output = args.OUTPUT
    else:
        output = global_fsh.OUTPUT_DIR
    global_logger.init(error_file=f"{output}/error.log", out_file=f"{output}/out.log")
    sp = SDKPackager(args.SYSROOT)
    sp.generate(args.OUTPUT, args.EXTRACT)
    """
    su = SDKUpdater("./var/output/SDK-1.0.0.2608-x86_64-ubuntu20.04-native-Debug.run")
    su.update("./example/update", "./u")


if __name__ == "__main__":
    _tests()
