/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef DATABASEREADER_H
#define DATABASEREADER_H

#include <iostream>
#include <memory>

#include <QStringList>
#include <QtSql>

#include "pcm_definitions.h"
#include "pcm_simulationSet.h"

class DatabaseReader
{
public:
    DatabaseReader();

    virtual ~DatabaseReader();

    bool SetDatabase(QString &dbName);

    bool OpenDataBase();
    bool CloseDataBase();
    bool IsDataBaseOpen();

    bool ReadCaseList(QStringList &caseList);

    PCM_SimulationSet *Read(const QString &pcmCase);

    bool ReadParticipantData(const QString &pcmCase,
                             std::vector<PCM_ParticipantData *> &participants);
    bool ReadDynamicsData(const QString &pcmCase,
                          std::vector<PCM_InitialValues *> &initials);
    bool ReadTrajectoryData(const QString &pcmCase,
                            std::vector<PCM_Trajectory *> &trajectories);
    bool ReadMarksData(const QString &pcmCase,
                       PCM_Data *pcmData);
    bool ReadObjectsData(const QString &pcmCase,
                         PCM_Data *pcmData);
    bool ReadViewObjectsData(const QString &pcmCase,
                             PCM_Data *pcmData);
    bool ReadIntendedCourseData(const QString &pcmCase,
                                PCM_Data *pcmData);
    bool ReadGlobalData(const QString &pcmCase,
                        PCM_Data *pcmData);

private:
    bool AddLineData(PCM_LineContainer *lineContainer,
                     int lineNo,
                     int pointNo,
                     double x,
                     double y,
                     double z);

    QString databaseName = "";
    QSqlDatabase db;
    QString connection;
};

#endif // DATABASEREADER_H
