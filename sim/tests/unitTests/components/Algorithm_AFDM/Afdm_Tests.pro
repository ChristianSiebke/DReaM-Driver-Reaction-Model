################################################################################
# Copyright (c) 2017-2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../testing.pri)

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

UNIT_UNDER_TEST = $$OPEN_SRC/components/AlgorithmAFDM/src

INCLUDEPATH += \
    . \
    ../../../.. \
    $$OPEN_SRC/components/Sensor_Driver/src/Signals \
    $$UNIT_UNDER_TEST

HEADERS += \
    $$OPEN_SRC/components/Sensor_Driver/src/Signals/sensorDriverSignal.h \
    $$UNIT_UNDER_TEST/followingDriverModel.h

SOURCES += \
    $$UNIT_UNDER_TEST/followingDriverModel.cpp \
    Afdm_Tests.cpp

