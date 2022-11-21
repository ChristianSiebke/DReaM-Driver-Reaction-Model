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

include(../../../testing.pri)

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

UNIT_UNDER_TEST = $$OPEN_SRC/components/Sensor_OSI/src
WORLD_OSI = $$OPEN_SRC/core/opSimulation/modules/World_OSI

INCLUDEPATH += \
    $$UNIT_UNDER_TEST \
    $$WORLD_OSI \
    $$WORLD_OSI/OWL/fakes \
    ../../../..

HEADERS += \
    $$OPEN_SRC/Common/vector2d.h \
    $$UNIT_UNDER_TEST/sensorGeometric2D.h \
    $$UNIT_UNDER_TEST/objectDetectorBase.h \
    $$WORLD_OSI/OWL/DataTypes.h \
    $$WORLD_OSI/OWL/OpenDriveTypeMapper.h \
    $$WORLD_OSI/WorldObjectAdapter.h \
    $$WORLD_OSI/WorldData.h \
    $$WORLD_OSI/WorldDataException.h \
    sensorOSI_TestsCommon.h

SOURCES += \
    $$UNIT_UNDER_TEST/sensorGeometric2D.cpp \
    $$UNIT_UNDER_TEST/objectDetectorBase.cpp \
    $$WORLD_OSI/OWL/DataTypes.cpp \
    $$WORLD_OSI/OWL/OpenDriveTypeMapper.cpp \
    $$WORLD_OSI/WorldObjectAdapter.cpp \
    $$WORLD_OSI/WorldData.cpp \
    $$WORLD_OSI/WorldDataException.cpp \
    sensorOSI_Tests.cpp

LIBS += -lopen_simulation_interface -lprotobuf
