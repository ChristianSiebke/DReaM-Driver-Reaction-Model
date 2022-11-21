/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "TrafficSimulationXMLLoadModel.h"

bool TrafficSimulationXMLLoadModel::load(const QString &filepath,
                                         TrafficSimulationInterface *const trafficSimulation)
{
    QFile file(filepath);
    bool success = true;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QXmlStreamReader xml(&file);

        //verify Root element
        xml.readNextStartElement();

        if (xml.name() != KeyRoot)
            return false;

        // set the Profiles catalogue and generate agents list
        xml.readNextStartElement();
        if (xml.name() == KeyProfilesCatalogue)
        {
            TrafficSimulationInterface::ProfilesCatalogue catalogue = xml.readElementText();
            trafficSimulation->setProfilesCatalogue(catalogue);
        }
        else
            return false;

        while (xml.readNextStartElement())
        {
            if (xml.name() == KeyExperiment)
                success = loadExperiment(xml, trafficSimulation->getExperiment()) && success;
            else if (xml.name() == KeyEnvironment)
            {
                trafficSimulation->getEnvironment()->setName(xml.attributes().value("Name").toString());
                success = loadEnvironment(xml, trafficSimulation->getEnvironment()) && success;
            }
            else if (xml.name() == KeyScenario)
            {
                trafficSimulation->getScenario()->setName(xml.attributes().value("Name").toString());
                success = loadScenarioConfig(xml, trafficSimulation->getScenario()) && success;
            }
            else if (xml.name() == KeyTraffic)
            {
                trafficSimulation->getTraffic()->setName(xml.attributes().value("Name").toString());
                success = loadTraffic(xml, trafficSimulation->getTraffic());
            }
            else
                xml.skipCurrentElement();
        }

        return success;
    }

    return false;
}

bool TrafficSimulationXMLLoadModel::loadAgentProfiles(TrafficSimulationInterface::ProfilesCatalogue catalogue,
                                                      QList<QString> &agents)
{
    agents.clear();
    QFile file(catalogue);

    if (!catalogue.isEmpty() && file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QXmlStreamReader xml(&file);

        // verify header
        xml.readNextStartElement();
        if (xml.name() != "Profiles")
            return false;

        while (xml.readNextStartElement())
        {
            if (xml.name() == "AgentProfiles")
            {
                while (xml.readNextStartElement())
                {
                    if (xml.name() == "AgentProfile")
                    {
                        agents << xml.attributes().value("Name").toString();
                        xml.skipCurrentElement();
                    }

                    else
                        xml.skipCurrentElement();
                }
            }
            else
                xml.skipCurrentElement();
        }
    }

    return true;
}

bool TrafficSimulationXMLLoadModel::loadExperiment(QXmlStreamReader &xml,
                                                   ExperimentInterface *experiment)
{
    bool success = true;
    QList<QString> elements = KeyListExperiment;

    while (xml.readNextStartElement())
    {
        if (xml.name() == KeyExperimentID)
        {
            xml.skipCurrentElement();
            elements.removeAll(KeyExperimentID);
        }
        else if (xml.name() == KeyExperimentNumberInvocations)
        {
            experiment->setNumberInvocations(xml.readElementText().toInt());
            elements.removeAll(KeyExperimentNumberInvocations);
        }
        else if (xml.name() == KeyExperimentSeed)
        {
            experiment->setSeed(xml.readElementText().toInt());
            elements.removeAll(KeyExperimentSeed);
        }
        else if (xml.name() == KeyExperimentLibraries)
        {
            success = loadLibraries(xml, experiment->getLibraries()) && success;
            elements.removeAll(KeyExperimentLibraries);
        }
        else if (xml.name() == KeyExperimentLoggingGroups)
        {
            success = loadLoggingGroups(xml, experiment->getLoggingGroups()) && success;
            elements.removeAll(KeyExperimentLoggingGroups);
        }
        else
            xml.skipCurrentElement();
    }

    return elements.isEmpty() && success;
}

bool TrafficSimulationXMLLoadModel::loadLibraries(QXmlStreamReader &xml,
                                                  ExperimentInterface::Libraries *const libraries)
{
    while (xml.readNextStartElement())
        libraries->insert(xml.name().toString(), xml.readElementText());

    return true;
}

bool TrafficSimulationXMLLoadModel::loadLoggingGroups(QXmlStreamReader &xml,
                                                      ExperimentInterface::LoggingGroups *const loggingGroups)
{
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeyExperimentLoggingGroup)
            loggingGroups->append(xml.readElementText());
        else
            return false;
    }

    return true;
}

bool TrafficSimulationXMLLoadModel::loadEnvironment(QXmlStreamReader &xml,
                                                    EnvironmentInterface *environment)
{
    bool success = true;
    QList<QString> keys = KeyListEnvironment;

    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());

        if (xml.name() == KeyTimeOfDayMap)
            success = loadTimeOfDayMap(xml, environment) && success;

        else if (xml.name() == KeyVisibilityDistanceMap)
            success = loadVisibilityDistanceMap(xml, environment) && success;

        else if (xml.name() == KeyFrictionMap)
            success = loadFrictionMap(xml, environment) && success;

        else if (xml.name() == KeyWeatherMap)
            success = loadWeatherMap(xml, environment) && success;

        else
            xml.skipCurrentElement();
    }

    return success && keys.isEmpty();
}

