#-----------------------------------------------------------------------------
# \file  AlgorithmFmuWrapper.pro
# \brief This file contains the information for the QtCreator-project of the
# module AlgorithmFmuWrapper
#
# Copyright (c) 2017, 2019 in-tech GmbH
# on behalf of BMW AG
#-----------------------------------------------------------------------------/
DEFINES += ALGORITHM_FMUWRAPPER_LIBRARY \
           USE_EXTENDED_OSI
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

QMAKE_CXXFLAGS += -fpermissive

INCLUDEPATH += \
    src \
    src/FmiImporter/include \
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary \
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary/FMI \    # required due to FMILibrary internal include structure
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary/FMI1 \   #  - " -
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary/FMI2 \   #  - " -
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary/JM \     #  - " -
    ../../core/slave/modules/World_OSI \
    ../../.. \
    ../..

SOURCES += \
    src/FmiImporter/src/Common/fmuChecker.c \
    src/FmiImporter/src/FMI1/fmi1_check.c \
    src/FmiImporter/src/FMI1/fmi1_cs_sim.c \
    src/FmiImporter/src/FMI1/fmi1_me_sim.c \
    src/FmiImporter/src/FMI2/fmi2_check.c \
    src/FmiImporter/src/FMI2/fmi2_cs_sim.c \
    src/FmiImporter/src/FMI2/fmi2_me_sim.c \
    AlgorithmFmuWrapper.cpp \
    src/fmuWrapper.cpp \
    src/OsmpFmuHandler.cpp

HEADERS += \
    src/FmiImporter/include/fmuChecker.h \
    AlgorithmFmuWrapper.h \
    src/fmuWrapper.h \
    src/OsmpFmuHandler.h

win32: {
    LIBS += -llibboost_filesystem-mgw81-mt-x64-1_72
}

unix: {
    LIBS += -lboost_filesystem
}

LIBS += -lfmilib_shared -lopen_simulation_interface -lprotobuf
