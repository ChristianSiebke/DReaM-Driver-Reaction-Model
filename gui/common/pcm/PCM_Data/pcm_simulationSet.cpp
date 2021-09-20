/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "pcm_simulationSet.h"

PCM_SimulationSet::PCM_SimulationSet(std::vector<PCM_ParticipantData *> participants,
                                     std::vector<PCM_InitialValues *> initials,
                                     std::vector<PCM_Trajectory *> trajectories,
                                     PCM_Data *pcmData) :
    participants(participants),
    initials(initials),
    trajectories(trajectories),
    pcmData(pcmData)
{
}

PCM_SimulationSet::~PCM_SimulationSet()
{
    for (PCM_ParticipantData *participant : participants)
    {
        if (participant != nullptr)
        {
            delete participant;
        }
    }

    for (PCM_InitialValues *initial : initials)
    {
        if (initial != nullptr)
        {
            delete initial;
        }
    }

    for (PCM_Trajectory *trajectory : trajectories)
    {
        if (trajectory != nullptr)
        {
            delete trajectory;
        }
    }

    if (pcmData != nullptr)
    {
        delete pcmData;
    }
}

const std::vector<PCM_ParticipantData *> &PCM_SimulationSet::GetParticipants() const
{
    return participants;
}

const std::vector<PCM_InitialValues *> &PCM_SimulationSet::GetInitials() const
{
    return initials;
}

const std::vector<PCM_Trajectory *> &PCM_SimulationSet::GetTrajectories() const
{
    return trajectories;
}

const PCM_Data *PCM_SimulationSet::GetPcmData() const
{
    return pcmData;
}
