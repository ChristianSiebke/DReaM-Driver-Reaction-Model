#-----------------------------------------------------------------------------
# \file  algorithmAEB_Tests.pro
#
# Copyright (c) 2019 in-tech GmbH on behalf of BMW AG
#-----------------------------------------------------------------------------/
CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../testing.pri)

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

INCLUDEPATH += \
    $$relative_path($$OPEN_SRC)/Components/Algorithm_AEB

HEADERS += \
    AlgorithmAebOSIUnitTests.h \
    $$relative_path($$OPEN_SRC)/Common/boostGeometryCommon.h \
    $$relative_path($$OPEN_SRC)/Common/vector2d.h \
    $$relative_path($$OPEN_SRC)/Components/Algorithm_AEB/algorithm_autonomousEmergencyBrakingImplementation.h \
    $$relative_path($$OPEN_SRC)/Components/Algorithm_AEB/boundingBoxCalculation.h

SOURCES += \
    AlgorithmAeb_Tests.cpp \
    BoundingBoxCalculation_Tests.cpp \
    $$relative_path($$OPEN_SRC)/Common/vector2d.cpp \
    $$relative_path($$OPEN_SRC)/Components/Algorithm_AEB/algorithm_autonomousEmergencyBrakingImplementation.cpp \
    $$relative_path($$OPEN_SRC)/Components/Algorithm_AEB/boundingBoxCalculation.cpp

LIBS += \
    -lopen_simulation_interface \
    -lzmq \
    -lprotobuf
