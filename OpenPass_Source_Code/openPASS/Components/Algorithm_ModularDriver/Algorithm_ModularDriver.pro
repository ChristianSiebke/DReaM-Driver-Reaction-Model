#-----------------------------------------------------------------------------
# /file  AlgorithmModularDriver.pro
# /brief This file contains the information for the QtCreator-project of the
# module AlgorithmModularDriver
#
# Copyright (c) 2018 in-tech GmbH
#
#-----------------------------------------------------------------------------/
DEFINES += ALGORITHM_MODULARDRIVER_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS +=  . \
            ../Sensor_Modular_Driver/Signals \
            ../Sensor_Modular_Driver/Container \
            ActionDeductionMethods \
            SituationAssessmentMethods \
            ../../Common \
            ../../Interfaces \
            ../Sensor_Driver/Signals \
            ..

INCLUDEPATH += $$SUBDIRS \
            ../Algorithm_Longitudinal \
            ..


SOURCES += \
    $$getFiles(SUBDIRS, cpp) \
    $$getFiles(SUBDIRS, cc) \
    $$getFiles(SUBDIRS, c) \
    ../Algorithm_Longitudinal/algorithm_longitudinalCalculations.cpp \

HEADERS += \
    $$getFiles(SUBDIRS, hpp) \
    $$getFiles(SUBDIRS, h) \
    ../Algorithm_Longitudinal/algorithm_longitudinalCalculations.h \
