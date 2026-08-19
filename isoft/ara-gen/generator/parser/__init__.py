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
Parser package.
"""

AR_NAMESPACE_URI = 'http://autosar.org/schema/r4.0'
AR_NAMESPACE = '{' + AR_NAMESPACE_URI + '}'
AR_NAMESPACE_LEN = len(AR_NAMESPACE)

# ARXML files should be encoded as UTF-8
TEXT_ENCODING = 'utf-8'

# So far only SHORT-NAME is supported as split key.
SPLIT_KEY = 'SHORT-NAME'
