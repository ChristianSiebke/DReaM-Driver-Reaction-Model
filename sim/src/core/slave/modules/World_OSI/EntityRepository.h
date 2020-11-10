/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <algorithm>
#include <exception>
#include <map>

#include "common/logEntryBase.h"
#include "common/openPassTypes.h"
#include "include/dataStoreInterface.h"
#include "include/roadInterface/roadElementTypes.h"
#include "include/roadInterface/roadObjectInterface.h"

namespace openpass::entity {

class EntityMetaInfo : public openpass::publisher::LogEntryBase
{
public:
    EntityMetaInfo(std::string name) :
        LogEntryBase(std::move(name))
    {
    }

    operator Acyclic() const noexcept override
    {
        return Acyclic(name, triggeringEntities, {}, parameter);
    }

    openpass::datastore::Parameter parameter;
};

using EntityGroupId = uint64_t;

enum class EntityType
{
    MovingObject,
    StationaryObject,
    Others
};


/// @brief Implementing classes of this interface shall handle unique IDs for various types of entities
class RepositoryInterface
{
public:
    virtual ~RepositoryInterface() = default;

    /// @brief Reset internally used indices
    virtual void Reset() = 0;

    /// @brief Registers a new entity without entity type (assigned to "others")
    /// @param metaInfo Additional info necessary e.g. for creation, or reporting
    /// @returns unique EntityId
    [[nodiscard]] virtual type::EntityId Register(EntityMetaInfo metaInfo) = 0;

    /// @brief Registers a new entity for given entity type
    /// @note  Throws if entity type is unknown
    ///
    /// @param entityType   @see EntityType
    /// @param metaInfo
    /// @return EntityId
    [[nodiscard]] virtual type::EntityId Register(EntityType entityType, EntityMetaInfo metaInfo) = 0;
};

/// @brief Handles unique IDs and reports creation to the datastore
class Repository final : public RepositoryInterface
{
    static constexpr size_t MAX_ENTITIES_PER_GROUP{100000};
    static constexpr char DATASTORE_TOPIC[] = {"Entities"};

    /// @brief Defines an index group for entities, such as "moving objects from 0 to 10000"
    class EntityGroup
    {
    public:
        /// @brief Create a group
        /// @param capacity   maximum number of entities within the group
        /// @param offset     initial index
        constexpr EntityGroup(size_t capacity, size_t offset) noexcept :
            offset{offset},
            end{offset + capacity},
            nextId{offset}
        {
        }

        /// @brief Retrieve the next index
        /// @note  Throws if too many indices are retrieved
        /// @returns initial index or incremented by one w.r.t to last call
        [[nodiscard]] size_t GetNextIndex()
        {
            if (nextId == end)
            {
                throw std::runtime_error("Too many entities for current group");
            }
            return nextId++;
        }

        /// @brief Resets the index to the original offset
        void Reset() noexcept
        {
            nextId = offset;
        }

    private:
        size_t offset;
        size_t end;
        size_t nextId;
    };

    /// @brief Registers an index group for the given entity type
    void RegisterGroup(EntityType entityType)
    {
        _repository.emplace(entityType, EntityGroup(MAX_ENTITIES_PER_GROUP,
                                                    MAX_ENTITIES_PER_GROUP * _repository.size()));
    }

    DataStoreWriteInterface *const _dataStore;
    std::map<EntityType, EntityGroup> _repository;

public:
    /// @brief Create a new repository
    /// @param datastore Reference to datastore
    Repository(DataStoreWriteInterface *const dataStore) :
        _dataStore(dataStore)
    {
        RegisterGroup(EntityType::MovingObject);
        RegisterGroup(EntityType::StationaryObject);
        RegisterGroup(EntityType::Others);
    }

    ~Repository() override = default;

    [[nodiscard]] type::EntityId Register(EntityMetaInfo metaInfo) override
    {
        return Register(EntityType::Others, std::move(metaInfo));
    }

    [[nodiscard]] type::EntityId Register(EntityType entityType, EntityMetaInfo metaInfo) override
    {
        const auto entityId = type::EntityId(_repository.at(entityType).GetNextIndex());
        _dataStore->PutAcyclic(0, entityId, DATASTORE_TOPIC, metaInfo);
        return entityId;
    }

    void Reset() override
    {
        std::for_each(_repository.begin(), _repository.end(),
            [](auto& repoEntries){ repoEntries.second.Reset(); });
    }
};

} // namespace openpass::entity
