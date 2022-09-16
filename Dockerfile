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

# fetch SDK installer from server
#TODO: replace URL of apache on this Ubuntu VM with download location on WI server
#RUN wget -nv -P/tmp 192.168.76.128/SDK-UCG-1.16.0-beta.7-debug.sh

RUN wget -nv -P/tmp http://srvde554.weidmueller.com:8071/repository/snapshots/dalos/feature/dev-jmenzel/dalos/SDK-UCG-1.16.0-beta.7-debug.sh

#RUN wget -nv -P/tmp 192.168.76.128/dalos-bleeding-glibc-x86_64-meta-toolchain-weidmueller-cortexa9hf-neon-ucg-toolchain-weidmueller-20220621075014-debug.sh
#RUN wget -nv -P/tmp 192.168.76.128/dalos-glibc-x86_64-meta-toolchain-weidmueller-cortexa9hf-neon-ucg-toolchain-weidmueller-20220525152023-debug.sh

# set execution permission for SDK installer
RUN chmod +x /tmp/SDK-UCG-*.sh
#RUN chmod +x /tmp/dalos-bleeding-glibc-x86_64-meta-toolchain-weidmueller-cortexa9hf-neon-ucg-toolchain-weidmueller-*.sh

# run SDK installer, we install to default path and say "yes" to all questions
RUN /tmp/SDK-UCG-*.sh -y
#RUN /tmp/dalos-bleeding-glibc-x86_64-meta-toolchain-weidmueller-cortexa9hf-neon-ucg-toolchain-weidmueller-*.sh -y

# copy our .bashrc file into the container. Inside the .bashrc we source the environment setup script of the SDK.
COPY .bashrc /root/
