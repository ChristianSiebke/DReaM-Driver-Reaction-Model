/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <QDomDocument>
#include <QFile>
#include <cassert>
#include "Interfaces/trajectoryInterface.h"
#include "importerLoggingHelper.h"

namespace Importer
{

class TrajectoryImporter
{
public:
    static bool Import(const std::string filename, TrajectoryInterface *trajectories);

private:
    static void ImportTrajectory(QDomElement trajectoryElement, TrajectoryInterface *trajectory);
    static void DetermineTrajectoryType(QDomElement trajectoryElement, TrajectoryInterface *trajectory);
};

} //namespace Importer
