/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "Interfaces/stochasticsInterface.h"
#include "NavigationInterface.h"
#include "WorldDataQuery.h"
#include <unordered_map>
#include "boost/functional/hash.hpp"

//! This struct serves as key for storing the lane streams in map.
//! It represents an interval on a lane together with a route.
//! If two instances compare equal it means that we can reuse the stored
//! lane stream instead of newly creating it, because CreateLaneStream
//! would return the same result with this parameters and s in the interval
struct LaneStreamSection
{
    explicit LaneStreamSection (const Route* route,
                                const std::string& roadId,
                                OWL::OdId laneId,
                                double startS,
                                double endS) :
        route(route),
        roadId{roadId},
        laneId{laneId},
        startS{startS},
        endS{endS}
    {
        hash = route->hash;
        boost::hash_combine(hash, laneId);
        boost::hash_combine(hash, roadId);
    }

    explicit LaneStreamSection (const Route* route,
                                const std::string& roadId,
                                OWL::OdId laneId,
                                double s) :
        LaneStreamSection(route, roadId, laneId, s, s)
    {
    }

    const Route* route;
    const std::string& roadId;
    OWL::OdId laneId;
    double startS;
    double endS;
    size_t hash;

    //! Two instances are equal if the routes are equal and the struct represents an interval
    //! on the same opendrive lane
    bool operator==(const LaneStreamSection& other) const
    {
        if (startS == endS) //Special case if this represents a point -> Point must be in the interval
        {
            return laneId == other.laneId && roadId == other.roadId && startS >= other.startS && startS <= other.endS && route->roads == other.route->roads;
        }
        if (other.startS == other.endS) //Special case if other represents a point -> Point must be in the interval
        {
            return laneId == other.laneId && roadId == other.roadId && other.startS >= startS && other.startS <= endS && route->roads == other.route->roads;
        }
        //General case -> Intervals must intersect
        return laneId == other.laneId && roadId == other.roadId && endS > other.startS && startS < other.endS && route->roads == other.route->roads;
    }
};

// see https://en.cppreference.com/w/cpp/utility/hash
namespace std
{
template<> struct hash<LaneStreamSection>
{
    size_t operator()(LaneStreamSection const& lss) const noexcept
    {
        return lss.hash;
    }
};
template<> struct hash<Route>
{
    size_t operator()(Route const& route) const noexcept
    {
        return route.hash;
    }
};
}

class StochasticNavigation final : public NavigationInterface
{
public:
    StochasticNavigation (StochasticsInterface& stochastics, const OWL::Interfaces::WorldData& worldData);
    ~StochasticNavigation() override;

    Route GetRoute(GlobalRoadPosition start) const override;

    //! \brief Returns the lane stream that corresponds to the given route and containing the specified road position
    //!
    //! For performance optimization the lane streams are internally cached. If there is already a lane stream in the
    //! cached that is containing the specified position and was created using the same route, it is simply returned.
    //! Otherwise a new lane stream is creating for this position and route and stored in the cache along with the information
    //! for which road positions it is valid.
    //!
    //! \param route        route of the agent
    //! \param position     position of the agent
    //! \return lane stream for given position and route
    const LaneStream& GetLaneStream(const Route& route, const GlobalRoadPosition& position) const;

    //! \brief Returns the lane stream that corresponds to the given route and containing the specified road position
    //!
    //! For performance optimization the lane streams are internally cached. If there is already a lane stream in the
    //! cached that is containing the specified position and was created using the same route, it is simply returned.
    //! Otherwise a new lane stream is creating for this position and route and stored in the cache along with the information
    //! for which road positions it is valid.
    //!
    //! \param route        route of the agent
    //! \param roadId       OpenDrive id of the road
    //! \param laneId      OpenDrive id of the lane
    //! \param distance     s coordinate on the lane
    //! \return lane stream for given position and route
    const LaneStream& GetLaneStream(const Route& route, const std::string& roadId, int laneId, double distance) const;

    //! ------------------------------------------------------------------------
    //! \brief GetRoadStream gets the RoadStream for the specified Route
    //!
    //! \param route the Route containing road information for the RoadStream
    //!
    //! \return the RoadStream for the specified Route
    //! ------------------------------------------------------------------------
    const RoadStream& GetRoadStream(const Route& route) const;
private:
    StochasticsInterface& stochastics;
    const OWL::Interfaces::WorldData& worldData;
    const WorldDataQuery wdQuery;
    mutable std::unordered_map<LaneStreamSection, LaneStream const * const> laneStreams;
    mutable std::unordered_map<Route, RoadStream const * const> roadStreams;
    mutable std::vector<const Route*> routes;
    mutable std::vector<std::shared_ptr<const LaneStream>> laneStreamCache;
    mutable std::vector<std::shared_ptr<const RoadStream>> roadStreamCache;
};
