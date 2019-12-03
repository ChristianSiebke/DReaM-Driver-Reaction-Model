/*******************************************************************************
* Copyright (c) 2017, 2019 in-tech GmbH
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

#include <boost/algorithm/string/trim.hpp>

namespace TAG = openpass::importer::xml::manipulatorImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::manipulatorImporter::attribute;

namespace Importer
{
std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulator(QDomElement& eventElement,
                                                                                const std::string& sequenceName)
{
    QDomElement actionElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(eventElement, TAG::action, actionElement),
                  "Could not import Manipulator. Tag " + std::string(TAG::action) + " is missing.");

    QDomElement actionTypeElement;
    if (SimulationCommon::GetFirstChildElement(actionElement, TAG::userDefined, actionTypeElement))
    {
        return ImportManipulatorFromUserDefinedElement(actionTypeElement,
                                                       sequenceName);
    }

    if (SimulationCommon::GetFirstChildElement(actionElement, TAG::Private, actionTypeElement))
    {
        return ImportManipulatorFromPrivateElement(actionTypeElement,
                                                   sequenceName);
    }

    if (SimulationCommon::GetFirstChildElement(actionElement, TAG::global, actionTypeElement))
    {
        return ImportManipulatorFromGlobalElement(actionTypeElement,
                                                  sequenceName);
    }

    LogErrorAndThrow("Invalid Action Type in openScenario file");
}

std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulatorFromUserDefinedElement(QDomElement& userDefinedElement,
                                                                                                      const std::string& sequenceName)
{
    QDomElement userDefinedChildElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(userDefinedElement, TAG::command, userDefinedChildElement),
                  "Could not import Manipulator from user defined element. Tag " + std::string(TAG::command) + " is missing.");

    std::string command = userDefinedChildElement.text().toStdString();
    boost::algorithm::trim(command);

    return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::UserDefinedCommandAction>(sequenceName,
                                                                                                             command));
}

std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulatorFromPrivateElement(QDomElement& privateElement,
                                                                                                  const std::string& sequenceName)
{
    QDomElement privateChildElement;
    if(SimulationCommon::GetFirstChildElement(privateElement, TAG::lateral, privateChildElement))
    {
        QDomElement lateralChildElement;
        if (SimulationCommon::GetFirstChildElement(privateChildElement, TAG::laneChange, lateralChildElement))
        {
            QDomElement targetElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(lateralChildElement, TAG::target, targetElement),
                        "Could not import Manipulator from private element. Tag " + std::string(TAG::target) + " is missing.");

            QDomElement typeElement;
            if (SimulationCommon::GetFirstChildElement(targetElement, TAG::relative, typeElement))
            {
                std::string object;
                double value{};

                ThrowIfFalse(SimulationCommon::ParseAttributeString(typeElement, ATTRIBUTE::object, object),
                              "Could not import Manipulator from private element. Type tag requires a " + std::string(ATTRIBUTE::object) + " attribute.");
                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(typeElement, ATTRIBUTE::value, value),
                              "Could not import Manipulator from private element. Type tag requires a " + std::string(ATTRIBUTE::value) + " attribute.");

                return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::PrivateLateralLaneChangeAction>(sequenceName,
                                                                                                                               openScenario::PrivateLateralLaneChangeActionType::Relative,
                                                                                                                               static_cast<int>(std::rint(value)),
                                                                                                                               object));
            }
            else
            {
                ThrowIfFalse(SimulationCommon::GetFirstChildElement(targetElement, TAG::absolute, typeElement),
                              "Could not import Manipulator from private element. Tag " + std::string(TAG::absolute) + " is missing.");

                double value;
                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(typeElement, ATTRIBUTE::value, value),
                              "Could not import Manipulator from private element. Type tag requires a " + std::string(ATTRIBUTE::value) + " attribute.");

                return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::PrivateLateralLaneChangeAction>(sequenceName,
                                                                                                                               openScenario::PrivateLateralLaneChangeActionType::Absolute,
                                                                                                                               static_cast<int>(std::rint(value))));
            }
        }

        LogErrorAndThrow("Invalid PrivateAction-Lateral Type in openScenario file");
    }

    LogErrorAndThrow("Invalid PrivateAction Type in openScenario file");
}

std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulatorFromGlobalElement(QDomElement &globalElement,
                                                                                         const std::string& sequenceName)
{
    QDomElement globalActionTypeElement;
    if (SimulationCommon::GetFirstChildElement(globalElement, TAG::entity, globalActionTypeElement))
    {
        std::string name;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(globalActionTypeElement, ATTRIBUTE::name, name),
                      "Could not import Manipulator from global element. Entity tag requires a " + std::string(ATTRIBUTE::name) + " attribute.");

        QDomElement entityTypeElement;
        if (SimulationCommon::GetFirstChildElement(globalActionTypeElement, TAG::add, entityTypeElement))
        {
                return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::GlobalEntityAction>(sequenceName,
                                                                                                                   openScenario::GlobalEntityActionType::Add,
                                                                                                                   name));
        }
        else
        {
           ThrowIfFalse(SimulationCommon::GetFirstChildElement(globalActionTypeElement, TAG::Delete, entityTypeElement),
                         "Could not import Manipulator from global element. Tag " + std::string(TAG::Delete) + " is missing.");

           return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::GlobalEntityAction>(sequenceName,
                                                                                                              openScenario::GlobalEntityActionType::Delete,
                                                                                                              name));
        }
    }

    LogErrorAndThrow("Invalid GlobalAction Type in openScenario file.");
}
}
