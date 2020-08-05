# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019 in-tech GmbH
# *               2019 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  Dynamics_Collision.pro
# \brief This file contains the information for the QtCreator-project of the
# module Dynamics_Collision
#-----------------------------------------------------------------------------/

DEFINES += DYNAMICS_COLLISIONPOSTCRASH_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

CONFIG(debug, release|debug):DEFINES += QT_DEBUG

SUBDIRS +=  . \
            ../../Common \
            ../../Interfaces \
            ..

INCLUDEPATH += $$SUBDIRS

SOURCES += \
    $$getFiles(SUBDIRS, cpp) \
    $$getFiles(SUBDIRS, cc) \
    $$getFiles(SUBDIRS, c)

HEADERS += \
    $$getFiles(SUBDIRS, hpp) \
    $$getFiles(SUBDIRS, h)
