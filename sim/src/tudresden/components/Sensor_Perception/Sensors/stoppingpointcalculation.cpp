#include "stoppingpointcalculation.h"

Line2d StoppingPointCalculation::CalcExtendedLine(const MentalInfrastructure::Lane *lane, double rotationAngle, bool roadIsPredJunction,
                                                  bool roadIsSuccJunction) {
    Common::Vector2d lineStart;
    Common::Vector2d laneDirection;

    bool rightWay = (roadIsPredJunction && lane->IsInRoadDirection()) || (roadIsSuccJunction && !lane->IsInRoadDirection());
    auto pointsList = lane->GetLanePoints();
    auto secondLast = [](bool rightWay, auto pointsList) {
        return rightWay ? std::next(pointsList.begin(), 2) : std::prev(pointsList.end(), 2);
    };
    auto last = [](bool rightWay, auto lane) { return rightWay ? lane->GetFirstPoint() : lane->GetLastPoint(); };

    laneDirection.x = last(rightWay, lane)->x - (*secondLast(rightWay, pointsList)).x;
    laneDirection.y = last(rightWay, lane)->y - (*secondLast(rightWay, pointsList)).y;

    lineStart.x = last(rightWay, lane)->x;
    lineStart.y = last(rightWay, lane)->y;

    Common::Vector2d copy = laneDirection;
    copy.Norm();
    copy.Scale(lane->GetWidth() / 2);
    copy.Rotate(rotationAngle);

    lineStart.Add(copy);

    Line2d line;
    line.start = lineStart;
    line.direction = laneDirection;

    return line;
}

bool StoppingPointCalculation::CalcCrossingLines(const MentalInfrastructure::Lane *succLane, ApproachDirection dir, CrossingLines &lines,
                                                 const MentalInfrastructure::Junction *junction) {
    auto incomingRoad = succLane->GetRoad();
    bool roadIsPredJunction = incomingRoad->IsPredecessorJunction() && incomingRoad->GetPredecessor() == junction;
    bool roadIsSuccJunction = incomingRoad->IsSuccessorJunction() && incomingRoad->GetSuccessor() == junction;

    const MentalInfrastructure::Section *section = nullptr;

    if (!roadIsPredJunction && !roadIsSuccJunction)
        return false;

    auto lanes = incomingRoad->GetLanes();

    const MentalInfrastructure::Lane *leftmostDrivingLane = nullptr;
    const MentalInfrastructure::Lane *rightmostDrivingLane = nullptr;
    const MentalInfrastructure::Lane *centerRightLane = nullptr;

    if (lanes.empty()) {
        return false;
    }

    const auto rightmostLane = lanes.front();
    const auto leftmostLane = lanes.back();

    const MentalInfrastructure::Lane *lastLane = nullptr;

    for (auto lane : lanes) {
        if (!lastLane) {
            lastLane = lane;
            continue;
        }
        if (lastLane->GetType() != LaneType::Driving && lane->GetType() == LaneType::Driving) {
            rightmostDrivingLane = lane;
        }
        if (lastLane->GetType() == LaneType::Driving && lane->GetType() != LaneType::Driving) {
            leftmostDrivingLane = lastLane;
        }
        if (std::stoi(lastLane->GetOpenDriveId()) < 0 && std::stoi(lane->GetOpenDriveId()) > 0) {
            centerRightLane = lastLane;
        }

        lastLane = lane;
    }

    if (!leftmostDrivingLane || !rightmostDrivingLane || !centerRightLane) {
        return false;
    }

    if (roadIsSuccJunction || roadIsPredJunction) {
        lines.roadId = incomingRoad->GetOpenDriveId();
        lines.appDir = dir;

        const auto oll = roadIsSuccJunction ? leftmostLane : rightmostLane;
        const auto orl = roadIsSuccJunction ? rightmostLane : leftmostLane;
        const auto ill = roadIsSuccJunction ? leftmostDrivingLane : rightmostDrivingLane;
        const auto irl = roadIsSuccJunction ? rightmostDrivingLane : leftmostDrivingLane;

        lines.outerLeftLine = CalcExtendedLine(oll, M_PI_2, roadIsPredJunction, roadIsSuccJunction);
        lines.outerRightLine = CalcExtendedLine(orl, -M_PI_2, roadIsPredJunction, roadIsSuccJunction);

        double centerAngle = roadIsSuccJunction ? M_PI_2 : -M_PI_2;

        lines.innerLeftLine = CalcExtendedLine(ill, M_PI_2, roadIsPredJunction, roadIsSuccJunction);
        lines.innerRightLine = CalcExtendedLine(irl, -M_PI_2, roadIsPredJunction, roadIsSuccJunction);
        lines.centerLine = CalcExtendedLine(centerRightLane, centerAngle, roadIsPredJunction, roadIsSuccJunction);

        return true;
    }
    else {
        return false;
    }
}

