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
# \file  SpawnPointScenario_OSI.pro
# \brief This file contains the information for the QtCreator-project of the
# module SpawnPointScenario_OSI
#-----------------------------------------------------------------------------/

DEFINES += SPAWNPOINT_SCENARIO_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../../global.pri)

INCLUDEPATH += \
        . \
        .. \
        ../../../CoreFramework/OpenPassSlave/modelElements \
        ../../../CoreFramework/CoreShare

DEPENDENCY_HEADERS = \
                     ../Common/SpawnPointDefinitions.h \
                     ../../../CoreFramework/CoreShare/log.h \
                     ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.h

DEPENDENCY_SOURCES = ../../../CoreFramework/CoreShare/log.cpp \
                     ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.cpp

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

