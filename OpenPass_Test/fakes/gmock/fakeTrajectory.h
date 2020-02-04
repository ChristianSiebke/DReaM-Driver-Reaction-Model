#pragma once

#include "gmock/gmock.h"
#include "Interfaces/trajectoryInterface.h"

class FakeTrajectory : public TrajectoryInterface
{
public:
    MOCK_METHOD2(AddRoadCoordinate, bool(int, RoadPosition));
    MOCK_METHOD2(AddWorldCoordinate, bool(int, Position));

    MOCK_METHOD0(GetTrajectoryType, TrajectoryType());
    MOCK_METHOD0(GetRoadCoordinates, std::map<int, RoadPosition> *());
    MOCK_METHOD0(GetWorldCoordinates, std::map<int, Position> *());

    MOCK_METHOD1(SetTrajectoryType, void(TrajectoryType));
};


