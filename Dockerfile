# Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
# SPDX-FileCopyrightText: 2023 
#
# SPDX-License-Identifier: Apache-2.0

FROM debian:latest AS intermediate

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# install some packages needed to set up the container and the dalos SDK in the container
RUN apt-get update && \
    apt-get install -y sudo \
    	build-essential \
        make \
        xz-utils \
        python3 \
        wget \
        iputils-ping \
        openssh-client \
        graphviz \
        doxygen \
        file \
        perl \
        bc \
        libelf-dev && \    
    useradd --create-home --shell /bin/bash buildx && \
    echo "buildx:buildx" | chpasswd && adduser buildx sudo

# create /usr/lib64 and link libc files there to avoid issues with u-os-x86-64 SDK when preparing kernel module build
RUN ln -s /usr/lib/x86_64-linux-gnu/libc.so.6 /usr/lib64/libc.so.6 && \
    ln -s /usr/lib/x86_64-linux-gnu/libc_nonshared.a /usr/lib64/libc_nonshared.a

# define user name and working directory
USER buildx
WORKDIR /home/buildx

# get the CA certificate file for test.mosquitto.org
RUN wget -nv -P/tmp https://test.mosquitto.org/ssl/mosquitto.org.crt
ADD --chown=buildx:buildx https://test.mosquitto.org/ssl/mosquitto.org.crt /tmp


FROM intermediate AS armv7
# trigger cache invalidation to force actual reload and (re-)install of the SDK
ARG CACHEBUST=1

ARG SDK_INSTALLER_VERSION="2.7.0"
ARG SDK_INSTALLER_PREFIX="UC20-WL2000-AC-SDK-"
ARG SDK_INSTALLER=$SDK_INSTALLER_PREFIX$SDK_INSTALLER_VERSION".sh"

# download SDK installer from WI's github
ADD --chmod=775 --chown=buildx:buildx https://github.com/weidmueller/u-os-sdk-quickstart/releases/download/v${SDK_INSTALLER_VERSION}_WL2000/$SDK_INSTALLER /tmp

# run SDK installer, we install to default path and say "yes" to all questions
 RUN /tmp/$SDK_INSTALLER -y -d /home/buildx

# delete the SDK installer after the sdk installation has finished.
 RUN rm -r /tmp/$SDK_INSTALLER

# the .bashrc of user buildx we source the environment setup script of the SDK.
 RUN echo "\n#initialize SDK environment variables\nsource /home/buildx/env*\n" >> /home/buildx/.bashrc

# additionally, initialize KERNEL_SRC in environment
RUN  echo "#initialize KERNEL_SRC in SDK environment\nexport KERNEL_SRC=\$OECORE_TARGET_SYSROOT/usr/src/kernel\n" >> /home/buildx/.bashrc

FROM intermediate AS armv8
# trigger cache invalidation to force actual reload and (re-)install of the SDK
ARG CACHEBUST=1

ARG SDK_INSTALLER_VERSION="2.7.0"
ARG SDK_INSTALLER_PREFIX="UC20-M3000-M4000-SDK-"
ARG SDK_INSTALLER=$SDK_INSTALLER_PREFIX$SDK_INSTALLER_VERSION".sh"

# download SDK installer from WI's github
ADD --chmod=775 --chown=buildx:buildx https://github.com/weidmueller/u-os-sdk-quickstart/releases/download/v${SDK_INSTALLER_VERSION}_M4k/$SDK_INSTALLER /tmp

# run SDK installer, we install to default path and say "yes" to all questions
RUN /tmp/$SDK_INSTALLER -y -d /home/buildx

# delete the SDK installer after the sdk installation has finished.
RUN rm -r /tmp/$SDK_INSTALLER

#In the .bashrc of user buildx we source the environment setup script of the SDK.
RUN echo "\n#initialize SDK environment variables\nsource /home/buildx/env*\n" >> /home/buildx/.bashrc

# additionally, initialize KERNEL_SRC in environment
RUN  echo "#initialize KERNEL_SRC in SDK environment\nexport KERNEL_SRC=\$OECORE_TARGET_SYSROOT/usr/src/kernel\n" >> /home/buildx/.bashrc

FROM intermediate AS x86_64
# trigger cache invalidation to force actual reload and (re-)install of the SDK
ARG CACHEBUST=1

ARG SDK_INSTALLER_VERSION="2.5.0"
ARG SDK_INSTALLER_PREFIX="UC20-S5000-SDK-"
ARG SDK_INSTALLER=$SDK_INSTALLER_PREFIX$SDK_INSTALLER_VERSION".sh"

# install SDK into container
RUN --mount=type=bind,source=U-OS-X86-64-SDK-debug.sh,target=/tmp/U-OS-X86-64-SDK-debug.sh \
    /tmp/U-OS-X86-64-SDK-debug.sh -y -d /home/buildx

#In the .bashrc of user buildx we source the environment setup script of the SDK.
RUN echo "\n#initialize SDK environment variables\nsource /home/buildx/env*\n" >> /home/buildx/.bashrc

# additionally, initialize KERNEL_SRC in environment
RUN  echo "#initialize KERNEL_SRC in SDK environment\nexport KERNEL_SRC=\$OECORE_TARGET_SYSROOT/usr/src/kernel\n" >> /home/buildx/.bashrc
