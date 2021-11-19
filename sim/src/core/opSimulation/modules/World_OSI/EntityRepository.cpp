/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "EntityRepository.h"

#include <algorithm>

namespace openpass::entity {

Repository::Repository(DataBufferWriteInterface *const databuffer) :
    _entityInfoPublisher(databuffer)
{
    static constexpr bool AS_PERSISTENT_GROUP{true};
    static constexpr bool AS_NOT_PERSISTENT_GROUP{false};

    RegisterGroup(EntityType::MovingObject, AS_NOT_PERSISTENT_GROUP);
    RegisterGroup(EntityType::StationaryObject, AS_PERSISTENT_GROUP);
    RegisterGroup(EntityType::Others, AS_PERSISTENT_GROUP);
}

Repository::~Repository()
{
}

openpass::type::EntityId Repository::Register(openpass::type::EntityInfo entityInfo)
{
    return Register(EntityType::Others, std::move(entityInfo));
}

openpass::type::EntityId Repository::Register(EntityType entityType, openpass::type::EntityInfo entityInfo)
{
    const auto entityId = type::EntityId(_repository.at(entityType).GetNextIndex());
    _entityInfoPublisher.Publish(entityId, entityInfo, _repository.at(entityType).GetPersistence());
    return entityId;
}

void Repository::Reset()
{
    std::for_each(_repository.begin(), _repository.end(),
                  [](auto &repoEntries) { repoEntries.second.Reset(); });
}

void Repository::RegisterGroup(EntityType entityType, bool persistence)
{
    _repository.emplace(entityType, EntityGroup(MAX_ENTITIES_PER_GROUP,
                                                MAX_ENTITIES_PER_GROUP * _repository.size(),
                                                persistence));
}

} // namespace openpass::entity
