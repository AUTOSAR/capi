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
ProcessView corresponding to a Process IM object.
"""
import hashlib

from generator.common.tree_helper import transition_number
from generator.intermediate_model.execution_management.process \
    import Process
from generator.views.view import View
from generator.views.view_factory import ViewFactory


class ProcessView(View):
    """Specialized View of a Process."""

    def __init__(self, process: Process) -> None:
        self._process = process
        self.clientId = self._process.clientId
        name = self._process.name
        self._id = ProcessView.get_process_id(name)
        super().__init__(process)

    @property
    def id(self):
        """Get the machine in the view"""
        if self.clientId is None:
            return hex(self._id)
        else:
            return self.clientId

    @property
    def npc_id(self):
        if "appId" in self._process.ipcConfiguration:
            return transition_number(self._process.ipcConfiguration["appId"])
        return transition_number(self.id)

    def get_si_mappings(self):
        provided = [{'si_fqn' : mapping['si_fqn'],
                     'service' : ViewFactory.create_service_view(mapping['service'])}
                    for mapping in self.model_object.si_mappings['provided']]
        required = [{'si_fqn' : mapping['si_fqn'],
                     'service' : ViewFactory.create_service_view(mapping['service'])}
                    for mapping in self.model_object.si_mappings['required']]
        return {'provided' : provided, 'required' : required}

    @property
    def persistency_mappings(self):
        """Fill port_type from port_type_raw: this is made for simplicity and is valid, because port type mapping is never changed"""
        mappings = self.model_object.persistency_mappings
        port_type_lookup = {
            "P-PORT-PROTOTYPE": "provided-port",
            "R-PORT-PROTOTYPE": "required-port",
            "PR-PORT-PROTOTYPE": "pr-port"
        }
        for m in mappings["kvs"] + mappings["file"]:
            m["port_type"] = port_type_lookup[m["port_type_raw"]]

        return mappings

    @staticmethod
    def get_process_id(name):
        hash_algo = hashlib.new('sha1')
        hash_algo.update(name.encode('UTF-8'))
        digest = hash_algo.hexdigest()
        # here 2 bytes are used therefore 4 digits are consumed
        return int(digest[0:4], base=16)