bool StoppingPointCalculation::CCLFromTurningLanes(const MentalInfrastructure::Lane *&lane, ApproachDirection dir, CrossingLines &lines,
                                                   const MentalInfrastructure::Junction *junction) {
    if (lane->GetSuccessors().size() == 1) {
        const auto succLane = lane->GetSuccessors().front();
        return CalcCrossingLines(succLane, dir, lines, junction);
    }

    return false;
}

bool StoppingPointCalculation::InsertCrossLines(const std::vector<const MentalInfrastructure::Lane *> &nextLanes,
                                                std::map<ApproachDirection, CrossingLines> &crossLineMap,
                                                const MentalInfrastructure::Junction *junction, const MentalInfrastructure::Lane *&SuccLane,
                                                CrossingLines &dummyCrossLines, ApproachDirection appDir) {
    if (!nextLanes.empty()) {
        auto turnLane = nextLanes.front();
        SuccLane = nextLanes.front();
        CrossingLines CrossLines;

        if (CCLFromTurningLanes(turnLane, appDir, CrossLines, junction)) {
            crossLineMap.insert(std::make_pair(appDir, CrossLines));
        }
        else {
            crossLineMap.insert(std::make_pair(appDir, dummyCrossLines));
        }
    }
    else {
        crossLineMap.insert(std::make_pair(appDir, dummyCrossLines));
    }

    return true;
}

StoppingPoint StoppingPointCalculation::CalculateStoppingPoint(const MentalInfrastructure::Road *road,
                                                               const MentalInfrastructure::Lane *lane, Line2d line,
                                                               StoppingPointType type) {
    StoppingPoint stoppingPoint = DummyStoppingPoint();
    MentalInfrastructure::LanePoint minEgoPoint(0.0, 0.0, 0.0, 0.0);
    MentalInfrastructure::LanePoint prev(0.0, 0.0, 0.0, 0.0);
    MentalInfrastructure::LanePoint next(0.0, 0.0, 0.0, 0.0);
    double minDistance = static_cast<double>(INFINITY);

    bool minPointNotAtEnd = false;

    // loop over all points on ego lane and crossroad lane
    // determine pair with minimum distance
    for (auto it = lane->GetLanePoints().begin(); it != lane->GetLanePoints().end(); it++) {
        auto egoPoint = *it;
        double egoX = egoPoint.x;
        double egoY = egoPoint.y;

        Common::Vector2d point(egoX, egoY);
        double distance = abs(line.distance(point));

        if (distance < minDistance) {
            minDistance = distance;
            minEgoPoint = egoPoint;

            if ((it++) != lane->GetLanePoints().end() && it != lane->GetLanePoints().begin()) {
                prev = *(it--);
                next = *(it++);
                minPointNotAtEnd = true;
            }
        }
    }

    if (minPointNotAtEnd) {
        Common::Vector2d origin{minEgoPoint.x, minEgoPoint.y};
        Common::Vector2d dirF{next.x, next.y};
        Common::Vector2d dirB{prev.x, prev.y};
        dirF.Sub(origin);
        dirB.Sub(origin);

        Line2d forward;
        Line2d backward;
        forward.start = origin;
        backward.start = origin;
        forward.direction = dirF;
        backward.direction = dirB;

        double offsetF = line.intersect(forward);
        double offsetB = line.intersect(backward);

        if (offsetB > 0 && offsetB < 1) {
            minEgoPoint.x = backward.start.x + offsetB * backward.direction.x;
            minEgoPoint.y = backward.start.y + offsetB * backward.direction.y;
            minEgoPoint.sOffset = minEgoPoint.sOffset + offsetB * (prev.sOffset - minEgoPoint.sOffset);
        }
        else if (offsetF > 0 && offsetF < 1) {
            minEgoPoint.x = forward.start.x + offsetF * forward.direction.x;
            minEgoPoint.y = forward.start.y + offsetF * forward.direction.y;
            minEgoPoint.sOffset = minEgoPoint.sOffset + offsetF * (next.sOffset - minEgoPoint.sOffset);
        }
    }

    // setting the struct
    stoppingPoint.type = type;
    stoppingPoint.sOffset = minEgoPoint.sOffset;
    stoppingPoint.road = road;
    stoppingPoint.lane = lane;
    stoppingPoint.posX = minEgoPoint.x;
    stoppingPoint.posY = minEgoPoint.y;

    return stoppingPoint;
}

