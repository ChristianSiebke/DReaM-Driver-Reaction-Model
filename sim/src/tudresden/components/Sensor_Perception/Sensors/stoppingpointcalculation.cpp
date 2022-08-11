#include "stoppingpointcalculation.h"

#include <iostream>

Line2d StoppingPointCalculation::CalcExtendedLine(const MentalInfrastructure::Lane *lane, double rotationAngle, bool roadIsPredJunction,
                                                  bool roadIsSuccJunction) {
    Common::Vector2d lineStart;
    Common::Vector2d laneDirection;

    bool rightWay = (roadIsPredJunction && lane->IsInRoadDirection()) || (roadIsSuccJunction && !lane->IsInRoadDirection());
    auto pointsList = lane->GetLanePoints();
    auto secondLast = [](bool rightWay, auto pointsList) {
        return rightWay ? std::next(pointsList.begin(), 1) : std::prev(pointsList.end(), 2);
    };
    auto last = [](bool rightWay, auto lane) { return rightWay ? lane->GetFirstPoint() : lane->GetLastPoint(); };

    laneDirection.x = last(rightWay, lane)->x - (*secondLast(rightWay, pointsList)).x;
    laneDirection.y = last(rightWay, lane)->y - (*secondLast(rightWay, pointsList)).y;

    // std::cout << "Vec calc: " << laneDirection << " = "
    //           << "(" << last(rightWay, lane)->x << "," << last(rightWay, lane)->y << ") - (" << (*secondLast(rightWay, pointsList)).x <<
    //           ","
    //           << (*secondLast(rightWay, pointsList)).y << ")" << std::endl;
    // std::cout << "Vec before: " << laneDirection << std::endl;

    lineStart.x = last(rightWay, lane)->x;
    lineStart.y = last(rightWay, lane)->y;

    double xCpy = laneDirection.x;
    double yCpy = laneDirection.y;

    Common::Vector2d copy(xCpy, yCpy);
    copy.Norm();
    copy.Scale(lane->GetWidth() / 2);

    // lane->IsInRoadDirection() ? rotationAngle *= 1 : rotationAngle *= -1;

    copy.Rotate(rotationAngle);

    lineStart.Add(copy);

    // std::cout << "Vec after: " << laneDirection << std::endl;

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
        // std::cout << "Lane " << lane->GetRoad()->GetOpenDriveId() << " is in direction? " << lane->IsInRoadDirection() << std::endl;
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

// https://www.geeksforgeeks.org/program-for-point-of-intersection-of-two-lines/
Common::Vector2d lineLineIntersection(Common::Vector2d A, Common::Vector2d B, Common::Vector2d C, Common::Vector2d D) {
    // Line AB represented as a1x + b1y = c1
    double a1 = B.y - A.y;
    double b1 = A.x - B.x;
    double c1 = a1 * (A.x) + b1 * (A.y);

    // Line CD represented as a2x + b2y = c2
    double a2 = D.y - C.y;
    double b2 = C.x - D.x;
    double c2 = a2 * (C.x) + b2 * (C.y);

    double determinant = a1 * b2 - a2 * b1;

    if (determinant == 0) {
        // The lines are parallel. This is simplified
        // by returning a pair of FLT_MAX
        return Common::Vector2d(FLT_MAX, FLT_MAX);
    }
    else {
        double x = (b2 * c1 - b1 * c2) / determinant;
        double y = (a1 * c2 - a2 * c1) / determinant;
        return Common::Vector2d(x, y);
    }
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
    bool minPointAtNext = false;

    // loop over all points on ego lane
    // determine point with minimum distance
    for (auto it = lane->GetLanePoints().begin(); it != lane->GetLanePoints().end(); it++) {
        auto egoPoint = *it;
        double egoX = egoPoint.x;
        double egoY = egoPoint.y;

        Common::Vector2d point(egoX, egoY);
        double distance = abs(line.distance(point));

        if (distance < minDistance) {
            minDistance = distance;
            minEgoPoint = egoPoint;

            if (std::next(it) != lane->GetLanePoints().end() && it != lane->GetLanePoints().begin()) {
                prev = *std::prev(it);
                next = *std::next(it);
                minPointNotAtEnd = true;
            }
            else if (std::next(it) != lane->GetLanePoints().end()) {
                prev = {0.0, 0.0, 0.0, 0.0};
                next = *std::next(it);
                minPointNotAtEnd = false;
                minPointAtNext = false;
            }
            else if (it != lane->GetLanePoints().begin()) {
                prev = *std::prev(it);
                next = {0.0, 0.0, 0.0, 0.0};
                minPointNotAtEnd = false;
                minPointAtNext = false;
            }
        }
    }

    Common::Vector2d origin{minEgoPoint.x, minEgoPoint.y};
    Common::Vector2d dirF{next.x, next.y};
    Common::Vector2d dirB{prev.x, prev.y};

    if (minPointNotAtEnd) {
        Common::Vector2d pointF = lineLineIntersection(origin, dirF, line.start, line.direction + line.start);
        Common::Vector2d pointB = lineLineIntersection(origin, dirB, line.start, line.direction + line.start);

        Common::Vector2d distF = origin - pointF;
        Common::Vector2d distB = origin - pointB;

        double dF = distF.Length();
        double dB = distB.Length();

        if (dF < dB) {
            minEgoPoint.x = pointF.x;
            minEgoPoint.y = pointF.y;
            double offsetF = dF / (origin - dirF).Length();

            minEgoPoint.sOffset = minEgoPoint.sOffset + offsetF * (next.sOffset - minEgoPoint.sOffset);
        }
        else {
            minEgoPoint.x = pointB.x;
            minEgoPoint.y = pointB.y;
            double offsetB = dB / (origin - dirB).Length();

            minEgoPoint.sOffset = minEgoPoint.sOffset + offsetB * (prev.sOffset - minEgoPoint.sOffset);
        }
    }

    // Common::Vector2d dirF{next.x - minEgoPoint.x, next.y - minEgoPoint.y};
    // Common::Vector2d dirB{prev.x - minEgoPoint.x, prev.y - minEgoPoint.y};

    // Line2d forward;
    // Line2d backward;
    // forward.start = origin;
    // backward.start = origin;
    // forward.direction = dirF;
    // backward.direction = dirB;

    // if (minPointNotAtEnd) {
    //     double offsetF = line.intersect(forward);
    //     double offsetB = line.intersect(backward);

    //     if (offsetB >= -1 && offsetB <= 1) {
    //         minEgoPoint.x = origin.x + offsetB * dirB.x;
    //         minEgoPoint.y = origin.y + offsetB * dirB.y;
    //         minEgoPoint.sOffset = minEgoPoint.sOffset + offsetB * (prev.sOffset - minEgoPoint.sOffset);
    //     }
    //     else if (offsetF >= -1 && offsetF <= 1) {
    //         minEgoPoint.x = origin.x + offsetF * dirF.x;
    //         minEgoPoint.y = origin.y + offsetF * dirF.y;
    //         minEgoPoint.sOffset = minEgoPoint.sOffset + offsetF * (next.sOffset - minEgoPoint.sOffset);
    //     }
    // }
    // else {
    //     double offset = 0;
    //     Common::Vector2d dir;
    //     double sOffset;
    //     if (minPointAtNext) {
    //         offset = line.intersect(backward);
    //         dir = dirB;
    //         sOffset = prev.sOffset;
    //     }
    //     else {
    //         offset = line.intersect(forward);
    //         dir = dirF;
    //         sOffset = next.sOffset;
    //     }

    //     if (offset != 0) {
    //         minEgoPoint.x = origin.x + offset * dir.x;
    //         minEgoPoint.y = origin.y + offset * dir.y;
    //         minEgoPoint.sOffset = minEgoPoint.sOffset + offset * (sOffset - minEgoPoint.sOffset);
    //     }
    // }

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

std::unordered_map<StoppingPointType, StoppingPoint>
StoppingPointCalculation::DetermineStoppingPoints(const MentalInfrastructure::Junction *junction,
                                                  const MentalInfrastructure::Lane *egoLane) {
    std::unordered_map<StoppingPointType, StoppingPoint> stoppingPoints;

    SP_ROW_Data rowData = DetermineROWData(egoLane->GetType());

    NextDirectionLanes nextLanes;
    // assumption movingInLaneDirection = true for now
    if (auto nextLanesPtr = InfrastructurePerception::NextLanes(true, egoLane)) {
        if (nextLanesPtr.has_value()) {
            nextLanes = nextLanesPtr.value();
        }
        else {
            return stoppingPoints;
        }
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
