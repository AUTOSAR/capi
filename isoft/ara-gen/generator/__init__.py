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
Application setup
"""

import logging

# Add custom TRACE level for logging
TRACE_LEVEL_NAME = 'TRACE'
TRACE_METHOD_NAME = 'trace'
# Unfortunately, we cannot set this to DEBUG-10, as that is already used for NOTSET.
TRACE_LEVEL_NUM = logging.DEBUG - 5


def log_for_level(self, message, *args, **kwargs):
    """ Log a message with level TRACE on this Logger. """
    if self.isEnabledFor(TRACE_LEVEL_NUM):
        # pylint: disable=protected-access
        self._log(TRACE_LEVEL_NUM, message, args, **kwargs)


def log_to_root(message, *args, **kwargs):
    """ Log a message with level TRACE on the root Logger. """
    logging.log(TRACE_LEVEL_NUM, message, *args, **kwargs)


logging.addLevelName(TRACE_LEVEL_NUM, TRACE_LEVEL_NAME)
setattr(logging, TRACE_LEVEL_NAME, TRACE_LEVEL_NUM)
setattr(logging.getLoggerClass(), TRACE_METHOD_NAME, log_for_level)
setattr(logging, TRACE_METHOD_NAME, log_to_root)
