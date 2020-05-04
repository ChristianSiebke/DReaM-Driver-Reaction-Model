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
namespace TAG_SCENARIO = openpass::importer::xml::scenarioImporter::tag;
namespace ATTRIBUTE_SCENARIO = openpass::importer::xml::scenarioImporter::attribute;

template <typename T>
T ParseAttributeHelper(const QDomElement& element, const char attributeName[], openScenario::Parameters& parameters, T&)
{
    return Importer::ParseAttribute<T>(element, attributeName, parameters);
}

namespace Importer
{
std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulator(QDomElement& eventElement,
                                                                                const std::string& eventName,
                                                                                const std::string& trajectoryCatalogPath,
                                                                                openScenario::Parameters& parameters)
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
                                                   trajectoryCatalogPath,
                                                   parameters);
    }

    if (SimulationCommon::GetFirstChildElement(actionElement, TAG::global, actionTypeElement))
    {
        return ImportManipulatorFromGlobalElement(actionTypeElement,
                                                  eventName,
                                                  parameters);
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
                                                                                                  const std::string& trajectoryCatalogPath,
                                                                                                  openScenario::Parameters& parameters)
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

            bool timeDefined = SimulationCommon::HasAttribute(dynamicsElement, ATTRIBUTE::time);
            bool distanceDefined = SimulationCommon::HasAttribute(dynamicsElement, ATTRIBUTE::distance);
            ThrowIfFalse(timeDefined || distanceDefined, dynamicsElement, "Either attribute time or distance is required for LaneChange action.");
            ThrowIfFalse(!timeDefined || !distanceDefined, dynamicsElement, "Only one of attribute time or distance may be defined for LaneChange action.");

            openScenario::LaneChangeParameter::DynamicsType dynamicsType;
            double dynamicsTarget;

            if (timeDefined)
            {
                dynamicsType = openScenario::LaneChangeParameter::DynamicsType::Time;
                dynamicsTarget = ParseAttribute<double>(dynamicsElement, ATTRIBUTE::time, parameters);
            }
            else
            {
                dynamicsType = openScenario::LaneChangeParameter::DynamicsType::Distance;
                dynamicsTarget = ParseAttribute<double>(dynamicsElement, ATTRIBUTE::distance, parameters);
            }

            std::string shape = ParseAttribute<std::string>(dynamicsElement, ATTRIBUTE::shape, parameters);
            ThrowIfFalse(shape == "sinusoidal", dynamicsElement, "Currently only shape sinusoidal supported for LaneChangeAction");

            QDomElement targetElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(lateralChildElement, TAG::target, targetElement),
                        lateralChildElement, "Tag " + std::string(TAG::target) + " is missing.");

            QDomElement typeElement;
            if (SimulationCommon::GetFirstChildElement(targetElement, TAG::relative, typeElement))
            {
                std::string object = ParseAttribute<std::string>(typeElement, ATTRIBUTE::object, parameters);
                int value = ParseAttribute<int>(typeElement, ATTRIBUTE::value, parameters);

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

                int value = ParseAttribute<int>(typeElement, ATTRIBUTE::value, parameters);

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
            openScenario::Parameters defaultParameters;
            openScenario::Parameters assignedParameters;
            if(!SimulationCommon::GetFirstChildElement(routingChildElement, TAG::trajectory, trajectoryElement))
            {
                QDomElement catalogReferenceElement;
                ThrowIfFalse(SimulationCommon::GetFirstChildElement(routingChildElement, TAG::catalogReference, catalogReferenceElement),
                             routingChildElement, "Tag " + std::string(TAG::trajectory) + " or " + std::string(TAG::catalogReference) + " is missing.");
                std::string catalogName = ParseAttribute<std::string>(catalogReferenceElement, ATTRIBUTE::catalogName, parameters);
                std::string entryName = ParseAttribute<std::string>(catalogReferenceElement, ATTRIBUTE::entryName, parameters);
                trajectoryElement = GetTrajectoryElementFromCatalog(catalogName, trajectoryCatalogPath, entryName, parameters);
                QDomElement parameterDeclarationElement;
                SimulationCommon::GetFirstChildElement(trajectoryElement, TAG_SCENARIO::parameterDeclaration, parameterDeclarationElement);
                if (!parameterDeclarationElement.isNull())
                {
                    ImportParameterDeclarationElement(parameterDeclarationElement, defaultParameters);
                }
                QDomElement parameterAssignmentsElement;
                SimulationCommon::GetFirstChildElement(catalogReferenceElement, TAG_SCENARIO::parameterAssignments, parameterAssignmentsElement);
                QDomElement parameterAssignmentElement;
                SimulationCommon::GetFirstChildElement(parameterAssignmentsElement, TAG_SCENARIO::parameterAssignment, parameterAssignmentElement);
                while (!parameterAssignmentElement.isNull())
                {
                    auto parameterName = ParseAttribute<std::string>(parameterAssignmentElement, ATTRIBUTE_SCENARIO::parameterRef, parameters);
                    auto foundParameter = defaultParameters.find(parameterName);
                    ThrowIfFalse(foundParameter != defaultParameters.cend(), parameterAssignmentElement, "Can not assign parameter \"" + parameterName + "\". No parameter with this name declared.");
                    std::visit([&](auto& value){
                        assignedParameters.insert({parameterName, ParseAttributeHelper(parameterAssignmentElement, ATTRIBUTE_SCENARIO::value, parameters, value)});
                        },
                        foundParameter->second);
                    parameterAssignmentElement = parameterAssignmentElement.nextSiblingElement(TAG_SCENARIO::parameterAssignment);
                }

            }
            else
            {
                defaultParameters = parameters;
            }
            trajectory.name = ParseAttribute<std::string>(trajectoryElement, ATTRIBUTE::name, defaultParameters, assignedParameters);
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
                trajectoryPoint.time = ParseAttribute<double>(vertexElement, ATTRIBUTE::reference, defaultParameters, assignedParameters);
                trajectoryPoint.x = ParseAttribute<double>(worldElement, ATTRIBUTE::x, defaultParameters, assignedParameters);
                trajectoryPoint.y = ParseAttribute<double>(worldElement, ATTRIBUTE::y, defaultParameters, assignedParameters);
                trajectoryPoint.yaw = ParseAttribute<double>(worldElement, ATTRIBUTE::h, defaultParameters, assignedParameters);
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
                                                                                                 const std::string& eventName,
                                                                                                 openScenario::Parameters& parameters)
{
    QDomElement globalActionTypeElement;
    if (SimulationCommon::GetFirstChildElement(globalElement, TAG::entity, globalActionTypeElement))
    {
        std::string name = ParseAttribute<std::string>(globalActionTypeElement, ATTRIBUTE::name, parameters);

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

QDomElement ManipulatorImporter::GetTrajectoryElementFromCatalog(const std::string& catalogName, const std::string& catalogPath, const std::string& entryName, openScenario::Parameters& parameters)
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
        std::string name = ParseAttribute<std::string>(trajectoryElement, ATTRIBUTE::name, parameters);
        if (name == entryName)
        {
            return trajectoryElement;
        }
        trajectoryElement = trajectoryElement.nextSiblingElement(TAG::trajectory);
    }

    LogErrorAndThrow("Entry " + entryName + " not found in TrajectoryCatalog " + catalogName);
}
}
