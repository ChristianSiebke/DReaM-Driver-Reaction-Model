/******************************************************************************
* Copyright (c) 2019 TU Dresden
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*****************************************************************************/

//-----------------------------------------------------------------------------
//! @file    MentalModelLane.h
//! @author  Christian Siebke
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief class to provide geometrical information of the lanes
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#ifndef MENTALMODELLANE_H
#define MENTALMODELLANE_H
 
#include "commonTools.h"
#include "list"
#include "vector"
#include "map"
 
class MentalModelLane
{
public:
    MentalModelLane(uint64_t id, int64_t odId, double l, double w);
    MentalModelLane() = default;
    ~MentalModelLane() = default;
 
    /**
     * @brief GetId
     * @return the id of the lane
     */
    uint64_t GetId() const;
 
    /**
     * @brief GetOdId
     * @return the OpenDrive Id of the lane (on its road)
     */
    int64_t GetOdId();
 
    /**
     * @brief GetLength
     * @return the length of the lane
     */
    double GetLength();
 
    /**
     * @brief GetWidth
     * @return the width of the lane
     */
    double GetWidth() const ;
 
    /**
     * @brief GetPoints
     * @return all stored points (and their heading / s-Offset) making up the lane
     */
    std::vector<std::tuple<double, double, double, double>> GetPoints() const;
 
    /**
     * @brief GetLastPoint
     * @return the last point (and heading) for the lane
     */
    std::tuple<double, double, double, double> GetLastPoint() const;
 
    /**
     * @brief GetFirstPoint
     * @return the first point (and heading) for the lane
     */
    std::tuple<double, double, double, double> GetFirstPoint() const;
 
    /**
     * @brief Adds a reference point for the lane
     * @param x x-coordinate of the point
     * @param y y-coordinate of the point
     * @param hdg heading at the point
     * @param so s-Offset at the point
     */
    void AddPoint(double x, double y, double hdg, double so);
 
    /**
     * @brief Adds a predecessor to the internal stored list. Will not add if the predecessor does already exist.
     * @param id id of the predecessor
     */
    void AddPredecessor(uint64_t id);
 
    /**
     * @brief GetPredecessors
     * @return the list of all stored predecessors
     */
    std::list<uint64_t> GetPredecessors() const;
 
    /**
     * @brief HasPredecessors
     * @return whether the lane hs at least one predecessor
     */
    bool HasPredecessors();
 
    /**
     * @brief ContainsPredecessor
     * @param otherLane id of the other lane
     * @return whether this lane has the other id as a predecessor
     */
    bool ContainsPredecessor(uint64_t otherLane);
 
    /**
     * @brief Adds a successor to the internal stored list. Will not add if the successor does already exist.
     * @param id id of the successor
     */
    void AddSuccessor(uint64_t id);
 
    /**
     * @brief GetSuccessors
     * @return the list of all stored successors
     */
    std::list<uint64_t> GetSuccessors() const;
 
    /**
     * @brief HasSuccessors
     * @return whether the lane has at least one successor
     */
    bool HasSuccessors();
 
    /**
     * @brief ContainsSuccessor
     * @param otherLane id of the other lane
     * @return whether this lane has the other id as a successor
     */
    bool ContainsSuccessor(uint64_t otherLane);
 
private:
    uint64_t laneId;
    int64_t odId;
    double length;
    double width;
 
    std::vector<std::tuple<double, double, double, double>> points;
 
    std::list<uint64_t> predecessorIds;
    std::list<uint64_t> successorIds;

};
 
#endif // MENTALMODELLANE_H
