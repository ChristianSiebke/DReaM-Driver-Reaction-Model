/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "ExperimentModel.h"

ExperimentInterface::ID const ExperimentModel::id_default = 0;
ExperimentInterface::NumberInvocations const ExperimentModel::numberInvocations_default = 1;
ExperimentInterface::Seed const ExperimentModel::seed_default = 0;

ExperimentModel::ExperimentModel(QObject *parent)
    : ExperimentInterface(parent)
    , id(id_default)
    , numberInvocations(numberInvocations_default)
    , seed(seed_default)
    , libraries(new Libraries)
    , loggingGroups(new LoggingGroups)
{
}

ExperimentInterface::ID ExperimentModel::getID() const
{
    return id;
}

void ExperimentModel::setNumberInvocations(NumberInvocations const & _numberInvocations)
{
    numberInvocations = _numberInvocations;
}

ExperimentInterface::NumberInvocations ExperimentModel::getNumberInvocations() const
{
    return numberInvocations;
}

void ExperimentModel::setSeed(Seed const & _seed)
{
    seed = _seed;
}

ExperimentInterface::Seed ExperimentModel::getSeed() const
{
    return seed;
}

ExperimentInterface::Libraries * ExperimentModel::getLibraries() const
{
    return libraries;
}

ExperimentInterface::LoggingGroups * ExperimentModel::getLoggingGroups() const
{
    return loggingGroups;
}
