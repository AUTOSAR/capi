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
import sys



class GeneratorSettings(object):
    """
    Convenience class for storing and accessing a set of settings for the generator.
    """

    def __init__(self, args):
        self._args = args
        self._log = logging.getLogger(__name__)

        if not self.output_dir:
            self._log.error("The generator must be given an output directory as an input")
            sys.exit("error: the following arguments are required: -o/--output, file\n")

    def __getattr__(self, item):
        # pass through getters to underlying args object
        return getattr(self._args, item)

    @property
    def user_defined_libs(self):
        return self._args.ipc_libs + self._args.icc_libs


