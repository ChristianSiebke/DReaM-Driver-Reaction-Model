/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/publisherInterface.h"

class FakeAgentDataPublisher : public PublisherInterface
{
public:
    MOCK_METHOD2(Publish, void(const openpass::databuffer::Key&, const openpass::databuffer::Value&));
};
