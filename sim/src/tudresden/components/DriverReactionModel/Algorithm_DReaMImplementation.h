/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
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
#include "Components/TrafficSignalMemory/TrafficSignalMemory.h"
#include "DriverReactionModel.h"
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
        logger(agent->GetId(), QCoreApplication::applicationDirPath().toStdString() + "\\" +
                                   CommandLineParser::Parse(QCoreApplication::arguments()).resultsPath),
        loggerInterface(logger),
        DReaM(QCoreApplication::applicationDirPath().toStdString() + "\\" +
                  CommandLineParser::Parse(QCoreApplication::arguments()).configsPath + "\\" + "behaviour.xml",
              QCoreApplication::applicationDirPath().toStdString() + "\\" +
                  CommandLineParser::Parse(QCoreApplication::arguments()).resultsPath + "\\",
              loggerInterface, cycleTime, stochastics,(DReaMDefinitions::AgentVehicleType)agent->GetVehicleModelParameters().vehicleType) {
        CommandLineArguments parsedArguments = CommandLineParser::Parse(QCoreApplication::arguments());
        std::string resultPath = QCoreApplication::applicationDirPath().toStdString() + "\\" + parsedArguments.resultsPath + "\\";
        std::string logPath = resultPath + "agent" + std::to_string(agent->GetId()) + ".txt";
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
    void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time);

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

    //! All visual perception information of sensor_DriverPerception
    std::vector<std::shared_ptr<GeneralAgentPerception>> ambientAgents;

    //! All perceived traffic signs
    std::vector<const MentalInfrastructure::TrafficSignal*> trafficSignals;

    //! Ego perception information of sensor_DriverPerception
    std::shared_ptr<DetailedAgentPerception> egoPerception;

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

    double out_longitudinalAccelerationWish = 0;
    GazeState outGazeState;
    std::vector<Common::Vector2d> segmentControlFixPoints{};
    Logger logger;
    LoggerInterface loggerInterface;
    ObservationInterface *observerInstance{nullptr};
    //-END-reaction time--//
};
