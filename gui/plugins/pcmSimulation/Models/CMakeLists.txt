# /*********************************************************************
# * Copyright (c) 2021 ITK Engineering GmbH
# * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

add_subdirectory(ConfigurationGeneratorPcm)

set(SOURCES
    ModelPcm.cpp
    FileHelper.cpp
)

set(HEADERS
    ModelPcm.h
    FileHelper.h
)

add_library(${PROJECT_NAME}_Models STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME}_Models PRIVATE ConfigurationGeneratorPcm DataStructuresXosc DataStructuresXml)
