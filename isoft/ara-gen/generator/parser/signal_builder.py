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
# pylint: disable=too-many-lines,too-many-public-methods

from generator.intermediate_model.communication_management.ara_com_deployment.someip.someip_service_deployment import SomeIpServiceDeployment
from generator.intermediate_model.communication_management.ara_com_deployment.user_defined.user_defined_service_deployment import UserDefinedServiceDeployment
from generator.intermediate_model.communication_management.etosmapping import EventToSignalMapping
from generator.intermediate_model.communication_management.stoemapping import SignalToEventMapping
from generator.intermediate_model.communication_management.sub_eandsmapping import SubEvenAndSignalMapping


from generator.intermediate_model.communication_management.signal import Signal
from generator.intermediate_model.communication_management.signaltype_item import Signaltypeitem

from generator.intermediate_model.communication_management.signal_bindinfo import SignalBindInfo
from generator.intermediate_model.communication_management.socketinfo import SocketInfo



from generator.common.tree_helper import get_element_or_die, get_element_or_none, to_list, int_or_unmodified, get_element_or_str, get_element_or_0, get_element_or_false, transition_number, to_str


class SignalBuilder(object):
    """
    Handles parsing the ARXML part, related to network binding, and provides service deployment objects.
    """

    def __init__(self, model):

        self.model = model
    def IsHex(self ,s):
        if "0x" in s:
            return True
        else:
            return False

    def get_signals_byservicedeploymentid(self ,servicedeploymentid):

        serviceid10 =  transition_number(servicedeploymentid)

        result = []

        ar_soconopduidentfiers = self.model.find_elements_of_type('SOCKET-CONNECTION-IPDU-IDENTIFIER')
        for ar_soconopduidentfier in ar_soconopduidentfiers:
            headerid = get_element_or_none(ar_soconopduidentfier, 'HEADER-ID')
            headeridint = int_or_unmodified(headerid)



            serviceid = headeridint >> 16
            eventid = headeridint & 0xFFFF
