/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/*!
* \file  basicDataStoreImplementation.h
* 
* \brief Stores data sent by publishers and provides acces to this data for observers
*/

#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "Common/commonTools.h"
#include "Common/runtimeInformation.h"
#include "Interfaces/dataStoreInterface.h"

using namespace openpass::datastore;

using Persistence = bool;
using StaticStore = std::unordered_map<Key, std::tuple<Value, Persistence>>;
using CyclicStore = std::vector<CyclicRow>;
using AcyclicStore = std::vector<AcyclicRow>;

template <typename... Ts>
using StoreIndex = std::multimap<std::tuple<Ts...>, size_t>;

class CyclicResult : public CyclicResultInterface
{
public:
    CyclicResult(const CyclicStore& store, const CyclicRowRefs& elements);
    virtual ~CyclicResult() override = default;

    size_t size() const override;
    const CyclicRow& at(const size_t index) const override;
    CyclicRowRefs::const_iterator begin() const override;
    CyclicRowRefs::const_iterator end() const override;

private:
    const CyclicStore& store;       //!< Reference to the cyclic datastore associated with the result set
    const CyclicRowRefs elements;   //!< The result set (referencing elements in a datastore)
};

class AcyclicResult : public AcyclicResultInterface
{
public:
    AcyclicResult(const AcyclicStore& store, const AcyclicRowRefs& elements);
    virtual ~AcyclicResult() override = default;

    size_t size() const override;
    const AcyclicRow& at(const size_t index) const override;
    AcyclicRowRefs::const_iterator begin() const override;
    AcyclicRowRefs::const_iterator end() const override;

private:
    const AcyclicStore& store;       //!< Reference to the acyclic datastore associated with the result set
    const AcyclicRowRefs elements;   //!< The result set (referencing elements in a datastore)
};

/*!
 * \brief This class implements a basic version of a data store
 *
 * Data is stored in a simple mapping usign the timestamp and agent id as keys,
 * values are stored as key/value pairs.
 */
class BasicDataStoreImplementation : public DataStoreInterface
{
public:
    const std::string COMPONENTNAME = "BasicDataStore";

    BasicDataStoreImplementation(const openpass::common::RuntimeInformation *runtimeInformation, const CallbackInterface *callbacks);
    BasicDataStoreImplementation(const BasicDataStoreImplementation &) = delete;
    BasicDataStoreImplementation(BasicDataStoreImplementation &&) = delete;
    BasicDataStoreImplementation &operator=(const BasicDataStoreImplementation &) = delete;
    BasicDataStoreImplementation &operator=(BasicDataStoreImplementation &&) = delete;
    virtual ~BasicDataStoreImplementation() override = default;

    void PutCyclic(const openpass::type::Timestamp time, const openpass::type::EntityId entityId, const Key &key, const Value &value) override;

    void PutAcyclic(const openpass::type::Timestamp time, const openpass::type::EntityId entityId, const Key &key, const openpass::datastore::Acyclic &acyclic) override;

    void PutStatic(const Key &key, const Value &value, bool persist = false) override;

    void Clear() override;

    std::unique_ptr<CyclicResultInterface> GetCyclic(const std::optional<openpass::type::Timestamp> time, const std::optional<openpass::type::EntityId> entityId, const Key &key) const override;

    std::unique_ptr<AcyclicResultInterface> GetAcyclic(const std::optional<openpass::type::Timestamp> time, const std::optional<openpass::type::EntityId> entityId, const Key &key) const override;

    Values GetStatic(const Key &key) const override;

    Keys GetKeys(const Key &key) const override;

protected:
    StaticStore staticStore;     //!< Container for DataStore static values
    CyclicStore cyclicStore;     //!< Container for DataStore cyclic values
    AcyclicStore acyclicStore;   //!< Container for DataStore acyclic values

    StoreIndex<openpass::type::Timestamp> timestampIndex;                                   //!< Index for timestamp based cyclics access
    StoreIndex<openpass::type::EntityId> entityIdIndex;                                     //!< Index for entity id based cyclics access
    StoreIndex<openpass::type::Timestamp, openpass::type::EntityId> timeAndEntityIdIndex;   //!< Index for (time, entity) based cyclics access

private:
    template <typename... Ts>
    std::unique_ptr<CyclicResultInterface> GetIndexed(const std::optional<openpass::type::Timestamp> time, const std::optional<openpass::type::EntityId> entityId, const Tokens &tokens) const;

    std::unique_ptr<CyclicResultInterface> GetCyclic(const Key& key) const;
    std::unique_ptr<AcyclicResultInterface> GetAcyclic(const Key& key) const;
    CyclicRows GetStatic(const Tokens &tokens) const;
    Keys GetStaticKeys(const Tokens &tokens) const;
};
