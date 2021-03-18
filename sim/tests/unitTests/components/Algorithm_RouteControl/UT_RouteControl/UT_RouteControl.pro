#****************************
# Copyright (c) 2018 Daimler AG.
# All rights reserved.
#****************************/

include(../../../components_custom/UT.pri)

TARGET = tst_ut_RouteControl

INCLUDEPATH += . \
               $$MAIN_SRC_DIR/src \
               $$MAIN_SRC_DIR/src/common \
               $$MAIN_SRC_DIR/src/components/Algorithm_Routecontrol \

HEADERS += $$MAIN_SRC_DIR/src/common/vector2d.h \
           $$MAIN_SRC_DIR//src/components/Algorithm_Routecontrol/routeControl.h \

SOURCES += $$MAIN_SRC_DIR/src/components/Algorithm_Routecontrol/routeControl.cpp \
           tst_ut_RouteControl.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

