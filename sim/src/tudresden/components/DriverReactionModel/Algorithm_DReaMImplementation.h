/******************************************************************************
 * Copyright (c) 2020 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/

/** \addtogroup DReaM
 * @{
 * \brief Driver Reaction Model (models the behavior of the driver)
 *
 * This component models the processing and decision making process of the
 * driver.
 *
 * \section Algorithm_DReaMImplementation Inputs
 * Input variables:
 * name | meaning
 * -----|------
 *                    |
 *
 * Input channel IDs:
 * Input ID | signal class | contained variables
 * ------------|--------------|-------------
 *
 *
 *
 * \section Algorithm_DReaMImplementation Outputs
 * Output variables:
 * name | meaning
 * -----|------
 *
 * Output channel IDs:
 * Output Id | signal class | contained variables
 * ------------|--------------|-------------
 *  |  |
 *
 * \section Algorithm_DReaMImplementation_ExternalParameters External
 * parameters none
 *
 * \section Algorithm_DReaMImplementation_InternalParameters Internal
 * paramters name | value | meaning
 * -----|-------|------
 * none
 *
 * \section Algorithm_DReaMImplementation_ConfigParameters Parameters to be
 * specified in agentConfiguration.xml none
 *
 *   @} */

#pragma once
#include <QCommandLineParser>
#include <qcoreapplication.h>

#include "Common/ComplexSignals.h"
#include "Common/primitiveSignals.h"
#include "Components/ActionDecision/ActionDecision.h"
#include "Components/CognitiveMap/CognitiveMap.h"
#include "Components/GazeMovement/GazeMovement.h"
#include "Components/Importer/BehaviourImporter.h"
#include "Components/Navigation.h"
#include "Components/TrafficSignMemory/TrafficSignMemory.h"
#include "DriverReactionModel.h"
#include "agentstaterecorder.h"
#include "core/opSimulation/framework/commandLineParser.h"
#include "core/opSimulation/framework/sampler.h"
#include "include/modelInterface.h"
#include "include/observationInterface.h"
#include "include/publisherInterface.h"
/*!
 * \brief models the behavior of the driver
 *
 * This class models the processing and decision making process of the driver.
 *
 * \ingroup Algorithm_DReaMImplementation
 */
class AlgorithmDReaMImplementation : public AlgorithmInterface {
  public:
    const std::string COMPONENTNAME = "Driver Reaction Model (DReaM)";

    AlgorithmDReaMImplementation(std::string componentName, bool isInit, int priority, int offsetTime, int responseTime, int cycleTime,
                                 StochasticsInterface *stochastics, const ParameterInterface *parameters, PublisherInterface *publisher,
                                 const CallbackInterface *callbacks, AgentInterface *agent) :
        AlgorithmInterface(componentName, isInit, priority, offsetTime, responseTime, cycleTime, stochastics, parameters, publisher,
                           callbacks, agent),
        logger(agent->GetId()),
        loggerInterface(logger) {
        auto arguments = QCoreApplication::arguments();
        CommandLineArguments parsedArguments = CommandLineParser::Parse(arguments);
        std::string resultPath = QCoreApplication::applicationDirPath().toStdString() + "\\" + parsedArguments.resultsPath + "\\";
        std::string logPath = resultPath + "agent" + std::to_string(agent->GetId()) + ".txt";
        std::string ConfigPath =
            QCoreApplication::applicationDirPath().toStdString() + "\\" + parsedArguments.configsPath + "\\" + "behaviour.xml";
        logger.SetPath(logPath);

        // TODO: wrap all in DReaM constructor ----
        BehaviourImporter importer(ConfigPath, &loggerInterface);
        behaviourData = importer.GetBehaviourData();
        std::unique_ptr<Component::ComponentInterface> cognitiveMap =
            std::make_unique<CognitiveMap::CognitiveMap>(cycleTime, stochastics, &loggerInterface, *behaviourData);
        std::unique_ptr<Component::ComponentInterface> navigation =
            std::make_unique<Navigation::Navigation>(cognitiveMap->GetWorldRepresentation(), cognitiveMap->GetWorldInterpretation(),
                                                     routeElement, cycleTime, stochastics, &loggerInterface, *behaviourData);
        std::unique_ptr<Component::ComponentInterface> gazeMovement =
            std::make_unique<GazeMovement::GazeMovement>(cognitiveMap->GetWorldRepresentation(), cognitiveMap->GetWorldInterpretation(),
                                                         cycleTime, stochastics, &loggerInterface, *behaviourData);
        std::unique_ptr<Component::ComponentInterface> actionDecision =
            std::make_unique<ActionDecision::ActionDecision>(cognitiveMap->GetWorldRepresentation(), cognitiveMap->GetWorldInterpretation(),
                                                             cycleTime, stochastics, &loggerInterface, *behaviourData);
        DReaM.SetComponent(100, std::move(cognitiveMap));
        DReaM.SetComponent(90, std::move(navigation));
        DReaM.SetComponent(80, std::move(gazeMovement));
        DReaM.SetComponent(70, std::move(actionDecision));
        agentStateRecorder = &agentStateRecorder::getInstance(resultPath);
        //------------------------------------------------------
    }

