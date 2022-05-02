#pragma once

#include "Common/commonTools.h"
#include "Element.h"

namespace MentalInfrastructure {

class Road;

class TrafficSignal : public Element {
public:
    TrafficSignal(OdId openDriveId, DReaMId dreamId, const MentalInfrastructure::Road *road, double s, Common::Vector2d pos) :
        Element(openDriveId, dreamId), road(road), s(s), position(pos) {
    }
    virtual ~TrafficSignal() {
    }

    const Common::Vector2d GetPosition() const {
        return position;
    }

    double GetS() const {
        return s;
    }

    const MentalInfrastructure::Road *GetRoad() const {
        return road;
    }

protected:
    const MentalInfrastructure::Road *road;
    double s;
    Common::Vector2d position;
};

} // namespace MentalInfrastructure