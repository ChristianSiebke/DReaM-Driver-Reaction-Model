# /*********************************************************************
# * Copyright (c) 2017, 2019, 2020 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

CONFIG += OPENPASS_GTEST OPENPASS_GTEST_DEFAULT_MAIN
DEFINES += USE_EXTENDED_OSI

include(../../../testing.pri)

QMAKE_CXXFLAGS += -fpermissive
win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

INCLUDEPATH += \
    . \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/FmiImporter/include \
    $$OPEN_SRC/core/opSimulation/modules/World_OSI \
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary \
    FMI \    # third party
    FMI1 \
    FMI2 \
    JM \
    ../../../..

HEADERS += \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/OsmpFmuHandler.h \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/FmiImporter/include/fmuChecker.h \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/FmiImporter/include/fmi1_input_reader.h \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/FmiImporter/include/fmi2_input_reader.h

SOURCES += \
    OsmpFmuUnitTests.cpp \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/OsmpFmuHandler.cpp \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/FmiImporter/src/Common/fmuChecker.c \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/FmiImporter/src/FMI1/fmi1_check.c \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/FmiImporter/src/FMI1/fmi1_cs_sim.c \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/FmiImporter/src/FMI1/fmi1_me_sim.c \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/FmiImporter/src/FMI2/fmi2_check.c \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/FmiImporter/src/FMI2/fmi2_cs_sim.c \
    $$OPEN_SRC/components/Algorithm_FmuWrapper/src/FmiImporter/src/FMI2/fmi2_me_sim.c

win32 {
    LIBS += -lboost_filesystem-mt
} else {
    LIBS += -lboost_filesystem
}

LIBS += -lfmilib_shared -lopen_simulation_interface -lprotobuf

