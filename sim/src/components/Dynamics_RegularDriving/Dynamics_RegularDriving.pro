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
# \file  Dynamics_RegularDriving.pro
# \brief This file contains the information for the QtCreator-project of the
# module Dynamics_RegularDriving
#-----------------------------------------------------------------------------/

DEFINES += DYNAMICS_REGULAR_DRIVING_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
    dynamics_regularDriving.cpp \
    src/regularDriving.cpp

HEADERS += \
    dynamics_regularDriving.h \
    src/regularDriving.h
