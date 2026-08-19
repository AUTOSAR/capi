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
View factory class.
"""

from generator.views.service_view import ServiceView
from generator.views.someip.someip_service_view import SomeIpServiceView
from generator.views.user_defined.user_defined_service_view import UserDefinedServiceView
from generator.views.dds.dds_service_view import DdsServiceView

class ViewFactory(object):

    @staticmethod
    def create_service_view(service):
        result = None
        if service.service_deployment is not None:
            classname = service.service_deployment.__class__.__name__
            if classname == 'SomeIpServiceDeployment':
                result = SomeIpServiceView(service)
            elif classname == 'UserDefinedServiceDeployment':
                result = UserDefinedServiceView(service)
            elif classname == 'DdsServiceDeployment':
                result = DdsServiceView(service)
            else:
                assert False, "Unknown deployment type {}".format(classname)
        else:
            result = ServiceView(service)
        return result
