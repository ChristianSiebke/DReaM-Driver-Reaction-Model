################################################################################
# Copyright (c) 2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../../../testing.pri)

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

SPAWNPOINTS = $$OPEN_SRC/core/opSimulation/modules/Spawners

DEPENDENCIES_SOURCES = \
                       $$OPEN_SRC/core/common/log.cpp

INCLUDEPATH += \
    $$SPAWNPOINTS \
    $$OPEN_SRC/core \
    $$OPEN_SRC/core/opSimulation \
    $$OPEN_SRC/core/opSimulation/modelElements \
    ../../../../../.. \
    ../../../../../../..

HEADERS += \
    $$OPEN_SRC/core/opSimulation/framework/sampler.h \
    $$SPAWNPOINTS/common/WorldAnalyzer.h

SOURCES += \
    $$DEPENDENCIES_SOURCES \
    $$SPAWNPOINTS/common/WorldAnalyzer.cpp \
    $$OPEN_SRC/core/opSimulation/framework/sampler.cpp \
    spawnerWorldAnalyzer_Tests.cpp
