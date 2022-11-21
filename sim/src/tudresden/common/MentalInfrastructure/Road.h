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

#include "Element.h"
#include "Junction.h"
#include "Trafficsign.h"

namespace MentalInfrastructure {

class Lane;
class TrafficSign;

///
/// \brief Represents a road in the road network.
///
class Road : public Element {
  public:
      Road(OdId openDriveId, DReaMId dreamId, double posX, double posY, double hdg, double length) :
          Element(openDriveId, dreamId), startPos(Common::Vector2d(posX, posY)), roadHdg(hdg), roadLength(length) {
      }
    ~Road() override {}

    ///
    /// Basic road data
    ///

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
    /// Junction data
    ///

    ///
    /// \brief Returns whether the road is on a junction.
    ///
    bool IsOnJunction() const;

    ///
    /// \brief Sets the Junction the road is on.
    ///
    void SetOnJunction(const Junction *junction);

    ///
    /// \brief Returns the Junction the road is on (can be nullptr).
    ///
    const Junction *GetJunction() const;

    ///
    /// Predecessor data
    ///

    ///
    /// \brief Sets the predecessor of the road (must be an Junction or Road!).
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
    /// \brief Returns whether the predecessor of this road is an Junction.
    ///
    bool IsPredecessorJunction() const;

    ///
    /// Successor data
    ///

    ///
    /// \brief Sets the successor of the road (must be an Junction or Road!).
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
    /// \brief Returns whether the successor of this road is an Junction.
    ///
    bool IsSuccessorJunction() const;

    ///
    /// Section data
    ///

    ///
    /// \brief Returns all lanes on this road.
    ///
    const std::vector<const Lane *> &GetLanes() const;

    ///
    /// \brief Adds a lane to the road.
    /// \param lane the Lane to add
    ///
    void AddLane(const Lane *lane);

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
      Common::Vector2d startPos;
      double roadHdg;
      double roadLength;

      ///
      /// Pointers to other objects which are children / siblings of this road.
      ///
      const Element *successor = nullptr;
      const Element *predecessor = nullptr;
      const Junction *junctionPtr = nullptr;

      std::vector<const Lane *> lanes;
      std::vector<const TrafficSign *> trafficSigns;
};

} // namespace MentalInfrastructure
