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
import logging

from generator.common.tree_helper import get_element_or_none, get_element_or_die, transition_number, get_element_or_0, get_element_or_str, short_name
from generator.parser import commen_util
from generator.parser.cpp_impltypes import CppTypesParser


class PhmBuilder:
    def __init__(self, model, package_path_fallback):
        self._log = logging.getLogger(__name__)
        self.model = model
        self._package_path_fallback = package_path_fallback

    def get_phm_deployments(self, machine_fqn):
        phm_deployment = {
            "shortName": str(short_name(machine_fqn)).lower(),
            "supervisionMode": [],
            "contributes": []
        }
        ar_phm_contribution_mappings = self.model.find_elements_of_type('PHM-CONTRIBUTION-TO-MACHINE-MAPPING')
        for ar_phm_contribution_mapping in ar_phm_contribution_mappings:
            machine_ref = get_element_or_none(ar_phm_contribution_mapping, "MACHINE-REF")
            if machine_ref is not None and machine_ref.text == machine_fqn:
                ar_phm_contribution_refs = ar_phm_contribution_mapping.find_elements_of_type('PHM-CONTRIBUTION-REF')
                for ar_phm_contribution_ref in ar_phm_contribution_refs:
                    phm_contribution = self.get_phm_contribution(ar_phm_contribution_ref, machine_fqn)
                    if phm_contribution is not None:
                        phm_deployment["contributes"].append(phm_contribution)

        self.get_supervision_mode_infos(phm_deployment, machine_fqn)
        self.get_sm_port_mapping(phm_deployment, machine_fqn)
        return phm_deployment

    def get_sm_port_mapping(self, phm_deployment, machine_fqn):
        mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        for item in mapping:
            machine = get_element_or_none(item, "MACHINE-REF")
            if machine is not None and machine.text == machine_fqn:
                process_ref = get_element_or_die(item, "PROCESS-REF")
                ar_process = self.model.find_referable(process_ref.text)
                exe = get_element_or_none(ar_process, "EXECUTABLE-REF")
                if exe is not None and "smd" == short_name(exe.text):
                    phm_deployment["recovery_notification_to_pport_prototype_mapping"] = self.get_recovery_notification_mapping(process_ref.text)
                    break

    def get_recovery_notification_mapping(self, process_fqn):
        ar_mappings = self.model.find_elements_of_type('RECOVERY-NOTIFICATION-TO-P-PORT-PROTOTYPE-MAPPING')
        result_mapping = []
        for ar_mapping in ar_mappings:
            process_ref_mapping = get_element_or_none(ar_mapping, "PROCESS-REF")
            if process_ref_mapping is not None and process_ref_mapping.text == process_fqn:
                recovery_action = get_element_or_none(ar_mapping, "RECOVERY-ACTION-IREF")
                instance_specifier = commen_util.get_instance_specifier_port(self, ar_mapping, recovery_action)
                recovery_notify_ref = get_element_or_none(ar_mapping, "RECOVERY-NOTIFICATION-REF")
                if recovery_notify_ref:
                    health_channel_id = "/".join(str(recovery_notify_ref.text).split("/")[:-1])
                else:
                    health_channel_id = "0"
                result_mapping.append({
                    "instance_specifier": instance_specifier,
                    "healthChannelId": health_channel_id
                })
        return result_mapping


    def get_supervision_mode_infos(self, phm_deployment, machine_fqn):
        ar_processes = self.get_machine_processes(machine_fqn)
        for ar_process in ar_processes:
            ar_state_dependent_startup_configs = ar_process.find_elements_of_type('STATE-DEPENDENT-STARTUP-CONFIG')
            for ar_state_dependent_startup_config in ar_state_dependent_startup_configs:
                process_error_code = -1
                startupConfig = get_element_or_none(ar_state_dependent_startup_config, "STARTUP-CONFIG-REF")
                if startupConfig is not None:
                    ar_startup_config = self.model.find_referable(startupConfig.text)
                    if ar_startup_config is not None:
                        execution_error_ref = get_element_or_none(ar_startup_config, "EXECUTION-ERROR-REF")
                        if execution_error_ref is not None:
                            ar_process_execution_error = self.model.find_referable(execution_error_ref.text)
                            process_error_code = transition_number(get_element_or_none(ar_process_execution_error, "EXECUTION-ERROR"), defaultInt=-1)

                ar_function_group_state_irefs = ar_state_dependent_startup_config.find_elements_of_type('FUNCTION-GROUP-STATE-IREF')
                for ar_function_group_state_ref in ar_function_group_state_irefs:
                    ar_context_mode_declaration_group_prototype_ref = get_element_or_none(ar_function_group_state_ref, "CONTEXT-MODE-DECLARATION-GROUP-PROTOTYPE-REF")
                    ar_tar_mode_declaration_ref = get_element_or_none(ar_function_group_state_ref, 'TARGET-MODE-DECLARATION-REF')
                    if ar_context_mode_declaration_group_prototype_ref is not None:
                        supervision_mode = self.set_all_fg_states(ar_context_mode_declaration_group_prototype_ref, phm_deployment)
                        if ar_tar_mode_declaration_ref is not None:
                            mode_short_name = short_name(ar_tar_mode_declaration_ref.text)
                            supervisionModeInfo = None
                            for infoItem in supervision_mode["fgStates"]:
                                if infoItem["shortName"] == mode_short_name:
                                    supervisionModeInfo = infoItem
                                    break
                            if supervisionModeInfo is not None:
                                processFqn = ar_process.get_fqn()
                                processExeErrorInfo = {
                                    "processName": processFqn,
                                    "executionError": process_error_code
                                }
                                if processExeErrorInfo not in supervisionModeInfo["processes"]:
                                    supervisionModeInfo["processes"].append(processExeErrorInfo)
                                self.get_phm_supervision_by_process(processFqn, supervisionModeInfo)

    def set_all_fg_states(self, group_prototype_ref, phm_deployment):
        supervision_mode = {
            "fqn": group_prototype_ref.text,
            "fgStates": []
        }
        for item in phm_deployment["supervisionMode"]:
            if item["fqn"] == group_prototype_ref.text:
                return item
        phm_deployment["supervisionMode"].append(supervision_mode)
        ar_mode_declaration_group = self.model.find_referable(group_prototype_ref.text)
        ar_type_ref = get_element_or_none(ar_mode_declaration_group, "TYPE-TREF")
        if ar_type_ref is not None:
            ar_mode_declaration_group = self.model.find_referable(ar_type_ref.text)
            ar_mode_declarations = ar_mode_declaration_group.find_elements_of_type('MODE-DECLARATION')
            for ar_mode_declaration in ar_mode_declarations:
                shortname = get_element_or_die(ar_mode_declaration, "SHORT-NAME").text
                supervision_mode["fgStates"].append({
                    "shortName": shortname,
                    "phmSupervision": [],
                    "processes": []
                })
        return supervision_mode

    def get_phm_supervision_by_process(self, processFqn, supervisionModeInfo):
        sup_checkpoint = []
        ar_phm_checkpoint_refs = self.model.find_elements_of_type('SUPERVISION-CHECKPOINT')
        for ar_phm_checkpoint_ref in ar_phm_checkpoint_refs:
            process = get_element_or_die(ar_phm_checkpoint_ref, "PROCESS-REF").text
            if process == processFqn:
                sup_checkpoint.append(ar_phm_checkpoint_ref.get_fqn())
        result_list = supervisionModeInfo["phmSupervision"]
        ar_phm_alive_supervisions = self.model.find_elements_of_type('ALIVE-SUPERVISION')
        for ar_phm_alive_supervision in ar_phm_alive_supervisions:
            shortname = ar_phm_alive_supervision.get_fqn()
            checkpoint_ref = get_element_or_die(ar_phm_alive_supervision, "CHECKPOINT-REF")
            if checkpoint_ref.text in sup_checkpoint and shortname not in result_list:
                supervisionModeInfo["phmSupervision"].append(shortname)
        ar_phm_logical_supervisions = self.model.find_elements_of_type('LOGICAL-SUPERVISION')
        for ar_phm_logical_supervision in ar_phm_logical_supervisions:
            shortname = ar_phm_logical_supervision.get_fqn()
            ar_phm_initial_checkpoint_refs = ar_phm_logical_supervision.find_elements_of_type('INITIAL-CHECKPOINT-REF')
            for ar_phm_initial_checkpoint_ref in ar_phm_initial_checkpoint_refs:
                if ar_phm_initial_checkpoint_ref.text in sup_checkpoint and shortname not in result_list:
                    supervisionModeInfo["phmSupervision"].append(shortname)
            ar_phm_final_checkpoint_refs = ar_phm_logical_supervision.find_elements_of_type('FINAL-CHECKPOINT-REF')
            for ar_phm_final_checkpoint_ref in ar_phm_final_checkpoint_refs:
                if ar_phm_final_checkpoint_ref.text in sup_checkpoint and shortname not in result_list:
                    supervisionModeInfo["phmSupervision"].append(shortname)
            ar_phm_transition_refs = ar_phm_logical_supervision.find_elements_of_type('TRANSITION-REF')
            for ar_phm_transition_ref in ar_phm_transition_refs:
                self.set_transition(shortname, ar_phm_transition_ref, sup_checkpoint, supervisionModeInfo)
        ar_phm_deadlined_supervisions = self.model.find_elements_of_type('DEADLINE-SUPERVISION')
        for ar_phm_deadlined_supervision in ar_phm_deadlined_supervisions:
            shortname = ar_phm_deadlined_supervision.get_fqn()
            transition_ref = get_element_or_die(ar_phm_deadlined_supervision, "CHECKPOINT-TRANSITION-REF")
            self.set_transition(shortname, transition_ref, sup_checkpoint, supervisionModeInfo)

    def set_transition(self, shortname, transition_ref, sup_checkpoint, supervisionModeInfo):
        ar_phm_transition = self.model.find_referable(transition_ref.text)
        ar_phm_transitions_source = get_element_or_die(ar_phm_transition, "SOURCE-REF")
        if ar_phm_transitions_source.text in sup_checkpoint and shortname not in supervisionModeInfo["phmSupervision"]:
            supervisionModeInfo["phmSupervision"].append(shortname)
        ar_phm_transitions_target = get_element_or_die(ar_phm_transition, "TARGET-REF")
        if ar_phm_transitions_target.text in sup_checkpoint and shortname not in supervisionModeInfo["phmSupervision"]:
            supervisionModeInfo["phmSupervision"].append(shortname)

    def get_phm_contribution(self, ar_phm_contribution_ref, machine_fqn):

        ar_phm_contribution = self.model.find_referable(ar_phm_contribution_ref.text)
        shortname = get_element_or_die(ar_phm_contribution, "SHORT-NAME")
        phm_contribution = {
            "shortName": shortname,
            "globalSupervision": [],
            "healthChannelSupervision": [],
            "healthChannelExternalStatus": [],
            "localSupervision": [],
            "aliveSupervision": [],
            "deadlineSupervision": [],
            "logicalSupervision": [],
            "supervisionCheckpoint": [],
            "healthChannelPoint": []
        }
        ar_phm_checkpoint_refs = ar_phm_contribution.find_elements_of_type('SUPERVISION-CHECKPOINT')
        for ar_phm_checkpoint_ref in ar_phm_checkpoint_refs:
            checkpoint_short_name = get_element_or_none(ar_phm_checkpoint_ref, "SHORT-NAME")
            checkpoint_ref = get_element_or_none(ar_phm_checkpoint_ref, "PHM-CHECKPOINT-IREF/TARGET-PHM-CHECKPOINT-REF")
            checkpoint_id = 0
            status = ""
            if checkpoint_ref is not None:
                ar_checkpoint = self.model.find_referable(checkpoint_ref.text)
                checkpoint_id = transition_number(get_element_or_0(ar_checkpoint, "CHECKPOINT-ID"))
            status_ref = get_element_or_none(ar_phm_checkpoint_ref, "PHM-CHECKPOINT-IREF/TARGET-PHM-CHECKPOINT-REF")
            if status_ref is not None:
                status = status_ref.text.split("/")[-1]

            identifier = self.get_checkpoint_identifier(ar_phm_checkpoint_ref)
            process_name = get_element_or_str(ar_phm_checkpoint_ref, "PROCESS-REF")
            machineFqn = commen_util.find_machine_fqn_by_process(self, process_name)
            if machineFqn == machine_fqn:
                phm_contribution["supervisionCheckpoint"].append({
                    "shortName": checkpoint_short_name,
                    "fqn": ar_phm_checkpoint_ref.get_fqn(),
                    "processName": process_name,
                    "status": status,
                    "checkpointId": checkpoint_id,
                    "IdentifierR": identifier
                })
            else:
                assert False, "[PHM ] CODE-006: PhmContribution.SupervisionCheckpoint.processRef does not belong to {}. FQN:{}. LocalPath:{}.".format(machine_fqn, ar_phm_checkpoint_ref.get_fqn(), ar_phm_checkpoint_ref.get_path())
        quoteLocalSupervision = {}
        ar_phm_global_supervisions = ar_phm_contribution.find_elements_of_type('GLOBAL-SUPERVISION')
        for ar_phm_global_supervision in ar_phm_global_supervisions:
            global_supervision = self.get_global_supervision(ar_phm_global_supervision, quoteLocalSupervision)
            phm_contribution["globalSupervision"].append(global_supervision)

        self.get_health_channel_supervisions(ar_phm_contribution, phm_contribution)
        self.get_health_channel_external_status(ar_phm_contribution, phm_contribution, machine_fqn)
        ar_phm_local_supervisions = ar_phm_contribution.find_elements_of_type('LOCAL-SUPERVISION')
        for ar_phm_local_supervision in ar_phm_local_supervisions:
            self.get_local_supervision(ar_phm_local_supervision, phm_contribution)
        return phm_contribution

    def get_health_channel_supervisions(self, ar_phm_contribution, phm_contribution):
        ar_health_channel_supervisions = ar_phm_contribution.find_elements_of_type("HEALTH-CHANNEL-SUPERVISION")
        for ar_health_channel_supervision in ar_health_channel_supervisions:
            shortname = get_element_or_die(ar_health_channel_supervision, "SHORT-NAME")
            fqn = ar_health_channel_supervision.get_fqn()
            supervision = get_element_or_str(ar_health_channel_supervision, "SUPERVISION-REF")
            ar_recovery_notification = get_element_or_none(ar_health_channel_supervision, "RECOVERY-NOTIFICATION")
            if ar_recovery_notification is not None:
                # recovery_shortname = get_element_or_none(ar_recovery_notification, "SHORT-NAME")
                recoveryNotificationRetry = transition_number(get_element_or_none(ar_recovery_notification, "RECOVERY-NOTIFICATION-RETRY"), defaultInt=0)
                recoveryNotificationTimeout = get_element_or_0(ar_recovery_notification, "RECOVERY-NOTIFICATION-TIMEOUT")
                phm_contribution["healthChannelSupervision"].append({
                    "shortName": shortname,
                    "metaModelIdentifier": fqn,
                    "supervision": supervision,
                    "recoveryNotification": {
                        "recoveryNotificationRetry": recoveryNotificationRetry,
                        "recoveryNotificationTimeout": recoveryNotificationTimeout
                    }
                })

    def get_health_channel_external_status(self, ar_phm_contribution, phm_contribution, machine_fqn):
        ar_health_channel_status_map = ar_phm_contribution.find_elements_of_type("HEALTH-CHANNEL-EXTERNAL-STATUS")
        for ar_health_channel_status in ar_health_channel_status_map:
            shortname = get_element_or_none(ar_health_channel_status, "SHORT-NAME")
            fqn = ar_health_channel_status.get_fqn()
            processName = get_element_or_str(ar_health_channel_status, "PROCESS-REF")
            machineFqn = commen_util.find_machine_fqn_by_process(self, processName)
            if machineFqn == machine_fqn:
                healthChannelPoint = []
                ar_health_channel_points = ar_health_channel_status.find_elements_of_type("HEALTH-CHANNEL-EXTERNAL-REPORTED-STATUS")
                for ar_health_channel_point in ar_health_channel_points:
                    status_ref = get_element_or_none(ar_health_channel_point, "STATUS-REF")
                    if status_ref is not None:
                        healthChannelPoint.append(status_ref)
                        phm_health_channel_interface_ref = "/".join(status_ref.text.split("/")[0:-1])
                        ar_phm_channel_interface = self.model.find_referable(phm_health_channel_interface_ref)
                        interface_name = get_element_or_die(ar_phm_channel_interface, "SHORT-NAME")
                        isHave = False
                        for item in phm_contribution["healthChannelPoint"]:
                            if item["shortName"] == interface_name:
                                isHave = True
                                break
                        if not isHave:
                            healthChannelPointE = {
                                "shortName": interface_name,
                                "healthStatus": []
                            }
                            ar_phm_channel_statuses = ar_phm_channel_interface.find_elements_of_type('PHM-HEALTH-CHANNEL-STATUS')
                            for ar_phm_channel_status in ar_phm_channel_statuses:
                                status_shortname = get_element_or_die(ar_phm_channel_status, "SHORT-NAME")
                                status_id = transition_number(get_element_or_0(ar_phm_channel_status, "STATUS-ID"))
                                triggersRecoveryNotification = get_element_or_none(ar_phm_channel_status, "TRIGGERS-RECOVERY-NOTIFICATION")
                                if triggersRecoveryNotification is None:
                                    triggersRecoveryNotification = False
                                healthChannelPointE["healthStatus"].append({
                                    "shortName": status_shortname,
                                    "statusId": status_id,
                                    "triggersRecoveryNotification": triggersRecoveryNotification
                                })
                            phm_contribution["healthChannelPoint"].append(healthChannelPointE)


                phmHealthChannelInterface = get_element_or_none(ar_health_channel_status, "HEALTH-CHANNEL-IREF/TARGET-HEALTH-CHANNEL-REF")
                phmHealthChannelInterfaceName = ""
                if phmHealthChannelInterface is not None:
                    phmHealthChannelInterfaceName = phmHealthChannelInterface.text.split("/")[-1]
                ar_health_channel = get_element_or_none(ar_health_channel_status, "HEALTH-CHANNEL-IREF")
                identifierR = commen_util.get_instance_specifier_port(self, ar_health_channel_status, ar_health_channel)
                ar_recovery_notification = get_element_or_none(ar_health_channel_status, "RECOVERY-NOTIFICATION")
                if ar_recovery_notification is not None:
                    recovery_shortname = get_element_or_none(ar_recovery_notification, "SHORT-NAME")
                    recoveryNotificationRetry = transition_number(get_element_or_none(ar_recovery_notification, "RECOVERY-NOTIFICATION-RETRY"), defaultInt=0)
                    recoveryNotificationTimeout = get_element_or_0(ar_recovery_notification, "RECOVERY-NOTIFICATION-TIMEOUT")
                    phm_contribution["healthChannelExternalStatus"].append({
                        "shortName": shortname,
                        "metaModelIdentifier": fqn,
                        "IdentifierR": identifierR,
                        "phmHealthChannelInterface": phmHealthChannelInterfaceName,
                        "healthChannelPoint": healthChannelPoint,
                        "recoveryNotification": {
                            "recoveryNotificationRetry": recoveryNotificationRetry,
                            "recoveryNotificationTimeout": recoveryNotificationTimeout
                        },
                        "processName": processName
                    })
            else:
                assert False, "[PHM ] CODE-007: PhmContribution.HealthChannelExternalStatus.processRef does not belong to {}. FQN:{}. LocalPath:{}.".format(machine_fqn, ar_health_channel_status.get_fqn(), ar_health_channel_status.get_path())

    def get_global_supervision(self, ar_phm_global_supervision, quoteLocalSupervision):
        fqn = ar_phm_global_supervision.get_fqn()
        expired_supervision_cycles_tolerance = transition_number(get_element_or_none(ar_phm_global_supervision, "EXPIRED-SUPERVISION-CYCLES-TOLERANCE"), defaultInt=0)
        supervision_cycle = transition_number(get_element_or_die(ar_phm_global_supervision, 'SUPERVISION-CYCLE'))
        global_supervision = {
            "shortName": fqn,
            "supervisionCycle": supervision_cycle,
            "expiredSupervisonCycleTolerance": expired_supervision_cycles_tolerance,
            "localSupervision": []
        }
        phm_local_supervisions = ar_phm_global_supervision.find_elements_of_type('LOCAL-SUPERVISION-REF')
        for phm_local_supervision in phm_local_supervisions:
            assert phm_local_supervision.text not in quoteLocalSupervision, "[PHM ] CODE-003: A LocalSupervision[{}] can only be referenced by a GlobalSupervision. But it is currently quoted by [{}, {}]. ".format(phm_local_supervision.text, fqn, quoteLocalSupervision[phm_local_supervision.text])
            quoteLocalSupervision[phm_local_supervision.text] = fqn
            global_supervision["localSupervision"].append(phm_local_supervision.text)
        return global_supervision

    def get_local_supervision(self, ar_phm_local_supervision, phm_contribution):
        shortname = get_element_or_die(ar_phm_local_supervision, "SHORT-NAME")
        failed_supervision_cycles_tolerance = transition_number(get_element_or_none(ar_phm_local_supervision, "FAILED-SUPERVISION-CYCLES-TOLERANCE"), defaultInt=0)
        local_supervision = {
            "shortName": shortname,
            "fqn": ar_phm_local_supervision.get_fqn(),
            "failedSupervisionCyclesTolerance": failed_supervision_cycles_tolerance,
            "aliveSupervision": [],
            "deadlineSupervision": [],
            "logicalSupervision": []
        }
        ar_phm_alive_supervisions = ar_phm_local_supervision.find_elements_of_type('ALIVE-SUPERVISION')
        for ar_phm_alive_supervision in ar_phm_alive_supervisions:
            local_supervision["aliveSupervision"].append(ar_phm_alive_supervision.get_fqn())
            phm_contribution["aliveSupervision"].append(self.get_phm_alive_supervision(ar_phm_alive_supervision))
        ar_phm_logical_supervisions = ar_phm_local_supervision.find_elements_of_type('LOGICAL-SUPERVISION')
        for ar_phm_logical_supervision in ar_phm_logical_supervisions:
            local_supervision["logicalSupervision"].append(ar_phm_logical_supervision.get_fqn())
            phm_contribution["logicalSupervision"].append(self.get_phm_logical_supervision(ar_phm_logical_supervision))
        ar_phm_deadlined_supervisions = ar_phm_local_supervision.find_elements_of_type('DEADLINE-SUPERVISION')
        for ar_phm_deadlined_supervision in ar_phm_deadlined_supervisions:
            local_supervision["deadlineSupervision"].append(ar_phm_deadlined_supervision.get_fqn())
            phm_contribution["deadlineSupervision"].append(self.get_phm_deadline_supervision(ar_phm_deadlined_supervision))

        phm_contribution["localSupervision"].append(local_supervision)

    def get_phm_alive_supervision(self, ar_phm_alive_supervision):
        shortname = get_element_or_die(ar_phm_alive_supervision, "SHORT-NAME")
        alive_reference_cycle = get_element_or_die(ar_phm_alive_supervision, "ALIVE-REFERENCE-CYCLE")
        expected_alive_indications = transition_number(get_element_or_die(ar_phm_alive_supervision, "EXPECTED-ALIVE-INDICATIONS"))
        max_margin = transition_number(get_element_or_die(ar_phm_alive_supervision, "MAX-MARGIN"))
        min_margin = transition_number(get_element_or_die(ar_phm_alive_supervision, "MIN-MARGIN"))
        checkpoint_ref = get_element_or_die(ar_phm_alive_supervision, "CHECKPOINT-REF")
        phm_alive_supervision = {
                    "shortName": shortname,
                    "fqn": ar_phm_alive_supervision.get_fqn(),
                    "checkpointFqn": checkpoint_ref.text,
                    "expectedAliveIndications": expected_alive_indications,
                    "maxMargin": max_margin,
                    "minMargin": min_margin,
                    "aliveRefrenceCycle": alive_reference_cycle
                }
        return phm_alive_supervision

    def get_phm_logical_supervision(self, ar_phm_logical_supervision):
        shortname = get_element_or_die(ar_phm_logical_supervision, "SHORT-NAME")
        phm_logicalsupervision = {
            "shortName": shortname,
            "fqn": ar_phm_logical_supervision.get_fqn(),
            "initialCheckpoint": [],
            "finalCheckpoint": [],
            "transition": []
        }
        ar_phm_initial_checkpoint_refs = ar_phm_logical_supervision.find_elements_of_type('INITIAL-CHECKPOINT-REF')
        for ar_phm_initial_checkpoint_ref in ar_phm_initial_checkpoint_refs:
            phm_logicalsupervision["initialCheckpoint"].append(ar_phm_initial_checkpoint_ref.text)

        ar_phm_final_checkpoint_refs = ar_phm_logical_supervision.find_elements_of_type('FINAL-CHECKPOINT-REF')
        for ar_phm_final_checkpoint_ref in ar_phm_final_checkpoint_refs:
            phm_logicalsupervision["finalCheckpoint"].append(ar_phm_final_checkpoint_ref.text)

        ar_phm_transition_refs = ar_phm_logical_supervision.find_elements_of_type('TRANSITION-REF')
        for ar_phm_transition_ref in ar_phm_transition_refs:
            transition = self.get_transition(ar_phm_transition_ref)
            phm_logicalsupervision["transition"].append(transition)

        return phm_logicalsupervision

    def get_phm_deadline_supervision(self, ar_phm_deadlined_supervision):
        shortname = get_element_or_die(ar_phm_deadlined_supervision, "SHORT-NAME")
        max_deadline = get_element_or_die(ar_phm_deadlined_supervision, "MAX-DEADLINE")
        min_deadline = get_element_or_die(ar_phm_deadlined_supervision, "MIN-DEADLINE")
        phm_deadline_supervision = {
            "shortName": shortname,
            "fqn": ar_phm_deadlined_supervision.get_fqn(),
            "checkpointTransition": [],
            "maxDeadline": max_deadline,
            "minDeadline": min_deadline
        }
        ar_phm_transition_ref = get_element_or_die(ar_phm_deadlined_supervision, "CHECKPOINT-TRANSITION-REF")
        transition = self.get_transition(ar_phm_transition_ref)
        phm_deadline_supervision["checkpointTransition"].extend([transition["sourceCheckpoint"], transition["targetCheckpoint"]])
        return phm_deadline_supervision

    def get_transition(self, ar_phm_transition_ref):
        ar_phm_transition = self.model.find_referable(ar_phm_transition_ref.text)
        transition_short_name = get_element_or_die(ar_phm_transition, "SHORT-NAME")
        ar_phm_transitions_source = get_element_or_die(ar_phm_transition, "SOURCE-REF").text
        ar_phm_transitions_target = get_element_or_die(ar_phm_transition, "TARGET-REF").text
        transition = {
            "shortName": transition_short_name,
            "sourceCheckpoint": ar_phm_transitions_source,
            "targetCheckpoint": ar_phm_transitions_target
        }
        return transition

    def get_checkpoint_identifier(self, ar_phm_checkpoint_ref):
        ar_phm_checkpoint = get_element_or_none(ar_phm_checkpoint_ref, "PHM-CHECKPOINT-IREF")
        return commen_util.get_instance_specifier_port(self, ar_phm_checkpoint_ref, ar_phm_checkpoint)

    def get_machine_processes(self, machineFqn):
        ar_processes = []
        proc_to_machine_mappings = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        for mapping in proc_to_machine_mappings:
            ar_machine_ref = get_element_or_none(mapping, "MACHINE-REF")
            if ar_machine_ref is not None and ar_machine_ref.text == machineFqn:
                ar_process = self.model.find_referable(mapping.PROCESS_REF)
                ar_processes.append(ar_process)
        return ar_processes

    def populate_interface(self, ar_interface, service):
        service.namespaces = self._get_service_namespaces(ar_interface)
        service.lowNamespaces =  [x.lower() for x in service.namespaces]

    def _get_service_namespaces(self, ar_interface):
        log = logging.getLogger(__name__)
        if hasattr(ar_interface, 'NAMESPACES'):
            namespaces = ar_interface.NAMESPACES
            if namespaces is not None:
                return CppTypesParser.parse_namespaces(namespaces)
        else:
            if self._package_path_fallback:
                log.warning("No namespaces were found in the following interface %s! Fallback to Packages hierarchy", ar_interface.SHORT_NAME)
                return ar_interface.get_parent_package_hierarchy_as_list()
            else:
                log.warning("No namespaces were found in the following interface %s!", ar_interface.SHORT_NAME)
                return []

    def get_phmhealthchannel_identifier(self, ar_health_channel_Status):
        ar_health_channel = get_element_or_none(ar_health_channel_Status, "HEALTH-CHANNEL-IREF")
        return commen_util.get_instance_specifier_port(self, ar_health_channel_Status, ar_health_channel)
