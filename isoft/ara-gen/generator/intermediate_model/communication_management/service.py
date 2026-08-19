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
Service class.
"""

# Unused import check suppressed, because only used in type annotation, which is not found by PyLint
from typing import List, Dict   # pylint: disable=unused-import

from ..utils.container import Container
from .event import Event
from .method import Method
from .field import Field
from .signal import Signal
from .etosmapping import EventToSignalMapping
from .stoemapping import SignalToEventMapping
from .ara_com_deployment.service_deployment import ServiceDeployment
from generator.intermediate_model.communication_management.ara_com_deployment.\
    someip.someip_service_deployment import SomeIpServiceDeployment  # noqa
from generator.intermediate_model.communication_management.ara_com_deployment.\
    user_defined.user_defined_service_deployment import UserDefinedServiceDeployment  # noqa
from generator.intermediate_model.communication_management.ara_com_deployment.dds.\
    dds_service_deployment import DdsServiceDeployment  # noqa

class Service(object):
    """Describes a Service that can be provided by
    or requested from a component"""
    def __init__(self, fqn: str) -> None:
        if fqn == "":
            raise AttributeError("A Service must have a name")
        self.major_version = None
        self.minor_version = None
        self._fqn = fqn
        self.namespaces = list()  # type: List[str]
        self.lowNamespaces = []  # type: List[str]
        self._exposed_api_content = {}   # type: Dict
        self._exposed_api_content["events"] = Container()
        self._exposed_api_content["methods"] = Container()
        self._exposed_api_content["fields"] = Container()
        self._exposed_api_content["signals"] = Container()
        self._exposed_api_content["etosmappings"] = Container()
        self._exposed_api_content["stoemappings"] = Container()
        self._exposed_api_content["errors"] = set()
        self._exposed_api_content["ap_errors"] = set()
        self.service_deployment = None  # type: ServiceDeployment
        self._machine = None
        self.is_show = True
        self.cg_sign = None
        self.bindtypes = []
        self.someipdep_name = []
        self.ddsdep_name = []
        self.userdefdep_name = []
        self.iccdep_name = []
        self.service_deploymentinfos = []



    def __str__(self):
        return "Service(%s)->API(%s)" % (self.name, self._exposed_api_content)

    def __repr__(self):
        return self.__str__()


    @property
    def name(self) -> str:
        """Get the Service name"""
        return self._fqn.split("/")[-1]

    def add_event(self, event: Event):
        """Add an Event to this Service."""
        self._exposed_api_content["events"].add(event)

    def add_method(self, method: Method):
        """Add a Method to this Service."""
        self._exposed_api_content["methods"].add(method)
        self._exposed_api_content["ap_errors"].update(method.errors)

    def add_field(self, field: Field):
        """Add a Field to this Service."""
        self._exposed_api_content["fields"].add(field)
    def add_signal(self,signal:Signal):
        self._exposed_api_content["signals"].add(signal)
    def add_etosmapping(self,etosmapping):
        self._exposed_api_content["etosmappings"].add(etosmapping)
    def add_stoemapping(self, stoemapping):
        self._exposed_api_content["stoemappings"].add(stoemapping)
    def get_signalbyname(self,signalname):
        for signal in self._exposed_api_content["signals"]:
            if signal.name == signalname:
                return signal

    @property
    def events(self) -> Container:
        """Get the Events of this Service."""
        return self._exposed_api_content["events"]

    @property
    def methods(self) -> Container:
        """Get the Methods of this Service."""
        return self._exposed_api_content["methods"]

    @property
    def fields(self) -> Container:
        """Get the Fields of this Service."""
        return self._exposed_api_content["fields"]
    @property
    def signals(self) -> Container:
        """Get the signals of this Service."""
        return self._exposed_api_content["signals"]
    @property
    def etosmappings(self) -> Container:
        """Get the etosmappings of this Service."""
        return self._exposed_api_content["etosmappings"]
    @property
    def stoemappings(self) -> Container:
        """Get the stoemappings of this Service."""
        return self._exposed_api_content["stoemappings"]

    @property
    def errors(self):
        """Get the possible errors of this Service"""
        return self._exposed_api_content["errors"]

    @property
    def ap_errors(self):
        return self._exposed_api_content["ap_errors"]

    @property
    def error_domains(self):
        domains = set()
        for ap_error in self.ap_errors:
            domains.add(ap_error.domain)
        return domains

    @property
    def fqn(self):
        return self._fqn

    def add_error(self, error):
        """Add an error to the possible errors of this Service"""
        self._exposed_api_content["errors"].add(error)
    @property
    def machine(self):
        return self._machine
    def set_machine(self,machine):
        self._machine = machine
    def adjust_signaluserevent(self):
        for event in self.events:
            for signal in self.signals:
                if event.event_deployment.deployment_id == signal.eventid-32768:
                    for etosmapping in self.etosmappings:
                        if etosmapping.event == event.name:
                            if len(etosmapping.order_submapping)>0:
                                for itemtype in signal.typeitems:
                                    for ordermapings in etosmapping.order_submapping:
                                        for submaping in ordermapings["submappinglist"]:
                                            if itemtype.name == submaping.signaltypevalue:
                                                itemtype.set_eventnamevalue(submaping.eventtypevalue)
                                signal.set_userevent(True)
                                etosmapping.set_userevent(True)
                                for submapping in  etosmapping.order_submapping:
                                    submapping["userevent"] = True
                    for stoemapping in self.stoemappings:
                        for e in stoemapping.event:
                            if e == event.name:
                                if len(stoemapping.order_submapping) > 0:
                                    for itemtype in signal.typeitems:
                                        for ordermapings in stoemapping.order_submapping:
                                            for submaping in ordermapings["submappinglist"]:
                                                if itemtype.name == submaping.signaltypevalue:
                                                    itemtype.set_eventnamevalue(submaping.eventtypevalue)
                                    signal.set_userevent(True)
                                    stoemapping.set_userevent(True)
                                    for submapping in stoemapping.order_submapping:
                                        submapping["userevent"] = True

    @property
    def bind_type(self):
        if type(self.service_deployment) is DdsServiceDeployment:
           return "fastdds"

        elif type(self.service_deployment) is SomeIpServiceDeployment:
            return "nsomeip"

        elif type(self.service_deployment) is UserDefinedServiceDeployment:
            return self.service_deployment.depType

        return ""
    def set_bind_type_new(self,type):
        self.bindtypes.append(type)

    @property
    def bind_types(self):
        return self.bindtypes

    def add_service_deploymentinfo(self,type,name):
        if type == "nsomeip" and name not in self.someipdep_name:
            self.someipdep_name.append(name)
        elif type == "fastdds" and name not in self.ddsdep_name:
            self.ddsdep_name.append(name)
        elif type == "npc" and name not in self.userdefdep_name:
            self.userdefdep_name.append(name)
        elif type == "icc" and name not in self.iccdep_name:
            self.iccdep_name.append(name)









