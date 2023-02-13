# # uc-sdk-hello-world

# Overview:

This example collection demonstrates usage of the C/C++ SDK for the WI UOS on IOT-GW30 and UC20-WL2000.

The example "helloworld" uses the mosquitto MQTT library and the openSSL 3.x library to connect to test.mosquitto.org. After a successful connect, the library sends a small JSON message to a hardcoded topic every 5s.
It demonstrates usage of the SDK with cmake, usage of the SDK with autotools and usage of self-build c/c++ shared libraries in your own c/c++ projects.

The example "hello_io_world" demonstrates process data exchange with UR20 modules attached to a UC20-WL2000.

The example collection is structured as a VSC workplace folder with four projects in it:

1. openSSL 3.x
2. mosquitto
3. helloworld
4. hello_io_world

The workplace folder also provides files that configure the build environment in a Docker container. VSC operates Docker for you to create this container and connect to it to use it as a build server.

# How to prepare the build environment:

- use Ubuntu 20.04 or higher, native or in a VM, as a development environment

- install Visual Studio Code (VSC)

- install Docker, see here: `https://docs.docker.com/engine/install/ubuntu/`

- install the Visual Studio Code Extensions "Dev Containers", "C/C++ Extension Pack" and "Native Debug" in VSC

- get the example with `git clone https://github.com/weidmueller/uc-sdk-hello-world.git`

- get the submodules with libraries required for the example: `cd uc-sdk-hello-world && git submodule update --init --remote --rebase`

- in VSC's menu, go to *File -> Open Workspace* from file.. and there open uc-sdk-hello-world/workspace.code-workspace

- in VSC's left corner of the lower status bar, use the green >< "Open a remote window" button and select "Reopen in container". The first time you do this, VSC lets docker create the container from scratch, so be patient.

# For the helloworld example:

- in VSC's menu, use *Terminal -> Run* task... to select and run the following items:
    - configure openSSL
    - make install openssl
    - mosquitto: cmake
    - mosquitto: make install
    - cmake helloworld
    - make helloworld
    
Now, your helloworld binary is ready.

# For the hello_io_world example:

- in VSC's menu, use *Terminal -> Run* task... to select and run the following items:
    - cmake hello_io_world
    - make hello_io_world

Now your hello_io_world binary is ready. 

# Debugging on the target device:

If you like, run a debug session on an IOT-GW30 or UC20-WL2000 with a UR20-DO16 attached to it:

Both the hello_io_world and helloworld project in the workspace have a launch configuration that copies the binary to the remote device, namely to root@192.168.0.101/tmp.
Then the launch configuration starts gdbserver for a debug session of your binary on the remote device and finally launches gdb for a remote debug session with VSC on your host system.

- make sure that a user with the name "admin" and administrator priviledges exists on your target device and activate the SSH access via the device's web interface -> Identity and Access -> SSH access.
- if you do this for the first time, run `ssh admin@192.168.0.101` in a VSC terminal before you start any launch configuration. Hit 'yes' when ssh asks about the unknown certificate, then exit the ssh connection, again.
- hit Ctrl-Shift-D or View->Run for the Run-and-Debug view of VSC. Then find the "Launch" dropdown list near the left end of the bar below the menu bar.
    - select either helloworld or hello_io_world.
- in VSC, go to Run -> Start Debugging
- VSC may ask for your device's password two times. Be patient and enter both times.
- after everything has started, observe the gdb control bar with the play, step into, step over, restart and stop buttons. 
- press "play" and switch VSC to the terminal -> the helloworld example prints some status output to the console.
- A remark regarding the hello_io_world example: 
    - hello_io_world uses sched_setscheduler() and some other system calls that require root priviledges. In order to provide them, the launch configuration calls a prelaunch task that copies the bash script "example_io_root" to the PLC and starts this bash script via ssh. The bash script in turn calls gdbserver with sudo. For this to work properly, you must edit example_io_root and fill in the password of the user "admin" on your target device.
    - because the hello_io_world example needs to run gdbserver with sudo, the stdio redirect connection to gdb on the development host did not work and i did not figure out why, yet. That is why i had to use the TCP connection. With tcp connection, gdbserver does not forward stdout from the application. If you want to view the printouts from hello_io_world, you need to ssh into the target and run `sudo gdbserver :2159 /tmp/example_io` by hand. Then you can observe the printouts of hello_io_world in the ssh shell. Use the launch configuration "hello_io_world no autostart". Use the task "copy example_io to target" to copy the hello_io_world executable to the target's /tmp folder initially or after you rebuilt it.