/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef OPENSCENARIOWRITER_H
#define OPENSCENARIOWRITER_H

#include <iostream>

#include <QDir>

#include "DataStructuresXosc/XoscScenario.h"
#include "pcm_simulationSet.h"

class OpenScenarioWriter
{
public:
    OpenScenarioWriter() = default;
    virtual ~OpenScenarioWriter() = default;

    const QString CreateScenarioFile(const QString &scenarioFile,
                                     const PCM_SimulationSet *simSet);

private:
    const QString WriteScenarioFile(const QString &scenarioFile,
                                    XoscScenario &scenarioConfig);
};

#endif // OPENSCENARIOWRITER_H
