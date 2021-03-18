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
               $$MAIN_SRC_DIR/../../common/pcm \
               $$MAIN_SRC_DIR/components/Algorithm_Switch

HEADERS += $$MAIN_SRC_DIR/components/Algorithm_Switch/switch.h \
           $$MAIN_SRC_DIR/../../common/pcm/controlData.h \
           tst_ut_switch.h


SOURCES += $$MAIN_SRC_DIR/components/Algorithm_Switch/switch.cpp \
           tst_ut_switch.cpp

Release:DESTDIR = $$DIR_RELEASE
Debug:DESTDIR = $$DIR_DEBUG
