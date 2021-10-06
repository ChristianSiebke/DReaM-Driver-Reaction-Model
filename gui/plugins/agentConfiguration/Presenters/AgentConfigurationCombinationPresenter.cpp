/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "AgentConfigurationCombinationPresenter.h"

#include <QFileDialog>

AgentConfigurationCombinationPresenter::AgentConfigurationCombinationPresenter(AgentConfigurationInterface *agentConfiguration,
                                                                               AgentConfigurationView *agentConfigurationView,
                                                                               SystemInterface *system,
                                                                               ProjectInterface *const project,
                                                                               QObject *parent) :
    QObject(parent), agentConfiguration(agentConfiguration), agentConfigurationView(agentConfigurationView), currentVehicleProfile(nullptr), vehicleProfiles(agentConfiguration->getVehicleProfiles()), sensorProfiles(agentConfiguration->getSensorProfiles()), componentProfiles(agentConfiguration->getVehicleComponentProfiles()), system(system), project(project)
{
    connect(agentConfigurationView, &AgentConfigurationView::ExportSetup, this, &AgentConfigurationCombinationPresenter::convert);
}

void AgentConfigurationCombinationPresenter::convert()
{
    QString const filename = QFileDialog::getSaveFileName(
        agentConfigurationView, QObject::tr("openPASS / Save setup"),
        project->getConfigPath(), QStringLiteral("ProfilesCatalogue (*.xml)"));

    if (!filename.isEmpty())
    {
        // Clear registered profiles
        componentProfiles->clear();
        sensorProfiles->clear();

        // Go through all defined vehicle Profiles
        for (auto profile : vehicleProfiles->values())
        {
            bool dynamicMode = true;
            QString name = profile->getName();
            currentVehicleProfile = profile;
            currentVehicleProfile->getSensors()->clear();
            currentVehicleProfile->getVehicleComponents()->clear();

            // load all the XML structure within the system config
            QString path = project->absoluteToConfigPath(currentVehicleProfile->getSystemConfig());
            system->load(path, &dynamicMode);

            // go through all systems incorporated into the profile and add it to the profile in the way
            // needed for the combinationConfig
            for (auto type : currentVehicleProfile->getSystemProfiles()->keys())
                for (auto system : currentVehicleProfile->getSystems(type)->keys())
                    addSystem(system.second);
        }

        // Save the Combination Config
        AgentConfigurationCombinationXMLSaveModel::save(filename, agentConfiguration);
    }
}

void AgentConfigurationCombinationPresenter::addSystem(const QString &systemTitle)
{
    SystemItemInterface *currentSystem = nullptr;
    SystemComponentItemInterface *currentAlgo = nullptr;
    VehicleComponentItemInterface::Type type;
    VehicleComponentProfileMapInterface::ID componentProfile_id;

    // 1. Determine the system ID of system with given title
    for (auto item : system->getSystems()->values())
        if (item->getTitle() == systemTitle)
        {
            currentSystem = item;
            break;
        }

    if (currentSystem != nullptr)
    {
        // 2. Find the component of type "Algorithm". It will always take the first one it finds!
        for (auto item : currentSystem->getComponents()->values())
            if (item->getType() == SystemComponentItemInterface::Type::Algorithm)
            {
                currentAlgo = item;
                break;
            }

        if (currentAlgo != nullptr)
        {
            // 3. Find out, whether a VehicleComponent exists with given library
            type = currentAlgo->getTitle();
            //  type = currentAlgo->getLibrary();
            componentProfile_id = VehicleComponentProfileMapInterface::ID(currentSystem->getTitle(), type);

            if (currentVehicleProfile->getVehicleComponents()->contains(type))
            {
                // 3.a Is there already a vehicle component profiles corresponding to the system name?
                addVehicleComponentProfile(componentProfile_id, currentAlgo->getParameters());
            }
            else
            {
                // 3.b Create Vehicle component according to type
                currentVehicleProfile->getVehicleComponents()->add(type);

                // Add the vehicle component profile (the method checks whether it already exists
                addVehicleComponentProfile(componentProfile_id, currentAlgo->getParameters());
            }

            // 4. Link the vehicle component profile to the newly added component entry with a default probability assigned to the
            // current system!
            if (!currentVehicleProfile->getVehicleComponents()->getItem(type)->getProfiles()->contains(componentProfiles->getItem(componentProfile_id)))
            {
                double probability = currentVehicleProfile->getSystems(type)->value(QPair<unsigned int, QString>(currentSystem->getID(), currentSystem->getTitle()));

                currentVehicleProfile->getVehicleComponents()->getItem(type)->getProfiles()->insert(componentProfiles->getItem(componentProfile_id), probability);
            }

            // 5. Loop over all Sensors connected to the Algorithm
            for (auto connection : currentSystem->getConnections()->values())
            {
                if (connection->getTarget()->getComponent()->getID() == currentAlgo->getID() &&
                    connection->getSource()->getComponent()->getType() == SystemComponentItemInterface::Type::Sensor)
                {
                    // add the sensor to combination XML
                    addSensor(connection->getSource()->getComponent(),
                              currentVehicleProfile->getVehicleComponents()->getItem(type),
                              connection->getTarget());
                }
            }
        }
    }
}

