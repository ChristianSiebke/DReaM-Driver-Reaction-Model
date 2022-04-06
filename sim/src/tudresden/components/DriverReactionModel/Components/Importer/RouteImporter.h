#ifndef ROUTEIMPORTER_H
#define ROUTEIMPORTER_H

#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>
#include <QFile>

#include "LoggerInterface.h"
#include "common/xmlParser.h"

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
    RouteImporter(std::string path, LoggerInterface *loggerinterface);
    ~RouteImporter() = default;

    bool Import(const std::string &filename);
    std::vector<Import::Position> GetDReaMRoute(std::string);

private:
    Import::LanePosition ImportLanePosition(QDomElement positionElement);
    Import::RoadPosition ImportRoadPosition(QDomElement positionElement);
    void Log(const std::string &message, LogLevel_new level = info) const {
        if (loggerInterface == nullptr) {
            throw std::runtime_error(message);
        }
        loggerInterface->Log(message, level);
    }
    std::map<std::string, std::vector<Import::Position>> waypointsMap;
    LoggerInterface *loggerInterface;
};

#endif // ROUTEIMPORTER_H