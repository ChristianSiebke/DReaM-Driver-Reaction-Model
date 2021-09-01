/*******************************************************************************
* Copyright (c) 2019, 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "SpawnerPreRunCommonDefinitions.h"
#include "common/SpawnerDefinitions.h"
#include "common/commonTools.h"
#include "include/parameterInterface.h"

using namespace SpawnerPreRunCommonDefinitions;

constexpr char S_START[] = {"SStart"}; //!< Name of the spawner parameter
constexpr char S_END[] = {"SEnd"}; //!< Name of the spawner parameter
constexpr char S_LENGTH[] = {"SLength"}; //!< Name of the spawner parameter
constexpr int MAX_SEARCH_DEPTH = 10;

//! Calculates a possible way in the graph to the target road via depth search
//!
//! \param targetRoad   id of the target road
//! \param roadGraph    road graph
//! \param start        start of search
//! \return route from start to target (excluding start)
static std::pair<std::vector<RouteElement>,RoadGraphVertex> GetWayToNextRoad(const std::string& targetRoad, const RoadGraph& roadGraph, const RoadGraphVertex start)
{
    for (auto [successor, successorsEnd] = adjacent_vertices(start, roadGraph); successor != successorsEnd; ++successor)
    {
        const auto& routeElement = get(RouteElement{}, roadGraph, *successor);
        if (routeElement.roadId == targetRoad)
        {
            return {{routeElement}, *successor};
        }
        auto [wayToNextRoad, targetVertex] = GetWayToNextRoad(targetRoad, roadGraph, *successor);
        if (!wayToNextRoad.empty())
        {
            wayToNextRoad.insert(wayToNextRoad.begin(), routeElement);
            return {wayToNextRoad, targetVertex};
        }
    }
    return {{}, 0};
}

//! Calculates the at most two possible routes corresponding to the defined spawn area.
//! The first route starts at the first road in the list and continues along the next elements (potentially with other roads in between)
//! as long as there is a possible way in the network.
//! If there is no such way, the rest of the list is ignored.
//! If the first route has lanes in both directions on all roads, then a second route is also returned, which is the reverse.
//! If a specified road doesn't exist, the route(s) built up to this road are returned.
//! This also means, an empty list of routes is returned, if the first road doesn't exist.
//!
//! \param roads    list of roads defined in the spawner profile
//! \param world    world
//! \return routes to spawn in
static std::vector<std::vector<RouteElement>> GetRoutesForSpawnArea(const std::vector<std::string>& roads, const WorldInterface& world)
{
    std::vector<RouteElement> route;
    bool finished = false;
    const auto& firstRoad = roads.front();
    bool inOdDirection = true;
    bool inOdDirectionExisting = world.IsDirectionalRoadExisting(firstRoad, true);
    bool againstOdDirectionExisting = world.IsDirectionalRoadExisting(firstRoad, false);
    if (!inOdDirectionExisting && !againstOdDirectionExisting)
    {
        return {};
    }
    RoadGraph roadGraphInOdDirection;
    RoadGraphVertex startInOdDirection;
    if (inOdDirectionExisting)
    {
        std::tie(roadGraphInOdDirection, startInOdDirection) = world.GetRoadGraph({firstRoad, true}, MAX_SEARCH_DEPTH);
    }
    RoadGraph roadGraphAgainstOdDirection;
    RoadGraphVertex startAgainstOdDirection;
    if (againstOdDirectionExisting)
    {
        std::tie(roadGraphAgainstOdDirection, startAgainstOdDirection) = world.GetRoadGraph({firstRoad, false}, MAX_SEARCH_DEPTH);
    }
    RoadGraphVertex current{startInOdDirection};
    if (roads.size() == 1)
    {
        route.emplace_back(firstRoad, true);
    }
    else
    {
        const auto& secondRoad = roads.at(1);
        if (inOdDirectionExisting && !GetWayToNextRoad(secondRoad, roadGraphInOdDirection, startInOdDirection).first.empty())
        {
            route.emplace_back(firstRoad, true);
        }
        else if (againstOdDirectionExisting && !GetWayToNextRoad(secondRoad, roadGraphAgainstOdDirection, startAgainstOdDirection).first.empty())
        {
            route.emplace_back(firstRoad, false);
            inOdDirection = false;
            current = startAgainstOdDirection;
        }
        else
        {
            route.emplace_back(firstRoad, true);
            finished = true;
        }
    }
    for (auto roadId = roads.begin() + 1; roadId != roads.end() && !finished; ++roadId)
    {
        const auto [wayToNextRoad, next] = GetWayToNextRoad(*roadId, inOdDirection ? roadGraphInOdDirection : roadGraphAgainstOdDirection, current);
        if (wayToNextRoad.empty())
        {
            finished = true;
        }
        else
        {
            route.insert(route.end(), wayToNextRoad.begin(), wayToNextRoad.end());
            current = next;
        }
    }

    bool isReverseRouteExisting{true};
    for (const auto& routeElement : route)
    {
        if (!world.IsDirectionalRoadExisting(routeElement.roadId, !routeElement.inOdDirection))
        {
            isReverseRouteExisting = false;
            break;
        }
    }

    if (isReverseRouteExisting)
    {
        std::vector<RouteElement> reverseRoute;
        for (auto routeElement = route.rbegin(); routeElement != route.rend(); ++routeElement)
        {
            reverseRoute.emplace_back(routeElement->roadId, !routeElement->inOdDirection);
        }
        return {route, reverseRoute};
    }

    return {route};
}

//! Calculates the SpawnAreas (i.e. LaneStreams with range) that where defined as parameters of the spawner
//!
//! \param parameter    spawner parameters
//! \param world        world
//! \param callbacks    callbacks for logging
//! \return areas to spawn in
static std::vector<SpawnArea> ExtractSpawnAreas(const ParameterInterface &parameter, const WorldInterface& world, const CallbackInterface* callbacks)
{
    using namespace helper;

    std::vector<SpawnArea> spawnAreas;

    const auto&spawnZonesList = map::query(parameter.GetParameterLists(), SPAWNZONES);
    SPAWNER_THROWIFFALSE(spawnZonesList.has_value(), "No SpawnZones provided for SpawnerPreRunCommon");

    for (const auto&spawnZoneParameter : spawnZonesList.value())
    {
        const auto roadIdsElement = map::query(spawnZoneParameter->GetParametersStringVector(), ROADS);
        const auto laneIdsElement = map::query(spawnZoneParameter->GetParametersIntVector(), LANES);
        const auto sStartElement = map::query(spawnZoneParameter->GetParametersDouble(), S_START);
        const auto sEndElement = map::query(spawnZoneParameter->GetParametersDouble(), S_END);
        const auto sLengthElement = map::query(spawnZoneParameter->GetParametersDouble(), S_LENGTH);

        SPAWNER_THROWIFFALSE(roadIdsElement.has_value(), "No road ids provided in SceneryInformation for SpawnerPreRunCommon");

        const auto routes = GetRoutesForSpawnArea(roadIdsElement.value(), world);
        bool firstRoute = true;
        for (const auto& route : routes)
        {
            const auto roadStream = world.GetRoadStream(route);
            auto sStartOnStream = 0.;
            auto sEndOnStream = roadStream->GetLength();
            if (firstRoute)
            {
                if (sStartElement.has_value())
                {
                    double sStart = std::clamp(sStartElement.value(), 0.0, world.GetRoadLength(route.front().roadId));
                    sStartOnStream = roadStream->GetStreamPosition(GlobalRoadPosition{route.front().roadId, 0, sStart, 0, 0}).s;
                }
                if (sEndElement.has_value())
                {
                    double sEnd = std::clamp(sEndElement.value(), 0.0, world.GetRoadLength(route.back().roadId));
                    sEndOnStream = roadStream->GetStreamPosition(GlobalRoadPosition{route.back().roadId, 0, sEnd, 0, 0}).s;
                }
                else if (sLengthElement.has_value())
                {
                    sEndOnStream = sStartOnStream + sLengthElement.value();
                }
            }
            else
            {
                if (sStartElement.has_value())
                {
                    double sStart = std::clamp(sStartElement.value(), 0.0, world.GetRoadLength(route.back().roadId));
                    sEndOnStream = roadStream->GetStreamPosition(GlobalRoadPosition{route.back().roadId, 0, sStart, 0, 0}).s;
                }
                if (sEndElement.has_value())
                {
                    double sEnd = std::clamp(sEndElement.value(), 0.0, world.GetRoadLength(route.front().roadId));
                    sStartOnStream = roadStream->GetStreamPosition(GlobalRoadPosition{route.front().roadId, 0, sEnd, 0, 0}).s;
                }
                else if (sLengthElement.has_value())
                {
                    sStartOnStream = sEndOnStream - sLengthElement.value();
                }
            }
            SPAWNER_THROWIFFALSE(sStartOnStream < sEndOnStream, "Invalid range");
            firstRoute = false;
            if (laneIdsElement.has_value())
            {
                for (auto& laneId : laneIdsElement.value())
                {
                    if (route.begin()->inOdDirection ? (laneId > 0) : (laneId < 0))
                    {
                        continue;
                    }
                    spawnAreas.emplace_back(roadStream->GetLaneStream({sStartOnStream, 0.}, laneId),
                                            sStartOnStream,
                                            sEndOnStream);
                }
            }
            else
            {
                auto laneStreams = roadStream->GetAllLaneStreams();
                for (auto& laneStream : laneStreams)
                {
                    spawnAreas.emplace_back(std::move(laneStream),
                                            sStartOnStream,
                                            sEndOnStream);
                }
            }
        }
    }

    return spawnAreas;
}

static PreRunSpawnerParameters ExtractSpawnZonesParameters(const ParameterInterface &parameter, const WorldInterface& world, const CallbackInterface* callbacks)
{
    return {ExtractMinimumSpawnBuffer(parameter), ExtractSpawnAreas(parameter, world, callbacks), ExtractAgentProfileLaneMaps(parameter, callbacks)};
}