#            signalname = get_element_or_none(ar_soconopduidentfier,'SHORT-NAME')
            signalname =ar_soconopduidentfier.attrib["S"]

            if serviceid == serviceid10:
                pdutriggering_ref = get_element_or_none(ar_soconopduidentfier, 'PDU-TRIGGERING-REF')
                if pdutriggering_ref is not None:
                    ar_pdutriggering = self.model.find_referable(pdutriggering_ref.text)

                    pduport_refs = get_element_or_none(ar_pdutriggering, 'I-PDU-PORT-REFS/I-PDU-PORT-REF')

                    for pduport_ref in pduport_refs:
                        ar_pduport = self.model.find_referable(pduport_ref.text)
                        communi_direct = get_element_or_str(ar_pduport, "COMMUNICATION-DIRECTION")

                    pdu_ref = get_element_or_die(ar_pdutriggering, "I-PDU-REF")
                    ar_pdu = self.model.find_referable(pdu_ref.text)
                    pduname = pdu_ref.text.split("/")[-1]
                    signal = Signal(signalname)
                    signal.set_serviceid(serviceid)
                    signal.set_eventid(eventid + 32768)

                    ar_isignaltoipdumappings = ar_pdu.find_elements_of_type("I-SIGNAL-TO-I-PDU-MAPPING")
                    hasdynamiclength = get_element_or_false(ar_pdu, "HAS-DYNAMIC-LENGTH")

                    signal.set_hasdynamiclenght(hasdynamiclength)
                    for ar_isignaltoipdumapping in ar_isignaltoipdumappings:
                        signal_ref = get_element_or_none(ar_isignaltoipdumapping, 'I-SIGNAL-REF')

                        if signal_ref:
                            byte_order = get_element_or_str(ar_isignaltoipdumapping, 'PACKING-BYTE-ORDER')
                            start_position = get_element_or_0(ar_isignaltoipdumapping, 'START-POSITION')

                            ar_signal = self.model.find_referable(signal_ref.text)
                            shortname = get_element_or_none(ar_signal, "SHORT-NAME")
                            signaletype = get_element_or_str(ar_signal, "I-SIGNAL-TYPE")
                            length = get_element_or_0(ar_signal, "LENGTH")
                            signaltypeitem = Signaltypeitem(to_str(shortname), signaletype, length)
                            signaltypeitem.set_packing_byte_order(byte_order)
                            signaltypeitem.set_start_position(start_position)
                            signal.add_typeitem(signaltypeitem)

                    result.append(signal)

        return result

    def get_eventtosignalmapping(self, mappings, event):
        for eventtosignalmapping in mappings:
            if eventtosignalmapping.event == event:
                return eventtosignalmapping
        return None

    def get_signaltoeventmapping(self, mappings, signal):
        for signaltoeventmapping in mappings:
            if signaltoeventmapping.signal == signal:
                return signaltoeventmapping
        return None

    def IsSignalIN_OR_OUT(self, sinalfqn, inorout):
        ar_pdutriggerings = self.model.find_elements_of_type("PDU-TRIGGERING")

        for ar_pdutriggering in ar_pdutriggerings:
            pduport_refs = get_element_or_none(ar_pdutriggering, 'I-PDU-PORT-REFS/I-PDU-PORT-REF')

            ar_soconipduidentfiers = self.model.find_elements_of_type(
                'SO-CON-I-PDU-IDENTIFIER',
                accept=lambda e: str(
                    e.PDU_TRIGGERING_REF) == ar_pdutriggering.get_fqn())
            signalname = ""
            for ar_soconipduidentfier in ar_soconipduidentfiers:
                signalname = ar_soconipduidentfier.get_fqn().split("/")[-1]

            direct = []
            for pduport_ref in pduport_refs:
                ar_pduport = self.model.find_referable(pduport_ref.text)
                communi_direct = get_element_or_str(ar_pduport, "COMMUNICATION-DIRECTION")
                direct.append(communi_direct)

            ipduref = get_element_or_die(ar_pdutriggering, 'I-PDU-REF')
            ar_isignalipdu = self.model.find_referable(ipduref.text)

            ar_isignaltoipdumappings = ar_isignalipdu.find_elements_of_type("I-SIGNAL-TO-I-PDU-MAPPING")
            for ar_isignaltoipdumapping in ar_isignaltoipdumappings:
                signalgroup_ref = get_element_or_none(ar_isignaltoipdumapping, 'I-SIGNAL-GROUP-REF')
                if signalgroup_ref:
                    ar_signalgroup = self.model.find_referable(signalgroup_ref.text)

                    signals_refs = ar_signalgroup.find_elements_of_type('I-SIGNAL-REF')
                    for signals_ref in signals_refs:
                        if signals_ref.text == sinalfqn:
                            if inorout in direct:
                                return signalname
                            else:
                                return ""
                else:
                    signal_ref = get_element_or_none(ar_isignaltoipdumapping, 'I-SIGNAL-REF')
                    if to_str(signal_ref) == sinalfqn:
                        if inorout in direct:
                            return signalname
                        else:
                            return ""

        return ""

    def get_eventtype_namespace(self, ar_eventtype):
        symbol_props = ar_eventtype.find_elements_of_type("SYMBOL-PROPS")
        namespace_list = []

        for symbol_prop in symbol_props:
            if symbol_prop.SYMBOL:
                namespace_list.append(symbol_prop.SYMBOL)
            else:
                namespace_list.append(symbol_prop.SHORT_NAME)

        # return [x.lower() for x in namespace_list]
        return namespace_list

    def get_etosmappings_byservice(self, servicefqn):

        result = []

        ar_signal_base_event_to_signal_mappings = self.model.find_elements_of_type('SIGNAL-BASED-EVENT-ELEMENT-TO-I-SIGNAL-TRIGGERING-MAPPING')
        for ar_signal_base_event_to_signal_mapping in ar_signal_base_event_to_signal_mappings:
            signalinterface_ref = get_element_or_none(ar_signal_base_event_to_signal_mapping,
                                                      'DATA-PROTOTYPE-IN-SERVICE-INTERFACE-REF/ELEMENT-IN-IMPL-DATATYPE/PORT-INTERFACE-REF')
            if signalinterface_ref and servicefqn == signalinterface_ref.text:
                eventtype = self.get_event_type(ar_signal_base_event_to_signal_mapping)
                transmissiontrigger = get_element_or_false(ar_signal_base_event_to_signal_mapping, "TRANSMISSION-TRIGGER")

                isignaletriggeringref = get_element_or_none(ar_signal_base_event_to_signal_mapping, "I-SIGNAL-TRIGGERING-REF")
                if isignaletriggeringref is not None:
                    ar_isignaletriggeringref = self.model.find_referable(isignaletriggeringref.text)
                    isignalref = get_element_or_none(ar_isignaletriggeringref, "I-SIGNAL-REF")
                    if isignalref is not None:
                        signalname = self.IsSignalIN_OR_OUT(isignalref.text, "OUT")
                        if signalname != "":
                            event = get_element_or_none(ar_signal_base_event_to_signal_mapping,
                                                        'DATA-PROTOTYPE-IN-SERVICE-INTERFACE-REF/ELEMENT-IN-IMPL-DATATYPE/ROOT-DATA-PROTOTYPE-REF')
                            if event is not None:
                                event = event.text.split("/")[-1]
                                eventtosignalmapping = self.get_eventtosignalmapping(result, event)
                                if not eventtosignalmapping:
                                    eventtosignalmapping = EventToSignalMapping()
                                    eventtosignalmapping.set_event(event)
                                    result.append(eventtosignalmapping)
                                eventtosignalmapping.add_signals(signalname)
                                if eventtype is not None:
                                    eventtosignalmapping.set_eventtype(eventtype)
                                subeventtosignalmapping = SubEvenAndSignalMapping()
                                subeventtosignalmapping.set_event(event)
                                subeventtosignalmapping.set_signal(signalname)

                                eventvalue = get_element_or_none(ar_signal_base_event_to_signal_mapping, 'DATA-PROTOTYPE-IN-SERVICE-INTERFACE-REF/ELEMENT-IN-IMPL-DATATYPE/TARGET-DATA-PROTOTYPE-REF')
                                if eventvalue is not None:
                                    eventvalue = eventvalue.text.split("/")[-1]
                                    subeventtosignalmapping.set_eventtypevalue(eventvalue)
                                ar_isignalref = self.model.find_referable(isignalref.text)
                                signalshortname = get_element_or_die(ar_isignalref, "SHORT-NAME")
                                subeventtosignalmapping.set_signaltypevalue(signalshortname.text)
                                subeventtosignalmapping.set_transmissiontrigger(transmissiontrigger)
                                eventtosignalmapping.add_submapping(subeventtosignalmapping)

        return result

    def get_stoemappings_byservice(self, servicefqn):
        result = []

        ar_signal_base_event_to_signal_mappings = self.model.find_elements_of_type(
            'SIGNAL-BASED-EVENT-ELEMENT-TO-I-SIGNAL-TRIGGERING-MAPPING')
        for ar_signal_base_event_to_signal_mapping in ar_signal_base_event_to_signal_mappings:
            signalinterface_ref = get_element_or_none(ar_signal_base_event_to_signal_mapping, 'DATA-PROTOTYPE-IN-SERVICE-INTERFACE-REF/ELEMENT-IN-IMPL-DATATYPE/PORT-INTERFACE-REF')
            if signalinterface_ref and servicefqn == signalinterface_ref.text:
                eventtype = self.get_event_type(ar_signal_base_event_to_signal_mapping)
                transmissiontrigger = get_element_or_false(ar_signal_base_event_to_signal_mapping, "TRANSMISSION-TRIGGER")
                isignaletriggeringref = get_element_or_none(ar_signal_base_event_to_signal_mapping, "I-SIGNAL-TRIGGERING-REF")
                if isignaletriggeringref is not None:
                    ar_isignaletriggeringref = self.model.find_referable(isignaletriggeringref.text)
                    isignalref = get_element_or_none(ar_isignaletriggeringref, "I-SIGNAL-REF")
                    if isignalref is not None:
                        signalname = self.IsSignalIN_OR_OUT(isignalref.text, "IN")
                        if signalname != "":
                            event = get_element_or_none(ar_signal_base_event_to_signal_mapping, 'DATA-PROTOTYPE-IN-SERVICE-INTERFACE-REF/ELEMENT-IN-IMPL-DATATYPE/ROOT-DATA-PROTOTYPE-REF')
                            if event is not None:
                                event = event.text.split("/")[-1]
                            signaltoeventmapping = self.get_signaltoeventmapping(result, signalname)
                            if not signaltoeventmapping:
                                signaltoeventmapping = SignalToEventMapping()
                                signaltoeventmapping.set_signal(signalname)
                                if event is not None:
                                    signaltoeventmapping.add_event(event)
                                result.append(signaltoeventmapping)
                            subsignaltoeventmapping = SubEvenAndSignalMapping()
                            if event is not None:
                                subsignaltoeventmapping.set_event(event)
                            subsignaltoeventmapping.set_signal(signalname)

                            signaltoeventmapping.set_eventtype(eventtype)

                            eventvalue = get_element_or_none(ar_signal_base_event_to_signal_mapping, 'DATA-PROTOTYPE-IN-SERVICE-INTERFACE-REF/ELEMENT-IN-IMPL-DATATYPE/TARGET-DATA-PROTOTYPE-REF')
                            if eventvalue is not None:
                                eventvalue = eventvalue.text.split("/")[-1]
                                subsignaltoeventmapping.set_eventtypevalue(eventvalue)
                            ar_isignalref = self.model.find_referable(isignalref.text)
                            signalshortname = get_element_or_die(ar_isignalref, "SHORT-NAME")
                            subsignaltoeventmapping.set_signaltypevalue(signalshortname.text)
                            subsignaltoeventmapping.set_transmissiontrigger(transmissiontrigger)
                            signaltoeventmapping.add_submapping(subsignaltoeventmapping)
        return result

    def get_event_type(self, ar_signal_base_event_to_signal_mapping):
        eventtype_ref = get_element_or_none(ar_signal_base_event_to_signal_mapping, 'DATA-PROTOTYPE-IN-SERVICE-INTERFACE-REF/ELEMENT-IN-IMPL-DATATYPE/TARGET-DATA-PROTOTYPE-REF')
        eventtype = None
        if eventtype_ref is not None:
            eventtype_text = "/".join(eventtype_ref.text.split("/")[0: -1])
            ar_eventtype = self.model.find_referable(eventtype_text)

            namespace = self.get_eventtype_namespace(ar_eventtype)
            eventtypename = eventtype_text.split("/")[-1]
            namespace.append(eventtypename)
            eventtype = "::".join(namespace)
        return eventtype

    def get_signalbindinfo(self, si_mapping):
        signalbinginfo = SignalBindInfo()

        self.get_SecOC(si_mapping, signalbinginfo)

        services_provided = [m['service'] for m in si_mapping['provided']]
        services_required = [m['service'] for m in si_mapping['required']]
        for service in services_provided:
            servicedeploymentid = service.service_deployment.deployment_id

            serviceid10 = transition_number(servicedeploymentid)

            ar_socketconnections = self.model.find_elements_of_type('SOCKET-CONNECTION')

            for ar_socketconnection in ar_socketconnections:
                headerid = get_element_or_none(ar_socketconnection, 'PDUS/SOCKET-CONNECTION-IPDU-IDENTIFIER/HEADER-ID')
                headeridint = int_or_unmodified(headerid)

                serviceid = headeridint >> 16
                eventid = headeridint & 0xFFFF

                if serviceid == serviceid10:
                    socketinfo = SocketInfo()
                    socketpro = get_element_or_none(ar_socketconnection, 'SOCKET-PROTOCOL')
                    if socketpro is not None:
                        socketinfo.set_socketpro(socketpro.text)
                    localport_ref = get_element_or_none(ar_socketconnection, 'LOCAL-PORT-REF')
                    if localport_ref is not None:
                        ar_localport_ref = self.model.find_referable(localport_ref.text)

                        localaddress = get_element_or_none(ar_localport_ref, 'IP-ADDRESS')
                        if localaddress is not None:
                            socketinfo.set_localaddress(localaddress.text)
                        localport = get_element_or_none(ar_localport_ref, 'PORT-ADDRESS')
                        if localport is not None:
                            socketinfo.set_localport(localport.text)
                    remoteport_ref = get_element_or_none(ar_socketconnection, 'REMOTE-PORT-REF')
                    if remoteport_ref is not None:
                        ar_remoteport_ref = self.model.find_referable(remoteport_ref.text)
                        remoteaddress = get_element_or_none(ar_remoteport_ref, 'IP-ADDRESS')
                        if remoteaddress is not None:
                            socketinfo.set_remoteaddress(remoteaddress.text)
                        remoteport = get_element_or_none(ar_remoteport_ref, 'PORT-ADDRESS')
                        if remoteport is not None:
                            socketinfo.set_remoteport(remoteport.text)

                    signalbinginfo.add_socketinfo(socketinfo)
        for service in services_required:
            servicedeploymentid = service.service_deployment.deployment_id

            serviceid10 = transition_number(servicedeploymentid)

            ar_socketconnections = self.model.find_elements_of_type('SOCKET-CONNECTION')

            for ar_socketconnection in ar_socketconnections:
                headerid = get_element_or_none(ar_socketconnection, 'PDUS/SOCKET-CONNECTION-IPDU-IDENTIFIER/HEADER-ID')
                headeridint = int_or_unmodified(headerid)

                serviceid = headeridint >> 16
                eventid = headeridint & 0xFFFF

                if serviceid == serviceid10:
                    for event in service.events:
                        if event.event_deployment.serializer == "SIGNAL-BASED":
                            signalbinginfo.add_service(service)

        return signalbinginfo

    def get_pdu_triggering(self, signalTriggeringRef):
        if signalTriggeringRef is not None:
            pduTriggeringMapping = self.model.find_elements_of_type("PDU-TRIGGERING")
            for pudItem in pduTriggeringMapping:
                iSignalTriggeringRefCons = get_element_or_none(pudItem, "I-SIGNAL-TRIGGERINGS/I-SIGNAL-TRIGGERING-REF-CONDITIONAL")
                for iSignalTriggeringRefCon in to_list(iSignalTriggeringRefCons):
                    iSignalTriggeringRef = get_element_or_none(iSignalTriggeringRefCon, "I-SIGNAL-TRIGGERING-REF")
                    if iSignalTriggeringRef is not None:
                        iSignalTriggeringRef = to_list(iSignalTriggeringRef)
                        for iSignalTriggeringRefItem in iSignalTriggeringRef:
                            if iSignalTriggeringRefItem.text == signalTriggeringRef.text:
                                return pudItem
        return None

    def get_key_slot(self, si_fqn):
        instance_ele = self.model.find_referable(si_fqn)
        com_config_list = get_element_or_none(instance_ele, "SECURE-COM-CONFIGS/SERVICE-INTERFACE-ELEMENT-SECURE-COM-CONFIG")
        if com_config_list is not None:
            com_config_list = to_list(com_config_list)
            for com_config in com_config_list:
                key_slot_mapping = self.model.find_elements_of_type("COM-SEC-OC-TO-CRYPTO-KEY-SLOT-MAPPING")
                for key_slot_item in key_slot_mapping:
                    com_config_ref = get_element_or_none(key_slot_item, "SERVICE-ELEMENT-SECURE-COM-CONFIG-REF")
                    if com_config_ref is not None and com_config_ref.text == com_config.get_fqn():
                        key_slot_ref = get_element_or_none(key_slot_item, "CRYPTO-KEY-SLOT-REF")
                        if key_slot_ref is not None:
                            return key_slot_ref.text[1:]
        return None

    def get_SecOC(self, si_mapping, signalBingInfo):
        for item in si_mapping['provided'] + si_mapping['required']:
            if "si_fqn" in item:
                si_fqn = item["si_fqn"]
                secOCList = []
                secOCSomeipList = []
                secOCSomeSignalList = []

                signalMapping = self.model.find_elements_of_type("SERVICE-INSTANCE-TO-SIGNAL-MAPPING")
                for signalItem in signalMapping:
                    instance_ref = get_element_or_none(signalItem, "SERVICE-INSTANCE-REF")
                    if instance_ref is not None and instance_ref.text == si_fqn:
                        signalTriggeringMapping = get_element_or_none(signalItem, "EVENT-ELEMENT-MAPPINGS/SIGNAL-BASED-EVENT-ELEMENT-TO-I-SIGNAL-TRIGGERING-MAPPING")
                        if signalTriggeringMapping is not None:
                            signalTriggeringMapping = to_list(signalTriggeringMapping)
                            for signalTriggeringItem in signalTriggeringMapping:
                                signalTriggeringRef = get_element_or_none(signalTriggeringItem, "I-SIGNAL-TRIGGERING-REF")
                                pudItem = self.get_pdu_triggering(signalTriggeringRef)
                                if pudItem is not None:
                                    secOC = {}
                                    someSignalOb = {}
                                    key_slot_ref = self.get_key_slot(si_fqn)
                                    if key_slot_ref is not None:
                                        secOC["cryptoKeySlot"] = key_slot_ref
                                    pduIdentifierList = self.model.find_elements_of_type("SO-CON-I-PDU-IDENTIFIER")
                                    for pduIdentifier in pduIdentifierList:
                                        pduTrRef = get_element_or_none(pduIdentifier, "PDU-TRIGGERING-REF")
                                        if pduTrRef is not None and pduTrRef.text == pudItem.get_fqn():
                                            headerId = get_element_or_none(pduIdentifier, "HEADER-ID")
                                            if headerId is not None:
                                                someSignalOb["pduId"] = transition_number(headerId)
                                                break

                                    iPduPortRef = get_element_or_none(pudItem, "I-PDU-PORT-REFS/I-PDU-PORT-REF")
                                    if iPduPortRef is not None:
                                        iPduPortEle = self.model.find_referable(iPduPortRef.text)
                                        useAuthDataFreshness = get_element_or_none(iPduPortEle, "USE-AUTH-DATA-FRESHNESS")
                                        if useAuthDataFreshness is not None:
                                            secOC["useAuthDataFreshness"] = useAuthDataFreshness
                                    ocCryptoMappingRef = get_element_or_none(pudItem, "SEC-OC-CRYPTO-MAPPING-REF")
                                    if ocCryptoMappingRef is not None:
                                        ocCryptoMappingEle = self.model.find_referable(ocCryptoMappingRef.text)
                                        authenticationRef = get_element_or_none(ocCryptoMappingEle, "AUTHENTICATION-REF")
                                        if authenticationRef is not None:
                                            authenticationEle = self.model.find_referable(authenticationRef.text)
                                            algorithmFamily = get_element_or_none(authenticationEle, "ALGORITHM-FAMILY")
                                            if algorithmFamily is not None:
                                                secOC["algorithmFamily"] = algorithmFamily
                                            algorithmMode = get_element_or_none(authenticationEle, "ALGORITHM-MODE")
                                            if algorithmMode is not None:
                                                secOC["algorithmMode"] = algorithmMode
                                            algorithmSecondaryFamily = get_element_or_none(authenticationEle, "ALGORITHM-SECONDARY-FAMILY")
                                            if algorithmSecondaryFamily is not None:
                                                secOC["algorithmSecondaryFamily"] = algorithmSecondaryFamily
                                    securedIPdu = None
                                    securedIPduList = self.model.find_elements_of_type("SECURED-I-PDU")
                                    for securedIPduItem in securedIPduList:
                                        payloadRef = get_element_or_none(securedIPduItem, "PAYLOAD-REF")
                                        if payloadRef is not None and pudItem.get_fqn() == payloadRef.text:
                                            securedIPdu = securedIPduItem
                                            break
                                    if securedIPdu is not None:
                                        dataId = get_element_or_none(securedIPdu, "SECURE-COMMUNICATION-PROPS/DATA-ID")
                                        if dataId is not None:
                                            secOC["dataId"] = dataId
                                            someSignalOb["dataId"] = dataId
                                        valueId = get_element_or_none(securedIPdu, "SECURE-COMMUNICATION-PROPS/FRESHNESS-VALUE-ID")
                                        if valueId is not None:
                                            secOC["freshnessId"] = valueId
                                        dataLength = get_element_or_none(securedIPdu, "SECURE-COMMUNICATION-PROPS/AUTH-DATA-FRESHNESS-LENGTH")
                                        if dataLength is not None:
                                            secOC["authDataFreshnessLength"] = dataLength
                                        startPosition = get_element_or_none(securedIPdu, "SECURE-COMMUNICATION-PROPS/AUTH-DATA-FRESHNESS-START-POSITION")
                                        if startPosition is not None:
                                            secOC["authDataFreshnessStartPosition"] = startPosition
                                        buildAttempts = get_element_or_none(securedIPdu, "SECURE-COMMUNICATION-PROPS/AUTHENTICATION-BUILD-ATTEMPTS")
                                        if buildAttempts is not None:
                                            secOC["authenticationBuildAttempts"] = buildAttempts
                                        authenticationRetries = get_element_or_none(securedIPdu, "SECURE-COMMUNICATION-PROPS/AUTHENTICATION-RETRIES")
                                        if authenticationRetries is not None:
                                            secOC["authenticationRetries"] = authenticationRetries
                                        securedAreaLength = get_element_or_none(securedIPdu, "SECURE-COMMUNICATION-PROPS/SECURED-AREA-LENGTH")
                                        if securedAreaLength is not None:
                                            secOC["securedAreaLength"] = securedAreaLength
                                        securedAreaOffset = get_element_or_none(securedIPdu, "SECURE-COMMUNICATION-PROPS/SECURED-AREA-OFFSET")
                                        if securedAreaOffset is not None:
                                            secOC["securedAreaOffset"] = securedAreaOffset
                                        useSecuredPduHeader = get_element_or_none(securedIPdu, "USE-SECURED-PDU-HEADER")
                                        if useSecuredPduHeader is not None:
                                            secOC["useSecuredPduHeader"] = useSecuredPduHeader
                                        freshness_props_ref = get_element_or_str(securedIPdu, "FRESHNESS-PROPS-REF")
                                        if freshness_props_ref != "":
                                            freshness_props_ele = self.model.find_referable(freshness_props_ref.text)
                                            freshness_length = get_element_or_none(freshness_props_ele, "FRESHNESS-VALUE-LENGTH")
                                            if freshness_length is not None:
                                                secOC["freshnessValueLength"] = freshness_length
                                            freshness_tx_length = get_element_or_none(freshness_props_ele, "FRESHNESS-VALUE-TX-LENGTH")
                                            if freshness_tx_length is not None:
                                                secOC["freshnessValueTxLength"] = freshness_tx_length
                                            category = get_element_or_none(freshness_props_ele, "CATEGORY")
                                            if category is not None:
                                                secOC["profileCategory"] = category
                                            userFT = get_element_or_none(freshness_props_ele, "USE-FRESHNESS-TIMESTAMP")
                                            if userFT is not None:
                                                secOC["useFreshnessTimestamp"] = userFT
                                        authentication_prop_ref = get_element_or_none(securedIPdu, "AUTHENTICATION-PROPS-REF")
                                        if authentication_prop_ref is not None:
                                            authentication_prop_ele = self.model.find_referable(authentication_prop_ref.text)
                                            tx_length = get_element_or_none(authentication_prop_ele, "AUTH-INFO-TX-LENGTH")
                                            if tx_length is not None:
                                                secOC["authInfoTxLength"] = tx_length
                                            category = get_element_or_none(authentication_prop_ele, "CATEGORY")
                                            if category is not None:
                                                if "profileCategory" in secOC:
                                                    profileCategory = secOC["profileCategory"]
                                                    if profileCategory != category:
                                                        assert False, "CATEGORY Inconsistent configuration.['{}','{}']".format(freshness_props_ref, authentication_prop_ref)
                                                else:
                                                    secOC["profileCategory"] = category

                                    if len(secOC) > 0 and secOC not in secOCList:
                                        secOCList.append(secOC)
                                    if len(someSignalOb) > 0 and someSignalOb not in secOCSomeSignalList:
                                        secOCSomeSignalList.append(someSignalOb)
                if "service" in item:
                    service = item["service"]
                    ar_instance_mapping = None
                    if type(service.service_deployment) in [SomeIpServiceDeployment, UserDefinedServiceDeployment]:
                        ports = service.service_deployment.ports
                        if ports is not None and "ar_instance_mapping" in ports:
                            instance_machine_mapping_fqn = ports["ar_instance_mapping"]
                            ar_instance_mapping = self.model.find_referable(instance_machine_mapping_fqn)
                    instance_ele = self.model.find_referable(si_fqn)
                    com_config_list = get_element_or_none(instance_ele, "SECURE-COM-CONFIGS/SERVICE-INTERFACE-ELEMENT-SECURE-COM-CONFIG")
                    if com_config_list is not None:
                        com_config_list = to_list(com_config_list)
                        for com_config in com_config_list:
                            secOC = {}
                            someipOb = {
                                "serviceId": transition_number(service.service_deployment.deployment_id),
                                "instanceId": transition_number(service.service_deployment.instance_id)
                            }
                            dataId = get_element_or_none(com_config, "DATA-ID")
                            valueId = get_element_or_none(com_config, "FRESHNESS-VALUE-ID")
                            if dataId is not None:
                                secOC["dataId"] = dataId
                                someipOb["dataId"] = dataId
                            if valueId is not None:
                                secOC["freshnessId"] = valueId

                            key_slot_mapping = self.model.find_elements_of_type("COM-SEC-OC-TO-CRYPTO-KEY-SLOT-MAPPING")
                            for key_slot_item in key_slot_mapping:
                                com_config_ref = get_element_or_none(key_slot_item, "SERVICE-ELEMENT-SECURE-COM-CONFIG-REF")
                                if com_config_ref is not None and com_config_ref.text == com_config.get_fqn():
                                    key_slot_ref = get_element_or_none(key_slot_item, "CRYPTO-KEY-SLOT-REF")
                                    if key_slot_ref is not None:
                                        secOC["cryptoKeySlot"] = key_slot_ref.text[1:]
                                        break

                            refList = ["EVENT-REF", "FIELD-NOTIFIER-REF", "GETTER-CALL-REF", "METHOD-CALL-REF", "SETTER-CALL-REF", "GETTER-RETURN-REF", "METHOD-RETURN-REF", "SETTER-RETURN-REF"]
                            inputRef = None
                            refType = None
                            for ref in refList:
                                inputRef = get_element_or_none(com_config, ref)
                                if inputRef is not None:
                                    refType = ref
                                    index = refList.index(ref)
                                    if index <= 1:
                                        someipOb["type"] = 0
                                    elif index <= 4:
                                        someipOb["type"] = 1
                                    else:
                                        someipOb["type"] = 2
                                    break
                            if inputRef is not None:
                                inputDeployment = self.model.find_referable(inputRef.text)
                                protocol = None
                                if refType in ["EVENT-REF"]:
                                    protocol = get_element_or_none(inputDeployment, "TRANSPORT-PROTOCOL")
                                    eventOrMethod = get_element_or_none(inputDeployment, "EVENT-ID")
                                    if eventOrMethod is not None:
                                        someipOb["eventOrMethodId"] = transition_number(eventOrMethod) + 32768
                                elif refType in ["METHOD-CALL-REF", "METHOD-RETURN-REF"]:
                                    protocol = get_element_or_none(inputDeployment, "TRANSPORT-PROTOCOL")
                                    eventOrMethod = get_element_or_none(inputDeployment, "METHOD-ID")
                                    if eventOrMethod is not None:
                                        someipOb["eventOrMethodId"] = transition_number(eventOrMethod)
                                elif refType in ["FIELD-NOTIFIER-REF"]:
                                    protocol = get_element_or_none(inputDeployment, "NOTIFIER/TRANSPORT-PROTOCOL")
                                    eventOrMethod = get_element_or_none(inputDeployment, "NOTIFIER/EVENT-ID")
                                    if eventOrMethod is not None:
                                        someipOb["eventOrMethodId"] = transition_number(eventOrMethod) + 32768
                                elif refType in ["GETTER-CALL-REF", "GETTER-RETURN-REF"]:
                                    protocol = get_element_or_none(inputDeployment, "GET/TRANSPORT-PROTOCOL")
                                    eventOrMethod = get_element_or_none(inputDeployment, "GET/METHOD-ID")
                                    if eventOrMethod is not None:
                                        someipOb["eventOrMethodId"] = transition_number(eventOrMethod)
                                elif refType in ["SETTER-CALL-REF", "SETTER-RETURN-REF"]:
                                    protocol = get_element_or_none(inputDeployment, "SET/TRANSPORT-PROTOCOL")
                                    eventOrMethod = get_element_or_none(inputDeployment, "SET/METHOD-ID")
                                    if eventOrMethod is not None:
                                        someipOb["eventOrMethodId"] = transition_number(eventOrMethod)
                                if protocol is not None and ar_instance_mapping is not None:
                                    if protocol == "TCP":
                                        com_prop = get_element_or_none(ar_instance_mapping, "SECURE-COM-PROPS-FOR-TCP-REFS/SECURE-COM-PROPS-FOR-TCP-REF")
                                        if com_prop is not None:
                                            tcp_prop = self.model.find_referable(com_prop.text)
                                            self.get_com_prop(tcp_prop, secOC)
                                    elif protocol == "UDP":
                                        com_prop = get_element_or_none(ar_instance_mapping, "SECURE-COM-PROPS-FOR-UDP-REFS/SECURE-COM-PROPS-FOR-UDP-REF")
                                        if com_prop is not None:
                                            udp_prop = self.model.find_referable(com_prop.text)
                                            self.get_com_prop(udp_prop, secOC)
                            if len(secOC) > 0 and secOC not in secOCList:
                                secOCList.append(secOC)
                            if len(someipOb) > 0 and someipOb not in secOCSomeipList:
                                secOCSomeipList.append(someipOb)
                if len(secOCList) > 0:
                    signalBingInfo.secOC = {
                        "secoc_profiles": secOCList
                    }
                if len(secOCSomeipList) > 0 or len(secOCSomeSignalList) > 0:
                    signalBingInfo.secOC_binding = {
                        "secoc_someip": secOCSomeipList,
                        "secoc_signal": secOCSomeSignalList
                    }

    def get_com_prop(self, prop, secOC):
        auth_algorithm = get_element_or_none(prop, "AUTH-ALGORITHM")
        if auth_algorithm is not None:
            secOC["authAlgorithm"] = auth_algorithm
        tx_length = get_element_or_none(prop, "AUTH-INFO-TX-LENGTH")
        if tx_length is not None:
            secOC["authInfoTxLength"] = tx_length
        freshness_length = get_element_or_none(prop, "FRESHNESS-VALUE-LENGTH")
        if freshness_length is not None:
            secOC["freshnessValueLength"] = freshness_length
        freshness_tx_length = get_element_or_none(prop, "FRESHNESS-VALUE-TX-LENGTH")
        if freshness_tx_length is not None:
            secOC["freshnessValueTxLength"] = freshness_tx_length
        category = get_element_or_none(prop, "CATEGORY")
        if category is not None:
            secOC["profileCategory"] = category
