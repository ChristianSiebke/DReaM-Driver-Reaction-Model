/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentConfigurationCombinationPresenter.h
//! @ingroup agentConfigurationPlugin
//! @brief This class defines a presenter object for exporting the model data
//!        (AgentConfigurationInterface) of this plugin to the profiles catalogue
//!        XML file needed by the simulation.
//!
//!        The only reason why this class exists is because, for a dynamic agent
//!        profile, the profiles catalogue partly repeats information already
//!        contained in a system config file. In order to maintain a consistent
//!        way of defining systems using the system config, this presenter object
//!        gathers the required information from the system config and exports it
//!        into the format needed by the profiles catalogue.
//!
//! @note  In future releases, the way of defining dynamic and static agent will
//!        be unified in order to avoid such a doubling of information in two
//!        different files. Hence, this class is only temporary and we refrain
//!        from giving a detailed documentation here.
//-----------------------------------------------------------------------------

#ifndef VEHICLEPROFILESCOMBINATIONPRESENTER_H
#define VEHICLEPROFILESCOMBINATIONPRESENTER_H

#include <QObject>

#include "Models/AgentConfigurationCombinationXMLSaveModel.h"
#include "Views/AgentConfigurationView.h"
#include "openPASS-AgentConfiguration/SensorProfileMapInterface.h"
#include "openPASS-AgentConfiguration/VehicleComponentProfileMapInterface.h"
#include "openPASS-AgentConfiguration/VehicleProfileMapInterface.h"
#include "openPASS-Project/ProjectInterface.h"
#include "openPASS-System/SystemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class defines a presenter object for exporting the model data
//!        (AgentConfigurationInterface) of this plugin to the profiles catalogue
//!        XML file needed by the simulation.
//!
//!        The only reason why this class exists is because, for a dynamic agent
//!        profile, the profiles catalogue partly repeats information already
//!        contained in a system config file. In order to maintain a consistent
//!        way of defining systems using the system config, this presenter object
//!        gathers the required information from the system config and exports it
//!        into the format needed by the profiles catalogue.
//!
//! @note  In future releases, the way of defining dynamic and static agent will
//!        be unified in order to avoid such a doubling of information in two
//!        different files. Hence, this class is only temporary and we refrain
//!        from giving a detailed documentation here.
//-----------------------------------------------------------------------------
class AgentConfigurationCombinationPresenter : public QObject
{
    Q_OBJECT

public:
    explicit AgentConfigurationCombinationPresenter(AgentConfigurationInterface *agentConfiguration,
                                                    AgentConfigurationView *agentConfigurationView,
                                                    SystemInterface *system,
                                                    ProjectInterface *const project,
                                                    QObject *parent = nullptr);

    ~AgentConfigurationCombinationPresenter() = default;

public Q_SLOTS:
    void convert();

private:
    void addSystem(QString const &systemTitle);

    void addVehicleComponentProfile(VehicleComponentProfileMapInterface::ID id,
                                    SystemComponentParameterMapInterface *parameters);

    void addSensor(SystemComponentItemInterface *sensor,
                   VehicleComponentItemInterface *component,
                   SystemComponentInputItemInterface const *AlgoInput);

private:
    SensorProfileItemInterface *findSensorProfile(SystemComponentItemInterface *sensor);

    SensorItemInterface *findVehicleSensor(SystemComponentItemInterface *sensor,
                                           SensorProfileItemInterface *profile);

    bool sensorParametersIdentical(ParameterMapInterface *profileParameters,
                                   SystemComponentParameterMapInterface *sensorParameters);

    bool sensorPositionsIdentical(SystemComponentItemInterface *sensor,
                                  SensorItemInterface *vehicleSensor);

private:
    AgentConfigurationInterface *const agentConfiguration;
    AgentConfigurationView *const agentConfigurationView;

private:
    VehicleProfileItemInterface *currentVehicleProfile;

private:
    VehicleProfileMapInterface *const vehicleProfiles;
    SensorProfileMapInterface *const sensorProfiles;
    VehicleComponentProfileMapInterface *const componentProfiles;
    SystemInterface *const system;
    ProjectInterface *const project;
};

#endif // VEHICLEPROFILESCOMBINATIONPRESENTER_H
