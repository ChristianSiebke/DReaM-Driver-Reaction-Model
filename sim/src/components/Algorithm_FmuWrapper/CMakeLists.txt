################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Algorithm_FmuWrapper)

add_compile_definitions(ALGORITHM_FMUWRAPPER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    AlgorithmFmuWrapper.h
    src/fmuWrapper.h
    src/OsmpFmuHandler.h
    src/variant_visitor.h

  SOURCES
    AlgorithmFmuWrapper.cpp
    src/fmuWrapper.cpp
    src/OsmpFmuHandler.cpp
    src/FmiImporter/src/Common/fmuChecker.c
    src/FmiImporter/src/FMI1/fmi1_check.c
    src/FmiImporter/src/FMI1/fmi1_cs_sim.c
    src/FmiImporter/src/FMI1/fmi1_me_sim.c
    src/FmiImporter/src/FMI2/fmi2_check.c
    src/FmiImporter/src/FMI2/fmi2_cs_sim.c
    src/FmiImporter/src/FMI2/fmi2_me_sim.c

  INCDIRS
    ${FMILibrary_INCLUDE_DIR}
    ${FMILibrary_INCLUDE_DIR}/FMI
    ${FMILibrary_INCLUDE_DIR}/FMI1
    ${FMILibrary_INCLUDE_DIR}/FMI2
    ${FMILibrary_INCLUDE_DIR}/JM
    src/FmiImporter/include
    ../../core/opSimulation/modules/World_OSI

  LIBRARIES
    Qt5::Core
    ${FMILibrary_LIBRARY_DIR}
    Common

  LINKOSI
)
