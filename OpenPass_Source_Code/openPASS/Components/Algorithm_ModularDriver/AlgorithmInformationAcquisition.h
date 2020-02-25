/******************************************************************************
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*****************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AlgorithmInformationAcquisition.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief This file contains the calculations
//!
//! This class contains the acquisition of informations like visual and auditive.
//! Singleton such that data calculations is generated only once and not seperately
//! for each instance.
//!
//-----------------------------------------------------------------------------

#pragma once

//#include <vector>
#include <ContainerStructures.h>
#include "Interfaces/stochasticsInterface.h"
#include "Interfaces/observationInterface.h"

//! \brief provide data and calculate driver's acquisition of informations from the sensors.
//!
//! \ingroup AlgorithmInformationAcquisition

class InformationAcquisition
{
public:

    InformationAcquisition(StochasticsInterface *stochastics, ObservationInterface *observation); // avoid access of constructor of singleton outside class

    void LogSetValues(int time);

    void SetInformationAcquisition_Input(InformationAcquisition_Input_TD *Input_TD, InformationAcquisition_Input_BU *Input_BU);
    void SetEnvironmentInformation(InformationAcquisition_Output *IA_Output);
    void SetEgoData(InformationAcquisition_Output *IA_Output);
    void SetSurroundingMovingObjects(InformationAcquisition_Output *IA_Output);

    // Checks if current viewed vehicle is eighter changing its lane to the viewed targetlane or covering it secondary
    bool CheckItOnViewedLane(SurroundingMovingObjectsData *it, int targetlaneid);

    bool IsReactive();

    void SetMeanAvertViewTime(int MeanReactionTime)
    {
        meanAvertViewTime = MeanReactionTime;
    }
    void SetAvertViewTimeDeviation(int ReactionTimeDeviation)
    {
        AvertViewTimeDeviation = ReactionTimeDeviation;
    }

    InformationAcquisition_Output * GetInformationAcquisition_Output();

private:

    bool Logged = false;

    double thw_Wish;
    double v_Wish;
    double v_y_Max;
    double MinGap;

    int meanAvertViewTime = 0;
    int AvertViewTimeDeviation = 0;
    int minAvertViewTime = 0;

    int reactionTimeTrigger = 0;
    int currentReactionTime = 0;

    InformationAcquisition_Input_BU *IA_Input_BU;
    InformationAcquisition_Input_TD *IA_Input_TD;
    StochasticsInterface *_stochastic;
    ObservationInterface *_observation;

};


