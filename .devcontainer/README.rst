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


========================
Use of the Dev Container
========================

To use the dev container do the following:

Utilize your IDE's capability to launch and connect to the dev container.
In case this is not how you prefer to work, you can also look into the cli tool https://github.com/devcontainers/cli.

**Just starting the docker image is not sufficient for a working setup.**

Depending on your company's setup you may have to install certificates inside the container to allow it to access the sources for required dependencies during the build.
Save your companies firewall certificate in path `.devcontainer/firwall.crt` to include it in the devcontainers certificate storage during build. This path is added to `.gitignore` to prevent uploads of the certificate.

Use within VS Code
==================

Install the extension ``Dev Containers`` by Microsoft (id: ``ms-vscode-remote.remote-containers``).
Execute the command ``Dev Containers: Reopen in Container``. 

Use within Other Tools
======================

Please refer to https://containers.dev/supporting for more information about other tools.
