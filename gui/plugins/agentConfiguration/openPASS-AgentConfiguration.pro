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
# \file  openPASS-AgentConfiguration.pro
# \brief This plugin is intended to configure agents to be used within the
#        traffiSimulationPlugin
#-----------------------------------------------------------------------------/

CONFIG += OPENPASS_LIBRARY_GUI
include(../../../global.pri)

TARGET = AgentConfiguration
TEMPLATE = lib
CONFIG += plugin c++17 no_keywords
QT += core gui widgets
QMAKE_CXXFLAGS += -DQT_NO_UNICODE_LITERAL

VERSION = 0.6
QMAKE_TARGET_PRODUCT = openPASS
QMAKE_TARGET_DESCRIPTION = openPASS AgentConfiguration
QMAKE_TARGET_COMPANY = Volkswagen Group of America
QMAKE_TARGET_COPYRIGHT = Volkswagen Group of America

win32: TARGET_EXT = .dll

include(Interfaces/Interfaces.pri)
include(Models/Models.pri)
include(Presenters/Presenters.pri)
include(Views/Views.pri)

INCLUDEPATH += ../../common/

HEADERS += \
    AgentConfigurationPlugin.h \


SOURCES += \
    AgentConfigurationPlugin.cpp
