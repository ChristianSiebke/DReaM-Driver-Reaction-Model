/*******************************************************************************
* Copyright (c) 2017, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "manipulatorImporter.h"
#include "xmlParser.h"
#include "Common/openScenarioDefinitions.h"
#include "importerLoggingHelper.h"
#include "framework/directories.h"
#include <cmath>
#include <boost/algorithm/string/trim.hpp>

namespace TAG = openpass::importer::xml::manipulatorImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::manipulatorImporter::attribute;

namespace Importer
{
std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulator(QDomElement& eventElement,
                                                                                const std::string& eventName,
                                                                                const std::string& trajectoryCatalogPath)
{
    QDomElement actionElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(eventElement, TAG::action, actionElement),
                  eventElement, "Tag " + std::string(TAG::action) + " is missing.");

    QDomElement actionTypeElement;
    if (SimulationCommon::GetFirstChildElement(actionElement, TAG::userDefined, actionTypeElement))
    {
        return ImportManipulatorFromUserDefinedElement(actionTypeElement,
                                                       eventName);
    }

    if (SimulationCommon::GetFirstChildElement(actionElement, TAG::Private, actionTypeElement))
    {
        return ImportManipulatorFromPrivateElement(actionTypeElement,
                                                   eventName,
                                                   trajectoryCatalogPath);
    }

    if (SimulationCommon::GetFirstChildElement(actionElement, TAG::global, actionTypeElement))
    {
        return ImportManipulatorFromGlobalElement(actionTypeElement,
                                                  eventName);
    }

    LogErrorAndThrow("Invalid Action Type in OpenSCENARIO file");
}

std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulatorFromUserDefinedElement(QDomElement& userDefinedElement,
                                                                                                      const std::string& eventName)
{
    QDomElement userDefinedChildElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(userDefinedElement, TAG::command, userDefinedChildElement),
                 userDefinedElement, "Tag " + std::string(TAG::command) + " is missing.");

    std::string command = userDefinedChildElement.text().toStdString();
    boost::algorithm::trim(command);

    return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::UserDefinedCommandAction>(eventName,
                                                                                                             command));
}

std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulatorFromPrivateElement(QDomElement& privateElement,
                                                                                                  const std::string& eventName,
                                                                                                  const std::string& trajectoryCatalogPath)
{
    QDomElement privateChildElement;
    if(SimulationCommon::GetFirstChildElement(privateElement, TAG::lateral, privateChildElement))
    {
        QDomElement lateralChildElement;
        if (SimulationCommon::GetFirstChildElement(privateChildElement, TAG::laneChange, lateralChildElement))
        {
            QDomElement dynamicsElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(lateralChildElement, TAG::dynamics, dynamicsElement),
                        lateralChildElement, "Tag " + std::string(TAG::dynamics) + " is missing.");

            double dynamicsTarget;
            bool timeDefined = SimulationCommon::ParseAttributeDouble(dynamicsElement, ATTRIBUTE::time, dynamicsTarget);
            bool distanceDefined = SimulationCommon::ParseAttributeDouble(dynamicsElement, ATTRIBUTE::distance, dynamicsTarget);
            ThrowIfFalse(timeDefined || distanceDefined, dynamicsElement, "Either attribute time or distance is required for LaneChange action.");
            ThrowIfFalse(!timeDefined || !distanceDefined, dynamicsElement, "Only one of attribute time or distance may be defined for LaneChange action.");
            openScenario::LaneChangeParameter::DynamicsType dynamicsType{timeDefined ? openScenario::LaneChangeParameter::DynamicsType::Time : openScenario::LaneChangeParameter::DynamicsType::Distance};

            std::string shape;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(dynamicsElement, ATTRIBUTE::shape, shape),
                        dynamicsElement, "Tag " + std::string(ATTRIBUTE::shape) + " is missing.");
            ThrowIfFalse(shape == "sinusoidal", dynamicsElement, "Currently only shape sinusoidal supported for LaneChangeAction");

            QDomElement targetElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(lateralChildElement, TAG::target, targetElement),
                        lateralChildElement, "Tag " + std::string(TAG::target) + " is missing.");

            QDomElement typeElement;
            if (SimulationCommon::GetFirstChildElement(targetElement, TAG::relative, typeElement))
            {
                std::string object;
                int value{};

                ThrowIfFalse(SimulationCommon::ParseAttributeString(typeElement, ATTRIBUTE::object, object),
                             typeElement, "Attribute " + std::string(ATTRIBUTE::object) + " is missing.");
                ThrowIfFalse(SimulationCommon::ParseAttributeInt(typeElement, ATTRIBUTE::value, value),
                             typeElement, "Atrribute " + std::string(ATTRIBUTE::value) + " is missing.");

                return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::PrivateLateralLaneChangeAction>(eventName,
                                                                                                                               openScenario::LaneChangeParameter{openScenario::LaneChangeParameter::Type::Relative,
                                                                                                                                                                 value,
                                                                                                                                                                 object,
                                                                                                                                                                 dynamicsTarget,
                                                                                                                                                                 dynamicsType}));
            }
            else
            {
                ThrowIfFalse(SimulationCommon::GetFirstChildElement(targetElement, TAG::absolute, typeElement),
                             targetElement, "Tag " + std::string(TAG::absolute) + " is missing.");

                int value;
                ThrowIfFalse(SimulationCommon::ParseAttributeInt(typeElement, ATTRIBUTE::value, value),
                             typeElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing.");

                return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::PrivateLateralLaneChangeAction>(eventName,
                                                                                                                               openScenario::LaneChangeParameter{openScenario::LaneChangeParameter::Type::Absolute,
                                                                                                                                                                 value,
                                                                                                                                                                 "",
                                                                                                                                                                 dynamicsTarget,
                                                                                                                                                                 dynamicsType}));
            }
        }

        LogErrorAndThrow("Invalid PrivateAction-Lateral Type in openScenario file");
    }
    else if(SimulationCommon::GetFirstChildElement(privateElement, TAG::routing, privateChildElement))
    {
        QDomElement routingChildElement;
        if(SimulationCommon::GetFirstChildElement(privateChildElement, TAG::followTrajectory, routingChildElement))
        {
            openScenario::Trajectory trajectory;
            QDomElement trajectoryElement;
            if(!SimulationCommon::GetFirstChildElement(routingChildElement, TAG::trajectory, trajectoryElement))
            {
                QDomElement catalogReferenceElement;
                ThrowIfFalse(SimulationCommon::GetFirstChildElement(routingChildElement, TAG::catalogReference, catalogReferenceElement),
                             routingChildElement, "Tag " + std::string(TAG::trajectory) + " or " + std::string(TAG::catalogReference) + " is missing.");
                std::string catalogName;
                ThrowIfFalse(SimulationCommon::ParseAttribute(catalogReferenceElement, ATTRIBUTE::catalogName, catalogName),
                             catalogReferenceElement, "Attribute " + std::string(ATTRIBUTE::catalogName) + " is missing.");
                std::string entryName;
                ThrowIfFalse(SimulationCommon::ParseAttribute(catalogReferenceElement, ATTRIBUTE::entryName, entryName),
                             catalogReferenceElement, "Attribute " + std::string(ATTRIBUTE::entryName) + " is missing.");
                trajectoryElement = GetTrajectoryElementFromCatalog(catalogName, trajectoryCatalogPath, entryName);

            }
            ThrowIfFalse(SimulationCommon::ParseAttribute(trajectoryElement, ATTRIBUTE::name, trajectory.name),
                         trajectoryElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");
            QDomElement vertexElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(trajectoryElement, TAG::vertex, vertexElement),
                         trajectoryElement, "Tag " + std::string(TAG::vertex) + " is missing.");
            while (!vertexElement.isNull())
            {
                QDomElement positionElement;
                ThrowIfFalse(SimulationCommon::GetFirstChildElement(vertexElement, TAG::position, positionElement),
                             vertexElement, "Tag " + std::string(TAG::position) + " is missing.");
                QDomElement worldElement;
                ThrowIfFalse(SimulationCommon::GetFirstChildElement(positionElement, TAG::world, worldElement),
                             positionElement, "Tag " + std::string(TAG::world) + " is missing.");

                openScenario::TrajectoryPoint trajectoryPoint;
                ThrowIfFalse(SimulationCommon::ParseAttribute(vertexElement, ATTRIBUTE::reference, trajectoryPoint.time),
                             vertexElement, "Attribute " + std::string(ATTRIBUTE::reference) + " is missing.");
                ThrowIfFalse(SimulationCommon::ParseAttribute(worldElement, ATTRIBUTE::x, trajectoryPoint.x),
                             worldElement, "Attribute " + std::string(ATTRIBUTE::x) + " is missing.");
                ThrowIfFalse(SimulationCommon::ParseAttribute(worldElement, ATTRIBUTE::y, trajectoryPoint.y),
                             worldElement, "Attribute " + std::string(ATTRIBUTE::y) + " is missing.");
                ThrowIfFalse(SimulationCommon::ParseAttribute(worldElement, ATTRIBUTE::h, trajectoryPoint.yaw),
                             worldElement, "Attribute " + std::string(ATTRIBUTE::h) + " is missing.");
                trajectory.points.push_back(trajectoryPoint);

                vertexElement = vertexElement.nextSiblingElement(TAG::vertex);
            }

            return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::PrivateFollowTrajectoryAction>(eventName, trajectory));
        }

        LogErrorAndThrow("Invalid PrivateAction-Routing Type in openScenario file");
    }

    LogErrorAndThrow("Invalid PrivateAction Type in openScenario file");
}

std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulatorFromGlobalElement(QDomElement &globalElement,
                                                                                         const std::string& eventName)
{
    QDomElement globalActionTypeElement;
    if (SimulationCommon::GetFirstChildElement(globalElement, TAG::entity, globalActionTypeElement))
    {
        std::string name;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(globalActionTypeElement, ATTRIBUTE::name, name),
                     globalActionTypeElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

        QDomElement entityTypeElement;
        if (SimulationCommon::GetFirstChildElement(globalActionTypeElement, TAG::add, entityTypeElement))
        {
                return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::GlobalEntityAction>(eventName,
                                                                                                                   openScenario::GlobalEntityActionType::Add,
                                                                                                                   name));
        }
        else
        {
           ThrowIfFalse(SimulationCommon::GetFirstChildElement(globalActionTypeElement, TAG::Delete, entityTypeElement),
                        globalActionTypeElement, "Tag " + std::string(TAG::Delete) + " is missing.");

           return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::GlobalEntityAction>(eventName,
                                                                                                              openScenario::GlobalEntityActionType::Delete,
                                                                                                              name));
        }
    }

    LogErrorAndThrow("Invalid GlobalAction Type in openScenario file.");
}

QDomElement ManipulatorImporter::GetTrajectoryElementFromCatalog(const std::string& catalogName, const std::string& catalogPath, const std::string& entryName)
{
    std::locale::global(std::locale("C"));

    QFile xmlFile(QString::fromStdString(catalogPath)); // automatic object will be closed on destruction
    ThrowIfFalse(xmlFile.open(QIODevice::ReadOnly), "Could not open TrajectoryCatalog (" + catalogName + ")");

    QByteArray xmlData(xmlFile.readAll());
    QDomDocument document;
    QString errorMsg;
    int errorLine;
    ThrowIfFalse(document.setContent(xmlData, &errorMsg, &errorLine), "Invalid xml format (" + catalogName + ") in line " + std::to_string(errorLine) + ": " + errorMsg.toStdString());

    QDomElement documentRoot = document.documentElement();
    ThrowIfFalse(!documentRoot.isNull(), "TrajectoryCatalog has no document root");

    QDomElement catalogElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::catalog, catalogElement),
                 documentRoot, "Tag " + std::string(TAG::catalog) + " is missing.");

    QDomElement trajectoryElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(catalogElement, TAG::trajectory, trajectoryElement),
                 catalogElement, "Tag " + std::string(TAG::trajectory) + " is missing.");
    while (!trajectoryElement.isNull())
    {
        std::string name;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(trajectoryElement, ATTRIBUTE::name, name),
                     trajectoryElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");
        if (name == entryName)
        {
            return trajectoryElement;
        }
        trajectoryElement = trajectoryElement.nextSiblingElement(TAG::trajectory);
    }

    LogErrorAndThrow("Entry " + entryName + " not found in TrajectoryCatalog " + catalogName);
}
}
