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

FROM capi_devcontainer AS capi_demo

USER vscode

# Copy over the sdk
# This requires the sdk to be built first. The simplest way is to do this via the devcontainer.
# It will automatically place the SDK in the right location for it to be found.
# The path relative to the build context is used here since it is not known where the user clones the repo on their machine.
# See also: https://docs.docker.com/reference/dockerfile/#copying-from-the-build-context
COPY --chown=vscode:vscode ${REL_SDK_OUTPUT_DIR} ${SDK_OUTPUT_DIR}
# Install the SDK
RUN ${SDK_OUTPUT_DIR}/SDK-*.run ${SDK_INST_DIR}

# The samples directory
ENV REL_CAPI_SAMPLE_DIR="samples"
# Copy over the samples directory
COPY --chown=vscode:vscode ${REL_CAPI_SAMPLE_DIR} ${CAPI_SRC_DIR}/${REL_CAPI_SAMPLE_DIR}/
# The source directory of the sample applications relative to the samples directory root
ENV REL_CAPI_SAMPLE_APP_SRC_DIR="helloworld-cm"
# Build sample application
RUN ${SDK_INST_DIR}/ara-sysroot/build.sh -i ${CAPI_SRC_DIR}/${REL_CAPI_SAMPLE_DIR}/${REL_CAPI_SAMPLE_APP_SRC_DIR}

ENV MACHINE_NAME=Machine2

# Configure the machine on container startup and then run it.
# Note: generally hostname -I may return several addresses. 
#       Given the concrete setup it will only return one address. This command relies on that. 
#       If there were multiple addresses returned the command will result in an error.
CMD \
    ${SDK_INST_DIR}/ara-sysroot/config.sh -m ${MACHINE_NAME} -s ${ARA_SYSROOT} -a ${CAPI_SRC_DIR}/samples/helloworld-cm -p $(hostname -I) \
    && sudo ${ARA_SYSROOT}/run.sh -R
