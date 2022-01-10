/********************************************************************************
 * Copyright (c) 2017 ITK Engineering GmbH
 *               2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "systemConfigImporter.h"

#include <algorithm>
#include <QCoreApplication>

#include "common/commonTools.h"
#include "importerLoggingHelper.h"
#include "importer/importerCommon.h"

using namespace SimulationCommon;

namespace TAG = openpass::importer::xml::systemConfigImporter::tag;

namespace Importer {

openpass::parameter::ParameterSetLevel1 SystemConfigImporter::ImportSystemParameters(QDomElement& parametersElement)
{
    openpass::parameter::ParameterSetLevel1 param;
    QDomElement parameterElement = parametersElement.firstChildElement("parameter");
    while (!parameterElement.isNull())
    {
        //int id = parameterElement.firstChildElement("id").text().toInt();
        std::string id = parameterElement.firstChildElement("id").text().toStdString();

        QString type = parameterElement.firstChildElement("type").text();
        QString value = parameterElement.firstChildElement("value").text();

        if (type == "int")
        {
            param.emplace_back(id, value.toInt());
        }
        else if (type == "double")
        {
            param.emplace_back(id, value.toDouble());
        }
        else if (type == "bool")
        {
            value = value.toLower();
            bool valueRobust = false;
            if (value == "true" || (value.size() == 1 && value.toInt() > 0))
            {
                valueRobust = true;
            }
            param.emplace_back(id, valueRobust);
        }
        else if (type == "string")
        {
            param.emplace_back(id, value.toStdString());
        }
        else if (type == "intVector")
        {
            std::vector<int> vector{};
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
            param.emplace_back(id, vector);
        }
        else if (type == "doubleVector")
        {
            std::vector<double>vector {};
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
            param.emplace_back(id, vector);
        }
        else if (type == "boolVector")
        {
            std::vector<bool> vector{};

            const auto items{CommonHelper::TokenizeString(value.toStdString())};

            for (const auto& item : items)
            {
                if (item == "0" || item == "false")
                {
                    vector.push_back(false);
                }
                else if (item == "1" || item == "true")
                {
                    vector.push_back(true);
                }
                else
                {
                    throw std::runtime_error("Invalid boolean value in SystemConfig parameter at line " + std::to_string(parametersElement.lineNumber()));
                }
            }

            param.emplace_back(id, vector);
        }
        else if (type == "normalDistribution")
        {
            QDomElement valueElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(parameterElement, "value", valueElement), parameterElement,
                         "Missing value");

            double min, max, mean, sd;
            ThrowIfFalse(SimulationCommon::Parse(valueElement, "min", min), valueElement,
                         "Missing min");
            ThrowIfFalse(SimulationCommon::Parse(valueElement, "max", max), valueElement,
                         "Missing max");

            if (!SimulationCommon::Parse(valueElement, "mean", mean))
            {
                ThrowIfFalse(SimulationCommon::Parse(valueElement, "mu", min), valueElement,
                             "Missing mean or mu");
            }

            if (!SimulationCommon::Parse(valueElement, "sd", sd))
            {
                ThrowIfFalse(SimulationCommon::Parse(valueElement, "sigma", sd), valueElement,
                             "Missing sd or sigma");
            }

            openpass::parameter::NormalDistribution distribution{mean, sd, min, max};
            param.emplace_back(id, distribution);
        }
        else if (type == "logNormalDistribution")
        {
            QDomElement valueElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(parameterElement, "value", valueElement), parameterElement,
                         "Missing value");

            double min, max, mean, sd{0}, mu{0}, sigma{0};
            ThrowIfFalse(SimulationCommon::Parse(valueElement, "min", min), valueElement,
                         "Missing min");
            ThrowIfFalse(SimulationCommon::Parse(valueElement, "max", max), valueElement,
                         "Missing max");

            if (SimulationCommon::Parse(valueElement, "mean", mean) &&
                SimulationCommon::Parse(valueElement, "sd", sd))
            {
                auto distribution =  openpass::parameter::LogNormalDistribution::CreateWithMeanSd(mean, sd, min, max);
                param.emplace_back(id, distribution);
            }
            else
            {
                ThrowIfFalse(SimulationCommon::Parse(valueElement, "mu", mu)
                             && SimulationCommon::Parse(valueElement, "sigma", sigma), valueElement,
                             "Missing mean and sd or mu and sigma");

                openpass::parameter::LogNormalDistribution distribution{mu, sigma, min, max};
                param.emplace_back(id, distribution);
            }
        }
        else if (type == "exponentialDistribution")
        {
            QDomElement valueElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(parameterElement, "value", valueElement), parameterElement,
                         "Missing value");

            double min, max, lambda, mean{0};
            ThrowIfFalse(SimulationCommon::Parse(valueElement, "min", min), valueElement,
                         "Missing min");
            ThrowIfFalse(SimulationCommon::Parse(valueElement, "max", max), valueElement,
                         "Missing max");

            if (!SimulationCommon::Parse(valueElement, "lambda", lambda))
            {
                ThrowIfFalse(SimulationCommon::Parse(valueElement, "mean", mean), valueElement,
                             "Missing mean or mu");
                lambda = 1 / mean;
            }

            openpass::parameter::ExponentialDistribution distribution{lambda, min, max};
            param.emplace_back(id, distribution);
        }
        else if (type == "uniformDistribution")
        {
            QDomElement valueElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(parameterElement, "value", valueElement), parameterElement,
                         "Missing value");

            double min, max;
            ThrowIfFalse(SimulationCommon::Parse(valueElement, "min", min), valueElement,
                         "Missing min");
            ThrowIfFalse(SimulationCommon::Parse(valueElement, "max", max), valueElement,
                         "Missing max");

            openpass::parameter::UniformDistribution distribution{min, max};
            param.emplace_back(id, distribution);
        }
        else if (type == "gammaDistribution")
        {
            QDomElement valueElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(parameterElement, "value", valueElement), parameterElement,
                         "Missing value");

            double min, max, mean, sd{0}, shape{0}, scale{0};
            ThrowIfFalse(SimulationCommon::Parse(valueElement, "min", min), valueElement,
                         "Missing min");
            ThrowIfFalse(SimulationCommon::Parse(valueElement, "max", max), valueElement,
                         "Missing max");

            if (SimulationCommon::Parse(valueElement, "mean", mean) &&
                SimulationCommon::Parse(valueElement, "sd", sd))
            {
                auto distribution =  openpass::parameter::GammaDistribution::CreateWithMeanSd(mean, sd, min, max);
                param.emplace_back(id, distribution);
            }
            else
            {
                ThrowIfFalse(SimulationCommon::Parse(valueElement, "shape", shape)
                             && SimulationCommon::Parse(valueElement, "scale", scale), valueElement,
                             "Missing mean and sd or mu and sigma");

                openpass::parameter::GammaDistribution distribution{shape, scale, min, max};
                param.emplace_back(id, distribution);
            }
        }
    
        parameterElement = parameterElement.nextSiblingElement(TAG::parameter);
    }
    return param;
}


bool SystemConfigImporter::ImportSystemConfigContent(const std::string& filename, QDomDocument& document)
{
    std::locale::global(std::locale("C"));

    if(!QFileInfo(QString::fromStdString(filename)).exists())
    {
        LOG_INTERN(LogLevel::Info) << "SystemConfig: " + filename + " does not exist.";
        return false;
    }

    QFile xmlFile(QString::fromStdString(filename)); // automatic object will be closed on destruction
    ThrowIfFalse(xmlFile.open(QIODevice::ReadOnly),
                 "an error occurred during agent type import: " + filename);

    QByteArray xmlData(xmlFile.readAll());
    QString errorMsg {};
    int errorLine {};
    ThrowIfFalse(document.setContent(xmlData, &errorMsg, &errorLine),
                 "invalid xml file format of file " + filename + " in line " + std::to_string(errorLine) + " : " + errorMsg.toStdString());

    return true;
}

bool SystemConfigImporter::Import(const std::string& filename,                                  
                                  std::shared_ptr<Configuration::SystemConfig> systemConfig)
{
    try
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
                ThrowIfFalse(SimulationCommon::ParseInt(systemElement, "id", agentId), systemElement, "Unable to retrieve agent id.");
                LOG_INTERN(LogLevel::DebugCore) << "agent type id: " << agentId <<
                                                   " *********************************************************";

                // retrieve agent priority
                int agentPriority;
                ThrowIfFalse(SimulationCommon::ParseInt(systemElement, "priority", agentPriority), systemElement, "Unable to retrieve agent priority.");
                ThrowIfFalse(0 <= agentPriority, "Invalid agent priority.");

                LOG_INTERN(LogLevel::DebugCore) << "agent type priority: " << agentPriority;

                // create agent
                ThrowIfFalse(0 == agentTypes.count(agentId), systemElement, "Duplicate agent id."); // avoid duplicate types

                std::shared_ptr<core::AgentType> agent = std::make_shared<core::AgentType>();
                ThrowIfFalse(agent != nullptr, "Agent is null");

                ThrowIfFalse(agentTypes.insert({agentId, agent}).second, systemElement, "Unable to add agent.");

                // parse components
                QDomElement componentsElement;
                ThrowIfFalse(SimulationCommon::GetFirstChildElement(systemElement, TAG::components, componentsElement),
                             systemElement, "Tag " + std::string(TAG::components) + " is missing.");

                QDomElement componentElement;
                if (SimulationCommon::GetFirstChildElement(componentsElement, TAG::component, componentElement))
                {
                    while (!componentElement.isNull())
                    {
                        // retrieve component id
                        std::string componentId;
                        ThrowIfFalse(SimulationCommon::ParseString(componentElement, "id", componentId), componentElement, "Unable to retrieve component id.");
                        LOG_INTERN(LogLevel::DebugCore) << "component type id: " << componentId <<
                                                           " ---------------------------------------------------------";

                        // retrieve component library
                        std::string library;
                        ThrowIfFalse(SimulationCommon::ParseString(componentElement, "library", library), componentElement, "Unable to retrieve component library.");
                        ThrowIfFalse(!library.empty(), componentElement, "Component library is empty.");
                        LOG_INTERN(LogLevel::DebugCore) << "library: " << library;

                        QDomElement scheduleElement = componentElement.firstChildElement("schedule");

                        // retrieve component priority
                        int componentPriority;
                        ThrowIfFalse(SimulationCommon::ParseInt(scheduleElement, "priority", componentPriority), scheduleElement, "Unable to retrieve component priority.");
                        ThrowIfFalse(0 <= componentPriority, scheduleElement, "Invalid component priority.");
                        LOG_INTERN(LogLevel::DebugCore) << "component priority: " << componentPriority;

                        // retrieve component offset time
                        int offsetTime = 0; // must be set to 0 for init tasks for scheduler
                        ThrowIfFalse(SimulationCommon::ParseInt(scheduleElement, "offset", offsetTime), scheduleElement, "Unable to retrieve component offset time");
                        ThrowIfFalse(0 <= offsetTime, scheduleElement, "Invalid component offset time.");
                        LOG_INTERN(LogLevel::DebugCore) << "offset time: " << offsetTime;

                        // retrieve component response time
                        int responseTime = 0; // must be set to 0 for init tasks for scheduler
                        ThrowIfFalse(SimulationCommon::ParseInt(scheduleElement, "response", responseTime), scheduleElement, "Unable to retrieve component response time.");
                        ThrowIfFalse(0 <= responseTime, scheduleElement, "Invalid component response time.");
                        LOG_INTERN(LogLevel::DebugCore) << "response time: " << responseTime;

                        // retrieve component cycle time
                        int cycleTime = 0; // must be set to 0 for init tasks for scheduler
                        ThrowIfFalse(SimulationCommon::ParseInt(scheduleElement, "cycle", cycleTime), scheduleElement, "Unable to retrieve component cycle time.");
                        ThrowIfFalse(0 <= cycleTime, scheduleElement, "Invalid component cycle time.");
                        LOG_INTERN(LogLevel::DebugCore) << "cycle time: " << cycleTime;

                        bool isInitComponent = false;
                        if (cycleTime == 0)
                        {
                            isInitComponent = true;
                        }

                        auto component = std::make_shared<core::ComponentType>(componentId,
                                                                                          isInitComponent,
                                                                                          componentPriority,
                                                                                          offsetTime,
                                                                                          responseTime,
                                                                                          cycleTime,
                                                                                          library);
                        ThrowIfFalse(component != nullptr, componentElement, "Component is null.");
                        ThrowIfFalse(agent->AddComponent(component), componentElement, "Unable to add component.");

                        // parse model parameters
                        LOG_INTERN(LogLevel::DebugCore) << "import model parameters...";
                        QDomElement parametersElement;

                        ThrowIfFalse(SimulationCommon::GetFirstChildElement(componentElement, TAG::parameters, parametersElement),
                                     componentElement, "Tag " + std::string(TAG::parameters) + " is missing.");

                        try
                        {
                            component->SetModelParameter(ImportSystemParameters(parametersElement));
                        }
                        catch(const std::runtime_error& error)
                        {
                            LogErrorAndThrow("Unable to import system parameters: " + std::string(error.what()));
                        }

                        componentElement = componentElement.nextSiblingElement(TAG::component);
                    } // component loop
                } // if components exist

                // parse connections
                QDomElement connectionsElement;
                ThrowIfFalse(SimulationCommon::GetFirstChildElement(systemElement, TAG::connections, connectionsElement),
                             systemElement, "Tag " + std::string(TAG::connections) + " is missing.");

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
    catch (const std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "SystemConfig import failed: " + std::string(e.what());
        return false;
    }
}

} //namespace importer
