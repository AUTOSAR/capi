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

============
AUTOSAR CAPI
============

AUTOSAR CAPI (AUTOSAR Common Adaptive Platform Implementation)
is a community-source implementation of the **AUTOSAR Adaptive Platform** standard.

The project's objectives and background are described at: https://www.autosar.org/capi

CAPI is an initiative driven by the AUTOSAR development cooperation. The Adaptive Platform standard defines the architecture and interfaces (the "what"), while CAPI will provide certification-ready, automotive-grade source code implementation (the "how"). By offering pre-built, automotive-grade middleware foundation code, it helps the industry avoid reinventing the wheel, improves interoperability between suppliers, reduces integration costs, and steadily advances consensus standards into deliverable code through quarterly releases, thereby accelerating SDV development and allowing teams to focus their energy on differentiating features rather than non-differentiating middleware.

CAPI concentrates on implementing and releasing the newest-possible implementation of the AUTOSAR Adaptive Platform.
The maintenance of previous releases is not part of the CAPI project but one of the various business
opportunities facilitated by CAPI.
While CAPI is a solid foundation, actual certification for the use in automotive systems and integration with the environment is left to AUTOSAR partners.


Terms of Use / License
======================

CAPI is an AUTOSAR project and therefore falls under certain conditions for its use.

All files – regardless of the type – are made available as released by AUTOSAR for the purpose of INFORMATION ONLY.

The material contained in those files is protected by copyright and other types of intellectual property rights, and the commercial exploitation of the contained material requires a license to such intellectual property rights.

If you are interested in commercial exploitation of released AUTOSAR Work, please apply for an AUTOSAR partnership. 

See also: `Joining AUTOSAR <#joining-autosar>`_, `the license file <LICENSE>`_


Try out CAPI
============

An easy to set up demo for CAPI is available that is also suited to allow your own experiments with CAPI.
This section describes the steps to try out CAPI using the demo.
For a more in-depth guide and alternative setup possibilities please consult
`the detailed description <docs/building_and_running_CAPI.rst>`_.

Prerequisites
-------------

- Unix-like OS (WSL works, MacOS untested)
- Git
- VS Code with ``Dev Containers`` extension from Microsoft
- Docker and Docker Compose

Steps
-----

1. Clone the CAPI repository

   .. code-block:: bash

      git clone git@github.com:autosar/capi.git

2. Open the repository in VS Code in its development container

   .. code-block:: bash

      code capi

   Run the VS Code command ``Dev Containers: Reopen in Container``.

3. Build and configure the demo

   Run all following commands of this tutorial in terminals in the development container.

   .. code-block:: bash

      # Compile CAPI source code
      ${SDK_UTILS} build -o "${CAPI_BUILD_DIR}" -v "2608" "apall#${CAPI_SRC_DIR}"
      # Pack and generate SDK
      ${SDK_UTILS} pack -o "${SDK_OUTPUT_DIR}" "${CAPI_BUILD_DIR}"

      # Install SDK
      ${SDK_OUTPUT_DIR}/SDK-*.run ${SDK_INST_DIR}
      # Build sample application
      ${SDK_INST_DIR}/ara-sysroot/build.sh -i "${CAPI_SRC_DIR}/samples/helloworld-cm"

      # Configure Machine
      ${SDK_INST_DIR}/ara-sysroot/config.sh -m Machine1 -s ${ARA_SYSROOT} -a ${CAPI_SRC_DIR}/samples/helloworld-cm -p $(hostname -I)

   Then build the demo container, playing the communication counterpart.
   The first time this will take some time, since the application is built during the setup of the container.

   .. code-block:: bash

      # Build the demo container
      docker compose -f "${CAPI_SRC_DIR}/demo/docker-compose.yml" build capi_demo

   For this introduction a demo application is used.
   You can find more information about the demo application's structure and behavior in `docs/tutorials/demo.rst Section: Demo Application Overview <docs/tutorials/demo.rst#demo-application-overview>`_.

4. Start the applications and set the function group states to *Driving*

   For the machine that runs in the development container:

   .. code-block:: bash

      # Start machine
      sudo ${ARA_SYSROOT}/run.sh -R

      # Switch into driving function group state
      sudo ${ARA_SYSROOT}/run.sh -c HelloWorldGroup.Driving

   For the machine that runs in the demo container:

   .. code-block:: bash

      # Start the capi_demo container
      docker compose -f "${CAPI_SRC_DIR}/demo/docker-compose.yml" up capi_demo

      # Switch into driving function group state
      docker exec capi_demo sudo ${ARA_SYSROOT}/run.sh -c HelloWorldGroup.Driving

   You should now see logs of the two machines echoing each other.

