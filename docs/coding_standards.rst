..
    Disclaimer

    This work (specification and/or software implementation) and the material
    contained in it, as released by AUTOSAR, is for the purpose of information
    only. AUTOSAR and the companies that have contributed to it shall not be
    liable for any use of the work.

    The material contained in this work is protected by copyright and other
    types of intellectual property rights. The commercial exploitation of the
    material contained in this work requires a license to such intellectual
    property rights.

    This work may be utilized or reproduced without any modification, in any
    form or by any means, for informational purposes only. For any other
    purpose, no part of the work may be utilized or reproduced, in any form
    or by any means, without permission in writing from the publisher.

    The work has been developed for automotive applications only. It has
    neither been developed, nor tested for non-automotive applications.

    The word AUTOSAR and the AUTOSAR logo are registered trademarks.
    --------------------------------------------------------------------------

=====================
CAPI Coding Standards
=====================

Preface
=======

This document describes the ruleset that the CAPI community has given itself.
If you find a rule that should be discussed or think something is missing, open an improvement or bug ticket against this document.
The document is maintained by SDE <sde@autosar.org>.

Included Standards
==================

MISRA C++:2023
--------------

Adhere to the MISRA C++:2023 [2023misra] rules with the exceptions described in the following subsections.
In case of conflict, the MISRA C++ rules take precedence over any other rules.
Nevertheless, please report any conflicts to SDE <sde@autosar.org> so that they can be considered in the next version of this document.

Exceptions will be added here
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

C++ Core Guidelines
-------------------

Adhere to the C++ Core Guidelines [stroustrupcore] with the exceptions described in the following subsections.
In case of conflict, the C++ Core Guidelines take precedence over Section 1.3.
Nevertheless, please report any conflicts to SDE <sde@autosar.org> so that they can be considered in the next version of this document.

Exceptions will be added here
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

C++ Coding Standards
--------------------

Adhere to the C++ Coding Standards [sutter2004coding] with the exceptions described in the following subsections.
If you find a conflict with other rules in this document, please inform SDE <sde@autosar.org> so that it can be considered in the next version of this document.

Don't worry that this book is from 2004.
The rules that are not explicitly excluded are still valid.

Exceptions will be added here
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Design Style
============



Coding Style
============

Adhere to a consistent style
----------------------------

Be consistent regarding indentation, line length, naming and comment style in each source file.

Consider being consistent in all files of the project.
Nevertheless, if a source file uses a different style than other source files, consistency inside the file is most important.

Never use underhand names
-------------------------

Never user names that start with an underscore or contain a double underscore.

Comments
--------

Describe the detailed design in a Doxygen comment for the related unit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use the conventions in `Source Code Documentation Style <source_code_documentation_style.rst>`_ when writing the detailed-design comments.

Rationale: The detailed-design document is automatically generated from these comments.
The automation is built on theses conventions.

References
==========

.. [stroustrupcore] B. Stroustrup, H. Sutter (eds.). C++ Core Guidelines. https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines
.. [sutter2004coding] H. Sutter, A. Alexandrescu. C++ Coding Standards: 101 Rules, Guidelines, and Best Practices. Addison-Wesley Professional, 2004
.. [2023misra] MISRA C++:2023: Guidelines for the use of C++17 in critical systems. MISRA, 2023