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
# \file  openPASS-System.pro
# \brief This file is the building project of the plugin manging the window
#        and the interface that includes data related to the System
#-----------------------------------------------------------------------------/

CONFIG += OPENPASS_LIBRARY_GUI
include(../../../sim/global.pri)

TARGET = System
TEMPLATE = lib
CONFIG += plugin c++17 no_keywords
QT += core gui widgets

VERSION = 0.6
QMAKE_TARGET_PRODUCT = openPASS
QMAKE_TARGET_DESCRIPTION = openPASS System
QMAKE_TARGET_COMPANY = Volkswagen Group of America
QMAKE_TARGET_COPYRIGHT = Volkswagen Group of America

win32: TARGET_EXT = .dll

include(Interfaces/Interfaces.pri)
include(Models/Models.pri)
include(Views/Views.pri)

RESOURCES += \
    openPASS-System.qrc

HEADERS += \
    SystemPlugin.h

SOURCES += \
    SystemPlugin.cpp

DIR_PRO = $${PWD}

win32 {
DIR_PRO ~= s,/,\\,g
DESTDIR_GUI ~= s,/,\\,g
SUBDIR_LIB_COMPONENTS ~= s,/,\\,g
copydata.commands = xcopy /s /q /y /i $$DIR_PRO\..\..\components $${DESTDIR_GUI}$${SUBDIR_LIB_COMPONENTS}\
} else {
copydata.commands = cp -f -R $$DIR_PRO/../../components $${DESTDIR_GUI}$${SUBDIR_LIB_COMPONENTS}
}
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
