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

#include "fakeDataStore.h"
using ::testing::NiceMock;

class FakePublisher : public PublisherInterface
{
public:
    NiceMock<FakeDataStore> fakeDataStore;
    FakePublisher() : PublisherInterface(&fakeDataStore) {}

    MOCK_METHOD2(Publish,
                 void(const openpass::datastore::Key &key, const openpass::datastore::Value &value));
    MOCK_METHOD1(Publish,
                 void(const openpass::publisher::LogEntryBase &event));
    MOCK_METHOD2(Publish,
                 void(const openpass::datastore::Key &key, const openpass::datastore::ComponentEvent &event));
};
