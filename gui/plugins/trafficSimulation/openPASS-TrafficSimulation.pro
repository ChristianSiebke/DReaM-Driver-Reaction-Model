# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  openPASS-TrafficSimulation.pro
# \brief This project file configures the Traffic Simulation plugin which can be
#        used to edit the slaveConfig xml file for traffic flow simulations.
#-----------------------------------------------------------------------------/

CONFIG += OPENPASS_LIBRARY_GUI
include(../../../global.pri)

TARGET = TrafficSimulation
TEMPLATE = lib
CONFIG += plugin c++11 no_keywords
QT += core gui widgets
QMAKE_CXXFLAGS += -DQT_NO_UNICODE_LITERAL

VERSION = 0.6
QMAKE_TARGET_PRODUCT = openPASS
QMAKE_TARGET_DESCRIPTION = openPASS TrafficSimulation
QMAKE_TARGET_COMPANY =  Volkswagen Group of America
QMAKE_TARGET_COPYRIGHT =  Volkswagen Group of America

win32: TARGET_EXT = .dll

include(Interfaces/Interfaces.pri)
include(Models/Models.pri)
include(Presenters/Presenters.pri)
include(Views/Views.pri)

INCLUDEPATH += ../../common/

HEADERS += \
    TrafficSimulationPlugin.h

SOURCES += \
    TrafficSimulationPlugin.cpp
