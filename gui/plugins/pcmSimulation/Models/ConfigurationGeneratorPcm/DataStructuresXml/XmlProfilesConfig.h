/*********************************************************************
* Copyright (c) 2017, 2018, 2020, 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#pragma once

#include "XmlAgent.h"
#include "XmlObservation.h"
#include "XmlSpawnPoint.h"

class XmlProfilesConfig: public XmlBaseClass
{
public:
    XmlProfilesConfig( int startTime, QString endTime, int nInvocations, QString weekday, int timeOfDay,
                  int randomSeed );

    ~XmlProfilesConfig();

    bool WriteToXml( QXmlStreamWriter *xmlWriter );

    void AddSpawnPoint( XmlSpawnPoint *sp );
    void AddModelId(int id);
    void AddObservation( XmlObservation *observation );

private:
    int startTime = 0;             //!< simulation start time
    QString endTime;               //!< simulation end time
    int numberInvocations =
        0;     //!< number of repetitions of the same situation described with this file
    QString weekday;               //!< weekday of the situation
    int timeOfDay = -1;            //!< hour of the day, the situation happens
    int randomSeed =
        -1;           //!< random seed of the situation for initialisation of non-deterministic modules
    QString worldLibrary = "World_PCM";//!< World.dll
    QString stochastics = "Stochastics"; //!< Stochastics.dll
    QString collisionDetection = "CollisionDetection_Impact"; //!< CollisionDetection.dll

    std::vector<XmlSpawnPoint *> spawnPoints;  //!< vector of all spawn points of the situation
    std::vector<int> modelIds;                 //!< vector of all model ids
    std::vector<XmlObservation *> observations;  //!< vector of all observations of the situation
};

