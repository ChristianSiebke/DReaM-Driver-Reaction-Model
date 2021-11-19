################################################################################
# Copyright (c) 2019-2020 ITK Engineering GmbH
#               2017-2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  Dynamics_RegularTwoTrack.pro
# \brief This file contains the information for the QtCreator-project of the
# module Dynamics_RegularTwoTrack
#-----------------------------------------------------------------------------/

DEFINES += DYNAMICS_REGULAR_TWOTRACK_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
    dynamics_regularTwoTrack.cpp \
    src/dynamics_regularTwoTrackImpl.cpp \
    src/dynamics_twotrack_tire.cpp \
    src/dynamics_twotrack_vehicle.cpp

HEADERS += \
    dynamics_regularTwoTrack.h \
    src/dynamics_regularTwoTrackImpl.h \
    src/dynamics_twotrack_tire.h \
    src/dynamics_twotrack_vehicle.h
