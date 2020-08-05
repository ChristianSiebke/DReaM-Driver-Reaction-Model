/******************************************************************************
* Copyright (c) 2019 TU Dresden
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*****************************************************************************/

//-----------------------------------------------------------------------------
//! @file  Algorithm_ModularDriver_implementation.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief Represents the cognitive processes of the driver from information acquisition to action excecution.
//!
//! This class contains the data calculations.
//! Singleton such that data calculations is generated only once and not seperately
//! for each instance.
//!
//-----------------------------------------------------------------------------


/*! \addtogroup Algorithm_ModularDriver
 * @{
 *
 * \details
 *
 * \section Algorithm_ModularDriver_Inputs Inputs
 * Input variables:
 * name | meaning
 * -----|---------
 * IA_I_BU.StaticEnvironment   | Container with the static elements in the environment inside the InformationAcquisition Input Bottom Up
 * IA_I_BU.EgoData   | Container with the ego information inside the InformationAcquisition Input Bottom Up
 * IA_I_BU.SurroundingMovingObjectsData   | Container with the moving elements in the environment inside the InformationAcquisition Input Bottom Up
 *
 * Input channel IDs:
 * Input ID | signal class | contained variables
 * ------------|--------------|-------------
 * 0 | structSignal<StaticEnvironmentData> | IA_I_BU.StaticEnvironment
 * 1 | structSignal<egoData> | IA_I_BU.EgoData
 * 2 | structSignal<SurroundingMovingObjectsData> | IA_I_BU.SurroundingMovingObjectsData
 *
 * @} */

#pragma once

#include <string>
#include <iostream>
//#include <map>
#include "modelInterface.h"
#include "primitiveSignals.h"
#include <AlgorithmInformationAcquisition.h>
#include <AlgorithmMentalModel.h>
#include <AlgorithmSituationAssessment.h>
#include <AlgorithmActionDeduction.h>
#include <AlgorithmActionExecution.h>

//! \ingroup Algorithm_ModularDriver
class AlgorithmModularDriverImplementation : public SensorInterface
{
public:
    //! Name of the current component
    const std::string COMPONENTNAME = "AlgorithmModularDriver";

    //! \brief Constructor.
    //!
    //! \param [in] componentId   Component ID
    //! \param [in] isInit        Component's init state
    //! \param [in] priority      Task priority level
    //! \param [in] offsetTime    Start time offset
    //! \param [in] responseTime  Update response time
    //! \param [in] cycleTime     Cycle time
    //! \param [in] stochastics   Stochastics instance
    //! \param [in] world         World interface
    //! \param [in] parameters    Paramaters
    //! \param [in] observations  Observation instance
    //! \param [in] callbacks     Callbacks
    //! \param [in] agent         Agent
    AlgorithmModularDriverImplementation(
            std::string componentName,
            bool isInit,
            int priority,
            int offsetTime,
            int responseTime,
            int cycleTime,
            StochasticsInterface *stochastics,
            WorldInterface *world,
            const ParameterInterface *parameters,
            const std::map<int, ObservationInterface*> *observations,
            const CallbackInterface *callbacks,
            AgentInterface *agent);

    AlgorithmModularDriverImplementation(const AlgorithmModularDriverImplementation&) = delete;
    AlgorithmModularDriverImplementation(AlgorithmModularDriverImplementation&&) = delete;
    AlgorithmModularDriverImplementation& operator=(const AlgorithmModularDriverImplementation&) = delete;
    AlgorithmModularDriverImplementation& operator=(AlgorithmModularDriverImplementation&&) = delete;
    virtual ~AlgorithmModularDriverImplementation();

