################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
add_openpass_target(
  NAME Common TYPE library LINKAGE shared COMPONENT common

  HEADERS
    events/acquirePositionEvent.h
    events/basicEvent.h
    events/collisionEvent.h
    events/componentStateChangeEvent.h
    events/laneChangeEvent.h
    events/speedActionEvent.h
    events/trajectoryEvent.h
    accelerationSignal.h
    agentCompToCompCtrlSignal.h
    acquirePositionSignal.h
    boostGeometryCommon.h
    commonTools.h
    compCtrlToAgentCompSignal.h
    compatibility.h
    dynamicsSignal.h
    eventDetectorDefinitions.h
    eventTypes.h
    globalDefinitions.h
    lateralSignal.h
    longitudinalSignal.h
    observationLibraryDefinitions.h
    opExport.h
    openPassTypes.h
    openPassUtils.h
    openScenarioDefinitions.h
    parameter.h
    parametersVehicleSignal.h
    primitiveSignals.h
    runtimeInformation.h
    secondaryDriverTasksSignal.h
    sensorDataSignal.h
    sensorDefinitions.h
    spawnPointLibraryDefinitions.h
    speedActionSignal.h
    steeringSignal.h
    stochasticDefinitions.h
    trajectorySignal.h
    vector2d.h
    vector3d.h
    vectorSignals.h
    version.h
    worldDefinitions.h

  SOURCES
    commonTools.cpp
    vector3d.cpp
    eventDetectorDefinitions.cpp
)
