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

#include <iostream>
#include <variant>

#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>
#include <QFile>

#include "Common/Definitions.h"
#include "common/xmlParser.h"
namespace GlobalObserver::Routes {

namespace Import {
//! OSC Position
struct LanePosition {
    OdId roadId{};
    int laneId{};
    double s{};
};

//! OSC RoadPosition
struct RoadPosition {
    OdId roadId{};
    double s{};
    double t{};
};
typedef std::variant<Import::RoadPosition, Import::LanePosition> Position;
} // namespace Import

class RouteImporter {
public:
    RouteImporter(const std::string &path);
    ~RouteImporter() = default;

    bool Import(const std::string &filename);
    std::map<std::string, std::vector<Import::Position>> GetDReaMRoute();

private:
    Import::LanePosition ImportLanePosition(QDomElement positionElement);
    Import::RoadPosition ImportRoadPosition(QDomElement positionElement);
    std::map<std::string, std::vector<Import::Position>> waypointsMap{};
};
}