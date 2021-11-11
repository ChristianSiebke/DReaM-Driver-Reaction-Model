################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME AlgorithmFmuWrapper_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/Algorithm_FmuWrapper/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN
  LINKOSI

  SOURCES
    OsmpFmuUnitTests.cpp
    ${COMPONENT_SOURCE_DIR}/OsmpFmuHandler.cpp
    ${COMPONENT_SOURCE_DIR}/FmiImporter/src/Common/fmuChecker.c
    ${COMPONENT_SOURCE_DIR}/FmiImporter/src/FMI1/fmi1_check.c
    ${COMPONENT_SOURCE_DIR}/FmiImporter/src/FMI1/fmi1_cs_sim.c
    ${COMPONENT_SOURCE_DIR}/FmiImporter/src/FMI1/fmi1_me_sim.c
    ${COMPONENT_SOURCE_DIR}/FmiImporter/src/FMI2/fmi2_check.c
    ${COMPONENT_SOURCE_DIR}/FmiImporter/src/FMI2/fmi2_cs_sim.c
    ${COMPONENT_SOURCE_DIR}/FmiImporter/src/FMI2/fmi2_me_sim.c

  HEADERS
    ${COMPONENT_SOURCE_DIR}/OsmpFmuHandler.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${COMPONENT_SOURCE_DIR}/FmiImporter/include
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI
    # FMILibrary internals
    ${FMILibrary_INCLUDE_DIR}
    ${FMILibrary_INCLUDE_DIR}/FMI
    ${FMILibrary_INCLUDE_DIR}/FMI1
    ${FMILibrary_INCLUDE_DIR}/FMI2
    ${FMILibrary_INCLUDE_DIR}/JM

  LIBRARIES
    Qt5::Core
    Common
    ${FMILibrary_LIBRARY_DIR}
)

