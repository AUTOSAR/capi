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
Component class.
"""

from ..utils.container import Container
from ..communication_management.service import Service
from ..persistency.kvs_interface import KvsInterface

from ..phm.supervisedentityinterface import SupervisedEntityInterface
from ..phm.healthchannelinterface import HealthChannelInterface
from ..phm.sernotificalinterface import SERNotificalInterface
from ..phm.hcrnotificalinterface import HCRNotificalInterface



class Component(object):
    """An Adaptive Component corresponds to one functional
    entity capable of communicating via ara::com"""
    def __init__(self, name, fqn=None):
        if name == "":
            raise AssertionError("A component must have a name")
        self._name = name
        self._required_services = Container()
        self._provided_services = Container()
        self._persistency_interfaces = Container()
        self._phm_supervisedentity_interfaces = Container()
        self._phm_healthchannel_interfaces = Container()
        self._phm_sernotifical_interfaces = Container()
        self._phm_hcrnotifical_interfaces = Container()
        self._diag_element = []
        self._diag_identifier = []
        self._diag_routine = []

        self.fqn = fqn
        self._intaerfaces = Container()

    def __str__(self):
        return "Component(%s)->Interfaces(%s)" % (self.name, self.services)

    def __repr__(self):
        return self.__str__()

    def add_required_service(self, service: Service):
        """Adds the service to the set of services in this component,
        the set is composed of all services required or provided
        by the component"""
        for required_service in self._required_services:
            if self.issameservice(service,required_service):
                return
        self._required_services.add(service)

    def add_provided_service(self, service: Service):
        """Adds the service to the set of services in this component,
        the set is composed of all services required or provided
        by the component"""
        for provided_services in self._provided_services:
            if self.issameservice(service, provided_services):
                return
        self._provided_services.add(service)

    def issameservice(self,service1: Service,service2: Service):
        includename1 = ""
        for name in service1.namespaces:
            includename1 = includename1+name
        includename1 = includename1+service1.name
        includename2 = ""
        for name in service1.namespaces:
            includename2 = includename2 + name
        includename2 = includename2 + service2.name
        if includename1 == includename2 and service1.bind_type == service2.bind_type:
            if service1.service_deployment is not None and service2.service_deployment is not None:
                if service1.service_deployment.name != service2.service_deployment.name:
                    return False
            return True
        else:
            return False

    def add_persistency_interface(self, interface: KvsInterface):
        self._persistency_interfaces.add(interface)

    def add_phm_supervisedentity_interface(self,interface:SupervisedEntityInterface):
        self._phm_supervisedentity_interfaces.add(interface)
    def add_phm_healthchannel_interface(self,interface:HealthChannelInterface):
        self._phm_healthchannel_interfaces.add(interface)
    def add_phm_sernotifical_interface(self,interface:SERNotificalInterface):
        self._phm_sernotifical_interfaces.add(interface)
    def add_phm_hcrnotifical_interface(self,interface:HCRNotificalInterface):
        self._phm_hcrnotifical_interfaces.add(interface)

    @property
    def name(self) -> str:
        """Get the name of the component"""
        return self._name

    @property
    def required_services(self) -> [Service]:
        """Get the set of services belonging to this Component"""
        return self._required_services

    @property
    def provided_services(self) -> [Service]:
        """Get the set of services belonging to this Component"""
        return self._provided_services

    @property
    def services(self) -> [Service]:
        return self._provided_services + self._required_services

    @property
    def persistency_interfaces(self) -> [KvsInterface]:
        return self._persistency_interfaces

    @property
    def phm_supervisedentity_interfaces(self) -> [SupervisedEntityInterface]:
        return self._phm_supervisedentity_interfaces

    @property
    def phm_healthchannel_interfaces(self) -> [HealthChannelInterface]:
        return self._phm_healthchannel_interfaces

    @property
    def phm_sernotifical_interfaces(self) -> [SERNotificalInterface]:
        return self._phm_sernotifical_interfaces

    @property
    def phm_hcrnotifical_interfaces(self) -> [HCRNotificalInterface]:
        return self._phm_hcrnotifical_interfaces

    @property
    def diag_element(self):
        return self._diag_element

    def add_diag_element(self, item):
        self._diag_element.append(item)

    @property
    def diag_identifier(self):
        return self._diag_identifier

    def add_diag_identifier(self, item):
        self._diag_identifier.append(item)

    @property
    def diag_routine(self):
        return self._diag_routine

    def add_diag_routine(self, item):
        self._diag_routine.append(item)

    @property
    def interfaces(self):
        return self._intaerfaces

    def add_interface(self,interface):
        lower_interface = interface.replace("-","").lower()
        if lower_interface not in self._intaerfaces:
            self._intaerfaces.add(lower_interface)
