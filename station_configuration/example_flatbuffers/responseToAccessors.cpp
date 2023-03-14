// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#include "iostream"

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>
#include <interface.h>

#include "printByteVect.h"

using namespace std;
using namespace weidmueller::ucontrol::devices;

int fbResponseToAccessors(uint8_t* pRawResponseBuffer){

    uint32_t* pRawSize;

    /* extract the actual message size from the response buffer and check the size and message payload. */
    pRawSize = (uint32_t*)pRawResponseBuffer;

    if (pRawSize == 0){
        cout << "Error: response buffer is empty." << endl;
        return -1;
    }
    auto myVeri = flatbuffers::Verifier((uint8_t*)(pRawResponseBuffer+4), *pRawSize-4);

    if(!VerifyMessageBuffer(myVeri)) {
        cout << "Error: response buffer contains invalid message." << endl;
        return -1;
    };

    /* extract the AnyResponse content from the message */
    auto message = GetMessage((uint8_t*)(pRawResponseBuffer+4));

    auto myResponse = message->message_as_Response();

    auto respType = myResponse->response_type();

    cout << "response type: " << EnumNameAnyResponse(respType) << endl << endl;

    /* interpret the AnyResponse content depending on the response type */
    switch (respType) {
        case AnyResponse::NONE:
            cout << "Error: cannot process response of type NONE." << endl;
            return -1;
            break;

        case AnyResponse::GetVersionResponse:
            {
                auto VersionResponse = myResponse->response_as_GetVersionResponse();
                cout << "The version is: " << VersionResponse->version()->str() << endl;
            }
            break;

        case AnyResponse::DeployResponse:
            cout << "The DeployResponse is intentionally empty." << endl;
            break;

        case AnyResponse::GetDeviceStateResponse:
            {
                auto DeviceStateResponse = myResponse->response_as_GetDeviceStateResponse();
                cout << "The device state is: " << DeviceStateResponse->state() << endl;
                break;
            }

        case AnyResponse::ListDeviceStatesResponse:
        {
            auto ListDeviceStatesResponse = myResponse->response_as_ListDeviceStatesResponse();
            auto deviceStates = ListDeviceStatesResponse->states();
            cout << "The states are: " << endl;
            for(const auto& value: *deviceStates) {
                std::cout << value << " ";
            }
            cout << endl;
            break;
        }
        case AnyResponse::ListDevicesResponse:
        {
            auto listDevicesResponse = myResponse->response_as_ListDevicesResponse();
            auto devicesList = listDevicesResponse->devices();
            for (const auto& device: *devicesList){
                cout << "slot: " << dec << unsigned(device->slot()) << endl;
                cout << "   state: " << device->state() << endl;
                cout << "   id: " << dec << device->id() << endl;
                cout << "   name: " << device->name()->str() << endl;
                cout << "   serial no.: " << device->serial_number()->str() << endl;
                cout << "   order no.: " << device->order_number()->str() << endl;
                cout << "   hw version: " << device->hardware_version()->str() << endl;
                cout << "   sw version: " << device->software_version()->str() << endl;
                cout << "   fpga version: " << device->fpga_version()->str() << endl;
                cout << "   eeprom version: " << device->eeprom_version()->str() << endl;
                cout << "   calibration date: " << device->calibration_date()->str() << endl;
                cout << endl;
            }
            break;
        }

        case AnyResponse::GetDeviceDiagAlarmResponse:
            {
                auto diagAlarmsResponse = myResponse->response_as_GetDeviceDiagAlarmResponse();
                auto pDiagData = diagAlarmsResponse->data();
                PrintByteVect(pDiagData);
                break;
            }

        case AnyResponse::AckDeviceDiagAlarmResponse:
            {
                cout << "The AckDeviceDiagAlarmResponse is intentionally empty." << endl;
                break;
            }

        case AnyResponse::GetDeviceProcAlarmResponse:
            {
                auto deviceProcAlarmResp = myResponse->response_as_GetDeviceProcAlarmResponse();
                auto procAlarmData = deviceProcAlarmResp->data();
                PrintByteVect(procAlarmData);
                break;
            }

        case AnyResponse::AckDeviceProcAlarmResponse:
            {
                cout << "The AckDeviceProcAlarmResponse is intentionally empty." << endl;
                break;
            }

        case AnyResponse::SetDeviceParameterResponse:
            {
                cout << "The SetDeviceParameterResponse is intentionally empty." << endl;
                break;
            }

        case AnyResponse::GetDeviceParameterResponse:
            {
                auto deviceParameters = myResponse->response_as_GetDeviceParameterResponse();
                auto pVectData = deviceParameters->data();
                PrintByteVect(pVectData);
                break;
            }
    }

    return(0);
}