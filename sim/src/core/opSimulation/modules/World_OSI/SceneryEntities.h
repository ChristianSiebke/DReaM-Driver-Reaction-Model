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

#include "EntityInfo.h"
#include "include/roadInterface/roadLaneInterface.h"
#include "include/roadInterface/roadLaneRoadMark.h"
#include "include/roadInterface/roadObjectInterface.h"
#include "include/roadInterface/roadSignalInterface.h"

namespace openpass::utils {

static constexpr char ENTITY_SOURCE[]{"OpenDRIVE"};
static constexpr char ENTITY_VERSION[]{"1.6"};

/**
 * @brief Adapter function for publishing OpenDRIVE::RoadObjects
 * 
 * @param entity the RoadObject
 * @return The entity info 
 */
[[nodiscard]] const openpass::type::EntityInfo GetEntityInfo(const RoadObjectInterface &entity)
{
    return {
        ENTITY_SOURCE,
        {{"version", std::string(ENTITY_VERSION)},
         {"name", entity.GetName()},
         {"id", entity.GetId()},
         {"type", openpass::utils::to_string(entity.GetType())}}};
}

/**
 * @brief Adapter function for publishing OpenDRIVE::RoadLanes
 * 
 * @param entity the RoadLane
 * @return The entity info 
 */
[[nodiscard]] const openpass::type::EntityInfo GetEntityInfo(const RoadLaneInterface &entity)
{
    return {
        ENTITY_SOURCE,
        {
            {"version", std::string(ENTITY_VERSION)},
            {"name", std::string("Lane")},
            {"id", entity.GetId()},
            {"type", openpass::utils::to_string(entity.GetType())},
        }};
}

/**
 * @brief Adapter function for publishing OpenDRIVE::RoadLaneRoadMarks
 * 
 * @param entity the RoadLaneRoadMark
 * @return The entity info 
 */
[[nodiscard]] const openpass::type::EntityInfo GetEntityInfo(const RoadLaneRoadMark &entity)
{
    return {
        ENTITY_SOURCE,
        {
            {"version", std::string(ENTITY_VERSION)},
            {"name", std::string("LaneRoadMark")},
            {"type", openpass::utils::to_string(entity.GetType())},
        }};
}

/**
 * @brief Adapter function for publishing OpenDRIVE::RoadSignals
 * 
 * @param entity the RoadSignal
 * @return The entity info 
 */
[[nodiscard]] const openpass::type::EntityInfo GetEntityInfo(const RoadSignalInterface &entity)
{
    return {
        ENTITY_SOURCE,
        {{"version", std::string(ENTITY_VERSION)},
         {"name", std::string("RoadSignal")},
         {"id", entity.GetId()},
         {"type", entity.GetType()},
         {"subtype", entity.GetSubType()}}};
}

} // namespace openpass::utils
