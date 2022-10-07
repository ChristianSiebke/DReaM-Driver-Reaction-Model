/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#pragma once

#include <variant>

#include "RouteConverter.h"
#include "RouteImporter.h"
#include "Sensors/driverperception.h"
#include "WorldData.h"
#include "common/complexSignals.h"
#include "common/primitiveSignals.h"
#include "common/vector2d.h"
#include "core/opSimulation/framework/commandLineParser.h"
#include "include/modelInterface.h"
#include "include/observationInterface.h"
#include "sensor_perception_logic.h"

class Sensor_Perception_Implementation : SensorInterface {
public:
    const std::string COMPONENTNAME = "Sensor_Perception";

    Sensor_Perception_Implementation(std::string componentName, bool isInit, int priority, int offsetTime, int responseTime, int cycleTime,
                                     StochasticsInterface *stochastics, WorldInterface *world, const ParameterInterface *parameters,
                                     PublisherInterface *const publisher, const CallbackInterface *callbacks, AgentInterface *agent) :
        SensorInterface(componentName, isInit, priority, offsetTime, responseTime, cycleTime, stochastics, world, parameters, publisher,
                        callbacks, agent),
        sensorPerceptionLogic(agent, world),
        converter(world) {
        // TODO: waypoints/Route must be passed via the openPASS framework.
        // So far the openPASS framework (AgentInterface/egoAgent) has no
        // function to get the waypoints/route  -->  redundante import
        auto arguments = QCoreApplication::arguments();
        CommandLineArguments parsedArguments = CommandLineParser::Parse(arguments);
        std::string scenarioConfigPath =
            QCoreApplication::applicationDirPath().toStdString() + "\\" + parsedArguments.configsPath + "\\" + "Scenario.xosc";
        RouteImporter routeImporter(scenarioConfigPath, nullptr);
        auto routeImport = routeImporter.GetDReaMRoute(GetAgent()->GetScenarioName());
        route = converter.Convert(routeImport);
        //-----
    }
    ~Sensor_Perception_Implementation() {
    }

    //-----------------------------------------------------------------------------
    //! Function is called by framework when another component delivers a signal over
    //! a channel to this component (scheduler calls update taks of other component).
    //!
    //! @param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
    //! @param[in]     data           Referenced signal (copied by sending component)
    //! @param[in]     time           Current scheduling time
    //-----------------------------------------------------------------------------
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time);

    //-----------------------------------------------------------------------------
    //! Function is called by framework when this component has to deliver a signal over
    //! a channel to another component (scheduler calls update task of this component).
    //!
    //! @param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
    //! @param[out]    data           Referenced signal (copied by this component)
    //! @param[in]     time           Current scheduling time
    //-----------------------------------------------------------------------------
    virtual void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

    //-----------------------------------------------------------------------------
    //! Function is called by framework when the scheduler calls the trigger task
    //! of this component
    //!
    //! @param[in]     time           Current scheduling time
    //-----------------------------------------------------------------------------
    virtual void Trigger(int time);

private:
    void UpdateGraphPosition();
    void GetNewRoute();
    SensorPerceptionLogic sensorPerceptionLogic;
    GazeState currentGazeState;
    std::vector<InternWaypoint> route;
    RouteConverter converter;
};
