/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "StochasticNavigation.h"

StochasticNavigation::StochasticNavigation(StochasticsInterface &stochastics, const OWL::Interfaces::WorldData &worldData) :
    stochastics(stochastics),
    worldData(worldData),
    wdQuery(worldData)
{
}

StochasticNavigation::~StochasticNavigation()
{
    for (auto route : routes)
    {
        delete route;
    }
}

Route StochasticNavigation::GetRoute(GlobalRoadPosition start) const
{
    std::vector<RouteElement> roads;
    std::vector<std::string> junctions;
    bool reachedEndOfNetwork = false;
    bool inStreamDirection = start.laneId < 0;
    roads.push_back({start.roadId, inStreamDirection});
    std::string currentRoad = start.roadId;

    while (!reachedEndOfNetwork)
    {
        const auto& nextElement = inStreamDirection ? wdQuery.GetRoadSuccessor(currentRoad) : wdQuery.GetRoadPredecessor(currentRoad);
        switch (nextElement.type)
        {
        case RoadNetworkElementType::None :
            reachedEndOfNetwork = true;
            break;
        case RoadNetworkElementType::Road :
            inStreamDirection = (currentRoad == wdQuery.GetRoadPredecessor(nextElement.id).id); //Direction of next road is heading away from this road, if this road is the predecessor of the next road
            roads.push_back({nextElement.id, inStreamDirection});
            currentRoad = nextElement.id;
            break;
        case RoadNetworkElementType::Junction :
            junctions.emplace_back(nextElement.id);
            const auto& connections = wdQuery.GetConnectionsOnJunction(nextElement.id, currentRoad);
            if (connections.empty())
            {
                reachedEndOfNetwork = true;
                break;
            }
            auto index =static_cast<size_t>(stochastics.GetUniformDistributed(0, connections.size()));
            inStreamDirection = connections[index].outgoingStreamDirection;
            roads.push_back({connections[index].connectingRoadId, true});
            roads.push_back({connections[index].outgoingRoadId, inStreamDirection});
            currentRoad = connections[index].outgoingRoadId;
        }
    }
    size_t hash = 0;
    for (const auto& element : roads)
    {
        boost::hash_combine(hash, element.roadId);
        boost::hash_combine(hash, element.inRoadDirection);
    }
    return Route{roads, junctions, hash};
}

const LaneStream& StochasticNavigation::GetLaneStream(const Route& route,  const GlobalRoadPosition& position) const
{
    return GetLaneStream(route,  position.roadId, position.laneId, position.roadPosition.s);
}

const LaneStream& StochasticNavigation::GetLaneStream(const Route& route, const std::string& roadId, int laneId, double distance) const
{
    distance = std::max(0.0, distance);
    LaneStreamSection streamSection{&route, roadId, laneId, distance}; //For searching the cache, wether this position and route is already stored
    auto iter = laneStreams.find(streamSection);
    if (iter != laneStreams.end())
    {
        //LaneStream was already created -> reuse it
        return *iter->second;
    }

    //No reuseable LaneStream stored -> create a new one
    auto newRoute = new Route(route);
    routes.push_back(newRoute);
    auto laneStream = wdQuery.CreateLaneStream(route.roads, roadId, laneId, distance);
    laneStreamCache.emplace_back(laneStream);

    //Store the LaneStream in the map for all intervals that it is valid for
    for (const auto& lane :laneStream->GetElements())
    {
        auto& roadId = worldData.GetRoadIdMapping().at(lane().GetRoad().GetId());
        auto laneId = worldData.GetLaneIdMapping().at(lane().GetId());
        auto sStart = lane().GetDistance(OWL::MeasurementPoint::RoadStart);
        auto sEnd = lane().GetDistance(OWL::MeasurementPoint::RoadEnd);
        laneStreams.try_emplace(LaneStreamSection{newRoute, roadId, laneId, sStart, sEnd}, laneStream.get());
    }
    return *laneStream;
}

const RoadStream& StochasticNavigation::GetRoadStream(const Route& route) const
{
    auto iter = roadStreams.find(route);
    if (iter != roadStreams.end())
    {
        //RoadStream was already created -> reuse it
        return *iter->second;
    }

    //No reuseable RoadStream stored -> create a new one
    auto roadStream = wdQuery.CreateRoadStream(route.roads);
    roadStreamCache.emplace_back(roadStream);

    //Store the RoadStream in the map for this route
    roadStreams.try_emplace(route, roadStream.get());
    return *roadStream;
}
