/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "nfd-controller.hpp"
#include "nfd-control-response.hpp"

namespace ndn {
namespace nfd {

Controller::Controller(Face& face)
  : m_face(face)
{
}

void
Controller::processCommandResponse(const Data& data,
                                   const shared_ptr<ControlCommand>& command,
                                   const CommandSucceedCallback& onSuccess,
                                   const CommandFailCallback& onFailure)
{
  /// \todo verify Data signature

  const uint32_t serverErrorCode = 500;

  ControlResponse response;
  try {
    response.wireDecode(data.getContent().blockFromValue());
  }
  catch (ndn::Tlv::Error& e) {
    if (static_cast<bool>(onFailure))
      onFailure(serverErrorCode, e.what());
    return;
  }

  uint32_t code = response.getCode();
  const uint32_t errorCodeLowerBound = 400;
  if (code >= errorCodeLowerBound) {
    if (static_cast<bool>(onFailure))
      onFailure(code, response.getText());
    return;
  }

  ControlParameters parameters;
  try {
    parameters.wireDecode(response.getBody());
  }
  catch (ndn::Tlv::Error& e) {
    if (static_cast<bool>(onFailure))
      onFailure(serverErrorCode, e.what());
    return;
  }

  try {
    command->validateResponse(parameters);
  }
  catch (ControlCommand::ArgumentError& e) {
    if (static_cast<bool>(onFailure))
      onFailure(serverErrorCode, e.what());
    return;
  }

  onSuccess(parameters);
}


void
Controller::selfRegisterPrefix(const Name& prefixToRegister,
                               const SuccessCallback& onSuccess,
                               const FailCallback& onFail)
{
  const uint32_t selfFaceId = 0;

  ControlParameters parameters;
  parameters.setName(prefixToRegister)
            .setFaceId(selfFaceId);

  this->start<FibAddNextHopCommand>(parameters,
                                    bind(onSuccess),
                                    bind(onFail, _2));
}

void
Controller::selfDeregisterPrefix(const Name& prefixToDeRegister,
                                 const SuccessCallback& onSuccess,
                                 const FailCallback& onFail)
{
  const uint32_t selfFaceId = 0;

  ControlParameters parameters;
  parameters.setName(prefixToDeRegister)
            .setFaceId(selfFaceId);

  this->start<FibRemoveNextHopCommand>(parameters,
                                       bind(onSuccess),
                                       bind(onFail, _2));
}

} // namespace nfd
} // namespace ndn
