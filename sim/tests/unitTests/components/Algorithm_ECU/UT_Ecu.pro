CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../testing.pri)

TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG += thread
CONFIG += qt

MAIN_SRC_DIR = $$PWD/../../../../src
include($$MAIN_SRC_DIR/global.pri)

INCLUDEPATH += c:/OpenPASS/thirdParty/include

INCLUDEPATH += . \
               $$MAIN_SRC_DIR/common \
               $$MAIN_SRC_DIR/../../common/pcm/PCM_Data \
               $$MAIN_SRC_DIR/components/Algorithm_ECU

HEADERS += $$MAIN_SRC_DIR/common/vector2d.h \
           $$MAIN_SRC_DIR/components/Algorithm_ECU/ecu.h \
           tst_ut_ecu.h


SOURCES += $$MAIN_SRC_DIR/components/Algorithm_ECU/ecu.cpp \
           tst_ut_ecu.cpp

Release:DESTDIR = $$DIR_RELEASE
Debug:DESTDIR = $$DIR_DEBUG
