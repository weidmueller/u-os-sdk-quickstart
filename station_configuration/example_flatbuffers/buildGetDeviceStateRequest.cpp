// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#include "iostream"

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>
#include <interface.h>

using namespace std;
using namespace weidmueller::ucontrol::devices;

void buildGetDeviceStateRequest(uint8_t* p_raw_request_buffer){
  /* create a flatbuffer with the following message */
  /*
    #message = '{
                  message_type: "Request",
                  message: {
                            request_type: "GetDeviceStateRequest",
                            request: {
                                      name: "slio/3"
                                      }
                            }
                }'
  */

  flatbuffers::FlatBufferBuilder fbb(1024);

  /* create name, add it to a 'get device state request' builder and serialize. */
  auto name = fbb.CreateString("slio/2");
  GetDeviceStateRequestBuilder builder(fbb);
  builder.add_name(name);
  auto gdsrGetDeviceStateRequest = builder.Finish();

  /* add the 'get device state' request and the request's type to an AnyRequest
     and serialize */
  RequestBuilder request_builder(fbb);
  request_builder.add_request_type(AnyRequest::GetDeviceStateRequest);
  request_builder.add_request(gdsrGetDeviceStateRequest.Union());
  auto built_request = request_builder.Finish();

  /* add the message type and the request to an AnyMessage and serialize */
  MessageBuilder message_builder(fbb);
  message_builder.add_message_type(AnyMessage::Request);
  message_builder.add_message(built_request.Union());
  auto built_message = message_builder.Finish();

  fbb.Finish(built_message);
  
  uint8_t* buf = fbb.GetBufferPointer();
  int buf_size = fbb.GetSize();

  /* the uc-slio-service expects the uint32_t message length before the flat-
  buffer payload */
    memcpy(p_raw_request_buffer+4, buf, buf_size);
  uint32_t* pRawSize = (uint32_t*)p_raw_request_buffer;
  *pRawSize = buf_size+4;
}
