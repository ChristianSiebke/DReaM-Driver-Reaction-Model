/********************************************************************************
 * Copyright (c) 2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  vehicleModelImporter.h
//! @brief This file contains the importer of the vehicle Model Catalog.
//-----------------------------------------------------------------------------

#ifndef VEHICLEMODELIMPORT_H
#define VEHICLEMODELIMPORT_H

#include <QDomDocument>
#include <QFile>

#include "pcm_participantData.h"

/*!
* \brief The ScenarioImporterPCM class
* This class is responsible to import pcm data from a given scenario configuration file.
*/

class VehicleModelImporter
{
public:
    VehicleModelImporter() = default;
    VehicleModelImporter(const VehicleModelImporter &) = delete;
    VehicleModelImporter(VehicleModelImporter &&) = delete;
    VehicleModelImporter &operator=(const VehicleModelImporter &) = delete;
    VehicleModelImporter &operator=(VehicleModelImporter &&) = delete;
    virtual ~VehicleModelImporter() = default;

    //-----------------------------------------------------------------------------
    //! Imports data structures from the vehicle model catalog file
    //!
    //! @param[in]  filename       path to vehicle model catalog file
    //! @param[in]  particpants    particpants object to fill with data from the file
    //! @return                    true on success
    //-----------------------------------------------------------------------------
    bool Import(const std::string &filename,
                std::map<int, PCM_ParticipantData *> &particpants);

private:
    //! Parse all participants.
    //!
    //! \param[in] vehicleNode    node with particpant data
    //! \param[in,out] particpants  particpants map to be filled
    //! \return true for success
    bool ParseParticipants(int id, QDomNode vehicleNode,
                           std::map<int, PCM_ParticipantData *> &particpants);

    //! Find the node with a given tag name
    //!
    //! \param[in] baseNode     node to start from
    //! \param[in] tagName      name of the tag to look from
    //! \return first node which was find, isNull if not found
    QDomNode FindNodeByTag(QDomNode baseNode, std::string tagName);
};

#endif // VEHICLEMODELIMPORT_H
