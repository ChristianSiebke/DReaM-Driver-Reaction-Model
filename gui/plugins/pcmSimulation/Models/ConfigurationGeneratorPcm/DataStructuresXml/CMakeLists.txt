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

set(SOURCES
    XmlAgent.cpp
    XmlAgentEquipment.cpp
    XmlComponent.cpp
    XmlComponentObservation.cpp
    XmlComponentSignal.cpp
    XmlGlobalData.cpp
    XmlLine.cpp
    XmlMarks.cpp
    XmlModelsConfig.cpp
    XmlObject.cpp
    XmlObservation.cpp
    XmlParameter.cpp
    XmlPoint.cpp
    XmlProfilesConfig.cpp
    XmlScenery.cpp
    XmlSlaveConfig.cpp
    XmlSpawnPoint.cpp
    XmlTrajectory.cpp
    XmlViewObject.cpp
)

set(HEADERS
    XmlAgent.h
    XmlAgentEquipment.h
    XmlComponent.h
    XmlComponentObservation.h
    XmlComponentSignal.h
    XmlGlobalData.h
    XmlLine.h
    XmlMarks.h
    XmlModelsConfig.h
    XmlObject.h
    XmlObservation.h
    XmlParameter.h
    XmlPoint.h
    XmlProfilesConfig.h
    XmlScenery.h
    XmlSlaveConfig.h
    XmlSpawnPoint.h
    XmlTrajectory.h
    XmlViewObject.h
)

add_library(DataStructuresXml STATIC ${SOURCES} ${HEADERS})
