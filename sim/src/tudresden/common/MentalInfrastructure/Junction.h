/******************************************************************************
 * Copyright (c) 2020 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/
#pragma once

#include <unordered_map>
#include <vector>

#include "Element.h"

namespace MentalInfrastructure {

class Road;

///
/// \brief Struct to encapsulate the conenction information for one connection of a junction.
///
struct JunctionConnection {
    const Road *from;
    const Road *with;
    const Road *to;
};

///
/// \brief Represents a junction in the road network.
///
class Junction : public Element {
public:
    Junction(std::string openDriveId) : Element(openDriveId) {
    }
    ~Junction() override {
    }

    ///
    /// \brief Returns the road that connects two other roads.
    /// \param from the the incoming road
    /// \param to the target road
    /// \return the connecting road (can be nullptr)
    ///
    const Road *GetRoadForConnection(const Road *from, const Road *to) const;

    ///
    /// \brief Returns all possible connections for a given incoming road.
    /// \param from the incoming road
    /// \return all possible target roads (can be empty)
    ///
    const std::vector<const Road *> GetPossibleConnections(const Road *from) const;

    ///
    /// \brief Returns all possible connection roads for a given incoming road.
    /// \param from the incoming road
    /// \return all possible connecting roads (on the junction, can be empty)
    ///
    const std::vector<const Road *> GetConnectionRoads(const Road *from) const;

    ///
    /// \brief Returns the target road of a connecting road.
    /// \param on the connecting road
    /// \return the target road (can be nullptr if the road is not on the junction)
    ///
    const Road *GetNextRoad(const Road *on);

    ///
    /// \brief Returns all connecting roads of the junction.
    ///
    const std::vector<const Road *> &GetAllRoadsOnJunction() const {
        return roadsOnJunction;
    }

    ///
    /// \brief Returns all roads leading to this junction.
    ///
    const std::vector<const Road *> &GetIncomingRoads() const {
        return incomingRoads;
    }

    ///
    /// \brief Returns all connecting roads (on the junction) that lead to a specific target road.
    /// \param to the target road
    /// \return the list of all connecting roads leading towards the target (can be empty)
    ///
    const std::vector<const Road *> PredecessorConnectionRoads(const Road *to) const;

    ///
    /// \brief Adds a connection to the junction
    /// \param from the incoming road
    /// \param with the connecting road (on the junction)
    /// \param to the target road
    ///
    void AddConnection(const Road *from, const Road *with, const Road *to);

private:
    std::vector<const Road *> roadsOnJunction;
    std::vector<JunctionConnection> connections;
    std::vector<const Road *> incomingRoads;
};
} // namespace MentalInfrastructure
