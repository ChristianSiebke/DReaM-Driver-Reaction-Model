#ifndef ROUTEIMPORTER_H
#define ROUTEIMPORTER_H

#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>
#include <QFile>

#include "LoggerInterface.h"
#include "common/xmlParser.h"

class RouteImporter {
public:
    RouteImporter(std::string path, LoggerInterface *loggerinterface);
    ~RouteImporter() = default;

    bool Import(const std::string &filename);
    DReaMRoute GetDReaMRoute(std::string);

private:
    std::map<std::string,DReaMRoute> dReaMRouteMap;
    DReaMRoute dReaMRoute;
    LoggerInterface *loggerInterface;

    void Log(const std::string &message, LogLevel_new level = info) const {
        loggerInterface->Log(message, level);
    }
};

#endif // ROUTEIMPORTER_H