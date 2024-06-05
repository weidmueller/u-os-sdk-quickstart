# Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
# SPDX-FileCopyrightText: 2023 
#
# SPDX-License-Identifier: Apache-2.0

FROM debian:bullseye AS intermediate

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# install some packages needed to set up the container and the dalos SDK in the container
RUN apt-get update && \
    apt-get install -y sudo \
    	build-essential \
        make \
        xz-utils \
        python \
        wget \
        iputils-ping \
        openssh-client \
        graphviz \
        doxygen \
        perl && \
    useradd --create-home --shell /bin/bash buildx && \
    echo "buildx:buildx" | chpasswd && adduser buildx sudo

# define user name and working directory
USER buildx
WORKDIR /home/buildx

# get the CA certificate file for test.mosquitto.org
#RUN wget -nv -P/tmp https://test.mosquitto.org/ssl/mosquitto.org.crt
ADD --chown=buildx:buildx https://test.mosquitto.org/ssl/mosquitto.org.crt /tmp


FROM intermediate AS armv7
# trigger cache invalidation to force actual reload and (re-)install of the SDK
ARG CACHEBUST=1

ARG SDK_INSTALLER="UC20-WL2000-AC-SDK-2.1.0.sh"

# download SDK installer from WI's github
ADD --chmod=775 --chown=buildx:buildx https://github.com/weidmueller/u-os-sdk-quickstart/releases/download/v2.1.0_WL2000/$SDK_INSTALLER /tmp

# run SDK installer, we install to default path and say "yes" to all questions
RUN /tmp/$SDK_INSTALLER -y -d /home/buildx

# delete the SDK installer after the sdk installation has finished.
RUN rm -r /tmp/$SDK_INSTALLER

# the .bashrc of user buildx we source the environment setup script of the SDK.
RUN echo "\n#initialize SDK environment variables\nsource /home/buildx/env*\n" >> /home/buildx/.bashrc


FROM intermediate AS armv8
# trigger cache invalidation to force actual reload and (re-)install of the SDK
ARG CACHEBUST=1

ARG SDK_INSTALLER="UC20-M3000-M4000-SDK-2.1.0.sh"

# download SDK installer from WI's github
ADD --chmod=755 --chown=buildx:buildx https://github.com/weidmueller/u-os-sdk-quickstart/releases/download/2.1.0/$SDK_INSTALLER /tmp

# run SDK installer, we install to default path and say "yes" to all questions
RUN /tmp/$SDK_INSTALLER -y -d /home/buildx

# delete the SDK installer after the sdk installation has finished.
RUN rm -r /tmp/$SDK_INSTALLER

# the .bashrc of user buildx we source the environment setup script of the SDK.
RUN echo "\n#initialize SDK environment variables\nsource /home/buildx/env*\n" >> /home/buildx/.bashrc

