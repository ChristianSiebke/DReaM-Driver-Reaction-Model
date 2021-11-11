/********************************************************************************
 * Copyright (c) 2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef PCM_SIMULATIONSET_H
#define PCM_SIMULATIONSET_H

#include "pcm_data.h"
#include "pcm_initialValues.h"
#include "pcm_participantData.h"
#include "pcm_trajectory.h"

//-----------------------------------------------------------------------------
//! Class to store the information on the pcm simulation
//-----------------------------------------------------------------------------
class
    PCM_SimulationSet
{
public:
    PCM_SimulationSet() = default;
    //-----------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param[in]     participants     vector of participants
    //! @param[in]     initials         vector of initial values
    //! @param[in]     trajectories     vector of trajectories
    //! @param[in]     pcmData          pcm data
    //!
    //-----------------------------------------------------------------------------
    PCM_SimulationSet(std::vector<PCM_ParticipantData *> participants,
                      std::vector<PCM_InitialValues *> initials,
                      std::vector<PCM_Trajectory *> trajectories,
                      PCM_Data *pcmData);

    // removing operators
    PCM_SimulationSet(const PCM_SimulationSet &) = delete;
    PCM_SimulationSet(PCM_SimulationSet &&) = delete;
    PCM_SimulationSet &operator=(const PCM_SimulationSet &) = delete;
    PCM_SimulationSet &operator=(PCM_SimulationSet &&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor
    //-----------------------------------------------------------------------------
    ~PCM_SimulationSet();

    const std::vector<PCM_ParticipantData *> &GetParticipants() const;
    const std::vector<PCM_InitialValues *> &GetInitials() const;
    const std::vector<PCM_Trajectory *> &GetTrajectories() const;
    const PCM_Data *GetPcmData() const;

private:
    const std::vector<PCM_ParticipantData *> participants; //!< vector of participants
    const std::vector<PCM_InitialValues *> initials;       //!< vector of initial values
    const std::vector<PCM_Trajectory *> trajectories;      //!< vector of trajectories
    const PCM_Data *pcmData;                               //!< pcm data
};

#endif // PCM_SIMULATIONSET_H
