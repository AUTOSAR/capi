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


Package Source Cleanup Scripts
==============================

This directory contains package-specific scripts that remove selected bundled
content from unpacked third-party source archives before configuration and
compilation.

Purpose
-------

Some upstream source archives contain optional examples, benchmarks, bindings,
or bundled third-party components that are not used by CAPI but can still
trigger license findings.

Removing such content before configuration and compilation makes the intended
exclusion explicit and allows the regular package build to verify that the
removed content is not required.

Design
------

Each package has a dedicated cleanup script, for example:

* ``cleanup-zlib-sources.sh``
* ``cleanup-eigen-sources.sh``

The package-specific scripts define:

* how the expected package source root is identified;
* which bundled paths are removed;
* any additional package-specific checks, such as a version check.

Shared logging, validation, removal, and verification behavior is implemented
in ``cleanup-common.sh``.

The common helper is sourced by the package-specific scripts and is not
intended to be executed directly.

Cleanup paths are defined directly in the respective package-specific script.
They are not accepted as command-line arguments.

Before a path is removed:

* the package source root is verified;
* the configured cleanup path must exist;
* unsafe paths are rejected.

After removal, the script verifies that the path no longer exists. Any
unexpected condition stops the build.

Usage
-----

The package-specific cleanup script must be executed from the root of the
unpacked package source tree before the package's normal configuration command.

Example package configuration:

.. code-block:: json

    {
        "name": "foo",
        "version": "1.2.3",
        "preproc": "${CAPI_SRC_DIR}/isoft/sdk-utils/etc/package/package-source-cleanup/cleanup-foo-sources.sh",
        "configure": "<OTHER_COMMANDS>",
        "build": "..."
    }

The package-specific scripts must be executable:

.. code-block:: shell

    chmod +x "${CAPI_SRC_DIR}"/isoft/sdk-utils/etc/package/package-source-cleanup/cleanup-*-sources.sh

``cleanup-common.sh`` only needs to be readable because it is sourced by the
package-specific scripts.

Adding Another Package
----------------------

For another package:

#. Create a dedicated ``cleanup-<package>-sources.sh`` script.
#. Source ``cleanup-common.sh``.
#. Define checks that identify the expected package source root.
#. Define the bundled paths to remove directly in the script.
#. Do not accept cleanup paths as command-line arguments.
#. Add a version check only when a reliable version source is available.
#. Verify the cleanup with a clean package build.

Do not add package-specific paths or package-specific source-tree checks to
``cleanup-common.sh``.
