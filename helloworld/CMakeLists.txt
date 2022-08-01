cmake_minimum_required(VERSION 3.10)

# set the project name
project(example VERSION 0.0.1)

# add the executable
add_executable(example main.c)

target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libmosquitto_static.a)
target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libpthread.so)
target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libssl.a)
target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libcrypto.a)
target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libdl.so)

#target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libpthread.so)
#target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libssl.a)
#target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libcrypto.a)
#target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libmosquitto.so)
#target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libdl.so)
