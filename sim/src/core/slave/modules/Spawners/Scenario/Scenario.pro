# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  SpawnPointScenario.pro
# \brief This file contains the information for the QtCreator-project of the
# module SpawnPointScenario
#-----------------------------------------------------------------------------/

# shortened .pro file name due to MinGW path length problems
TARGET = SpawnPointScenario

DEFINES += SPAWNPOINT_SCENARIO_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../../../../global.pri)

INCLUDEPATH += \
    .. \
    ../../../ \
    ../../../.. \
    ../../../../.. \
    ../../../../../.. \
    ../../../modelElements

DEPENDENCY_HEADERS = \
                     ../common/SpawnPointDefinitions.h \
                     ../common/stochasticDefinitions.h \
                     ../../../modelElements/agentBlueprint.h \
                     ../../../framework/sampler.h

DEPENDENCY_SOURCES = \
                     ../../../modelElements/agentBlueprint.cpp \
                     ../../../framework/sampler.cpp

SPAWNPOINT_HEADERS = SpawnPointScenario.h \
                     SpawnPointScenarioExport.h \
                     SpawnPointScenarioGlobal.h

SPAWNPOINT_SOURCES = SpawnPointScenario.cpp \
                     SpawnPointScenarioExport.cpp

SOURCES += \
    $$DEPENDENCY_SOURCES \
    $$SPAWNPOINT_SOURCES

HEADERS += \
    $$DEPENDENCY_HEADERS \
    $$SPAWNPOINT_HEADERS