5. You can orderly shut down the machines like this:

   For the machine that runs in the development container:

   .. code-block:: bash

      sudo ${ARA_SYSROOT}/run.sh -s

   For the machine that runs in the demo container:

   .. code-block:: bash

      docker exec capi_demo sudo ${ARA_SYSROOT}/run.sh -s


Based on this setup you can experiment with CAPI.
Implement your changes to the stack, adapt the demo application ...


Project Status
==============

The current version of the code has been contributed by ``iSOFT Infrastructure Software Co., Ltd.``.
It follows the widely-used AUTOSAR Adaptive Platform release R20-11, and has forward-looking compatibility with some R23-11 interfaces.
It implements all 15 core functional clusters including Communication, Execution Management, Logging, and Diagnostics.
More details can be found in the `release notes <https://github.com/autosar/capi/releases>`_.


How to Contribute
=================

AUTOSAR CAPI is a community-source project that publishes its results.
Therefore, contributing to CAPI requires a partnership with AUTOSAR of a type that includes "Cooperation in Working Groups" (see: https://www.autosar.org/about/partners).


.. _joining-autosar:

Joining AUTOSAR
===============

Joining AUTOSAR is a low hurdle and offers many benefits on top of the ability to commercially exploit and contribute to and thus influence CAPI.
There are even good chances that your company is a partner already.

If you want to become an AUTOSAR partner you can find more information on `the AUTOSAR website <https://www.autosar.org/about/partners>`_


Source Code Directory Structure
===============================

The source code directory structure is as follows:

.. code-block::

    ├── cmake                          # CMake project configuration files
    ├── com                            # AP Communication module
    ├── core                           # AP Common data types module
    ├── crypto                         # AP Crypto module
    ├── demo                           # Setup for the demo
    ├── diag                           # AP Diagnostics module
    ├── docs                           # Central project documentation files
    ├── exec                           # AP Execution Management module
    ├── fw                             # AP Firewall module
    ├── iam                            # AP Identity and Access Management
    ├── idsm                           # AP Intrusion Detection System Management
    ├── isoft                          # iSOFT in-house foundational library, provides infrastructure for AP functional clusters
    │   ├── apcommon-cmake-modules     # Common CMake configurations
    │   ├── ara-fsh                    # AP runtime file directory structure definition
    │   ├── ara-gen                    # Generator
    │   ├── ara-tools                  # Runtime tools
    │   ├── arxmls                     # Standard model definitions
    │   ├── common                     # Common types, macros, and other definitions
    │   ├── crc                        # CRC checksum algorithm implementation
    │   ├── e2e                        # End-to-end communication protection implementation
    │   ├── ipc                        # General IPC library
    │   ├── ipcbase                    # IPC operating system related implementation
    │   ├── manifestreader             # Manifest reader
    │   ├── nai                        # OS Native Abstraction Interface
    │   ├── naicpp                     # C++ wrapper for nai
    │   ├── npc                        # Zero-copy IPC library
    │   ├── nsomeip                    # SOME/IP protocol implementation
    │   ├── sdk-utils                  # SDK tools, used to compile AP source code and generate SDK
    │   └── serialize                  # Serialization library
    ├── LICENSES                       # Copies of licenses of third-party open source components
    ├── log                            # AP Logging and Tracing module
    ├── nm                             # AP Network Management module
    ├── per                            # AP Persistence module
    ├── phm                            # AP Platform Health Management module
    ├── samples                        # AP Application examples
    ├── sm                             # AP State Management module
    ├── tsync                          # AP Time Synchronization module
    └── ucm                            # AP Update and Configuration Management module


Collected Links
===============

- `CAPI information on the AUTOSAR homepage <https://www.autosar.org/capi>`_
- `Detailed description on how to build and run the project <docs/building_and_running_CAPI.rst>`_
- `Details about the demo <docs/tutorials/demo.rst>`_
- `Release page <https://github.com/autosar/capi/releases>`_
- `License information <LICENSE>`_
