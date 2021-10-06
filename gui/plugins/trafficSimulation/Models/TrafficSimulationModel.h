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
//! @file TrafficSimulationModel.h
//! @ingroup trafficSimulationPlugin
//! @brief The TrafficSimulationModel class implements functionality of the
//!        TrafficSimulationInterface class.
//-----------------------------------------------------------------------------
#ifndef TRAFFICSIMULATIONMODEL_H
#define TRAFFICSIMULATIONMODEL_H

#include <QObject>

#include "Models/EnvironmentModel.h"
#include "Models/ExperimentModel.h"
#include "Models/ScenarioModel.h"
#include "Models/TrafficModel.h"
#include "openPASS-TrafficSimulation/TrafficSimulationInterface.h"

//-----------------------------------------------------------------------------
//! @brief The TrafficSimulationModel class implements functionality of the
//!        TrafficSimulationInterface class.
//-----------------------------------------------------------------------------
class TrafficSimulationModel : public TrafficSimulationInterface
{
    Q_OBJECT
public:
    explicit TrafficSimulationModel(QObject *const parent = nullptr);
    virtual ~TrafficSimulationModel() = default;

public:
    virtual ExperimentInterface *getExperiment() const override;
    virtual EnvironmentInterface *getEnvironment() const override;
    virtual ScenarioInterface *getScenario() const override;
    virtual TrafficInterface *getTraffic() const override;

public:
    virtual void setProfilesCatalogue(ProfilesCatalogue const &_profilesCatalogue) override;
    virtual ProfilesCatalogue getProfilesCatalogue() const override;

private:
    ExperimentModel *experiment;
    EnvironmentModel *environment;
    ScenarioModel *scenario;
    TrafficModel *traffic;
    ProfilesCatalogue profilesCatalogue;
};

#endif // TRAFFICSIMULATIONMODEL_H
