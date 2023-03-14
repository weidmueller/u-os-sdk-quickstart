// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#include "iostream"

#include "prepareRequestFromJSON.h"
#include "buildGetDeviceStateRequest.h"
#include "buildDeployRequest.h"
#include "transceive.h"
#include "responseToJSON.h"
#include "responseToAccessors.h"

#include "requestsJSON.h"

/**
 * \mainpage Introduction
 * 
 * This example shows the interaction with the UOS station configuration service.
 * It is intended for C/C++ linux application programmers.
 * 
 * Applications send requests that are coded either as JSON or in a flatbuffers
 * binary blob to the uc-slio-service via a Unix socket. @n
 * The service responds in the same format as the request, or with a JSON-encoded
 * 'invalid request' response. @n 
 * The 'devices' mentionend in the request descriptions are the UR20 modules
 * attached to the PLC.
 * 
 * Available requests are:
 * 
 * - get the uc-slio-service version
 * - list the devices attached to the PLC
 * - list the states of the devices
 * - deploy a complete station configuration
 * - get device diagnostic alarms
 * - acknowledge device diagnostic alarms
 * - get device process alarms
 * - acknowledge device process alarms
 * - get device parameters
 * - set device parameters
 *
 * Create a flatbuffer binary with flatbuffers builders:
 * - simple example in buildGetDeviceStateRequest.h
 * - less simple example in buildDeployRequest.h
 * 
 * Interpret a flatbuffers binary:
 * - fetch specific content with accessors in responseToAccessors.h
 * 
 * The author expects that most users want to use accessors and builders to get / set flatbuffer content.
 * Just for completeness, these two examples show @n how to parse JSON to flatbuffer binary and vice versa:
 * - parse JSON to flatbuffer binary: prepareRequestFromJSON.h
 * - a list of the available requests: requestsJSON.h NB: the deploy request is incomplete and will provoke
 * an invalid response. Use buildDeployRequest(), instead.
 * - parse a flatbuffer to JSON: responseToJSON.h
 
 * 
 * main.cpp shows how to put the pieces together.
 */

/**
 * @file main.cpp
 * @author w010174
 * @date March 8th 2023
 * @brief A code example for UR20 PLC station configuration.
 *
 * This example will demonstrate requests in flatbuffers binary format, only.
 * It will show how to parse JSON requests into a flatbuffers binary and how to
 *  build a flatbuffers binary from scratch with flatbuffers builders.
 * 
 * For requests sent in JSON format, refer to 
 * station_configuration/example_JSON/unix_sock.py.
 * 
 * WI recommends to read the flatbuffers tutorial available online at:
 * https://google.github.io/flatbuffers/flatbuffers_guide_tutorial.html
 * 

 * 
 */

using namespace std;

/**
 * @brief request in JSON format.
 *
 * Please find the available requests in include/requestsJSON.h. The example
 * will convert them to flatbuffers binary; They are in JSON format for better
 * human readability.
 */
extern std::string req_json;

/**
 * @brief This app demonstrates flatbuffers- and JSON-driven station configuration services.
 * 
 * The app's structure consists of three parts:
 * - prepare the request
 * - send the request and receive the response
 * - interpret the response
 * 
 * The code shows how to compile alternatives for preparation and interpretation 
 * into the app.
 * 
 */
int main(void){

    char raw_request_buffer[102500];
    char raw_response_buffer[1025];

    cout << endl << "This is the flatbuffers station configuration example." << endl << endl;


    /* --- prepare request --- */
    /*
        This part offers two alternatives:
        - parse a JSON string into a flatbuffers request in JSONtoRequest()
        - build a flatbuffers binary with flatbuffers builders, see 
            buildGetDeviceStateRequest() and buildDeployRequest()
        Please comment in / out the desired code and run the example.
        NB: use only _one_ alternative to create a uc-slio-service message at a time!
    */
    cout << "build request.." << endl;
    //cout << "The JSON request string is: " << endl << req_json << endl;
    int request_len = JSONtoRequest((uint8_t*)raw_request_buffer, req_json);
    //buildGetDeviceStateRequest((uint8_t*)raw_request_buffer);
    //buildDeployRequest((uint8_t*) raw_request_buffer);


    /* --- open socket, send request, receive response, close socket --- */
    cout << "..transceiving.." << endl << endl;
    transceive((uint8_t*)raw_request_buffer, (uint8_t*)raw_response_buffer, sizeof(raw_request_buffer));


    /* --- interpret response --- */
    /*
        This part offers two alternatives, too:
        - parse the received flatbuffer binary into JSON and print it
        - use flatbuffers accessors to read the field values contained in 
            the flatbuffers binary and print them
    */
    cout << "Convert the response to JSON: " << endl;
    fbResponsetoJSON((uint8_t*) raw_response_buffer);

    cout << "Now read the response content with accessor methods." << endl << endl;
    fbResponseToAccessors((uint8_t*) raw_response_buffer);

    return(0);
}