bool TrafficSimulationXMLLoadModel::loadTimeOfDayMap(QXmlStreamReader &xml, EnvironmentInterface *environment)
{
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeyTimeOfDay)
        {
            EnvironmentItemInterface *item = environment->createItem(xml.attributes().value("Value").toString(),
                                                                     xml.attributes().value("Probability").toDouble());

            EnvironmentInterface::ID id = (environment->getTimeOfDayMap()->isEmpty()) ? 1 : environment->getTimeOfDayMap()->lastKey() + 1;

            environment->getTimeOfDayMap()->insert(id, item);

            xml.readElementText();
        }
        else
        {
            xml.skipCurrentElement();
            return false;
        }
    }

    return true;
}

bool TrafficSimulationXMLLoadModel::loadVisibilityDistanceMap(QXmlStreamReader &xml, EnvironmentInterface *environment)
{
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeyVisibilityDistance)
        {
            EnvironmentItemInterface *item = environment->createItem(xml.attributes().value("Value").toString(),
                                                                     xml.attributes().value("Probability").toDouble());

            EnvironmentInterface::ID id = (environment->getVisibilityDistanceMap()->isEmpty()) ? 1 : environment->getVisibilityDistanceMap()->lastKey() + 1;

            environment->getVisibilityDistanceMap()->insert(id, item);

            xml.readElementText();
        }
        else
        {
            xml.skipCurrentElement();
            return false;
        }
    }

    return true;
}

bool TrafficSimulationXMLLoadModel::loadFrictionMap(QXmlStreamReader &xml, EnvironmentInterface *environment)
{
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeyFriction)
        {
            EnvironmentItemInterface *item = environment->createItem(xml.attributes().value("Value").toString(),
                                                                     xml.attributes().value("Probability").toDouble());

            EnvironmentInterface::ID id = (environment->getFrictionMap()->isEmpty()) ? 1 : environment->getFrictionMap()->lastKey() + 1;

            environment->getFrictionMap()->insert(id, item);

            xml.readElementText();
        }
        else
        {
            xml.skipCurrentElement();
            return false;
        }
    }

    return true;
}

bool TrafficSimulationXMLLoadModel::loadWeatherMap(QXmlStreamReader &xml, EnvironmentInterface *environment)
{
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeyWeather)
        {
            EnvironmentItemInterface *item = environment->createItem(xml.attributes().value("Value").toString(),
                                                                     xml.attributes().value("Probability").toDouble());

            EnvironmentInterface::ID id = (environment->getWeatherMap()->isEmpty()) ? 1 : environment->getWeatherMap()->lastKey() + 1;

            environment->getWeatherMap()->insert(id, item);

            xml.readElementText();
        }
        else
        {
            xml.skipCurrentElement();
            return false;
        }
    }

    return true;
}

bool TrafficSimulationXMLLoadModel::loadScenarioConfig(QXmlStreamReader &xml, ScenarioInterface *scenario)
{
    bool success = true;
    QList<QString> keys = KeyListScenario;

    while (xml.readNextStartElement())
    {
        if (xml.name() == KeyScenarioFile)
        {
            scenario->setScenarioFile(xml.readElementText());
            keys.removeAll(KeyScenarioFile);
        }
    }

    return keys.isEmpty() && success;
}

bool TrafficSimulationXMLLoadModel::loadEntities(ScenarioInterface *scenario, QString const &scenarioFile)
{
    bool success = true;
    QFile file(scenarioFile);

    // Read the scenario file and get Entities
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QXmlStreamReader xml(&file);
        xml.readNextStartElement();

        // verify openScenario Header
        if (xml.name() != "OpenSCENARIO")
            return false;

        // scan the scenario file for <entities>
        while (xml.readNextStartElement())
        {
            if (xml.name() == "Entities")
                success = loadScenarioObjects(xml, scenario->getEntities()) && success;

            else
                xml.skipCurrentElement();
        }
    }

    Q_EMIT scenario->setEntities();

    return success;
}

