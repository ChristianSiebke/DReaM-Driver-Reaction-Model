################################################################################
# Copyright (c) 2017-2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

DEFINES += ACTION_LONGITUDINAL_DRIVER_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
    action_longitudinalDriver.cpp \
    src/longitudinalDriver.cpp

HEADERS += \
    action_longitudinalDriver.h \
    src/longitudinalDriver.h
