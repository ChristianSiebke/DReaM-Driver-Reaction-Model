# /*********************************************************************
# * Copyright (c) 2017 2018 2019 in-tech GmbH
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

UNIT_UNDER_TEST = $$OPEN_SRC/CoreModules/SpawnPoint_OSI

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

INCLUDEPATH += \
    $$UNIT_UNDER_TEST

HEADERS += \
    $$UNIT_UNDER_TEST/SpawnPoint.h \
    $$UNIT_UNDER_TEST/SpawnPointSceneryParser.h

SOURCES += \
    $$OPEN_SRC/CoreFramework/CoreShare/log.cpp \
    $$OPEN_SRC/CoreFramework/CoreShare/parameters.cpp \
    $$UNIT_UNDER_TEST/SpawnPoint.cpp \
    $$UNIT_UNDER_TEST/SpawnPointSceneryParser.cpp \
    sceneryParser_Tests.cpp \
    spawnPoint_Tests.cpp

