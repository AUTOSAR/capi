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

from generator.common.tree_helper import get_element_or_die, get_element_or_none, int_or_unmodified
from generator.intermediate_model.communication_management.ara_com_deployment.dds.event_deployment import EventDeployment
from generator.intermediate_model.communication_management.ara_com_deployment.dds.method_deployment import MethodDeployment
from generator.intermediate_model.communication_management.ara_com_deployment.dds.field_deployment import FieldDeployment
from generator.common.tree_helper import short_name, to_list,get_element_or_str,transition_number


class DdsServiceDeploymentBuilder:

    def __init__(self, arxml_root):
        self.arxml_root = arxml_root
        self.serviceInterfaceId = {}

    def _get_protocols(self, ar_depl):
        result = []
        protocolList = get_element_or_none(ar_depl, "TRANSPORT-PROTOCOLS/TRANSPORT-PROTOCOL")
        if protocolList is not None:
            for protocol in protocolList:
                result.append(str(protocol))
        return result

    def _get_event_deployment(self, ar_event_depl):
        return {
            "name": ar_event_depl.get_fqn(),
            "topic": get_element_or_die(ar_event_depl, "TOPIC-NAME"),
            "protocols": self._get_protocols(ar_event_depl)
        }

    def _get_field_deployment(self, ar_deployment, ar_field_depl):
        result = {"getter": {}, "setter": {}, "notifier": {}}
        req_topic = get_element_or_die(ar_deployment, "FIELD-REQUEST-TOPIC-NAME").text
        rep_topic = get_element_or_die(ar_deployment, "FIELD-REPLY-TOPIC-NAME").text
        protocols = self._get_protocols(ar_deployment)
        result['getter'] = {
            "name": "artifical_get",
            "protocols": protocols,
            "request_topic": req_topic,
            "reply_topic": rep_topic
        }
        result['setter'] = {
            "name": "artifical_set",
            "protocols": protocols,
            "request_topic": req_topic,
            "reply_topic": rep_topic
        }

        ar_notifier = get_element_or_none(ar_field_depl, "NOTIFIER")
        if ar_notifier is not None:
            result['notifier'] = self._get_event_deployment(ar_notifier)

        return result

    def _get_method_deployment(self, ar_si):
        return {
            "name": "artifical_method",
            "protocols": self._get_protocols(ar_si),
            "request_topic": get_element_or_die(ar_si, "METHOD-REQUEST-TOPIC-NAME").text,
            "reply_topic": get_element_or_die(ar_si, "METHOD-REPLY-TOPIC-NAME").text
        }

    def get_instance_deployment(self, ar_deployment):
        serviceId = str(get_element_or_die(ar_deployment, "SERVICE-INTERFACE-ID"))
        if not serviceId.isalnum():
            assert False, "[CM  ] CODE-006: DdsServiceInterfaceDeployment.serviceInterfaceId[{}] is not accepted by the pattern '[A-Za-z0-9]+'.Fqn:{}. LocalPath:{}. ".format(serviceId, ar_deployment.get_fqn(), ar_deployment.get_path())
        if not serviceId[0].isalpha():
            assert False, "[CM  ] CODE-006: DdsServiceInterfaceDeployment.serviceInterfaceId[{}] must start with a letter. Fqn:{}. LocalPath:{}. ".format(serviceId, ar_deployment.get_fqn(), ar_deployment.get_path())
        if serviceId in self.serviceInterfaceId:
            assert self.serviceInterfaceId[serviceId] == ar_deployment.get_fqn(), "[CM  ] CODE-006: DdsServiceInterfaceDeployment.serviceInterfaceId[{}] must be unique. Currently exists in [{}, {}]. LocalPath:{}. ".format(serviceId, ar_deployment.get_fqn(), self.serviceInterfaceId[serviceId], ar_deployment.get_path())
        else:
            self.serviceInterfaceId[serviceId] = ar_deployment.get_fqn()
        atKey = True
        sd = get_element_or_none(ar_deployment, "ADMIN-DATA/SDGS/SDG/SD")
        if sd is not None and sd.attrib["GID"] == "iSOFT:com:DdsServiceInterfaceDeploymentSdgProps:EnableEventIdlWithKey":
            atKey = sd

        result = {
            "service_id": serviceId,
            "atKey": atKey,
            "events": {}, "methods": {}, "fields": {}
        }
        interface_ref = get_element_or_none(ar_deployment, "SERVICE-INTERFACE-REF")
        ar_interface = self.arxml_root.find_referable(interface_ref.text)
        interfaceOb = {
            "events": [],
            "fields": []
        }
        events = get_element_or_none(ar_interface, "EVENTS/VARIABLE-DATA-PROTOTYPE")
        if events is not None:
            for item in events:
                interfaceOb["events"].append(item.get_fqn())
        fields = get_element_or_none(ar_interface, "FIELDS/FIELD")
        if fields is not None:
            for item in fields:
                interfaceOb["fields"].append(item.get_fqn())

        ar_dds_event_deployments = get_element_or_none(ar_deployment, "EVENT-DEPLOYMENTS/DDS-EVENT-DEPLOYMENT")
        ar_dds_event_deployments = to_list(ar_dds_event_deployments)
        for ar_ed in ar_dds_event_deployments:
            key = get_element_or_die(ar_ed, "EVENT-REF").text
            if key in interfaceOb["events"]:
                interfaceOb["events"].remove(key)
            result["events"][key] = self._get_event_deployment(ar_ed)

        ar_methods = get_element_or_none(ar_interface, "METHODS/CLIENT-SERVER-OPERATION")
        ar_methods = to_list(ar_methods)

        for ar_m in ar_methods:
            key = ar_m.get_fqn()
            result["methods"][key] = self._get_method_deployment(ar_deployment)

        ar_dds_field_deployments = get_element_or_none(ar_deployment, "FIELD-DEPLOYMENTS/DDS-FIELD-DEPLOYMENT")
        ar_dds_field_deployments = to_list(ar_dds_field_deployments)
        for ar_fd in ar_dds_field_deployments:
            key = get_element_or_die(ar_fd, "FIELD-REF").text
            if key in interfaceOb["fields"]:
                interfaceOb["fields"].remove(key)
            filedEle = self._get_field_deployment(ar_deployment, ar_fd)
            self.interface_field_deployment(ar_interface, key, filedEle, ar_deployment)
            result["fields"][key] = filedEle

        assert len(interfaceOb["events"]) + len(interfaceOb["fields"]) == 0, \
            "[CM  ] CODE-005: Failed (The interface [{}] should be deployed properly in [{}]).LocalPath:{}:{}.".format(ar_interface.get_fqn(), ar_deployment.get_fqn(), ar_interface.get_path(), ar_deployment.get_path())

        return result

    def interface_field_deployment(self, interface, filedFqn, filedEle, ar_deployment):
        interfaceFile = self.arxml_root.find_referable(filedFqn)
        configuration = True
        if interfaceFile.HAS_NOTIFIER and "name" not in filedEle["notifier"]:
            configuration = False
        assert configuration, \
            "[CM  ] CODE-005: Failed (The interface [{}] should be deployed properly in [{}]).LocalPath:{}:{}.".format(interface.get_fqn(), ar_deployment.get_fqn(), interface.get_path(), ar_deployment.get_path())

    def get_provided_si_deployment(self, provided_si):
        qos_profile = get_element_or_die(provided_si, "QOS-PROFILE")
        instance_id = transition_number(get_element_or_die(provided_si, "SERVICE-INSTANCE-ID"))
        domain_id = get_element_or_die(provided_si, "DOMAIN-ID")
        if int(domain_id)<0 or int(domain_id)>232:
            assert False, "[CM  ] CODE-007: The value of DdsProvidedServiceInstance.domainId[{}] must between '[0-232]'. Fqn:{}. LocalPath:{}. ".format(domain_id.text, provided_si.get_fqn(), provided_si.get_path())

        ar_transport_plugins = get_element_or_none(provided_si, "TRANSPORT-PLUGINS")
        transport_plugins = []
        if ar_transport_plugins is not None:
            transport_plugins = [str(plugin) \
                                 for plugin in \
                                 ar_transport_plugins.find_elements_of_type('TRANSPORT-PLUGIN')]
        ar_events_qos = get_element_or_none(provided_si, "EVENT-QOS-PROPSS")
        events_qos = {}
        if ar_events_qos is not None:
            events_qos = {
                property.EVENT_REF : get_element_or_str(property, "QOS-PROFILE")
                for property in ar_events_qos.find_elements_of_type('DDS-EVENT-QOS-PROPS')
            }
        ar_methods_qos = get_element_or_none(provided_si, "METHOD-QOS-PROPSS")
        methods_qos = {}
        if ar_methods_qos is not None:
            methods_qos = {
                property.METHOD_REF : get_element_or_str(property, "QOS-PROFILE")
                for property in ar_methods_qos.find_elements_of_type('DDS-METHOD-QOS-PROPS')
            }
        ar_fields_get_set_qos = get_element_or_none(provided_si, "FIELD-GET-SET-QOS-PROPSS")
        fields_get_set_qos = {}
        if ar_fields_get_set_qos is not None:
            fields_get_set_qos = {
                property.FIELD_REF : get_element_or_str(property, "QOS-PROFILE")
                for property in ar_fields_get_set_qos.find_elements_of_type('DDS-FIELD-QOS-PROPS')
            }
        ar_fields_notifier_qos = get_element_or_none(provided_si, "FIELD-NOTIFIER-QOS-PROPSS")
        fields_notifier_qos = {}
        if ar_fields_notifier_qos is not None:
            fields_notifier_qos = {
                property.FIELD_REF : get_element_or_str(property, "QOS-PROFILE")
                for property in ar_fields_notifier_qos.find_elements_of_type('DDS-FIELD-QOS-PROPS')
            }

        returnOb = {
            "instance_id" : int_or_unmodified(instance_id),
            "domain_id" : domain_id,
            "transport_plugins" : transport_plugins,
            "qos_profile" : qos_profile,
            "events_qos" : events_qos,
            "methods_qos" : methods_qos,
            "fields_getset_qos" : fields_get_set_qos,
            "fields_notifier_qos" : fields_notifier_qos
        }
        returnOb["methods_qos"] = {
            "artifical_method": returnOb["qos_profile"]
        }
        returnOb["fields_getset_qos"] = {
            "artifical_get": returnOb["qos_profile"],
            "artifical_set": returnOb["qos_profile"]
        }
        return returnOb

    def get_required_si_deployment(self, required_si):
        qos_profile = get_element_or_die(required_si, "QOS-PROFILE")
        instance_id = transition_number(get_element_or_die(required_si, "REQUIRED-SERVICE-INSTANCE-ID"))
        domain_id = get_element_or_die(required_si, "DOMAIN-ID")
        if int(domain_id)<0 or int(domain_id)>232:
            assert False, "[CM  ] CODE-007: The value of DdsRequiredServiceInstance.domainId[{}] must between '[0-232]'. Fqn:{}. LocalPath:{}.".format(domain_id.text, required_si.get_fqn(), required_si.get_path())

        ar_transport_plugins = get_element_or_none(required_si, "TRANSPORT-PLUGINS")
        transport_plugins = []
        if ar_transport_plugins is not None:
            transport_plugins = [str(plugin) \
                                 for plugin in \
                                 ar_transport_plugins.find_elements_of_type('TRANSPORT-PLUGIN')]
        ar_events_qos = get_element_or_none(required_si, "EVENT-QOS-PROPSS")
        events_qos = {}
        if ar_events_qos is not None:
            events_qos = {
                property.EVENT_REF : get_element_or_str(property, "QOS-PROFILE")
                for property in ar_events_qos.find_elements_of_type('DDS-EVENT-QOS-PROPS')
            }
        ar_methods_qos = get_element_or_none(required_si, "METHOD-QOS-PROPSS")
        methods_qos = {}
        if ar_methods_qos is not None:
            methods_qos = {
                property.METHOD_REF : get_element_or_str(property, "QOS-PROFILE")
                for property in ar_methods_qos.find_elements_of_type('DDS-METHOD-QOS-PROPS')
            }
        ar_fields_get_set_qos = get_element_or_none(required_si, "FIELD-GET-SET-QOS-PROPSS")
        fields_get_set_qos = {}
        if ar_fields_get_set_qos is not None:
            fields_get_set_qos = {
                property.FIELD_REF : get_element_or_str(property, "QOS-PROFILE")
                for property in ar_fields_get_set_qos.find_elements_of_type('DDS-FIELD-QOS-PROPS')
            }
        ar_fields_notifier_qos = get_element_or_none(required_si, "FIELD-NOTIFIER-QOS-PROPSS")
        fields_notifier_qos = {}
        if ar_fields_notifier_qos is not None:
            fields_notifier_qos = {
                property.FIELD_REF : get_element_or_str(property, "QOS-PROFILE")
                for property in ar_fields_notifier_qos.find_elements_of_type('DDS-FIELD-QOS-PROPS')
            }
        blacklisted_versions = self._get_blacklisted_versions(required_si)

        returnOb = {
            "instance_id": int_or_unmodified(instance_id),
            "domain_id": domain_id,
            "transport_plugins": transport_plugins,
            "qos_profile": qos_profile,
            "events_qos": events_qos,
            "methods_qos": methods_qos,
            "fields_getset_qos": fields_get_set_qos,
            "fields_notifier_qos": fields_notifier_qos,
            "blacklisted_versions": blacklisted_versions
        }
        returnOb["methods_qos"] = {
            "artifical_method": returnOb["qos_profile"]
        }
        returnOb["fields_getset_qos"] = {
            "artifical_get": returnOb["qos_profile"],
            "artifical_set": returnOb["qos_profile"]
        }
        return returnOb

    def _get_blacklisted_versions(self, required_si):
        ar_blacklisted_versions = get_element_or_none(required_si, "BLACKLISTED-VERSIONS/DDS-SERVICE-VERSION")
        blacklisted_versions = []
        if ar_blacklisted_versions is not None:
            for ar_bv in ar_blacklisted_versions:
                major = get_element_or_die(ar_bv, "MAJOR-VERSION") # multiplicity: 1
                major = transition_number(major,defaultInt=0)
                minor = get_element_or_die(ar_bv, "MINOR-VERSION") # multiplicity: 1
                minor = transition_number(minor, defaultInt=0)
                version = {"major": major, "minor": minor}
                blacklisted_versions.append(version)
        return blacklisted_versions

    def _create_event_deployment(self, depl, events_qos, qos_profile):
        if not depl: return None
        ret_val = EventDeployment()
        ret_val.topic = str(
            depl['topic'])
        for proto in depl["protocols"]:
            ret_val.add_transport_protocol(proto)
        if depl['name'] in events_qos:
            ret_val.qos_profile = events_qos[depl['name']]
        if ret_val.qos_profile == "":
            ret_val.qos_profile = qos_profile
        return ret_val


    def _create_method_deployment(self, depl, methods_qos, qos_profile):
        if not depl: return None
        ret_val = MethodDeployment(depl["request_topic"], depl["reply_topic"])
        for proto in depl["protocols"]:
            ret_val.add_transport_protocol(proto)
        if depl['name'] in methods_qos:
            ret_val.qos_profile = methods_qos[depl['name']]
        if ret_val.qos_profile == "":
            ret_val.qos_profile = qos_profile
        return ret_val

    def _populate_method_deployment(self, methods, method_deployment, qos_profile, methods_qos):
        for method_fqn, method_depl in method_deployment.items():
            method_name = short_name(method_fqn)
            for method in methods:
                if method.name == method_name:
                    method.deployment = self._create_method_deployment(method_depl, methods_qos, qos_profile)

    def _populate_event_deployment(self, events, event_deployment, qos_profile, events_qos):
        for event_fqn, event_depl in event_deployment.items():
            event_name = short_name(event_fqn)
            for event in events:
                if event.name == event_name:
                    event.event_deployment = self._create_event_deployment(event_depl, events_qos, qos_profile)

    def _populate_field_deployment(self, fields, field_deployment, qos_profile, fields_getset_qos, fields_notifier_qos):
        for field_fqn, field_depl in field_deployment.items():
            field_name = short_name(field_fqn)
            for field in fields:
                if field.name == field_name:
                    field_deployment = FieldDeployment()
                    field_deployment.getter = \
                        self._create_method_deployment(field_depl['getter'], fields_getset_qos, qos_profile)
                    field_deployment.setter = \
                        self._create_method_deployment(field_depl['setter'], fields_getset_qos, qos_profile)
                    field_deployment.notifier = \
                        self._create_event_deployment(field_depl['notifier'], fields_notifier_qos, qos_profile)
                    field.deployment = field_deployment

    def populate_deployment(self, service, deployment):
        if deployment.required:
            self._populate_event_deployment(
                service.events, deployment.required['events'], deployment.required['qos_profile'] if "qos_profile" in deployment.required else "", deployment.required['events_qos'] if "events_qos" in deployment.required else {})
            self._populate_method_deployment(
                service.methods, deployment.required['methods'], deployment.required['qos_profile'] if "qos_profile" in deployment.required else "", deployment.required['methods_qos'] if "methods_qos" in deployment.required else {})
            self._populate_field_deployment(
                service.fields, deployment.required['fields'], deployment.required['qos_profile'] if "qos_profile" in deployment.required else "",
                deployment.required['fields_getset_qos'] if "fields_getset_qos" in deployment.required else {}, deployment.required['fields_notifier_qos'] if "fields_notifier_qos" in deployment.required else {})
        elif deployment.provided:
            self._populate_event_deployment(
                service.events, deployment.provided['events'], deployment.provided['qos_profile'] if "qos_profile" in deployment.provided else "", deployment.provided['events_qos'] if "events_qos" in deployment.provided else {})
            self._populate_method_deployment(
                service.methods, deployment.provided['methods'], deployment.provided['qos_profile'] if "qos_profile" in deployment.provided else "", deployment.provided['methods_qos'] if "methods_qos" in deployment.provided else {})
            self._populate_field_deployment(
                service.fields, deployment.provided['fields'], deployment.provided['qos_profile'] if "qos_profile" in deployment.provided else "",
                deployment.provided['fields_getset_qos'] if "fields_getset_qos" in deployment.provided else {}, deployment.provided['fields_notifier_qos'] if "fields_notifier_qos" in deployment.provided else {})
