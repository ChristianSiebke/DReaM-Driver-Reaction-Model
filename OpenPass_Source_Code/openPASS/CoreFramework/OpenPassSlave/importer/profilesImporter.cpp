/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "profilesImporter.h"
#include "CoreFramework/CoreShare/log.h"
#include "Common/sensorDefinitions.h"
#include "importerLoggingHelper.h"

using namespace Importer;
using namespace SimulationCommon;

namespace TAG = openpass::importer::xml::profilesImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::profilesImporter::attribute;

void ProfilesImporter::ImportAgentProfiles(QDomElement agentProfilesElement,
                                           std::unordered_map<std::string, AgentProfile>& agentProfiles)
{
    QDomElement agentProfileElement;
    ThrowIfFalse(GetFirstChildElement(agentProfilesElement, TAG::agentProfile, agentProfileElement),
                 agentProfilesElement, "Tag " + std::string(TAG::agentProfile) + " is missing.");

    while (!agentProfileElement.isNull())
    {
        std::string agentProfileName;
        ThrowIfFalse(ParseAttributeString(agentProfileElement, ATTRIBUTE::name, agentProfileName),
                     agentProfileElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

        std::string profileType;
        ThrowIfFalse(ParseAttributeString(agentProfileElement, ATTRIBUTE::type, profileType),
                     agentProfileElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");

        AgentProfile agentProfile;

        if (profileType == "Dynamic")
        {
            agentProfile.type = AgentProfileType::Dynamic;

            //Parses all driver profiles
            QDomElement driverProfilesElement;
            ThrowIfFalse(GetFirstChildElement(agentProfileElement, TAG::driverProfiles, driverProfilesElement),
                         agentProfileElement, "Tag " + std::string(TAG::driverProfiles) + " is missing.");
            ThrowIfFalse(ImportProbabilityMap(driverProfilesElement, ATTRIBUTE::name, TAG::driverProfile, agentProfile.driverProfiles),
                         driverProfilesElement, "Invalid probalities");

            //Parses all vehicle profiles
            QDomElement vehicleProfilesElement;
            ThrowIfFalse(GetFirstChildElement(agentProfileElement, TAG::vehicleProfiles, vehicleProfilesElement),
                         agentProfileElement, "Tag " + std::string(TAG::vehicleProfiles) + " is missing.");
            ThrowIfFalse(ImportProbabilityMap(vehicleProfilesElement, ATTRIBUTE::name, TAG::vehicleProfile, agentProfile.vehicleProfiles),
                         vehicleProfilesElement, "Invalid probalities");
        }
        else
        {
            ThrowIfFalse(profileType == "Static", "Invalid agent profile type.");

            agentProfile.type = AgentProfileType::Static;
            QDomElement systemElement;
            ThrowIfFalse(GetFirstChildElement(agentProfileElement, TAG::system, systemElement),
                         agentProfileElement, "Tag " + std::string(TAG::system) + " is missing.");

            std::string systemConfigFile;
            ThrowIfFalse(ParseString(systemElement, ATTRIBUTE::file, systemConfigFile),
                         systemElement, "Attribute " + std::string(ATTRIBUTE::file) + " is missing.");

            agentProfile.systemConfigFile = systemConfigFile;

            int systemId;
            ThrowIfFalse(ParseInt(systemElement, ATTRIBUTE::id, systemId),
                         systemElement, "Attribute " + std::string(ATTRIBUTE::id) + " is missing.");

            agentProfile.systemId = systemId;

            std::string vehicleModel;
            ThrowIfFalse(ParseString(agentProfileElement, ATTRIBUTE::vehicleModel, vehicleModel),
                         agentProfileElement, "Attribute " + std::string(ATTRIBUTE::vehicleModel) + " is missing.");

            agentProfile.vehicleModel = vehicleModel;
        }

        auto insertReturn = agentProfiles.insert({agentProfileName, agentProfile});
        ThrowIfFalse(insertReturn.second, agentProfileElement, "AgentProfile names need to be unique.");

        agentProfileElement = agentProfileElement.nextSiblingElement(TAG::agentProfile);
    }
}

void ProfilesImporter::ImportSpawnPointProfiles(const QDomElement& spawnPointProfilesElement, SpawnPointProfiles& spawnPointProfiles)
{
    QDomElement spawnPointProfileElement;
    GetFirstChildElement(spawnPointProfilesElement, TAG::spawnPointProfile, spawnPointProfileElement);

    while (!spawnPointProfileElement.isNull())
    {
        std::string profileName;
        ThrowIfFalse(ParseAttributeString(spawnPointProfileElement, ATTRIBUTE::name, profileName),
                     spawnPointProfilesElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

        openpass::parameter::Container parameters {};
        try
        {
            parameters = openpass::parameter::Import(spawnPointProfileElement);
        }
        catch(const std::runtime_error &error)
        {
            LogErrorAndThrow("Could not import driver profile parameters: " + std::string(error.what()));
        }

        auto insertReturn = spawnPointProfiles.emplace(profileName, parameters);
        ThrowIfFalse(insertReturn.second, spawnPointProfilesElement, "Spawn point profile names need to be unique");

        spawnPointProfileElement = spawnPointProfileElement.nextSiblingElement(TAG::spawnPointProfile);
    }
}

void ProfilesImporter::ImportDriverProfiles(QDomElement driverProfilesElement,
                                            DriverProfiles& driverProfiles)
{
    QDomElement driverProfileElement;
    GetFirstChildElement(driverProfilesElement, TAG::driverProfile, driverProfileElement);

    while (!driverProfileElement.isNull())
    {
        std::string profileName;
        ThrowIfFalse(ParseAttributeString(driverProfileElement, ATTRIBUTE::name, profileName),
                     driverProfileElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

		openpass::parameter::Container parameters {};
        try
        {
	        parameters = openpass::parameter::Import(driverProfileElement);
        }
        catch(const std::runtime_error &error)
        {
            LogErrorAndThrow("Could not import driver profile parameters: " + std::string(error.what()));
        }

        ThrowIfFalse(openpass::parameter::Get<std::string>(parameters, ATTRIBUTE::type).has_value(),
                     driverProfileElement, "Driver profile needs a type.");

        auto insertReturn = driverProfiles.emplace(profileName, parameters);
        ThrowIfFalse(insertReturn.second, driverProfileElement, "Driver profile names need to be unique.");

        driverProfileElement = driverProfileElement.nextSiblingElement(TAG::driverProfile);
    }
}


void ProfilesImporter::ImportAllVehicleComponentProfiles(QDomElement vehicleComponentProfilesElement,
                                                         std::unordered_map<std::string, VehicleComponentProfiles>& vehicleComponentProfilesMap)
{
    QDomElement vehicleComponentProfileElement;

    if (GetFirstChildElement(vehicleComponentProfilesElement, TAG::vehicleComponentProfile, vehicleComponentProfileElement))
    {
        while (!vehicleComponentProfileElement.isNull())
        {
            std::string componentType;
            std::string profileName;

            ThrowIfFalse(ParseAttributeString(vehicleComponentProfileElement, ATTRIBUTE::type, componentType),
                         vehicleComponentProfileElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");
            ThrowIfFalse(ParseAttributeString(vehicleComponentProfileElement, ATTRIBUTE::name, profileName),
                         vehicleComponentProfileElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

			openpass::parameter::Container parameters {};
            try
            {
                parameters = openpass::parameter::Import(vehicleComponentProfileElement);
            }
            catch (const std::runtime_error &error)
            {
                LogErrorAndThrow("Could not import vehicle component parameters: " + std::string(error.what()));
            }

            if (vehicleComponentProfilesMap.count(componentType) == 0)
            {
                VehicleComponentProfiles vehicleComponentProfiles;
                vehicleComponentProfilesMap.insert({componentType, vehicleComponentProfiles});
            }

            auto insertReturn = vehicleComponentProfilesMap.at(componentType).emplace(profileName, parameters);
            ThrowIfFalse(insertReturn.second, vehicleComponentProfileElement, "Component profile names need to be unqiue.");

            vehicleComponentProfileElement = vehicleComponentProfileElement.nextSiblingElement(
                                                 QString::fromStdString(TAG::vehicleComponentProfile));
        }
    }
}

void ProfilesImporter::ImportSensorProfiles(QDomElement sensorProfilesElement, openpass::sensors::Profiles& sensorProfiles)
{
    QDomElement sensorProfileElement;
    GetFirstChildElement(sensorProfilesElement, TAG::sensorProfile, sensorProfileElement);

    while (!sensorProfileElement.isNull())
    {
        std::string profileName;
        ThrowIfFalse(ParseAttributeString(sensorProfileElement, ATTRIBUTE::name, profileName),
                     sensorProfileElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

        std::string sensorType;
        ThrowIfFalse(ParseAttributeString(sensorProfileElement, ATTRIBUTE::type, sensorType),
                     sensorProfileElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");

        openpass::parameter::Container parameters {};
        try
        {
        	parameters = openpass::parameter::Import(sensorProfileElement);
        }
        catch(const std::runtime_error &error)
        {
            LogErrorAndThrow("Could not import sensor parameters: " + std::string(error.what()));
        }

        ThrowIfFalse(openpass::parameter::Get<openpass::parameter::NormalDistribution>(parameters, ATTRIBUTE::latency).has_value(),
                     sensorProfileElement, "Sensor profile parameter needs a latency");

        sensorProfiles.emplace_back(openpass::sensors::Profile{profileName, sensorType, parameters});
        sensorProfileElement = sensorProfileElement.nextSiblingElement(TAG::sensorProfile);
    }
}

void ProfilesImporter::ImportSensorLinksOfComponent(QDomElement sensorLinksElement, std::list<SensorLink>& sensorLinks)
{
    QDomElement sensorLinkElement;
    GetFirstChildElement(sensorLinksElement, TAG::sensorLink, sensorLinkElement);

    while (!sensorLinkElement.isNull())
    {
        int sensorId;
        std::string inputId;

        ThrowIfFalse(ParseAttributeInt(sensorLinkElement, ATTRIBUTE::sensorId, sensorId),
                     sensorLinkElement, "Attribute " + std::string(ATTRIBUTE::sensorId) + " is missing.");
        ThrowIfFalse(ParseAttributeString(sensorLinkElement, ATTRIBUTE::inputId, inputId),
                     sensorLinkElement, "Attribute " + std::string(ATTRIBUTE::inputId) + " is missing.");

        SensorLink sensorLink{};
        sensorLink.sensorId = sensorId;
        sensorLink.inputId = inputId;
        sensorLinks.push_back(sensorLink);

        sensorLinkElement = sensorLinkElement.nextSiblingElement(TAG::sensorLink);
    }
}

void ProfilesImporter::ImportVehicleComponent(QDomElement vehicleComponentElement, VehicleComponent& vehicleComponent)
{
    ThrowIfFalse(ParseAttributeString(vehicleComponentElement, ATTRIBUTE::type, vehicleComponent.type),
                 vehicleComponentElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");

    QDomElement profilesElement;
    ThrowIfFalse(GetFirstChildElement(vehicleComponentElement, TAG::profiles, profilesElement),
                 vehicleComponentElement, "Tag " + std::string(TAG::profiles) + " is missing.");
    ThrowIfFalse(ImportProbabilityMap(profilesElement, ATTRIBUTE::name, "Profile", vehicleComponent.componentProfiles, false),
                 profilesElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

    QDomElement sensorLinksElement;
    GetFirstChildElement(vehicleComponentElement, TAG::sensorLinks, sensorLinksElement);
    ImportSensorLinksOfComponent(sensorLinksElement, vehicleComponent.sensorLinks);
}

void ProfilesImporter::ImportAllVehicleComponentsOfVehicleProfile(QDomElement vehicleProfileElement,
                                                                  VehicleProfile& vehicleProfile)
{
    QDomElement vehicleComponentsElement;
    ThrowIfFalse(GetFirstChildElement(vehicleProfileElement, TAG::components, vehicleComponentsElement),
                 vehicleProfileElement, "Tag " + std::string(TAG::components) + " is missing.");

    QDomElement componentElement;
    GetFirstChildElement(vehicleComponentsElement, TAG::component, componentElement);

    while (!componentElement.isNull())
    {
        VehicleComponent vehicleComponent;
        ImportVehicleComponent(componentElement, vehicleComponent);

        vehicleProfile.vehicleComponents.push_back(vehicleComponent);
        componentElement = componentElement.nextSiblingElement(TAG::component);
    }
}

void ProfilesImporter::ImportSensorParameters(QDomElement sensorElement, openpass::sensors::Parameter& sensor)
{
    ThrowIfFalse(ParseAttributeInt(sensorElement, ATTRIBUTE::id, sensor.id),
                 sensorElement, "Attribute " + std::string(ATTRIBUTE::id) + " is missing.");

    QDomElement positionElement;
    ThrowIfFalse(GetFirstChildElement(sensorElement, TAG::position, positionElement),
                 sensorElement, "Tag " + std::string(TAG::position) + " is missing.");
    ThrowIfFalse(ParseAttributeString(positionElement, ATTRIBUTE::name, sensor.position.name),
                 positionElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");
    ThrowIfFalse(ParseAttributeDouble(positionElement, ATTRIBUTE::longitudinal, sensor.position.longitudinal),
                 positionElement, "Attribute " + std::string(ATTRIBUTE::longitudinal) + " is missing.");
    ThrowIfFalse(ParseAttributeDouble(positionElement, ATTRIBUTE::lateral, sensor.position.lateral),
                 positionElement, "Attribute " + std::string(ATTRIBUTE::lateral) + " is missing.");
    ThrowIfFalse(ParseAttributeDouble(positionElement, ATTRIBUTE::height, sensor.position.height),
                 positionElement, "Attribute " + std::string(ATTRIBUTE::height) + " is missing.");
    ThrowIfFalse(ParseAttributeDouble(positionElement, ATTRIBUTE::pitch, sensor.position.pitch),
                 positionElement, "Attribute " + std::string(ATTRIBUTE::pitch) + " is missing.");
    ThrowIfFalse(ParseAttributeDouble(positionElement, ATTRIBUTE::yaw, sensor.position.yaw),
                 positionElement, "Attribute " + std::string(ATTRIBUTE::yaw) + " is missing.");
    ThrowIfFalse(ParseAttributeDouble(positionElement, ATTRIBUTE::roll, sensor.position.roll),
                 positionElement, "Attribute " + std::string(ATTRIBUTE::roll) + " is missing.");

    QDomElement profileElement;
    ThrowIfFalse(GetFirstChildElement(sensorElement, TAG::profile, profileElement),
                 sensorElement, "Tag " + std::string(TAG::profile) + " is missing.");
    ThrowIfFalse(ParseAttributeString(profileElement, ATTRIBUTE::type, sensor.profile.type),
                 profileElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");
    ThrowIfFalse(ParseAttributeString(profileElement, ATTRIBUTE::name, sensor.profile.name),
                 profileElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");
}

void ProfilesImporter::ImportAllSensorsOfVehicleProfile(QDomElement vehicleProfileElement,
        VehicleProfile& vehicleProfile)
{
    QDomElement sensorsElement;
    ThrowIfFalse(GetFirstChildElement(vehicleProfileElement, TAG::sensors, sensorsElement),
                 vehicleProfileElement, "Tag " + std::string(TAG::sensors) + " is missing.");

    QDomElement sensorElement;
    GetFirstChildElement(sensorsElement, TAG::sensor, sensorElement);

    while (!sensorElement.isNull())
    {
        openpass::sensors::Parameter sensor;
        ImportSensorParameters(sensorElement, sensor);

        vehicleProfile.sensors.push_back(sensor);
        sensorElement = sensorElement.nextSiblingElement(TAG::sensor);
    }
}

VehicleProfile ProfilesImporter::ImportVehicleProfile(QDomElement vehicleProfileElement)
{
    VehicleProfile vehicleProfile;

    QDomElement vehicleModelElement;
    ThrowIfFalse(GetFirstChildElement(vehicleProfileElement, TAG::model, vehicleModelElement),
                 vehicleProfileElement, "Tag " + std::string(TAG::model) + " is missing.");
    ThrowIfFalse(ParseAttributeString(vehicleModelElement, ATTRIBUTE::name, vehicleProfile.vehicleModel),
                 vehicleModelElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

    ImportAllVehicleComponentsOfVehicleProfile(vehicleProfileElement, vehicleProfile);
    ImportAllSensorsOfVehicleProfile(vehicleProfileElement, vehicleProfile);

    return vehicleProfile;
}

void ProfilesImporter::ImportVehicleProfiles(QDomElement vehicleProfilesElement,
        std::unordered_map<std::string, VehicleProfile>& vehicleProfiles)
{
    QDomElement vehicleProfileElement;
    GetFirstChildElement(vehicleProfilesElement, TAG::vehicleProfile, vehicleProfileElement);

    while (!vehicleProfileElement.isNull())
    {
        std::string profileName;
        ThrowIfFalse(ParseAttributeString(vehicleProfileElement, ATTRIBUTE::name, profileName),
                     vehicleProfileElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

        auto vehicleProfile = ImportVehicleProfile(vehicleProfileElement);
        vehicleProfiles.insert(std::make_pair<std::string&, VehicleProfile&>(profileName, vehicleProfile));

        vehicleProfileElement = vehicleProfileElement.nextSiblingElement(TAG::vehicleProfile);
    }
}

bool ProfilesImporter::Import(const std::string& filename, Profiles& profiles)
{
    try
    {
        std::locale::global(std::locale("C"));

        QFile xmlFile(filename.c_str()); // automatic object will be closed on destruction
        ThrowIfFalse(xmlFile.open(QIODevice::ReadOnly), "an error occurred during profilesCatalog import");

        QByteArray xmlData(xmlFile.readAll());
        QDomDocument document;
        QString errorMsg;
        int errorLine;
        ThrowIfFalse(document.setContent(xmlData, &errorMsg, &errorLine), "Invalid xml format (" + filename + ") in line " + std::to_string(errorLine) + ": " + errorMsg.toStdString());

        QDomElement documentRoot = document.documentElement();
        ThrowIfFalse(!documentRoot.isNull(), "invalid document root " + filename);

        std::string configVersion;
        ParseAttributeString(documentRoot, ATTRIBUTE::schemaVersion, configVersion);
        ThrowIfFalse(configVersion.compare(supportedConfigVersion) == 0, "ProfilesCatalog version is not supported. Supported version is " + std::string(supportedConfigVersion));

        //Import agent profiles
        QDomElement agentProfilesElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::agentProfiles, agentProfilesElement),
                     "AgentProfiles element is missing.");
        ImportAgentProfiles(agentProfilesElement, profiles.GetAgentProfiles());

        //Import driver profiles
        QDomElement driverProfilesElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::driverProfiles, driverProfilesElement),
                     "DriverProfiles element is missing.");
        ImportDriverProfiles(driverProfilesElement, profiles.GetDriverProfiles());

        //Import all VehicleComponent profiles
        QDomElement vehicleComponentProfilesElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::vehicleComponentProfiles, vehicleComponentProfilesElement),
                     "VehicleComponentProfiles element is missing.");
        ImportAllVehicleComponentProfiles(vehicleComponentProfilesElement, profiles.GetVehicleComponentProfiles());

        //Import vehicle profiles
        QDomElement vehicleProfilesElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::vehicleProfiles, vehicleProfilesElement),
                     "VehicleProfiles element is missing.");
        ImportVehicleProfiles(vehicleProfilesElement, profiles.GetVehicleProfiles());

        //Import sensor profiles
        QDomElement sensorProfilesElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::sensorProfiles, sensorProfilesElement),
                     "SensorProfiles element is missing.");
        ImportSensorProfiles(sensorProfilesElement, profiles.GetSensorProfiles());
        
        //Import spawn point profiles
        QDomElement spawnPointProfilesElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::spawnPointProfiles, spawnPointProfilesElement),
                     "SpawnPointProfiles element is missing.");
        ImportSpawnPointProfiles(spawnPointProfilesElement, profiles.GetSpawnPointProfiles());

        return true;
    }
    catch (std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "Profiles import failed: " + std::string(e.what());
        return false;
    }

}
