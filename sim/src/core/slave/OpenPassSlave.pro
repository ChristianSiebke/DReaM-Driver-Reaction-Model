# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
# * Copyright (c) 2016, 2017 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  OpenPassSlave.pro
# \brief This file contains the information for the QtCreator-project of the
#        openPASS Slave
#-----------------------------------------------------------------------------

CONFIG += OPENPASS_EXECUTABLE
include(../../../global.pri)

win32 {
# -DLOG_TIME_ENABLED: enable logging of time information
LIBS += -lws2_32
}

OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc
UI_DIR = .ui

# accumulate list of files for given directories (first parameter)
# according to file name ending (second parameter)
defineReplace(getFiles) {
    variable = $$1
    files = $$eval($$variable)
    result =
    for(file, files) {
        result += $$files($$file/*.$$2)
    }
    return($$result)
}

SUBDIRS += \
    bindings \
    framework \
    framework/scheduler \
    importer \
    importer/road \
    modelElements \
    ../common/cephesMIT

INCLUDEPATH += $$SUBDIRS \
    .. \
    ../.. \
    ../../.. \
    ../../../..

SOURCES += \
    $$getFiles(SUBDIRS, cpp) \
    $$getFiles(SUBDIRS, cc) \
    $$getFiles(SUBDIRS, c) \
    ../common/callbacks.cpp \
    ../common/coreDataPublisher.cpp \
    ../common/log.cpp \
    ../../common/eventDetectorDefinitions.cpp \
    ../../../../common/xmlParser.cpp

HEADERS += \
    $$getFiles(SUBDIRS, hpp) \
    $$getFiles(SUBDIRS, h) \
    ../common/callbacks.h \
    ../common/coreDataPublisher.h \
    ../common/log.h \
    ../../common/eventDetectorDefinitions.h \
    ../../../../common/xmlParser.h
