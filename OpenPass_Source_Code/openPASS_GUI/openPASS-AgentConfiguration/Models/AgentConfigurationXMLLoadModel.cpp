/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "openPASS-Project/ProjectInterface.h"

#include "AgentConfigurationXMLLoadModel.h"

#include <QFile>

bool AgentConfigurationXMLLoadModel::load(const QString &filepath,
                                          const AgentConfigurationInterface * const agentConfiguration)
{
    QFile file(filepath);
    bool success = true;
    QList<QString> keys = KeyListProfiles;

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QXmlStreamReader xml(&file);

        //verify Root element
        xml.readNextStartElement();

        if(xml.name() != KeyRoot)
            return false;

        while( xml.readNextStartElement() )
        {

            keys.removeAll(xml.name().toString());

            if( xml.name() == KeyAgentProfiles )
            {
                success = loadAgentProfiles(xml, agentConfiguration->getAgents()) && success;
            }
            else if (xml.name() == KeyDriverProfiles)
            {
                success = loadDriverProfiles(xml, agentConfiguration->getDriverProfiles()) && success;
            }
            else if (xml.name() == KeyVehicleProfiles)
            {
                success = loadVehicleProfiles(xml, agentConfiguration->getVehicleProfiles()) && success;
            }
            else
                xml.skipCurrentElement();
        }

        return success & keys.isEmpty();
    }

    return false;
}


bool AgentConfigurationXMLLoadModel::loadAgentProfiles(QXmlStreamReader &xml, AgentMapInterface *agentProfiles)
{

    bool success = true;

    while(xml.readNextStartElement())
    {
        if(xml.name() == KeyAgentProfile )
            success = loadAgentProfile(xml, agentProfiles) && success;

        else
            xml.skipCurrentElement();
    }

    return success;
}

bool AgentConfigurationXMLLoadModel::loadAgentProfile(QXmlStreamReader &xml, AgentMapInterface *agentProfiles)
{
    QList<QString> keys;
    bool success = true;

    AgentItemInterface::Name name = xml.attributes().value("Name").toString();
    agentProfiles->add(name);
    AgentItemInterface * agent = agentProfiles->getItem(name);
    AgentItemInterface::Type type = xml.attributes().value("Type").toString();
    agent->setType(type);

    if(type == "Dynamic")
    {
        keys = KeyListDynamicAgentProfile;

        while(xml.readNextStartElement())
        {
            keys.removeAll(xml.name().toString());

            if(xml.name() == KeyDriverProfiles)
                success = loadAgentDriverProfiles(xml, agent->getDriverProfiles()) && success;

            else if ( xml.name() == KeyVehicleProfiles )
                success = loadAgentVehicleProfiles(xml, agent->getVehicleProfiles()) && success;

            else
                xml.skipCurrentElement();
        }

        return success && keys.isEmpty();
    }
    else if(type == "Static")
    {
        keys = KeyListStaticAgentProfile;

        while(xml.readNextStartElement())
        {
            keys.removeAll(xml.name().toString());

            if(xml.name() == KeySystem)
                success = loadAgentSystem(xml, agent) && success;
            else if (xml.name() == KeyVehicleModel)
                success = loadAgentVehicleModel(xml, agent) && success;
            else
                xml.skipCurrentElement();

        }

        return success && keys.isEmpty();
    }
    else
        return false;
}

bool AgentConfigurationXMLLoadModel::loadAgentDriverProfiles(QXmlStreamReader &xml,
                                                             AgentItemInterface::DriverProfiles *driverProfiles)
{
    while(xml.readNextStartElement())
    {
        if(xml.name() == KeyDriverProfile)
        {
            AgentItemInterface::DriverProfile profile = xml.attributes().value("Name").toString();
            AgentItemInterface::Probability prob = xml.attributes().value("Probability").toDouble();
            driverProfiles->insert(profile,prob);
            xml.skipCurrentElement();
        }
        else
            xml.skipCurrentElement();
    }

   return true;
}

