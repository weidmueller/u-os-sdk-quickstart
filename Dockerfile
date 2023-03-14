# Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
# SPDX-FileCopyrightText: 2023 
#
# SPDX-License-Identifier: Apache-2.0

FROM debian:bullseye

#ARG DEBIAN_FRONTEND noninteractive

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# install some packages needed to set up the container and the dalos SDK in the container
RUN apt-get update && \
    apt-get install -y \
        make \
        xz-utils \
        python \
        wget \
        iputils-ping \
        openssh-client \
        perl

# trigger cache invalidation to force actual reload and (re-)install of the SDK
ARG CACHEBUST=1

# fetch SDK installer from server
ARG SDK_INSTALLER=dalos-glibc-x86_64-meta-toolchain-weidmueller-cortexa9t2hf-neon-ucm-toolchain-2.0.0-beta.5+snapshot.sh

#TODO: replace URL of apache on this Ubuntu VM with download location on WI server
RUN wget -nv -P/tmp 192.168.76.128/$SDK_INSTALLER

# set execution permission for SDK installer
RUN chmod +x /tmp/$SDK_INSTALLER

# run SDK installer, we install to default path and say "yes" to all questions
RUN /tmp/$SDK_INSTALLER -y

# delete the SDK installer after the sdk installation has finished.
RUN rm -r /tmp/$SDK_INSTALLER

# copy our .bashrc file into the container. Inside the .bashrc we source the environment setup script of the SDK.
COPY .bashrc /root/
