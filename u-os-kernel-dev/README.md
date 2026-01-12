# u-OS 2.5.0 Kernel sources for IOT-GW (ucg), M3000, M4000 (ucu) and WL2000 (ucm)

From u-OS 2.5.0 on, the SDK installer includes the following components:

- kernel sources including configuration and patches
- toolchain including cross compiler

Also, u-os-sdk-quickstart holds:

- minimal sample kernel module source to verify the development setup
- devcontainer setup to be able to use the environment in Visual Studio Code

## Important note, please read

u-OS and u-OS devices use secure boot and WI will make secure boot mandatory in the near future.
Therefore, u-OS devices will not boot modified kernels not published by WI.
Do not change kernel/CONFIG. Do not copy a modified kernel to your u-OS device.
However, this does not affect the usability of custom kernel modules; You may
still create, build and use your own kernel modules on u-OS devices, even with
secure boot.

## System requirements

This environment requires a Linux System (e.g. Ubuntu 24.04). This is tested
on a Windows PC in a virtual machine with Ubuntu 24.04 with activated docker and installed
Visual Studio Code (version 1.106.3)

## Setup environment in Visual Studio Code

Open Visual Studio Code. In the lower left corner, click the "><"-Symbol. VSC opens a
drop-down menu in the upper middle of the window. Select "open folder in container", navigate to
and select u-os-sdk-qickstart.

Visual Studio Code detects the `.devcontainer` folder and shows a drop-down menu letting you select DALOS-SDK 32bit or DALOS-SDK 64bit. Choose according to your target architecture.

VS Code now builds the development container. Wait until that process has finished. The build process also sources the environment setup of the SDK and sets up the kernel source path in the container's shell environment variable $KERNEL_SRC.

Open a terminal by clicking `Terminal` -> `New Terminal` in the top menu bar.

Execute all following commands in the terminal you opened in the step before.

## Prepare Linux kernel sources

For preparation to build linux kernel modules, execute these commands:

    $ cd $KERNEL_SRC
    $ make scripts
    $ make modules_prepare

Now the kernel-source folder is in the required state to build out-of-tree kernel modules.

## Build a demonstrator out-of-tree kernel module

The folder u-os-kernel-dev/hello-mod contains sources for a minimal kernel module to verify the
development setup. To build this module, change into its folder and
execute the following commands:

    $ make

This creates the kernel module `hello.ko` in the current folder. This file is a hello-world-like kernel module.
To test on the device, copy it to the device e.g. via scp and load it via insmod. Follow the commands below:

    $ scp hello.ko admin@192.168.0.101:/tmp
    $ ssh admin@192.168.0.101
    $ sudo /usr/sbin/insmod /tmp/hello.ko

The output is now visible in the kernel log.

    $ dmesg

Check that demsg's output contains the string `Hello World!` in one of the last lines.

To unload the module, run on the device:

    $ sudo rmmod hello