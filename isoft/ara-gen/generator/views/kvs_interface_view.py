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
from generator.views.model_to_view import ViewManipulations

from generator.intermediate_model.persistency.kvs_interface \
    import KvsInterface

class KvsInterfaceView(View):
    def __init__(self, kvs_interface: KvsInterface) -> None:
        self._kvs_interface = kvs_interface
        self.type_views = [ViewManipulations.convert_to_view_tree(t) for t in kvs_interface.referred_types]
        super().__init__(kvs_interface)

    def __lt__(self, other):
        return self.fqn < other.fqn

    def __eq__(self, other):
        if type(other) is KvsInterfaceView:
            return self.fqn == other.fqn
        return False

