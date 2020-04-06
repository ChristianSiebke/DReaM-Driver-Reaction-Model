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
/** \file  basicDataStoreImplementation.cpp */
//-----------------------------------------------------------------------------

#include "basicDataStoreImplementation.h"

#include <memory>
#include <utility>

#include "Common/commonTools.h"
#include "Common/openPassUtils.h"
#include "Interfaces/parameterInterface.h"
#include "Interfaces/stochasticsInterface.h"
#include "Interfaces/worldInterface.h"

using namespace openpass::type;

/*!
 * \brief Tests, if two vectors of tokens have matching elements
 *
 * The test result will be true, if each token A in the matcher, the token B at the same position (index) in the reference are string equal.
 * A matcher token containing \c openpass::datastore::WILDCARD ('*') will match every string in the reference.
 * The test result will always be false, if the matcher contains more elements than the reference.
 *
 * \param[in]    matcher     Tokens to test against reference
 * \param[out]   reference   A full stack of tokens to match against (i. e. a 'path' of DataStore keys in the DataStore hierarchy)
 *
 * \return True if the token vectores are considered to be matching, false otherwise.
 */
static bool TokensMatch(const Tokens& matcher, const Tokens& reference)
{
    if (matcher.size() > reference.size())
    {
        return false;
    }

    for (size_t pos = 0; pos < matcher.size(); ++pos)
    {
        if (matcher[pos] != WILDCARD && matcher[pos] != reference[pos])
        {
            return false;
        }
    }

    return true;
}

CyclicResult::CyclicResult(const CyclicStore& store, const CyclicRowRefs& elements) :
    store{store},
    elements{elements}
{
}

size_t CyclicResult::size() const
{
    return elements.size();
}

const CyclicRow& CyclicResult::at(const size_t index) const
{
    return elements.at(index);
}

CyclicRowRefs::const_iterator CyclicResult::begin() const
{
    return elements.cbegin();
}

CyclicRowRefs::const_iterator CyclicResult::end() const
{
    return elements.cend();
}

AcyclicResult::AcyclicResult(const AcyclicStore& store, const AcyclicRowRefs& elements) :
    store{store},
    elements{elements}
{
}

size_t AcyclicResult::size() const
{
    return elements.size();
}

const AcyclicRow& AcyclicResult::at(const size_t index) const
{
    return elements.at(index);
}

AcyclicRowRefs::const_iterator AcyclicResult::begin() const
{
    return elements.cbegin();
}

AcyclicRowRefs::const_iterator AcyclicResult::end() const
{
    return elements.cend();
}

BasicDataStoreImplementation::BasicDataStoreImplementation(const openpass::common::RuntimeInformation *runtimeInformation, const CallbackInterface *callbacks) :
    DataStoreInterface(runtimeInformation, callbacks)
{
}

template <>
std::unique_ptr<CyclicResultInterface> BasicDataStoreImplementation::GetIndexed<EntityId>([[maybe_unused]] const std::optional<Timestamp> time, const std::optional<EntityId> entityId, const Tokens &tokens) const
{
    CyclicRowRefs rowRefs;

    auto range = entityIdIndex.equal_range(entityId.value());

    for (auto it = range.first; it != range.second; ++it)
    {
        const auto& storeValue = cyclicStore.at(it->second);

        if (TokensMatch(tokens, Tokens{storeValue.key}))
        {
            rowRefs.emplace_back(storeValue);
        }
    }

    return std::make_unique<CyclicResult>(cyclicStore, rowRefs);
}

template <>
std::unique_ptr<CyclicResultInterface> BasicDataStoreImplementation::GetIndexed<Timestamp>(const std::optional<Timestamp> time, [[maybe_unused]] const std::optional<EntityId> entityId, const Tokens &tokens) const
{
    CyclicRowRefs rowRefs;

    auto range = timestampIndex.equal_range(time.value());

    for (auto it = range.first; it != range.second; ++it)
    {
        const auto& storeValue = cyclicStore.at(it->second);

        if (TokensMatch(tokens, CommonHelper::TokenizeString(storeValue.key, SEPARATOR[0])))
        {
            rowRefs.emplace_back(storeValue);
        }
    }

    return std::make_unique<CyclicResult>(cyclicStore, rowRefs);
}

