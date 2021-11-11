################################################################################
# Copyright (c) 2020-2021 ITK Engineering GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  Dynamics_TwoTrack.pro
# \brief This file contains the information for the QtCreator-project of the
#        module Dynamics_TwoTrack
#-----------------------------------------------------------------------------/

DEFINES += DYNAMICS_TWOTRACK_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)


SUBDIRS += . \
           src \
           ../../common/pcm \

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

INCLUDEPATH += $$SUBDIRS

SOURCES += \
    $$getFiles(SUBDIRS, cpp) \
    $$getFiles(SUBDIRS, cc) \
    $$getFiles(SUBDIRS, c)

HEADERS += \
    $$getFiles(SUBDIRS, hpp) \
    $$getFiles(SUBDIRS, h)
