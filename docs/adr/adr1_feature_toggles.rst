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

ADR 1: Usage of Feature Toggles
===============================

Context
-------

Terminology:
In this ADR, "feature" denotes some functionality of CAPI that can sensibly be developed and used independently.
"Feature" is not related to the AUTOSAR feature graph or document.

Long-running feature branches have several disadvantages:

* The ongoing work is not directly visible to developers on other branches.
* Conflicting changes are likely to go unnoticed until merging the branch.
* Merge conflicts are likely and hard to resolve.

These disadvantages are resolved by continuous integration into the main development branch.
Feature toggles allow this while still being able to switch off unfinished features.

SC decided that CAPI releases shall not contain features that are not yet part of a release of the AUTOSAR standard.
Nevertheless, AUTOSAR partners should have the possibility to use such features.
This is possible with feature toggles, which can be switched on by interested partners.

A disadvantage of feature toggles is that they require strict management to keep the code understandable.

Decision
--------

We will use feature toggles to allow continuous integration while isolating unfinished changes.
Accordingly, we encourage all contributors to avoid long-running feature branches.
We encourage them to ask for a feature toggle and create pull-requests often.
Exceptions for special occasions are possible, for example, for large cross-cutting changes.

Feature toggles default to off until the feature is completely implemented and part of a release of the AUTOSAR standard.
A feature toggle is removed if a feature is part of a CAPI release, i.e., if the toggle defaulted to true in a release.
A feature toggle and the enclosed source code are removed when work on a feature is cancelled.

Status
------

Accepted

Consequences
------------

SDE, SDI and the maintainers need to manage the feature toggles to avoid chaos.
Pull requests require a check that ensures that all unfinished work is completely deactivated with the respective feature toggle.

CAPI releases contain source code and no compiled binaries.
The SC requests to exclude source code of unfinished features and of features that are not yet part of a release of the AUTOSAR standard.
Thus, we need a solution for automatic removal of the source code that is switched of via feature toggles.