template <>
std::unique_ptr<CyclicResultInterface> BasicDataStoreImplementation::GetIndexed<Timestamp, EntityId>(const std::optional<Timestamp> time, const std::optional<EntityId> entityId, const Tokens &tokens) const
{
    std::vector<size_t> filteredIndexes;
    CyclicRowRefs rowRefs;

    auto range = timeAndEntityIdIndex.equal_range(std::make_tuple(time.value(), entityId.value()));

    for (auto it = range.first; it != range.second; ++it)
    {
        const auto& storeValue = cyclicStore.at(it->second);

        if (TokensMatch(tokens, CommonHelper::TokenizeString(storeValue.key, SEPARATOR[0])))
        {
            rowRefs.emplace_back(storeValue);
        }
    }

    return std::make_unique<CyclicResult>(cyclicStore, rowRefs);
}

std::unique_ptr<CyclicResultInterface> BasicDataStoreImplementation::GetCyclic(const Key& key) const
{
    const Tokens tokens = CommonHelper::TokenizeString(key, SEPARATOR[0]);
    CyclicRowRefs rowRefs;

    for (const auto& storeValue : cyclicStore)
    {
        if (TokensMatch(tokens, CommonHelper::TokenizeString(storeValue.key, SEPARATOR[0])))
        {
            rowRefs.emplace_back(storeValue);
        }
    }

    return std::make_unique<CyclicResult>(cyclicStore, rowRefs);
}

std::unique_ptr<CyclicResultInterface> BasicDataStoreImplementation::GetCyclic(const std::optional<Timestamp> time, const std::optional<EntityId> entityId, const Key &key) const
{
    const Tokens tokens = CommonHelper::TokenizeString(key, SEPARATOR[0]);

    if (!time.has_value() && entityId.has_value())
    {
        return GetIndexed<EntityId>(time, entityId, tokens);
    }
    else if (time.has_value() && !entityId.has_value())
    {
        return GetIndexed<Timestamp>(time, entityId, tokens);
    }
    else if (time.has_value() && entityId.has_value())
    {
        return GetIndexed<Timestamp, EntityId>(time, entityId, tokens);
    }
    else
    {
        return GetCyclic(key);
    }
}

void BasicDataStoreImplementation::PutCyclic(const Timestamp time, const EntityId agentId, const Key &key, const Value &value)
{
    cyclicStore.emplace_back(time, agentId, key, value);

    size_t newValueIndex = cyclicStore.size() - 1;
    timestampIndex.emplace(time, newValueIndex);
    entityIdIndex.emplace(agentId, newValueIndex);
    timeAndEntityIdIndex.emplace(std::tuple(time, agentId), newValueIndex);
}

void BasicDataStoreImplementation::PutAcyclic(const Timestamp time, const EntityId entityId, const Key &key, const openpass::datastore::Acyclic&acyclic)
{
    acyclicStore.emplace_back(time, entityId, key, acyclic);
}

void BasicDataStoreImplementation::PutStatic(const Key &key, const Value &value, bool persist)
{
    if (staticStore.find(key) == staticStore.end())
    {
        staticStore[key] = {value, persist};
    }
}

