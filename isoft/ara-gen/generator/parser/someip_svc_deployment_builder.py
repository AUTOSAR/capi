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
SOMEIP service deployment builder class.
"""

# pylint: disable=too-many-lines,too-many-public-methods
import collections
from generator.intermediate_model.communication_management.ara_com_deployment.someip.event_deployment \
    import EventDeployment
from generator.intermediate_model.communication_management.ara_com_deployment.someip.method_deployment \
    import MethodDeployment
from generator.intermediate_model.communication_management.ara_com_deployment.someip.field_deployment \
    import FieldDeployment
from generator.intermediate_model.communication_management.ara_com_deployment.someip.event_group_deployment \
    import EventGroupDeployment
from generator.common.tree_helper import short_name, get_element_or_die, get_element_or_none, to_milliseconds, to_list, positive_int, int_or_unmodified, transition_number, get_element_or_0, get_hump
from generator.parser import commen_util


class SomeIpServiceDeploymentBuilder:
    """
    Handles parsing the ARXML part, related to someip network binding.
    """

    def __init__(self, model):
        self.model = model
        self.uniqueCheckOb = {}

    # @uptrace{constr_3408}
    @staticmethod
    def _valid_someip_event_id(ar_event_depl, processed_events):
        event_id = positive_int(transition_number(get_element_or_die(ar_event_depl, "EVENT-ID")))
        assert event_id < 32768, "[CM  ] CODE-001: Events in some/ip must have event id < 0x8000 (32768), event[{}] has id {}. LocalPath:{}. ".format(ar_event_depl.get_fqn(), event_id, ar_event_depl.get_path())
        assert event_id not in processed_events, "[CM  ] CODE-001: Duplicate ID {} for event[{}]. LocalPath:{}. ".format(event_id, ar_event_depl.get_fqn(), ar_event_depl.get_path())
        processed_events.append(event_id)
        return event_id

    # @uptrace{constr_3409}
    @staticmethod
    def _valid_someip_method_id(ar_method_depl, processed_methods):
        method_id = positive_int(transition_number(get_element_or_die(ar_method_depl, "METHOD-ID")))
        assert method_id < 32768, "[CM  ] CODE-001: Methods in some/ip must have event id < 0x8000 (32768), method[{}] has id {}. LocalPath:{}. ".format(ar_method_depl.get_fqn(), method_id, ar_method_depl.get_path())
        assert method_id not in processed_methods, "[CM  ] CODE-001: Duplicate ID {} for method[{}]. LocalPath:{}. ".format(method_id, ar_method_depl.get_fqn(), ar_method_depl.get_path())
        processed_methods.append(method_id)
        return method_id

    @staticmethod
    def _valid_someip_transport(ar_depl):
        proto = get_element_or_die(ar_depl, "TRANSPORT-PROTOCOL").text
        assert proto in ["UDP", "TCP"], "[CM  ] CODE-002: Allowed protocols are UDP and TCP, deployment[{}] has protocol {}. LocalPath:{}.".format(ar_depl.get_fqn(), proto, ar_depl.get_path())
        return proto

    @staticmethod
    def _get_event_serializer(ar_depl):
        serializer = get_element_or_none(ar_depl, "SERIALIZER")
        if serializer:
            return serializer.text
        return ""

    @staticmethod
    def _get_event_deployment(ar_event_depl, processed_events, ar_si):
        """
        Extract someip event deployment information, will be returned as
        a dict with information
        """
        result = {
            "name": ar_event_depl.get_fqn(),
            "id": SomeIpServiceDeploymentBuilder._valid_someip_event_id(ar_event_depl, processed_events),
            "proto": SomeIpServiceDeploymentBuilder._valid_someip_transport(ar_event_depl),
            "serializer": SomeIpServiceDeploymentBuilder._get_event_serializer(ar_event_depl),
            "separation_time": to_milliseconds(get_element_or_none(ar_event_depl, "SEPARATION-TIME")),
            "maximum_segment_length": get_element_or_none(ar_event_depl, "MAXIMUM-SEGMENT-LENGTH")
        }
        serializer = get_element_or_none(ar_event_depl, "SERIALIZER")
        last = get_hump(serializer)
        if last != "":
            result["serializerDump"] = last
        if ar_si is not None:
            event_props = ar_si.find_elements_of_type("SOMEIP-EVENT-PROPS")
            for event_prop in event_props:
                eventRef = get_element_or_none(event_prop, "EVENT-REF")
                if eventRef is not None and eventRef.text == ar_event_depl.get_fqn():
                    collection_props = get_element_or_none(event_prop, "COLLECTION-PROPS")
                    if collection_props is not None:
                        result["collection_props"] = {
                            "buffer_timeout": to_milliseconds(get_element_or_none(collection_props, "UDP-COLLECTION-BUFFER-TIMEOUT")),
                            "trigger": get_element_or_none(collection_props, "UDP-COLLECTION-TRIGGER")
                        }
                        break
        return result

    @staticmethod
    def _get_field_deployment(ar_field_depl, processed_events, processed_methods, ar_si):
        """
        Extract someip field deployment information, will be returned as
        a dict with information
        """
        result = {"getter": {}, "setter": {}, "notifier": {}}
        ar_getter = get_element_or_none(ar_field_depl, "GET")
        if ar_getter is not None:
            result["getter"] = {
                "name": get_element_or_die(ar_getter, "SHORT-NAME"),
                "id": SomeIpServiceDeploymentBuilder._valid_someip_method_id(ar_getter, processed_methods),
                "proto": SomeIpServiceDeploymentBuilder._valid_someip_transport(ar_getter),
                "length_request": get_element_or_none(ar_getter, "MAXIMUM-SEGMENT-LENGTH-REQUEST"),
                "length_response": get_element_or_none(ar_getter, "MAXIMUM-SEGMENT-LENGTH-RESPONSE"),
                "time_request": to_milliseconds(get_element_or_none(ar_getter, "SEPARATION-TIME-REQUEST")),
                "time_response": to_milliseconds(get_element_or_none(ar_getter, "SEPARATION-TIME-RESPONSE"))
            }
            if ar_si is not None:
                method_props = ar_si.find_elements_of_type("SOMEIP-METHOD-PROPS")
                for method_prop in method_props:
                    methodRef = get_element_or_none(method_prop, "METHOD-REF")
                    if methodRef is not None and methodRef.text == ar_getter.get_fqn():
                        collection_props = get_element_or_none(method_prop, "COLLECTION-PROPS")
                        if collection_props is not None:
                            result["getter"]["collection_props"] = {
                                "buffer_timeout": to_milliseconds(get_element_or_none(collection_props, "UDP-COLLECTION-BUFFER-TIMEOUT")),
                                "trigger": get_element_or_none(collection_props, "UDP-COLLECTION-TRIGGER")
                            }
                            break

        ar_setter = get_element_or_none(ar_field_depl, "SET")
        if ar_setter is not None:
            result["setter"] = {
                "name": get_element_or_die(ar_setter, "SHORT-NAME"),
                "id": SomeIpServiceDeploymentBuilder._valid_someip_method_id(ar_setter, processed_methods),
                "proto": SomeIpServiceDeploymentBuilder._valid_someip_transport(ar_setter),
                "length_request": get_element_or_none(ar_setter, "MAXIMUM-SEGMENT-LENGTH-REQUEST"),
                "length_response": get_element_or_none(ar_setter, "MAXIMUM-SEGMENT-LENGTH-RESPONSE"),
                "time_request": to_milliseconds(get_element_or_none(ar_setter, "SEPARATION-TIME-REQUEST")),
                "time_response": to_milliseconds(get_element_or_none(ar_setter, "SEPARATION-TIME-RESPONSE"))
            }
            if ar_si is not None:
                method_props = ar_si.find_elements_of_type("SOMEIP-METHOD-PROPS")
                for method_prop in method_props:
                    methodRef = get_element_or_none(method_prop, "METHOD-REF")
                    if methodRef is not None and methodRef.text == ar_setter.get_fqn():
                        collection_props = get_element_or_none(method_prop, "COLLECTION-PROPS")
                        if collection_props is not None:
                            result["setter"]["collection_props"] = {
                                "buffer_timeout": to_milliseconds(get_element_or_none(collection_props, "UDP-COLLECTION-BUFFER-TIMEOUT")),
                                "trigger": get_element_or_none(collection_props, "UDP-COLLECTION-TRIGGER")
                            }
                            break

        ar_notifier = get_element_or_none(ar_field_depl, "NOTIFIER")
        if ar_notifier is not None:
            result["notifier"] = {
                "name": ar_notifier.get_fqn(),
                "short_name": ar_notifier.SHORT_NAME,
                "id": SomeIpServiceDeploymentBuilder._valid_someip_event_id(ar_notifier, processed_events),
                "proto": SomeIpServiceDeploymentBuilder._valid_someip_transport(ar_notifier),
                "serializer": SomeIpServiceDeploymentBuilder._get_event_serializer(ar_notifier),
                "separation_time": to_milliseconds(get_element_or_none(ar_notifier, "SEPARATION-TIME")),
                "maximum_segment_length": get_element_or_none(ar_notifier, "MAXIMUM-SEGMENT-LENGTH")
            }
            serializer = get_element_or_none(ar_notifier, "SERIALIZER")
            last = get_hump(serializer)
            if last != "":
                result["notifier"]["serializerDump"] = last
            if ar_si is not None:
                event_props = ar_si.find_elements_of_type("SOMEIP-EVENT-PROPS")
                for event_prop in event_props:
                    eventRef = get_element_or_none(event_prop, "EVENT-REF")
                    if eventRef is not None and eventRef.text == ar_notifier.get_fqn():
                        collection_props = get_element_or_none(event_prop, "COLLECTION-PROPS")
                        if collection_props is not None:
                            result["notifier"]["collection_props"] = {
                                "buffer_timeout": to_milliseconds(get_element_or_none(collection_props, "UDP-COLLECTION-BUFFER-TIMEOUT")),
                                "trigger": get_element_or_none(collection_props, "UDP-COLLECTION-TRIGGER")
                            }
                            break
        return result

    @staticmethod
    def _get_method_deployment(ar_method_depl, processed_methods, ar_si):
        """
        Extract someip method deployment information, will be returned as
        a dict with information
        """
        result = {
            "name": get_element_or_die(ar_method_depl, "SHORT-NAME"),
            "id": SomeIpServiceDeploymentBuilder._valid_someip_method_id(ar_method_depl, processed_methods),
            "proto": SomeIpServiceDeploymentBuilder._valid_someip_transport(ar_method_depl),
            "length_request": get_element_or_none(ar_method_depl, "MAXIMUM-SEGMENT-LENGTH-REQUEST"),
            "length_response": get_element_or_none(ar_method_depl, "MAXIMUM-SEGMENT-LENGTH-RESPONSE"),
            "time_request": to_milliseconds(get_element_or_none(ar_method_depl, "SEPARATION-TIME-REQUEST")),
            "time_response": to_milliseconds(get_element_or_none(ar_method_depl, "SEPARATION-TIME-RESPONSE"))
        }
        if ar_si is not None:
            method_props = ar_si.find_elements_of_type("SOMEIP-METHOD-PROPS")
            for method_prop in method_props:
                methodRef = get_element_or_none(method_prop, "METHOD-REF")
                if methodRef is not None and methodRef.text == ar_method_depl.get_fqn():
                    collection_props = get_element_or_none(method_prop, "COLLECTION-PROPS")
                    if collection_props is not None:
                        result["collection_props"] = {
                            "buffer_timeout": to_milliseconds(get_element_or_none(collection_props, "UDP-COLLECTION-BUFFER-TIMEOUT")),
                            "trigger": get_element_or_none(collection_props, "UDP-COLLECTION-TRIGGER")
                        }
                        break
        return result

    def _get_eventgrps_deployment(self, ar_event_groups, events_deployment, fields_deployment, ar_si, ports_config, isPR):
        """
        Extract someip event group deployment information, will be returned
        as a dict with information
        """

        if ar_event_groups is None:
            return {}

        event_group_depls = ar_event_groups.find_elements_of_type("SOMEIP-EVENT-GROUP")
        result = {"event_groups": collections.OrderedDict()}
        allGroupIdOb = {}
        for event_group in event_group_depls:
            fqn = event_group.get_fqn()
            eventId = transition_number(get_element_or_die(event_group, "EVENT-GROUP-ID"))
            if str(eventId) in allGroupIdOb:
                oldFqn = allGroupIdOb[str(eventId)]
                assert False, "[CM  ] CODE-017: Event group Id[{}] is duplicate in event group [{}, {}]. FQN:{}. LocalPath:{}. ".format(str(eventId), oldFqn, fqn, fqn, event_group.get_path())
            else:
                allGroupIdOb[str(eventId)] = fqn
            result["event_groups"][fqn] = {
                "id": transition_number(event_group.EVENT_GROUP_ID),
                "events": []
            }
            if isPR is not None and ar_si is not None:
                if isPR == "P":
                    ar_someip_provided_event_groups = ar_si.find_elements_of_type('SOMEIP-PROVIDED-EVENT-GROUP')
                    for ar_someip_provided_event_group in ar_someip_provided_event_groups:
                        event_group_ref = get_element_or_none(ar_someip_provided_event_group, "EVENT-GROUP-REF")
                        if event_group_ref and event_group_ref.text == fqn:
                            subs = self.get_subs(ports_config, fqn)
                            result["event_groups"][fqn]["subs"] = subs
                            multicast_threshold = transition_number(get_element_or_0(ar_someip_provided_event_group, "MULTICAST-THRESHOLD"))
                            result["event_groups"][fqn]["threshold"] = multicast_threshold
                            ipv4address = get_element_or_none(ar_someip_provided_event_group, "IPV-4-MULTICAST-IP-ADDRESS")
                            ipv6address = get_element_or_none(ar_someip_provided_event_group, "IPV-6-MULTICAST-IP-ADDRESS")
                            udpport = get_element_or_none(ar_someip_provided_event_group, "EVENT-MULTICAST-UDP-PORT")
                            if ipv4address:
                                result["event_groups"][fqn]["multicast_ip"] = ipv4address.text
                            elif ipv6address:
                                result["event_groups"][fqn]["multicast_ip"] = "["+ipv6address.text+"]"

                            if udpport is not None:
                                result["event_groups"][fqn]["muticast_port"] = udpport.text

                            configRef = get_element_or_none(ar_someip_provided_event_group, "SD-SERVER-EVENT-GROUP-TIMING-CONFIG-REF")
                            showConfig = False
                            if configRef is not None:
                                showConfig = True
                                configEle = self.model.find_referable(configRef.text)
                                request_response_delay = get_element_or_none(configEle, "REQUEST-RESPONSE-DELAY")
                                if request_response_delay is not None:
                                    result["event_groups"][fqn]["request_response_delay"] = {
                                        "max_value": to_milliseconds(get_element_or_die(request_response_delay, "MAX-VALUE")),
                                        "min_value": to_milliseconds(get_element_or_die(request_response_delay, "MIN-VALUE"))
                                    }
                            result["event_groups"][fqn]["showConfig"] = showConfig
                elif isPR == "R":
                    ar_someip_required_event_groups = ar_si.find_elements_of_type('SOMEIP-REQUIRED-EVENT-GROUP')
                    for ar_someip_required_event_group in ar_someip_required_event_groups:
                        event_group_ref = get_element_or_none(ar_someip_required_event_group, "EVENT-GROUP-REF")
                        if event_group_ref and event_group_ref.text == fqn:
                            ar_sdclienteventgrouptimingconfigref = get_element_or_none(ar_someip_required_event_group, "SD-CLIENT-EVENT-GROUP-TIMING-CONFIG-REF")
                            showConfig = False
                            if ar_sdclienteventgrouptimingconfigref is not None:
                                showConfig = True
                                ar_sdclienteventgrouptimingconfig = self.model.find_referable(ar_sdclienteventgrouptimingconfigref.text)
                                subscribe_timeo = get_element_or_none(ar_sdclienteventgrouptimingconfig, "SUBSCRIBE-EVENTGROUP-RETRY-DELAY")
                                if subscribe_timeo is not None:
                                    result["event_groups"][fqn]["subscribe_timeo"] = to_milliseconds(transition_number(subscribe_timeo))
                                subscribe_renew = get_element_or_none(ar_sdclienteventgrouptimingconfig, "SUBSCRIBE-EVENTGROUP-RETRY-MAX")
                                if subscribe_renew is not None:
                                    result["event_groups"][fqn]["subscribe_renew"] = transition_number(subscribe_renew)
                                ttl = get_element_or_none(ar_sdclienteventgrouptimingconfig, "TIME-TO-LIVE")
                                if ttl is not None:
                                    result["event_groups"][fqn]["ttl"] = transition_number(ttl)
                                request_response_delay = get_element_or_none(ar_sdclienteventgrouptimingconfig, "REQUEST-RESPONSE-DELAY")
                                if request_response_delay is not None:
                                    result["event_groups"][fqn]["request_response_delay"] = {
                                        "max_value": to_milliseconds(get_element_or_die(request_response_delay, "MAX-VALUE")),
                                        "min_value": to_milliseconds(get_element_or_die(request_response_delay, "MIN-VALUE"))
                                    }
                            result["event_groups"][fqn]["showConfig"] = showConfig
            event_refs = event_group.find_elements_of_type("EVENT-REF", DEST="SOMEIP-EVENT-DEPLOYMENT")
            for event_ref in event_refs:
                for _, depl in events_deployment.items():
                    if depl["name"] == event_ref:
                        result["event_groups"][fqn]["events"].append(depl["id"])
                        break
                for _, depl in fields_deployment.items():
                    if depl:
                        notifier_depl = depl["notifier"]
                        if (notifier_depl and
                                notifier_depl["name"] == event_ref):
                            result["event_groups"][fqn]["events"].append(notifier_depl["id"])
                            break
        return result

    def get_instance_deployment(self, ar_deployment, ar_si=None, ports_config=None, isPR=None):
        """
        Extract someip deployment information, will be returned as a
        dict with information
        """
        serviceId = transition_number(get_element_or_die(ar_deployment, "SERVICE-INTERFACE-ID"))
        if serviceId < 0 or serviceId > 65535:
            assert False, "[CM  ] CODE-013: The value of attribute SOMEIP-SERVICE-INTERFACE-DEPLOYMENT.SERVICE-INTERFACE-ID[{}] shall not exceed the closed interval 0 .. 65535. FQN:{}. LocalPath:{}. ".format(str(serviceId), ar_deployment.get_fqn(), ar_deployment.get_path())
        processed_events = []
        processed_methods = []
        result = {
            "service_id": get_element_or_die(ar_deployment, "SERVICE-INTERFACE-ID"),
            "events": {}, "methods": {}, "fields": {}, "event_groups": {},
            "have_event_notifier": False, "have_method_get_set": False,
            "service_major_version": transition_number(get_element_or_none(ar_deployment, "SERVICE-INTERFACE-VERSION/MAJOR-VERSION"),defaultInt=0),
            "service_minor_version": transition_number(get_element_or_none(ar_deployment, "SERVICE-INTERFACE-VERSION/MINOR-VERSION"),defaultInt=0)
        }
        event_group_depls = {"event_groups": {}}
        interface_ref = get_element_or_none(ar_deployment, "SERVICE-INTERFACE-REF")
        interface_ele = self.model.find_referable(interface_ref.text)
        interfaceOb = {
            "events": [],
            "methods": [],
            "fields": []
        }

        events = get_element_or_none(interface_ele, "EVENTS/VARIABLE-DATA-PROTOTYPE")
        if events is not None:
            for item in events:
                interfaceOb["events"].append(item.get_fqn())
        methods = get_element_or_none(interface_ele, "METHODS/CLIENT-SERVER-OPERATION")
        if methods is not None:
            for item in methods:
                interfaceOb["methods"].append(item.get_fqn())
        fields = get_element_or_none(interface_ele, "FIELDS/FIELD")
        if fields is not None:
            for item in fields:
                interfaceOb["fields"].append(item.get_fqn())
        event_deployment = []
        ar_someip_event_deployments = get_element_or_none(ar_deployment, "EVENT-DEPLOYMENTS/SOMEIP-EVENT-DEPLOYMENT")
        ar_someip_event_deployments = to_list(ar_someip_event_deployments)
        for ar_ed in ar_someip_event_deployments:
            event_deployment.append(ar_ed.get_fqn())
            key = get_element_or_die(ar_ed, "EVENT-REF").text
            if key in interfaceOb["events"]:
                interfaceOb["events"].remove(key)
            result["events"][key] = self._get_event_deployment(ar_ed, processed_events, ar_si)
            result["have_event_notifier"] = True
        ar_someip_field_deployments = get_element_or_none(ar_deployment, "FIELD-DEPLOYMENTS/SOMEIP-FIELD-DEPLOYMENT")
        ar_someip_field_deployments = to_list(ar_someip_field_deployments)
        for ar_fd in ar_someip_field_deployments:
            key = get_element_or_die(ar_fd, "FIELD-REF").text
            if key in interfaceOb["fields"]:
                interfaceOb["fields"].remove(key)
            fieldsEle = self._get_field_deployment(ar_fd, processed_events, processed_methods, ar_si)
            if "name" in fieldsEle["notifier"]:
                event_deployment.append(fieldsEle["notifier"]["name"])
                result["have_event_notifier"] = True
            if "name" in fieldsEle["getter"] or "name" in fieldsEle["setter"]:
                result["have_method_get_set"] = True
            self.interface_field_deployment(interface_ele, key, fieldsEle, ar_deployment)
            result["fields"][key] = fieldsEle

        ar_someip_method_deployments = get_element_or_none(ar_deployment, "METHOD-DEPLOYMENTS/SOMEIP-METHOD-DEPLOYMENT")
        ar_someip_method_deployments = to_list(ar_someip_method_deployments)
        for ar_md in ar_someip_method_deployments:
            key = get_element_or_die(ar_md, "METHOD-REF").text
            if key in interfaceOb["methods"]:
                interfaceOb["methods"].remove(key)
            result["methods"][key] = self._get_method_deployment(ar_md, processed_methods, ar_si)
            result["have_method_get_set"] = True

        event_groups = to_list(get_element_or_none(ar_deployment, "EVENT-GROUPS/SOMEIP-EVENT-GROUP"))
        for event_group in event_groups:
            event_refs = to_list(get_element_or_none(event_group, "EVENT-REFS/EVENT-REF"))
            for ref in event_refs:
                if ref.text in event_deployment:
                    event_deployment.remove(ref.text)

        assert len(interfaceOb["events"]) + len(interfaceOb["fields"]) + len(interfaceOb["methods"]) == 0 and len(event_deployment) == 0, \
            "[CM  ] CODE-003: Failed (The interface [{}] should be deployed properly in [{}]).LocalPath:{}:{}.".format(interface_ele.get_fqn(), ar_deployment.get_fqn(), interface_ele.get_path(), ar_deployment.get_path())

        event_group_depls = self._get_eventgrps_deployment(
            get_element_or_none(ar_deployment, "EVENT-GROUPS"),
            result["events"],
            result["fields"],
            ar_si,
            ports_config,
            isPR
        )

        result = {**result, **event_group_depls}
        return result

    def interface_field_deployment(self, interface, filedFqn, filedEle, ar_deployment):
        interfaceFile = self.model.find_referable(filedFqn)
        configuration = True
        if interfaceFile.HAS_NOTIFIER and "name" not in filedEle["notifier"]:
            configuration = False
        if interfaceFile.HAS_GETTER and "name" not in filedEle["getter"]:
            configuration = False
        if interfaceFile.HAS_SETTER and "name" not in filedEle["setter"]:
            configuration = False
        assert configuration, \
            "[CM  ] CODE-003: Failed (The interface [{}] should be deployed properly in [{}]).LocalPath:{}:{}.".format(interface.get_fqn(), ar_deployment.get_fqn(), interface.get_path(), ar_deployment.get_path())

    def get_provided_si_deployment(self, provided_si):
        """
        Extract provided someip deployment information, will be returned as a
        dict with information
        """
        load_balancing_priority = get_element_or_none(provided_si, "LOAD-BALANCING-PRIORITY")
        load_balancing_weight = get_element_or_none(provided_si, "LOAD-BALANCING-WEIGHT")
        sd_config_ref = get_element_or_none(provided_si, "SD-SERVER-CONFIG-REF")
        sd_config = self.model.find_referable(str(sd_config_ref)) if sd_config_ref is not None else None
        showSdConfig = False
        service_offer_time_to_live = None
        if sd_config is not None:
            showSdConfig = True
            service_offer_time_to_live = transition_number(get_element_or_die(sd_config, "SERVICE-OFFER-TIME-TO-LIVE"))
        offer_cyclic_delay = get_element_or_none(sd_config, "OFFER-CYCLIC-DELAY")
        initial_offer = get_element_or_none(sd_config, "INITIAL-OFFER-BEHAVIOR")
        showInitial = False
        initial_delay_max = None
        initial_delay_min = None
        if initial_offer is not None:
            showInitial = True
            initial_delay_max = get_element_or_die(initial_offer, "INITIAL-DELAY-MAX-VALUE")
            initial_delay_min = get_element_or_die(initial_offer, "INITIAL-DELAY-MIN-VALUE")
        base_delay = get_element_or_none(initial_offer, "INITIAL-REPETITIONS-BASE-DELAY")
        initial_repetitions_max = transition_number(get_element_or_none(initial_offer, "INITIAL-REPETITIONS-MAX"), defaultInt=None)
        responseDelay = get_element_or_none(sd_config, "REQUEST-RESPONSE-DELAY")
        showResponseDelay = False
        request_response_delay_max = None
        request_response_delay_min = None
        if responseDelay is not None:
            showResponseDelay = True
            request_response_delay_max = get_element_or_die(responseDelay, "MAX-VALUE")
            request_response_delay_min = get_element_or_die(responseDelay, "MIN-VALUE")
        instance_id = transition_number(get_element_or_none(provided_si, "SERVICE-INSTANCE-ID"), defaultInt=None)
        isShow = True
        sdg_caption = get_element_or_none(provided_si, "ADMIN-DATA/SDGS/SDG/SDG-CAPTION")
        if sdg_caption is not None:
            short_name_sdg = get_element_or_none(sdg_caption, "SHORT-NAME")
            if short_name_sdg is not None and "DiagnosticInterfaceDeployment" == short_name_sdg:
                isShow = False
        result = {
            "sd_config": {
                "showSdConfig": showSdConfig,
                "showInitial": showInitial,
                "initial_delay_max": to_milliseconds(initial_delay_max),
                "initial_delay_min": to_milliseconds(initial_delay_min),
                "initial_repetitions_base_delay": to_milliseconds(base_delay),
                "initial_repetitions_max": initial_repetitions_max,
                "cyclic_delay": to_milliseconds(offer_cyclic_delay),
                "ttl": service_offer_time_to_live,
                "showResponseDelay": showResponseDelay,
                "request_response_delay_max": to_milliseconds(request_response_delay_max),
                "request_response_delay_min": to_milliseconds(request_response_delay_min)
            },
            "instance_id": int_or_unmodified(instance_id),
            "load_balancing_priority": load_balancing_priority,
            "load_balancing_weight": load_balancing_weight,
            "is_show": isShow
        }

        return result

    def get_required_si_deployment(self, required_si):
        """
        Extract required someip deployment information, will be returned as a
        dict with information
        """
        load_balancing_priority = get_element_or_none(required_si, "LOAD-BALANCING-PRIORITY")
        load_balancing_weight = get_element_or_none(required_si, "LOAD-BALANCING-WEIGHT")
        sd_config_ref = get_element_or_none(required_si, "SD-CLIENT-CONFIG-REF")
        sd_config = self.model.find_referable(str(sd_config_ref)) if sd_config_ref is not None else None
        showSdConfig = False
        service_find_time_to_live = None
        if sd_config is not None:
            showSdConfig = True
            service_find_time_to_live = transition_number(get_element_or_die(sd_config, "SERVICE-FIND-TIME-TO-LIVE"))
        initial_find = get_element_or_none(sd_config, "INITIAL-FIND-BEHAVIOR")
        showInitial = False
        initial_delay_max = None
        initial_delay_min = None
        if initial_find is not None:
            showInitial = True
            initial_delay_max = get_element_or_die(initial_find, "INITIAL-DELAY-MAX-VALUE")
            initial_delay_min = get_element_or_die(initial_find, "INITIAL-DELAY-MIN-VALUE")
        base_delay = get_element_or_none(initial_find, "INITIAL-REPETITIONS-BASE-DELAY")
        initial_repetitions_max = transition_number(get_element_or_none(initial_find, "INITIAL-REPETITIONS-MAX"), defaultInt=None)
        instance_id = transition_number(get_element_or_none(required_si, "REQUIRED-SERVICE-INSTANCE-ID"), defaultInt=None)

        isShow = True
        sdg_caption = get_element_or_none(required_si, "ADMIN-DATA/SDGS/SDG/SDG-CAPTION")
        if sdg_caption is not None:
            short_name_sdg = get_element_or_none(sdg_caption, "SHORT-NAME")
            if short_name_sdg is not None and "DiagnosticInterfaceDeployment" == short_name_sdg:
                isShow = False
        minor_version = self._get_required_minimum_minor_version(required_si)
        version_driven_find_behavior = self._get_version_driven_find_behavior(required_si)
        if version_driven_find_behavior is not None and version_driven_find_behavior == "MINIMUM-MINOR-VERSION":
            if minor_version is None:
                assert False, "[CM  ] CODE-004: RequiredSomeipServiceInstance[{}].versionDrivenFindBehavior = \"MINIMUM-MINOR-VERSION\", requiredMinorVersion must be configured. FQN:{}. LocalPath:{}. ".format(required_si.SHORT_NAME, required_si.get_fqn(), required_si.get_path())
        result = {
            "sd_config": {
                "showSdConfig": showSdConfig,
                "showInitial": showInitial,
                "initial_delay_max": to_milliseconds(initial_delay_max),
                "initial_delay_min": to_milliseconds(initial_delay_min),
                "initial_repetitions_base_delay": to_milliseconds(base_delay),
                "initial_repetitions_max": initial_repetitions_max,
                "ttl": service_find_time_to_live
            },
            "instance_id": int_or_unmodified(instance_id),
            "is_show": isShow,
            "required_minimum_minor_version": minor_version,
            "version_driven_find_behavior": version_driven_find_behavior,
            "version_driven_find_behavior_": get_hump(version_driven_find_behavior),
            "load_balancing_priority": load_balancing_priority,
            "load_balancing_weight": load_balancing_weight,
            "blacklisted_versions": self._get_blacklisted_versions(required_si)
        }
        return result

    def _get_required_minimum_minor_version(self, required_si):
        required_minimum_minor_version = get_element_or_none(required_si, "REQUIRED-MINOR-VERSION")
        return required_minimum_minor_version

    def _get_version_driven_find_behavior(self, required_si):
        version_driven_find_behavior = get_element_or_none(required_si, "VERSION-DRIVEN-FIND-BEHAVIOR")
        return version_driven_find_behavior

    def _get_blacklisted_versions(self, required_si):
        ar_blacklisted_versions = get_element_or_none(required_si, "BLACKLISTED-VERSIONS/SOMEIP-SERVICE-VERSION")
        ar_blacklisted_versions = to_list(ar_blacklisted_versions)
        blacklisted_versions = []
        for ar_bv in ar_blacklisted_versions:
            major = get_element_or_none(ar_bv, "MAJOR-VERSION")  # multiplicity: 0..1
            major = transition_number(major,defaultInt=0)
            minor = get_element_or_die(ar_bv, "MINOR-VERSION")  # multiplicity: 1
            minor = transition_number(minor,defaultInt=0)
            version = {"major": major, "minor": minor}
            blacklisted_versions.append(version)
        return blacklisted_versions

    def get_net_ports_config(self, ar_instance_mapping):
        """
        Get network port config information, will be returned as a dict
        with information
        """
        ports_config = {
            "ar_instance_mapping": ar_instance_mapping.get_fqn()
        }
        ar_tcp_port = transition_number(get_element_or_none(ar_instance_mapping, "TCP-PORT"), defaultInt=None)
        ar_udp_port = transition_number(get_element_or_none(ar_instance_mapping, "UDP-PORT"), defaultInt=None)
        size_threshold = transition_number(get_element_or_none(ar_instance_mapping, "UDP-COLLECTION-BUFFER-SIZE-THRESHOLD"), defaultInt=None)
        if ar_tcp_port is not None:
            ports_config["tcp"] = ar_tcp_port
            ports_config["tcpEnd"] = ar_tcp_port
        if ar_udp_port is not None:
            ports_config["udp"] = ar_udp_port
            ports_config["udpEnd"] = ar_udp_port
        comPropTcp = get_element_or_none(ar_instance_mapping, "SECURE-COM-PROPS-FOR-TCP-REFS/SECURE-COM-PROPS-FOR-TCP-REF")
        if comPropTcp is not None:
            comPropTcpOb = self.model.find_referable(comPropTcp.text)
            tlsCryptoCipherSuite = get_element_or_none(comPropTcpOb, "TLS-CIPHER-SUITES/TLS-CRYPTO-CIPHER-SUITE")
            if tlsCryptoCipherSuite is not None:
                tlsCryptoCipherSuiteOb = {
                    "tls_version": "12",
                    "tls_verify_client": "0",
                    "tls_peer_ca": "",
                    "tls_local_cert": "",
                    "tls_local_privkey": ""
                }
                ports_config["tlsCryptoCipherSuiteOb"] = tlsCryptoCipherSuiteOb
                version = get_element_or_none(tlsCryptoCipherSuite, "TLS-CIPHER-VERSION")
                if version is not None:
                    tlsCryptoCipherSuiteOb["tls_version"] = str(version).split("-")[-1]
                sdgTlv = get_element_or_none(tlsCryptoCipherSuite, "ADMIN-DATA/SDGS/SDG")
                if sdgTlv is not None and sdgTlv.attrib["GID"] == "iSOFT:com:TlsCryptoCipherSuite":
                    sdList = to_list(get_element_or_none(sdgTlv,"SD"))
                    for sdItem in sdList:
                        if sdItem.attrib["GID"] == "iSOFT:com:TlsCryptoCipherSuite:TlsCipherSuiteName":
                            tlsCryptoCipherSuiteOb["tls_ciphersuite_name"] = sdItem.text
                        elif sdItem.attrib["GID"] == "iSOFT:com:TlsCryptoCipherSuite:TlsCipherPSKKey":
                            tlsCryptoCipherSuiteOb["tls_psk_key_hex"] = sdItem.text
                        elif sdItem.attrib["GID"] == "iSOFT:com:TlsCryptoCipherSuite:TlsVerifyClient":
                            tlsCryptoCipherSuiteOb["tls_verify_client"] = str(sdItem.text)
                        elif sdItem.attrib["GID"] == "iSOFT:com:TlsCryptoCipherSuite:TlsPeerCAFile":
                            tlsCryptoCipherSuiteOb["tls_peer_ca"] = sdItem.text
                        elif sdItem.attrib["GID"] == "iSOFT:com:TlsCryptoCipherSuite:TlsLocalCertFile":
                            tlsCryptoCipherSuiteOb["tls_local_cert"] = sdItem.text
                        elif sdItem.attrib["GID"] == "iSOFT:com:TlsCryptoCipherSuite:TlsLocalPrivateKeyFile":
                            tlsCryptoCipherSuiteOb["tls_local_privkey"] = sdItem.text

        communication_connector_ref = get_element_or_none(ar_instance_mapping, "COMMUNICATION-CONNECTOR-REF")
        if communication_connector_ref is not None:
            communication_connector = self.model.find_referable(communication_connector_ref.text)
            endpoint_info_ref = get_element_or_none(communication_connector, "UNICAST-NETWORK-ENDPOINT-REF")
            if endpoint_info_ref is not None:
                endpoint_info = self.model.find_referable(endpoint_info_ref.text)
                ip = commen_util.get_network_address(endpoint_info)
                if ip != "":
                    ports_config["host"] = ip
        ports_config["size_threshold"] = size_threshold
        unICastConfigList = []
        sdg = get_element_or_none(ar_instance_mapping, "ADMIN-DATA/SDGS/SDG")
        if sdg is not None and sdg.attrib["GID"] == "iSOFT:com:SomeipServiceInstanceToMachineMapping":
            sdgList = get_element_or_none(sdg, "SDG")
            if sdgList is not None:
                sdgList = to_list(sdgList)
                for sdgItem in sdgList:
                    if sdgItem.attrib["GID"] == "iSOFT:com:SomeipServiceInstanceToMachineMapping:SomeipRemoteUnicastConfig":
                        unICastConfig = {
                            "strategy": "Static"
                        }
                        sdList = get_element_or_none(sdgItem, "SD")
                        if sdList is not None:
                            sdList = to_list(sdList)
                            for sd in sdList:
                                if sd.attrib["GID"] == "iSOFT:com:SomeipRemoteUnicastConfig:Ipv4Address":
                                    unICastConfig["ipV4"] = sd.text
                                elif sd.attrib["GID"] == "iSOFT:com:SomeipRemoteUnicastConfig:Ipv6Address":
                                    unICastConfig["ipV6"] = sd.text
                                elif sd.attrib["GID"] == "iSOFT:com:SomeipRemoteUnicastConfig:TcpPort":
                                    unICastConfig["tcp"] = sd.text
                                elif sd.attrib["GID"] == "iSOFT:com:SomeipRemoteUnicastConfig:UdpPort":
                                    unICastConfig["udp"] = sd.text
                                elif sd.attrib["GID"] == "iSOFT:com:SomeipRemoteUnicastConfig:ClientSDStrategy":
                                    unICastConfig["strategy"] = sd.text
                        refSdgList = get_element_or_none(sdgItem, "SDG")
                        eventGroupList = []
                        if refSdgList is not None:
                            refSdgList = to_list(refSdgList)
                            for refSdg in refSdgList:
                                if refSdg.attrib["GID"] == "iSOFT:com:SomeipRemoteUnicastConfig:SomeipEventGroup":
                                    sdxRef = get_element_or_none(refSdg, "SDX-REF")
                                    if sdxRef is not None:
                                        sdxRefList = to_list(sdxRef)
                                        for item in sdxRefList:
                                            if item.attrib["DEST"] == "SOMEIP-EVENT-GROUP":
                                                eventGroupList.append(item.text)
                        unICastConfig["eventGroupList"] = eventGroupList
                        unICastConfigList.append(unICastConfig)
            sdList = get_element_or_none(sdg, "SD")
            if sdList is not None:
                sdList = to_list(sdList)
                for sdItem in sdList:
                    if sdItem.attrib["GID"] == "iSOFT:com:SomeipServiceInstanceToMachineMapping:ClientBindUdpEndPort":
                        ports_config["udpEnd"] = sdItem.text
                    elif sdItem.attrib["GID"] == "iSOFT:com:SomeipServiceInstanceToMachineMapping:ClientBindTcpEndPort":
                        ports_config["tcpEnd"] = sdItem.text

        ports_config["unICastConfigList"] = unICastConfigList
        return ports_config

    def _populate_eventgrp_deployment(self, service_deployment, event_group_depls):
        """Populate an event group deployment"""
        for event_group_depl_dict in event_group_depls.values():
            event_group_id = int(event_group_depl_dict["id"])
            threshold = 0
            if "threshold" in event_group_depl_dict:
                threshold = event_group_depl_dict["threshold"]
            event_group_deployment = EventGroupDeployment(event_group_id)
            event_group_deployment.set_threshold(threshold)
            if "subs" in event_group_depl_dict:
                event_group_deployment.set_subs(event_group_depl_dict["subs"])
            if "subscribe_timeo" in event_group_depl_dict:
                event_group_deployment.set_subscribe_timeo(event_group_depl_dict["subscribe_timeo"])
            if "subscribe_renew" in event_group_depl_dict:
                event_group_deployment.set_subscribe_renew(event_group_depl_dict["subscribe_renew"])
            if "ttl" in event_group_depl_dict:
                event_group_deployment.set_ttl(event_group_depl_dict["ttl"])
            if "showConfig" in event_group_depl_dict:
                event_group_deployment.showConfig = event_group_depl_dict["showConfig"]
            if "request_response_delay" in event_group_depl_dict:
                event_group_deployment.request_response_delay = event_group_depl_dict["request_response_delay"]
            service_deployment.add_event_group_deployment(
                event_group_deployment)
            for event in event_group_depl_dict["events"]:
                event_group_deployment.add_event(int(event))
            if "muticast_port" in event_group_depl_dict:
                muticast_port = event_group_depl_dict["muticast_port"]
                event_group_deployment.set_muticast_port(muticast_port)
            if "multicast_ip" in event_group_depl_dict:
                multicast_ip = event_group_depl_dict["multicast_ip"]
                event_group_deployment.set_multicast_ip(multicast_ip)

    def _populate_event_deployment(self, events, event_deployment):
        """Populate an event deployment"""
        for event_fqn, event_depl in event_deployment.items():
            event_name = short_name(event_fqn)
            for event in events:
                if event.name == event_name:
                    event.event_deployment = EventDeployment()
                    event.event_deployment.deployment_id = int(
                        event_depl["id"])
                    event.event_deployment.proto = event_depl["proto"]
                    event.event_deployment.serializer = event_depl["serializer"]
                    event.event_deployment.event_depl = event_depl

    def _populate_method_deployment(self, methods, method_deployment):
        """Populate a method deployment"""
        for method_fqn, method_depl in method_deployment.items():
            method_name = short_name(method_fqn)
            for method in methods:
                if method.name == method_name:
                    method.deployment = MethodDeployment()
                    method.deployment.deployment_id = method_depl["id"]
                    method.deployment.proto = method_depl["proto"]
                    method.deployment.method_depl = method_depl

    def _populate_field_deployment(self, fields, field_deployment):
        """Populate a field deployment"""
        for field_fqn, field_depl in field_deployment.items():
            field_name = short_name(field_fqn)
            field_getter = field_depl["getter"]
            field_setter = field_depl["setter"]
            field_notifier = field_depl["notifier"]
            for field in fields:
                if field.name == field_name:
                    field.deployment = FieldDeployment()
                    field.deployment["getter"] = field_getter
                    field.deployment["setter"] = field_setter
                    field.deployment["notifier"] = field_notifier

    def populate_deployment(self, service, deployment):
        """Populate a service deployment"""
        if deployment.required:
            self._populate_event_deployment(
                service.events, deployment.required["events"])
            self._populate_method_deployment(
                service.methods, deployment.required["methods"])
            self._populate_field_deployment(
                service.fields, deployment.required["fields"])
            self._populate_eventgrp_deployment(
                deployment,
                deployment.required["event_groups"])
        elif deployment.provided:
            self._populate_event_deployment(
                service.events, deployment.provided["events"])
            self._populate_method_deployment(
                service.methods, deployment.provided["methods"])
            self._populate_field_deployment(
                service.fields, deployment.provided["fields"])
            self._populate_eventgrp_deployment(
                deployment,
                deployment.provided["event_groups"])

    def get_subs(self, ports_config, eventgroupfqn):
        subList = []
        if ports_config is not None and "unICastConfigList" in ports_config:
            unICastConfigList = ports_config["unICastConfigList"]
            for unICastConfig in unICastConfigList:
                eventGroupList = unICastConfig["eventGroupList"]
                if eventgroupfqn in eventGroupList:
                    sub = {}
                    ip = None
                    if "ipV4" in unICastConfig:
                        ip = unICastConfig["ipV4"]
                    elif "ipV6" in unICastConfig:
                        ip = unICastConfig["ipV6"]
                    if "tcp" in unICastConfig and ip is not None:
                        sub["unicastTcpIp"] = "{}:{}".format(ip, unICastConfig["tcp"])
                    if "udp" in unICastConfig and ip is not None:
                        sub["unicastUdpIp"] = "{}:{}".format(ip, unICastConfig["udp"])
                    strategy = unICastConfig["strategy"]
                    if sub not in subList and strategy == "Static":
                        subList.append(sub)

        return subList
