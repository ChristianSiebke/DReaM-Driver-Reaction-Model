# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  Sensor_OSI.pro
# \brief This file contains the information for the QtCreator-project of the
# module Sensor_OSI
#-----------------------------------------------------------------------------/

DEFINES += SENSOR_OBJECT_DETECTOR_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

INCLUDEPATH += \
    ../../CoreModules/World_OSI

SOURCES += \
    ../../CoreModules/World_OSI/OWL/DataTypes.cpp \
    ../../CoreModules/World_OSI/OWL/OpenDriveTypeMapper.cpp \
    ../../CoreModules/World_OSI/WorldData.cpp \
    ../../CoreModules/World_OSI/WorldDataException.cpp \
    ../../Common/vector2d.cpp \
    objectDetectorBase.cpp \
    sensorGeometric2D.cpp \
    sensorObjectDetectorFactory.cpp

HEADERS += \
    ../../CoreModules/World_OSI/WorldData.h \
    objectDetectorBase.h \
    sensorGeometric2D.h \
    sensorObjectDetectorFactory.h \
    sensorObjectDetectorGlobal.h

LIBS += -lopen_simulation_interface -lprotobuf
