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
AraGenerator
"""
import datetime
import glob
import json
import logging
import os
import subprocess
import sys

from tools.araGenerator.araGenerator_settings import AraGeneratorSettings

ARA_GEN_FILE_PATH_IN_CORE = "bin/aragen"
ARA_DIR_NAME = "ara"
MACHINES_ROOT_DIR_NAME = "machines"
MACHINE_MANIFEST_FILE_NAME = "machine_manifest.json"

ARA_VER_CFG_BASE_SECTION_NAME = "base"
ARA_VER_CFG_UCM_SECTION_NAME = "ucm"
PLATFORM_FRAMEWORK_NAME = "platform_framework"
FRAMEWORK_VERSION_NAME = "framework_version"

FRAMEWORK_SWCL_NAME = "framework"

# TODO hanzhibo  Generator 应该只是对 ara-gen的不含专用业务的包装--subprocess.call（不需要 _generate_manifest） 并且可放到 ara-tools/ara-Generator 做为ara-tools公共类库使用，
#  使用方(如configMachine.py)在调用后  对输出做处理,   如 configMachine.py 增加 -w --workdir 目录，  对应到ara-tools/ara-Generator 的-o参数， 不需要 TMP_DIR_4_ARA_GEN MACHINES_ROOT_DIR_NAME，然后把 -w中的文件做相关的处理

class AraGenerator:
    """
    AraGenerator.
    """

    def __init__(self, args, args2AraGen):
        self._settings = AraGeneratorSettings(args)
        self._log = logging.getLogger(__name__)
        self._args2AraGen = args2AraGen

    # @handle_method_exceptions(error_string_template="machine manifest generation ERROR: {exception_text}")
    # def _generate_manifest(self, workdir):
        # self._log.debug("_generate_manifest, workdir:%s", workdir)

        # self._log.debug("_generate_manifest, ara_sysroot:%s", self._settings.ara_sysroot)
        # self._log.debug("_generate_manifest, machine:%s", self._settings.machine)
        # self._log.debug("_generate_manifest, files:%s", self._settings.files)
        # ara_gen_path = os.path.join(self._settings.ara_sysroot, ARA_GEN_FILE_PATH_IN_ARA_SYSROOT)
        # self._log.debug("_generate_manifest, ara_gen_path:%s", ara_gen_path)
        # # retcode = subprocess.call([ara_gen_path, '-m', *(self._settings.machine), '-o', self._settings.output_path, *(self._settings.files)])
        # retcode = subprocess.call([ara_gen_path, '-m', self._settings.machine, '-o', workdir, '--someip-libs=nsomeip', *(self._settings.files)])
        # self._log.debug("_generate_manifest, retcode:%d", retcode)
        # if(0 != retcode):
        #     self._log.error("_generate_manifest, failed to call ara_gen_path:%s with retcode:%d", ara_gen_path, retcode)
        #     sys.exit("error: failed to call ara_gen_path:{0} with retcode:{1} when _generate_manifest.\n".format(ara_gen_path, retcode))

    def _log_invocation_details(self):
        config_message = "AraGenerator started with:\n\t"
        config_message += "\n\t".join(self._settings.configuration_dump)
        config_message += "\n\t"
        config_message += "\n\t".join(self._args2AraGen)
        config_message = config_message.rstrip()

        self._log.debug(config_message)

    def generate(self,ara_gen_path = None,fail_exit = True):
        """
        Generates files.
        """
        self._log_invocation_details()

        # # 与配置机器时一致:若有多个版本的软件集,则为高版本有效。后面可调整为使用指定版本的core软件集
        
        
        #self._log.debug("generate, ara_sysroot:%s", self._settings.ara_sysroot)
        if ara_gen_path == None:
            ara_gen_path = self.getAraGenPathFromAraSysroot()
        self._log.info("generate, ara_gen_path:%s", ara_gen_path)
        
        # 判断ara_gen_path是否有效
        if not os.path.exists(ara_gen_path):
            self._log.error("generate, Can't find ara_gen_path:%s, so we will exit.", ara_gen_path)
            sys.exit("error: Please check the ara_gen_path:{}.\n".format(ara_gen_path))
        
        # ara_gen_path = "/home/hanzhibo/ap-all/isoft/ara-gen/aragen"
        command_line = ara_gen_path + ' ' + ' '.join(self._args2AraGen)
        self._log.info("begin ara-gen call:%s", command_line)
        starttime = datetime.datetime.now()
        # completedProcess = subprocess.run([ara_gen_path, *(self._args2AraGen)], capture_output=True, encoding='UTF-8')
        completedProcess = subprocess.run([ara_gen_path, *(self._args2AraGen)], stderr=subprocess.STDOUT,stdout=subprocess.PIPE, encoding='UTF-8')
        endtime = datetime.datetime.now()
        self._log.info("end ara-gen call returncode:%d at %d seconds", completedProcess.returncode,(endtime - starttime).seconds)
        
        if(0 != completedProcess.returncode):
            out = "generate, failed to call ara_gen_path:{} {} with completedProcess.returncode:{} \n{} ".format( ara_gen_path, " ".join(self._args2AraGen),completedProcess.returncode,completedProcess.stdout)
            self._log.error(out)
            if fail_exit:
                sys.exit(1)
            else:
                return out
        else:
            self._log.debug("generate, completedProcess.stdout:%s", completedProcess.stdout)
            return completedProcess.stdout
    
    
    # 将来调用aragen应通过generate的ara_gen_path参数指定路径而不应该通过ara_sysroot去查找,如:打包将来也是在工具中,工具当然知道framework软件集的路径.
    def getAraGenPathFromAraSysroot(self):
        """
        read the dir config for the ara in ara_sysroot and get the ara gen path.
        """
        if not self._settings.ara_sysroot:
            self._log.error("getAraGenPathFromAraSysroot, the araGenerator need to be given an directory as ara_sysroot.")
            sys.exit("error: the following argument is needed: -r/--ara_sysroot, directory.\n")
        
        araDir = os.path.join(self._settings.ara_sysroot, ARA_DIR_NAME)
        #   根据原来ara_verX.json(若有,取X最大的)的内容,获取framework软件集,core软件集,swcls软件集,var文件夹所在的位置的配置
        list_of_ara_ver_cfg_files = sorted( filter( os.path.isfile, glob.glob(os.path.join(araDir,'ara_ver*.json'))) )
        self._log.info("getAraGenPathFromAraSysroot, list_of_ara_ver_cfg_files:%s", list_of_ara_ver_cfg_files)
        
        # 读取platform_framework,platform_core,application_swcls,platform_var
        platform_framework = ""
        if(len(list_of_ara_ver_cfg_files) > 0):
            # 取X最大的ara_verX.json
            max_version_ara_ver_cfg_file = list_of_ara_ver_cfg_files[-1]
            self._log.debug("getAraGenPathFromAraSysroot, max_version_ara_ver_cfg_file:%s", max_version_ara_ver_cfg_file)

            # 解析ara_verX.json
            with open(max_version_ara_ver_cfg_file) as user_file:
                file_contents = user_file.read()
            config = json.loads(file_contents)
            self._log.debug("getAraGenPathFromAraSysroot, get config:%s", config)
            
            if ARA_VER_CFG_BASE_SECTION_NAME in config:
                # 读取platform_framework
                if PLATFORM_FRAMEWORK_NAME in config[ARA_VER_CFG_BASE_SECTION_NAME]:
                    platform_framework = config[ARA_VER_CFG_BASE_SECTION_NAME][PLATFORM_FRAMEWORK_NAME]
            
            # 读取framework_version
            if ARA_VER_CFG_UCM_SECTION_NAME in config and FRAMEWORK_VERSION_NAME in config[ARA_VER_CFG_UCM_SECTION_NAME]:
                framework_version = config[ARA_VER_CFG_UCM_SECTION_NAME][FRAMEWORK_VERSION_NAME]
            else:
                self._log.error("getAraGenPathFromAraSysroot, can't find %s option in max_version_ara_ver_cfg_file:%s", FRAMEWORK_VERSION_NAME, max_version_ara_ver_cfg_file)
                sys.exit(
                    "error: can't find {0} option in max_version_ara_ver_cfg_file:{1} when getAraGenPathFromAraSysroot\n".format(FRAMEWORK_VERSION_NAME, max_version_ara_ver_cfg_file))
        else:
            self._log.error("getAraGenPathFromAraSysroot, can't find ara_ver*.json in araDir:%s", araDir)
            sys.exit(
                "error: can't find ara_ver*.json in araDir{0}, please config machine at first.\n".format(araDir))

        self._log.debug("getAraGenPathFromAraSysroot, got platform_framework:%s", platform_framework)
        self._log.debug("getAraGenPathFromAraSysroot, got framework_version:%s", framework_version)
        
        if (len(platform_framework) == 0):
            self._framework_dir = araDir
        else:
            self._framework_dir = platform_framework
        
        self._log.debug("getAraGenPathFromAraSysroot, got self._framework_dir:%s", self._framework_dir)

        # 得到aragen路径
        araGenPath = os.path.join(self._framework_dir, FRAMEWORK_SWCL_NAME, framework_version, "bin", "aragen")
        self._log.debug("getAraGenPathFromAraSysroot, got araGenPath:%s", araGenPath)
        
        return araGenPath