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

from generator.views.view import View

class IdlHeaderView(View):
    def __init__(self, includepath,includefile) -> None:
        super().__init__(None)
        self._includepath = includepath
        self._includefile = includefile

    @property
    def idl_self_include(self) -> str:
        return '"' + self._includepath + self._includefile + '"'

    @property
    def idl_self_include_fastdds(self) -> str:
        if self._includepath == "":
            if self._includefile == "dds/DdsDcpsInfrastructure.idl":
                return None
            else:
                return '"' + self._includefile + '"'

        else:
            return '"' + self._includepath + self._includefile + '"'

    @property
    def idl_converter_self_include(self):
        return None

