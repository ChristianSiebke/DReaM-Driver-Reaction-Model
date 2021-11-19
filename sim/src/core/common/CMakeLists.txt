################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME CoreCommon)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE static COMPONENT common

  HEADERS
    callbacks.h
    coreDataPublisher.h
    ../../common/xmlParser.h
    cephesMIT/mconf.h

  SOURCES
    callbacks.cpp
    coreDataPublisher.cpp
    ../../common/xmlParser.cpp
    cephesMIT/const.c
    cephesMIT/fresnl.c
    cephesMIT/polevl.c

  LIBRARIES
    Qt5::Core
    Qt5::Xml

  INCDIRS
    ../../../..
)

set_target_properties(CoreCommon PROPERTIES FOLDER "core")
