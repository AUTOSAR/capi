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

from generator.views.service_view import ServiceView

class ServiceInstanceView(ServiceView):
    def __init__(self, service) -> None:
        super().__init__(service)

    def __eq__(self, other):
        return self._internal_id == other._internal_id

    def __lt__(self, other):
        return self._internal_id < other._internal_id

    def __hash__(self):
        return hash(self._internal_id)

    def __str__(self):
        return "ServiceInstanceView({})".format(self._internal_id)

    def __repr__(self):
        return self.__str__()


    @property
    def _internal_id(self):
        return "{}+{}".format(self.fqn, self.instance_id)


    @property
    def instance_id(self):
        assert False, "instance_id getter must be implemented in derived class"