void AgentConfigurationCombinationPresenter::addVehicleComponentProfile(VehicleComponentProfileMapInterface::ID id,
                                                                        SystemComponentParameterMapInterface *parameters)
{
    if (!componentProfiles->contains(id))
    {
        componentProfiles->add(id);
        VehicleComponentProfileItemInterface *profile = componentProfiles->getItem(id);

        for (auto parameter : parameters->values())
        {
            // generate ID of the parameters and add it to the profile parameters
            ParameterMapInterface::ID param_id = profile->getParameters()->generateID();
            profile->getParameters()->add(param_id);

            // adopt all the values of the system component parameters
            ParameterItemInterface *vehicleParameter = profile->getParameters()->getItem(param_id);
            vehicleParameter->setKey(parameter->getTitle());
            vehicleParameter->setType(parameter->getType());
            vehicleParameter->setValue(parameter->getValue());
        }
    }
}

void AgentConfigurationCombinationPresenter::addSensor(SystemComponentItemInterface *sensor,
                                                       VehicleComponentItemInterface *component,
                                                       const SystemComponentInputItemInterface *AlgoInput)
{
    SensorProfileItemInterface *profile = findSensorProfile(sensor);
    SensorItemInterface *vehicleSensor = findVehicleSensor(sensor, profile);
    component->getSensorLinks()->insert(vehicleSensor->getID(), AlgoInput->getTitle());
}

SensorProfileItemInterface *AgentConfigurationCombinationPresenter::findSensorProfile(SystemComponentItemInterface *sensor)
{
    SensorProfileMapInterface::ID id;
    SensorProfileMapInterface::Type type = sensor->getTitle();
    //sensor->getLibrary();

    // 1. Find sensor profile with given sensor parameters
    for (auto sensorProfile : sensorProfiles->values())
    {
        if (sensorProfile->getType() == type &&
            sensorParametersIdentical(sensorProfile->getParameters(), sensor->getParameters()))
            return sensorProfile;
    }

    // 2. no sensor profile has been found, so it is now created
    id = SensorProfileMapInterface::ID(sensorProfiles->generateName(type), type);

    sensorProfiles->add(id);

    ParameterMapInterface *profileParameters = sensorProfiles->getItem(id)->getParameters();
    for (auto parameter : sensor->getParameters()->values())
    {
        // exclude parameters with ID < 6, as these describe positions
        if (parameter->getID() >= 6)
        {
            // generate parameter ID and add parameter to profile
            ParameterMapInterface::ID param_id = profileParameters->generateID();
            profileParameters->add(param_id);

            // adopt the properties from the given sensor
            ParameterItemInterface *profileParameter = profileParameters->getItem(param_id);
            profileParameter->setKey(parameter->getTitle());
            profileParameter->setType(parameter->getType());
            profileParameter->setValue(parameter->getValue());
        }
    }

    return sensorProfiles->getItem(id);
}

bool AgentConfigurationCombinationPresenter::sensorParametersIdentical(ParameterMapInterface *profileParameters,
                                                                       SystemComponentParameterMapInterface *sensorParameters)
{
    QStringList ProfileValues;
    QStringList ParameterValues;

    for (auto profileParameter : profileParameters->values())
        ProfileValues << profileParameter->getValue();

    for (auto parameter : sensorParameters->values())
    {
        if (parameter->getID() >= 6)
            ParameterValues << parameter->getValue();
    }

    return (ProfileValues == ParameterValues);
}

SensorItemInterface *AgentConfigurationCombinationPresenter::findVehicleSensor(SystemComponentItemInterface *sensor,
                                                                               SensorProfileItemInterface *profile)
{
    for (auto vehicleSensor : currentVehicleProfile->getSensors()->values())
    {
        if (vehicleSensor->getSensorProfile()->getID() == profile->getID() &&
            sensorPositionsIdentical(sensor, vehicleSensor))
            return vehicleSensor;
    }

    SensorItemInterface::ID vehicleSensor_ID = currentVehicleProfile->getSensors()->generateID();
    currentVehicleProfile->getSensors()->add(vehicleSensor_ID);

    SensorItemInterface *vehicleSensor = currentVehicleProfile->getSensors()->getItem(vehicleSensor_ID);
    SensorPosition position;

    // Note: The position parameters are assumed to be the first 6 component parameters of the sensor!
    position.Name = QString("Default");

    position.longitudinal = sensor->getParameters()->getItem(0)->getValue().toDouble();
    position.lateral = sensor->getParameters()->getItem(1)->getValue().toDouble();
    position.height = sensor->getParameters()->getItem(2)->getValue().toDouble();

    position.pitch = sensor->getParameters()->getItem(3)->getValue().toDouble();
    position.yaw = sensor->getParameters()->getItem(4)->getValue().toDouble();
    position.roll = sensor->getParameters()->getItem(5)->getValue().toDouble();

    vehicleSensor->setSensorPosition(position);
    vehicleSensor->setSensorProfile(profile);

    return vehicleSensor;
}

bool AgentConfigurationCombinationPresenter::sensorPositionsIdentical(SystemComponentItemInterface *sensor,
                                                                      SensorItemInterface *vehicleSensor)
{
    SensorPosition position = vehicleSensor->getSensorPosition();

    QList<double> vehicleSensorCoordinates = {position.longitudinal, position.lateral, position.height,
                                              position.pitch, position.yaw, position.roll};

    QList<double> SensorCoordinates;

    // NOTE: Positions of the sensor are assumed to correspond to the first 6 parameters!
    for (unsigned int i = 0; i < 6; i++)
        SensorCoordinates << sensor->getParameters()->getItem(i)->getValue().toDouble();

    return (SensorCoordinates == vehicleSensorCoordinates);
}
