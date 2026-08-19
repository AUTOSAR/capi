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
Process class.
"""


class Process(object):
    def __init__(self, fqn, function, executable, em_info, deterministic_client, si_mappings, persistency_mappings,time_base_mappings,machine, recovery_to_p_port_mappings,recovery_notification_infos, swcl_info, crypto, e2e, logTrace, signalbindinfo, rawmappings, cg):
        self.fqn = fqn
        self.function = function
        self.executable = executable
        self.em_info = em_info
        self.deterministic_client = deterministic_client
        self.si_mappings = si_mappings
        self.persistency_mappings = persistency_mappings
        self.time_base_mappings = time_base_mappings
        self.machine = machine
        self.recovery_to_p_port_mappings = recovery_to_p_port_mappings
        self.recovery_notification_infos = recovery_notification_infos
        self.swcl_info = swcl_info
        self.crypto = crypto
        self.e2e = e2e
        self.logTrace = logTrace
        self.signalbindinfo = signalbindinfo
        self.rawmappings = rawmappings
        self.cg = cg
        self.state_manager = None
        self.ipcName = None
        self.ipcConfiguration = {}
        self.clientId = None
        self.networkConfig = None

    def __str__(self):
        return "Process(%s)->Executable(%s)" % (self.name, self.executable)

    def __repr__(self):
        return self.__str__()

    @property
    def name(self):
        return self.fqn.split("/")[-1]
