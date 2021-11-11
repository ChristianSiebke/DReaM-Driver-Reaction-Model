################################################################################
# Copyright (c) 2019 ITK Engineering GmbH
#               2017-2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  Dynamics_Collision.pro
# \brief This file contains the information for the QtCreator-project of the
# module Dynamics_Collision
#-----------------------------------------------------------------------------/

DEFINES += DYNAMICS_COLLISIONPOSTCRASH_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
    dynamics_collisionPostCrash.cpp \
    src/collisionPostCrash.cpp

HEADERS += \
    dynamics_collisionPostCrash.h \
    src/collisionPostCrash.h
