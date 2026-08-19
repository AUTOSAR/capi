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
Generator Settings
"""

import logging

ARA_DIR_NAME = "ara"

class AraGeneratorSettings(object):
    """
    Convenience class for storing and accessing a set of settings for the araGenerator.
    """

    def __init__(self, args):
        self._args = args
        self._log = logging.getLogger(__name__)

        # if not self.ara_sysroot:
        #     self._log.error("The araGenerator must be given an directory as ara_sysroot.")
        #     sys.exit("error: the following arguments are required: -r/--ara_sysroot, directory.\n")

        # # self._log.info('AraGeneratorSettings.__init__, self.ara_sysroot:%s', self.ara_sysroot)

        # # 检查ara_sysroot的合法性
        # if not os.path.exists(os.path.join(self.ara_sysroot, ARA_DIR_NAME)):
        #     self._log.error("Can't find %s directory in ara_sysroot:%s, so we will exit.", ARA_DIR_NAME, self.ara_sysroot)
        #     sys.exit("error: Please check the ara_sysroot.\n")

        # # workdir合法性检查
        # if not self.workdir:
        #     self._log.error("The araGenerator must be given an directory as workdir.")
        #     sys.exit("error: the following arguments are required: -w/--workdir, directory.\n")
            
        # self._log.info('AraGeneratorSettings.__init__, self.workdir:%s', self.workdir)
        
        # files合法性检查
        # if not self.files:
        #     self._log.error("The araGenerator must be given a list of files that will be input for the generator.")
        #     sys.exit("error: the following arguments are required: files.\n")

    def __getattr__(self, item):
        # pass through getters to underlying args object
        return getattr(self._args, item)

    @property
    def configuration_dump(self):
        result = []
        # result += ["debug: {}".format(self.debug)]
        # result += ["quiet: {}".format(self.quiet)]
        # result += ["verbose: {}".format(self.verbose)]
        # result += ["ara_sysroot: {}".format(self.ara_sysroot)]
        # result += ["target_machines: {}".format(self.machines)]
        # result += ["workdir: {}".format(self.workdir)]
        # result += ["files: {}".format(self.files)]
        return result

