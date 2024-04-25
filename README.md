<!--
Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
SPDX-FileCopyrightText: 2023 

SPDX-License-Identifier: Apache-2.0
-->

# u-os-sdk-quickstart

## Overview:

This example collection demonstrates usage of the C/C++ SDK for the WI u-Os devices. It is intended for software developers that have experience with Linux application development.

For a basic overview and an introduction to the u-Os SDK installer, see the helloworld [README](helloworld/README.md).

For more advanced examples read on about the VSC workplace folder with the following projects in it:

1. openSSL 3.x
2. mosquitto
3. helloworld_mqtt
4. flatbuffers

The example "helloworld_mqtt" uses the mosquitto MQTT(S) library and the openSSL 3.x library to connect to test.mosquitto.org. After a successful connect, the library sends a small JSON message to a hardcoded topic every 5s.
It demonstrates usage of the SDK with cmake, usage of the SDK with autotools and how to build c/c++ shared and static libraries and use them in your own c/c++ projects.
The flatbuffers library is going to be useful for building user applications because some u-Os services use flatbuffers interfaces.

The workplace folder also provides files that configure a build environment in a Docker container. VSC operates Docker for you to create this container and connect to it to use it as a build server.

## How to prepare the build environment:

- use Ubuntu 20.04 or higher, native or in a VM, as a development environment. This requires approx. 40GB of disk space, so reserve accordingly when you create a VM.

- install Visual Studio Code (VSC)

- install Docker, see here: [install](https://docs.docker.com/engine/install/ubuntu/) and here: [postinstall](https://docs.docker.com/engine/install/linux-postinstall/)

- install the Visual Studio Code Extensions "Dev Containers", "C/C++ Extension Pack" and "Native Debug" in VSC

- in your home folder, get the example with `git clone --recurse-submodules https://github.com/weidmueller/u-os-sdk-quickstart.git`

- in VSC's menu, go to *File -> Open Workspace from file..* and there open u-os-sdk-quickstart/workspace.code-workspace. Ignore the `compilerPath "${OECORE_NATIVE_SYSROOT}`-related warnings, for now. Click "reopen in container" or proceed to the next step:

- in VSC's left corner of the lower status bar, use the green >< "Open a remote window" button and select "Reopen in container". The first time you do this, VSC lets docker create the container from scratch, so be patient. Hint: in the container, your user is buildx with the sudo password buildx.

## Documentation
Each project has doxygen documentation that explains it from a higher level down to the details. 
In VSC's menu, use *Terminal -> Run task...* to select and run *doxygen <name of project\>*.
VSC will run doxygen to generate a html folder in the project. Please navigate to the respective folder with Ubuntu's file explorer and open index.html.

## Build instructions

Some projects depend on each other. You need to build openSSL first, then mosquitto and then helloworld_mqtt.

- in VSC's menu, use *Terminal -> Run task...* to select and run the following items. Replace <name of project\> with the name of each project:
    - *cmake <name of project\>* or *configure <name of project\>*, depending on the project
    - *make <name of project\>*
    - *make install <name of project\>* only for openSSL and mosquitto, this installs the respective library binaries into the SDK's target sysroot.

- for the helloworld_mqtt example, build openssl and mosquitto, first. Also, read the doxygen documentation in the html folder.

- for the flatbuffers library, see the [README](flatbuffers/README.md).

## Debugging on the target device:

NB: For simplicity, the following paragraph assumes that your target's ip address is 192.168.0.101. Please update the settings.json file in the projects you want to debug with your target's actual ip address before you try to launch a debug configuration.

If you like, run a debug session on an IOT-GW30 or UC20-WL2000 with a UR20-DO16 attached to it:

Both the hello_io_world and helloworld_mqtt project in the workspace have a launch configuration that copies gdbserver and the binary to the remote device, namely to admin@192.168.0.101/tmp.
Then the launch configuration starts gdbserver for a debug session of your binary on the remote device and finally launches gdb for a remote debug session with VSC on your host system.

- make sure that a user with the name "admin" and administrator priviledges exists on your target device and activate the SSH access via the device's web interface -> Identity and Access -> SSH access.
- if you do this for the first time, run `ssh admin@<your target's ip address>` in a VSC terminal before you start any launch configuration. Hit 'yes' when ssh asks about the unknown certificate, then exit the ssh connection, again.
- hit Ctrl-Shift-D or View->Run for the Run-and-Debug view of VSC. Then find the "Launch" dropdown list near the left end of the bar below the menu bar.
    - select the project you want to debug.
- in VSC, go to Run -> Start Debugging
- VSC may ask for your device's password two times. Be patient and enter both times.
- after everything has started, observe the gdb control bar with the play, step into, step over, restart and stop buttons. 
- press "play" and switch VSC to the terminal -> the helloworld example prints some status output to the console.
- the download to the device may take longer than gdb's connect-to-target timeout. If you experience timeout errors, run the task "remote gdb server <name of project\>" first, then launch the debug session.
