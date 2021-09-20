/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "bindings/dataBuffer.h"

namespace SimulationSlave {

using namespace openpass::type;

bool DataBuffer::Instantiate()
{
    if (!dataBufferBinding)
    {
        return false;
    }
    else if (!implementation)
    {
        implementation = dataBufferBinding->Instantiate();
        if (!implementation)
        {
            return false;
        }
    }
    return true;
}

bool DataBuffer::isInstantiated() const
{
    return implementation != nullptr;
}

void DataBuffer::PutCyclic(const EntityId agentId, const Key &key, const Value &value)
{
    return implementation->PutCyclic(agentId, key, value);
}

void DataBuffer::PutAcyclic(const EntityId agentId, const Key &key, const Acyclic &acyclic)
{
    return implementation->PutAcyclic(agentId, key, acyclic);
}

void DataBuffer::PutStatic(const Key &key, const Value &value, bool persist)
{
    return implementation->PutStatic(key, value, persist);
}

void DataBuffer::ClearRun()
{
    return implementation->ClearRun();
}

void DataBuffer::ClearTimeStep()
{
    return implementation->ClearTimeStep();
}

std::unique_ptr<CyclicResultInterface> DataBuffer::GetCyclic(const std::optional<openpass::type::EntityId> entityId, const Key &key) const
{
    return implementation->GetCyclic(entityId, key);
}

std::unique_ptr<AcyclicResultInterface> DataBuffer::GetAcyclic(const std::optional<openpass::type::EntityId> entityId, const Key &key) const
{
    return implementation->GetAcyclic(entityId, key);
}

Values DataBuffer::GetStatic(const Key &key) const
{
    return implementation->GetStatic(key);
}

Keys DataBuffer::GetKeys(const Key &key) const
{
    return implementation->GetKeys(key);
}

} // namespace SimulationSlave
