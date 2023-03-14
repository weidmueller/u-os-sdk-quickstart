// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#include "iostream"

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>
#include <interface.h>

#include "printBuffer.h"

#include "sys/socket.h"

using namespace std;

int transceive (uint8_t* raw_request_buffer, uint8_t* raw_response_buffer, uint resp_buf_size){

    /* -- the uc-slio-service expects a flatbuffer message to start with a uint32_t size --*/
    uint32_t buf_len = *((uint32_t*)raw_request_buffer);

//    cout << "buffer contains: " << endl;
//    PrintBuffer(raw_request_buffer, buf_len);

    /* --- create and open a socket to the uc-slio-service */
    int client_fd;

    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
            cout << endl << "Socket creation error" << endl;
            return -1;
        }

    struct sockaddr_un {
        short sun_family;           /* AF_UNIX */
        char sun_path[108];         /* path name (gag) */
    };

    sockaddr_un saUcSlioService;
    saUcSlioService.sun_family = AF_UNIX;
    strcpy(saUcSlioService.sun_path, "/var/run/devices.sock");
    int len = strlen(saUcSlioService.sun_path) + sizeof(saUcSlioService.sun_family);

    int status;
    if ((status = connect(client_fd, (struct sockaddr*)&saUcSlioService, len)) < 0) {
        cout << endl << "Connection Failed with status " << hex << status << endl;
        return -1;
    }
    int len_sent;
    len_sent = send(client_fd, raw_request_buffer, buf_len, 0);
    if (len_sent < 0){
      cout << "Error: socket::send failed with return code " << len_sent << "." << endl;
      close(client_fd);
      return -1;
    }
    if (len_sent == 0){
      cout << "Error: socket::send has sent 0 bytes." << endl;
      close(client_fd);
      return -1;
    }

    int len_recvd{0};
    if ((len_recvd = recv(client_fd, raw_response_buffer, resp_buf_size, 0)) >0) {
//      cout << "received response has " << dec << len_recvd << " bytes." << endl;
//      PrintBuffer((uint8_t*)raw_response_buffer, len_recvd);
    } else {
      if (len_recvd < 0) {
        cout << "Error on recv() call." << endl;
      } else {
        cout << "Server socket closed." << endl;
      }
      close(client_fd);
      return -1;
    }

    // closing the connected socket
    close(client_fd);
    return 0;
}