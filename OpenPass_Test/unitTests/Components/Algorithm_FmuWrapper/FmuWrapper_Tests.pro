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
include(../../../testing.pri)

QMAKE_CXXFLAGS += -fpermissive
win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

INCLUDEPATH += \
    . \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/FmiImporter/include \
    $$OPEN_SRC/CoreModules/World_OSI \
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary \
    FMI \    # third party
    FMI1 \
    FMI2 \
    JM

HEADERS += \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/OsmpFmuHandler.h \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/FmiImporter/include/fmuChecker.h \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/FmiImporter/include/fmi1_input_reader.h \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/FmiImporter/include/fmi2_input_reader.h

SOURCES += \
    OsmpFmuUnitTests.cpp \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/OsmpFmuHandler.cpp \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/FmiImporter/src/Common/fmuChecker.c \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/FmiImporter/src/FMI1/fmi1_check.c \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/FmiImporter/src/FMI1/fmi1_cs_sim.c \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/FmiImporter/src/FMI1/fmi1_me_sim.c \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/FmiImporter/src/FMI2/fmi2_check.c \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/FmiImporter/src/FMI2/fmi2_cs_sim.c \
    $$OPEN_SRC/Components/Algorithm_FmuWrapper/FmiImporter/src/FMI2/fmi2_me_sim.c

win32 {
    LIBS += \
        -llibboost_filesystem-mgw71-1_63 \
        -llibboost_system-mgw71-1_63 \
        -lshlwapi
} else {
    LIBS += \
        -lboost_filesystem \
        -lboost_system
}

LIBS += -lfmilib_shared -lopen_simulation_interface -lprotobuf

