/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "RouteImporter.h"

namespace GlobalObserver::Routes {

RouteImporter::RouteImporter(std::string path) {
    if (!Import(path)) {
        std::cout << "Could not import: " + path << std::endl;
    }
}

bool RouteImporter::Import(const std::string &filename) {
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
    if (!SimulationCommon::GetFirstChildElement(documentRoot, "Storyboard", storyboardElement)) {
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
        std::vector<Import::Position> waypoints;
        if (!SimulationCommon::ParseAttributeString(privateElement, "entityRef", id)) {
            return false;
        }

        QDomElement privateActionElement;
        SimulationCommon::GetFirstChildElement(privateElement, "PrivateAction", privateActionElement);
        while (!privateActionElement.isNull()) {
            QDomElement routingActionElement;
            if (SimulationCommon::GetFirstChildElement(privateActionElement, "RoutingAction", routingActionElement)) {
                QDomElement childOfRoutingActionElement;
                if (SimulationCommon::GetFirstChildElement(routingActionElement, "AssignRouteAction", childOfRoutingActionElement)) {
                    QDomElement routeElement;
                    if (!SimulationCommon::GetFirstChildElement(childOfRoutingActionElement, "Route", routeElement)) {
                        return false;
                    }
                    QDomElement waypointElement;
                    if (!SimulationCommon::GetFirstChildElement(routeElement, "Waypoint", waypointElement)) {
                        return false;
                    }
                    else {
                        Import::Position position;

                        while (!waypointElement.isNull()) {
                            QDomElement positionElement;

                            if (!SimulationCommon::GetFirstChildElement(waypointElement, "Position", positionElement)) {
                                return false;
                            }

                            QDomElement childOfPositionElement;
                            if (SimulationCommon::GetFirstChildElement(positionElement, "RoadPosition", childOfPositionElement)) {
                                position = ImportRoadPosition(childOfPositionElement);
                            }
                            else if (SimulationCommon::GetFirstChildElement(positionElement, "LanePosition", childOfPositionElement)) {
                                position = ImportLanePosition(childOfPositionElement);
                            }
                            else {
                                std::string message = "Position type not supported. Currently supported are: lanePosition, roadPosition";
                                throw std::logic_error(message);
                            }
                            waypoints.push_back(position);
                            waypointElement = waypointElement.nextSiblingElement("Waypoint");
                        }
                    }
                }
                else {
                    // TODO: some other means of navigation was tried that cannot be handled. throw/output/log something?
                }
            }
            privateActionElement = privateActionElement.nextSiblingElement("PrivateAction");
        }
        waypointsMap.insert(std::make_pair(id, waypoints));
        privateElement = privateElement.nextSiblingElement("Private");
    }
    return true;
}

Import::LanePosition RouteImporter::ImportLanePosition(QDomElement positionElement) {
    Import::LanePosition lanePosition;
    if (!SimulationCommon::ParseAttributeDouble(positionElement, "s", lanePosition.s))
        throw std::logic_error(__FILE__ " " + std::to_string(__LINE__) + " Can not parse s coordinate");
    std::string laneIdString;
    SimulationCommon::ParseAttributeString(positionElement, "laneId", laneIdString);
    try {
        lanePosition.laneId = std::stoi(laneIdString);
    }
    catch (std::invalid_argument) {
        std::string message = "LaneId must be integer";
        throw std::logic_error(message);
    }
    catch (std::out_of_range) {
        std::string message = "LaneId is out of range";
        throw std::logic_error(message);
    }
    if (!SimulationCommon::ParseAttributeString(positionElement, "roadId", lanePosition.roadId))
        throw std::logic_error(__FILE__ " " + std::to_string(__LINE__) + " Can not parse roadId coordinate");

    return lanePosition;
}

Import::RoadPosition RouteImporter::ImportRoadPosition(QDomElement positionElement) {
    Import::RoadPosition roadPosition;
    if (!SimulationCommon::ParseAttributeDouble(positionElement, "s", roadPosition.s))
        throw std::logic_error(__FILE__ " " + std::to_string(__LINE__) + " Can not parse s coordinate");
    if (!SimulationCommon::ParseAttributeDouble(positionElement, "t", roadPosition.t))
        throw std::logic_error(__FILE__ " " + std::to_string(__LINE__) + " Can not parse t coordinate");
    if (!SimulationCommon::ParseAttributeString(positionElement, "roadId", roadPosition.roadId))
        throw std::logic_error(__FILE__ " " + std::to_string(__LINE__) + " Can not parse roadId coordinate");
    return roadPosition;
}

std::vector<Import::Position> RouteImporter::GetDReaMRoute(std::string id) {
    auto iter = waypointsMap.find(id);
    if (iter != waypointsMap.end()) {
        return waypointsMap.at(id);
    }
    return {};
}

} // namespace GlobalObserver::Routes
