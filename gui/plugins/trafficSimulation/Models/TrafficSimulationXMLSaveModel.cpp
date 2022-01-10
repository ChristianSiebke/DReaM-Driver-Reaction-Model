/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "TrafficSimulationXMLSaveModel.h"

bool TrafficSimulationXMLSaveModel::save(QString const &filepath,
                                         TrafficSimulationInterface const *const trafficSimulation)
{
    QFile file(filepath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QXmlStreamWriter xml(&file);

        xml.setAutoFormatting(true);
        xml.setAutoFormattingIndent(2);

        // Start Document
        xml.writeStartDocument();

        // Root Element
        xml.writeStartElement(KeyRoot);
        xml.writeAttribute("SchemaVersion", "0.6.1");

        // Save Profiles catalogue
        xml.writeTextElement(KeyProfilesCatalogue, trafficSimulation->getProfilesCatalogue());

        // Save Experiment
        saveExperiment(xml, trafficSimulation->getExperiment());

        // Save Environment
        saveEnvironment(xml, trafficSimulation->getEnvironment());

        // Save Scenario
        saveScenario(xml, trafficSimulation->getScenario());

        // Save Traffic
        saveTraffic(xml, trafficSimulation->getTraffic());

        xml.writeEndElement();
        // End Document
        xml.writeEndDocument();

        return true;
    }

    return false;
}

void TrafficSimulationXMLSaveModel::saveExperiment(QXmlStreamWriter &xml,
                                                   ExperimentInterface *experiment)
{
    xml.writeStartElement(KeyExperiment);

    xml.writeTextElement(KeyExperimentID, QString::number(experiment->getID()));
    xml.writeTextElement(KeyExperimentNumberInvocations, QString::number(experiment->getNumberInvocations()));
    xml.writeTextElement(KeyExperimentSeed, QString::number(experiment->getSeed()));

    saveLibraries(xml, experiment->getLibraries());
    saveLoggingGroups(xml, experiment->getLoggingGroups());

    xml.writeEndElement();
    xml.writeComment("#############################################################################################");
}

void TrafficSimulationXMLSaveModel::saveLibraries(QXmlStreamWriter &xml,
                                                  ExperimentInterface::Libraries *const libraries)
{
    xml.writeStartElement(KeyExperimentLibraries);

    for (auto type : libraries->keys())
        xml.writeTextElement(type, libraries->value(type));

    xml.writeEndElement();
}

void TrafficSimulationXMLSaveModel::saveLoggingGroups(QXmlStreamWriter &xml,
                                                      ExperimentInterface::LoggingGroups *const loggingGroups)
{
    xml.writeStartElement(KeyExperimentLoggingGroups);

    for (auto loggingGroup : *loggingGroups)
        xml.writeTextElement(KeyExperimentLoggingGroup, loggingGroup);

    xml.writeEndElement();
}

void TrafficSimulationXMLSaveModel::saveEnvironment(QXmlStreamWriter &xml,
                                                    EnvironmentInterface *environment)
{
    // Save Environment
    xml.writeStartElement(KeyEnvironment);
    xml.writeAttribute(QString("Name"), environment->getName());

    saveTimeOfDayMap(xml, environment->getTimeOfDayMap());

    saveVisibilityDistanceMap(xml, environment->getVisibilityDistanceMap());

    saveFrictionMap(xml, environment->getFrictionMap());

    saveWeatherMap(xml, environment->getWeatherMap());

    xml.writeEndElement();
    xml.writeComment("#############################################################################################");
}

void TrafficSimulationXMLSaveModel::saveTimeOfDayMap(QXmlStreamWriter &xml,
                                                     EnvironmentInterface::TimeOfDayMap *map)
{
    xml.writeStartElement(KeyTimeOfDayMap);
    for (auto item : map->values())
    {
        xml.writeStartElement(KeyTimeOfDay);
        xml.writeAttribute(QString("Value"), item->getValue());
        xml.writeAttribute(QString("Probability"), QString::number(item->getProbability()));
        xml.writeEndElement();
    }

    xml.writeEndElement();
}

void TrafficSimulationXMLSaveModel::saveVisibilityDistanceMap(QXmlStreamWriter &xml,
                                                              EnvironmentInterface::VisibilityDistanceMap *map)
{
    xml.writeStartElement(KeyVisibilityDistanceMap);
    for (auto item : map->values())
    {
        xml.writeStartElement(KeyVisibilityDistance);
        xml.writeAttribute(QString("Value"), item->getValue());
        xml.writeAttribute(QString("Probability"), QString::number(item->getProbability()));
        xml.writeEndElement();
    }

    xml.writeEndElement();
}

void TrafficSimulationXMLSaveModel::saveFrictionMap(QXmlStreamWriter &xml,
                                                    EnvironmentInterface::FrictionMap *map)
{
    xml.writeStartElement(KeyFrictionMap);
    for (auto item : map->values())
    {
        xml.writeStartElement(KeyFriction);
        xml.writeAttribute(QString("Value"), item->getValue());
        xml.writeAttribute(QString("Probability"), QString::number(item->getProbability()));
        xml.writeEndElement();
    }

    xml.writeEndElement();
}