bool TrafficSimulationXMLLoadModel::loadScenarioObjects(QXmlStreamReader &xml, ScenarioInterface::Entities *entities)
{
    while (xml.readNextStartElement())
    {
        if (xml.name() == "Object")
        {
            ScenarioInterface::Name name = xml.attributes().value("name").toString();

            while (xml.readNextStartElement())
            {
                if (xml.name() == "CatalogReference")
                {
                    ScenarioInterface::CatalogueName catalogName = xml.attributes().value("catalogName").toString();
                    ScenarioInterface::EntryName entryName = xml.attributes().value("entryName").toString();
                    entities->insert(name, ScenarioInterface::CatalogueRef(catalogName, entryName));
                    xml.skipCurrentElement();
                }
                else
                    xml.skipCurrentElement();
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool TrafficSimulationXMLLoadModel::loadTraffic(QXmlStreamReader &xml, TrafficInterface *traffic)
{
    bool success = true;
    QList<QString> keys = KeyListTraffic;

    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());

        if (xml.name() == KeyTrafficParameters)
            success = loadTrafficParameters(xml, traffic) && success;

        else if (xml.name() == KeyTrafficRegularLane)
            success = loadTrafficAgents(xml, traffic->getAgentsRegularLane()) && success;

        else if (xml.name() == KeyTrafficRightmostLane)
            success = loadTrafficAgents(xml, traffic->getAgentsRightMostLane()) && success;
        else
            xml.skipCurrentElement();
    }

    return success && keys.isEmpty();
}

bool TrafficSimulationXMLLoadModel::loadTrafficParameters(QXmlStreamReader &xml, TrafficInterface *traffic)
{
    bool success = true;
    QList<QString> keys = KeyListTrafficParameters;

    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());

        if (xml.name() == KeyTrafficVolumes)
            success = loadTrafficVolumes(xml, traffic->getVolumes()) && success;

        else if (xml.name() == KeyTrafficPlatoonRates)
            success = loadTrafficPlatoonRates(xml, traffic->getPlatoonRates()) && success;

        else if (xml.name() == KeyTrafficVelocities)
            success = loadTrafficVelocities(xml, traffic->getVelocities()) && success;

        else if (xml.name() == KeyTrafficHomogeneities)
            success = loadTrafficHomogeneities(xml, traffic->getHomogeneities()) && success;

        else
            xml.skipCurrentElement();
    }

    return success && keys.isEmpty();
}

bool TrafficSimulationXMLLoadModel::loadTrafficVolumes(QXmlStreamReader &xml, TrafficItemMapInterface *volumes)
{
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeyTrafficVolume)
        {
            TrafficItemMapInterface::ID id = volumes->generateID();
            volumes->add(id, 0.0, "");
            TrafficItemInterface *volume = volumes->getItem(id);

            volume->setValue(xml.attributes().value("Value").toString());
            volume->setProbability(xml.attributes().value("Probability").toDouble());

            xml.readElementText();
        }
        else
        {
            xml.skipCurrentElement();
            return false;
        }
    }

    return true;
}

bool TrafficSimulationXMLLoadModel::loadTrafficPlatoonRates(QXmlStreamReader &xml, TrafficItemMapInterface *platoonRates)
{
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeyTrafficPlatoonRate)
        {
            TrafficItemMapInterface::ID id = platoonRates->generateID();
            platoonRates->add(id, 0, "");
            TrafficItemInterface *platoonRate = platoonRates->getItem(id);

            platoonRate->setValue(xml.attributes().value("Value").toString());
            platoonRate->setProbability(xml.attributes().value("Probability").toDouble());

            xml.readElementText();
        }
        else
        {
            xml.skipCurrentElement();
            return false;
        }
    }

    return true;
}

bool TrafficSimulationXMLLoadModel::loadTrafficVelocities(QXmlStreamReader &xml, TrafficItemMapInterface *velocities)
{
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeyTrafficVelocity)
        {
            TrafficItemMapInterface::ID id = velocities->generateID();
            velocities->add(id, 0, "");
            TrafficItemInterface *velocity = velocities->getItem(id);

            velocity->setValue(xml.attributes().value("Value").toString());
            velocity->setProbability(xml.attributes().value("Probability").toDouble());

            xml.readElementText();
        }
        else
        {
            xml.skipCurrentElement();
            return false;
        }
    }
    return true;
}

bool TrafficSimulationXMLLoadModel::loadTrafficHomogeneities(QXmlStreamReader &xml, TrafficItemMapInterface *homogeneities)
{
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeyTrafficHomogeneity)
        {
            TrafficItemMapInterface::ID id = homogeneities->generateID();
            homogeneities->add(id, 0, "");
            TrafficItemInterface *homogeneity = homogeneities->getItem(id);

            homogeneity->setValue(xml.attributes().value("Value").toString());
            homogeneity->setProbability(xml.attributes().value("Probability").toDouble());

            xml.readElementText();
        }
        else
        {
            xml.skipCurrentElement();
            return false;
        }
    }
    return true;
}

bool TrafficSimulationXMLLoadModel::loadTrafficAgents(QXmlStreamReader &xml, TrafficItemMapInterface *agents)
{
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeyTrafficAgent)
        {
            TrafficItemMapInterface::ID id = agents->generateID();
            agents->add(id, 0, "");
            TrafficItemInterface *agent = agents->getItem(id);

            agent->setValue(xml.attributes().value("Name").toString());
            agent->setProbability(xml.attributes().value("Probability").toDouble());

            xml.readElementText();
        }
        else
        {
            xml.skipCurrentElement();
            return false;
        }
    }

    return true;
}
