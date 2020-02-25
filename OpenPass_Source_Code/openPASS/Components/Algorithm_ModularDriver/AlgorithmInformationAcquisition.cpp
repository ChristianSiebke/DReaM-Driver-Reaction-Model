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
//! @file  AlgorithmInformationAcquisition.cpp
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief This file contains the calculations
//!
//! This class contains the acquisition of informations like visual and auditive.
//! Singleton such that data calculations is generated only once and not seperately
//! for each instance.
//!
//-----------------------------------------------------------------------------


#include <iostream>
#include "AlgorithmInformationAcquisition.h"

InformationAcquisition::InformationAcquisition(StochasticsInterface *stochastics, ObservationInterface *observation):
    _stochastic(stochastics),
    _observation(observation)
{
    v_y_Max = _stochastic->GetNormalDistributed(1,0.2); //TODO
}

void InformationAcquisition::SetInformationAcquisition_Input(InformationAcquisition_Input_TD *Input_TD, InformationAcquisition_Input_BU *Input_BU)
{ 
    IA_Input_BU = Input_BU;
    IA_Input_TD = Input_TD;
}

void InformationAcquisition::SetEgoData(InformationAcquisition_Output *IA_Output)
{
    IA_Output->Ego = &IA_Input_BU->EgoData;
}

void InformationAcquisition::SetEnvironmentInformation(InformationAcquisition_Output *IA_Output)
{
   IA_Output->EnvironmentInfo= &IA_Input_BU->StaticEnvironment;
}

void InformationAcquisition::SetSurroundingMovingObjects(InformationAcquisition_Output *IA_Output)
{
    if (IsReactive())
    {
        IA_Output->SurroundingMovingObjects = &IA_Input_BU->SurroundingMovingObjects;
    }
    else
    {
        IA_Input_BU->SurroundingMovingObjects.clear();
        IA_Output->SurroundingMovingObjects = &IA_Input_BU->SurroundingMovingObjects;
    }
}

bool InformationAcquisition::IsReactive()
{
    if (reactionTimeTrigger>=currentReactionTime)
    {
        currentReactionTime = std::max(minAvertViewTime,(int) std::round(_stochastic->GetLogNormalDistributed(meanAvertViewTime,AvertViewTimeDeviation)/10)*10);
        reactionTimeTrigger = 0;
        return true;
    }
    else
    {
        reactionTimeTrigger = reactionTimeTrigger + 100;
        return false;
    }
}

void InformationAcquisition::LogSetValues(int time)
{
    _observation->Insert(time,
                        IA_Input_BU->EgoData.GetState()->id,
                        LoggingGroup::Driver,
                        "CurrentAvertViewTime",
                        std::to_string(currentReactionTime));

}

