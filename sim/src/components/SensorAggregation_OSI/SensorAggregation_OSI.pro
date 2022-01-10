################################################################################
# Copyright (c) 2017-2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  SensorAggregation_OSI.pro
# \brief This file contains the information for the QtCreator-project of the
# module SensorAggregation_OSI
#-----------------------------------------------------------------------------/

DEFINES += SENSOR_AGGREGATION_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)


SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
    sensorAggregation.cpp \
    src/sensorAggregationImpl.cpp

HEADERS += \
    sensorAggregation.h \
    src/sensorAggregationImpl.h

LIBS += \
    -lopen_simulation_interface \
    -lprotobuf
