/********************************************************************************
 * Copyright (c) 2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef CONFIGSERREADERPCM_H
#define CONFIGSERREADERPCM_H

#include <memory>

#include "pcm_simulationSet.h"

//-----------------------------------------------------------------------------
//! Class can read a config set and retrieve pcm simualtion data
//-----------------------------------------------------------------------------
class ConfigSetReaderPcm
{
public:
    ConfigSetReaderPcm(const QString &configFolder);

    // removing operators
    ConfigSetReaderPcm(const ConfigSetReaderPcm &) = delete;
    ConfigSetReaderPcm(PCM_SimulationSet &&) = delete;
    ConfigSetReaderPcm &operator=(const ConfigSetReaderPcm &) = delete;
    ConfigSetReaderPcm &operator=(ConfigSetReaderPcm &&) = delete;

    PCM_SimulationSet *Read();

private:
    const QString &configFolder;
};

#endif // CONFIGSERREADERPCM_H
