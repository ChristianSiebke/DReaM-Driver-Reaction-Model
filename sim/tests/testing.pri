################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#               2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

include($$PWD/../defaults.pri)

isEmpty(OPEN_SRC) {
    OPEN_SRC=$$absolute_path($$system_path($$PWD/../src))
    warning(OPEN_SRC not set. Falling back to default ($$OPEN_SRC))
} else {
    message(OPEN_SRC set to $$OPEN_SRC)
}

!exists($$OPEN_SRC) {
    error(Path $$OPEN_SRC does not exists)
}

isEmpty(EXTRA_INCLUDE_PATH) {
    EXTRA_INCLUDE_PATH="$$PWD/../deps/thirdParty/include"
}

isEmpty(EXTRA_LIB_PATH) {
    EXTRA_LIB_PATH=$$PWD/../deps/thirdParty/lib
}

isEmpty(DESTDIR) {
    DESTDIR=/OpenPASS/bin/tests
    warning(DESTDIR not set. Falling back to default ($$DESTDIR))
}

include($$PWD/../ccache.pri)

CONFIG+=c++17
DEFINES+=QMAKE_BUILD

## debug postfix not working in qmake build
DEFINES+=DEBUG_POSTFIX=\\\"\\\"

debug {
    # Helps preventing x86_64-w64-mingw32 related error "Fatal error: can't close debug\SOMEFILE.o: File too big"
    win32:QMAKE_CXXFLAGS += -O1
}


## executable destination directories ##
win32 {
    DIR_DEBUG = "C:/OpenPASS/BinDebug"
    DIR_RELEASE = "C:/OpenPASS/BinRelease"
}

unix {
    DIR_DEBUG = "/OpenPASS/BinDebug"
    DIR_RELEASE = "/OpenPASS/BinRelease"
}

## activate log time output
DEFINES += LOG_TIME_ENABLED

## simulation libraries destination sub-directory ##
SUBDIR_LIB_SIMS = "/lib"
DEFINES += SUBDIR_LIB_SIM=\\\"/lib\\\"

## gui libraries destination sub-directory ##
SUBDIR_LIB_GUIS = "/gui"
DEFINES += SUBDIR_LIB_GUI=\\\"/gui\\\"

## Qt plugins sub-directory ##
DEFINES += SUBDIR_LIB_PLUGIN=\\\"/plugin\\\"

# prefer bundled libs in opSimulation directory over system libraries
QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'

##################################################################
# Configuration specific for open pass gtest/gmock test projects #
# Usage:                                                         #
# set "CONFIG += OPENPASS_GTEST" before including this file      #
##################################################################

