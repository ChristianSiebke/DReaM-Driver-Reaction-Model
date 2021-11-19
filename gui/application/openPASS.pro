################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#               2019 Volkswagen Group of America
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

include(../../sim/global.pri)

TARGET = openPASS
TEMPLATE = app
CONFIG += c++17 no_keywords
QT += core gui widgets

VERSION = 0.7
QMAKE_TARGET_PRODUCT = openPASS
QMAKE_TARGET_DESCRIPTION = openPASS
QMAKE_TARGET_COMPANY = Volkswagen Group of America
QMAKE_TARGET_COPYRIGHT = Volkswagen Group of America

Release:DESTDIR = $${DESTDIR_GUI}
Debug:DESTDIR = $${DESTDIR_GUI}

DEFINES += APPLICATION_NAME=\\\"$$TARGET\\\"
DEFINES += APPLICATION_VERSION=\\\"$$VERSION\\\"

include(Interfaces/Interfaces.pri)
include(Models/Models.pri)

RC_ICONS += \
    openPASS.ico

SOURCES += \
    openPASS.cpp
