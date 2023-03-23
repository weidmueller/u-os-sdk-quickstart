// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#ifndef REQUESTSJSON_H_
#define REQUESTSJSON_H_

#include "iostream"

//#define REQUEST_GETDEVICEPARAMETER 1
//#define REQUEST_GETVERSION 1
//#define REQUEST_LISTDEVICESTATES 1
//#define REQUEST_LISTDEVICES 1
#define REQUEST_GETDEVICEDIAGALARM 1
//#define REQUEST_ACKDEVICEDIAGALARM 1
//#define REQUEST_GETDEVICEPROCALARM 1
//#define REQUEST_ACKDEVICEPROCALARM 1
//#define REQUEST_GETDEVICEPARAMETER 1
//#define REQUEST_SETDEVICEPARAMETER 1
//#define REQUEST_DEPLOY 1 
/*
 * NB: JSON deploy request is incomplete and will result in an error response.
 * Please use buildDeployRequest() for a correct deploy request, instead.
 */

#if REQUEST_GETVERSION
    std::string req_json = "{\n"
        "  message_type: \"Request\",\n"
        "  message: {\n"
        "    request_type: \"GetVersionRequest\",\n"
        "  }\n"
        "}\n";

#elif REQUEST_LISTDEVICESTATES
    std::string req_json = "{\n"
        "  message_type: \"Request\",\n"
        "  message: {\n"
        "    request_type: \"ListDeviceStatesRequest\",\n"
        "    request: {\n"
        "               name: \"slio\"\n"
        "             }\n"
        "  }\n"
        "}\n";

#elif REQUEST_LISTDEVICES
    std::string req_json = "{\n"
        "  message_type: \"Request\",\n"
        "  message: {\n"
        "    request_type: \"ListDevicesRequest\",\n"
        "    request: {\n"
        "               name: \"slio\"\n"
        "             }\n"
        "  }\n"
        "}\n";

#elif REQUEST_GETDEVICEDIAGALARM
    std::string req_json = "{\n"
        "  message_type: \"Request\",\n"
        "  message: {\n"
        "    request_type: \"GetDeviceDiagAlarmRequest\",\n"
        "    request: {\n"
        "               name: \"slio/2\"\n"
        "             }\n"
        "  }\n"
        "}\n";

#elif REQUEST_ACKDEVICEDIAGALARM
    std::string req_json = "{\n"
        "  message_type: \"Request\",\n"
        "  message: {\n"
        "    request_type: \"AckDeviceDiagAlarmRequest\",\n"
        "    request: {\n"
        "               name: \"slio/2\"\n"
        "             }\n"
        "  }\n"
        "}\n";

#elif REQUEST_GETDEVICEPROCALARM
    std::string req_json = "{\n"
        "  message_type: \"Request\",\n"
        "  message: {\n"
        "    request_type: \"GetDeviceProcAlarmRequest\",\n"
        "    request: {\n"
        "               name: \"slio/3\"\n"
        "             }\n"
        "  }\n"
        "}\n";

#elif REQUEST_ACKDEVICEPROCALARM
    std::string req_json = "{\n"
        "  message_type: \"Request\",\n"
        "  message: {\n"
        "    request_type: \"AckDeviceProcAlarmRequest\",\n"
        "    request: {\n"
        "               name: \"slio/3\"\n"
        "             }\n"
        "  }\n"
        "}\n";

#elif REQUEST_GETDEVICEPARAMETER
    std::string req_json = "{\n"
        "  message_type: \"Request\",\n"
        "  message: {\n"
        "    request_type: \"GetDeviceParameterRequest\",\n"
        "    request: {\n"
        "               name: \"slio/2\"\n"
        "             }\n"
        "  }\n"
        "}\n";

#elif REQUEST_SETDEVICEPARAMETER
    std::string req_json = "{\n"
        "  message_type: \"Request\",\n"
        "  message: {\n"
        "    request_type: \"SetDeviceParameterRequest\",\n"
        "    request: {\n"
        "               name: \"slio/2\",\n"
        "               data: [1, 0, 15, 0, 255, 255, 255, 255]\n"
        "             }\n"
        "  }\n"
        "}\n";

#elif REQUEST_DEPLOY
    std::string req_json = "{message_type: \"Request\", message: {request_type: \"DeployRequest\",request: {data: []}}}\n";
 
#else
    #error "Error: Please choose one REQUEST_<requestname> - macro."
#endif

#endif