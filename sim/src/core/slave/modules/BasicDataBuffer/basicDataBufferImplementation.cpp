/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  basicDataBufferImplementation.cpp */
//-----------------------------------------------------------------------------

#include "basicDataBufferImplementation.h"

#include <memory>
#include <utility>

#include "common/commonTools.h"
#include "common/openPassUtils.h"
#include "include/parameterInterface.h"
#include "include/stochasticsInterface.h"
#include "include/worldInterface.h"

using namespace openpass::type;

/*!
 * \brief Tests, if two vectors of tokens have matching elements
 *
 * The test result will be true, if each token A in the matcher, the token B at the same position (index) in the reference are string equal.
 * A matcher token containing \c openpass::databuffer::WILDCARD ('*') will match every string in the reference.
 * The test result will always be false, if the matcher contains more elements than the reference.
 *
 * \param[in]    matcher     Tokens to test against reference
 * \param[out]   reference   A full stack of tokens to match against (i. e. a 'path' of DataBuffer keys in the DataBuffer hierarchy)
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

BasicDataBufferImplementation::BasicDataBufferImplementation(const openpass::common::RuntimeInformation *runtimeInformation, const CallbackInterface *callbacks) :
    DataBufferInterface(runtimeInformation, callbacks)
{
}

template <>
std::unique_ptr<CyclicResultInterface> BasicDataBufferImplementation::GetIndexed<EntityId>(const std::optional<EntityId> entityId, const Tokens &tokens) const
{
    CyclicRowRefs rowRefs;

    auto range = entityIdIndex.equal_range(entityId.value());

    for (auto it = range.first; it != range.second; ++it)
    {
        const auto& storeValue = cyclicStore.at(it->second);

        if (TokensMatch(tokens, storeValue.tokens))
        {
            rowRefs.emplace_back(storeValue);
        }
    }

    return std::make_unique<CyclicResult>(cyclicStore, rowRefs);
}

std::unique_ptr<CyclicResultInterface> BasicDataBufferImplementation::GetCyclic(const Key& key) const
{
    const Tokens tokens = CommonHelper::TokenizeString(key, SEPARATOR);
    CyclicRowRefs rowRefs;

    for (const auto& storeValue : cyclicStore)
    {
        if (TokensMatch(tokens, storeValue.tokens))
        {
            rowRefs.emplace_back(storeValue);
        }
    }

    return std::make_unique<CyclicResult>(cyclicStore, rowRefs);
}

std::unique_ptr<CyclicResultInterface> BasicDataBufferImplementation::GetCyclic(const std::optional<EntityId> entityId, const Key &key) const
{
    const Tokens tokens = CommonHelper::TokenizeString(key, SEPARATOR);

    if (entityId.has_value())
    {
        return GetIndexed<EntityId>(entityId, tokens);
    }
    else
    {
        return GetCyclic(key);
    }
}

void BasicDataBufferImplementation::PutCyclic(const EntityId agentId, const Key &key, const Value &value)
{
    cyclicStore.emplace_back(agentId, key, value);

    size_t newValueIndex = cyclicStore.size() - 1;
    entityIdIndex.emplace(agentId, newValueIndex);
}

void BasicDataBufferImplementation::PutAcyclic(const EntityId entityId, const Key &key, const Acyclic &acyclic)
{
    acyclicStore.emplace_back(entityId, key, acyclic);
}

void BasicDataBufferImplementation::PutStatic(const Key &key, const Value &value, bool persist)
{
    if (staticStore.find(key) == staticStore.end())
    {
        staticStore[key] = {value, persist};
    }
}

void BasicDataBufferImplementation::ClearRun()
{
    ClearTimeStep();

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

void BasicDataBufferImplementation::ClearTimeStep()
{
    cyclicStore.clear();
    entityIdIndex.clear();
    acyclicStore.clear();
}

std::unique_ptr<AcyclicResultInterface> BasicDataBufferImplementation::GetAcyclic(const Key& key) const
{
    const Tokens tokens = CommonHelper::TokenizeString(key, SEPARATOR);

    AcyclicRowRefs rowRefs;

    for (const auto& storeValue : acyclicStore)
    {
        if (TokensMatch(tokens, CommonHelper::TokenizeString(storeValue.key, SEPARATOR)))
        {
            rowRefs.emplace_back(storeValue);
        }
    }

    return std::make_unique<AcyclicResult>(acyclicStore, rowRefs);
}

std::unique_ptr<AcyclicResultInterface> BasicDataBufferImplementation::GetAcyclic([[maybe_unused]] const std::optional<EntityId> entityId, const Key &key) const
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

Values BasicDataBufferImplementation::GetStatic(const Key &key) const
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

Keys BasicDataBufferImplementation::GetKeys(const Key &key) const
{
    const Tokens tokens = CommonHelper::TokenizeString(key, SEPARATOR);

    if (tokens.at(0) == "Cyclics")
    {
        if (tokens.size() == 1)
        {
            Keys keys;
            EntityId lastEntityId{-1};

            auto it = entityIdIndex.cbegin();

            while (it != entityIdIndex.cend())
            {
                lastEntityId = std::get<0>(it->first);
                keys.push_back(std::to_string(lastEntityId));
                it = entityIdIndex.upper_bound(lastEntityId);
            }

            return keys;
        }
        else
        {
            std::set<Key> result;
            Tokens searchKeyTokens{tokens.cbegin() + 2, tokens.cend()};

            const auto entries = GetIndexed<EntityId>(std::stod(tokens.at(1)), searchKeyTokens);

            for (const auto& entry : *entries)
            {
                const auto& row = entry.get();

                size_t tokenCount = searchKeyTokens.size();

                if (searchKeyTokens == Tokens{row.tokens.cbegin(), std::next(row.tokens.cbegin(), static_cast<ssize_t>(tokenCount))})   // match given tokens against currently processed store entry
                {
                    // retrieve the token following the last matched one
                    result.insert(row.tokens.at(searchKeyTokens.size()));
                }
            }

            return {result.cbegin(), result.cend()};
        }
    }
    else if (tokens.at(0) == "Statics")
    {
        return GetStaticKeys(tokens);
    }

    LOG(CbkLogLevel::Warning, "Using unsupported key format for GetKeys() on databuffer: '" + key + "'");
    return {};
}

Keys BasicDataBufferImplementation::GetStaticKeys(const Tokens &tokens) const
{
    const Tokens searchKeyTokens{tokens.cbegin() + 1, tokens.cend()};
    std::set<Key> result;  // keys in result  shall be unique

    for (const auto& [storedKey, storedValue] : staticStore)
    {
        const auto storedKeyTokens = CommonHelper::TokenizeString(storedKey, SEPARATOR);
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
