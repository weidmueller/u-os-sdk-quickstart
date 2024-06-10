# Flatbuffers

This VSC project assists software developers in linking their applications against the google flatbuffers library, see here: [google flatbuffers](https://flatbuffers.dev/).

WI has added a flatbuffers VSC project to the u-os-sdk-quickstart repository because some u-Os services use flatbuffers format for their API.
Flatbuffers projects may require usage of the flatc compiler to generate c/c++ headers from your schema files. The flatc compiler needs to run - well - at compile time and 
therefore on your host x86 architecture. On the other hand, you want to run your application on the target device which has armhf architecture. Therefore, you need the flatbuffers
library to be compiled for your device's architecture. Because is a bit tricky to achive mixed target compilation, WI has prepared some VSC tasks and a toolchain config cmake file for you.

## Build instructions

-- in VSC's menu, use *Terminal -> Run task...* to select and run the following items:
    - *cmake library flatbuffers*
    - *make  library flatbuffers*
    - *make install library flatbuffers*

The *<do something> library flatbuffers* tasks have dependency entries for *do something> flatc flatbuffers* tasks. Therefore, VSC knows to automatically run the flatc-related
tasks beforehand.

The *install* tasks will copy the relevant files to the correct locations in the host- and target sysroot of the u-Os SDK.

After you have completed the configuration, build and install of the flatbuffers library, you are ready to build applications that use flatbuffers.

## Configuration management

Currently, the google flatbuffers is a quite dynamic open source project with frequent releases of new versions. Changes may break the flatbuffers API; It may be adviseable
to add a `branch = <tag or commit id\>` line to the flatbuffers related entry in the .gitmodules file of your project.