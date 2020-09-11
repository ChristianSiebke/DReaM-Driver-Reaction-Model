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

#include "include/dataStoreInterface.h"

class FakeDataStore : public DataStoreInterface
{
public:
    MOCK_CONST_METHOD3(GetCyclic, std::unique_ptr<CyclicResultInterface>(const std::optional<openpass::type::Timestamp> time, const std::optional<openpass::type::EntityId> entityId, const Key& key));
    MOCK_CONST_METHOD3(GetAcyclic, std::unique_ptr<AcyclicResultInterface>(const std::optional<openpass::type::Timestamp> time, const std::optional<openpass::type::EntityId> entityId, const Key& key));
    MOCK_CONST_METHOD1(GetStatic, Values(const Key& key));
    MOCK_CONST_METHOD1(GetKeys, Keys(const Key& key));
    MOCK_METHOD4(PutCyclic, void(const openpass::type::Timestamp time, const openpass::type::EntityId entityId, const Key& key, const Value& value));
    MOCK_METHOD4(PutAcyclic, void(const openpass::type::Timestamp time, const openpass::type::EntityId entityId, const Key& key, const Acyclic& event));
    MOCK_METHOD3(PutStatic, void(const Key& key, const Value& value, bool persist));
    MOCK_METHOD0(Clear, void());
};
