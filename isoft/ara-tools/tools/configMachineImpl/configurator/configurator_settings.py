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
Configurator Settings
"""

import logging
import os
import sys

ARA_DIR_NAME = "ara"
FRAMEWORK_DIR_NAME = "framework"

class ConfiguratorSettings(object):
    """
    Convenience class for storing and accessing a set of settings for the configurator.
    """

    def __init__(self, args):
        self._args = args
        self._log = logging.getLogger(__name__)

        if not self.ara_sysroot:
            self._log.error("The configMachine must be given an directory as ara_sysroot")
            sys.exit("error: the following arguments are required: -r/--ara_sysroot, directory\n")

        # Check validity of ara_sysroot
        if not os.path.exists(self.ara_sysroot):
            self._log.error("Can't find ara_sysroot:%s, so we will exit.", self.ara_sysroot)
            sys.exit("error: Please check the ara_sysroot.\n")

        # Check validity of withSDK
        self._log.info('ConfiguratorSettings.__init__, self.withSDK:%s', self.withSDK)
        if self.withSDK:
            if not os.path.exists(os.path.join(self.withSDK, ARA_DIR_NAME, FRAMEWORK_DIR_NAME)):
                self._log.error("Can't find %s/%s directory in withSDK:%s, so we will exit.", ARA_DIR_NAME, FRAMEWORK_DIR_NAME, self.withSDK)
                sys.exit("error: Please check the withSDK.\n")
                
        # Check canUseExistingExe
        self._log.info('ConfiguratorSettings.__init__, self.canUseExistingExe:%s, self.workdir:%s', self.canUseExistingExe, self.workdir)

    def __getattr__(self, item):
        # pass through getters to underlying args object
        return getattr(self._args, item)

    @property
    def configuration_dump(self):
        result = []
        result += ["debug: {}".format(self.debug)]
        result += ["quiet: {}".format(self.quiet)]
        result += ["verbose: {}".format(self.verbose)]
        result += ["ara_sysroot: {}".format(self.ara_sysroot)]
        result += ["target_machines: {}".format(self.machines)]
        result += ["workdir: {}".format(self.workdir)]
        result += ["files: {}".format(self.files)]
        return result

