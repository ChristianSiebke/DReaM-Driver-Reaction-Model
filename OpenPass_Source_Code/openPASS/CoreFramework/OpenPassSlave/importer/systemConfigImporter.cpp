/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*               2017 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "importerLoggingHelper.h"
#include "systemConfigImporter.h"
#include <QCoreApplication>

using namespace SimulationCommon;

namespace TAG = openpass::importer::xml::systemConfigImporter::tag;

namespace Importer {

//! From parameterParser.cpp in openPASS_Source_Code/CoreFramework/CoreShare/
bool SystemConfigImporter::ImportSystemParameters(QDomElement& parametersElement, ParameterInterface* parameters)
{
    QDomElement parameterElement = parametersElement.firstChildElement("parameter");
    while (!parameterElement.isNull())
    {
        //int id = parameterElement.firstChildElement("id").text().toInt();
        std::string id = parameterElement.firstChildElement("id").text().toStdString();

        QString type = parameterElement.firstChildElement("type").text();
        QString value = parameterElement.firstChildElement("value").text();

        if (type == "int")
        {
            ThrowIfFalse(parameters->AddParameterInt(id, value.toInt()), "Unable to add int parameter.");
        }
        else if (type == "double")
        {
            ThrowIfFalse(parameters->AddParameterDouble(id, value.toDouble()), "Unable to add double parameter.");
        }
        else if (type == "bool")
        {
            value = value.toLower();
            bool valueRobust = false;
            if (value == "true" || (value.size() == 1 && value.toInt() > 0))
            {
                valueRobust = true;
            }
            ThrowIfFalse(parameters->AddParameterBool(id, valueRobust), "Unable to add bool parameter.");
        }
        else if (type == "string")
        {
            ThrowIfFalse(parameters->AddParameterString(id, value.toStdString()), "Unable to add string parameter.");
        }
        else if (type == "intVector")
        {
            std::vector<int> vector{};
            try
            {
                std::stringstream valueStream(value.toStdString());

                int item;
                while (valueStream >> item)
                {
                    vector.push_back(item);

                    if (valueStream.peek() == ',')
                    {
                        valueStream.ignore();
                    }
                }
            }
            catch (...)
            {
                return false;
            }
            ThrowIfFalse(parameters->AddParameterIntVector(id, vector), "an error occurred furing import of parameters");
        }
        else if (type == "doubleVector")
        {
            std::vector<double>vector {};
            try
            {
                std::stringstream valueStream(value.toStdString());

                double item;
                while (valueStream >> item)
                {
                    vector.push_back(item);

                    if (valueStream.peek() == ',')
                    {
                        valueStream.ignore();
                    }
                }
            }
            catch (...)
            {
                return false;
            }
            ThrowIfFalse(parameters->AddParameterDoubleVector(id, vector), "an error occurred during import of parameters");
        }
        else if (type == "boolVector")
        {
            std::vector<bool> vector{};
            try
            {
                std::stringstream valueStream(value.toStdString());

                bool item;
                while (valueStream >> item)
                {
                    vector.push_back(item);

                    if (valueStream.peek() == ',')
                    {
                        valueStream.ignore();
                    }
                }
            }
            catch (...)
            {
                return false;
            }
            ThrowIfFalse(!parameters->AddParameterBoolVector(id, vector), "an error occurred during import of parameters");
        }
        parameterElement = parameterElement.nextSiblingElement(TAG::parameter);
    }
    return true;
}


bool SystemConfigImporter::ImportSystemConfigContent(const std::string& filename, QDomDocument& document)
{
    std::locale::global(std::locale("C"));

    ThrowIfFalse(QFileInfo(QString::fromStdString(filename)).exists(),
                 "Configuration does not exist: " + filename);

    QFile xmlFile(QString::fromStdString(filename)); // automatic object will be closed on destruction
    ThrowIfFalse(xmlFile.open(QIODevice::ReadOnly),
                 "an error occurred during agent type import: " + filename);

    QByteArray xmlData(xmlFile.readAll());
    QString errorMsg;
    int errorLine;
    ThrowIfFalse(document.setContent(xmlData, &errorMsg, &errorLine),
                 "invalid xml file format of file " + filename + " in line " + std::to_string(errorLine) + " : " + errorMsg.toStdString());

    return true;
}

bool SystemConfigImporter::Import(const std::string& filename,
                                  std::shared_ptr<Configuration::SystemConfig> systemConfig)
{
    QDomDocument document;
    if (!ImportSystemConfigContent(filename, document))
    {
        return false;
    }

    QDomElement documentRoot = document.documentElement();
    if (documentRoot.isNull())
    {
        return false;
    }

    // parse agents
    auto& agentTypes = systemConfig->GetSystems();
    QDomElement systemElement;
    if (SimulationCommon::GetFirstChildElement(documentRoot, TAG::system, systemElement))
    {
        while (!systemElement.isNull())
        {
            // retrieve agent id
            int agentId;
            ThrowIfFalse(SimulationCommon::ParseInt(systemElement, "id", agentId), "Unable to retrieve agent id.");
            LOG_INTERN(LogLevel::DebugCore) << "agent type id: " << agentId <<
                                            " *********************************************************";

            // retrieve agent priority
            int agentPriority;
            ThrowIfFalse(SimulationCommon::ParseInt(systemElement, "priority", agentPriority), "Unable to retrieve agent priority.");
            ThrowIfFalse(0 <= agentPriority, "Invalid agent priority.");

            LOG_INTERN(LogLevel::DebugCore) << "agent type priority: " << agentPriority;

            // create agent
            ThrowIfFalse(0 == agentTypes.count(agentId), "Duplicate agent."); // avoid duplicate types

            std::shared_ptr<SimulationSlave::AgentType> agent = std::make_shared<SimulationSlave::AgentType>();
            ThrowIfFalse(agent != nullptr, "Agent is null");

            ThrowIfFalse(agentTypes.insert({agentId, agent}).second, "Unable to add agent.");

            // parse components
            QDomElement componentsElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(systemElement, TAG::components, componentsElement),
                          "Unable to parse components. Tag " + std::string(TAG::components) + " is missing.");

            QDomElement componentElement;
            if (SimulationCommon::GetFirstChildElement(componentsElement, TAG::component, componentElement))
            {
                while (!componentElement.isNull())
                {
                    // retrieve component id
                    std::string componentId;
                    ThrowIfFalse(SimulationCommon::ParseString(componentElement, "id", componentId), "Unable to retrieve component id.");
                    LOG_INTERN(LogLevel::DebugCore) << "component type id: " << componentId <<
                                                    " ---------------------------------------------------------";

                    // retrieve component library
                    std::string library;
                    ThrowIfFalse(SimulationCommon::ParseString(componentElement, "library", library), "Unable to retrieve component library.");
                    ThrowIfFalse(!library.empty(), "Component library is empty.");
                    LOG_INTERN(LogLevel::DebugCore) << "library: " << library;

                    QDomElement scheduleElement = componentElement.firstChildElement("schedule");

                    // retrieve component priority
                    int componentPriority;
                    ThrowIfFalse(SimulationCommon::ParseInt(scheduleElement, "priority", componentPriority), "Unable to retrieve component priority.");
                    ThrowIfFalse(0 <= componentPriority, "Invalid component priority.");
                    LOG_INTERN(LogLevel::DebugCore) << "component priority: " << componentPriority;

                    // retrieve component offset time
                    int offsetTime = 0; // must be set to 0 for init tasks for scheduler
                    ThrowIfFalse(SimulationCommon::ParseInt(scheduleElement, "offset", offsetTime), "Unable to retrieve component offset time");
                    ThrowIfFalse(0 <= offsetTime, "Invalid component offset time.");
                    LOG_INTERN(LogLevel::DebugCore) << "offset time: " << offsetTime;

                    // retrieve component response time
                    int responseTime = 0; // must be set to 0 for init tasks for scheduler
                    ThrowIfFalse(SimulationCommon::ParseInt(scheduleElement, "response", responseTime), "Unable to retrieve component response time.");
                    ThrowIfFalse(0 <= responseTime, "Invalid component response time.");
                    LOG_INTERN(LogLevel::DebugCore) << "response time: " << responseTime;

                    // retrieve component cycle time
                    int cycleTime = 0; // must be set to 0 for init tasks for scheduler
                    ThrowIfFalse(SimulationCommon::ParseInt(scheduleElement, "cycle", cycleTime), "Unable to retrieve component cycle time.");
                    ThrowIfFalse(0 <= cycleTime, "Invalid component cycle time.");
                    LOG_INTERN(LogLevel::DebugCore) << "cycle time: " << cycleTime;

                    bool isInitComponent = false;
                    if (cycleTime == 0)
                    {
                        isInitComponent = true;
                    }

                    auto component = std::make_shared<SimulationSlave::ComponentType>(componentId,
                                     isInitComponent,
                                     componentPriority,
                                     offsetTime,
                                     responseTime,
                                     cycleTime,
                                     library);
                    ThrowIfFalse(component != nullptr, "Component is null.");

                    auto parameters = systemConfig->AddModelParameters();
                    component->SetModelParameter(parameters);

                    ThrowIfFalse(agent->AddComponent(component), "Unable to add component.");


                    // parse model parameters
                    LOG_INTERN(LogLevel::DebugCore) << "import model parameters...";

                    QDomElement parametersElement;
                    ThrowIfFalse(SimulationCommon::GetFirstChildElement(componentElement, TAG::parameters, parametersElement),
                                  "Could not parse model parameters. Tag " + std::string(TAG::parameters) + " is missing.");

                    ThrowIfFalse(ImportSystemParameters(parametersElement, component->GetModelParameters()),
                                  "Unable to import system parameters");

                    componentElement = componentElement.nextSiblingElement(TAG::component);
                } // component loop
            } // if components exist

            // parse connections
            QDomElement connectionsElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(systemElement, TAG::connections, connectionsElement),
                          "Could not parse connections. Tag " + std::string(TAG::connections) + " is missing.");

            std::map<std::pair<std::string, int>, int> channelMap;
            QDomElement connectionElement;
            if (SimulationCommon::GetFirstChildElement(connectionsElement, TAG::connection, connectionElement))
            {
                while (!connectionElement.isNull())
                {
                    QDomElement sourceElement = connectionElement.firstChildElement(TAG::source);
                    std::string sourceId = sourceElement.firstChildElement(TAG::component).text().toStdString();
                    int sourceOutputId = sourceElement.firstChildElement(TAG::output).text().toInt();


                    int channelId = connectionElement.firstChildElement(TAG::id).text().toInt();
                    std::pair<std::string, int> componentPair = std::make_pair(sourceId, sourceOutputId);
                    std::map<std::pair<std::string, int>, int>::iterator channelIterator;
                    channelIterator = channelMap.find(componentPair);

                    if (channelIterator == channelMap.end())
                    {
                        //                        channelId = channelMap.size();
                        channelMap.emplace(std::make_pair(componentPair, channelId));
                        agent->AddChannel(channelId);

                        agent->GetComponents().at(sourceId)->AddOutputLink(sourceOutputId, channelId);
                    }
                    else
                    {
                        //                        channelId = channelIterator->second;
                    }

                    QDomElement targetElement = connectionElement.firstChildElement(TAG::target);
                    while (!targetElement.isNull())
                    {
                        std::string targetId = targetElement.firstChildElement(TAG::component).text().toStdString();
                        int targetInputId = targetElement.firstChildElement(TAG::input).text().toInt();
                        agent->GetComponents().at(targetId)->AddInputLink(targetInputId, channelId);
                        targetElement = targetElement.nextSiblingElement(TAG::target);
                    }

                    connectionElement = connectionElement.nextSiblingElement(TAG::connection);
                }
            }

            systemElement = systemElement.nextSiblingElement(TAG::system);
        }
    }

    return true;
}

} //namespace importer
