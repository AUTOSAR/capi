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
SoftwarePackageView corresponding to a Process IM object.
"""
import hashlib
from generator.intermediate_model.software_package.software_package \
    import SoftwarePackage
from generator.views.view import View
from generator.views.view_factory import ViewFactory

class SoftwarePackageView(View):
    """Specialized View of a SoftwarePackage."""

    def __init__(self, softwarePackage: SoftwarePackage) -> None:
        self._softwarePackage = softwarePackage
        name = self._softwarePackage.name
        hash_algo = hashlib.new('sha1')
        hash_algo.update(name.encode('UTF-8'))
        digest = hash_algo.hexdigest()
        # here 2 bytes are used therefore 4 digits are consumed
        self._id = int(digest[0:4], base=16)

        super().__init__(softwarePackage)

    @property
    def id(self):
        """Get the machine in the view"""
        return hex(self._id)
