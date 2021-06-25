/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "profilesImporter.h"
#include "common/sensorDefinitions.h"
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
            ThrowIfFalse(ImportProbabilityMap(driverProfilesElement, ATTRIBUTE::name, TAG::driverProfile, agentProfile.driverProfiles, LogErrorAndThrow),
                         driverProfilesElement, "Invalid probalities");

            //Parses all vehicle profiles
            QDomElement vehicleProfilesElement;
            ThrowIfFalse(GetFirstChildElement(agentProfileElement, TAG::vehicleProfiles, vehicleProfilesElement),
                         agentProfileElement, "Tag " + std::string(TAG::vehicleProfiles) + " is missing.");
            ThrowIfFalse(ImportProbabilityMap(vehicleProfilesElement, ATTRIBUTE::name, TAG::vehicleProfile, agentProfile.vehicleProfiles, LogErrorAndThrow),
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

void ProfilesImporter::ImportProfileGroups(ProfileGroups& profileGroups, QDomElement& profilesElement)
{
    QDomElement profileGroupElement;
    ThrowIfFalse(GetFirstChildElement(profilesElement, TAG::profileGroup, profileGroupElement),
                 "ProfileGroup element is missing.");

    while (!profileGroupElement.isNull())
    {
        std::string profileType;
        ThrowIfFalse(ParseAttributeString(profileGroupElement, ATTRIBUTE::type, profileType),
                     profileGroupElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");

        QDomElement profileElement;
        GetFirstChildElement(profileGroupElement, TAG::profile, profileElement);

        while (!profileElement.isNull())
        {
            std::string profileName;
            ThrowIfFalse(ParseAttributeString(profileElement, ATTRIBUTE::name, profileName),
                         profileElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

            openpass::parameter::ParameterSetLevel1 parameters {};
            try
            {
                parameters = openpass::parameter::Import(profileElement, profilesElement);
            }
            catch(const std::runtime_error &error)
            {
                LogErrorAndThrow("Could not import spawner profile parameters: " + std::string(error.what()));
            }

            auto insertReturn = profileGroups[profileType].emplace(profileName, parameters);
            ThrowIfFalse(insertReturn.second, profileElement, "Profile names need to be unique within a ProfileGroup");

            profileElement = profileElement.nextSiblingElement(TAG::profile);
        }
        profileGroupElement = profileGroupElement.nextSiblingElement(TAG::profileGroup);
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
    ThrowIfFalse(ImportProbabilityMap(profilesElement, ATTRIBUTE::name, "Profile", vehicleComponent.componentProfiles, LogErrorAndThrow, false),
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
        QString errorMsg{};
        int errorLine{};
        ThrowIfFalse(document.setContent(xmlData, &errorMsg, &errorLine), "Invalid xml format (" + filename + ") in line " + std::to_string(errorLine) + ": " + errorMsg.toStdString());

        ImporterCommon::validateProfilesCatalogXMLSchema(xmlFile.fileName(), xmlData);

        QDomElement documentRoot = document.documentElement();
        ThrowIfFalse(!documentRoot.isNull(), "invalid document root " + filename);

        std::string configVersion;
        ParseAttributeString(documentRoot, ATTRIBUTE::schemaVersion, configVersion);
        ThrowIfFalse(configVersion.compare(supportedConfigVersion) == 0, "ProfilesCatalog version is not supported. Supported version is " + std::string(supportedConfigVersion));

        QDomElement agentProfilesElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::agentProfiles, agentProfilesElement),
                     "AgentProfiles element is missing.");
        ImportAgentProfiles(agentProfilesElement, profiles.GetAgentProfiles());

        QDomElement vehicleProfilesElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::vehicleProfiles, vehicleProfilesElement),
                     "VehicleProfiles element is missing.");
        ImportVehicleProfiles(vehicleProfilesElement, profiles.GetVehicleProfiles());

        ImportProfileGroups(profiles.GetProfileGroups(), documentRoot);

        return true;
    }
    catch (std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "Profiles import failed: " + std::string(e.what());
        return false;
    }

}
