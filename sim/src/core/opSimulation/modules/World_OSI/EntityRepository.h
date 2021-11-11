/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <map>
#include <stdexcept>

#include "EntityInfoPublisher.h"
#include "common/openPassTypes.h"

namespace openpass::entity {

using EntityGroupId = uint64_t;

/**
 * @brief Supported entity types
 * 
 * Moving objects are considered to be available only for one run
 * Stationary objects and others will marked as persistent
 */
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
    /// @param entityInfo Additional info necessary e.g. for creation, or reporting
    /// @returns unique EntityId
    [[nodiscard]] virtual type::EntityId Register(openpass::type::EntityInfo entityInfo) = 0;

    /// @brief Registers a new entity for given entity type
    /// @note  Throws if entity type is unknown
    ///
    /// @param entityType   @see EntityType
    /// @param entityInfo
    /// @return EntityId
    [[nodiscard]] virtual type::EntityId Register(EntityType entityType, openpass::type::EntityInfo entityInfo) = 0;
};

/// @brief Handles unique IDs and reports creation to the entityInfoPublisher
class Repository final : public RepositoryInterface
{
    static constexpr size_t MAX_ENTITIES_PER_GROUP{1000000};

    /// @brief Defines an index group for entities, such as "moving objects from 0 to 10000"
    class EntityGroup
    {
    public:
        /// @brief Create a group
        /// @param capacity    maximum number of entities within the group
        /// @param offset      initial index
        /// @param persistent true if group shall not be affected by a reset (e.g. lanes)
        constexpr EntityGroup(size_t capacity, size_t offset, bool persistent) noexcept :
            offset{offset},
            end{offset + capacity},
            nextId{offset},
            persistent{persistent}
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

        /// @brief Get persistence of the current group
        /// @returns True if group is unaffected of resets
        [[nodiscard]] constexpr bool GetPersistence() noexcept
        {
            return persistent;
        }

        /// @brief Resets the index to the original offset
        /// @note  If group is persistent, the index remains unchanged
        void Reset() noexcept
        {
            if (!persistent)
            {
                nextId = offset;
            }
        }

    private:
        size_t offset;
        size_t end;
        size_t nextId;
        bool persistent;
    };

public:
    /// @brief Create a new repository
    /// @param databuffer Reference to a DataBufferWriteInterface
    Repository(DataBufferWriteInterface *const databuffer);

    ~Repository() override;

    [[nodiscard]] type::EntityId Register(openpass::type::EntityInfo entityInfo) override;
    [[nodiscard]] type::EntityId Register(EntityType entityType, openpass::type::EntityInfo entityInfo) override;
    void Reset() override;

private:
    /// @brief Registers an index group for the given entity type
    void RegisterGroup(EntityType entityType, bool persistence);

    openpass::publisher::EntityInfoPublisher _entityInfoPublisher;
    std::map<EntityType, EntityGroup> _repository;
};

} // namespace openpass::entity