bool AgentConfigurationXMLLoadModel::loadAgentVehicleProfiles(QXmlStreamReader &xml,
                                                              AgentItemInterface::VehicleProfiles *vehicleProfiles)
{
    while(xml.readNextStartElement())
    {
        if(xml.name() == KeyVehicleProfile)
        {
            AgentItemInterface::VehicleProfile profile = xml.attributes().value("Name").toString();
            AgentItemInterface::Probability prob = xml.attributes().value("Probability").toDouble();
            vehicleProfiles->insert(profile,prob);
            xml.skipCurrentElement();
        }
        else
            xml.skipCurrentElement();
    }

   return true;
}

bool AgentConfigurationXMLLoadModel::loadAgentVehicleModel(QXmlStreamReader &xml, AgentItemInterface *agent)
{
    QList<QString> keys = KeyListVehicleModel;

    while(xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());

        if(xml.name() == KeyVehicleModelCatalogue)
            agent->setVehicleModelCatalogue(xml.readElementText());
        else if (xml.name() == KeyModel)
            agent->setVehicleModel(xml.readElementText());
        else
            xml.skipCurrentElement();
    }

    return keys.isEmpty();
}

bool AgentConfigurationXMLLoadModel::loadAgentSystem(QXmlStreamReader &xml, AgentItemInterface *agent)
{
    QList <QString> keys = KeyListSystem;

    QMap<AgentItemInterface::SystemID, AgentItemInterface::SystemTitle> Titles;
    bool success = true;

    // 1. determine system config
    xml.readNextStartElement();
    if(xml.name() == KeySystemFile)
    {
        agent->setSystemConfig(xml.readElementText());
        keys.removeAll(KeySystemFile);
    }
    else
        return false;

    // 2. if successful, go through system config and load system titles
    success = loadTitlesfromSystemConfig(agent->getSystemConfig(), Titles) && success;

    // 3. get system specification from xml
    xml.readNextStartElement();
    if( xml.name() == KeySystemID )
    {
        AgentItemInterface::SystemID id = xml.readElementText().toUInt();
        agent->getSystem()->first = id;
        agent->getSystem()->second = Titles[id];
        keys.removeAll(KeySystemID);
    }
    else
        xml.skipCurrentElement();

    xml.skipCurrentElement();
    return success && keys.isEmpty();
}

