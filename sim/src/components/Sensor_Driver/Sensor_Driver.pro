# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
# *               2018 AMFD GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  Sensor_Driver.pro
# \brief This file contains the information for the QtCreator-project of the
# module Sensor_Driver
#-----------------------------------------------------------------------------/

DEFINES += SENSOR_DRIVER_LIBRARY
CONFIG += OPENPASS_LIBRARY

include(../../../global.pri)

SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
    sensor_driver.cpp \
    src/sensor_driverCalculations.cpp \
    src/sensor_driverImpl.cpp

HEADERS += \
    sensor_driver.h \
    src/sensor_driverCalculations.h \
    src/sensor_driverImplementation.h \
    src/Signals/sensorDriverSignal.h \
    src/Signals/sensor_driverDefinitions.h \
    ../../core/slave/modules/World_OSI/RoutePlanning/RouteCalculation.h
