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

add_subdirectory(DataStructuresXml)
add_subdirectory(DataStructuresXosc)

set(SOURCES
    XmlMergeHelper.cpp
    stochasticsPCM.cpp
    openScenarioWriter.cpp
    DatabaseReaderPcm.cpp
    ConfigWriter.cpp
    ConfigSetReaderPcm.cpp
    ConfigGeneratorPcm.cpp
)

set(HEADERS
    XmlMergeHelper.h
    stochasticsPCM.h
    openScenarioWriter.h
    DatabaseReaderPcm.h
    ConfigWriter.h
    ConfigSetReaderPcm.h
    ConfigGeneratorPcm.h
)

add_library(ConfigurationGeneratorPcm STATIC ${SOURCES} ${HEADERS})
target_link_libraries(ConfigurationGeneratorPcm Gui_Common)