StoppingPoint StoppingPointCalculation::DummyStoppingPoint() {
    StoppingPoint stoppingPoint;
    stoppingPoint.type = StoppingPointType::NONE;
    stoppingPoint.sOffset = 0;
    stoppingPoint.distanceToEgo = -1;
    stoppingPoint.road = nullptr;
    stoppingPoint.lane = nullptr;

    return stoppingPoint;
}

SP_ROW_Data StoppingPointCalculation::DetermineROWData(LaneType type) {
    SP_ROW_Data rowData;

    switch (type) {
    case LaneType::Sidewalk:
    case LaneType::Biking:
        rowData.calcPedLeft = false;
        rowData.calcPedRight = false;
        rowData.calcPedCross1 = false;
        rowData.calcPedCross2 = false;
        rowData.calcVehicleLeft = false;
        rowData.calcVehicleCross = true;
        break;
    case LaneType::Driving:
    default:
        rowData.calcPedLeft = true;
        rowData.calcPedRight = true;
        rowData.calcPedCross1 = true;
        rowData.calcPedCross2 = true;
        rowData.calcVehicleLeft = true;
        rowData.calcVehicleCross = true;
        break;
    }

    return rowData;
}

std::map<StoppingPointType, StoppingPoint> StoppingPointCalculation::DetermineStoppingPoints(const MentalInfrastructure::Junction *junction,
                                                                                             const MentalInfrastructure::Lane *egoLane) {
    std::map<StoppingPointType, StoppingPoint> stoppingPoints;

    SP_ROW_Data rowData = DetermineROWData(egoLane->GetType());

    NextDirectionLanes nextLanes;
    // assumption movingInLaneDirection = true for now
    if (auto nextLanesPtr = InfrastructurePerception::NextLanes(egoLane->IsInRoadDirection(), egoLane)) {
        nextLanes = *nextLanesPtr;
    }

    const MentalInfrastructure::Lane *leftSuccLane = nullptr;
    const MentalInfrastructure::Lane *straightSuccLane = nullptr;
    const MentalInfrastructure::Lane *rightSuccLane = nullptr;

    std::map<ApproachDirection, CrossingLines> crossLineMap;

    CrossingLines dummyCrossLines;
    dummyCrossLines.appDir = ApproachDirection::Invalid;

    ApproachDirection appDir = ApproachDirection::Ego;
    CrossingLines CrossLines;

    if (CalcCrossingLines(egoLane, appDir, CrossLines, junction)) {
        crossLineMap.insert(std::make_pair(appDir, CrossLines));
    }
    else {
        crossLineMap.insert(std::make_pair(appDir, dummyCrossLines));
    }

    InsertCrossLines(nextLanes.leftLanes, crossLineMap, junction, leftSuccLane, dummyCrossLines, ApproachDirection::Left);
    InsertCrossLines(nextLanes.straightLanes, crossLineMap, junction, straightSuccLane, dummyCrossLines, ApproachDirection::StraightAhead);
    InsertCrossLines(nextLanes.rightLanes, crossLineMap, junction, rightSuccLane, dummyCrossLines, ApproachDirection::Right);

    // BEGIN inserting StoppingPoints

    // CrossingOne
    if (rowData.calcPedCross1 && straightSuccLane && crossLineMap.at(ApproachDirection::Right).appDir != ApproachDirection::Invalid) {
        StoppingPointType type = StoppingPointType::Pedestrian_Crossing_ONE;

        stoppingPoints.insert(std::make_pair(type, CalculateStoppingPoint(straightSuccLane->GetRoad(), straightSuccLane,
                                                                          crossLineMap.at(ApproachDirection::Right).outerLeftLine, type)));
    }
    else if (rowData.calcPedCross1 && straightSuccLane && crossLineMap.at(ApproachDirection::Left).appDir != ApproachDirection::Invalid) {
        StoppingPointType type = StoppingPointType::Pedestrian_Crossing_ONE;

        stoppingPoints.insert(std::make_pair(type, CalculateStoppingPoint(straightSuccLane->GetRoad(), straightSuccLane,
                                                                          crossLineMap.at(ApproachDirection::Left).outerRightLine, type)));
    }
    else {
        StoppingPointType type = StoppingPointType::Pedestrian_Crossing_ONE;
        stoppingPoints.insert(std::make_pair(type, DummyStoppingPoint()));
    }

    // CrossingTWO
    if (rowData.calcPedCross2 && straightSuccLane && crossLineMap.at(ApproachDirection::Right).appDir != ApproachDirection::Invalid) {
        StoppingPointType type = StoppingPointType::Pedestrian_Crossing_TWO;

        stoppingPoints.insert(std::make_pair(type, CalculateStoppingPoint(straightSuccLane->GetRoad(), straightSuccLane,
                                                                          crossLineMap.at(ApproachDirection::Right).innerRightLine, type)));
    }
    else if (rowData.calcPedCross2 && straightSuccLane && crossLineMap.at(ApproachDirection::Left).appDir != ApproachDirection::Invalid) {
        StoppingPointType type = StoppingPointType::Pedestrian_Crossing_TWO;

        stoppingPoints.insert(std::make_pair(type, CalculateStoppingPoint(straightSuccLane->GetRoad(), straightSuccLane,
                                                                          crossLineMap.at(ApproachDirection::Left).innerLeftLine, type)));
    }
    else {
        StoppingPointType type = StoppingPointType::Pedestrian_Crossing_TWO;
        stoppingPoints.insert(std::make_pair(type, DummyStoppingPoint()));
    }

    // Ped Left
    if (rowData.calcPedLeft && leftSuccLane && crossLineMap.at(ApproachDirection::Ego).appDir != ApproachDirection::Invalid) {
        StoppingPointType type = StoppingPointType::Pedestrian_Left;

        stoppingPoints.insert(std::make_pair(type, CalculateStoppingPoint(leftSuccLane->GetRoad(), leftSuccLane,
                                                                          crossLineMap.at(ApproachDirection::Ego).innerLeftLine, type)));
    }
    else {
        StoppingPointType type = StoppingPointType::Pedestrian_Left;
        stoppingPoints.insert(std::make_pair(type, DummyStoppingPoint()));
    }

    // Ped Right
    if (rowData.calcPedRight && rightSuccLane && crossLineMap.at(ApproachDirection::Ego).appDir != ApproachDirection::Invalid) {
        StoppingPointType type = StoppingPointType::Pedestrian_Right;

        stoppingPoints.insert(std::make_pair(type, CalculateStoppingPoint(rightSuccLane->GetRoad(), rightSuccLane,
                                                                          crossLineMap.at(ApproachDirection::Ego).innerRightLine, type)));
    }
    else {
        StoppingPointType type = StoppingPointType::Pedestrian_Right;
        stoppingPoints.insert(std::make_pair(type, DummyStoppingPoint()));
    }

    // Veh Cross
    if (rowData.calcVehicleCross && straightSuccLane && crossLineMap.at(ApproachDirection::Left).appDir != ApproachDirection::Invalid) {
        StoppingPointType type = StoppingPointType::Vehicle_Crossroad;

        stoppingPoints.insert(std::make_pair(type, CalculateStoppingPoint(straightSuccLane->GetRoad(), straightSuccLane,
                                                                          crossLineMap.at(ApproachDirection::Left).innerRightLine, type)));
    }
    else if (rowData.calcVehicleLeft && straightSuccLane &&
             crossLineMap.at(ApproachDirection::Right).appDir != ApproachDirection::Invalid) {
        StoppingPointType type = StoppingPointType::Vehicle_Crossroad;
        stoppingPoints.insert(std::make_pair(type, CalculateStoppingPoint(straightSuccLane->GetRoad(), straightSuccLane,
                                                                          crossLineMap.at(ApproachDirection::Right).innerRightLine, type)));
    }
    else {
        StoppingPointType type = StoppingPointType::Vehicle_Crossroad;
        stoppingPoints.insert(std::make_pair(type, DummyStoppingPoint()));
    }

    // Vehicle LeftTurn
    if (rowData.calcVehicleLeft && leftSuccLane && crossLineMap.at(ApproachDirection::StraightAhead).appDir != ApproachDirection::Invalid) {
        StoppingPointType type = StoppingPointType::Vehicle_Left;

        stoppingPoints.insert(
            std::make_pair(type, CalculateStoppingPoint(leftSuccLane->GetRoad(), leftSuccLane,
                                                        crossLineMap.at(ApproachDirection::StraightAhead).centerLine, type)));
    }
    else {
        StoppingPointType type = StoppingPointType::Vehicle_Left;
        stoppingPoints.insert(std::make_pair(type, DummyStoppingPoint()));
    }

    return stoppingPoints;
}
