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

from typing import List

class ApError(object):
    def __init__(self, fqn: str, desc: str, error_code: int) -> None:
        self._fqn = fqn
        self._desc = desc
        self._error_code = error_code
        self._domain = None

    def __str__(self):
        return "<ApError: {0} {1}>".format(self.domain, self.name)

    def __repr__(self):
        return self.__str__()

    @property
    def name(self):
        return self._fqn.split("/")[-1]

    @property
    def fqn(self):
        return self._fqn

    @property
    def description(self):
        return self._desc

    @property
    def error_code(self):
        return self._error_code

    def set_domain(self, domain):
        if (self._domain is not None) and (self._domain != domain):
            raise AttributeError("ApError belongs to two domains.")
        self._domain = domain

    @property
    def domain(self):
        return self._domain

class ApErrorDomain(object):
    def __init__(self, fqn: str, namespace: List[str], value: int) -> None:
        self._fqn = fqn
        self._namespace = namespace
        self._value = value
        self._errors = []

    def __str__(self):
        return "<ErrorDomain: {0} {1} (errors: {2})>".format(self._namespace, self.name, len(self._errors))

    def __repr__(self):
        return self.__str__()

    @property
    def name(self):
        return self._fqn.split("/")[-1]

    @property
    def fqn(self):
        return self._fqn

    def add_error(self, error: ApError):
        error.set_domain(self)
        self._errors += [error]

    @property
    def namespace(self):
        return self._namespace

    @property
    def errors(self):
        return self._errors

    @property
    def value(self):
        return self._value
