# /*********************************************************************
# * Copyright (c) 2019 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/


CONFIG += OPENPASS_LIBRARY_GUI
include(../../../sim/global.pri)

TARGET = Statistics
TEMPLATE = lib
CONFIG += console plugin c++17 no_keywords
QT += core concurrent gui sql widgets xml

VERSION = 0.6
QMAKE_TARGET_PRODUCT = openPASS
QMAKE_TARGET_DESCRIPTION = openPASS Statistics
QMAKE_TARGET_COMPANY = ITK Engineering GmbH
QMAKE_TARGET_COPYRIGHT = ITK Engineering GmbH

win32: TARGET_EXT = .dll

include(Interfaces/Interfaces.pri)
include(Models/Models.pri)
include(Presenters/Presenters.pri)
include(Views/Views.pri)

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

SUBDIRS +=  .

INCLUDEPATH += $$SUBDIRS \
    .. \
    ../../common

HEADERS += \
    $$getFiles(SUBDIRS, hpp) \
    $$getFiles(SUBDIRS, h) \
    ../../common/Histogram.h \
    ../../common/RowHistograms.h \
    ../../common/TableModel.h

SOURCES += \
    $$getFiles(SUBDIRS, cpp) \
    $$getFiles(SUBDIRS, cc) \
    $$getFiles(SUBDIRS, c) \
    ../../common/Histogram.cpp \
    ../../common/RowHistograms.cpp \
    ../../common/TableModel.cpp

