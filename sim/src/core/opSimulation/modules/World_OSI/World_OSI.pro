# /*********************************************************************
# * Copyright (c) 2017-2021 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  World_OSI.pro
# \brief This file contains the information for the QtCreator-project of the
#        module World_OSI
#-----------------------------------------------------------------------------/

DEFINES += WORLD_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../../../global.pri)

SUBDIRS += \
    . \
    Localization \
    OWL \
    RoutePlanning \
    ../../../../..

INCLUDEPATH += \
    $$SUBDIRS \
    .. \
    ../../.. \
    ../../../.. \
    ../../../../..

SOURCES += \
    ../../../../common/commonTools.cpp \
    $$getFiles(SUBDIRS, cpp) \
    $$getFiles(SUBDIRS, cc) \
    $$getFiles(SUBDIRS, c)

HEADERS += \
    ../../../../common/commonTools.h \
    $$getFiles(SUBDIRS, hpp) \
    $$getFiles(SUBDIRS, h)

LIBS += -lopen_simulation_interface -lprotobuf
