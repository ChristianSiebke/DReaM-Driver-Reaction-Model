################################################################################
# Copyright (c) 2017-2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  SpawnerScenario.pro
# \brief This file contains the information for the QtCreator-project of the
# module SpawnerScenario
#-----------------------------------------------------------------------------/

# shortened .pro file name due to MinGW path length problems
TARGET = SpawnerScenario

DEFINES += SPAWNER_SCENARIO_LIBRARY
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
                     ../common/SpawnerDefinitions.h \
                     ../common/stochasticDefinitions.h \
                     ../../../modelElements/agentBlueprint.h \
                     ../../../framework/sampler.h

DEPENDENCY_SOURCES = \
                     ../../../modelElements/agentBlueprint.cpp \
                     ../../../framework/sampler.cpp

SPAWNER_HEADERS = SpawnerScenario.h \
                     SpawnerScenarioExport.h \
                     SpawnerScenarioGlobal.h

SPAWNER_SOURCES = SpawnerScenario.cpp \
                     SpawnerScenarioExport.cpp

SOURCES += \
    $$DEPENDENCY_SOURCES \
    $$SPAWNER_SOURCES

HEADERS += \
    $$DEPENDENCY_HEADERS \
    $$SPAWNER_HEADERS

