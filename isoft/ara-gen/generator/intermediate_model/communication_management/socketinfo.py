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


class SocketInfo(object):

    def __init__(self):

        self._socketpro = ""
        self._localaddress = ""
        self._localport = ""
        self._remoteaddress = ""
        self._remoteport = ""
    @property
    def socketpro(self):
        return self._socketpro
    def set_socketpro(self,socketpro):
        self._socketpro = socketpro

    @property
    def localaddress(self):
        return self._localaddress

    def set_localaddress(self,localaddress):
        self._localaddress = localaddress

    @property
    def localport(self):
        return self._localport

    def set_localport(self, localport):
        self._localport = localport

    @property
    def remoteaddress(self):
        return self._remoteaddress

    def set_remoteaddress(self, remoteaddress):
        self._remoteaddress = remoteaddress

    @property
    def remoteport(self):
        return self._remoteport

    def set_remoteport(self, remoteport):
        self._remoteport = remoteport



