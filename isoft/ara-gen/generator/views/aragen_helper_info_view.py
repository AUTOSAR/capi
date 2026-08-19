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



class AragenHelperInfoView():


    def __init__(self) -> None:
        self._ARA_GEN_ARA_LIBS = "ara::core ara::exec::execution_client"
        self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE = ""
        self._hascomservice = False
        self._isnsomeip = False
        self._isfastdds = False
        self._isDiag = False
        self._isNPC = False
        self._isIcc = False
        self._isSaw = False
        self._isCg = False
        self._findpakage =[]
        super().__init__()

    @property
    def ARA_GEN_ARA_LIBS(self):
        return self._ARA_GEN_ARA_LIBS

    def add_ARA_GEN_ARA_LIB(self,ARA_GEN_ARA_LIB):
        self._ARA_GEN_ARA_LIBS = self._ARA_GEN_ARA_LIBS + " " + ARA_GEN_ARA_LIB

    @property
    def ARA_GEN_ARA_LIBS_LOOP_REFERENCE(self):
        return self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE

    def add_ARA_GEN_ARA_LIBS_LOOP_REFERENCE(self, ARA):
        if ARA in self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE:
            return
        self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE = self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE + " " + ARA

    @property
    def hascomservice(self):
        return self._hascomservice

    def set_hascomservice(self,hascomservice):
        self._hascomservice = hascomservice
        if "ara_com" in self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE:
            return
        self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE = self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE + "ara_com"


    @property
    def isnsomeip(self):
        return self._isnsomeip

    def set_isnsomeip(self,isnsomeip):
        if " ara_com_nsomeip" in self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE:
            return
        self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE = self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE + " ara_com_nsomeip"
        self._isnsomeip = isnsomeip

    @property
    def isNPC(self):
        return self._isNPC

    def set_isNPC(self, isNPC):
        if " ara_com_npc" in self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE:
            return
        self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE = self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE + " ara_com_npc"
        self._isNPC = isNPC

    @property
    def isIcc(self):
        return self._isIcc

    def set_isIcc(self, isIcc):
        if " ara_com_icc" in self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE:
            return
        self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE = self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE + " ara_com_icc"
        self._isIcc = isIcc

    @property
    def isfastdds(self):
        return self._isfastdds

    def set_isfastdds(self, isfastdds):
        if " ara_com_fastdds" in self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE:
            return
        self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE = self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE + " ara_com_fastdds"
        self._isfastdds = isfastdds


    @property
    def findpakage(self):
        return self._findpakage

    def set_issaw(self,issaw):
        if " ara_com" in self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE:
            self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE = self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE + " ara_com_raw"
        else:
            self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE = self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE + " ara_com"
            self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE = self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE + " ara_com_raw"

        self._isSaw = issaw

    def set_cg(self, isCg):
        if " ara_com_cg" in self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE:
            return
        self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE = self._ARA_GEN_ARA_LIBS_LOOP_REFERENCE + " ara_com_cg"
        self._isCg = isCg

    @property
    def isCg(self):
        return self._isCg
    @property
    def issaw(self):
        return self._isSaw
