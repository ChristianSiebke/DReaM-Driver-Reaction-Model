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

namespace TAG = openpass::importer::xml::profilesImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::profilesImporter::attribute;
using namespace Importer;
using namespace SimulationCommon;

void ProfilesImporter::ImportAgentProfiles(QDomElement agentProfilesElement,
                                           std::unordered_map<std::string, AgentProfile>& agentProfiles)
{
    QDomElement agentProfileElement;
    ThrowIfFalse(GetFirstChildElement(agentProfilesElement, TAG::agentProfile, agentProfileElement), "At least one agent profile is required");

    while (!agentProfileElement.isNull())
    {
        std::string agentProfileName;
        ThrowIfFalse(ParseAttributeString(agentProfileElement, ATTRIBUTE::name, agentProfileName), "AgentProfile name is invalid.");

        std::string profileType;
        ThrowIfFalse(ParseAttributeString(agentProfileElement, ATTRIBUTE::type, profileType), "AgentProifle type is invalid.");

        AgentProfile agentProfile;

        if (profileType == "Dynamic")
        {
            agentProfile.type = AgentProfileType::Dynamic;

            //Parses all driver profiles
            QDomElement driverProfilesElement;
            ThrowIfFalse((GetFirstChildElement(agentProfileElement, TAG::driverProfiles, driverProfilesElement)
                           && ImportProbabilityMap(driverProfilesElement, ATTRIBUTE::name, TAG::driverProfile, agentProfile.driverProfiles)),
                          "Could not import DriverProfiles.");

            //Parses all vehicle profiles
            QDomElement vehicleProfilesElement;
            ThrowIfFalse((GetFirstChildElement(agentProfileElement, TAG::vehicleProfiles, vehicleProfilesElement)
                           && ImportProbabilityMap(vehicleProfilesElement, ATTRIBUTE::name, TAG::vehicleProfile, agentProfile.vehicleProfiles)),
                          "Could not import VehicleProfiles");
        }
        else
        {
            ThrowIfFalse(profileType != "Static", "Invalid agent profile type.");

            agentProfile.type = AgentProfileType::Static;
            QDomElement systemElement;
            ThrowIfFalse(GetFirstChildElement(agentProfileElement, TAG::system, systemElement), "Could not import System.");

            std::string systemConfigFile;
            ThrowIfFalse(ParseString(systemElement, ATTRIBUTE::file, systemConfigFile), "Could not import SystemConfig file.");

            agentProfile.systemConfigFile = systemConfigFile;

            int systemId;
            ThrowIfFalse(ParseInt(systemElement, ATTRIBUTE::id, systemId), "Could not import System id.");

            agentProfile.systemId = systemId;

            std::string vehicleModel;
            ThrowIfFalse(ParseString(agentProfileElement, ATTRIBUTE::vehicleModel, vehicleModel), "Could not import VehicleModel.");

            agentProfile.vehicleModel = vehicleModel;
        }

        auto insertReturn = agentProfiles.insert({agentProfileName, agentProfile});
        ThrowIfFalse(insertReturn.second, "AgentProfile names need to be unique.");

        agentProfileElement = agentProfileElement.nextSiblingElement(TAG::agentProfile);
    }
}

