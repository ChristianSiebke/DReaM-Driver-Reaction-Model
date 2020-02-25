# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019 in-tech GmbH
# *               2016, 2017 ITK Engnineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  Sensor_Criticality.pro
# \brief This file contains the information for the QtCreator-project of the
# module Sensor_Criticality
#-----------------------------------------------------------------------------/

DEFINES += SENSOR_CRITICALITY_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS +=  . \
            ../Sensor_Modular_Driver/Signals \
            ../Sensor_Modular_Driver/Container \
            ../Algorithm_ModularDriver/SituationAssessmentMethods \
            ../../Common \
            ..

INCLUDEPATH += $$SUBDIRS \
            ../../Common \
            ../../Interfaces \
            ../Sensor_Driver/Signals \
            ..

SOURCES += \
    $$getFiles(SUBDIRS, cpp) \
    $$getFiles(SUBDIRS, cc) \
    $$getFiles(SUBDIRS, c)\

HEADERS += \
    $$getFiles(SUBDIRS, hpp) \
    $$getFiles(SUBDIRS, h) \
