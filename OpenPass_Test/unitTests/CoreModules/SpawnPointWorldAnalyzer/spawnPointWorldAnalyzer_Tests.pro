# /*********************************************************************
# * Copyright (c) 2019 in-tech GmbH
# * 
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../testing.pri)

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

SPAWNPOINTS = $$OPEN_SRC/CoreModules/SpawnPoints

DEPENDENCIES_SOURCES = \
                       $$OPEN_SRC/CoreFramework/CoreShare/log.cpp

INCLUDEPATH += \
    $$SPAWNPOINTS \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/modelElements

HEADERS += \
    $$SPAWNPOINTS/Common/WorldAnalyzer.h

SOURCES += \
    $$DEPENDENCIES_SOURCES \
    $$SPAWNPOINTS/Common/WorldAnalyzer.cpp \
    spawnPointWorldAnalyzer_Tests.cpp
