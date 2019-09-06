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

#include <boost/algorithm/string/trim.hpp>

namespace Importer
{
std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulator(QDomElement& eventElement,
                                                                                const std::string& sequenceName)
{
    QDomElement actionElement;
    CHECKFALSE(SimulationCommon::GetFirstChildElement(eventElement, "Action", actionElement));

    QDomElement actionTypeElement;
    if (SimulationCommon::GetFirstChildElement(actionElement, "UserDefined", actionTypeElement))
    {
        return ImportManipulatorFromUserDefinedElement(actionTypeElement,
                                                       sequenceName);
    }
    else if (SimulationCommon::GetFirstChildElement(actionElement, "Private", actionTypeElement))
    {
        return ImportManipulatorFromPrivateElement(actionTypeElement,
                                                   sequenceName);
    }
    else if (SimulationCommon::GetFirstChildElement(actionElement, "Global", actionTypeElement))
    {
        return ImportManipulatorFromGlobalElement(actionTypeElement,
                                                  sequenceName);
    }
    else {
        LogAndThrowError("Invalid Action Type in openScenario file");
    }
}

std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulatorFromUserDefinedElement(QDomElement& userDefinedElement,
                                                                                                      const std::string& sequenceName)
{
    QDomElement userDefinedChildElement;
    CHECKFALSE(SimulationCommon::GetFirstChildElement(userDefinedElement, "Command", userDefinedChildElement))

    std::string command = userDefinedChildElement.text().toStdString();
    boost::algorithm::trim(command);

    return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::UserDefinedCommandAction>(sequenceName,
                                                                                                             command));
}

std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulatorFromPrivateElement(QDomElement& privateElement,
                                                                                                  const std::string& sequenceName)
{
    QDomElement privateChildElement;
    if(SimulationCommon::GetFirstChildElement(privateElement, "Lateral", privateChildElement))
    {
        QDomElement lateralChildElement;
        if (SimulationCommon::GetFirstChildElement(privateChildElement, "LaneChange", lateralChildElement))
        {
            QDomElement targetElement;
            CHECKFALSE(SimulationCommon::GetFirstChildElement(lateralChildElement, "Target", targetElement));

            QDomElement typeElement;
            if (SimulationCommon::GetFirstChildElement(targetElement, "Relative", typeElement))
            {
                std::string object;
                double value{};

                CHECKFALSE(SimulationCommon::ParseAttributeString(typeElement, "object", object));
                CHECKFALSE(SimulationCommon::ParseAttributeDouble(typeElement, "value", value));

                return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::PrivateLateralLaneChangeAction>(sequenceName,
                                                                                                                               openScenario::PrivateLateralLaneChangeActionType::Relative,
                                                                                                                               static_cast<int>(std::rint(value)),
                                                                                                                               object));
            }
            else
            {
                CHECKFALSE(SimulationCommon::GetFirstChildElement(targetElement, "Absolute", typeElement));

                double value;
                CHECKFALSE(SimulationCommon::ParseAttributeDouble(typeElement, "value", value));

                return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::PrivateLateralLaneChangeAction>(sequenceName,
                                                                                                                               openScenario::PrivateLateralLaneChangeActionType::Absolute,
                                                                                                                               static_cast<int>(std::rint(value))));
            }
        }
        else
        {
            LogAndThrowError("Invalid PrivateAction-Lateral Type in openScenario file");
        }
    }
    else
    {
        LogAndThrowError("Invalid PrivateAction Type in openScenario file");
    }
}

std::shared_ptr<ScenarioActionInterface> ManipulatorImporter::ImportManipulatorFromGlobalElement(QDomElement &globalElement,
                                                                                         const std::string& sequenceName)
{
    QDomElement globalActionTypeElement;
    if (SimulationCommon::GetFirstChildElement(globalElement, "Entity", globalActionTypeElement))
    {
        std::string name;
        CHECKFALSE(SimulationCommon::ParseAttributeString(globalActionTypeElement, "name", name));

        QDomElement entityTypeElement;
        if (SimulationCommon::GetFirstChildElement(globalActionTypeElement, "Add",entityTypeElement))
        {
                return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::GlobalEntityAction>(sequenceName,
                                                                                                                   openScenario::GlobalEntityActionType::Add,
                                                                                                                   name));
        }
        else
        {
           CHECKFALSE(SimulationCommon::GetFirstChildElement(globalActionTypeElement, "Delete",entityTypeElement));

           return std::shared_ptr<ScenarioActionInterface>(std::make_shared<openScenario::GlobalEntityAction>(sequenceName,
                                                                                                              openScenario::GlobalEntityActionType::Delete,
                                                                                                              name));
        }
    }
    else
    {
        LogAndThrowError("Invalid GlobalAction Type in openScenario file");
    }
}

[[ noreturn ]] void ManipulatorImporter::LogAndThrowError(const std::string &message)
{
    LOG_INTERN(LogLevel::Error) << message;
    throw std::runtime_error(message);
}
}
