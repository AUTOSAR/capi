# Disclaimer
#
# This work (specification and/or software implementation) and the material
# contained in it, as released by AUTOSAR, is for the purpose of information
# only. AUTOSAR and the companies that have contributed to it shall not be
# liable for any use of the work.
#
# The material contained in this work is protected by copyright and other
# types of intellectual property rights. The commercial exploitation of the
# material contained in this work requires a license to such intellectual
# property rights.
#
# This work may be utilized or reproduced without any modification, in any
# form or by any means, for informational purposes only. For any other
# purpose, no part of the work may be utilized or reproduced, in any form
# or by any means, without permission in writing from the publisher.
#
# The work has been developed for automotive applications only. It has
# neither been developed, nor tested for non-automotive applications.
#
# The word AUTOSAR and the AUTOSAR logo are registered trademarks.
# --------------------------------------------------------------------------

# Simple dockerfile optimized for the use with devcontainer

# Install dependencies according to readme
# Also install git-lfs

FROM mcr.microsoft.com/devcontainers/base:ubuntu20.04 AS capi_devcontainer

# If corporate firewall certificate is needed, place it to this folder
# as "firewall.crt". It will be copied and installed if filename is matching,
# otherwise no action is done and no error is reported.
# use the [t] pattern to make the copy not fail if there is no firewall.crt the pattern will only match "firewall.crt"
COPY firewall.cr[t] /usr/local/share/ca-certificates/firewall.crt
RUN if [ -f /usr/local/share/ca-certificates/firewall.crt ]; \
    then update-ca-certificates; \
    echo "export NODE_EXTRA_CA_CERTS=/usr/local/share/ca-certificates/firewall.crt" \
    >> /home/vscode/.bashrc; fi

RUN apt update && apt install -y \
    build-essential \
    pkg-config \
    cmake \
    make \
    patch \
    tar \
    sed \
    git \
    gperf \
    autoconf \
    libtool \
    pigz \
    python3-pip \
    git-lfs

RUN pip install \ 
    gitpython==3.1.50 \
    json5==0.14.0 \
    jsonpath==0.82.2 \
    pyelftools==0.32 \
    Jinja2==3.1.6 \
    lxml==4.9.1 \
    xmltodict==0.14.2 \
    pyinstaller==6.21.0 \
    psutil==7.2.2

# Configure some presets for the devcontainer, mostly according to the readme

# CAPI source code directory
ENV CAPI_SRC_DIR="/workspaces/capi"
# Home directory
ENV HOME="/home/vscode"
# SDK-UTILS script
ENV SDK_UTILS="${CAPI_SRC_DIR}/isoft/sdk-utils/bin/sdk-utils.py"
# CAPI build directory, used to store temporary files during compilation
ENV CAPI_BUILD_DIR="${CAPI_SRC_DIR}/build"
# CAPI_SDK directory, used to store generated SDK files
# The relative path from the CAPI source code directory is used in containers using this as the base image
ENV REL_SDK_OUTPUT_DIR="build/sdk"
ENV SDK_OUTPUT_DIR="${CAPI_SRC_DIR}/${REL_SDK_OUTPUT_DIR}"
# SDK_INST directory, used to store installed SDK
ENV SDK_INST_DIR="${HOME}/capi/sdk-native"
# ARA_SYSROOT directory
ENV ARA_SYSROOT="${HOME}/capi/ara-sysroot"

# Fully install git lfs
USER vscode
RUN git lfs install
