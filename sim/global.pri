################################################################################
# Copyright (c) 2018-2019 ITK Engineering GmbH
#               2018 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

include(defaults.pri)
include(ccache.pri)

!isEmpty(INTERFACE_OVERRIDE_PATH) {
    INCLUDEPATH += $$INTERFACE_OVERRIDE_PATH
    INCLUDEPATH += $$INTERFACE_OVERRIDE_PATH/core/opSimulation
}

INCLUDEPATH += $$PWD/openPASS
CONFIG += c++17
## debug postfix not working in qmake build
DEFINES+=DEBUG_POSTFIX=\\\"\\\"

## common and CoreShare are not built as libraries in the qmake build
DEFINES+=QMAKE_BUILD

## activate log time output
DEFINES += LOG_TIME_ENABLED

## simulation libraries destination sub-directory ##
SUBDIR_LIB_SIMS = "/lib"
DEFINES += SUBDIR_LIB_SIM=\\\"/lib\\\"

## gui libraries destination sub-directory ##
SUBDIR_LIB_GUIS = "/gui"
SUBDIR_LIB_COMPONENTS = "\components"

DEFINES += SUBDIR_LIB_GUI=\\\"/gui\\\"
DEFINES += SUBDIR_LIB_COMPONENTS=\\\"/components\\\"

## Qt plugins sub-directory ##
DEFINES += SUBDIR_LIB_PLUGIN=\\\"/plugin\\\"

## Export configuration ##
#CONFIG += USEOPENPASSSIMULATIONASLIBRARY
#CONFIG += USEOPENPASSSIMULATIONMANAGERASLIBRARY

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

# prefer bundled libs in OpenPassSimulation directory over system libraries
QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'

win32:DEFINES+=_USE_MATH_DEFINES

######################################################################
# Configuration specific for open pass gui library                   #
# Usage:                                                             #
# set "CONFIG += OPENPASS_LIBRARY_GUI" before including this file    #
######################################################################
OPENPASS_LIBRARY_GUI {
    message("[$$TARGET] Set library configuration")
    TEMPLATE = lib
    CONFIG += shared
    CONFIG -= lib_bundle
    QMAKE_LFLAGS += -L$$system_path($$EXTRA_LIB_PATH)
    QMAKE_CFLAGS += -isystem $$EXTRA_INCLUDE_PATH
    QMAKE_CXXFLAGS += -isystem $$EXTRA_INCLUDE_PATH
    Debug:DESTDIR = $${DESTDIR_GUI}$${SUBDIR_LIB_GUIS}
    Release:DESTDIR = $${DESTDIR_GUI}$${SUBDIR_LIB_GUIS}
    #debug/release predicates are buggy on linux qmake
    unix:DESTDIR=$${DESTDIR_GUI}$${SUBDIR_LIB_GUIS}
    message("[$$TARGET] Build path set to $$DESTDIR")
}

##################################################################
# Configuration specific for open pass simulation libraries      #
# Usage:                                                         #
# set "CONFIG += OPENPASS_LIBRARY" before including this file    #
##################################################################
OPENPASS_LIBRARY {
    message("[$$TARGET] Set library configuration")
    TEMPLATE = lib
    CONFIG += shared
    CONFIG -= lib_bundle
    QMAKE_LFLAGS += -L$$system_path($$EXTRA_LIB_PATH)
    QMAKE_CFLAGS += -isystem $$EXTRA_INCLUDE_PATH
    QMAKE_CXXFLAGS += -isystem $$EXTRA_INCLUDE_PATH
    Debug:DESTDIR = $${DESTDIR_SIMULATION}$${SUBDIR_LIB_SIMS}
    Release:DESTDIR = $${DESTDIR_SIMULATION}$${SUBDIR_LIB_SIMS}
    #debug/release predicates are buggy on linux qmake
    unix:DESTDIR=$${DESTDIR_SIMULATION}$${SUBDIR_LIB_SIMS}
    message("[$$TARGET] Build path set to $$DESTDIR")
}

##################################################################
# Configuration specific for open pass executables               #
# Usage:                                                         #
# set "CONFIG += OPENPASS_EXECUTABLE" before including this file #
##################################################################
OPENPASS_EXECUTABLE {
    message("[$$TARGET] Building executable")
    QT += core xml xmlpatterns
    QT -= gui
    TEMPLATE = app
    CONFIG -= app_bundle
    CONFIG += no_include_pwd
    INCLUDEPATH += .
    QMAKE_CFLAGS += -isystem $$EXTRA_INCLUDE_PATH
    QMAKE_CXXFLAGS += -isystem $$EXTRA_INCLUDE_PATH
    Debug:DESTDIR = $${DESTDIR_SIMULATION}
    Release:DESTDIR = $${DESTDIR_SIMULATION}
    #debug/release predicates are buggy on linux qmake
    unix:DESTDIR=$${DESTDIR_SIMULATION}
    QMAKE_PRE_LINK += $$sprintf($$QMAKE_MKDIR_CMD, $$DESTDIR)
}

##################################################################
# Configuration specific for open pass testing projects          #
# Usage:                                                         #
# set "CONFIG += OPENPASS_TESTING" before including this file    #
##################################################################

OPENPASS_TESTING {
    message("[$$TARGET] Building test")

    QT += xml xmlpatterns
    TEMPLATE = app
    CONFIG += console
    CONFIG += testcase
    CONFIG -= app_bundle
    CONFIG += COPY_RESOURCES

    QMAKE_CFLAGS += -isystem $$EXTRA_INCLUDE_PATH
    QMAKE_CXXFLAGS += -isystem $$EXTRA_INCLUDE_PATH
    Debug:DESTDIR = $${DIR_DEBUG}
    Release:DESTDIR = $${DIR_RELEASE}
    #debug/release predicates are buggy on linux qmake
    unix:DESTDIR=$${DIR_DEBUG}

    QMAKE_PRE_LINK += $$sprintf($$QMAKE_MKDIR_CMD, $$DESTDIR)

    # reduce gtest warnings
    # CCFLAG += -Wno-zero-as-null-pointer-constant -Wno-padded

    #INCLUDEPATH += ../TestClasses
    LIBS += -lgtest -lgmock

    CONFIG(CODECOVERAGE) {
        message("[$$TARGET] Enabling code coverage generation")
        QMAKE_LFLAGS += -lgcov --coverage
        QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    }
}
