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
#include "Lane.h"
#include "list"

namespace MentalInfrastructure {

class Road;

///
/// \brief Represents a part of a road in the road network. Sections always belong to a Road.
///
class Section : public Element {
  public:
    Section(Id sectionId, const Road* road, const Section* predecessor, const Section* successor)
        : Element(sectionId), road(road), predecessorSection(predecessor), successorSection(successor) {}
    ~Section() override {}

    ///
    /// Basic section data
    ///

    ///
    /// \brief Returns the Road this section is on.
    ///
    const Road* GetRoad() const;

    ///
    /// \brief Returns the successor section (can be nullptr).
    ///
    const Section* GetSuccessor() const;

    ///
    /// \brief Returns the predecessor section (can be nullptr).
    ///
    const Section* GetPredecessor() const;

    ///
    /// Lane data
    ///

    ///
    /// \brief Adds a lane to this section.
    /// \param lane the Lane to add
    ///
    void AddLane(const Lane* lane);

    ///
    /// \brief Returns all Lanes that belong to this section.
    ///
    const std::vector<const Lane*>& GetLanes() const;

    ///
    /// \brief Returns all Lanes of this section that are in road direction.
    ///
    std::vector<const Lane*> GetLanesInRoadDirection() const;

    ///
    /// \brief Returns all Lanes of this section that are no in road direction.
    ///
    std::vector<const Lane*> GetLanesNotInRoadDirection() const;

  private:
    const Road* road;

    const Section* predecessorSection;
    const Section* successorSection;

    std::vector<const Lane*> lanes;
};
} // namespace MentalInfrastructure