void BasicDataStoreImplementation::Clear()
{
    cyclicStore.clear();
    entityIdIndex.clear();
    timestampIndex.clear();
    timeAndEntityIdIndex.clear();

    acyclicStore.clear();

    auto it = staticStore.begin();

    while (it != staticStore.end())
    {
        if (std::get<Persistence>(it->second) == false)
        {
            it = staticStore.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::unique_ptr<AcyclicResultInterface> BasicDataStoreImplementation::GetAcyclic(const Key& key) const
{
    const Tokens tokens = CommonHelper::TokenizeString(key, SEPARATOR[0]);

    AcyclicRowRefs rowRefs;

    for (const auto& storeValue : acyclicStore)
    {
        if (TokensMatch(tokens, CommonHelper::TokenizeString(storeValue.key, SEPARATOR[0])))
        {
            rowRefs.emplace_back(storeValue);
        }
    }

    return std::make_unique<AcyclicResult>(acyclicStore, rowRefs);
}

std::unique_ptr<AcyclicResultInterface> BasicDataStoreImplementation::GetAcyclic([[maybe_unused]] const std::optional<Timestamp> time, [[maybe_unused]] const std::optional<EntityId> entityId, const Key &key) const
{
    if (key == WILDCARD)
    {
        AcyclicRowRefs rowRefs;

        for (const auto& storeValue : acyclicStore)
        {
            rowRefs.emplace_back(storeValue);
        }

        return std::make_unique<AcyclicResult>(acyclicStore, rowRefs);
    }
    else
    {
        return GetAcyclic(key);
    }
}

Values BasicDataStoreImplementation::GetStatic(const Key &key) const
{
    try
    {
        // currently, the result is always a vector containing only one element
        return {std::get<Value>(staticStore.at(key))};
    }
    catch (const std::out_of_range&)
    {
        return {};
    }
}

Keys BasicDataStoreImplementation::GetKeys(const Key &key) const
{
    const Tokens tokens = CommonHelper::TokenizeString(key, SEPARATOR[0]);

    if (tokens.at(0) == "Cyclics")
    {
        if (tokens.size() == 1)
        {
            Keys keys;
            Timestamp lastTimestamp{-1};

            auto it = timestampIndex.cbegin();

            while (it != timestampIndex.cend())
            {
                lastTimestamp = std::get<0>(it->first);
                keys.push_back(std::to_string(lastTimestamp));
                it = timestampIndex.upper_bound(lastTimestamp);
            }

            return keys;
        }
        else if (tokens.size() >= 3)
        {
            std::set<Key> result;
            Tokens searchKeyTokens{tokens.cbegin() + 3, tokens.cend()};

            const auto entries = GetIndexed<Timestamp, EntityId>(std::stod(tokens.at(1)), std::stod(tokens.at(2)), searchKeyTokens);

            for (const auto& entry : *entries)
            {
                const auto& row = entry.get();
                const Tokens storedKeyTokens = CommonHelper::TokenizeString(row.key, SEPARATOR[0]);

                size_t tokenCount = searchKeyTokens.size();

                if (searchKeyTokens == Tokens{storedKeyTokens.cbegin(), std::next(storedKeyTokens.cbegin(), static_cast<ssize_t>(tokenCount))})   // match given tokens against currently processed store entry
                {
                    // retrieve the token following the last matched one
                    result.insert(storedKeyTokens.at(searchKeyTokens.size()));
                }
            }

            return {result.cbegin(), result.cend()};
        }
    }
    else if (tokens.at(0) == "Statics")
    {
        return GetStaticKeys(tokens);
    }

    LOG(CbkLogLevel::Warning, "Using unsupported key format for GetKeys() on datastore: '" + key + "'");
    return {};
}

Keys BasicDataStoreImplementation::GetStaticKeys(const Tokens &tokens) const
{
    const Tokens searchKeyTokens{tokens.cbegin() + 1, tokens.cend()};
    std::set<Key> result;  // keys in result  shall be unique

    for (const auto& [storedKey, storedValue] : staticStore)
    {
        const auto storedKeyTokens = CommonHelper::TokenizeString(storedKey, SEPARATOR[0]);
        size_t tokenCount = searchKeyTokens.size();

        if (tokenCount == 0)   // all keys at top level are requested
        {
            result.insert(storedKeyTokens.front());
        }
        else if (tokenCount >= storedKeyTokens.size())
        {
            // key can't match (search key has more elements than currently processed store entry)
        }
        else if (searchKeyTokens == Tokens{storedKeyTokens.cbegin(), std::next(storedKeyTokens.cbegin(), static_cast<ssize_t>(tokenCount))})   // match given tokens agains currently processed store entry
        {
            // retrieve the token following the last matched one
            result.insert(storedKeyTokens.at(searchKeyTokens.size()));
        }
    }

    return {result.cbegin(), result.cend()};
}
