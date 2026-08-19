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
Machine class.
"""


class Machine(object):
    """Machine"""
    def __init__(self, fqn, configuration):
        self.ip_address = "127.0.0.1"
        self.network_mask = "255.255.255.255"
        self.gateway = "127.0.0.0"
        self.sd_address = ""
        self.sd_port = ""
        self._fqn = fqn
        self._configuration = configuration
        self.maxtransmissunit = 0
        self.usd_sd_option = -1
        self.si_mappings = []
        self.nsomeipdProcessName = ""
        self.multiple_host = []

    def __repr__(self):
        return self.__str__()

    @property
    def name(self):
        return self._fqn.split("/")[-1]

    @property
    def fqn(self):
        return self._fqn

    @property
    def configuration(self):
        return self._configuration