void TrafficSimulationXMLSaveModel::saveWeatherMap(QXmlStreamWriter &xml,
                                                   EnvironmentInterface::WeatherMap *map)
{
    xml.writeStartElement(KeyWeatherMap);
    for (auto item : map->values())
    {
        xml.writeStartElement(KeyWeather);
        xml.writeAttribute(QString("Value"), item->getValue());
        xml.writeAttribute(QString("Probability"), QString::number(item->getProbability()));
        xml.writeEndElement();
    }

    xml.writeEndElement();
}

void TrafficSimulationXMLSaveModel::saveScenario(QXmlStreamWriter &xml,
                                                 ScenarioInterface *scenario)
{
    // Save Scenario Configs
    xml.writeStartElement(KeyScenario);
    xml.writeAttribute(QString("Name"), scenario->getName());

    xml.writeTextElement(KeyScenarioFile, scenario->getScenarioFile());

    xml.writeEndElement();
    xml.writeComment("#############################################################################################");
}

void TrafficSimulationXMLSaveModel::saveTraffic(QXmlStreamWriter &xml,
                                                TrafficInterface *traffic)
{
    xml.writeStartElement(KeyTraffic);
    xml.writeAttribute(QString("Name"), traffic->getName());

    saveTrafficParameters(xml, traffic);
    saveTrafficRegularLane(xml, traffic->getAgentsRegularLane());
    saveTrafficRightmostLane(xml, traffic->getAgentsRightMostLane());

    xml.writeEndElement();
    xml.writeComment("#############################################################################################");
}

void TrafficSimulationXMLSaveModel::saveTrafficParameters(QXmlStreamWriter &xml,
                                                          TrafficInterface *traffic)
{
    xml.writeStartElement(KeyTrafficParameters);

    saveTrafficVolumes(xml, traffic->getVolumes());
    saveTrafficPlatoonRates(xml, traffic->getPlatoonRates());
    saveTrafficVelocities(xml, traffic->getVelocities());
    saveTrafficHomogeneities(xml, traffic->getHomogeneities());

    xml.writeEndElement();
}

void TrafficSimulationXMLSaveModel::saveTrafficVolumes(QXmlStreamWriter &xml,
                                                       TrafficItemMapInterface *volumes)
{
    xml.writeStartElement(KeyTrafficVolumes);

    for (TrafficItemInterface *volume : volumes->values())
    {
        xml.writeStartElement(KeyTrafficVolume);
        xml.writeAttribute("Value", volume->getValue());
        xml.writeAttribute("Probability", QString("%1").arg(volume->getProbability()));
        xml.writeEndElement();
    }

    xml.writeEndElement();
}

void TrafficSimulationXMLSaveModel::saveTrafficPlatoonRates(QXmlStreamWriter &xml,
                                                            TrafficItemMapInterface *platoonRates)
{
    xml.writeStartElement(KeyTrafficPlatoonRates);

    for (TrafficItemInterface *platoonRate : platoonRates->values())
    {
        xml.writeStartElement(KeyTrafficPlatoonRate);
        xml.writeAttribute("Value", platoonRate->getValue());
        xml.writeAttribute("Probability", QString("%1").arg(platoonRate->getProbability()));
        xml.writeEndElement();
    }

    xml.writeEndElement();
}

void TrafficSimulationXMLSaveModel::saveTrafficVelocities(QXmlStreamWriter &xml,
                                                          TrafficItemMapInterface *velocities)
{
    xml.writeStartElement(KeyTrafficVelocities);

    for (TrafficItemInterface *velocity : velocities->values())
    {
        xml.writeStartElement(KeyTrafficVelocity);
        xml.writeAttribute("Value", velocity->getValue());
        xml.writeAttribute("Probability", QString("%1").arg(velocity->getProbability()));
        xml.writeEndElement();
    }

    xml.writeEndElement();
}

void TrafficSimulationXMLSaveModel::saveTrafficHomogeneities(QXmlStreamWriter &xml,
                                                             TrafficItemMapInterface *homogeneities)
{
    xml.writeStartElement(KeyTrafficHomogeneities);

    for (TrafficItemInterface *homogeneity : homogeneities->values())
    {
        xml.writeStartElement(KeyTrafficHomogeneity);
        xml.writeAttribute("Value", homogeneity->getValue());
        xml.writeAttribute("Probability", QString("%1").arg(homogeneity->getProbability()));
        xml.writeEndElement();
    }

    xml.writeEndElement();
}

void TrafficSimulationXMLSaveModel::saveTrafficRegularLane(QXmlStreamWriter &xml,
                                                           TrafficItemMapInterface *agents)
{
    xml.writeStartElement(KeyTrafficRegularLane);

    for (TrafficItemInterface *agent : agents->values())
    {
        xml.writeStartElement(KeyTrafficAgent);
        xml.writeAttribute("Name", agent->getValue());
        xml.writeAttribute("Probability", QString("%1").arg(agent->getProbability()));
        xml.writeEndElement();
    }

    xml.writeEndElement();
}

void TrafficSimulationXMLSaveModel::saveTrafficRightmostLane(QXmlStreamWriter &xml,
                                                             TrafficItemMapInterface *agents)
{
    xml.writeStartElement(KeyTrafficRightmostLane);

    for (TrafficItemInterface *agent : agents->values())
    {
        xml.writeStartElement(KeyTrafficAgent);
        xml.writeAttribute("Name", agent->getValue());
        xml.writeAttribute("Probability", QString("%1").arg(agent->getProbability()));
        xml.writeEndElement();
    }

    xml.writeEndElement();
}