void ProfilesImporter::ImportDriverProfiles(QDomElement driverProfilesElement,
                                                                         DriverProfiles& driverProfiles)
{
    QDomElement driverProfileElement;
    ThrowIfFalse(GetFirstChildElement(driverProfilesElement, TAG::driverProfile, driverProfileElement), "At least one driver profile is required.");

    while (!driverProfileElement.isNull())
    {
        std::string profileName;
        auto parameters = std::make_shared<ModelParameters>();

        ThrowIfFalse(ParseAttributeString(driverProfileElement, ATTRIBUTE::name, profileName),
                     "Could not import driver profile name.");

        try
        {
            ParameterImporter::ImportParameters(driverProfileElement, *parameters);
        }
        catch(const std::runtime_error &error)
        {
            LogErrorAndThrow("Could not import driver profile parameters: " + std::string(error.what()));
        }

        ThrowIfFalse(parameters->GetParametersString().find(ATTRIBUTE::type) != parameters->GetParametersString().end(), "Driver profile needs a type.");

        auto insertReturn = driverProfiles.emplace(profileName, parameters);
        ThrowIfFalse(insertReturn.second, "Driver profile names need to be unique.");

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
            auto parameters = std::make_shared<SimulationCommon::ModelParameters>();

            ThrowIfFalse(ParseAttributeString(vehicleComponentProfileElement, ATTRIBUTE::type, componentType), "Could not import component type.");
            ThrowIfFalse(ParseAttributeString(vehicleComponentProfileElement, ATTRIBUTE::name, profileName), "Could not import profile name.");

            try
            {
                ParameterImporter::ImportParameters(vehicleComponentProfileElement, *parameters);
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

            ThrowIfFalse(insertReturn.second, "Component profile names need to be unqiue.");

            vehicleComponentProfileElement = vehicleComponentProfileElement.nextSiblingElement(
                                                 QString::fromStdString(TAG::vehicleComponentProfile));
        }
    }
}

void ProfilesImporter::ImportSensorProfiles(QDomElement sensorProfilesElement, std::list<SensorProfile>& sensorProfiles)
{
    QDomElement sensorProfileElement;
    GetFirstChildElement(sensorProfilesElement, TAG::sensorProfile, sensorProfileElement);

    while (!sensorProfileElement.isNull())
    {
        std::string profileName;
        std::string sensorType;
        auto parameters = std::make_shared<SimulationCommon::Parameters>();

        ThrowIfFalse(ParseAttributeString(sensorProfileElement, ATTRIBUTE::name, profileName), "Sensor profile needs a name.");

        ThrowIfFalse(ParseAttributeString(sensorProfileElement, ATTRIBUTE::type, sensorType), "Sensor profile needs a type.");

        try
        {
            ParameterImporter::ImportParameters(sensorProfileElement, *parameters);
        }
        catch(const std::runtime_error &error)
        {
            LogErrorAndThrow("Could not import sensor parameters: " + std::string(error.what()));
        }

        sensorProfiles.emplace_back(SensorProfile{profileName, sensorType, parameters});
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

        ThrowIfFalse(ParseAttributeInt(sensorLinkElement, ATTRIBUTE::sensorId, sensorId), "Sensor link needs a SensorId.");
        ThrowIfFalse(ParseAttributeString(sensorLinkElement, ATTRIBUTE::inputId, inputId), "Sensor link needs a InputId.");

        SensorLink sensorLink{};
        sensorLink.sensorId = sensorId;
        sensorLink.inputId = inputId;
        sensorLinks.push_back(sensorLink);

        sensorLinkElement = sensorLinkElement.nextSiblingElement(TAG::sensorLink);
    }
}

void ProfilesImporter::ImportVehicleComponent(QDomElement vehicleComponentElement, VehicleComponent& vehicleComponent)
{
    ThrowIfFalse(ParseAttributeString(vehicleComponentElement, ATTRIBUTE::type, vehicleComponent.type), " Could not import vehicle component type");

    QDomElement profilesElement;
    GetFirstChildElement(vehicleComponentElement, TAG::profiles, profilesElement);
    ThrowIfFalse(ImportProbabilityMap(profilesElement, ATTRIBUTE::name, "Profile", vehicleComponent.componentProfiles, false), "Could not import ComponentProfile.");

    QDomElement sensorLinksElement;
    GetFirstChildElement(vehicleComponentElement, TAG::sensorLinks, sensorLinksElement);
    ImportSensorLinksOfComponent(sensorLinksElement, vehicleComponent.sensorLinks);
}

