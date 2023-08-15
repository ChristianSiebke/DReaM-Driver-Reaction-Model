/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
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
    const Road *from = nullptr;
    const Road *with = nullptr;
    const Road *to = nullptr;
};

///
/// \brief Represents a junction in the road network.
///
class Junction : public Element {
public:
    Junction(const OdId &openDriveId, DReaMId dreamId) : Element(openDriveId, dreamId) {
    }
    ~Junction() override {
    }

    ///
    /// \brief Returns all possible connection roads for a given incoming road.
    /// \param from the incoming road
    /// \return all possible connecting roads (on the junction, can be empty)
    ///
    const std::vector<const Road *> GetConnectionRoads(const Road *from) const;

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
    std::vector<const Road *> roadsOnJunction{};
    std::vector<JunctionConnection> connections{};
    std::vector<const Road *> incomingRoads{};
};
} // namespace MentalInfrastructure
