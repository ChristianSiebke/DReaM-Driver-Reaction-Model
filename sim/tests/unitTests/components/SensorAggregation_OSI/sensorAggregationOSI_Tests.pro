################################################################################
# Copyright (c) 2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../testing.pri)

UNIT_UNDER_TEST = $$OPEN_SRC/components/SensorAggregation_OSI/src

INCLUDEPATH += \
    . \
    $$UNIT_UNDER_TEST \
    ../../../..

HEADERS += \
    $$UNIT_UNDER_TEST/sensorAggregationImpl.h

SOURCES += \
    $$UNIT_UNDER_TEST/sensorAggregationImpl.cpp \
    sensorAggregationOSI_Tests.cpp

LIBS += \
    -lopen_simulation_interface \
    -lprotobuf