    /*!
    * \brief Update Inputs
    *
    * Function is called by framework when another component delivers a signal over
    * a channel to this component (scheduler calls update taks of other component).
    *
    * Refer to module description for input channels and input ids.
    *
    * @param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
    * @param[in]     data           Referenced signal (copied by sending component)
    * @param[in]     time           Current scheduling time
    */
    void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time);

    /*!
    * \brief Update outputs.
    *
    * Function is called by framework when this Component.has to deliver a signal over
    * a channel to another component (scheduler calls update task of this component).
    *
    * Refer to module description for output channels and output ids.
    *
    * @param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
    * @param[out]    data           Referenced signal (copied by this component)
    * @param[in]     time           Current scheduling time
    */
    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

    /*!
    * \brief Process data within component.
    *
    * Function is called by framework when the scheduler calls the trigger task
    * of this component
    *
    * @param[in]     time           Current scheduling time
    */
    void Trigger(int time);

    /*!
    * \brief Process all sensor information to handable structure (ego, surrounding, environment) within component.
    *
    * Function is called by the trigger to handle all sensor data (visual, auditory etc.)
    *
    * @param[in]     VisualEnvironmentContainer           Container with all Environment Informations detected via the visual system
    * @param[in]     VisualObjectsContainer               Container with all visually detected objects
    */
    void Information_Acquisition (InformationAcquisition_Input_TD *IA_I_TD, InformationAcquisition_Input_BU *IA_I_BU, InformationAcquisition_Output *IA_O, int time);

    void Mental_Model (MentalModel_Input_TD *MM_I_TD, MentalModel_Input_BU *MM_I_BU, MentalModel_Output *MM_O);

    void Situation_Assessment (SitationAssessment_Input *SA_I, SituationAssessment_Output_BU *SA_O_BU, SituationAssessment_Output_TD *&SA_O_TD, const VehicleModelParameters *vehicleParameters, int time);

    void Action_Deduction (ActionDeduction_Input *AD_I, ActionDeduction_Output_BU *AD_O_BU, ActionDeduction_Output_TD *AD_O_TD, const VehicleModelParameters *vehicleParameters, int time);

    void Action_Execution (ActionExecution_Input *AE_I, ActionExecution_Output_BU *AE_O_BU, ActionExecution_Output_TD *&AE_O_TD, const VehicleModelParameters *vehicleParameters, int time);

    //----------------------------------------------------------------

private:

    bool initializedVehicleModelParameters = false;

    //! The state of realignment to queue [-].
    bool out_realignToQueue = false;   

    StaticEnvironmentData StaticEnvironment;
    std::vector<SurroundingMovingObjectsData> SurroundingMovingObjects;
    egoData Ego;

    InformationAcquisition informationacquisition;
    MentalModel mentalmodel;
    SituationAssessment situationassesment;
    ActionDeduction actiondeduction;
    ActionExecution actionexecution;

    InformationAcquisition_Input_BU IA_I_BU;
    InformationAcquisition_Input_TD IA_I_TD;
    MentalModel_Input_TD MM_I_TD;
    SituationAssessment_Output_TD *SA_O_TD;
    ActionDeduction_Output_TD *AD_O_TD;
    ActionExecution_Output_TD *AE_O_TD;

    //! The longitudinal acceleration of the vehicle [m/s^2].
    double out_longitudinal_acc =0;

    //! The state of the turning indicator [-].
    int out_indicatorState = static_cast<int>(IndicatorState::IndicatorState_Off);
    //! Activation of HornSwitch [-].
    bool out_hornSwitch = false;
    //! Activation of Headlight [-].
    bool out_headLight = false;
    //! Activation of Highbeam Light [-].
    bool out_highBeamLight = false;
    //! Activation of Flasher [-].
    bool out_flasher = false;

    double out_desiredSteeringWheelAngle;

    //  --- Outputs
    //! Position of the accecaleration pedal position in percent.
    double out_accPedalPos = 0;
    //! Position of the brake pedal position in percent.
    double out_brakePedalPos = 0;
    //! Number of gears and the currently choosen gear.
    int out_gear = {0};

    std::vector<double> out_DoubleSignalVector = {0,0,0};

    //! component state for finely granulated evaluation of signal
    ComponentState componentState = ComponentState::Acting;


    //! Flag that indicates the need to initialize the InformationAcquisition - e.g parsing input informations
    bool initialisationIA = false;
    //! Flag that indicates the need to initialize the MentalModel - e.g parsing input informations
    bool initialisationMM = false;
    //! Flag that indicates the need to initialize the SituationAssessment - e.g parsing input informations
    bool initialisationSA = false;
    //! Flag that indicates the need to initialize the ActionDeduction - e.g parsing input informations
    bool initialisationAD = false;
    //! Flag that indicates the need to initialize the ActionExcecution - e.g parsing input informations
    bool initialisationAE = false;

    VehicleModelParameters vehicleParameters;

    StochasticsInterface *stochastic;

    ObservationInterface* observer = nullptr; ///!< Observer containing the eventnetwork into which (de-)activation events are inserted

};
