/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#ifndef STOPPINGPOINTCALCULATION_H
#define STOPPINGPOINTCALCULATION_H

#include "common/Definitions.h"
#include "common/Helper.h"

enum class ApproachDirection { Invalid, Left, Right, Ego, StraightAhead };

struct Line2d {
    Common::Vector2d start;
    Common::Vector2d direction;

    double distance(Common::Vector2d point) {
        Common::Vector2d pointToStart = start;
        pointToStart.Sub(point);

        double cross = abs(pointToStart.Cross(direction));

        double distance = cross / direction.Length();

        return distance;
    }

    double intersect(Line2d otherLine) {
        Common::Vector2d q = otherLine.start;
        Common::Vector2d p = start;
        Common::Vector2d s = otherLine.direction;
        Common::Vector2d r = direction;

        if(r.Cross(s) == 0) return maxDouble;

        q.Sub(p);
        return (q.Cross(s)) / r.Cross(s);
    }
};

struct CrossingLines {
    OdId roadId;
    ApproachDirection appDir;

    Line2d outerLeftLine;
    Line2d outerRightLine;

    Line2d centerLine;

    Line2d innerLeftLine;
    Line2d innerRightLine;
};

struct SP_ROW_Data {
    bool calcPedCross1 = true;
    bool calcPedCross2 = true;
    bool calcPedLeft = true;
    bool calcPedRight = true;
    bool calcVehicleCross = true;
    bool calcVehicleLeft = true;
};

class StoppingPointCalculation {
public:
    StoppingPointCalculation() {
    }
    ~StoppingPointCalculation() = default;

    /**
     * @brief CalcExtendedLine - Calculating the extended edges of a road
     * @param lane - the lane to calculate the edges from
     * @param rotationAngle - the rotation of the offset vector (-90° for left,
     * 90° for right)
     * @return the extended line
     */
    Line2d CalcExtendedLine(const MentalInfrastructure::Lane *lane, double rotationAngle, bool roadIsPredJunction, bool roadIsSuccJunction);

    /**
     * @brief CalcCrossingLines
     * @param lane - the lane approaching the Junction
     * @param dir - the direction the Junction is approached on as seen by the
     * Ego
     * @param lines - the lines struct for calculating
     * @return true if it worked
     */
    bool CalcCrossingLines(const MentalInfrastructure::Lane *lane, ApproachDirection dir, CrossingLines &lines,
                           const MentalInfrastructure::Junction *junction);

    /**
     * @brief CCLFromTurningLanes = helper for CalcCrossingLines
     * @param lane  - the turning lane on the Junction
     * @param dir - the direction the Junction is approached on as seen by the
     * Ego
     * @param lines - the lines struct for calculating
     * @return true if it worked
     */
    bool CCLFromTurningLanes(const MentalInfrastructure::Lane *&lane, ApproachDirection dir, CrossingLines &lines,
                             const MentalInfrastructure::Junction *junction);

    bool InsertCrossLines(const std::vector<const MentalInfrastructure::Lane *> &nextLanes,
                          std::map<ApproachDirection, CrossingLines> &crossLineMap, const MentalInfrastructure::Junction *junction,
                          const MentalInfrastructure::Lane *&SuccLane, CrossingLines &dummyCrossLines, ApproachDirection appDir);

    /**
     * @brief CalculateStoppingPoint - calculating a single stopping-point
     * @param road - ego road
     * @param lane - ego lane
     * @param line - line to intersect with lane
     * @param type - StoppingPointType
     * @return The Stopping-Point
     */
    StoppingPoint CalculateStoppingPoint(const MentalInfrastructure::Road *road, const MentalInfrastructure::Lane *lane, Line2d line,
                                         StoppingPointType type);

    /**
     * @brief DummyStoppingPoint
     * @return A dummy Stopping Point for filling the Map
     */
    StoppingPoint DummyStoppingPoint();

    /**
     * @brief DetermineROWData - determining which SPs should be calculated according to RightOfWay situation
     */
    SP_ROW_Data DetermineROWData(MentalInfrastructure::LaneType type);

    /**
     * @brief DetermineStoppingPoints - determining all stopping points on an
     * junction from a certain approach lane
     * @param egoLane - Lane from which junction is approached
     * @param junction on which the stopping points
     * are to be determined
     * @return Map of stoping points
     */
    std::unordered_map<StoppingPointType, StoppingPoint> DetermineStoppingPoints(const MentalInfrastructure::Junction *junction,
                                                                       const MentalInfrastructure::Lane *egoLane);
};

#endif // STOPPINGPOINTCALCULATION_H