    /*!
     * \brief Update Inputs
     *
     * Function is called by framework when another component delivers a signal
     * over a channel to this component (scheduler calls update taks of other
     * component).
     *
     * Refer to module description for input channels and input ids.
     *
     * @param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
     * @param[in]     data           Referenced signal (copied by sending
     * component)
     * @param[in]     time           Current scheduling time
     */
    void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const>& data, int time);

    /*!
     * \brief Update outputs.
     *
     * Function is called by framework when this component has to deliver a signal
     * over a channel to another component (scheduler calls update task of this
     * component).
     *
     * Refer to module description for output channels and output ids.
     *
     * @param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
     * @param[out]    data           Referenced signal (copied by this component)
     * @param[in]     time           Current scheduling time
     */
    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time);

    /*!
     * \brief Process data within component.
     *
     * Function is called by framework when the scheduler calls the trigger task
     * of this component
     *
     * @param[in]     time           Current scheduling time
     */
    void Trigger(int time);

    //! The route the agent has planned, consisting of one or multiple waypoints
    RouteElement routeElement;

    //! All visual perception information of sensor_DriverPerception
    std::vector<std::shared_ptr<AgentPerception>> ambientAgents;

    //! All perceived traffic signs
    std::vector<const MentalInfrastructure::TrafficSign*> trafficSigns;

    //! Ego perception information of sensor_DriverPerception
    std::shared_ptr<EgoPerception> egoPerception;

    //! All infrastructure perception information of sensor_DriverPerception
    std::shared_ptr<InfrastructurePerception> infrastructurePerception;

    // Driver Behaviour Model

    DriverReactionModel DReaM;

    //**************************************************
    // Output-----------------------------------------
    //**************************************************

    //! component state for finely granulated evaluation of signal
    ComponentState componentState = ComponentState::Acting;

    // LateralSignal
    //*************************************************
    //! The relative lateral position of the vehicle [m].
    double out_lateral_displacement = 0;
    //! The gain for lateral displacement error controller [-].
    double out_lateral_gain_displacement = 20.0;
    //! The heading angle error of the vehicle [rad].
    double out_heading_error = 0;
    //! The gain for heading error controller [-].
    double out_lateral_gain_heading_error = 7.5;
    //! The curvature of the lane at vehicle's position [1/m].
    double out_curvature = 0;
    //! The width of the lane containing the vehicle [m].
    double out_laneWidth = 0;
    //**************************************************

    // SecondaryDriverTasksSignal
    //*************************************************

    //! State of IndicatorSwitch [-].
    int out_indicatorState = 0;
    //! Activation of HornSwitch [-].
    bool out_hornSwitch = false;
    //! Activation of Headlight [-].
    bool out_headLight = false;
    //! Activation of Highbeam Light [-].
    bool out_highBeamLight = false;
    //! Activation of Flasher [-].
    bool out_flasher = false;
    //*************************************************

    double out_longitudinalaccelerationWish = 0;
    GazeState outGazeState;
    std::vector<Common::Vector2d> segmentControlFixPoints;

    Logger logger;
    LoggerInterface loggerInterface;
    std::unique_ptr<BehaviourData> behaviourData;

    agentStateRecorder* agentStateRecorder;
    ObservationInterface* observerInstance{nullptr};

    //-END-reaction time--//
};
