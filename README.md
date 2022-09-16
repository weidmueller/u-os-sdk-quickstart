# uc-sdk-hello-world

This example demonstrates usage of the C/C++ SDK for the WI IoT-GW30:

The example uses the mosquitto MQTT library and the openSSL 3.x library to connect to test.mosquitto.org. After a successful connect, the library sends a small JSON message to a hardcoded topic every 5s.

The example project is structured as a VSC workplace folder with three projects in it:

1. openSSL 3.x
2. mosquitto
3. the example itself

The workplace folder also provides files that configure the build environment in a Docker container. VSC operates Docker for you to create this container and connect to it to use it as a build server.

This is how to use the example:

- use Ubuntu 20.04 or higher, native or in a VM, as a development environment

- install Visual Studio Code (VSC)

- install Docker, see here: `https://docs.docker.com/engine/install/ubuntu/`

- get the example with `git clone https://github.com/weidmueller/uc-sdk-hello-world.git`

- in VSC, go to File -> Open Workspace from file.. and there open uc-sdk-hello-world/workspace.code-workspace

- in VSC's left corner of the lower status bar, use the green >< "Open a remote window" button and select "Reopen in container". The first time you do this, VSC lets docker create the container from scratch, so be patient.

- in VSC use Terminal -> Run task... to select and run the following items:
- configure openSSL
- make openSSL 
- make install openssl
- mosquitto: cmake
- mosquitto: make
- mosquitto: make install
- cmake helloworld
- make helloworld

Now, your binary is ready. If you like, run a debug session on a target:

The helloworld project in the workspace has a launch configuration that copies the binary to a remote device, namely to root@192.168.0.101/tmp. Then it starts gdbserver for a debug session of your binary on the remote device and finally launches gdb for a remote debug session with VSC on your host system. This is intended for use with the Weidmueller IoT-GW30 IoT Gateway.

- if you do this for the first time, run `ssh root@192.168.0.101` in a VSC terminal and click yes when ssh asks about the unknown certificate, then    exit the ssh connection, again
- in VSC, go to Run -> Start Debugging
- after everything has started, observe the gdb control bar with the play, step into, step over, restart and stop buttons. 
- press "play" and switch VSC to the terminal -> the example prints some status output to the console
