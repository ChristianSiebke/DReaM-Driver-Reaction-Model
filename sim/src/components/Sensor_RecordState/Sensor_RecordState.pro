################################################################################
# Copyright (c) 2016-2017 ITK Engineering GmbH
#               2017-2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  Sensor_RecordState.pro
# \brief This file contains the information for the QtCreator-project of the
# module Sensor_RecordState
#-----------------------------------------------------------------------------/

DEFINES += SENSOR_RECORD_STATE_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
    sensor_recordState.cpp \
    src/sensor_recordStateImpl.cpp

HEADERS += \
    sensor_recordState.h \
    src/sensor_recordStateImpl.h
