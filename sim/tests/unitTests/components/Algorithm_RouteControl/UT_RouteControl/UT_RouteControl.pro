#****************************
# Copyright (c) 2018 Daimler AG.
# All rights reserved.
#****************************/

include(../../../components_custom/UT.pri)

TARGET = tst_ut_RouteControl

INCLUDEPATH += . \
               $$MAIN_SRC_DIR/src \
               $$MAIN_SRC_DIR/src/common \
               $$MAIN_SRC_DIR/../common/pcm/PCM_Data \
               $$MAIN_SRC_DIR/src/components/Algorithm_Routecontrol \


HEADERS += $$MAIN_SRC_DIR/src/common/vector2d.h \
           $$MAIN_SRC_DIR//src/components/Algorithm_Routecontrol/routeControl.h \
           $$MAIN_SRC_DIR/../common/pcm/PCM_Data/pcm_trajectory.h \
           tst_ut_RouteControl.h


SOURCES += $$MAIN_SRC_DIR/src/components/Algorithm_Routecontrol/routeControl.cpp \
           $$MAIN_SRC_DIR/../common/pcm/PCM_Data/pcm_trajectory.h \
           tst_ut_RouteControl.cpp \
           main.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