void ProfilesImporter::ImportAllVehicleComponentsOfVehicleProfile(QDomElement vehicleProfileElement,
                                                                                                                     VehicleProfile& vehicleProfile)
{
    QDomElement vehicleComponentsElement;
    ThrowIfFalse(GetFirstChildElement(vehicleProfileElement, TAG::components, vehicleComponentsElement), "Missing Components tag.");

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

void ProfilesImporter::ImportSensorParameters(QDomElement sensorElement, SensorParameter& sensor)
{
    ThrowIfFalse(ParseAttributeInt(sensorElement, ATTRIBUTE::id, sensor.id), "Sensor needs an Id.");

    QDomElement positionElement;
    ThrowIfFalse(GetFirstChildElement(sensorElement, TAG::position, positionElement), "Sensor needs a position.");
    ThrowIfFalse(ParseAttributeString(positionElement, ATTRIBUTE::name, sensor.sensorPosition.name), "Sensorposition needs a Name.");
    ThrowIfFalse(ParseAttributeDouble(positionElement, ATTRIBUTE::longitudinal, sensor.sensorPosition.longitudinal), "Sensorposition needs a Longitudinal.");
    ThrowIfFalse(ParseAttributeDouble(positionElement, ATTRIBUTE::lateral, sensor.sensorPosition.lateral), "Sensorposition needs a Lateral.");
    ThrowIfFalse(ParseAttributeDouble(positionElement, ATTRIBUTE::height, sensor.sensorPosition.height), "Sensorposition needs a Height.");
    ThrowIfFalse(ParseAttributeDouble(positionElement, ATTRIBUTE::pitch, sensor.sensorPosition.pitch), "Sensorposition needs a Pitch.");
    ThrowIfFalse(ParseAttributeDouble(positionElement, ATTRIBUTE::yaw, sensor.sensorPosition.yaw), "Sensorposition needs a Yaw.");
    ThrowIfFalse(ParseAttributeDouble(positionElement, ATTRIBUTE::roll, sensor.sensorPosition.roll), "Sensorposition needs a Roll.");

    QDomElement profileElement;
    ThrowIfFalse(GetFirstChildElement(sensorElement, TAG::profile, profileElement), "Sensor needs a Profile.");
    ThrowIfFalse(ParseAttributeString(profileElement, ATTRIBUTE::type, sensor.sensorProfile.type), "SensorProfile needs a Type.");
    ThrowIfFalse(ParseAttributeString(profileElement, ATTRIBUTE::name, sensor.sensorProfile.name), "SensorProfile needs a Name.");
}

void ProfilesImporter::ImportAllSensorsOfVehicleProfile(QDomElement vehicleProfileElement,
        VehicleProfile& vehicleProfile)
{
    QDomElement sensorsElement;
    ThrowIfFalse(GetFirstChildElement(vehicleProfileElement, TAG::sensors, sensorsElement), "VehicleProfile has no Sensors Element.");

    QDomElement sensorElement;
    GetFirstChildElement(sensorsElement, TAG::sensor, sensorElement);
    while (!sensorElement.isNull())
    {
        SensorParameter sensor;
        ImportSensorParameters(sensorElement, sensor);

        vehicleProfile.sensors.push_back(sensor);
        sensorElement = sensorElement.nextSiblingElement(TAG::sensor);
    }
}

VehicleProfile ProfilesImporter::ImportVehicleProfile(QDomElement vehicleProfileElement)
{
    VehicleProfile vehicleProfile;

    QDomElement vehicleModelElement;
    ThrowIfFalse(GetFirstChildElement(vehicleProfileElement, TAG::model, vehicleModelElement), "VehicleProfile has no VehicleModel Element.");
    ThrowIfFalse(ParseAttributeString(vehicleModelElement, ATTRIBUTE::name, vehicleProfile.vehicleModel), "VehicleModel name is missing.");

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
        ThrowIfFalse(ParseAttributeString(vehicleProfileElement, ATTRIBUTE::name, profileName), "Vehicle profile name is missing.");

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
        ThrowIfFalse(document.setContent(xmlData), "invalid xml file format of file " + filename);

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

        return true;
    }
    catch (std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "Profiles import failed: " + std::string(e.what());
        return false;
    }

}
