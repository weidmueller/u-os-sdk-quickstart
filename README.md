# uc-sdk-hello-world

This example demonstrates usage of the C/C++ SDK for the WI IoT-GW30:

It uses the openSSL library and the mosquitto MQTT library to connect as MQTT client to test.mosquitto.org and then sends a small JSON message to a hardcoded topic every 5s.

This is how to use the example:

- use Ubuntu 20.04 or higher, native or in a VM, as a development environment

- install Visual Studio Code (VSC)

- get the example with `git clone https://github.com/weidmueller/uc-sdk-hello-world.git`

- in VSC, go to File -> Open Workspace from file.. and there open uc-sdk-hello-world/workspace.code-workspace

-in VSC's left corner of the lower status bar, use the green >< "Open a remote window" button and select "Reopen in container". The first time you do this, VSC lets docker create the container from scratch, so be patient.

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

The helloworld project in the workspace has a launch configuration that copies the binary to a remote device root@192.168.0.101/tmp. Then it starts gdbserver for a debug session of your binary on the remote device and finally launches gdb for a remote debug session with VSC on your host system.

- in VSC, go to Run -> Start Debugging
- after everything got started, observe the gdb control bar with the play, step into, step over, restart and stop buttons. 
- press "play" and switch VSC to the terminal -> the example provides some printfs that tell what it does