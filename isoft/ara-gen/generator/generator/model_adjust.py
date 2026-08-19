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
ModelAdjust class.
"""

class ModelAdjust(object):

    @staticmethod
    def is_has_phm(interfaces):
        phm_interfaces = ["phmhealthchannelrecoverynotificationinterface","phmsupervisionrecoverynotificationinterface","phmhealthchannelinterface",
                          "phmrecoveryactioninterface","phmsupervisedentityinterface"]
        for interface in interfaces:
            if interface in phm_interfaces:
                return True
        return False

    @staticmethod
    def is_has_per(interfaces):
        per_interfaces = ["persistencyfilestorageinterface",
                          "persistencykeyvaluestorageinterface"]
        for interface in interfaces:
            if interface in per_interfaces:
                return True
        return False

    @staticmethod
    def is_has_tsync(interfaces):
        tsync_interfaces = ["synchronizedtimebaseconsumerinterface",
                          "synchronizedtimebaseproviderinterface"]
        for interface in interfaces:
            if interface in tsync_interfaces:
                return True
        return False

    @staticmethod
    def is_has_diag(interfaces):
        diag_interfaces = ["diagnosticconditioninterface", "diagnosticdataelementinterface", "diagnosticdataidentifiergenericinterface",
                     "diagnosticdataidentifierinterface", "diagnosticdoipactivationlineinterface", "diagnosticdoipgroupidentificationinterface",
                     "diagnosticdoippowermodeinterface", "diagnosticdoiptriggervehicleannouncementinterface", "diagnosticdownloadinterface",
                     "diagnosticdtcinformationinterface", "diagnosticecuresetinterface", "diagnosticeventinterface",
                     "diagnosticgenericudsinterface", "diagnosticindicatorinterface", "diagnosticmonitorinterface",
                     "diagnosticoperationcycleinterface", "diagnosticroutinegenericinterface", "diagnosticroutineinterface", "diagnosticconversationinterface",
                     "diagnosticsecuritylevelinterface", "diagnosticservicevalidationinterface", "diagnosticuploadinterface", "diagnosticcomcontrolinterface", "diagnosticfiletransferinterface"]
        for interface in interfaces:
            if interface in diag_interfaces:
                    return True
        return False

    @staticmethod
    def is_has_raw(interfaces):
        raw_interfaces = ["rawdatastreamclientinterface",
                            "rawdatastreamserverinterface"]
        for interface in interfaces:
            if interface in raw_interfaces:
                return True
        return False

    @staticmethod
    def is_has_cg(component):
        for service in component.required_services:
            if service.cg_sign is not None:
                for pservice in component.provided_services:
                    if pservice.cg_sign is not None and service.cg_sign == pservice.cg_sign:
                        return True
        return False

    @staticmethod
    def is_has_crypto(interfaces):
        crypto_interfaces = ["cryptoproviderinterface", "cryptocertificateinterface", "cryptokeyslotinterface", "cryptotrustmasterinterface"]
        for interface in interfaces:
            if interface in crypto_interfaces:
                return True
        return False

    @staticmethod
    def is_has_fw(interfaces):
        for interface in interfaces:
            if interface == "firewallstateswitchinterface":
                return True
        return False

