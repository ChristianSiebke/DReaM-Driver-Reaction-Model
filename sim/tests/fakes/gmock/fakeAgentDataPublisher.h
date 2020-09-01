/*********************************************************************
 * Copyright (c) 2020 in-tech
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 **********************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/publisherInterface.h"

class FakeAgentDataPublisher : public PublisherInterface
{
public:
    MOCK_METHOD2(Publish, void(const openpass::datastore::Key&, const openpass::datastore::Value&));
};
