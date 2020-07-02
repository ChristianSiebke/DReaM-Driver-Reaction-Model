/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  dataStore.h
//! @brief This file provides the interface to a data store implementation
//-----------------------------------------------------------------------------

#pragma once

#include "Interfaces/dataStoreInterface.h"
#include "dataStoreBinding.h"

namespace SimulationSlave {

class DataStore : public DataStoreInterface
{
public:
    DataStore(DataStoreBinding *dataStoreBinding) :
        dataStoreBinding(dataStoreBinding)
    {
    }

    DataStore(const DataStore &) = delete;
    DataStore(DataStore &&) = delete;
    DataStore &operator=(const DataStore &) = delete;
    DataStore &operator=(DataStore &&) = delete;
    ~DataStore() override = default;

    bool Instantiate() override;
    bool isInstantiated() const override;

    void PutCyclic(const openpass::type::Timestamp time, const openpass::type::EntityId entityId, const Key &key, const Value &value) override;
    void PutAcyclic(const openpass::type::Timestamp time, const openpass::type::EntityId entityId, const Key &key, const Acyclic &acyclic) override;
    void PutStatic(const Key &key, const Value &value, bool persist) override;

    void Clear() override;

    std::unique_ptr<CyclicResultInterface> GetCyclic(const std::optional<openpass::type::Timestamp> time, const std::optional<openpass::type::EntityId> entityId, const Key &key) const override;
    std::unique_ptr<AcyclicResultInterface> GetAcyclic(const std::optional<openpass::type::Timestamp> time, const std::optional<openpass::type::EntityId> entityId, const Key &key) const override;
    Values GetStatic(const Key &key) const override;
    Keys GetKeys(const Key &key) const override;

private:
    DataStoreBinding *dataStoreBinding = nullptr;
    DataStoreInterface *implementation = nullptr;
};

} // namespace SimulationSlave
