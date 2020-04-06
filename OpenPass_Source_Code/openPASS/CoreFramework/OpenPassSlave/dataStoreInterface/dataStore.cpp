/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "dataStoreInterface/dataStore.h"

namespace SimulationSlave {

using namespace openpass::type;

bool DataStore::Instantiate()
{
    if (!dataStoreBinding)
    {
        return false;
    }
    else if (!implementation)
    {
        implementation = dataStoreBinding->Instantiate();
        if (!implementation)
        {
            return false;
        }
    }
    return true;
}

bool DataStore::isInstantiated() const
{
    return implementation != nullptr;
}

void DataStore::PutCyclic(const Timestamp time, const EntityId agentId, const Key &key, const Value &value)
{
    return implementation->PutCyclic(time, agentId, key, value);
}

void DataStore::PutAcyclic(const Timestamp time, const EntityId agentId, const Key &key, const Acyclic &acyclic)
{
    return implementation->PutAcyclic(time, agentId, key, acyclic);
}

void DataStore::PutStatic(const Key &key, const Value &value, bool persist)
{
    return implementation->PutStatic(key, value, persist);
}

void DataStore::Clear()
{
    return implementation->Clear();
}

std::unique_ptr<CyclicResultInterface> DataStore::GetCyclic(const std::optional<openpass::type::Timestamp> time, const std::optional<openpass::type::EntityId> entityId, const Key &key) const
{
    return implementation->GetCyclic(time, entityId, key);
}

std::unique_ptr<AcyclicResultInterface> DataStore::GetAcyclic(const std::optional<openpass::type::Timestamp> time, const std::optional<openpass::type::EntityId> entityId, const Key &key) const
{
    return implementation->GetAcyclic(time, entityId, key);
}

Values DataStore::GetStatic(const Key &key) const
{
    return implementation->GetStatic(key);
}

Keys DataStore::GetKeys(const Key &key) const
{
    return implementation->GetKeys(key);
}

} // namespace SimulationSlave
