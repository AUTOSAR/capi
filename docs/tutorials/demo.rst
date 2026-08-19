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

====
Demo
====

This file describes how to build and run a demo consisting of two machines communicating with each other.
It can be achieved with only a few steps.
Both machines are docker containers. One is the development container.
The communication partner is realized by a second container (demo container).

Prerequisites
=============

- Unix-like OS (WSL works, MacOS untested)
- Git
- VS Code with ``Dev Containers`` extension from Microsoft
- Docker and Docker Compose

Steps
=====

1. Clone the CAPI repository and leave the default name (``capi``)
2. Open the repository in VS Code in its development container

   When you are in the parent directory of the newly cloned ``capi`` directory:

   .. code-block:: bash

      code capi

   Run the VS Code command ``Dev Containers: Reopen in Container``.

3. Build and configure the demo

   Run all following commands of this tutorial in terminals in the development container.

   First, build and install CAPI:

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

   You can find more information about the demo application's structure and behavior in `Section: Demo Application Overview <#demo-application-overview>`_,

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


.. _demo-application-overview:

=========================
Demo Application Overview
=========================

In this section gives an overview of the demo application.
It adds context to the demo and its behavior but is not required to run the demo.

The CAPI source code directory includes a communication example program source code, located at ``${CAPI_SRC_DIR}/samples/helloworld-cm``.

The example defines a Service Interface containing:

- A Method: EchoMethod
- An Event: testEvent

The service is deployed through the SOME/IP protocol, fully demonstrating the design, configuration, deployment, and code implementation process based on Service-Oriented Architecture (SOA) on the AUTOSAR AP platform. The system consists of two running entities:

- P end (Provider instance): serverd, acting as the service provider
- R end (Requester instance): clientd, acting as the service requester

The two communicate via SOME/IP to achieve cross-process communication, verifying the basic functionality of the AP COM module.

Example Program Runtime Behavior Description
============================================

P end (serverd) Behavior
""""""""""""""""""""""""

- After successfully calling OfferService(), it enters the service-ready state and waits for R end's invocation requests.
- When receiving an EchoMethod invocation:
  - Triggers the testEvent in the method processing logic;
  - Prints log information in the following format:

.. code-block::

    12026/06/06 13:28:50.097688 30775.3253 91 ECU1 clie #COM 1790684 log info verbose 4 iSOFT for CAPI: EchoMethod call from client : Com-Client-Test[n]

The [n] in the log represents the sequence number passed by the R end in this request (starting from 1 and incrementing).

R end (clientd) Behavior
""""""""""""""""""""""""

- After successfully discovering the service, it cyclically calls EchoMethod at 1-second intervals;
- The request parameter for each call is the string: Com-Client-Test[n], where n starts from 1 and increments by 1 each time;
- After receiving the result from the server, it prints log information in the following format:

.. code-block::

    12026/06/06 13:28:50.097688 30775.3253 91 ECU1 clie #COM 1790684 log info verbose 4 iSOFT for CAPI: Helloworld-cm-Client [16] call EchoMethod recv echo: From service iSOFT for CAPI:Com-Client-Test[17]


.. note::

    The sequence number [16] displayed by the client in the log corresponds to its 16th sent request, while the server's echo of [17] is because the server may have performed a +1 operation on the sequence number during processing (the specific logic is determined by the example implementation, and this is for illustration only).

.. important::

    The example code configures two applications: client and server. Subsequent compilation, configuration, and execution need to be performed separately on different machines.
