# /*********************************************************************
# * Copyright (c) 2016 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  Sensor_Modular_Driver.pro
# \brief This file contains the information for the QtCreator-project of the
#         module Sensor_Modular_Driver
#-----------------------------------------------------------------------------/

DEFINES += SENSOR_MODULAR_DRIVER_LIBRARY
CONFIG += OPENPASS_LIBRARY

DEFINES += WORLD_LIBRARY
DEFINES += WORLD_CPP

include(../../../global.pri)

SUBDIRS +=  . \
            Container \
            Signals \
            ../../Interfaces \
            ../../Interfaces/roadInterface \
            ../../Common \

INCLUDEPATH += $$SUBDIRS \
            ../../CoreModules/World_OSI \
            ../../CoreModules/World_OSI/OWL \
            ../../CoreModules/World_OSI/Localization \
            ../../Common \
            ..

SOURCES += \
    $$getFiles(SUBDIRS, cpp) \
    $$getFiles(SUBDIRS, cc) \
    $$getFiles(SUBDIRS, c) \

HEADERS += \
    $$getFiles(SUBDIRS, hpp) \
    $$getFiles(SUBDIRS, h) \
