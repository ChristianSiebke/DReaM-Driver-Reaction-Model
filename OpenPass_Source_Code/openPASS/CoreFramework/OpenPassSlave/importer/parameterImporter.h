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

#include "Interfaces/parameterInterface.h"
#include "xmlParser.h"
#include "importerLoggingHelper.h"

namespace SimulationCommon {

class ParameterImporter
{
public:
    static void ImportBoolParameters(
        QDomElement componentParameterSetElement,
        ParameterInterface& componentParameterSet);

    static void ImportIntParameters(
        QDomElement componentParameterSetElement,
        ParameterInterface& componentParameterSet);

    static void ImportIntVectorParameters(
        QDomElement componentParameterSetElement,
        ParameterInterface& componentParameterSet);

    static void ImportDoubleParameters(
        QDomElement componentParameterSetElement,
        ParameterInterface& componentParameterSet);

    static void ImportDoubleVectorParameters(
        QDomElement componentParameterSetElement,
        ParameterInterface& componentParameterSet);

    static void ImportStringParameters(
        QDomElement componentParameterSetElement,
        ParameterInterface& componentParameterSet);

    static void ImportNormalDistributionParameters(
        QDomElement componentParameterSetElement,
        ParameterInterface& componentParameterSet);

    static void ImportParameterLists(
        QDomElement componentParameterSetElement,
        ParameterInterface& componentParameterSet);

    static void ImportParameters(
        QDomElement componentParameterSetElement,
        ParameterInterface& componentParameterSet);
};

} //namespace SimulationCommon
