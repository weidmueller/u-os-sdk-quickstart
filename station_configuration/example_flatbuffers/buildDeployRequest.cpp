// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#include "iostream"

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>
#include <interface.h>

#include "buildDeployRequest.h"

using namespace std;
using namespace weidmueller::ucontrol::devices;

void buildDeployRequest(uint8_t* p_raw_request_buffer){
  
  flatbuffers::FlatBufferBuilder fbb(102400);

  cout << "serialize deploy data into builtDeployRequest" << endl;
  /* put the deploy request into a flatbuffers Vector. */
  auto data = fbb.CreateVector(vData);
  /* create a DeployRequest and add the data */
  DeployRequestBuilder dRbuilder(fbb);
  dRbuilder.add_data(data);
  /* serialize the DeployRequest */
  auto builtDeployRequest = dRbuilder.Finish();

  cout << "create built_request" << endl;
  RequestBuilder request_builder(fbb);
  /* add type and content to a request union, then serialize it */
  request_builder.add_request_type(AnyRequest::DeployRequest);
  request_builder.add_request(builtDeployRequest.Union());
  auto built_request = request_builder.Finish();

  cout << "create built_message" << endl;
  MessageBuilder message_builder(fbb);
  /* add type and content to a message union, then serialize it */
  message_builder.add_message_type(AnyMessage::Request);
  message_builder.add_message(built_request.Union());
  auto built_message = message_builder.Finish();

  /* serialize the entire message*/
  fbb.Finish(built_message);
  
  uint8_t* buf = fbb.GetBufferPointer();
  int buf_size = fbb.GetSize();

  /* -- the uc-slio-service expects the uint32_t message length before the payload, be it JSON or flatbuffer --*/
  
  memcpy(p_raw_request_buffer+4, buf, buf_size);
  uint32_t* pRawSize = (uint32_t*)p_raw_request_buffer;
  *pRawSize = buf_size+4;

  cout << "..done." << endl << endl;
}
