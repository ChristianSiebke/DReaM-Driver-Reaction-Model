# /*********************************************************************
# * Copyright (c) 2019 in-tech GmbH
# *               2019 AMFD GmbH
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

UNIT_UNDER_TEST = $$OPEN_SRC/Components/Sensor_Driver

INCLUDEPATH += \
    . \
    $$UNIT_UNDER_TEST \
    $$OPEN_SRC/CoreModules/World_OSI/RoutePlanning

HEADERS += \
    $$UNIT_UNDER_TEST/sensor_driverCalculations.h \
    $$UNIT_UNDER_TEST/sensor_driverImplementation.h \
    $$OPEN_SRC/CoreModules/World_OSI/RoutePlanning/RouteCalculation.h

SOURCES += \
    $$UNIT_UNDER_TEST/sensor_driverCalculations.cpp \
    $$UNIT_UNDER_TEST/sensor_driverImplementation.cpp \
    sensorDriver_Tests.cpp
