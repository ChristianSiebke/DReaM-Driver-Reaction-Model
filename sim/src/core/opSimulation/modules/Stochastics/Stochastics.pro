################################################################################
# Copyright (c) 2016-2017 ITK Engineering GmbH
#               2017-2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  Stochastics.pro
# \brief This file contains the information for the QtCreator-project of the
#        module Stochastics
#-----------------------------------------------------------------------------/

DEFINES += STOCHASTICS_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../../../global.pri)

SUBDIRS +=  . \

INCLUDEPATH += \
    ../../../.. \
    ../../../../..

SOURCES += \
    $$getFiles(SUBDIRS, cpp) \
    $$getFiles(SUBDIRS, cc) \
    $$getFiles(SUBDIRS, c)

HEADERS += \
    $$getFiles(SUBDIRS, hpp) \
    $$getFiles(SUBDIRS, h)

unix {
    target.path = /usr/lib
    INSTALLS += target
}
