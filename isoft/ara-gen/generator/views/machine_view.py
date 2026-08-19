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
MachineView corresponding to a Machine IM object.
"""
import logging

from generator.common.tree_helper import get_hump
from generator.intermediate_model.execution_management.machine import Machine
from generator.parser.exception_handling import handle_exceptions
from generator.views.someip.someip_service_view import SomeIpServiceView
from generator.views.view import View
from generator.views.view_factory import ViewFactory


class MachineView(View):
    """Specialized View of a Machine."""

    def __init__(self, machine: Machine) -> None:
        super().__init__(machine)
        self._machine = machine
        self._log = logging.getLogger(__name__)
        ps = []
        rs = []
        for si_mapping in machine.si_mappings:
            ps += [ViewFactory.create_service_view(m["service"]) for m in si_mapping["provided"]]
            rs += [ViewFactory.create_service_view(m["service"]) for m in si_mapping["required"]]
        self._required_services = list(set(rs))
        self._provided_services = list(set(ps))
        self.all_services = list(set(rs + ps))

        someip_services = [si for si in (rs + ps) if isinstance(si, SomeIpServiceView)]
        self._populate_sd_config(someip_services)
        self._nm_contribution = {}
        self.iam_phm = {}
        self.iam_crypto = {}
        self.iam_someip = None
        self.phm_contribution = None
        self._crypto_contribution = {}
        self.idsm = None
        self.nsomeipdProcessName = machine.nsomeipdProcessName
        self.ide_info = None
        self.networkConfig = None

    @handle_exceptions(error_string_template="ERROR: {exception_text}")
    def _populate_sd_config(self, services):
        sd_config_list = list([service.service_deployment.sd_config for service in services])
        self._sd_config = dict()
        for config in sd_config_list:
            for key, value in config.items():
                if value is None:
                    continue
                if key == "initial_repetitions_max":
                    value = value + 1
                if key == "initial_repetitions_base_delay":
                    value = int(value / 2)
                if key not in self._sd_config:
                    self._sd_config[key] = value
                elif not value == self._sd_config[key]:
                    if key == "initial_delay_min" or key == "initial_delay_max" or key == "initial_repetitions_base_delay" or key == "initial_repetitions_max":
                        assert False, (
                            "[CM  ] CODE-010: Inconsistent Configuration for SOME/IP Service Discovery Configuration Value '{0}'. First: {1}, Second: {2}".format(get_hump(key), self._sd_config[key], value)
                    )


    @property
    def get_machine(self):
        """Get the machine in the view"""
        return self._machine

    @property
    def sd_config(self):
        """Get the service discovery configuration for the machine"""
        return self._sd_config

    @property
    def process_views(self):
        return self._process_views

    @property
    def services_list(self):
        _services = self.all_services
        _services.sort(key=lambda field: field.fqn.lower())
        return _services

    @property
    def provided_services_list(self):
        return sorted(self._provided_services)

    @property
    def required_services_list(self):
        return sorted(self._required_services)

    @property
    def nm_contribution(self):
        return self._nm_contribution

    @property
    def crypto_contribution(self):
        return self._crypto_contribution

