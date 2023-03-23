<!--
Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
SPDX-FileCopyrightText: 2023 

SPDX-License-Identifier: Apache-2.0
-->

# uc-sdk-hello-world - helloworld

## Overview:

This example demonstrates basic usage of the u-Os SDK installation. It is intended for linux application programmers that prefer to work on the shell to build their application or want to integrate the SDK with the IDE of their choice.

### SDK installation content
Since u-Os derives from Yocto, the SDK, too, is WI's adaption of the Yocto SDK. It encompasses a GNU cross toolchain, a target sysroot and a host sysroot. Please refer to the Yocto SDK documentation for more details:

- [Useful basic information and general installation hints](https://docs.yoctoproject.org/sdk-manual/using.html)

- [Usage information](https://docs.yoctoproject.org/sdk-manual/working-projects.html)
  
### build tools
The Yocto SDK usage information also covers autotools. For cmake, the u-Os SDK installation brings along a cmake that is aware of the cmake toolchain file $OECORE_NATIVE_SYSROOT/usr/share/cmake/OEToolchainConfig.cmake. Thus, you configure existing cmake projects correctly for cross compilation with the SDK by calling the SDK's cmake with the proper environment variables:

    `cmake .. -DCMAKE_INSTALL_PREFIX=$SDKTARGETSYSROOT/usr -DCMAKE_STAGING_PREFIX=$SDKTARGETSYSROOT/usr`

## How to prepare the build environment:

- use Ubuntu, at least 20.04 or Debian at least 11, native or in a VM, as a development environment

- in this repository, go to Releases, select the Release matching your target firmware and download the SDK installer script, e.g.  dalos-glibc-x86_64-meta-toolchain-weidmueller-cortexa9t2hf-neon-ucm-toolchain-2.0.0-beta.5+snapshot.sh, to /tmp

- run the installer script and follow its instructions

- export the build environment variables provided by the sdk:

    `source /opt/dalos*/2.0.0-beta.*/env*`

- run   

    `$CC --version`

    and verify the expected cross gcc version.

## Build and Run Instruction for helloworld
- clone this repository to a folder of your choice:

    `git clone https://github.com/weidmueller/uc-sdk-hello-world.git`

- cd into the helloworld folder

- compile and link the helloworld binary from the helloworld.c source:

    `$CC helloworld.c  -I$OECORE_TARGET_SYSROOT/usr/include -o helloworld`

- secure copy the binary to /tmp on your target (use your user name and target's ip address):
  
    `scp helloworld admin@192.168.0.101:/tmp`

- ssh into the target, replace your ip address and user name, again:

    `ssh admin@192.168.0.101`

- on the target, run the helloworld binary:

    `/tmp/helloworld`

##cmake and autotools practical usage
The examples in the VSC workspace cover these, please refer to the root [README](../README.md).

- for autotools, look at the configure task in openssl/.vscode/tasks.json.
- for cmake, look at the cmake task in helloworld_mqtt/.vscode/tasks.json.