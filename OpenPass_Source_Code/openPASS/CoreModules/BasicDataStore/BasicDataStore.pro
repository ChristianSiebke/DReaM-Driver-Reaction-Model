# /*********************************************************************
# * Copyright (c) 2020 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  BasicDataStore.pro
# \brief This file contains the information for the QtCreator-project of the
# module BasicDataStore
#-----------------------------------------------------------------------------/

CONFIG += OPENPASS_LIBRARY
DEFINES += BASIC_DATASTORE_LIBRARY

include(../../../global.pri)

INCLUDEPATH += \
    ../../CoreFrameWork/CoreShare

SOURCES += \
    basicDataStore.cpp \
    basicDataStoreImplementation.cpp

HEADERS += \
    basicDataStore.h \
    basicDataStoreImplementation.h
