# /*********************************************************************
# * Copyright (c) 2020 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  Sensor_Collision.pro
# \brief This file contains the information for the QtCreator-project of the
#        module Sensor_Collision
#-----------------------------------------------------------------------------/


DEFINES += SENSOR_COLLISION_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)


SUBDIRS += . \
           ../../../../common/pcm \

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
    $$getFiles(SUBDIRS, cpp) \
    $$getFiles(SUBDIRS, cc) \
    $$getFiles(SUBDIRS, c)

HEADERS += \
    $$getFiles(SUBDIRS, hpp) \
    $$getFiles(SUBDIRS, h)