#thirdparty restructure workaround
win32 {
    QMAKE_CFLAGS += -isystem $$PWD\..\deps\thirdParty\win64\boost\include\boost-1_72
    QMAKE_CFLAGS += -isystem $$PWD\..\deps\thirdParty\win64\FMILibrary\include
    QMAKE_CFLAGS += -isystem $$PWD\..\deps\thirdParty\win64\googletest\include
    QMAKE_CFLAGS += -isystem $$PWD\..\deps\thirdParty\win64\osi\include
    QMAKE_CFLAGS += -isystem $$PWD\..\deps\thirdParty\win64\protobuf\include
    QMAKE_CXXFLAGS += -isystem $$PWD\..\deps\thirdParty\win64\boost\include\boost-1_72
    QMAKE_CXXFLAGS += -isystem $$PWD\..\deps\thirdParty\win64\FMILibrary\include
    QMAKE_CXXFLAGS += -isystem $$PWD\..\deps\thirdParty\win64\googletest\include
    QMAKE_CXXFLAGS += -isystem $$PWD\..\deps\thirdParty\win64\osi\include
    QMAKE_CXXFLAGS += -isystem $$PWD\..\deps\thirdParty\win64\protobuf\include
    QMAKE_LFLAGS += -L $$PWD\..\deps\thirdParty\win64\boost\lib
    QMAKE_LFLAGS += -L $$PWD\..\deps\thirdParty\win64\FMILibrary\lib
    QMAKE_LFLAGS += -L $$PWD\..\deps\thirdParty\win64\googletest\lib
    QMAKE_LFLAGS += -L $$PWD\..\deps\thirdParty\win64\osi\lib\osi3
    QMAKE_LFLAGS += -L $$PWD\..\deps\thirdParty\win64\protobuf\lib
}
unix {
    QMAKE_CFLAGS += -isystem $$PWD/../deps/thirdParty/linux64/boost/include
    QMAKE_CFLAGS += -isystem $$PWD/../deps/thirdParty/linux64/FMILibrary/include
    QMAKE_CFLAGS += -isystem $$PWD/../deps/thirdParty/linux64/googletest/include
    QMAKE_CFLAGS += -isystem $$PWD/../deps/thirdParty/linux64/osi/include
    QMAKE_CFLAGS += -isystem $$PWD/../deps/thirdParty/linux64/protobuf/include
    QMAKE_CXXFLAGS += -isystem $$PWD/../deps/thirdParty/linux64/boost/include
    QMAKE_CXXFLAGS += -isystem $$PWD/../deps/thirdParty/linux64/FMILibrary/include
    QMAKE_CXXFLAGS += -isystem $$PWD/../deps/thirdParty/linux64/googletest/include
    QMAKE_CXXFLAGS += -isystem $$PWD/../deps/thirdParty/linux64/osi/include
    QMAKE_CXXFLAGS += -isystem $$PWD/../deps/thirdParty/linux64/protobuf/include
    QMAKE_LFLAGS += -L $$PWD/../deps/thirdParty/linux64/boost/lib
    QMAKE_LFLAGS += -L $$PWD/../deps/thirdParty/linux64/FMILibrary/lib
    QMAKE_LFLAGS += -L $$PWD/../deps/thirdParty/linux64/googletest/lib
    QMAKE_LFLAGS += -L $$PWD/../deps/thirdParty/linux64/osi/lib/osi3
    QMAKE_LFLAGS += -L $$PWD/../deps/thirdParty/linux64/protobuf/lib
}

OPENPASS_GTEST {
    message("[$$TARGET] Building test")

    QT += xml xmlpatterns
    TEMPLATE = app
    CONFIG += console
    CONFIG += testcase
    CONFIG -= app_bundle
    CONFIG += COPY_RESOURCES
    QMAKE_PRE_LINK += $$sprintf($$QMAKE_MKDIR_CMD, $$DESTDIR)

    QMAKE_LFLAGS += -L$$system_path($$EXTRA_LIB_PATH)
    QMAKE_CFLAGS += -isystem $$EXTRA_INCLUDE_PATH
    QMAKE_CXXFLAGS += -isystem $$EXTRA_INCLUDE_PATH

    LIBS += -lgtest -lgmock

    INCLUDEPATH+=$$OPEN_SRC
    INCLUDEPATH+=$$absolute_path($$system_path($$PWD))
    INCLUDEPATH+=$$absolute_path($$system_path($$PWD/common/gtest))
    INCLUDEPATH+=$$absolute_path($$system_path($$PWD/fakes/gmock))

    CONFIG(CODECOVERAGE) {
        message("[$$TARGET] Enabling code coverage generation")
        QMAKE_LFLAGS += -lgcov --coverage
        QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    }
}

OPENPASS_GTEST_DEFAULT_MAIN {
    message("[$$TARGET] Using default main implementation")
    SOURCES+= $$absolute_path($$system_path($$PWD/common/gtest/unitTestMain.cpp))
    SOURCES+= $$absolute_path($$system_path($$PWD/common/gtest/mainHelper.cpp))
}

COPY_RESOURCES {
    exists($$_PRO_FILE_PWD_/Resources) {
        win32:copydata.commands = xcopy /s /q /y /i $$system_path($$_PRO_FILE_PWD_/Resources) $$system_path($$DESTDIR/Resources)
        unix:copydata.commands = cp -f -R $$system_path($$_PRO_FILE_PWD_/Resources) $$DESTDIR
        first.depends = $(first) copydata
        export(first.depends)
        export(copydata.commands)
        QMAKE_EXTRA_TARGETS += first copydata
    }
}