bool AgentConfigurationXMLLoadModel::loadTitlesfromSystemConfig(AgentItemInterface::SystemConfig sysConfig,
                                                                QMap<AgentItemInterface::SystemID, AgentItemInterface::SystemTitle> &Titles)
{
    QFile file(ProjectInterface::instance()->absoluteToConfigPath(sysConfig));
    AgentItemInterface::SystemID id = 0;
    AgentItemInterface::SystemTitle title;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QXmlStreamReader xml(&file);
        xml.readNextStartElement();
        if(xml.name() == "systems")
        {
            // Search for systems
            while(xml.readNextStartElement())
            {

                if(xml.name() == "system")
                {
                    // Read id and title attributes if system was found
                    while(xml.readNextStartElement())
                    {
                        if( xml.name() == "id")
                            id = xml.readElementText().toUInt();
                        else if( xml.name() == "title")
                             title = xml.readElementText();
                        else
                            xml.skipCurrentElement();
                    }

                    // insert the id and titles found into the map
                    Titles.insert(id,title);

                }
                else
                    xml.skipCurrentElement();
            }

            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool AgentConfigurationXMLLoadModel::loadDriverProfiles(QXmlStreamReader &xml,
                                                        DriverProfileMapInterface *driverProfiles)
{
    bool success = true;

    while(xml.readNextStartElement())
    {
        if(xml.name() == KeyDriverProfile)
        {
            DriverProfileMapInterface::Name name = xml.attributes().value("Name").toString();
            driverProfiles->add(name);
            DriverProfileMapInterface::Item * profile = driverProfiles->getItem(name);
            success = loadDriverProfileParameters(xml, profile->getParameters()) && success;
        }
        else
            xml.skipCurrentElement();
    }

    return success;

}

bool AgentConfigurationXMLLoadModel::loadDriverProfileParameters(QXmlStreamReader &xml,
                                                                 DriverProfileItemInterface::Parameters *parameters)
{

    while( xml.readNextStartElement() )
    {
        DriverProfileItemInterface::Parameters::ID id = parameters->generateID();
        parameters->add(id);
        DriverProfileItemInterface::Parameters::Item * param = parameters->getItem(id);

        ParameterItemInterface::Type type = xml.name().toString();
        ParameterItemInterface::Key key = xml.attributes().value("Key").toString();
        ParameterItemInterface::Value value = xml.attributes().value("Value").toString();
        ParameterItemInterface::Unit unit = xml.attributes().value("Unit").toString();

        param->setKey(key);
        param->setType(type);
        param->setUnit(unit);
        param->setValue(value);

        xml.skipCurrentElement();
    }

    return true;
}

bool AgentConfigurationXMLLoadModel::loadVehicleProfiles(QXmlStreamReader &xml,
                                                         VehicleProfileMapInterface *vehicleProfiles)
{
    bool success = true;

    while(xml.readNextStartElement())
    {
        if( xml.name() == KeyVehicleProfile )
        {
            VehicleProfileMapInterface::Name name = xml.attributes().value("Name").toString();
            vehicleProfiles->add(name);
            VehicleProfileItemInterface * profile = vehicleProfiles->getItem(name);

            success = loadVehicleProfile(xml, profile) && success;
        }
        else
            xml.skipCurrentElement();
    }

    return success;
}

bool AgentConfigurationXMLLoadModel::loadVehicleProfile(QXmlStreamReader &xml,
                                                        VehicleProfileItemInterface *profile)
{
    bool success = true;
    QList <QString> keys = KeyListVehicleProfile;

    while(xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());

        if( xml.name() == KeySystemConfig)
        {
            profile->setSystemConfig( xml.attributes().value("Path").toString() );
            xml.skipCurrentElement();
        }

        else if( xml.name() == KeyModel )
        {
            profile->setModelCatalogue(xml.attributes().value("Catalogue").toString());
            profile->setModel( xml.attributes().value("Name").toString() );
            xml.skipCurrentElement();
        }

        else if ( xml.name() == KeySystemProfiles )
        {
            success = loadSystemProfiles( xml, profile->getSystemProfiles());
        }

        else
            xml.skipCurrentElement();
    }

    return success && keys.isEmpty();
}

bool AgentConfigurationXMLLoadModel::loadSystemProfiles(QXmlStreamReader &xml,
                                                        VehicleProfileItemInterface::SystemProfiles *systemProfiles)
{
    bool success = true;

    while(xml.readNextStartElement())
    {
        if(xml.name() == KeySystemProfile)
        {
            VehicleProfileItemInterface::AlgoType type = xml.attributes().value("Type").toString();
            VehicleProfileItemInterface::Systems * systems = new VehicleProfileItemInterface::Systems;
            systemProfiles->insert(type, systems);
            success = loadSystemProfile(xml, systems) && success;
        }
        else
            xml.skipCurrentElement();
    }

    return success;
}

bool AgentConfigurationXMLLoadModel::loadSystemProfile(QXmlStreamReader &xml,
                                                        VehicleProfileItemInterface::Systems *systems)
{

    while( xml.readNextStartElement() )
    {
        if( xml.name() == KeySystem )
        {
            VehicleProfileItemInterface::System system;
            VehicleProfileItemInterface::Probability prob = xml.attributes().value("Probability").toDouble();
            system.first = xml.attributes().value("Id").toUInt();
            system.second = xml.attributes().value("Title").toString();
            systems->insert(system, prob);

            xml.skipCurrentElement();
        }
        else
            xml.skipCurrentElement();
    }

    return true;
}

