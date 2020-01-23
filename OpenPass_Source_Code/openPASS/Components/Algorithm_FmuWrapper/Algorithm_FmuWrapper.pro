#-----------------------------------------------------------------------------
# \file  AlgorithmFmuWrapper.pro
# \brief This file contains the information for the QtCreator-project of the
# module AlgorithmFmuWrapper
#
# Copyright (c) 2017, 2019 in-tech GmbH
# on behalf of BMW AG
#-----------------------------------------------------------------------------/
DEFINES += ALGORITHM_FMUWRAPPER_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

QMAKE_CXXFLAGS += -fpermissive

INCLUDEPATH += \
    FmiImporter/include \
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary \
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary/FMI \    # required due to FMILibrary internal include structure
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary/FMI1 \   #  - " -
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary/FMI2 \   #  - " -
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary/JM \     #  - " -
    ../../CoreModules/World_OSI \

SOURCES += \
    FmiImporter/src/Common/fmuChecker.c \
    FmiImporter/src/FMI1/fmi1_check.c \
    FmiImporter/src/FMI1/fmi1_cs_sim.c \
    FmiImporter/src/FMI1/fmi1_me_sim.c \
    FmiImporter/src/FMI2/fmi2_check.c \
    FmiImporter/src/FMI2/fmi2_cs_sim.c \
    FmiImporter/src/FMI2/fmi2_me_sim.c \
    AlgorithmFmuWrapper.cpp \
    AlgorithmFmuWrapperImplementation.cpp \
    OsmpFmuHandler.cpp \
    ../../Common/vector2d.cpp

HEADERS += \
    FmiImporter/include/fmuChecker.h \
    AlgorithmFmuWrapper.h \
    AlgorithmFmuWrapperGlobal.h \
    AlgorithmFmuWrapperImplementation.h \
    OsmpFmuHandler.h

win32: {
    LIBS += \
        -llibboost_filesystem-mgw71-1_63 \
        -llibboost_system-mgw71-1_63
}

unix: {
    LIBS += -lboost_filesystem -lboost_system
}

LIBS += -lfmilib_shared -lopen_simulation_interface -lprotobuf
