/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLEPROFILESCOMBINATIONPRESENTER_H
#define VEHICLEPROFILESCOMBINATIONPRESENTER_H

#include "openPASS-System/SystemInterface.h"
#include "openPASS-AgentConfiguration/VehicleProfileMapInterface.h"
#include "openPASS-AgentConfiguration/SensorProfileMapInterface.h"
#include "openPASS-AgentConfiguration/VehicleComponentProfileMapInterface.h"
#include "openPASS-Project/ProjectInterface.h"

#include "Models/AgentConfigurationCombinationXMLSaveModel.h"

#include "Views/AgentConfigurationView.h"

#include <QObject>

class AgentConfigurationCombinationPresenter : public QObject
{
    Q_OBJECT

public:
    explicit AgentConfigurationCombinationPresenter(AgentConfigurationInterface * agentConfiguration,
                                                    AgentConfigurationView *agentConfigurationView,
                                                    SystemInterface * system,
                                                    ProjectInterface * const project,
                                                    QObject *parent = nullptr);


    ~AgentConfigurationCombinationPresenter() = default;

public Q_SLOTS:
    void convert();

private:
    void addSystem(QString const & systemTitle);

    void addVehicleComponentProfile(VehicleComponentProfileMapInterface::ID id,
                                    SystemComponentParameterMapInterface * parameters );

    void addSensor(SystemComponentItemInterface * sensor,
                   VehicleComponentItemInterface * component,
                   SystemComponentInputItemInterface const *  AlgoInput);

private:
    SensorProfileItemInterface * findSensorProfile(SystemComponentItemInterface * sensor);

    SensorItemInterface * findVehicleSensor(SystemComponentItemInterface * sensor,
                                            SensorProfileItemInterface * profile);

    bool sensorParametersIdentical(ParameterMapInterface* profileParameters,
                                   SystemComponentParameterMapInterface * sensorParameters);

    bool sensorPositionsIdentical(SystemComponentItemInterface * sensor,
                                  SensorItemInterface * vehicleSensor );

private:
    AgentConfigurationInterface * const agentConfiguration;
    AgentConfigurationView * const agentConfigurationView;

private:
    VehicleProfileItemInterface * currentVehicleProfile;

private:
    VehicleProfileMapInterface * const vehicleProfiles;
    SensorProfileMapInterface * const sensorProfiles;
    VehicleComponentProfileMapInterface * const componentProfiles;
    SystemInterface * const system;
    ProjectInterface * const project;

};

#endif // VEHICLEPROFILESCOMBINATIONPRESENTER_H
