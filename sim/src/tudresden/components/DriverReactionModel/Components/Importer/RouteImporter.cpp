#include "RouteImporter.h"

RouteImporter::RouteImporter(std::string path, LoggerInterface *loggerInterface) : loggerInterface{loggerInterface} {
    if (!Import(path)) {
        Log("Could not import Scenario.xosc in DReaM", LogLevel_new::error);
    }
}

bool RouteImporter::Import(const std::string &filename) {
    // what is this doing?
    std::locale::global(std::locale("C"));

    QFile xmlFile(filename.c_str());
    if (!xmlFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray xmlData(xmlFile.readAll());
    QDomDocument document;
    QString errorMsg;
    int errorLine;

    if (!document.setContent(xmlData, &errorMsg, &errorLine)) {
        return false;
    }
    QDomElement documentRoot = document.documentElement();
    if (documentRoot.isNull()) {
        return false;
    }

    QDomElement storyboardElement;
    if (!SimulationCommon::GetFirstChildElement(documentRoot, "storyboard", storyboardElement)) {
        return false;
    }

    QDomElement initElement;
    if (!SimulationCommon::GetFirstChildElement(storyboardElement, "Init", initElement)) {
        return false;
    }

    QDomElement actionsElement;
    if (!SimulationCommon::GetFirstChildElement(initElement, "Actions", actionsElement)) {
        return false;
    }

    QDomElement privateElement;
    if (!SimulationCommon::GetFirstChildElement(actionsElement, "Private", privateElement)) {
        return false;
    }

    while (!privateElement.isNull()) {
        std::string id;
        if (!SimulationCommon::ParseAttributeString(privateElement, "entityRef", id)) {
            return false;
        }

        QDomElement privateActionElement;
        SimulationCommon::GetFirstChildElement(privateElement, "PrivateAction", privateActionElement);
        while (!privateActionElement.isNull()) {
            QDomElement routingActionElement;
            if (SimulationCommon::GetFirstChildElement(privateActionElement, "RoutingAction", routingActionElement)) {
                QDomElement childOfRoutingActionElement;
                if (SimulationCommon::GetFirstChildElement(privateActionElement, "AssignRouteAction", childOfRoutingActionElement)) {
                    QDomElement routeElement;
                    if (!SimulationCommon::GetFirstChildElement(childOfRoutingActionElement, "Route", routeElement)) {
                        return false;
                    }
                    QDomElement waypointElement;
                    if (!SimulationCommon::GetFirstChildElement(routeElement, "Waypoint", waypointElement)) {
                        return false;
                    }
                    else {
                        // use ptr here?
                        Waypoint waypoint;

                        while (!waypointElement.isNull()) {
                            QDomElement positionElement;
                            if (!SimulationCommon::GetFirstChildElement(waypointElement, "Position", positionElement)) {
                                return false;
                            }
                            QDomElement childOfPositionElement;
                            if (!SimulationCommon::GetFirstChildElement(positionElement, "RoadPosition", childOfPositionElement)) {
                                return false;
                            }
                            else {
                                if (!SimulationCommon::ParseAttributeString(childOfPositionElement, "RoadId", waypoint.roadId)) {
                                    return false;
                                }
                            }
                            if (SimulationCommon::GetFirstChildElement(positionElement, "LanePosition", childOfPositionElement)) {
                                int owlId;
                                SimulationCommon::ParseAttribute(childOfPositionElement, "LaneId", owlId);
                                waypoint.laneId = owlId;
                            }
                            waypointElement = waypointElement.nextSiblingElement("Waypoint");
                        }
                        dReaMRoute.waypoints.push_back(waypoint);
                    }
                }
                else {
                    // TODO: some other means of navigation was tried that cannot be handled. throw/output/log something?
                }
            }
            privateActionElement = privateActionElement.nextSiblingElement("PrivateAction");
        }
        dReaMRouteMap.insert(std::make_pair(id, dReaMRoute));

        privateElement = privateElement.nextSiblingElement("Private");
    }
    return true;
}

DReaMRoute RouteImporter::GetDReaMRoute(std::string id) {
    return dReaMRouteMap.at(id);
}