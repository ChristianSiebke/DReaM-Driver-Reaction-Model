/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "ScenarioModel.h"

#include <QString>
#include <QMap>

ScenarioModel::ScenarioModel(QObject *parent)
    : ScenarioInterface(parent)
    , name(QString(""))
    , scenarioFile(QString(""))
    , entities(new Entities)
{
}

ScenarioModel::~ScenarioModel()
{
    delete entities;
}

ScenarioInterface::Name ScenarioModel::getName() const
{
    return name;
}

void ScenarioModel::setName(const Name &newName)
{
    name = newName;

    Q_EMIT modifiedName();
}

ScenarioInterface::Entities * ScenarioModel::getEntities() const
{
    return entities;
}

ScenarioInterface::ScenarioFile ScenarioModel::getScenarioFile() const
{
    return scenarioFile;
}

void ScenarioModel::setScenarioFile(const ScenarioFile &filename)
{
    scenarioFile=filename;
    Q_EMIT modifiedScenarioFile();
}


void ScenarioModel::setStatus(const Status &_status)
{
    status = _status;
    Q_EMIT modifiedStatus();
}

ScenarioInterface::Status ScenarioModel::getStatus() const
{
    return status;
}
