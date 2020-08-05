/******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
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

#include <QtGlobal>

#include "Algorithm_ModularDriver_implementation.h"
#include "../Common/lateralSignal.h"
#include "secondaryDriverTasksSignal.h"
#include "complexsignals.cpp"
#include "steeringSignal.h"
#include "vectorSignals.h"
#include "../Common/longitudinalSignal.h"
#include "../Common/accelerationSignal.h"
#include "../Common/parametersVehicleSignal.h"
#include <list>

#include <cassert>
#include <memory>
#include <qglobal.h>


AlgorithmModularDriverImplementation::AlgorithmModularDriverImplementation(
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
        AgentInterface *agent) :
        SensorInterface(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        world,
        parameters,
        observations,
        callbacks,
        agent),
        informationacquisition(stochastics, GetObservations()->at(0)),
        mentalmodel(stochastics),
        situationassesment(stochastics, cycleTime, GetObservations()->at(0)),
        actiondeduction(cycleTime, stochastics, GetObservations()->at(0)),
        actionexecution(stochastics)
{
    try
    {
        observer = GetObservations()->at(0);
        if (observer == nullptr)
        {
            throw std::runtime_error("");
        }
    }
    catch (...)
    {
        const std::string msg = COMPONENTNAME + " invalid observation module setup";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    if (parameters->GetParametersDouble().count("VelocityWish") > 0 || parameters->GetParametersDouble().count("7") > 0)
    {
        double VelocityWish = (parameters->GetParametersDouble().count("VelocityWish") > 0 ? parameters->GetParametersDouble().at("VelocityWish") : parameters->GetParametersDouble().at("7"));
        situationassesment.SetVWish(1/3.6 * VelocityWish);
    }
    if (parameters->GetParametersDouble().count("VelocityWishDeviation") > 0 || parameters->GetParametersDouble().count("8"))
    {
        double VelocityWishDeviation = (parameters->GetParametersDouble().count("VelocityWishDeviation") > 0 ? parameters->GetParametersDouble().at("VelocityWishDeviation") : parameters->GetParametersDouble().at("8"));
        situationassesment.SetVWishDeviation(1/3.6 * VelocityWishDeviation);
    }
    if (parameters->GetParametersDouble().count("TGapWish") > 0 || parameters->GetParametersDouble().count("9") > 0)
    {
        double TGapWish = (parameters->GetParametersDouble().count("TGapWish") > 0 ? parameters->GetParametersDouble().at("TGapWish") : parameters->GetParametersDouble().at("9"));
        situationassesment.SetTGapWish(TGapWish);
        actiondeduction.SetTGapWish(TGapWish);
    }
    if (parameters->GetParametersDouble().count("TGapWishDeviation") > 0 || parameters->GetParametersDouble().count("10") > 0)
    {
        double TGapWishDeviation = (parameters->GetParametersDouble().count("TGapWishDeviation") > 0 ? parameters->GetParametersDouble().at("TGapWishDeviation") : parameters->GetParametersDouble().at("10"));
        situationassesment.SetTGapWishDeviation(TGapWishDeviation);
    }
    if (parameters->GetParametersDouble().count("MaxComfortAcceleration") > 0 || parameters->GetParametersDouble().count("4") > 0)
    {
        double MaxComfortAcceleration = (parameters->GetParametersDouble().count("MaxComfortAcceleration") > 0 ? parameters->GetParametersDouble().at("MaxComfortAcceleration") : parameters->GetParametersDouble().at("4"));
        actiondeduction.SetMaxComfortAccel(MaxComfortAcceleration);
    }
    if (parameters->GetParametersDouble().count("MinComfortDeceleration") > 0 || parameters->GetParametersDouble().count("5") > 0)
    {
        double MinComfortDeceleration = (parameters->GetParametersDouble().count("MinComfortDeceleration") > 0 ? parameters->GetParametersDouble().at("MinComfortDeceleration") : parameters->GetParametersDouble().at("5"));
        actiondeduction.SetMinComfortDecel(MinComfortDeceleration);
    }
    if (parameters->GetParametersDouble().count("ComfortAccelerationDeviation") > 0 || parameters->GetParametersDouble().count("6") > 0)
    {
        double ComfortAccelerationDeviation = (parameters->GetParametersDouble().count("ComfortAccelerationDeviation") > 0 ? parameters->GetParametersDouble().at("ComfortAccelerationDeviation") : parameters->GetParametersDouble().at("6"));
        actiondeduction.SetComfortAccelDev(ComfortAccelerationDeviation);
    }
    if (parameters->GetParametersDouble().count("MeanSpeedLimitViolation") > 0 || parameters->GetParametersDouble().count("11") > 0)
    {
        double MeanSpeedLimitViolation = (parameters->GetParametersDouble().count("MeanSpeedLimitViolation") > 0 ? parameters->GetParametersDouble().at("MeanSpeedLimitViolation") : parameters->GetParametersDouble().at("11"));
        situationassesment.SetSpeedLimit_Violation(1/3.6 * MeanSpeedLimitViolation);
    }
    if (parameters->GetParametersDouble().count("MeanSpeedLimitViolationDeviation") > 0 || parameters->GetParametersDouble().count("12") > 0)
    {
        double MeanSpeedLimitViolationDeviation = (parameters->GetParametersDouble().count("MeanSpeedLimitViolationDeviation") > 0 ? parameters->GetParametersDouble().at("MeanSpeedLimitViolationDeviation") : parameters->GetParametersDouble().at("12"));
        situationassesment.SetSpeedLimit_ViolationDeviation(1/3.6 * MeanSpeedLimitViolationDeviation);
    }
    if (parameters->GetParametersDouble().count("MinDistance") > 0 || parameters->GetParametersDouble().count("13") > 0)
    {
        double MinDistance = (parameters->GetParametersDouble().count("MinDistance") > 0 ? parameters->GetParametersDouble().at("MinDistance") : parameters->GetParametersDouble().at("13"));
        situationassesment.SetMinDistance(MinDistance);
    }
    if (parameters->GetParametersDouble().count("SpeedGain") > 0 || parameters->GetParametersDouble().count("14") > 0)
    {
        double SpeedGain = (parameters->GetParametersDouble().count("SpeedGain") > 0 ? parameters->GetParametersDouble().at("SpeedGain") : parameters->GetParametersDouble().at("14"));
        actiondeduction.SetSpeedGain(SpeedGain);
    }
    if (parameters->GetParametersDouble().count("SpeedGainDeviation") > 0 || parameters->GetParametersDouble().count("15") > 0)
    {
        double SpeedGainDeviation = (parameters->GetParametersDouble().count("SpeedGainDeviation") > 0 ? parameters->GetParametersDouble().at("SpeedGainDeviation") : parameters->GetParametersDouble().at("15"));
        actiondeduction.SetSpeedGainDeviation(SpeedGainDeviation);
    }
    if (parameters->GetParametersDouble().count("KeepRight") > 0 || parameters->GetParametersDouble().count("16") > 0)
    {
        double KeepRight = (parameters->GetParametersDouble().count("KeepRight") > 0 ? parameters->GetParametersDouble().at("KeepRight") : parameters->GetParametersDouble().at("16"));
        actiondeduction.SetKeepRight(KeepRight);
    }
    if (parameters->GetParametersDouble().count("KeepRightDeviation") > 0 || parameters->GetParametersDouble().count("17") > 0)
    {
        double KeepRightDeviation = (parameters->GetParametersDouble().count("KeepRightDeviation") > 0 ? parameters->GetParametersDouble().at("KeepRightDeviation") : parameters->GetParametersDouble().at("17"));
        actiondeduction.SetKeepRightDeviation(KeepRightDeviation);
    }
    if (parameters->GetParametersDouble().count("Cooperative") > 0 || parameters->GetParametersDouble().count("18") > 0)
    {
        double Cooperative = (parameters->GetParametersDouble().count("Cooperative") > 0 ? parameters->GetParametersDouble().at("Cooperative") : parameters->GetParametersDouble().at("18"));
        actiondeduction.SetCooperative(Cooperative);
    }
    if (parameters->GetParametersDouble().count("CooperativeDeviation") > 0 || parameters->GetParametersDouble().count("19") > 0)
    {
        double CooperativeDeviation = (parameters->GetParametersDouble().count("CooperativeDeviation") > 0 ? parameters->GetParametersDouble().at("CooperativeDeviation") : parameters->GetParametersDouble().at("19"));
        actiondeduction.SetCooperativeDeviation(CooperativeDeviation);
    }
    if (parameters->GetParametersInt().count("AvertViewTimeMean") > 0 || parameters->GetParametersInt().count("0") > 0)
    {
        int AvertViewTimeMean = (parameters->GetParametersInt().count("AvertViewTimeMean") > 0 ? parameters->GetParametersInt().at("AvertViewTimeMean") : parameters->GetParametersInt().at("0"));
        informationacquisition.SetMeanAvertViewTime(AvertViewTimeMean);
    }
    if (parameters->GetParametersInt().count("AvertViewTimeDeviation") > 0 || parameters->GetParametersInt().count("1") > 0)
    {
        int AvertViewTimeDeviation = (parameters->GetParametersInt().count("AvertViewTimeDeviation") > 0 ? parameters->GetParametersInt().at("AvertViewTimeDeviation") : parameters->GetParametersInt().at("1"));
        informationacquisition.SetAvertViewTimeDeviation(AvertViewTimeDeviation);
    }
    if (parameters->GetParametersInt().count("ReactionTimeMean") > 0 || parameters->GetParametersInt().count("2") > 0)
    {
        int ReactionTimeMean = (parameters->GetParametersInt().count("ReactionTimeMean") > 0 ? parameters->GetParametersInt().at("ReactionTimeMean") : parameters->GetParametersInt().at("2"));
        situationassesment.SetMeanReactionTime(ReactionTimeMean);
    }
    if (parameters->GetParametersInt().count("ReactionTimeDeviation") > 0 || parameters->GetParametersDouble().count("3") > 0)
    {
        int ReactionTimeDeviation = (parameters->GetParametersInt().count("ReactionTimeDeviation") > 0 ? parameters->GetParametersInt().at("ReactionTimeDeviation") : parameters->GetParametersInt().at("3"));
        situationassesment.SetReactionTimeDeviation(ReactionTimeDeviation);
    }
    if (parameters->GetParametersInt().count("MinReactionTime") > 0)
    {
        int MinReactionTime = parameters->GetParametersInt().at("MinReactionTime");
        situationassesment.SetMinReactionTime(MinReactionTime);
    }
    if (parameters->GetParametersString().count("CriticalityLogging") > 0 || parameters->GetParametersDouble().count("20") > 0)
    {
        std::string CriticalityLogging = (parameters->GetParametersString().count("CriticalityLogging") > 0 ? parameters->GetParametersString().at("CriticalityLogging") : parameters->GetParametersString().at("20"));
        situationassesment.SetLoggingGroups(CriticalityLogging);
    }

    initialisationAD = true;
    initialisationAE = true;
    initialisationIA = true;
    initialisationMM = true;
    initialisationSA = true;
}

AlgorithmModularDriverImplementation::~AlgorithmModularDriverImplementation()
{}

void AlgorithmModularDriverImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time)
{
    if (localLinkId == 0)
    {
        const std::shared_ptr<structSignal<StaticEnvironmentData> const> signal = std::dynamic_pointer_cast<structSignal<StaticEnvironmentData> const>(data);
        if (!signal)
        {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
        IA_I_BU.StaticEnvironment = signal->value;
    }
    else if (localLinkId == 1)
    {
        const std::shared_ptr<structSignal<egoData> const> signal = std::dynamic_pointer_cast<structSignal<egoData> const>(data);
        if (!signal)
        {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
        IA_I_BU.EgoData = signal->value;
    }
    else if (localLinkId == 2)
    {
        const std::shared_ptr<structSignal<std::list<SurroundingMovingObjectsData>> const> signal = std::dynamic_pointer_cast<structSignal<std::list<SurroundingMovingObjectsData>> const>(data);
        if (!signal)
        {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
        IA_I_BU.SurroundingMovingObjects = signal->value;
    }
    else if (localLinkId == 3)
    {
        if (!initializedVehicleModelParameters)
        {
            // from ParametersAgent
            const std::shared_ptr<ParametersVehicleSignal const> signal = std::dynamic_pointer_cast<ParametersVehicleSignal const>(data);
            if (!signal)
            {
                const std::string msg = COMPONENTNAME + " invalid signaltype";
                LOG(CbkLogLevel::Debug, msg);
                throw std::runtime_error(msg);
            }
            vehicleParameters = signal->vehicleParameters;
            situationassesment.SetVehicleParameters(&vehicleParameters);
            actiondeduction.SetVehicleParameters(&vehicleParameters);
            actionexecution.SetVehicleParameters(&vehicleParameters);
            initializedVehicleModelParameters = true;
        }
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
    Q_UNUSED(localLinkId);
    Q_UNUSED(data);
    Q_UNUSED(time);
}

void AlgorithmModularDriverImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    if(localLinkId == 0)
    {
        try
        {
            data = std::make_shared<LongitudinalSignal const>(
                        componentState,
                        out_accPedalPos,
                        out_brakePedalPos,
                        out_gear);
        }
        catch(const std::bad_alloc&)
        {
            const std::string msg = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if(localLinkId == 1)
    {
        try
        {
            data = std::make_shared<SecondaryDriverTasksSignal const>(
                                                      out_indicatorState,
                                                      out_hornSwitch,
                                                      out_headLight,
                                                      out_highBeamLight,
                                                      out_flasher,
                                                      componentState);
        }
        catch(const std::bad_alloc&)
        {
            const std::string msg = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }

    else if (localLinkId == 2)
    {
        try
        {
            data = std::make_shared<SteeringSignal const>(ComponentState::Acting, out_desiredSteeringWheelAngle);
        }
        catch(const std::bad_alloc&)
        {
            const std::string msg = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if(localLinkId == 3)
    {
        try
        {
            data = std::make_shared<AccelerationSignal const>(componentState, out_longitudinal_acc);
        }
        catch(const std::bad_alloc&)
        {
            const std::string msg = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void AlgorithmModularDriverImplementation::Trigger(int time)
{
    InformationAcquisition_Output IA_O = {};
    Information_Acquisition(&IA_I_TD, &IA_I_BU, &IA_O, time);

    MentalModel_Input_BU MM_I_BU;
    MM_I_BU.IA_O = &IA_O;
    MentalModel_Output MM_O;
    Mental_Model(&MM_I_TD, &MM_I_BU, &MM_O);

    SitationAssessment_Input SA_I;
    SA_I.MM_O = &MM_O;
    SituationAssessment_Output_BU SA_O_BU;
    Situation_Assessment(&SA_I, &SA_O_BU, SA_O_TD, &vehicleParameters, time);
    IA_I_TD.SA_O_TD = SA_O_TD;

    ActionDeduction_Input AD_I;
    AD_I.SA_O_BU = &SA_O_BU;
    AD_I.MM_O = &MM_O;
    ActionDeduction_Output_BU AD_O_BU;
    Action_Deduction(&AD_I, &AD_O_BU, AD_O_TD, &vehicleParameters, time);
    IA_I_TD.AD_O_TD = AD_O_TD;

    ActionExecution_Input AE_I;
    AE_I.AD_O_BU = &AD_O_BU;
    AE_I.MM_O = &MM_O;
    ActionExecution_Output_BU AE_O_BU;
    Action_Execution(&AE_I, &AE_O_BU, AE_O_TD, &vehicleParameters, time);
    IA_I_TD.AE_O_TD = AE_O_TD;

    out_desiredSteeringWheelAngle = AE_O_BU.out_desiredSteeringWheelAngle;
    out_accPedalPos = AE_O_BU.out_accPP;
    out_brakePedalPos = AE_O_BU.out_brkPP;
    out_gear = AE_O_BU.out_gear;
    out_indicatorState = AE_O_BU.out_indicatorState;

    IA_I_BU = {};
}

void AlgorithmModularDriverImplementation::Information_Acquisition (InformationAcquisition_Input_TD *IA_Input_TD,
                                                                    InformationAcquisition_Input_BU *IA_Input_BU,
                                                                    InformationAcquisition_Output *IA_Output,
                                                                    int time)
{
    informationacquisition.SetInformationAcquisition_Input(IA_Input_TD, IA_Input_BU);
    informationacquisition.SetEgoData(IA_Output);
    informationacquisition.SetEnvironmentInformation(IA_Output);
    informationacquisition.SetSurroundingMovingObjects(IA_Output);
    informationacquisition.LogSetValues(time);
}

void AlgorithmModularDriverImplementation::Mental_Model (MentalModel_Input_TD *MM_Input_TD,
                                                         MentalModel_Input_BU *MM_Input_BU,
                                                         MentalModel_Output *MM_Output)
{
    mentalmodel.SetMentalModel_Input(MM_Input_BU, MM_Input_TD);
    mentalmodel.GetMentalModel_Output(MM_Output);
}

void AlgorithmModularDriverImplementation::Situation_Assessment (SitationAssessment_Input *SA_Input,
                                                                 SituationAssessment_Output_BU *SA_Output_BU,
                                                                 SituationAssessment_Output_TD *&SA_Output_TD,
                                                                 const VehicleModelParameters *vehicleParameters,
                                                                 int time)
{
    if (initialisationSA)
    {
        initialisationSA = false;
        situationassesment.Initialize();
    }
    situationassesment.SetSituationAssessment_Input(SA_Input);
    situationassesment.SetEnvironmentInformation(SA_Output_BU);
    situationassesment.Pigeonhole_SurroundingMovingObjectsToEgo(SA_Output_BU, time);

    situationassesment.UpdateSituationCalculations();
    situationassesment.CheckForInternalLogging(time);

    if (!situationassesment.InitialValuesLogged())
    situationassesment.LogSetValues(time);

    situationassesment.AssessEgoSituationAndWish(SA_Output_BU);

    SA_Output_TD = situationassesment.GetSituationAssessment_Output_TD();
}

void AlgorithmModularDriverImplementation::Action_Deduction (ActionDeduction_Input *AD_Input,
                                                             ActionDeduction_Output_BU *AD_Output_BU,
                                                             ActionDeduction_Output_TD *AD_Output_TD,
                                                             const VehicleModelParameters *vehicleParameters,
                                                             int time)
{
    if (initialisationAD)
    {
        initialisationAD = false;
        actiondeduction.Initialize();
    }

    actiondeduction.SetActionDeduction_Input(*AD_Input);
    actiondeduction.CalcAccelerationWish(*AD_Output_BU);

    if (!actiondeduction.InitialValuesLogged())
    actiondeduction.LogSetValues(time);

    actiondeduction.CheckLaneChange(&time, AD_Output_BU);
    actiondeduction.prepareLanechange(AD_Output_BU);

    actiondeduction.GetActionDecution_Output_BU(*AD_Output_BU);
    AD_Output_TD = actiondeduction.GetActionDecution_Output_TD();
}

void AlgorithmModularDriverImplementation::Action_Execution (ActionExecution_Input *AE_Input,
                                                             ActionExecution_Output_BU *AE_Output_BU,
                                                             ActionExecution_Output_TD *&AE_Output_TD,
                                                             const VehicleModelParameters *vehicleParameters,
                                                             int time)
{
    actionexecution.SetActionExecution_Input(AE_Input);
    actionexecution.CalculatePedalPosAndGear(AE_Output_BU);
    actionexecution.CalculateSteeringWheelAngle(AE_Output_BU, time);
    actionexecution.SetIndicatorStateDirection(AE_Output_BU);
    AE_Output_TD = actionexecution.GetActionExecution_Output_TD();
}
