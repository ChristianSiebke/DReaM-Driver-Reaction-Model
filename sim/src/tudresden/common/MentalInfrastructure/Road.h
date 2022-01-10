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

#include "Element.h"
#include "Intersection.h"
#include "Trafficsign.h"
#include <unordered_map>

namespace MentalInfrastructure {

class Section;
class TrafficSign;

///
/// \brief Represents a road in the road network.
///
class Road : public Element {
  public:
    Road(Id roadId, std::string odId, double posX, double posY, double hdg, double length)
        : Element(roadId), openDriveId(odId), startPos(Common::Vector2d(posX, posY)), roadHdg(hdg), roadLength(length) {}
    ~Road() override {}

    ///
    /// Basic road data
    ///

    ///
    /// \brief Returns the OpenDriveId of the road (unique).
    ///
    std::string GetOpenDriveId() const;

    ///
    /// \brief Returns the start position of the road.
    ///
    Common::Vector2d GetStartPosition() const;

    ///
    /// \brief Returns the total length of the road.
    ///
    double GetLength() const;

    ///
    /// \brief Returns the start heading of the road.
    ///
    double GetStartHeading() const;

    ///
    /// Intersection data
    ///

    ///
    /// \brief Returns whether the road is on an intersection.
    ///
    bool IsOnIntersection() const;

    ///
    /// \brief Sets the Intersection the road is on.
    ///
    void SetOnIntersection(const Intersection* intersection);

    ///
    /// \brief Returns the Intersection the road is on (can be nullptr).
    ///
    const Intersection* GetIntersection() const;

    ///
    /// Predecessor data
    ///

    ///
    /// \brief Sets the predecessor of the road (must be an Intersection or Road!).
    /// \param element the new predecessor
    ///
    void SetPredecessor(const Element* element);

    ///
    /// \brief Returns whether the road has a predecessor.
    ///
    bool HasPredecessor() const { return predecessor != nullptr; }

    ///
    /// \brief Returns the predecessor of this road (can be nullptr).
    ///
    const Element* GetPredecessor() const { return predecessor; }

    ///
    /// \brief Returns whether the predecessor of this road is an Intersection.
    ///
    bool IsPredecessorIntersection() const;

    ///
    /// Successor data
    ///

    ///
    /// \brief Sets the successor of the road (must be an Intersection or Road!).
    /// \param element the new successor
    ///
    void SetSuccessor(const Element* element);

    ///
    /// \brief Returns whether the road has a successor.
    ///
    bool HasSuccessor() const { return successor != nullptr; }

    ///
    /// \brief Returns the successor of this road (can be nullptr).
    ///
    const Element* GetSuccessor() const { return successor; }

    ///
    /// \brief Returns whether the successor of this road is an Intersection.
    ///
    bool IsSuccessorIntersection() const;

    ///
    /// Section data
    ///

    ///
    /// \brief Returns all sections this road is made up of.
    ///
    const std::vector<const Section*>& GetSections() const;

    ///
    /// \brief Adds a Section to the road.
    /// \param section the Section to add
    ///
    void AddSection(const Section* section);

    ///
    /// TrafficSign data
    ///

    ///
    /// \brief Returns all signs on this road.
    ///
    const std::vector<const TrafficSign*>& GetTrafficSigns() const;

    ///
    /// \brief Adds a TrafficSign to the road.
    /// \param the sign to add
    ///
    void AddTrafficSign(const TrafficSign* sign);

  private:
    ///
    /// Basic information about this road.
    ///

    std::string openDriveId;
    Common::Vector2d startPos;
    double roadHdg;
    double roadLength;

    ///
    /// Pointers to other objects which are children / siblings of this road.
    ///

    const Element* successor = nullptr;
    const Element* predecessor = nullptr;
    const Intersection* intersectionPtr = nullptr;

    std::vector<const Section*> sections;
    std::vector<const TrafficSign*> trafficSigns;
};

} // namespace MentalInfrastructure
