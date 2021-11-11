################################################################################
# Copyright (c) 2019 Volkswagen Group of America
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  openPASS-Project.pro
# \brief This file is the building project of the plugin manging the window
#        and the interface that includes data related to the Project
#-----------------------------------------------------------------------------/

CONFIG += OPENPASS_LIBRARY_GUI
include(../../../sim/global.pri)

TARGET = Project
TEMPLATE = lib
CONFIG += plugin c++17 no_keywords
QT += core gui widgets

VERSION = 0.6
QMAKE_TARGET_PRODUCT = openPASS
QMAKE_TARGET_DESCRIPTION = openPASS Project
QMAKE_TARGET_COMPANY = Volkswagen Group of America
QMAKE_TARGET_COPYRIGHT = Volkswagen Group of America


win32: TARGET_EXT = .dll


include(Interfaces/Interfaces.pri)
include(Models/Models.pri)
include(Presenters/Presenters.pri)
include(Views/Views.pri)

HEADERS += \
    ProjectPlugin.h

SOURCES += \
    ProjectPlugin.cpp
