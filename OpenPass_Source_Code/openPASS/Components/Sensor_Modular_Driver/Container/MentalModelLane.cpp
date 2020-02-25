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
//! @file    MentalModelLane.cpp
//! @author  Christian Siebke
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief class to provide geometrical information of the lanes
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#include "MentalModelLane.h"
#include "algorithm"
 
MentalModelLane::MentalModelLane(uint64_t id, int64_t odId, double l, double w) :
    laneId(id), odId(odId), length(l), width(w) {}
 
uint64_t MentalModelLane::GetId() const {
    return laneId;
}
 
int64_t MentalModelLane::GetOdId() {
    return odId;
}
 
double MentalModelLane::GetLength() {
    return length;
}
 
double MentalModelLane::GetWidth() const {
    return width;
}
 
std::vector<std::tuple<double, double, double, double>> MentalModelLane::GetPoints() const {
    return points;
}
 
std::tuple<double, double, double, double> MentalModelLane::GetLastPoint() const {
    if (points.size() == 0) return std::tuple<double, double, double, double>();
 
    std::vector<std::tuple<double, double, double, double>>::const_iterator it = points.begin();
    std::advance(it, points.size() - 1);
 
    return *it;
}
 
std::tuple<double, double, double, double> MentalModelLane::GetFirstPoint() const  {
    if (points.size() == 0) return std::tuple<double, double, double, double>();
 
    std::vector<std::tuple<double, double, double, double>>::const_iterator it = points.begin();
 
    return *it;
}
 
void MentalModelLane::AddPoint(double x, double y, double hdg, double so) {
    points.push_back(std::tuple<double, double, double, double>(x, y, hdg, so));
}
 
void MentalModelLane::AddPredecessor(uint64_t id) {
    bool found = (std::find(predecessorIds.begin(), predecessorIds.end(), id) != predecessorIds.end());
    if (!found) predecessorIds.push_back(id);
}
 
std::list<uint64_t> MentalModelLane::GetPredecessors() const{
    return predecessorIds;
}
 
bool MentalModelLane::HasPredecessors() {
    return predecessorIds.size() > 0;
}
 
bool MentalModelLane::ContainsPredecessor(uint64_t otherLane) {
    return (std::find(predecessorIds.begin(), predecessorIds.end(), otherLane) != predecessorIds.end());
}
 
void MentalModelLane::AddSuccessor(uint64_t id) {
    bool found = (std::find(successorIds.begin(), successorIds.end(), id) != successorIds.end());
    if (!found) successorIds.push_back(id);
}
 
std::list<uint64_t> MentalModelLane::GetSuccessors() const {
    return successorIds;
}
 
bool MentalModelLane::HasSuccessors() {
    return successorIds.size() > 0;
}
 
bool MentalModelLane::ContainsSuccessor(uint64_t otherLane) {
    return (std::find(successorIds.begin(), successorIds.end(), otherLane) != successorIds.end());
}
